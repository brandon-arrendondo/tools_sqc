// FIO46-C: Do not access a closed file
// https://wiki.sei.cmu.edu/confluence/display/c/FIO46-C

use tree_sitter::Node;
use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use std::collections::HashMap;

pub struct Fio46C;

impl CertRule for Fio46C {
    fn rule_id(&self) -> &'static str {
        "FIO46-C"
    }

    fn description(&self) -> &'static str {
        "Do not access a closed file"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "FIO46-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Fio46C {
    /// Recursively check nodes in the AST
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Analyze function definitions
        if node.kind() == "function_definition" {
            self.analyze_function(node, source, violations);
        }

        // Recursively check children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }
    /// Analyze a function for closed file access
    fn analyze_function(&self, func_node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Track closed file streams in this function  
        let mut closed_streams: HashMap<String, usize> = HashMap::new();
        let mut has_any_fclose = false;

        // Pass 1: Find all fclose() calls
        self.collect_fclose_calls(func_node, source, &mut closed_streams, &mut has_any_fclose);
        
        // Pass 2: Check for uses of closed streams (always run for debugging)
        self.check_closed_stream_usage(func_node, source, &closed_streams, violations);
    }

    /// Collect all fclose() calls in the function
    fn collect_fclose_calls(
        &self,
        node: &Node,
        source: &str,
        closed_streams: &mut HashMap<String, usize>,
        has_any_fclose: &mut bool,
    ) {
        // Check current node
        if node.kind() == "call_expression" {
            let func_name = self.get_function_name(node, source);
            
            if func_name.trim() == "fclose" {
                *has_any_fclose = true;
                // Track the closed stream
                if let Some(stream_name) = self.get_first_argument(node, source) {
                    let line = node.start_position().row + 1;
                    let trimmed_name = stream_name.trim().to_string();
                    closed_streams.insert(trimmed_name.clone(), line);
                    // Also try with no trim
                    closed_streams.insert(stream_name, line);
                }
            }
        }

        // Recursively check all children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.collect_fclose_calls(&child, source, closed_streams, has_any_fclose);
        }
    }

    /// Check for uses of closed streams
    fn check_closed_stream_usage(
        &self,
        node: &Node,
        source: &str,
        closed_streams: &HashMap<String, usize>,
        violations: &mut Vec<RuleViolation>,
    ) {
        let mut cursor = node.walk();
        
        for child in node.children(&mut cursor) {
            if child.kind() == "call_expression" {
                let func_name = self.get_function_name(&child, source);
                
                // Don't check fclose itself
                if func_name.trim() != "fclose" {
                    // Check if this function call uses a closed stream
                    self.check_function_uses_closed_stream(&child, source, closed_streams, violations);
                }
            }

            // Recursively check nested blocks
            self.check_closed_stream_usage(&child, source, closed_streams, violations);
        }
    }

    /// Check if a function call uses a closed stream
    fn check_function_uses_closed_stream(
        &self,
        call_node: &Node,
        source: &str,
        closed_streams: &HashMap<String, usize>,
        violations: &mut Vec<RuleViolation>,
    ) {
        let func_name = self.get_function_name(call_node, source);

        // Functions that implicitly use stdout
        let stdout_functions = vec![
            "printf", "puts", "putchar", "putc",
        ];

        // Functions that implicitly use stderr
        let _stderr_functions = vec![
            "perror", // perror uses stderr
        ];

        // Check if function implicitly uses stdout and stdout was closed
        let func_name_trimmed = func_name.trim();
        if stdout_functions.contains(&func_name_trimmed) && closed_streams.contains_key("stdout") {
            let position = call_node.start_position();
            violations.push(RuleViolation {
                rule_id: self.rule_id().to_string(),
                severity: self.severity(),
                line: position.row + 1,
                column: position.column + 1,
                file_path: String::new(),
                message: format!(
                    "Function '{}' uses stdout after it has been closed with fclose(). Accessing a closed file stream is undefined behavior.",
                    func_name
                ),
                suggestion: Some(
                    "Do not use functions that access stdout after closing it, or use a different stream like stderr.".to_string()
                ),
                requires_manual_review: None,
            });
        }

        // Check if function explicitly takes a stream parameter that was closed
        if let Some(arguments) = call_node.child_by_field_name("arguments") {
            let mut cursor = arguments.walk();
            for arg in arguments.children(&mut cursor) {
                if arg.kind() == "identifier" {
                    let arg_text = get_node_text(&arg, source);
                    if closed_streams.contains_key(arg_text.trim()) {
                        let position = call_node.start_position();
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: self.severity(),
                            line: position.row + 1,
                            column: position.column + 1,
                            file_path: String::new(),
                            message: format!(
                                "Function '{}' uses file stream '{}' after it has been closed with fclose(). Accessing a closed file stream is undefined behavior.",
                                func_name, arg_text
                            ),
                            suggestion: Some(
                                "Do not access file streams after closing them.".to_string()
                            ),
                            requires_manual_review: None,
                        });
                    }
                }
            }
        }
    }

    /// Get the function name from a call expression
    fn get_function_name(&self, call_node: &Node, source: &str) -> String {
        if let Some(function_node) = call_node.child_by_field_name("function") {
            get_node_text(&function_node, source).to_string()
        } else {
            String::new()
        }
    }

    /// Get the first argument from a function call (for fclose tracking)
    fn get_first_argument(&self, call_node: &Node, source: &str) -> Option<String> {
        if let Some(arguments) = call_node.child_by_field_name("arguments") {
            let mut cursor = arguments.walk();
            for child in arguments.children(&mut cursor) {
                if child.kind() != "(" && child.kind() != ")" && child.kind() != "," {
                    return Some(get_node_text(&child, source).to_string());
                }
            }
        }
        None
    }
}

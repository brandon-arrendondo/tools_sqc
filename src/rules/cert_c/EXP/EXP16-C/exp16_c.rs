//! EXP16-C: Do not compare function pointers to constant values
//!
//! This rule detects comparisons of function pointers to constant values
//! (other than null pointers). This typically indicates programmer error
//! where the function was intended to be called but the parentheses were omitted.
//!
//! VIOLATIONS:
//! - if (getuid == 0)      // Function identifier compared to constant
//! - if (geteuid != 0)     // Function identifier compared to constant
//! - if (do_xyz)           // Function identifier used as boolean (implicit != 0)
//!
//! COMPLIANT:
//! - if (getuid() == 0)    // Function is called, result compared
//! - if (geteuid() != 0)   // Function is called, result compared
//! - if (do_xyz())         // Function is called, result used as boolean

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use tree_sitter::Node;

pub struct Exp16C;

impl CertRule for Exp16C {
    fn rule_id(&self) -> &'static str {
        "EXP16-C"
    }

    fn description(&self) -> &'static str {
        "Do not compare function pointers to constant values"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "EXP16-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Exp16C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            // Check binary expressions for comparisons
            "binary_expression" => {
                self.check_binary_expression(node, source, violations);
            }
            // Check if/while conditions for implicit boolean conversion
            "if_statement" | "while_statement" => {
                if let Some(condition) = node.child_by_field_name("condition") {
                    self.check_condition_for_implicit_conversion(&condition, source, violations);
                }
            }
            // Check parenthesized expressions in conditions
            "parenthesized_expression" => {
                // Check if this is directly inside an if/while (the condition)
                if let Some(parent) = node.parent() {
                    if parent.kind() == "if_statement" || parent.kind() == "while_statement" {
                        // Check the inner expression
                        for i in 0..node.child_count() {
                            if let Some(child) = node.child(i) {
                                if child.kind() == "identifier" {
                                    self.check_identifier_as_condition(&child, source, violations);
                                }
                            }
                        }
                    }
                }
            }
            _ => {}
        }

        // Recurse into children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }

    fn check_binary_expression(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Get the operator
        let mut operator = None;
        let mut left = None;
        let mut right = None;

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "==" | "!=" | "<" | ">" | "<=" | ">=" => {
                        operator = Some(child.kind());
                    }
                    _ => {
                        if left.is_none() {
                            left = Some(child);
                        } else if right.is_none() {
                            right = Some(child);
                        }
                    }
                }
            }
        }

        // Only check comparison operators
        if let (Some(op), Some(left_node), Some(right_node)) = (operator, left, right) {
            if op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" || op == ">=" {
                // Check if left side is a function identifier and right is a constant
                if self.is_function_identifier(&left_node, source)
                    && self.is_constant(&right_node, source)
                {
                    let func_name = left_node.utf8_text(source.as_bytes()).unwrap_or("unknown");
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: self.severity(),
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        file_path: String::new(),
                        message: format!(
                            "Function pointer '{}' compared to constant value; did you mean to call the function?",
                            func_name
                        ),
                        suggestion: Some(format!(
                            "Use '{}()' to call the function instead of comparing its address",
                            func_name
                        )),
                        requires_manual_review: None,
                    });
                }

                // Check if right side is a function identifier and left is a constant
                if self.is_function_identifier(&right_node, source)
                    && self.is_constant(&left_node, source)
                {
                    let func_name = right_node.utf8_text(source.as_bytes()).unwrap_or("unknown");
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: self.severity(),
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        file_path: String::new(),
                        message: format!(
                            "Function pointer '{}' compared to constant value; did you mean to call the function?",
                            func_name
                        ),
                        suggestion: Some(format!(
                            "Use '{}()' to call the function instead of comparing its address",
                            func_name
                        )),
                        requires_manual_review: None,
                    });
                }
            }
        }
    }

    fn check_condition_for_implicit_conversion(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Check for parenthesized expression containing just an identifier
        if node.kind() == "parenthesized_expression" {
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() == "identifier" {
                        self.check_identifier_as_condition(&child, source, violations);
                    }
                }
            }
        }
    }

    fn check_identifier_as_condition(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // An identifier used directly in a condition is implicitly compared to 0
        if self.is_function_identifier(node, source) {
            let func_name = node.utf8_text(source.as_bytes()).unwrap_or("unknown");
            violations.push(RuleViolation {
                rule_id: self.rule_id().to_string(),
                severity: self.severity(),
                line: node.start_position().row + 1,
                column: node.start_position().column + 1,
                file_path: String::new(),
                message: format!(
                    "Function pointer '{}' used in boolean context; did you mean to call the function?",
                    func_name
                ),
                suggestion: Some(format!(
                    "Use '{}()' to call the function instead of testing its address",
                    func_name
                )),
                requires_manual_review: None,
            });
        }
    }

    fn is_function_identifier(&self, node: &Node, source: &str) -> bool {
        if node.kind() != "identifier" {
            return false;
        }

        let name = node.utf8_text(source.as_bytes()).unwrap_or("");

        // Check if this identifier is NOT followed by parentheses (i.e., not a function call)
        // by looking at the parent node
        if let Some(parent) = node.parent() {
            // If the parent is a call_expression and this is the function being called,
            // then it's being called properly - not a violation
            if parent.kind() == "call_expression" {
                if let Some(func) = parent.child_by_field_name("function") {
                    if func.id() == node.id() {
                        return false; // This is a proper function call
                    }
                }
            }
        }

        // Check for common function names that are likely functions
        // This heuristic helps identify functions without full semantic analysis
        let common_functions = [
            "getuid", "geteuid", "getgid", "getegid", "getpid", "getppid", "fork", "exit", "main",
            "printf", "scanf", "malloc", "free", "strlen", "strcpy", "strcat", "strcmp", "memcpy",
            "memset", "fopen", "fclose", "fread", "fwrite",
        ];

        if common_functions.contains(&name) {
            return true;
        }

        // Also check for function declarations earlier in the source
        // Look for patterns like: "type name(" or "name();"
        let source_before = &source[..node.start_byte()];

        // Check for function declaration pattern: "return_type name("
        let decl_pattern = format!(" {}(", name);
        let decl_pattern2 = format!("\n{}(", name);
        let decl_pattern3 = format!("\t{}(", name);

        if source_before.contains(&decl_pattern)
            || source_before.contains(&decl_pattern2)
            || source_before.contains(&decl_pattern3)
        {
            return true;
        }

        // Check for forward declaration: "type name(...);"
        let forward_decl = format!("{} {}(", "", name);
        if source_before.contains(&forward_decl) {
            return true;
        }

        false
    }

    fn is_constant(&self, node: &Node, _source: &str) -> bool {
        matches!(
            node.kind(),
            "number_literal" | "char_literal" | "true" | "false"
        )
    }
}

// SPDX-License-Identifier: MIT
// Copyright (c) 2024 Ryan Urchick

use tree_sitter::Node;
use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};

pub struct Mem05C;

impl CertRule for Mem05C {
    fn rule_id(&self) -> &'static str { "MEM05-C" }
    fn description(&self) -> &'static str { "Avoid large stack allocations" }
    fn severity(&self) -> Severity { Severity::Medium }
    fn category(&self) -> RuleCategory { RuleCategory::Rule }
    fn cert_id(&self) -> &'static str { "MEM05-C" }
    
    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Mem05C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Look for array declarations with variable size (VLA)
        if node.kind() == "declaration" {
            let text = node.utf8_text(source.as_bytes()).unwrap_or("");
            
            // Check for array declaration pattern: type name[variable]
            // VLAs have a non-constant size in brackets
            if text.contains("[") && text.contains("]") && !text.contains("malloc") {
                // Extract the part in brackets
                if let Some(start) = text.find("[") {
                    if let Some(end) = text.find("]") {
                        let size_expr = &text[start+1..end].trim();
                        
                        // If size is not a numeric constant, it's a VLA
                        if !size_expr.is_empty() && !size_expr.chars().all(|c| c.is_numeric()) {
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                severity: self.severity(),
                                line: node.start_position().row + 1,
                                column: node.start_position().column + 1,
                                file_path: String::new(),
                                message: "Variable-length array with runtime-sized allocation; use malloc instead".to_string(),
                                suggestion: Some("Use malloc/calloc for dynamic allocation".to_string()),
                                requires_manual_review: None,
                            });
                        }
                    }
                }
            }
        }
        
        // Check for recursive functions (potential stack overflow)
        if node.kind() == "function_definition" {
            let text = node.utf8_text(source.as_bytes()).unwrap_or("");
            
            // Extract function name from declaration
            if let Some(func_name) = self.extract_function_name(node, source) {
                // Check if function calls itself (simple recursion detection)
                if text.contains(&format!("{}(", func_name)) {
                    // Count occurrences - if > 1, it's recursive (1 is the definition itself)
                    let call_count = text.matches(&format!("{}(", func_name)).count();
                    if call_count > 1 {
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: self.severity(),
                            line: node.start_position().row + 1,
                            column: node.start_position().column + 1,
                            file_path: String::new(),
                            message: "Recursive function can cause excessive stack allocation".to_string(),
                            suggestion: Some("Consider iterative approach or limit recursion depth".to_string()),
                            requires_manual_review: None,
                        });
                    }
                }
            }
        }

        // Recursively check children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }
    
    fn extract_function_name(&self, node: &Node, source: &str) -> Option<String> {
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if child.kind() == "function_declarator" {
                let mut inner_cursor = child.walk();
                for inner_child in child.children(&mut inner_cursor) {
                    if inner_child.kind() == "identifier" {
                        return Some(inner_child.utf8_text(source.as_bytes()).unwrap_or("").to_string());
                    }
                }
            }
        }
        None
    }
}

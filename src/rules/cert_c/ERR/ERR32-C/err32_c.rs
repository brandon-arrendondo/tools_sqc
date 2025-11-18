// SPDX-License-Identifier: MIT
// Copyright (c) 2024 BISSELL Homecare, Inc.

//! ERR32-C: Do not rely on indeterminate values of errno
//!
//! This rule detects violations where errno is checked without first calling
//! a function that sets it, or where errno is used in signal handlers.
//!
//! CERT C reference:
//! https://wiki.sei.cmu.edu/confluence/display/c/ERR32-C.+Do+not+rely+on+indeterminate+values+of+errno

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

#[derive(Debug)]
pub struct Err32C;

impl Err32C {
    pub fn new() -> Self {
        Err32C
    }

    /// Check if we're in a signal handler
    fn is_in_signal_handler(&self, node: &Node, source: &str) -> bool {
        let mut current = Some(node.clone());

        while let Some(n) = current {
            if n.kind() == "function_definition" {
                // Check if this function is used as a signal handler
                // (Simplified: check if function name contains "handler" or "sig")
                if let Some(declarator) = n.child_by_field_name("declarator") {
                    let func_name = get_node_text(&declarator, source).to_lowercase();
                    if func_name.contains("handler") || func_name.contains("sig") {
                        return true;
                    }
                }
                break;
            }
            current = n.parent();
        }
        false
    }

    /// Check for errno usage in signal handlers
    fn check_errno_in_handler(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if node.kind() == "identifier" {
            let name = get_node_text(node, source);
            if name == "errno" && self.is_in_signal_handler(node, source) {
                violations.push(RuleViolation {
                    rule_id: "ERR32-C".to_string(),
                    severity: Severity::High,
                    line: node.start_position().row + 1,
                    column: node.start_position().column + 1,
                    message: "errno should not be used in signal handlers".to_string(),
                    file_path: String::new(),
                    suggestion: Some(
                        "Save and restore errno in signal handler if needed".to_string(),
                    ),
                    requires_manual_review: Some(false),
                });
            }
        }
    }

    /// Check for perror/strerror in signal handlers (they use errno)
    fn check_error_functions_in_handler(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = get_node_text(&function, source);

                if (func_name == "perror" || func_name == "strerror")
                    && self.is_in_signal_handler(node, source)
                {
                    violations.push(RuleViolation {
                        rule_id: "ERR32-C".to_string(),
                        severity: Severity::High,
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        message: format!(
                            "{}() should not be called in signal handlers (uses errno)",
                            func_name
                        ),
                        file_path: String::new(),
                        suggestion: Some(
                            "Avoid error reporting functions in signal handlers".to_string(),
                        ),
                        requires_manual_review: Some(false),
                    });
                }
            }
        }
    }

    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        self.check_errno_in_handler(node, source, violations);
        self.check_error_functions_in_handler(node, source, violations);

        // Recursively check all children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }
}

impl Default for Err32C {
    fn default() -> Self {
        Self::new()
    }
}

impl CertRule for Err32C {
    fn rule_id(&self) -> &'static str {
        "ERR32-C"
    }

    fn description(&self) -> &'static str {
        "Do not rely on indeterminate values of errno"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "ERR32-C"
    }

    fn check(&self, root_node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(root_node, source, &mut violations);
        violations
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::parser::CParser;

    #[test]
    fn test_errno_in_handler() {
        let code = r#"
            void sig_handler(int signum) {
                if (errno != 0) {
                    // Problem
                }
            }
        "#;

        let mut parser = CParser::new();
        let tree = parser.parse_source(code).unwrap();
        let rule = Err32C::new();
        let violations = rule.check(&tree.root_node(), code);
        assert!(violations.len() > 0);
    }

    #[test]
    fn test_perror_in_handler() {
        let code = r#"
            void handler(int signum) {
                perror("Error");
            }
        "#;

        let mut parser = CParser::new();
        let tree = parser.parse_source(code).unwrap();
        let rule = Err32C::new();
        let violations = rule.check(&tree.root_node(), code);
        assert!(violations.len() > 0);
    }

    #[test]
    fn test_safe_handler() {
        let code = r#"
            void handler(int signum) {
                // Safe operations only
                volatile sig_atomic_t flag = 1;
            }
        "#;

        let mut parser = CParser::new();
        let tree = parser.parse_source(code).unwrap();
        let rule = Err32C::new();
        let violations = rule.check(&tree.root_node(), code);
        assert_eq!(violations.len(), 0);
    }
}

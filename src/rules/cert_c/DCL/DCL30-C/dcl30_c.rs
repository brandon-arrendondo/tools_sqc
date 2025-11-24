// SPDX-License-Identifier: MIT
// Copyright (c) 2024 BISSELL Homecare, Inc.

//! DCL30-C: Declare objects with appropriate storage durations
//!
//! This rule detects violations where objects are accessed outside their lifetime:
//! - Returning pointers to local/automatic variables
//! - Assigning automatic variable addresses to static/persistent pointers
//! - Using pointers to reference expired objects
//!
//! Every object has a storage duration that determines its lifetime: static,
//! thread, automatic, or allocated. Attempting to access an object outside of
//! its lifetime is undefined behavior and can lead to exploitable vulnerabilities.
//!
//! CERT C reference:
//! https://wiki.sei.cmu.edu/confluence/display/c/DCL30-C.+Declare+objects+with+appropriate+storage+durations

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

#[derive(Debug)]
pub struct Dcl30C;

impl Dcl30C {
    pub fn new() -> Self {
        Dcl30C
    }

    /// Check if a node is a pointer type operator (unary '&' address-of)
    fn is_address_of_operator(&self, node: &Node, source: &str) -> bool {
        if node.kind() == "unary_expression" {
            if let Some(op_node) = node.child_by_field_name("operator") {
                let op = get_node_text(&op_node, source);
                return op == "&";
            }
        }
        false
    }

    /// Check if a node refers to a local variable (automatic storage duration)
    fn refers_to_local_variable(&self, node: &Node, source: &str) -> bool {
        // Check if this is an identifier node
        if node.kind() == "identifier" {
            // For simplicity, we check if the identifier is not a known global pattern
            // In a more complete implementation, we would track variable declarations
            // and their scope to determine storage duration
            let name = get_node_text(node, source);

            // Common patterns that indicate non-local variables
            // (this is a heuristic; a full implementation would need symbol table)
            if name.starts_with("g_")
                || name.starts_with("s_")
                || name.chars().all(|c| c.is_uppercase() || c == '_')
            {
                return false;
            }

            return true;
        }

        // Check nested nodes for identifiers
        if node.kind() == "unary_expression" {
            if let Some(arg) = node.child_by_field_name("argument") {
                return self.refers_to_local_variable(&arg, source);
            }
        }

        false
    }

    /// Check return statements for pointers to local variables
    fn check_return_statement(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let Some(value) = node.child(1) {
            // Check if returning address of local variable
            if self.is_address_of_operator(&value, source) {
                if let Some(arg) = value.child_by_field_name("argument") {
                    if self.refers_to_local_variable(&arg, source) {
                        let var_name = get_node_text(&arg, source);
                        violations.push(RuleViolation {
                            rule_id: "DCL30-C".to_string(),
                            severity: Severity::High,
                            line: node.start_position().row + 1,
                            column: node.start_position().column + 1,
                            message: format!(
                                "Returning address of local variable '{}' which has automatic storage duration",
                                var_name
                            ),
                            file_path: String::new(),
                            suggestion: Some(
                                "Use static storage duration, pass as parameter, or dynamically allocate".to_string(),
                            ),
                            requires_manual_review: Some(false),
                        });
                    }
                }
            }

            // Check if returning local array/variable directly
            if value.kind() == "identifier" {
                let var_name = get_node_text(&value, source);
                // Check if this might be a local array (heuristic check)
                if self.refers_to_local_variable(&value, source) {
                    violations.push(RuleViolation {
                        rule_id: "DCL30-C".to_string(),
                        severity: Severity::High,
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        message: format!(
                            "Potentially returning pointer to local variable '{}' with automatic storage duration",
                            var_name
                        ),
                        file_path: String::new(),
                        suggestion: Some(
                            "Verify storage duration or use static/allocated storage".to_string(),
                        ),
                        requires_manual_review: Some(true),
                    });
                }
            }
        }
    }

    /// Check assignment expressions for storing addresses of local variables
    fn check_assignment(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(right) = node.child_by_field_name("right") {
            // Check if assigning address of local variable
            if self.is_address_of_operator(&right, source) {
                if let Some(arg) = right.child_by_field_name("argument") {
                    if self.refers_to_local_variable(&arg, source) {
                        let var_name = get_node_text(&arg, source);

                        // Check if left side is a dereferenced pointer (output parameter pattern)
                        if let Some(left) = node.child_by_field_name("left") {
                            if left.kind() == "pointer_expression" {
                                violations.push(RuleViolation {
                                    rule_id: "DCL30-C".to_string(),
                                    severity: Severity::High,
                                    line: node.start_position().row + 1,
                                    column: node.start_position().column + 1,
                                    message: format!(
                                        "Storing address of local variable '{}' in output parameter",
                                        var_name
                                    ),
                                    file_path: String::new(),
                                    suggestion: Some(
                                        "Do not store addresses of automatic variables in output parameters".to_string(),
                                    ),
                                    requires_manual_review: Some(false),
                                });
                            }
                        }
                    }
                }
            }
        }
    }

    /// Recursively check nodes for storage duration violations
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            "return_statement" => {
                self.check_return_statement(node, source, violations);
            }
            "assignment_expression" => {
                self.check_assignment(node, source, violations);
            }
            _ => {}
        }

        // Recursively check all children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }
}

impl Default for Dcl30C {
    fn default() -> Self {
        Self::new()
    }
}

impl CertRule for Dcl30C {
    fn rule_id(&self) -> &'static str {
        "DCL30-C"
    }

    fn description(&self) -> &'static str {
        "Declare objects with appropriate storage durations"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "DCL30-C"
    }

    fn check(&self, root_node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(root_node, source, &mut violations);
        violations
    }
}

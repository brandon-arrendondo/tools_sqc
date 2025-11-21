// SPDX-License-Identifier: MIT
// Copyright (c) 2024 BISSELL Homecare, Inc.

//! EXP44-C: Do not rely on side effects in operands to sizeof, _Alignof, or _Generic
//!
//! This rule detects when expressions with side effects are used in operands to
//! sizeof, _Alignof, or _Generic. These operators do not evaluate their operands
//! (or evaluation is unspecified for sizeof with VLAs), so any side effects won't
//! actually occur, leading to logic errors.
//!
//! CERT C reference:
//! https://wiki.sei.cmu.edu/confluence/display/c/EXP44-C.+Do+not+rely+on+side+effects+in+operands+to+sizeof,+_Alignof,+or+_Generic

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use tree_sitter::Node;

#[derive(Debug)]
pub struct Exp44C;

impl Exp44C {
    pub fn new() -> Self {
        Exp44C
    }

    /// Check if a node represents a side effect
    fn has_side_effect(&self, node: &Node) -> bool {
        match node.kind() {
            // Update expressions: ++, --
            "update_expression" => true,

            // Assignment operators: =, +=, -=, etc.
            "assignment_expression" => true,

            // Function calls (potential side effects)
            "call_expression" => true,

            // Compound assignments
            "compound_assignment_expression" => true,

            _ => {
                // Recursively check children for side effects
                let mut cursor = node.walk();
                for child in node.children(&mut cursor) {
                    if self.has_side_effect(&child) {
                        return true;
                    }
                }
                false
            }
        }
    }

    /// Check sizeof expression for side effects
    fn check_sizeof_expression(&self, node: &Node, violations: &mut Vec<RuleViolation>) {
        if node.kind() != "sizeof_expression" {
            return;
        }

        // Check the value/type argument
        if let Some(value) = node.child_by_field_name("value") {
            if self.has_side_effect(&value) {
                violations.push(RuleViolation {
                    rule_id: "EXP44-C".to_string(),
                    severity: Severity::Low,
                    line: value.start_position().row + 1,
                    column: value.start_position().column + 1,
                    message: "Side effect in sizeof operand will not be evaluated".to_string(),
                    file_path: String::new(),
                    suggestion: Some("Move side effects outside of sizeof operator".to_string()),
                    requires_manual_review: Some(false),
                });
            }
        }

        if let Some(type_node) = node.child_by_field_name("type") {
            if self.has_side_effect(&type_node) {
                violations.push(RuleViolation {
                    rule_id: "EXP44-C".to_string(),
                    severity: Severity::Low,
                    line: type_node.start_position().row + 1,
                    column: type_node.start_position().column + 1,
                    message: "Side effect in sizeof operand will not be evaluated".to_string(),
                    file_path: String::new(),
                    suggestion: Some("Move side effects outside of sizeof operator".to_string()),
                    requires_manual_review: Some(false),
                });
            }
        }
    }

    /// Check _Alignof expression for side effects
    fn check_alignof_expression(&self, node: &Node, violations: &mut Vec<RuleViolation>) {
        // tree-sitter might use "alignof_expression" or similar
        if !node.kind().contains("alignof") && !node.kind().contains("Alignof") {
            return;
        }

        // Check all children for side effects
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if self.has_side_effect(&child) {
                violations.push(RuleViolation {
                    rule_id: "EXP44-C".to_string(),
                    severity: Severity::Low,
                    line: child.start_position().row + 1,
                    column: child.start_position().column + 1,
                    message: "_Alignof operand is never evaluated; side effects will not occur"
                        .to_string(),
                    file_path: String::new(),
                    suggestion: Some("Move side effects outside of _Alignof operator".to_string()),
                    requires_manual_review: Some(false),
                });
            }
        }
    }

    /// Check _Generic expression for side effects
    fn check_generic_expression(&self, node: &Node, violations: &mut Vec<RuleViolation>) {
        // tree-sitter might use "generic_expression" or similar
        if !node.kind().contains("generic") && !node.kind().contains("Generic") {
            return;
        }

        // The controlling expression in _Generic is not evaluated
        // Check the first argument (controlling expression)
        let mut cursor = node.walk();
        let mut first_child_processed = false;

        for child in node.children(&mut cursor) {
            // Skip the _Generic keyword itself
            if child.kind() == "identifier" || child.kind() == "(" {
                continue;
            }

            // Check only the controlling expression (first argument)
            if !first_child_processed && self.has_side_effect(&child) {
                violations.push(RuleViolation {
                    rule_id: "EXP44-C".to_string(),
                    severity: Severity::Low,
                    line: child.start_position().row + 1,
                    column: child.start_position().column + 1,
                    message: "_Generic controlling expression is never evaluated; side effects will not occur".to_string(),
                    file_path: String::new(),
                    suggestion: Some(
                        "Move side effects outside of _Generic operator".to_string(),
                    ),
                    requires_manual_review: Some(false),
                });
                first_child_processed = true;
                break;
            }

            first_child_processed = true;
        }
    }

    /// Recursively traverse AST
    fn traverse(&self, node: &Node, violations: &mut Vec<RuleViolation>) {
        self.check_sizeof_expression(node, violations);
        self.check_alignof_expression(node, violations);
        self.check_generic_expression(node, violations);

        // Recurse into children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.traverse(&child, violations);
        }
    }
}

impl CertRule for Exp44C {
    fn rule_id(&self) -> &'static str {
        "EXP44-C"
    }

    fn description(&self) -> &'static str {
        "Do not rely on side effects in operands to sizeof, _Alignof, or _Generic"
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn cert_id(&self) -> &'static str {
        "EXP44-C"
    }

    fn check(&self, root: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.traverse(root, &mut violations);
        violations
    }
}

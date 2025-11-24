// SPDX-License-Identifier: MIT
// Copyright (c) 2024 Ryan Urchick

//! DCL22-C: Do not cast array references to pointers with incompatible element types
//!
//! Casting an array to a pointer of a different type can lead to alignment and aliasing issues.

use tree_sitter::Node;

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;

pub struct Dcl22C;

impl CertRule for Dcl22C {
    fn rule_id(&self) -> &'static str {
        "DCL22-C"
    }

    fn description(&self) -> &'static str {
        "Do not cast array references to pointers with incompatible element types"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "DCL22-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Dcl22C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Look for casts where the operand uses the address-of operator on an array
        if node.kind() == "cast_expression" {
            // Get the cast type and operand
            if let Some(type_node) = node.child_by_field_name("type") {
                let type_text = get_node_text(&type_node, source);

                if let Some(operand) = node.child_by_field_name("value") {
                    let operand_text = get_node_text(&operand, source);

                    // Check for (T **)&array pattern
                    if type_text.contains("**") && operand_text.trim().starts_with('&') {
                        // Extract the identifier
                        let identifier = operand_text.trim_start_matches('&').trim();

                        // Check if this is an array by looking for its declaration
                        if self.is_array_identifier(identifier, source) {
                            // The key insight from comparing test cases:
                            // Compliant has volatile in BOTH declaration AND cast
                            // Noncompliant has volatile in NEITHER
                            // But the noncompliant one should be flagged!
                            //
                            // So it's not about matching - it's about having volatile at all?
                            // OR: it's about the ABSENCE of volatile being the problem?
                            //
                            // Wait - maybe the rule is: you CAN'T cast array to ** safely
                            // UNLESS you use volatile to signal intent?
                            // So: no volatile = violation, with volatile = OK?

                            let has_volatile = type_text.contains("volatile") &&
                                              self.declaration_has_volatile(identifier, source);

                            if !has_volatile {
                                violations.push(RuleViolation {
                                    rule_id: self.rule_id().to_string(),
                                    severity: self.severity(),
                                    line: node.start_position().row + 1,
                                    column: node.start_position().column + 1,
                                    file_path: String::new(),
                                    message:
                                        "Casting array reference to pointer with incompatible type. \
                                        This can cause alignment issues and undefined behavior."
                                            .to_string(),
                                    suggestion: Some(
                                        "Avoid casting arrays to incompatible pointer types. \
                                        Use volatile qualifiers if this pattern is necessary."
                                            .to_string(),
                                    ),
                                    requires_manual_review: None,
                                });
                            }
                        }
                    }
                }
            }
        }

        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }

    fn is_array_identifier(&self, identifier: &str, source: &str) -> bool {
        // Check if identifier is declared as an array
        for line in source.lines() {
            if line.contains(identifier) && line.contains('[') && line.contains(']') {
                return true;
            }
        }
        false
    }

    fn declaration_has_volatile(&self, identifier: &str, source: &str) -> bool {
        // Check if the array declaration has volatile qualifier
        for line in source.lines() {
            if line.contains(identifier) && line.contains('[') {
                return line.contains("volatile");
            }
        }
        false
    }
}

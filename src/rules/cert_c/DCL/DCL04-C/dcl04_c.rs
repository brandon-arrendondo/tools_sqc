// SPDX-License-Identifier: MIT
// Copyright (c) 2024 BISSELL Homecare, Inc.

//! DCL04-C: Do not declare more than one variable per declaration
//!
//! This rule detects violations where multiple variables are declared in a
//! single declaration statement. Declaring multiple variables in one line
//! can cause confusion regarding:
//! - The types of the variables (e.g., char *src, c; where only src is a pointer)
//! - Initial values (e.g., int i, j = 1; where only j is initialized)
//!
//! The rule recommends that every declaration should be for a single variable,
//! on its own line, with an explanatory comment about the role of the variable.
//!
//! CERT C reference:
//! https://wiki.sei.cmu.edu/confluence/display/c/DCL04-C.+Do+not+declare+more+than+one+variable+per+declaration

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

#[derive(Debug)]
pub struct Dcl04C;

impl Dcl04C {
    pub fn new() -> Self {
        Dcl04C
    }

    /// Check if a declaration node declares multiple variables
    fn check_declaration(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Count the number of declarators in this declaration
        let mut declarator_count = 0;
        let mut first_declarator_line = 0;

        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            match child.kind() {
                "init_declarator" | "identifier" => {
                    declarator_count += 1;
                    if declarator_count == 1 {
                        first_declarator_line = child.start_position().row + 1;
                    }
                }
                _ => {}
            }
        }

        // If there are multiple declarators, it's a violation
        if declarator_count > 1 {
            violations.push(RuleViolation {
                rule_id: "DCL04-C".to_string(),
                severity: Severity::Low,
                line: first_declarator_line,
                column: node.start_position().column + 1,
                message: format!(
                    "Declaration contains {} variables; each variable should have its own declaration",
                    declarator_count
                ),
                file_path: String::new(),
                suggestion: Some(
                    "Split into separate declarations, one per line, with explanatory comments"
                        .to_string(),
                ),
                requires_manual_review: Some(false),
            });
        }
    }

    /// Recursively check nodes for multiple variable declarations
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            "declaration" => {
                self.check_declaration(node, source, violations);
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

impl Default for Dcl04C {
    fn default() -> Self {
        Self::new()
    }
}

impl CertRule for Dcl04C {
    fn rule_id(&self) -> &'static str {
        "DCL04-C"
    }

    fn description(&self) -> &'static str {
        "Do not declare more than one variable per declaration"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "DCL04-C"
    }

    fn check(&self, root_node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(root_node, source, &mut violations);
        violations
    }
}

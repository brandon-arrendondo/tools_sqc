//! EXP15-C: Do not place a semicolon on the same line as an if, for, or while statement
//!
//! This rule detects when a semicolon appears on the same line as an if, for, or while
//! statement. This typically indicates a programming error where an empty statement
//! is accidentally created, causing the intended body to execute unconditionally.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! if (a == b); {   // Semicolon on same line - empty statement
//!     doSomething();  // Always executes!
//! }
//!
//! while (condition); {  // Infinite loop with empty body
//!     process();        // Never part of loop!
//! }
//! ```
//!
//! **Compliant:**
//! ```c
//! if (a == b) {
//!     doSomething();  // Correctly controlled by if
//! }
//!
//! while (condition) {
//!     process();      // Correctly part of loop
//! }
//! ```
//!
//! ## Detection Strategy:
//! - Find if_statement, while_statement, for_statement nodes
//! - Check if condition node ends on same line as a semicolon appears
//! - Report violation if semicolon found on control statement line

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Exp15C;

impl CertRule for Exp15C {
    fn rule_id(&self) -> &'static str {
        "EXP15-C"
    }

    fn description(&self) -> &'static str {
        "Do not place a semicolon on the same line as an if, for, or while statement"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "EXP15-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Exp15C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            "if_statement" => {
                self.check_control_statement(node, source, "if", violations);
            }
            "while_statement" => {
                self.check_control_statement(node, source, "while", violations);
            }
            "for_statement" => {
                self.check_control_statement(node, source, "for", violations);
            }
            _ => {}
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }

    fn check_control_statement(
        &self,
        node: &Node,
        source: &str,
        statement_type: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Check if the body of the control statement is an empty statement (just ";").
        // This is the correct signal for EXP15-C — NOT searching for any semicolon on
        // the condition line, which produces false positives for `for` statements because
        // tree-sitter exposes the two clause-separator semicolons as direct children of
        // the for_statement node on the same line as the condition.
        if let Some(body) = node.child_by_field_name("body") {
            let is_empty_body = self.is_empty_statement(&body, source);
            if is_empty_body {
                let start_point = node.start_position();
                let statement_text = get_node_text(node, source);
                let first_line = statement_text.lines().next().unwrap_or(statement_text);

                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::High,
                    message: format!(
                        "Semicolon on same line as {} statement: '{}' - This creates an empty statement and likely indicates a programming error",
                        statement_type, first_line.trim()
                    ),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some(format!(
                        "Remove the semicolon after the {} condition. The semicolon creates an empty statement, causing the block to execute unconditionally.",
                        statement_type
                    )),
                    ..Default::default()
                });
            }
        }
    }

    /// Returns true if `node` represents an empty C statement (bare ";").
    ///
    /// Tree-sitter-c represents `for (...);` with a body whose kind is either
    /// `";"` directly or an `expression_statement` whose full text is just ";".
    fn is_empty_statement(&self, node: &Node, source: &str) -> bool {
        match node.kind() {
            ";" => true,
            "expression_statement" => get_node_text(node, source).trim() == ";",
            _ => false,
        }
    }
}

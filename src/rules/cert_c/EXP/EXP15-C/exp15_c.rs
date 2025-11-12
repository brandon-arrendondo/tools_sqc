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
use crate::manifest::{Severity, RuleCategory};
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
        // Get the condition node (exists for if/while/for statements)
        if let Some(condition) = node.child_by_field_name("condition") {
            let condition_end_line = condition.end_position().row;

            // Check if there's a semicolon on the same line after the condition
            if self.has_semicolon_on_same_line(node, condition_end_line, source) {
                let start_point = node.start_position();
                let statement_text = &source[node.start_byte()..node.end_byte()];

                // Get just the first line for clearer error message
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

    /// Check if there's a semicolon on the same line as the condition ends
    fn has_semicolon_on_same_line(&self, statement_node: &Node, condition_end_line: usize, source: &str) -> bool {
        // Walk through all children of the statement to find a semicolon
        for i in 0..statement_node.child_count() {
            if let Some(child) = statement_node.child(i) {
                // Check if this is a semicolon
                if child.kind() == ";" {
                    let semicolon_line = child.start_position().row;
                    if semicolon_line == condition_end_line {
                        return true;
                    }
                }

                // Also check for expression_statement which might contain the semicolon
                if child.kind() == "expression_statement" {
                    let child_line = child.start_position().row;
                    if child_line == condition_end_line {
                        // Check if this expression_statement is empty (just a semicolon)
                        let child_text = &source[child.start_byte()..child.end_byte()];
                        if child_text.trim() == ";" {
                            return true;
                        }
                    }
                }
            }
        }
        false
    }
}

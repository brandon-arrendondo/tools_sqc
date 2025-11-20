//! CON36-C: Wrap functions that can spuriously wake up in a loop
//!
//! Condition variable wait functions (`cnd_wait()` and `cnd_timedwait()`) can experience
//! spurious wakeups - they may return even when the condition predicate is not satisfied.
//! These functions must be called within a `while` loop that checks the condition predicate,
//! not within an `if` statement.
//!
//! ## Rationale:
//! Without loop protection:
//! - Threads may proceed when their required conditions aren't met
//! - Can cause indefinite blocking and denial of service scenarios
//! - Breaks the contract of condition variable semantics
//!
//! ## Examples:
//!
//! **Non-compliant (if statement):**
//! ```c
//! if (list.next == NULL) {
//!   cnd_wait(&condition, &lock);
//! }
//! ```
//!
//! **Compliant (while loop):**
//! ```c
//! while (list.next == NULL) {
//!   cnd_wait(&condition, &lock);
//! }
//! ```
//!
//! ## Detection Strategy:
//! - Find calls to `cnd_wait()` or `cnd_timedwait()`
//! - Walk up the AST to find the nearest enclosing control structure
//! - Flag violations if the call is within an `if` statement
//! - Accept if within `while`, `for`, or `do_while` loops

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Con36C;

impl CertRule for Con36C {
    fn rule_id(&self) -> &'static str {
        "CON36-C"
    }

    fn description(&self) -> &'static str {
        "Wrap functions that can spuriously wake up in a loop"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "CON36-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Con36C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Look for call expressions
        if node.kind() == "call_expression" {
            if let Some(func) = node.child_by_field_name("function") {
                let func_name = get_node_text(&func, source);

                // Check if this is a spurious wakeup-prone function
                if matches!(func_name, "cnd_wait" | "cnd_timedwait") {
                    // Walk up the AST to find the enclosing control structure
                    if let Some(control_structure) = self.find_enclosing_control_structure(node) {
                        match control_structure.kind() {
                            "if_statement" => {
                                // Violation: spurious wakeup function in if statement
                                let line = node.start_position().row + 1;
                                violations.push(RuleViolation {
                                    rule_id: self.rule_id().to_string(),
                                    severity: Severity::Low,
                                    message: format!(
                                        "Function '{}' can spuriously wake up and must be wrapped in a while loop, not an if statement",
                                        func_name
                                    ),
                                    file_path: String::new(),
                                    line,
                                    column: 0,
                                    suggestion: Some(
                                        format!("Replace 'if' with 'while' to handle spurious wakeups from {}", func_name)
                                    ),
                                    ..Default::default()
                                });
                            }
                            "while_statement" | "for_statement" | "do_statement" => {
                                // Compliant: already in a loop
                            }
                            _ => {
                                // If not in any control structure, or in an unexpected one,
                                // we should flag it as potentially problematic
                                let line = node.start_position().row + 1;
                                violations.push(RuleViolation {
                                    rule_id: self.rule_id().to_string(),
                                    severity: Severity::Low,
                                    message: format!(
                                        "Function '{}' can spuriously wake up and should be wrapped in a while loop checking the condition predicate",
                                        func_name
                                    ),
                                    file_path: String::new(),
                                    line,
                                    column: 0,
                                    suggestion: Some(
                                        "Wrap the call in a while loop that checks the condition predicate".to_string()
                                    ),
                                    ..Default::default()
                                });
                            }
                        }
                    } else {
                        // No enclosing control structure found - definitely a violation
                        let line = node.start_position().row + 1;
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::Low,
                            message: format!(
                                "Function '{}' can spuriously wake up and must be wrapped in a while loop",
                                func_name
                            ),
                            file_path: String::new(),
                            line,
                            column: 0,
                            suggestion: Some(
                                "Wrap the call in a while loop that checks the condition predicate".to_string()
                            ),
                            ..Default::default()
                        });
                    }
                }
            }
        }

        // Recursively check children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }

    /// Walk up the AST to find the nearest enclosing control structure
    /// Skips if statements that appear to be error-handling checks
    /// Returns the first meaningful control structure (while/for/do or non-error-handling if)
    fn find_enclosing_control_structure<'a>(&self, node: &'a Node) -> Option<Node<'a>> {
        let mut current = node.parent();

        while let Some(parent) = current {
            match parent.kind() {
                "if_statement" => {
                    // Check if this is an error-handling if statement
                    if self.is_error_handling_if(&parent, node) {
                        // Skip error-handling if statements, continue looking for the outer loop
                        current = parent.parent();
                        continue;
                    }
                    // Non-error-handling if statement - this is a violation
                    return Some(parent);
                }
                "while_statement" | "for_statement" | "do_statement" => {
                    // Found a loop - this is what we're looking for
                    return Some(parent);
                }
                "function_definition" => {
                    // Stop at function boundary - no control structure found
                    return None;
                }
                _ => {
                    current = parent.parent();
                }
            }
        }

        None
    }

    /// Determine if an if statement is for error handling (checking return value)
    /// Error handling if statements typically check conditions like:
    /// - if (thrd_success != cnd_wait(...))
    /// - if (cnd_wait(...) != thrd_success)
    /// - if (cnd_wait(...) == thrd_error)
    fn is_error_handling_if(&self, if_node: &Node, call_node: &Node) -> bool {
        // Get the condition of the if statement
        if let Some(condition) = if_node.child_by_field_name("condition") {
            // Check if the call_node is a descendant of this condition
            // This would indicate the if is checking the return value of cnd_wait
            self.is_descendant_of(&condition, call_node)
        } else {
            false
        }
    }

    /// Check if target_node is a descendant of parent_node
    fn is_descendant_of(&self, parent_node: &Node, target_node: &Node) -> bool {
        if parent_node.id() == target_node.id() {
            return true;
        }

        for i in 0..parent_node.child_count() {
            if let Some(child) = parent_node.child(i) {
                if self.is_descendant_of(&child, target_node) {
                    return true;
                }
            }
        }

        false
    }
}

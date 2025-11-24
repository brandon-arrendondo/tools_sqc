//! CON41-C: Wrap functions that can fail spuriously in a loop
//!
//! This rule detects calls to atomic_compare_exchange_weak() and
//! atomic_compare_exchange_weak_explicit() that are not wrapped in a loop.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! success = atomic_compare_exchange_weak(ptr_to_head, &old_head, new_head);
//! if (!success) {
//!   cleanup_data_structure(new_head);
//! }
//! ```
//!
//! **Compliant:**
//! ```c
//! do {
//!   if (new_head != NULL) {
//!     cleanup_data_structure(new_head);
//!   }
//!   saved_old_head = old_head;
//! } while (!(success = atomic_compare_exchange_weak(
//!            ptr_to_head, &old_head, new_head
//!          )) && old_head == saved_old_head);
//! ```
//!
//! Or use the strong variant:
//! ```c
//! success = atomic_compare_exchange_strong(ptr_to_head, &old_head, new_head);
//! ```
//!
//! ## Detection Strategy:
//! - Find all calls to atomic_compare_exchange_weak functions
//! - Check if the call is inside a loop (do, while, for)
//! - Report violations when not wrapped in a loop

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Con41C;

impl CertRule for Con41C {
    fn rule_id(&self) -> &'static str {
        "CON41-C"
    }

    fn description(&self) -> &'static str {
        "Wrap functions that can fail spuriously in a loop"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "CON41-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Con41C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Look for call_expression nodes
        if node.kind() == "call_expression" {
            if let Some(func_node) = node.child_by_field_name("function") {
                let func_name = get_node_text(&func_node, source);

                // Check if it's one of the weak compare-exchange functions
                if self.is_weak_compare_exchange(&func_name) {
                    // Check if this call is inside a loop
                    if !self.is_inside_loop(node) {
                        let line = node.start_position().row + 1;
                        let column = node.start_position().column + 1;

                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::Low,
                            message: format!(
                                "Call to '{}' is not wrapped in a loop. This function can fail spuriously and should be wrapped in a do-while loop, or use atomic_compare_exchange_strong() instead.",
                                func_name.trim()
                            ),
                            file_path: String::new(),
                            line,
                            column,
                            suggestion: Some(
                                "Wrap this call in a loop (e.g., do-while) to handle spurious failures, or use atomic_compare_exchange_strong() instead".to_string()
                            ),
                            ..Default::default()
                        });
                    }
                }
            }
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }

    fn is_weak_compare_exchange(&self, func_name: &str) -> bool {
        let trimmed = func_name.trim();
        trimmed == "atomic_compare_exchange_weak"
            || trimmed == "atomic_compare_exchange_weak_explicit"
    }

    fn is_inside_loop(&self, node: &Node) -> bool {
        let mut current = node.parent();

        while let Some(parent) = current {
            match parent.kind() {
                "while_statement" | "do_statement" | "for_statement" => {
                    return true;
                }
                "function_definition" => {
                    // Stop at function boundary
                    return false;
                }
                _ => {
                    current = parent.parent();
                }
            }
        }

        false
    }
}

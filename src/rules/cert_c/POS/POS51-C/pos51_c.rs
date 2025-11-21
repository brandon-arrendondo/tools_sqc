//! POS51-C: Avoid deadlock with POSIX threads by locking in predefined order
//!
//! This rule addresses deadlock conditions that can occur when multiple threads
//! acquire locks in inconsistent orders. Deadlock requires all four conditions:
//! 1. Mutual exclusion
//! 2. Hold and wait
//! 3. No preemption
//! 4. Circular wait
//!
//! ## Non-compliant examples:
//!
//! **Locks acquired in argument order (variable between threads):**
//! ```c
//! void transfer(bank_account *from, bank_account *to, int amount) {
//!     pthread_mutex_lock(&(from->balance_mutex));
//!     pthread_mutex_lock(&(to->balance_mutex));
//!     // Transfer logic
//!     pthread_mutex_unlock(&(to->balance_mutex));
//!     pthread_mutex_unlock(&(from->balance_mutex));
//! }
//! ```
//!
//! ## Compliant solutions:
//!
//! **Conditional ordering based on unique IDs:**
//! ```c
//! void transfer(bank_account *from, bank_account *to, int amount) {
//!     if (from->id == to->id) {
//!         return; // Same account
//!     }
//!     if (from->id < to->id) {
//!         pthread_mutex_lock(&(from->balance_mutex));
//!         pthread_mutex_lock(&(to->balance_mutex));
//!     } else {
//!         pthread_mutex_lock(&(to->balance_mutex));
//!         pthread_mutex_lock(&(from->balance_mutex));
//!     }
//!     // Transfer logic
//! }
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Pos51C;

impl Pos51C {
    pub fn new() -> Self {
        Self
    }

    /// Check if a node is a pthread_mutex_lock() call
    fn is_mutex_lock_call(&self, node: &Node, source: &str) -> bool {
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = get_node_text(&function, source).trim();
                return func_name == "pthread_mutex_lock";
            }
        }
        false
    }

    /// Extract the mutex argument from pthread_mutex_lock() call
    fn get_mutex_argument(&self, node: &Node, source: &str) -> Option<String> {
        if let Some(arguments) = node.child_by_field_name("arguments") {
            for i in 0..arguments.child_count() {
                if let Some(arg) = arguments.child(i) {
                    if arg.kind() != "," && arg.kind() != "(" && arg.kind() != ")" {
                        return Some(get_node_text(&arg, source).trim().to_string());
                    }
                }
            }
        }
        None
    }

    /// Collect all pthread_mutex_lock() calls in a compound statement
    fn collect_mutex_locks<'a>(&self, node: &Node<'a>, source: &str) -> Vec<(Node<'a>, String)> {
        let mut locks = Vec::new();

        if self.is_mutex_lock_call(node, source) {
            if let Some(mutex_arg) = self.get_mutex_argument(node, source) {
                locks.push((*node, mutex_arg));
            }
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                locks.extend(self.collect_mutex_locks(&child, source));
            }
        }

        locks
    }

    /// Check if a node contains conditional statements (if/else) that might order locks
    fn contains_conditional_ordering(&self, node: &Node) -> bool {
        if node.kind() == "if_statement" {
            return true;
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if self.contains_conditional_ordering(&child) {
                    return true;
                }
            }
        }

        false
    }

    /// Check for multiple sequential mutex locks without conditional ordering
    fn check_function_for_deadlock(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if node.kind() != "function_definition" {
            return;
        }

        if let Some(body) = node.child_by_field_name("body") {
            // Collect all mutex lock calls in the function body
            let locks = self.collect_mutex_locks(&body, source);

            // If there are multiple locks, check for potential deadlock
            if locks.len() >= 2 {
                // Check if there's conditional ordering logic
                let has_conditional = self.contains_conditional_ordering(&body);

                // If multiple locks without conditional ordering, report violation
                if !has_conditional {
                    // Check if locks use different variables (not hardcoded)
                    let uses_variables = locks.iter().any(|(_, mutex_arg)| {
                        // Check if argument contains struct field access or pointer dereferencing
                        mutex_arg.contains("->") || mutex_arg.contains('.')
                    });

                    if uses_variables {
                        // Report violation at the location of the first lock
                        if let Some((first_lock, _)) = locks.first() {
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                severity: self.severity(),
                                message: format!(
                                    "Potential deadlock: multiple pthread_mutex_lock() calls ({} locks) without predefined ordering. Locks may be acquired in inconsistent order across threads.",
                                    locks.len()
                                ),
                                file_path: String::new(),
                                line: first_lock.start_position().row + 1,
                                column: first_lock.start_position().column + 1,
                                suggestion: Some(
                                    "Acquire locks in a predefined order (e.g., based on unique resource IDs). Use conditional statements to ensure consistent lock ordering: if (res1->id < res2->id) { lock(res1); lock(res2); } else { lock(res2); lock(res1); }".to_string()
                                ),
                                ..Default::default()
                            });
                        }
                    }
                }
            }
        }
    }
}

impl CertRule for Pos51C {
    fn rule_id(&self) -> &'static str {
        "POS51-C"
    }

    fn description(&self) -> &'static str {
        "Avoid deadlock with POSIX threads by locking in predefined order"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "POS51-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Pos51C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check for deadlock patterns in function definitions
        self.check_function_for_deadlock(node, source, violations);

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }
}

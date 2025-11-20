//! CON37-C: Do not call signal() in a multithreaded program
//!
//! The C Standard (C11 7.14.1.1 paragraph 7) states that the use of the signal()
//! function in a multithreaded program results in undefined behavior.
//!
//! ## Rationale:
//! - Mixing signals and threads causes undefined behavior in C11
//! - Signal handlers execute in an unpredictable thread context
//! - Race conditions and data corruption can occur
//! - POSIX implementations provide defined behavior but are still discouraged
//!
//! ## Examples:
//!
//! **Non-compliant (using signal() in multithreaded program):**
//! ```c
//! #include <signal.h>
//! #include <threads.h>
//!
//! void handler(int signum) { /* ... */ }
//!
//! int main(void) {
//!   signal(SIGINT, handler);  // Violation: signal() in multithreaded context
//!   thrd_t tid;
//!   thrd_create(&tid, thread_func, NULL);
//!   // ...
//! }
//! ```
//!
//! **Compliant (avoid signal() in multithreaded programs):**
//! ```c
//! // Use platform-specific thread-safe signal mechanisms
//! // or avoid mixing signals and threads entirely
//! ```
//!
//! ## Detection Strategy:
//! - Find calls to `signal()` function
//! - Check if the code uses threading functions (thrd_create, pthread_create, etc.)
//! - Flag violations when both are present

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Con37C;

impl CertRule for Con37C {
    fn rule_id(&self) -> &'static str {
        "CON37-C"
    }

    fn description(&self) -> &'static str {
        "Do not call signal() in a multithreaded program"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "CON37-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // First pass: check if the code uses threading functions
        let has_threading = self.has_threading_functions(node, source);

        // Second pass: find signal() calls
        if has_threading {
            self.check_signal_calls(node, source, &mut violations);
        }

        violations
    }
}

impl Con37C {
    /// Check if the code uses threading functions
    fn has_threading_functions(&self, node: &Node, source: &str) -> bool {
        if node.kind() == "call_expression" {
            if let Some(func) = node.child_by_field_name("function") {
                let func_name = get_node_text(&func, source);

                // Check for C11 thread functions and POSIX thread functions
                if matches!(
                    func_name,
                    "thrd_create"
                        | "pthread_create"
                        | "CreateThread"
                        | "thrd_join"
                        | "pthread_join"
                        | "mtx_init"
                        | "pthread_mutex_init"
                ) {
                    return true;
                }
            }
        }

        // Recursively check children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if self.has_threading_functions(&child, source) {
                    return true;
                }
            }
        }

        false
    }

    /// Find and flag signal() calls in multithreaded context
    fn check_signal_calls(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if node.kind() == "call_expression" {
            if let Some(func) = node.child_by_field_name("function") {
                let func_name = get_node_text(&func, source);

                if func_name == "signal" {
                    let line = node.start_position().row + 1;
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::Medium,
                        message: "Call to signal() in a multithreaded program results in undefined behavior (C11 7.14.1.1)".to_string(),
                        file_path: String::new(),
                        line,
                        column: 0,
                        suggestion: Some(
                            "Avoid mixing signals and threads. Use platform-specific thread-safe signal mechanisms or redesign to avoid signal handlers in multithreaded code".to_string()
                        ),
                        ..Default::default()
                    });
                }
            }
        }

        // Recursively check children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_signal_calls(&child, source, violations);
            }
        }
    }
}

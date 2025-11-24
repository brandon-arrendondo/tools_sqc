//! SIG02-C: Avoid using signals to implement normal functionality
//!
//! This rule detects when signals are misused for normal program functionality
//! such as inter-thread communication or synchronization, instead of being
//! reserved for abnormal events.
//!
//! ## Non-compliant example:
//!
//! ```c
//! // Using signals for inter-thread communication
//! void thread_notify(pid_t pid) {
//!     kill(pid, SIGUSR1);  // Misusing signal for normal communication
//! }
//!
//! void handler(int signum) {
//!     // Complex processing in signal handler
//!     process_data();
//!     update_state();
//! }
//!
//! signal(SIGUSR1, handler);
//! ```
//!
//! ## Compliant solution:
//!
//! ```c
//! // Use proper synchronization primitives
//! pthread_mutex_t mutex;
//! pthread_cond_t cond;
//!
//! void thread_notify() {
//!     pthread_mutex_lock(&mutex);
//!     pthread_cond_signal(&cond);
//!     pthread_mutex_unlock(&mutex);
//! }
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Sig02C;

impl Sig02C {
    pub fn new() -> Self {
        Self
    }

    /// Check for kill() calls with user signals (SIGUSR1, SIGUSR2)
    fn check_kill_call(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if node.kind() != "call_expression" {
            return;
        }

        if let Some(function_node) = node.child_by_field_name("function") {
            let function_name = get_node_text(&function_node, source);

            if function_name == "kill" {
                // Check if second argument is SIGUSR1 or SIGUSR2
                if let Some(args_node) = node.child_by_field_name("arguments") {
                    let args_text = get_node_text(&args_node, source);

                    if args_text.contains("SIGUSR1") || args_text.contains("SIGUSR2") {
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: self.severity(),
                            message: format!(
                                "Use of kill() with user-defined signal (SIGUSR1 or SIGUSR2) detected. This suggests using signals for normal inter-process communication instead of abnormal events. Consider using proper IPC mechanisms like message queues, pipes, or condition variables."
                            ),
                            file_path: String::new(),
                            line: node.start_position().row + 1,
                            column: node.start_position().column + 1,
                            suggestion: Some(
                                "Replace signal-based communication with appropriate IPC mechanisms: pthread_cond_t for threads, message queues for processes, or other synchronization primitives."
                                    .to_string(),
                            ),
                            ..Default::default()
                        });
                    }
                }
            }
        }
    }

    /// Check for signal() or sigaction() calls registering handlers for user signals
    fn check_signal_registration(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if node.kind() != "call_expression" {
            return;
        }

        if let Some(function_node) = node.child_by_field_name("function") {
            let function_name = get_node_text(&function_node, source);

            if function_name == "signal" || function_name == "sigaction" {
                // Check if first argument is SIGUSR1 or SIGUSR2
                if let Some(args_node) = node.child_by_field_name("arguments") {
                    let args_text = get_node_text(&args_node, source);

                    if args_text.contains("SIGUSR1") || args_text.contains("SIGUSR2") {
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::Medium,
                            message: format!(
                                "Registering signal handler for user-defined signal (SIGUSR1 or SIGUSR2) with {}(). This suggests using signals for normal functionality instead of abnormal events.",
                                function_name
                            ),
                            file_path: String::new(),
                            line: node.start_position().row + 1,
                            column: node.start_position().column + 1,
                            suggestion: Some(
                                "Signals should be reserved for abnormal events. For normal functionality like inter-thread communication, use condition variables, mutexes, or other synchronization mechanisms."
                                    .to_string(),
                            ),
                            ..Default::default()
                        });
                    }
                }
            }
        }
    }

    /// Check for raise() calls with user signals
    fn check_raise_call(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if node.kind() != "call_expression" {
            return;
        }

        if let Some(function_node) = node.child_by_field_name("function") {
            let function_name = get_node_text(&function_node, source);

            if function_name == "raise" {
                if let Some(args_node) = node.child_by_field_name("arguments") {
                    let args_text = get_node_text(&args_node, source);

                    if args_text.contains("SIGUSR1") || args_text.contains("SIGUSR2") {
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::Medium,
                            message: format!(
                                "Use of raise() with user-defined signal (SIGUSR1 or SIGUSR2) detected. This suggests using signals for normal control flow instead of abnormal events."
                            ),
                            file_path: String::new(),
                            line: node.start_position().row + 1,
                            column: node.start_position().column + 1,
                            suggestion: Some(
                                "Avoid using signals for normal program flow. Use function calls, return values, or proper control structures instead."
                                    .to_string(),
                            ),
                            ..Default::default()
                        });
                    }
                }
            }
        }
    }
}

impl CertRule for Sig02C {
    fn rule_id(&self) -> &'static str {
        "SIG02-C"
    }

    fn description(&self) -> &'static str {
        "Avoid using signals to implement normal functionality"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "SIG02-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Sig02C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check for various signal misuse patterns
        self.check_kill_call(node, source, violations);
        self.check_signal_registration(node, source, violations);
        self.check_raise_call(node, source, violations);

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }
}

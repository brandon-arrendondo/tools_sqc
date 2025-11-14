//! WIN02-C: Restrict privileges when spawning child processes
//!
//! CreateProcess() inherits parent privileges - unsafe.
//! Use CreateProcessAsUser() to specify exact privileges.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! CreateProcess(...);  // VIOLATION: inherits privileges
//! ```
//!
//! **Compliant:**
//! ```c
//! CreateProcessAsUser(token, ...);  // OK: explicit user/token
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{Severity, RuleCategory};
use tree_sitter::Node;

pub struct Win02C;

impl CertRule for Win02C {
    fn rule_id(&self) -> &'static str {
        "WIN02-C"
    }

    fn description(&self) -> &'static str {
        "Restrict privileges when spawning child processes"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "WIN02-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Win02C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = &source[function.start_byte()..function.end_byte()];

                if func_name == "CreateProcess" {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: "CreateProcess() called - spawns child with inherited privileges".to_string(),
                        file_path: String::new(),
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        suggestion: Some(
                            "Use CreateProcessAsUser() with appropriate token to explicitly control child process privileges".to_string()
                        ),
                        ..Default::default()
                    });
                }
            }
        }

        // Recurse
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }
}

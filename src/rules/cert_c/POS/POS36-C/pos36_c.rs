//! POS36-C: Observe correct revocation order while relinquishing privileges
//!
//! When dropping privileges, setgid() must be called BEFORE setuid().
//! Calling setuid() first drops superuser privileges needed by setgid().
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! setuid(getuid());  // Drop user privileges first
//! setgid(getgid());  // VIOLATION: Can't drop group privileges without root
//! ```
//!
//! **Compliant:**
//! ```c
//! setgid(getgid());  // Drop group privileges first (while still root)
//! setuid(getuid());  // Then drop user privileges
//! ```
//!
//! ## Detection Strategy:
//! - Track setuid() and setgid() calls in order
//! - Report violation if setuid() appears before setgid()

use super::super::{CertRule, RuleViolation};
use crate::manifest::{Severity, RuleCategory};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Pos36C;

impl CertRule for Pos36C {
    fn rule_id(&self) -> &'static str {
        "POS36-C"
    }

    fn description(&self) -> &'static str {
        "Observe correct revocation order while relinquishing privileges"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "POS36-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Check function bodies for privilege dropping order
        if node.kind() == "function_definition" {
            if let Some(body) = node.child_by_field_name("body") {
                self.check_privilege_order(&body, source, &mut violations);
            }
        }

        // Also check at file level (code outside functions)
        if node.kind() == "translation_unit" {
            self.check_privilege_order(node, source, &mut violations);
        }

        // Recurse
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                violations.extend(self.check(&child, source));
            }
        }

        violations
    }
}

impl Pos36C {
    fn check_privilege_order(&self, scope: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Collect all setuid() and setgid() calls in order
        let mut calls = Vec::new();
        self.collect_priv_calls(scope, source, &mut calls);

        // Check for setuid before setgid
        let mut seen_setuid = false;
        for call in &calls {
            if call.is_setuid {
                seen_setuid = true;
            } else if call.is_setgid && seen_setuid {
                // setgid after setuid - VIOLATION
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::High,
                    message: "setgid() called after setuid() - incorrect privilege revocation order".to_string(),
                    file_path: String::new(),
                    line: call.line,
                    column: call.column,
                    suggestion: Some(
                        "Call setgid() BEFORE setuid() to ensure group privileges are dropped while still having superuser privileges".to_string()
                    ),
                    ..Default::default()
                });
            }
        }

        // Also report setuid if it appears without a subsequent setgid (incomplete privilege drop)
        if seen_setuid && !calls.iter().any(|c| c.is_setgid) {
            if let Some(setuid_call) = calls.iter().find(|c| c.is_setuid) {
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::High,
                    message: "setuid() called without setgid() - incomplete privilege revocation".to_string(),
                    file_path: String::new(),
                    line: setuid_call.line,
                    column: setuid_call.column,
                    suggestion: Some(
                        "Call setgid() before setuid() to drop both group and user privileges".to_string()
                    ),
                    ..Default::default()
                });
            }
        }
    }

    fn collect_priv_calls(&self, node: &Node, source: &str, calls: &mut Vec<PrivCall>) {
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = get_node_text(&function, source);

                if func_name == "setuid" {
                    calls.push(PrivCall {
                        is_setuid: true,
                        is_setgid: false,
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                    });
                } else if func_name == "setgid" {
                    calls.push(PrivCall {
                        is_setuid: false,
                        is_setgid: true,
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                    });
                }
            }
        }

        // Recurse
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.collect_priv_calls(&child, source, calls);
            }
        }
    }
}

struct PrivCall {
    is_setuid: bool,
    is_setgid: bool,
    line: usize,
    column: usize,
}

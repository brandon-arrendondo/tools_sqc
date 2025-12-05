// SPDX-License-Identifier: MIT
// Copyright (c) 2024 Ryan Urchick

//! CON37-C: Do not call signal() in a multithreaded program
//!
//! Calling signal() in a multithreaded program results in undefined behavior.
//! Use platform-specific thread-safe alternatives instead.

use tree_sitter::Node;

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;

pub struct Con37C {
    pub has_thread_creation: bool,
    pub has_signal_call: bool,
}

impl CertRule for Con37C {
    fn rule_id(&self) -> &'static str {
        "CON37-C"
    }

    fn description(&self) -> &'static str {
        "Do not call signal() in a multithreaded program"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "CON37-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut checker = Con37C {
            has_thread_creation: false,
            has_signal_call: false,
        };

        let mut signal_positions = Vec::new();
        checker.scan_node(node, source, &mut signal_positions);

        let mut violations = Vec::new();

        // If we found both signal() and thread creation, report violations
        if checker.has_thread_creation && checker.has_signal_call {
            for (line, column) in signal_positions {
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: self.severity(),
                    line,
                    column,
                    file_path: String::new(),
                    message:
                        "Calling signal() in a multithreaded program causes undefined behavior."
                            .to_string(),
                    suggestion: Some(
                        "Use sigaction() on POSIX systems or platform-specific thread-safe alternatives."
                            .to_string(),
                    ),
                    requires_manual_review: None,
                });
            }
        }

        violations
    }
}

impl Con37C {
    fn scan_node(&mut self, node: &Node, source: &str, signal_positions: &mut Vec<(usize, usize)>) {
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = get_node_text(&function, source).trim();

                // Check for signal() call
                if func_name == "signal" {
                    self.has_signal_call = true;
                    signal_positions.push((
                        node.start_position().row + 1,
                        node.start_position().column + 1,
                    ));
                }

                // Check for thread creation functions
                if matches!(
                    func_name,
                    "thrd_create"
                        | "pthread_create"
                        | "CreateThread"
                        | "_beginthread"
                        | "_beginthreadex"
                ) {
                    self.has_thread_creation = true;
                }
            }
        }

        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.scan_node(&child, source, signal_positions);
        }
    }
}

// SPDX-License-Identifier: MIT
// Copyright (c) 2024 Ryan Urchick

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use tree_sitter::Node;

pub struct Int00C;

impl CertRule for Int00C {
    fn rule_id(&self) -> &'static str {
        "INT00-C"
    }
    fn description(&self) -> &'static str {
        "Understand integer conversion rules"
    }
    fn severity(&self) -> Severity {
        Severity::Medium
    }
    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }
    fn cert_id(&self) -> &'static str {
        "INT00-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Int00C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if node.kind() == "call_expression" {
            let text = node.utf8_text(source.as_bytes()).unwrap_or("");

            // Check for fscanf/scanf with format specifier mismatch
            if text.contains("fscanf") || text.contains("scanf") {
                // Look for %ld with &int_var or %d with &long_var patterns
                if text.contains("%ld") && !text.contains("long") {
                    // This is a heuristic - %ld should be used with long, not int
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: self.severity(),
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        file_path: String::new(),
                        message: "Format specifier %ld used with int variable; use %d for int"
                            .to_string(),
                        suggestion: Some(
                            "Use correct format specifier matching variable type".to_string(),
                        ),
                        requires_manual_review: None,
                    });
                }
            }
        }

        // Check for unsafe casts like (unsigned long)uint * uint
        if node.kind() == "assignment_expression" {
            let text = node.utf8_text(source.as_bytes()).unwrap_or("");

            // Pattern: c = (unsigned long)a * b where multiplication happens after cast
            // This is unsafe because the multiplication happens at unsigned int precision
            if text.contains("(unsigned long)") && text.contains("*") && !text.contains("uintmax_t")
            {
                // Check if there are unsigned int declarations in context
                let lines_before = source
                    .lines()
                    .take(node.start_position().row)
                    .collect::<Vec<_>>()
                    .join("\n");
                if lines_before.contains("unsigned int") {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: self.severity(),
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        file_path: String::new(),
                        message: "Unsafe integer conversion; result may not fit in unsigned long"
                            .to_string(),
                        suggestion: Some(
                            "Use uintmax_t for guaranteed safe conversion".to_string(),
                        ),
                        requires_manual_review: None,
                    });
                }
            }
        }

        // Recursively check children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }
}

// SPDX-License-Identifier: MIT
// Copyright (c) 2024 Ryan Urchick

use tree_sitter::Node;
use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};

pub struct Fio19C;

impl CertRule for Fio19C {
    fn rule_id(&self) -> &'static str { "FIO19-C" }
    fn description(&self) -> &'static str { "Do not use ftell() to determine file size" }
    fn severity(&self) -> Severity { Severity::Medium }
    fn category(&self) -> RuleCategory { RuleCategory::Rule }
    fn cert_id(&self) -> &'static str { "FIO19-C" }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Fio19C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Look for ftell() being used to determine file size
        // Pattern: variable = ftell(fp) where variable is used with malloc
        if node.kind() == "call_expression" {
            let text = node.utf8_text(source.as_bytes()).unwrap_or("");

            // Check for ftell but not ftello (ftello is acceptable when used properly)
            if text.contains("ftell(") && !text.contains("ftello(") {
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: self.severity(),
                    line: node.start_position().row + 1,
                    column: node.start_position().column + 1,
                    file_path: String::new(),
                    message: "Using ftell() to determine file size is unreliable; use fstat() instead".to_string(),
                    suggestion: Some("Use fstat() with st_size to get accurate file size".to_string()),
                    requires_manual_review: None,
                });
            }
        }

        // Recursively check children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }
}

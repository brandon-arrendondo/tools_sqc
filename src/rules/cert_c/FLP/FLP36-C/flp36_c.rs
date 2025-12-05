// SPDX-License-Identifier: MIT
// Copyright (c) 2024 Ryan Urchick

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use tree_sitter::Node;

pub struct Flp36C;

impl CertRule for Flp36C {
    fn rule_id(&self) -> &'static str {
        "FLP36-C"
    }
    fn description(&self) -> &'static str {
        "Preserve precision when converting between integer and floating point types"
    }
    fn severity(&self) -> Severity {
        Severity::Medium
    }
    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }
    fn cert_id(&self) -> &'static str {
        "FLP36-C"
    }

    fn check(&self, _node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        let mut long_vars = Vec::new();

        for (i, line) in source.lines().enumerate() {
            if line.contains("long int") || (line.contains("long") && !line.contains("double")) {
                let trimmed = line.trim();
                if let Some(pos) = trimmed.find("long") {
                    let after_long = &trimmed[pos..];
                    let after = if after_long.starts_with("long int") {
                        &after_long[8..].trim_start()
                    } else {
                        &after_long[4..].trim_start()
                    };

                    if let Some(space_idx) = after.find(|c: char| !c.is_alphanumeric() && c != '_')
                    {
                        let var_name = &after[..space_idx];
                        if !var_name.is_empty() && !var_name.starts_with("int") {
                            long_vars.push((var_name.to_string(), i));
                        }
                    }
                }
            }
        }

        // Check for FLOAT (not double) assignments using long variables
        for (i, line) in source.lines().enumerate() {
            // Only flag "float" not "double"
            if line.contains("float") && !line.contains("double") && line.contains("=") {
                for (var_name, _decl_line) in &long_vars {
                    if line.contains(var_name) {
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: self.severity(),
                            line: i + 1,
                            column: 1,
                            file_path: String::new(),
                            message: "Converting long int to float may lose precision".to_string(),
                            suggestion: Some("Use double instead of float".to_string()),
                            requires_manual_review: None,
                        });
                        break;
                    }
                }
            }
        }

        violations
    }
}

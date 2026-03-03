// SPDX-License-Identifier: MIT
// Copyright (c) 2024 Ryan Urchick

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use tree_sitter::Node;

pub struct Mem05C;

impl CertRule for Mem05C {
    fn rule_id(&self) -> &'static str {
        "MEM05-C"
    }
    fn description(&self) -> &'static str {
        "Avoid large stack allocations"
    }
    fn severity(&self) -> Severity {
        Severity::Medium
    }
    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }
    fn cert_id(&self) -> &'static str {
        "MEM05-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Mem05C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Look for array declarations with variable size (VLA)
        if node.kind() == "declaration" {
            let text = node.utf8_text(source.as_bytes()).unwrap_or("");

            // Check for array declaration pattern: type name[variable]
            // VLAs have a non-constant size in brackets
            if text.contains('[') && text.contains(']') && !text.contains("malloc") {
                // Extract the part in brackets
                if let Some(start) = text.find('[') {
                    if let Some(end) = text.find(']') {
                        let size_expr = &text[start + 1..end].trim();

                        // If size is not a numeric constant, it might be a VLA.
                        // But ALL_CAPS identifiers are likely preprocessor constants
                        // (e.g., SSL_CERT_BUFFER_SIZE), not runtime values.
                        if !size_expr.is_empty()
                            && !size_expr.chars().all(|c| c.is_numeric())
                            && !Self::is_likely_macro_constant(size_expr)
                        {
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                severity: self.severity(),
                                line: node.start_position().row + 1,
                                column: node.start_position().column + 1,
                                file_path: String::new(),
                                message: "Variable-length array with runtime-sized allocation; \
                                     use malloc instead"
                                    .to_string(),
                                suggestion: Some(
                                    "Use malloc/calloc for dynamic allocation".to_string(),
                                ),
                                requires_manual_review: None,
                            });
                        }
                    }
                }
            }
        }

        // Check for recursive functions (potential stack overflow)
        if node.kind() == "function_definition" {
            let text = node.utf8_text(source.as_bytes()).unwrap_or("");

            // Extract function name from declaration
            if let Some(func_name) = self.extract_function_name(node, source) {
                // Check if function calls itself (simple recursion detection).
                // Use word-boundary matching to avoid matching substrings
                // (e.g., pthread_mutex_init contains mutex_init).
                let call_count = Self::count_word_matches(text, &func_name);
                if call_count > 1 {
                    if true {
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: self.severity(),
                            line: node.start_position().row + 1,
                            column: node.start_position().column + 1,
                            file_path: String::new(),
                            message: "Recursive function can cause excessive stack allocation"
                                .to_string(),
                            suggestion: Some(
                                "Consider iterative approach or limit recursion depth".to_string(),
                            ),
                            requires_manual_review: None,
                        });
                    }
                }
            }
        }

        // Recursively check children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }

    /// Check if a size expression is likely a preprocessor macro constant.
    /// ALL_CAPS identifiers with optional underscores are conventionally macros.
    fn is_likely_macro_constant(expr: &str) -> bool {
        !expr.is_empty()
            && expr
                .chars()
                .all(|c| c.is_ascii_uppercase() || c == '_' || c.is_ascii_digit())
            && expr
                .chars()
                .next()
                .is_some_and(|c| c.is_ascii_uppercase() || c == '_')
    }

    /// Count word-boundary matches of `word` followed by `(` in `text`.
    /// A word boundary means the character before the match (if any) is not
    /// alphanumeric or underscore.
    fn count_word_matches(text: &str, word: &str) -> usize {
        let pattern = format!("{}(", word);
        let mut count = 0;
        let mut search_start = 0;
        while let Some(pos) = text[search_start..].find(&pattern) {
            let abs_pos = search_start + pos;
            // Check word boundary: char before match must not be alphanumeric/_
            let is_word_boundary = if abs_pos == 0 {
                true
            } else {
                let prev = text.as_bytes()[abs_pos - 1];
                !(prev.is_ascii_alphanumeric() || prev == b'_')
            };
            if is_word_boundary {
                count += 1;
            }
            search_start = abs_pos + pattern.len();
        }
        count
    }

    fn extract_function_name(&self, node: &Node, source: &str) -> Option<String> {
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if child.kind() == "function_declarator" {
                let mut inner_cursor = child.walk();
                for inner_child in child.children(&mut inner_cursor) {
                    if inner_child.kind() == "identifier" {
                        return Some(
                            inner_child
                                .utf8_text(source.as_bytes())
                                .unwrap_or("")
                                .to_string(),
                        );
                    }
                }
            }
        }
        None
    }
}

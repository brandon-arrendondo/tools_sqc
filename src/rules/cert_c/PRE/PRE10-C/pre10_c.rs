// SPDX-License-Identifier: MIT
// Copyright (c) 2024 Ryan Urchick

//! PRE10-C: Wrap multistatement macros in a do-while loop
//!
//! Multi-statement macros should be wrapped in do { ... } while(0) to prevent issues
//! when used in control structures without braces.

use tree_sitter::Node;

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;

pub struct Pre10C;

impl CertRule for Pre10C {
    fn rule_id(&self) -> &'static str {
        "PRE10-C"
    }

    fn description(&self) -> &'static str {
        "Wrap multistatement macros in a do-while loop"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "PRE10-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Pre10C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check for preprocessor #define directives
        if node.kind() == "preproc_def" || node.kind() == "preproc_function_def" {
            self.check_macro_definition(node, source, violations);
        }

        // Also check for problematic usage patterns: if without braces followed by multiple statements
        if node.kind() == "if_statement" {
            self.check_if_statement(node, source, violations);
            self.check_semicolon_before_else(node, source, violations);
        }

        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }

    fn check_if_statement(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check if the consequence is not a compound statement
        if let Some(consequence) = node.child_by_field_name("consequence") {
            if consequence.kind() != "compound_statement" {
                let consequence_text = get_node_text(&consequence, source).trim().to_string();

                // Check for identifier or call_expression that looks like a macro call
                // This catches patterns like: if (z == 0) SWAP(x, y);
                if consequence.kind() == "expression_statement" {
                    if let Some(expr) = consequence.named_child(0) {
                        if expr.kind() == "call_expression" {
                            // Get the function name
                            if let Some(func) = expr.child_by_field_name("function") {
                                let func_name = get_node_text(&func, source).trim();
                                // If it's all caps or looks like a macro (common convention)
                                if func_name.chars().all(|c| c.is_uppercase() || c == '_') {
                                    violations.push(RuleViolation {
                                        rule_id: self.rule_id().to_string(),
                                        severity: self.severity(),
                                        line: node.start_position().row + 1,
                                        column: node.start_position().column + 1,
                                        file_path: String::new(),
                                        message:
                                            "Control statement without braces calling what appears to be a macro. \
                                            If the macro expands to multiple statements, this will cause unexpected behavior."
                                                .to_string(),
                                        suggestion: Some(
                                            "Use braces {} around control statement bodies, or wrap multistatement \
                                            macros in do-while(0).".to_string()
                                        ),
                                        requires_manual_review: None,
                                    });
                                }
                            }
                        }
                    }
                }

                // Also check if there are multiple statements following (siblings after this if)
                if let Some(parent) = node.parent() {
                    let parent_text = get_node_text(&parent, source);

                    // If we find a pattern like: if (cond) statement1; statement2;
                    // This suggests a macro expanded to multiple statements
                    let lines_after_if: Vec<&str> = parent_text
                        .lines()
                        .skip_while(|l| !l.contains("if"))
                        .skip(1)
                        .take(3)
                        .collect();

                    let semicolon_count = lines_after_if.iter()
                        .filter(|l| l.contains(';'))
                        .count();

                    if semicolon_count >= 2 {
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: self.severity(),
                            line: node.start_position().row + 1,
                            column: node.start_position().column + 1,
                            file_path: String::new(),
                            message:
                                "Control statement without braces followed by multiple statements. \
                                This may indicate improper macro usage or unwrapped multistatement macro."
                                    .to_string(),
                            suggestion: Some(
                                "Use braces {} around control statement bodies, or wrap multistatement \
                                macros in do-while(0).".to_string()
                            ),
                            requires_manual_review: None,
                        });
                    }
                }
            }
        }
    }

    fn check_semicolon_before_else(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check for pattern: } ; else
        // This happens when a macro that ends in a block is followed by a semicolon
        // The semicolon breaks parsing, so the 'else' might not be in the tree as an alternative

        // Get the full text around this if statement from the source
        let if_end = node.end_byte();

        // Look ahead in the source after the if statement ends
        if if_end < source.len() {
            let remaining_text = &source[if_end..];
            let next_200_chars: String = remaining_text.chars().take(200).collect();

            // Check if we see whitespace/newline, then semicolon, then "else"
            let trimmed = next_200_chars.trim_start();

            if trimmed.starts_with(';') {
                // After the semicolon, we might have comments, so we need to be more careful
                // Just check if "else" appears somewhere in the next bit of text
                // This is a pragmatic approach for detecting the syntax error
                if trimmed.contains("else") {
                    // Make sure "else" is a keyword, not part of a comment or string
                    // Simple heuristic: look for "else" as a word boundary
                    let words: Vec<&str> = trimmed.split_whitespace().collect();
                    if words.iter().any(|w| w.starts_with("else")) {
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: self.severity(),
                            line: node.start_position().row + 1,
                            column: node.start_position().column + 1,
                            file_path: String::new(),
                            message:
                                "Semicolon after compound statement before 'else'. \
                                This indicates a macro that should be wrapped in do-while(0)."
                                    .to_string(),
                            suggestion: Some(
                                "Remove the semicolon or wrap the macro in do { ... } while(0) to allow \
                                a trailing semicolon.".to_string()
                            ),
                            requires_manual_review: None,
                        });
                    }
                }
            }
        }
    }


    fn check_macro_definition(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        let macro_text = get_node_text(node, source);

        // Check if macro contains multiple statements (has semicolons)
        let semicolon_count = macro_text.matches(';').count();

        // If macro has multiple statements (2+ semicolons for multi-statement)
        if semicolon_count >= 2 {
            // Check if it's wrapped in do-while
            let is_wrapped = macro_text.contains("do") &&
                           macro_text.contains("while") &&
                           macro_text.contains('}');

            if !is_wrapped {
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: self.severity(),
                    line: node.start_position().row + 1,
                    column: node.start_position().column + 1,
                    file_path: String::new(),
                    message:
                        "Multi-statement macro should be wrapped in do { ... } while(0) to prevent \
                        control flow issues when used without braces."
                            .to_string(),
                    suggestion: Some(
                        "Wrap macro body: #define MACRO(x) do { statement1; statement2; } while(0)".to_string()
                    ),
                    requires_manual_review: None,
                });
            }
        }
    }
}

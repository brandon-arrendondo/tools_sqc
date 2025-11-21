// SPDX-License-Identifier: MIT
// Copyright (c) 2024 BISSELL Homecare, Inc.

//! EXP47-C: Do not call va_arg with an argument of the incorrect type
//!
//! This rule detects when va_arg is called with a type that doesn't match
//! the type of the actual argument after default argument promotions.
//! Common violations include using va_arg with char, short, or float types
//! which undergo promotion to int/unsigned int or double when passed.
//!
//! CERT C reference:
//! https://wiki.sei.cmu.edu/confluence/display/c/EXP47-C.+Do+not+call+va_arg+with+an+argument+of+the+incorrect+type

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

#[derive(Debug)]
pub struct Exp47C;

impl Exp47C {
    pub fn new() -> Self {
        Exp47C
    }

    /// Check if a type undergoes default argument promotion
    /// Types that promote:
    /// - char, signed char, unsigned char -> int
    /// - short, unsigned short -> int (or unsigned int if larger)
    /// - float -> double
    fn is_promoted_type(&self, type_text: &str) -> Option<&'static str> {
        let trimmed = type_text.trim();

        // Check for char types (promote to int)
        if trimmed == "char"
            || trimmed == "signed char"
            || trimmed == "unsigned char"
            || trimmed.ends_with(" char")
        {
            return Some("int");
        }

        // Check for short types (promote to int or unsigned int)
        if trimmed == "short"
            || trimmed == "short int"
            || trimmed == "signed short"
            || trimmed == "signed short int"
        {
            return Some("int");
        }

        if trimmed == "unsigned short" || trimmed == "unsigned short int" {
            return Some("int or unsigned int");
        }

        // Check for float (promotes to double)
        if trimmed == "float" {
            return Some("double");
        }

        None
    }

    /// Extract the type argument from a va_arg call
    fn extract_va_arg_type<'a>(&self, node: &'a Node, source: &'a str) -> Option<String> {
        // va_arg is typically a macro call_expression
        // Looking for: va_arg(ap, type)
        if node.kind() != "call_expression" {
            return None;
        }

        // Check if this is a va_arg call
        if let Some(function) = node.child_by_field_name("function") {
            let func_name = get_node_text(&function, source);
            if func_name != "va_arg" {
                return None;
            }
        } else {
            return None;
        }

        // Get the arguments
        if let Some(arguments) = node.child_by_field_name("arguments") {
            // arguments is an argument_list node
            let mut cursor = arguments.walk();
            let mut arg_count = 0;

            for child in arguments.children(&mut cursor) {
                // Skip the parentheses and commas
                if child.kind() == "(" || child.kind() == ")" || child.kind() == "," {
                    continue;
                }

                arg_count += 1;

                // The second argument is the type
                if arg_count == 2 {
                    let type_text = get_node_text(&child, source).trim().to_string();
                    return Some(type_text);
                }
            }
        }

        None
    }

    /// Check a va_arg call for incorrect type usage
    fn check_va_arg_call(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(type_text) = self.extract_va_arg_type(node, source) {
            if let Some(correct_type) = self.is_promoted_type(&type_text) {
                violations.push(RuleViolation {
                    rule_id: "EXP47-C".to_string(),
                    severity: Severity::Medium,
                    line: node.start_position().row + 1,
                    column: node.start_position().column + 1,
                    message: format!(
                        "va_arg called with type '{}' which undergoes default argument promotion; use '{}' instead",
                        type_text, correct_type
                    ),
                    file_path: String::new(),
                    suggestion: Some(format!(
                        "Change va_arg type to '{}' and cast the result if needed: ({})va_arg(ap, {})",
                        correct_type, type_text, correct_type
                    )),
                    requires_manual_review: Some(false),
                });
            }
        }
    }

    /// Recursively traverse AST
    fn traverse(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check if this is a call expression (potential va_arg call)
        if node.kind() == "call_expression" {
            self.check_va_arg_call(node, source, violations);
        }

        // Recurse into children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.traverse(&child, source, violations);
        }
    }
}

impl CertRule for Exp47C {
    fn rule_id(&self) -> &'static str {
        "EXP47-C"
    }

    fn description(&self) -> &'static str {
        "Do not call va_arg with an argument of the incorrect type"
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn cert_id(&self) -> &'static str {
        "EXP47-C"
    }

    fn check(&self, root: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.traverse(root, source, &mut violations);
        violations
    }
}

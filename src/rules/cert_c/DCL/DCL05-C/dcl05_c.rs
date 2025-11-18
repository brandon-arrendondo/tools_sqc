// SPDX-License-Identifier: MIT OR Apache-2.0
// Copyright (c) 2025 Ryan Bissell

//! DCL05-C: Use typedefs of non-pointer types only
//!
//! This rule detects typedefs that define pointer types, which can lead to
//! confusion about const-qualification and make code harder to understand.
//!
//! Violations:
//! - typedef int *IntPtr;  // typedef of a pointer type
//!
//! Compliant:
//! - typedef int Integer;  // typedef of non-pointer type
//! - Integer *ptr;         // pointer declared explicitly
//!
//! References:
//! - https://wiki.sei.cmu.edu/confluence/display/c/DCL05-C.+Use+typedefs+of+non-pointer+types+only

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Dcl05C;

impl CertRule for Dcl05C {
    fn rule_id(&self) -> &'static str {
        "DCL05-C"
    }

    fn description(&self) -> &'static str {
        "Use typedefs of non-pointer types only"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "DCL05-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        check_typedef_declarations(node, source, &mut violations);
        check_complex_function_pointers(node, source, &mut violations);
        violations
    }
}

/// Check for typedef declarations that define pointer types
fn check_typedef_declarations(node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
    if node.kind() == "type_definition" {
        // Check if this typedef defines a pointer type
        if is_pointer_typedef(node, source) {
            // Extract the typedef name for better error message
            let typedef_name =
                extract_typedef_name(node, source).unwrap_or_else(|| "unknown".to_string());

            violations.push(RuleViolation {
                rule_id: "DCL05-C".to_string(),
                file_path: "".to_string(),
                message: format!(
                    "Typedef '{}' defines a pointer type, which can cause confusion with const-qualification",
                    typedef_name
                ),
                line: node.start_position().row + 1,
                column: node.start_position().column,
                severity: Severity::Medium,
                suggestion: Some("Use typedef of non-pointer type and declare pointers explicitly at point of use".to_string()),
                requires_manual_review: Some(false),
            });
        }
    }

    // Recurse through children
    let mut cursor = node.walk();
    for child in node.children(&mut cursor) {
        check_typedef_declarations(&child, source, violations);
    }
}

/// Check if a type_definition node defines a pointer type
fn is_pointer_typedef(node: &Node, source: &str) -> bool {
    // Look for pointer_declarator in the typedef
    contains_pointer_declarator(node)
}

/// Recursively check if node tree contains a pointer_declarator
fn contains_pointer_declarator(node: &Node) -> bool {
    if node.kind() == "pointer_declarator" {
        return true;
    }

    let mut cursor = node.walk();
    for child in node.children(&mut cursor) {
        if contains_pointer_declarator(&child) {
            return true;
        }
    }
    false
}

/// Extract the typedef name from a type_definition node
fn extract_typedef_name(node: &Node, source: &str) -> Option<String> {
    // The typedef name is usually in a type_identifier node
    find_type_identifier(node, source)
}

/// Recursively find a type_identifier node
fn find_type_identifier(node: &Node, source: &str) -> Option<String> {
    if node.kind() == "type_identifier" {
        return Some(get_node_text(node, source).to_string());
    }

    let mut cursor = node.walk();
    for child in node.children(&mut cursor) {
        if let Some(id) = find_type_identifier(&child, source) {
            return Some(id);
        }
    }
    None
}

/// Check for complex function pointer declarations without typedef
fn check_complex_function_pointers(node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
    // Look for function declarations with complex function pointer parameters or return types
    if node.kind() == "function_declarator" || node.kind() == "declaration" {
        let text = get_node_text(node, source);

        // Detect complex function pointer syntax: contains (* and multiple parentheses
        if is_complex_function_pointer_syntax(&text) {
            violations.push(RuleViolation {
                rule_id: "DCL05-C".to_string(),
                file_path: "".to_string(),
                message: "Complex function pointer declaration should use typedef for clarity"
                    .to_string(),
                line: node.start_position().row + 1,
                column: node.start_position().column,
                severity: Severity::Medium,
                suggestion: Some(
                    "Use typedef to simplify complex function pointer declarations".to_string(),
                ),
                requires_manual_review: Some(false),
            });
        }
    }

    // Recurse through children
    let mut cursor = node.walk();
    for child in node.children(&mut cursor) {
        check_complex_function_pointers(&child, source, violations);
    }
}

/// Check if text contains complex function pointer syntax
fn is_complex_function_pointer_syntax(text: &str) -> bool {
    // Complex function pointer has pattern like: void (*signal(int, void (*)(int)))(int);
    // Look for (*...)(  pattern which indicates function pointer return type
    let has_func_ptr_return = text.contains("(*") && text.matches('(').count() >= 3;

    // Also check for function pointers in parameter lists without typedef
    // Pattern: void (*)(int) - unnamed function pointer parameter
    let has_unnamed_func_ptr = text.contains("(*)(");

    has_func_ptr_return || has_unnamed_func_ptr
}

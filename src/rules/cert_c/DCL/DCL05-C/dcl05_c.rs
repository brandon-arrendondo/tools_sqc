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

        // First pass: collect typedef'd pointer types
        let mut pointer_typedefs = std::collections::HashSet::new();
        collect_pointer_typedefs(node, source, &mut pointer_typedefs);

        // Second pass: check usage of pointer typedefs with const
        check_const_pointer_typedef_usage(node, source, &pointer_typedefs, &mut violations);

        check_typedef_declarations(node, source, &mut violations);
        check_complex_function_pointers(node, source, &mut violations);
        violations
    }
}

/// Collect all typedef names that are pointer types
fn collect_pointer_typedefs(
    node: &Node,
    source: &str,
    pointer_typedefs: &mut std::collections::HashSet<String>,
) {
    if node.kind() == "type_definition" {
        if is_pointer_typedef(node, source) {
            if let Some(name) = extract_typedef_name(node, source) {
                pointer_typedefs.insert(name);
            }
        }
    }

    let mut cursor = node.walk();
    for child in node.children(&mut cursor) {
        collect_pointer_typedefs(&child, source, pointer_typedefs);
    }
}

/// Check for const used with pointer typedef (const makes pointer const, not data)
fn check_const_pointer_typedef_usage(
    node: &Node,
    source: &str,
    pointer_typedefs: &std::collections::HashSet<String>,
    violations: &mut Vec<RuleViolation>,
) {
    // Look for parameter declarations with const and pointer typedef
    // BUT NOT typedef definitions themselves (those are fine)
    if node.kind() == "parameter_declaration" {
        // Make sure we're not inside a typedef definition
        if is_inside_typedef(node) {
            return;
        }

        // Check if parameter has const qualifier (might be separate node)
        let has_const = has_const_qualifier(node, source);

        if let Some(type_node) = node.child_by_field_name("type") {
            let type_text = get_node_text(&type_node, source);

            // Check if const is used with a known pointer typedef
            // const might be in type_text OR in a separate qualifier node
            if has_const || type_text.contains("const") {
                // Check known pointer typedefs collected from the file
                for typedef_name in pointer_typedefs {
                    if type_text.contains(typedef_name) {
                        violations.push(RuleViolation {
                            rule_id: "DCL05-C".to_string(),
                            file_path: "".to_string(),
                            message: format!(
                                "Using 'const' with pointer typedef '{}' makes the pointer const, not the pointed-to data",
                                typedef_name
                            ),
                            line: node.start_position().row + 1,
                            column: node.start_position().column,
                            severity: Severity::Medium,
                            suggestion: Some(format!(
                                "Define a new typedef for const data (e.g., 'typedef const POINT *LPC{}') \
                                or use explicit pointer syntax", typedef_name
                            )),
                            requires_manual_review: Some(false),
                        });
                        return; // Only report once per parameter
                    }
                }

                // Also check for common Windows/library pointer typedef patterns
                // These follow convention: LP* (Long Pointer), P* (Pointer), *PTR (pointer suffix)
                if is_likely_pointer_typedef(&type_text) {
                    // Extract the type name from the const declaration
                    if let Some(typedef_name) = extract_type_from_const_decl(&type_text) {
                        violations.push(RuleViolation {
                            rule_id: "DCL05-C".to_string(),
                            file_path: "".to_string(),
                            message: format!(
                                "Using 'const' with pointer typedef '{}' makes the pointer const, not the pointed-to data",
                                typedef_name
                            ),
                            line: node.start_position().row + 1,
                            column: node.start_position().column,
                            severity: Severity::Medium,
                            suggestion: Some(format!(
                                "Define a new typedef for const data or use explicit pointer syntax"
                            )),
                            requires_manual_review: Some(false),
                        });
                    }
                }
            }
        }
    }

    let mut cursor = node.walk();
    for child in node.children(&mut cursor) {
        check_const_pointer_typedef_usage(&child, source, pointer_typedefs, violations);
    }
}

/// Check if node is inside a typedef definition
fn is_inside_typedef(node: &Node) -> bool {
    let mut current = node.parent();
    while let Some(parent) = current {
        if parent.kind() == "type_definition" {
            return true;
        }
        current = parent.parent();
    }
    false
}

/// Check if a parameter declaration has a const qualifier
fn has_const_qualifier(node: &Node, source: &str) -> bool {
    // Check for type_qualifier nodes containing "const"
    let mut cursor = node.walk();
    for child in node.children(&mut cursor) {
        if child.kind() == "type_qualifier" {
            let text = get_node_text(&child, source);
            if text.contains("const") {
                return true;
            }
        }
    }
    false
}

/// Check if a type name likely represents a pointer typedef
fn is_likely_pointer_typedef(type_text: &str) -> bool {
    // Common patterns for pointer typedefs:
    // - Starts with LP (Long Pointer - Windows convention)
    // - Starts with P (Pointer - common convention)
    // - Ends with PTR or POINTER
    // - Ends with _PTR or _POINTER

    let words: Vec<&str> = type_text.split_whitespace().collect();
    for word in words {
        // Skip const keyword
        if word == "const" {
            continue;
        }

        // Windows-style: LPPOINT, LPSTR, etc.
        if word.starts_with("LP") && word.len() > 2 {
            let rest = &word[2..];
            // Check if rest starts with uppercase (LPPOINT not LPprivate)
            if rest.chars().next().map_or(false, |c| c.is_uppercase()) {
                return true;
            }
        }

        // Pointer suffix: INTPTR, STRING_PTR, etc.
        if word.ends_with("PTR") || word.ends_with("POINTER") || word.ends_with("_PTR") {
            return true;
        }

        // Some P-prefix types: PSTR, PWSTR, etc. (but not generic P)
        if word.starts_with('P') && word.len() > 2 && word.chars().all(|c| c.is_uppercase()) {
            return true;
        }
    }

    false
}

/// Extract the type name from a const declaration (e.g., "const LPPOINT" -> "LPPOINT")
fn extract_type_from_const_decl(type_text: &str) -> Option<String> {
    let words: Vec<&str> = type_text.split_whitespace().collect();
    for word in words {
        if word != "const" {
            return Some(word.to_string());
        }
    }
    None
}

/// Check for typedef declarations that define pointer types
fn check_typedef_declarations(node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
    if node.kind() == "type_definition" {
        // Check if this typedef defines a pointer type
        if is_pointer_typedef(node, source) {
            // If the typedef includes const in its definition, it's compliant
            // Example: typedef const POINT *LPCPOINT; - this is OK
            let typedef_text = get_node_text(node, source);
            if typedef_text.contains("const") {
                // This typedef properly includes const, so it's OK
                return;
            }

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

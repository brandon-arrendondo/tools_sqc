//! API02-C: Functions that read or write to or from an array should take an argument
//! to specify the source or target size
//!
//! This rule detects function declarations where array/pointer parameters are not
//! accompanied by size parameters. Functions operating on arrays must accept an
//! additional parameter indicating maximum element count to prevent buffer overflows.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! char *strncpy(char *s1, const char *s2, size_t n);
//! // s1 and s2 are arrays, but no size parameters for them
//! // 'n' is copy count, not array capacity
//! ```
//!
//! **Compliant:**
//! ```c
//! char *improved_strncpy(char *s1, size_t s1count,
//!                        const char *s2, size_t s2count, size_t n);
//! // Each array has explicit size parameter
//! ```
//!
//! ## Detection Strategy:
//! - Find function_declarator nodes
//! - Identify pointer parameters (potential arrays)
//! - Check if immediately followed by size_t parameter
//! - Report if pointer lacks corresponding size parameter

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use crate::utility::cert_c::declarator_utils::is_pointer_declarator;
use tree_sitter::Node;

pub struct Api02C;

impl CertRule for Api02C {
    fn rule_id(&self) -> &'static str {
        "API02-C"
    }

    fn description(&self) -> &'static str {
        "Functions that read or write to or from an array should take an argument to specify the source or target size"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "API02-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Api02C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Look for function declarations
        if node.kind() == "declaration" {
            if let Some(declarator) = node.child_by_field_name("declarator") {
                if self.is_function_declarator(&declarator) {
                    self.check_function_parameters(&declarator, node, source, violations);
                }
            }
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }

    fn is_function_declarator(&self, node: &Node) -> bool {
        if node.kind() == "function_declarator" {
            return true;
        }

        // Check children (might be wrapped in pointer_declarator)
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if self.is_function_declarator(&child) {
                    return true;
                }
            }
        }

        false
    }

    fn check_function_parameters(
        &self,
        declarator: &Node,
        declaration: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        self.check_parameters_recursive(declarator, declaration, source, violations);
    }

    fn check_parameters_recursive(
        &self,
        node: &Node,
        declaration: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Found parameter_list
        if node.kind() == "parameter_list" {
            // Collect all parameters
            let mut params = Vec::new();
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() == "parameter_declaration" {
                        params.push(child);
                    }
                }
            }

            // Check each pointer parameter for missing size
            for i in 0..params.len() {
                if self.is_pointer_parameter(&params[i], source) {
                    // Check if next parameter is size_t
                    if i + 1 >= params.len() || !self.is_size_t_parameter(&params[i + 1], source) {
                        self.report_violation(declaration, &params[i], source, violations);
                    }
                }
            }
            return;
        }

        // Recursively search children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_parameters_recursive(&child, declaration, source, violations);
            }
        }
    }

    fn is_pointer_parameter(&self, param: &Node, source: &str) -> bool {
        // Get the type
        let type_node = match param.child_by_field_name("type") {
            Some(t) => t,
            None => return false,
        };

        let type_text = get_node_text(&type_node, source);

        // Skip const char * parameters — these are conventionally null-terminated
        // string inputs that use the null terminator as bounds, not explicit size.
        // This matches standard C conventions (strcmp, strdup, printf format, etc.)
        let param_text = get_node_text(param, source);
        let normalized = param_text.split_whitespace().collect::<Vec<_>>().join(" ");
        if normalized.starts_with("const char *") || normalized.starts_with("const char*") {
            return false;
        }
        // Also handle the tree-sitter pattern where const is a type_qualifier
        // and char is the type — check if type is "char" and there's a const qualifier
        if type_text == "char" {
            let mut has_const = false;
            for i in 0..param.child_count() {
                if let Some(child) = param.child(i) {
                    if child.kind() == "type_qualifier" {
                        let q = get_node_text(&child, source);
                        if q == "const" {
                            has_const = true;
                        }
                    }
                }
            }
            if has_const {
                if let Some(declarator) = param.child_by_field_name("declarator") {
                    if is_pointer_declarator(&declarator) {
                        return false;
                    }
                }
            }
        }

        // Check for pointer type
        if type_text.contains('*') {
            // Exclude function pointers (they're not arrays)
            if !type_text.contains("(*") && !type_text.contains("(* ") {
                return true;
            }
        }

        // Check declarator for pointer
        if let Some(declarator) = param.child_by_field_name("declarator") {
            if is_pointer_declarator(&declarator) {
                return true;
            }
        }

        false
    }

    fn is_size_t_parameter(&self, param: &Node, source: &str) -> bool {
        let type_node = match param.child_by_field_name("type") {
            Some(t) => t,
            None => return false,
        };

        let type_text = get_node_text(&type_node, source);

        // Accept size_t and common integer types used for sizes in embedded codebases
        matches!(
            type_text,
            "size_t"
                | "uint32_t"
                | "uint16_t"
                | "uint8_t"
                | "int32_t"
                | "int"
                | "unsigned"
                | "unsigned int"
                | "rsize_t"
        )
    }

    fn report_violation(
        &self,
        declaration: &Node,
        pointer_param: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let param_text = get_node_text(pointer_param, source);
        let decl_text = get_node_text(declaration, source);

        violations.push(RuleViolation {
            rule_id: self.rule_id().to_string(),
            severity: Severity::High,
            message: format!(
                "Function has pointer parameter without size argument: '{}' - Add size_t parameter to specify array capacity",
                decl_text.lines().next().unwrap_or(decl_text).trim()
            ),
            file_path: String::new(),
            line: declaration.start_position().row + 1,
            column: declaration.start_position().column + 1,
            suggestion: Some(format!(
                "Add a size_t parameter after '{}' to specify the maximum number of elements in the array",
                param_text.trim()
            )),
            ..Default::default()
        });
    }
}

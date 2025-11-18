//! ARR01-C: Do not apply the sizeof operator to a pointer when taking the size of an array
//!
//! This rule detects when `sizeof` is applied to array function parameters, which
//! have decayed to pointers. The sizeof operator on such parameters returns the
//! size of a pointer, not the size of the array.
//!
//! # Violation Patterns
//!
//! ```c
//! void function(int array[]) {
//!     size_t size = sizeof(array);  // VIOLATION: sizeof(int*), not sizeof(array)
//! }
//! ```
//!
//! ```c
//! void function(int array[10]) {
//!     size_t count = sizeof(array) / sizeof(array[0]);  // VIOLATION: still a pointer
//! }
//! ```
//!
//! # Compliant Solutions
//!
//! ```c
//! void function(int array[], size_t array_size) {
//!     // Pass size separately
//! }
//! ```
//!
//! ```c
//! int main() {
//!     int array[10];
//!     size_t size = sizeof(array);  // OK: array is a true array, not a parameter
//! }
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::{find_containing_function, get_node_text};
use std::collections::HashMap;
use tree_sitter::Node;

pub struct Arr01C;

impl CertRule for Arr01C {
    fn rule_id(&self) -> &'static str {
        "ARR01-C"
    }

    fn description(&self) -> &'static str {
        "Do not apply the sizeof operator to a pointer when taking the size of an array"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "ARR01-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Process each function definition separately to maintain proper scope
        self.check_function_definitions(node, source, &mut violations);

        // Also check for incomplete arrays (global scope)
        let mut incomplete_arrays = HashMap::new();
        self.collect_incomplete_arrays(node, source, &mut incomplete_arrays);
        self.check_sizeof_expressions(node, source, &incomplete_arrays, &mut violations);

        violations
    }
}

impl Arr01C {
    fn check_function_definitions(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if node.kind() == "function_definition" {
            // Collect parameters for this specific function
            let mut function_params = HashMap::new();
            if let Some(declarator) = node.child_by_field_name("declarator") {
                self.extract_array_params_from_declarator(
                    &declarator,
                    source,
                    &mut function_params,
                );
            }

            // Check sizeof expressions within this function's body
            if let Some(body) = node.child_by_field_name("body") {
                self.check_sizeof_expressions(&body, source, &function_params, violations);
            }
        }

        // Recursively process child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_function_definitions(&child, source, violations);
            }
        }
    }

    /// Collect all function parameters (both array syntax and pointer types)
    /// These parameters may have decayed from arrays, making sizeof incorrect
    fn collect_array_parameters(&self, node: &Node, source: &str) -> HashMap<String, usize> {
        let mut array_params = HashMap::new();
        self.collect_array_params_recursive(node, source, &mut array_params);
        self.collect_incomplete_arrays(node, source, &mut array_params);
        array_params
    }

    /// Collect incomplete array declarations (e.g., extern int arr[])
    /// Only collect arrays without size AND without initializer at global/file scope
    fn collect_incomplete_arrays(
        &self,
        node: &Node,
        source: &str,
        array_params: &mut HashMap<String, usize>,
    ) {
        // Only check direct children of translation_unit (file-scope declarations)
        if node.kind() == "translation_unit" {
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() == "declaration" {
                        self.check_global_declaration(&child, source, array_params);
                    }
                }
            }
        }
    }

    fn check_global_declaration(
        &self,
        node: &Node,
        source: &str,
        array_params: &mut HashMap<String, usize>,
    ) {
        // Check if this has an initializer
        let has_initializer = node.child_by_field_name("value").is_some();

        // Only process if no initializer (incomplete array)
        if !has_initializer {
            // Look for array declarators with no size
            if let Some(declarator) = node.child_by_field_name("declarator") {
                if self.is_incomplete_array_declarator(&declarator) {
                    if let Some(name) = self.extract_param_name(&declarator, source) {
                        let line = node.start_position().row + 1;
                        array_params.insert(name, line);
                    }
                }
            }
        }
    }

    fn is_incomplete_array_declarator(&self, declarator: &Node) -> bool {
        // Check for array_declarator with empty size
        if declarator.kind() == "array_declarator" {
            // If size field is missing or empty, it's incomplete
            if declarator.child_by_field_name("size").is_none() {
                return true;
            }
        }

        // Check children recursively
        for i in 0..declarator.child_count() {
            if let Some(child) = declarator.child(i) {
                if self.is_incomplete_array_declarator(&child) {
                    return true;
                }
            }
        }

        false
    }

    fn collect_array_params_recursive(
        &self,
        node: &Node,
        source: &str,
        array_params: &mut HashMap<String, usize>,
    ) {
        // Look for function definitions
        if node.kind() == "function_definition" {
            // Get the function declarator
            if let Some(declarator) = node.child_by_field_name("declarator") {
                self.extract_array_params_from_declarator(&declarator, source, array_params);
            }
        }

        // Recursively process child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.collect_array_params_recursive(&child, source, array_params);
            }
        }
    }

    fn extract_array_params_from_declarator(
        &self,
        declarator: &Node,
        source: &str,
        array_params: &mut HashMap<String, usize>,
    ) {
        // Find parameters list
        if let Some(params_node) = self.find_parameters_node(declarator) {
            // Process each parameter
            for i in 0..params_node.child_count() {
                if let Some(param) = params_node.child(i) {
                    if param.kind() == "parameter_declaration" {
                        self.process_parameter_declaration(&param, source, array_params);
                    }
                }
            }
        }
    }

    fn find_parameters_node<'a>(&self, node: &Node<'a>) -> Option<Node<'a>> {
        // For function_declarator, parameters is a direct field
        if node.kind() == "function_declarator" {
            return node.child_by_field_name("parameters");
        }

        // Recursively search in child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "function_declarator" {
                    return child.child_by_field_name("parameters");
                }
                if let Some(found) = self.find_parameters_node(&child) {
                    return Some(found);
                }
            }
        }

        None
    }

    fn process_parameter_declaration(
        &self,
        param: &Node,
        source: &str,
        array_params: &mut HashMap<String, usize>,
    ) {
        // Get the declarator
        if let Some(declarator) = param.child_by_field_name("declarator") {
            // Check if it's an array declarator OR pointer declarator
            let is_array = self.is_array_declarator(&declarator);
            let is_pointer = self.is_pointer_declarator(&declarator);

            if is_array || is_pointer {
                // Extract parameter name
                if let Some(param_name) = self.extract_param_name(&declarator, source) {
                    let line = param.start_position().row + 1;
                    array_params.insert(param_name, line);
                }
            }
        }
    }

    fn is_pointer_declarator(&self, declarator: &Node) -> bool {
        // Check if this is a pointer_declarator
        if declarator.kind() == "pointer_declarator" {
            return true;
        }

        // Check children
        for i in 0..declarator.child_count() {
            if let Some(child) = declarator.child(i) {
                if self.is_pointer_declarator(&child) {
                    return true;
                }
            }
        }

        false
    }

    fn is_array_declarator(&self, declarator: &Node) -> bool {
        // Check if this declarator or any child is an array_declarator
        if declarator.kind() == "array_declarator" {
            return true;
        }

        // Check children recursively
        for i in 0..declarator.child_count() {
            if let Some(child) = declarator.child(i) {
                if self.is_array_declarator(&child) {
                    return true;
                }
            }
        }

        false
    }

    fn extract_param_name(&self, declarator: &Node, source: &str) -> Option<String> {
        match declarator.kind() {
            "identifier" => Some(get_node_text(declarator, source).to_string()),
            "array_declarator" => {
                // The identifier is in the 'declarator' field of array_declarator
                if let Some(inner_declarator) = declarator.child_by_field_name("declarator") {
                    self.extract_param_name(&inner_declarator, source)
                } else {
                    None
                }
            }
            "pointer_declarator" => {
                // Handle pointer-to-array cases
                if let Some(inner_declarator) = declarator.child_by_field_name("declarator") {
                    self.extract_param_name(&inner_declarator, source)
                } else {
                    None
                }
            }
            _ => {
                // Search children
                for i in 0..declarator.child_count() {
                    if let Some(child) = declarator.child(i) {
                        if let Some(name) = self.extract_param_name(&child, source) {
                            return Some(name);
                        }
                    }
                }
                None
            }
        }
    }

    fn check_sizeof_expressions(
        &self,
        node: &Node,
        source: &str,
        array_params: &HashMap<String, usize>,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Look for sizeof_expression nodes
        if node.kind() == "sizeof_expression" {
            self.check_sizeof_operand(node, source, array_params, violations);
        }

        // Recursively check children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_sizeof_expressions(&child, source, array_params, violations);
            }
        }
    }

    fn check_sizeof_operand(
        &self,
        sizeof_node: &Node,
        source: &str,
        array_params: &HashMap<String, usize>,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Get the operand of sizeof
        if let Some(value_node) = sizeof_node.child_by_field_name("value") {
            // Check for flexible array member access (e.g., struct->member)
            if self.is_flexible_array_member_access(&value_node, source) {
                let start_point = sizeof_node.start_position();
                let sizeof_text = get_node_text(sizeof_node, source);

                violations.push(RuleViolation {
                    rule_id: "ARR01-C".to_string(),
                    severity: Severity::High,
                    message: "sizeof applied to flexible array member".to_string(),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some(format!(
                        "Do not use '{}' on flexible array members. \
                        Flexible array members have indeterminate size.",
                        sizeof_text
                    )),
                    ..Default::default()
                });
                return;
            }

            // Extract variable names from the operand
            let var_names = self.extract_variable_names(&value_node, source);

            // Check if any of these variables are array parameters
            for var_name in var_names {
                if array_params.contains_key(&var_name) {
                    // Verify this sizeof is within the same function where the parameter is declared
                    if self.is_sizeof_in_same_function(sizeof_node, &var_name, array_params) {
                        let start_point = sizeof_node.start_position();
                        let sizeof_text = get_node_text(sizeof_node, source);

                        violations.push(RuleViolation {
                            rule_id: "ARR01-C".to_string(),
                            severity: Severity::High,
                            message: format!(
                                "sizeof applied to pointer/array parameter '{}' which has decayed to a pointer",
                                var_name
                            ),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some(format!(
                                "Do not use '{}'. Array/pointer parameters decay to pointers. \
                                Pass the array size as a separate parameter instead.",
                                sizeof_text
                            )),
                            ..Default::default()
                        });
                    }
                }
            }
        }
    }

    fn is_flexible_array_member_access(&self, node: &Node, source: &str) -> bool {
        // Check if this is a field_expression (struct->member or struct.member)
        if node.kind() == "field_expression" {
            // Get the field name
            if let Some(field_node) = node.child_by_field_name("field") {
                let field_text = get_node_text(&field_node, source);
                // Common flexible array member patterns: data, array, items, etc.
                // But we need to check if it's actually a flexible array...
                // For now, check if the field looks like an array access pattern
                // This is a heuristic - a better approach would track struct definitions

                // Check if the parent struct has an array member
                // For simplicity, we'll check if field is named commonly for flexible arrays
                // or if we can find the struct definition
                return self.looks_like_flexible_array_field(field_text);
            }
        }

        false
    }

    fn looks_like_flexible_array_field(&self, field_name: &str) -> bool {
        // Common names for flexible array members
        matches!(
            field_name,
            "data" | "array" | "items" | "elements" | "buffer" | "payload"
        )
    }

    fn extract_variable_names(&self, node: &Node, source: &str) -> Vec<String> {
        let mut names = Vec::new();
        self.extract_variable_names_recursive(node, source, &mut names);
        names
    }

    fn extract_variable_names_recursive(&self, node: &Node, source: &str, names: &mut Vec<String>) {
        if node.kind() == "identifier" {
            names.push(get_node_text(node, source).to_string());
        }

        // Recursively search children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.extract_variable_names_recursive(&child, source, names);
            }
        }
    }

    fn is_sizeof_in_same_function(
        &self,
        sizeof_node: &Node,
        param_name: &str,
        array_params: &HashMap<String, usize>,
    ) -> bool {
        // Find the containing function
        if let Some(_func_node) = find_containing_function(sizeof_node) {
            // If we found the parameter in our map, and we're in a function,
            // assume they're in the same scope (this is a simplification)
            // A more robust check would compare function boundaries
            array_params.contains_key(param_name)
        } else {
            false
        }
    }
}

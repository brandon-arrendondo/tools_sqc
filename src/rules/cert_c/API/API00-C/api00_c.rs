//! API00-C: Functions should validate their parameters
//!
//! This rule detects functions that use pointer parameters without validating them first.
//! Proper parameter validation helps prevent NULL pointer dereferences and other undefined behavior.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! void setfile(FILE *file) {
//!     myFile = file;  // No validation of file parameter
//! }
//!
//! int string_length(const char *str) {
//!     return strlen(str);  // No NULL check before use
//! }
//! ```
//!
//! **Compliant:**
//! ```c
//! errno_t setfile(FILE *file) {
//!     if (file && !ferror(file) && !feof(file)) {
//!         myFile = file;
//!         return 0;
//!     }
//!     return -1;  // Error handling
//! }
//!
//! int safe_string_copy(char *dest, const char *src) {
//!     if (!dest || !src) {
//!         return -1;  // Validation before use
//!     }
//!     strcpy(dest, src);
//!     return 0;
//! }
//! ```
//!
//! ## Detection Strategy:
//! - Find function definitions with pointer parameters
//! - Check if pointer parameters are validated (NULL check) before being used
//! - Report violation if a pointer parameter is used without prior validation

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::{get_function_parameters, get_node_text, is_pointer_type};
use std::collections::HashSet;
use tree_sitter::Node;

pub struct Api00C;

impl CertRule for Api00C {
    fn rule_id(&self) -> &'static str {
        "API00-C"
    }

    fn description(&self) -> &'static str {
        "Functions should validate their parameters"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "API00-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Api00C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Look for function definitions
        if node.kind() == "function_definition" {
            self.check_function_parameter_validation(node, source, violations);
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }

    fn check_function_parameter_validation(
        &self,
        function_node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Get function parameters (handle nested declarators for pointer-returning functions)
        let params = match self.extract_function_parameters(function_node, source) {
            Some(p) => p,
            None => return, // No parameters
        };

        // Check if this is a debug/logging function (has both file AND line parameters)
        let has_debug_params = params
            .iter()
            .any(|(name, _)| matches!(name.to_lowercase().as_str(), "file" | "filename"))
            && params
                .iter()
                .any(|(name, _)| matches!(name.to_lowercase().as_str(), "line" | "lineno"));

        // Check if this is a qsort-style comparator function
        // Pattern: int func(const void* a, const void* b, ...)
        let is_comparator = params.len() >= 2
            && params.iter().take(2).all(|(_, param_type)| {
                param_type.contains("const void *") || param_type.contains("const void*")
            });

        if is_comparator {
            return; // Skip validation for qsort-style comparators
        }

        // Filter for pointer parameters, excluding debug parameters only if this is a debug function
        let pointer_params: Vec<String> = params
            .iter()
            .filter(|(name, param_type)| {
                is_pointer_type(param_type) && !(has_debug_params && self.is_debug_parameter(name))
            })
            .map(|(name, _)| name.clone())
            .collect();

        if pointer_params.is_empty() {
            return; // No pointer parameters to check
        }

        // Get function body
        let body = match function_node.child_by_field_name("body") {
            Some(b) => b,
            None => return, // No body (declaration only)
        };

        // Find validated parameters (those that appear in validation checks)
        let validated_params = self.find_validated_parameters(&body, &pointer_params, source);

        // Check which pointer parameters are used without validation
        for param_name in &pointer_params {
            if !validated_params.contains(param_name) {
                // Check if the parameter is actually used in the function
                if self.is_parameter_used(&body, param_name, source) {
                    self.report_violation(function_node, param_name, source, violations);
                }
            }
        }
    }

    /// Find parameters that are validated (checked for NULL) before use
    fn find_validated_parameters(
        &self,
        body: &Node,
        pointer_params: &[String],
        source: &str,
    ) -> HashSet<String> {
        let mut validated = HashSet::new();

        // Look for validation patterns at the start of the function
        self.check_validation_patterns(body, pointer_params, source, &mut validated);

        validated
    }

    /// Check for common validation patterns like:
    /// - if (!ptr) return;
    /// - if (ptr == NULL) return;
    /// - if (!ptr || !ptr2) return;
    /// - assert(ptr != NULL);
    fn check_validation_patterns(
        &self,
        node: &Node,
        pointer_params: &[String],
        source: &str,
        validated: &mut HashSet<String>,
    ) {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "if_statement" => {
                        // Check if this is a validation pattern
                        if let Some(condition) = child.child_by_field_name("condition") {
                            let validated_in_condition = self
                                .extract_validated_params_from_condition(
                                    &condition,
                                    pointer_params,
                                    source,
                                );

                            // Check if the if body is an early return/error handling
                            if self.is_early_return_pattern(&child, source) {
                                for param in validated_in_condition {
                                    validated.insert(param);
                                }
                            }
                        }
                    }
                    "expression_statement" => {
                        // Check for assert() or similar validation macros
                        let stmt_text = get_node_text(&child, source);
                        if stmt_text.contains("assert") || stmt_text.contains("ASSERT") {
                            for param in pointer_params {
                                if stmt_text.contains(param) && stmt_text.contains("NULL") {
                                    validated.insert(param.clone());
                                }
                            }
                        }
                    }
                    _ => {}
                }
            }
        }
    }

    /// Extract parameter names that are being validated in a condition
    fn extract_validated_params_from_condition(
        &self,
        condition: &Node,
        pointer_params: &[String],
        source: &str,
    ) -> Vec<String> {
        let mut validated_params = Vec::new();
        let condition_text = get_node_text(condition, source);

        for param in pointer_params {
            // Check for common validation patterns:
            // !param, param == NULL, param != NULL (when combined with early return)
            // NULL == param, NULL != param
            // Also handle patterns with || separators

            // Create patterns to match
            let patterns = vec![
                format!("!{}", param),        // !ptr
                format!("! {}", param),       // ! ptr (with space)
                format!("{} == NULL", param), // ptr == NULL
                format!("NULL == {}", param), // NULL == ptr
                format!("{} == 0", param),    // ptr == 0
                format!("0 == {}", param),    // 0 == ptr
                format!("{}==NULL", param),   // ptr==NULL (no spaces)
                format!("NULL=={}", param),   // NULL==ptr
                format!("{} != NULL", param), // ptr != NULL (positive check)
                format!("NULL != {}", param), // NULL != ptr
                format!("{}!=NULL", param),   // ptr!=NULL
                format!("NULL!={}", param),   // NULL!=ptr
            ];

            let mut is_validated = false;
            for pattern in &patterns {
                if condition_text.contains(pattern) {
                    is_validated = true;
                    break;
                }
            }

            // Also check for parameter appearing in logical expressions
            // This handles: if (!a || !b || !c)
            if !is_validated {
                // Check if param appears with ! before it (possibly after ||)
                let search_patterns = vec![
                    format!("||!{}", param),  // ||!ptr
                    format!("|| !{}", param), // || !ptr
                    format!("(!{}", param),   // (!ptr
                    format!("( !{}", param),  // ( !ptr
                    format!("!{}||", param),  // !ptr||
                    format!("!{} ||", param), // !ptr ||
                    format!("!{})", param),   // !ptr)
                    format!("!{} )", param),  // !ptr )
                ];

                for pattern in &search_patterns {
                    if condition_text.contains(pattern) {
                        is_validated = true;
                        break;
                    }
                }
            }

            // Check for positive validation (parameter being checked for truthiness)
            // e.g., if (file && !ferror(file))
            if !is_validated {
                let positive_patterns = vec![
                    format!("{} &&", param),  // ptr &&
                    format!("({}&&", param),  // (ptr&&
                    format!("({} &&", param), // (ptr &&
                    format!("&& {}", param),  // && ptr
                    format!("&&{}", param),   // &&ptr
                ];

                for pattern in &positive_patterns {
                    if condition_text.contains(pattern) {
                        is_validated = true;
                        break;
                    }
                }
            }

            if is_validated {
                validated_params.push(param.clone());
            }
        }

        validated_params
    }

    /// Check if an if statement represents an early return/error pattern
    fn is_early_return_pattern(&self, if_node: &Node, source: &str) -> bool {
        // Get the consequence (then branch)
        if let Some(consequence) = if_node.child_by_field_name("consequence") {
            return self.contains_return_or_error(&consequence, source);
        }
        false
    }

    /// Check if a node contains a return statement or error handling
    fn contains_return_or_error(&self, node: &Node, source: &str) -> bool {
        match node.kind() {
            "return_statement" => true,
            "compound_statement" => {
                // Check ALL statements for return or noreturn function calls
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        if child.kind() == "return_statement" {
                            return true;
                        }
                        // Check for noreturn functions (longjmp, exit, abort)
                        if child.kind() == "expression_statement" {
                            if self.is_noreturn_call(&child, source) {
                                return true;
                            }
                        }
                    }
                }
                false
            }
            _ => false,
        }
    }

    /// Check if an expression statement contains a noreturn function call
    fn is_noreturn_call(&self, expr_stmt: &Node, source: &str) -> bool {
        for i in 0..expr_stmt.child_count() {
            if let Some(child) = expr_stmt.child(i) {
                if child.kind() == "call_expression" {
                    if let Some(func) = child.child_by_field_name("function") {
                        let func_name = get_node_text(&func, source);
                        // Check for common noreturn functions
                        if matches!(
                            func_name,
                            "longjmp" | "exit" | "abort" | "_Exit" | "quick_exit" | "thrd_exit"
                        ) {
                            return true;
                        }
                    }
                }
            }
        }
        false
    }

    /// Check if a parameter is actually used in the function body
    fn is_parameter_used(&self, body: &Node, param_name: &str, source: &str) -> bool {
        self.check_parameter_usage(body, param_name, source)
    }

    fn check_parameter_usage(&self, node: &Node, param_name: &str, source: &str) -> bool {
        // Check if this node is an identifier matching the parameter name
        if node.kind() == "identifier" {
            let text = get_node_text(node, source);
            if text == param_name {
                // Check if it's actually being used (not just in a validation check)
                if let Some(parent) = node.parent() {
                    // Skip if this is part of a validation check condition
                    if !self.is_in_validation_context(node) {
                        return true;
                    }
                    // Still count dereference as usage even in validation context
                    if parent.kind() == "pointer_expression"
                        || parent.kind() == "field_expression"
                        || parent.kind() == "subscript_expression"
                    {
                        return true;
                    }
                }
            }
        }

        // Recursively check children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if self.check_parameter_usage(&child, param_name, source) {
                    return true;
                }
            }
        }

        false
    }

    /// Check if a node is part of a validation context (if condition checking for NULL)
    fn is_in_validation_context(&self, node: &Node) -> bool {
        let mut current = node.parent();
        let mut depth = 0;

        while let Some(parent) = current {
            depth += 1;
            if depth > 10 {
                break; // Avoid infinite loops
            }

            // If we're in a parenthesized expression within an if condition
            if parent.kind() == "if_statement" {
                return true;
            }

            // Check for binary expressions that are comparisons to NULL
            if parent.kind() == "binary_expression" {
                return true;
            }

            // Check for unary not operator
            if parent.kind() == "unary_expression" {
                return true;
            }

            current = parent.parent();
        }

        false
    }

    fn report_violation(
        &self,
        function_node: &Node,
        param_name: &str,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Get function name
        let func_name = self.get_function_name(function_node, source);

        violations.push(RuleViolation {
            rule_id: self.rule_id().to_string(),
            severity: Severity::Medium,
            message: format!(
                "Function '{}' does not validate pointer parameter '{}' before use",
                func_name, param_name
            ),
            file_path: String::new(),
            line: function_node.start_position().row + 1,
            column: function_node.start_position().column + 1,
            suggestion: Some(format!(
                "Add validation check for '{}' at the start of the function, e.g., 'if (!{}) {{ return error_code; }}'",
                param_name, param_name
            )),
            ..Default::default()
        });
    }

    fn get_function_name(&self, function_node: &Node, source: &str) -> String {
        // Find the function declarator
        for i in 0..function_node.child_count() {
            if let Some(child) = function_node.child(i) {
                if child.kind() == "function_declarator" {
                    // Get the identifier from the declarator
                    for j in 0..child.child_count() {
                        if let Some(declarator_child) = child.child(j) {
                            if declarator_child.kind() == "identifier" {
                                return get_node_text(&declarator_child, source).to_string();
                            }
                            // Handle pointer declarators like void (*func)(...)
                            if declarator_child.kind() == "parenthesized_declarator" {
                                if let Some(inner) = self.find_identifier(&declarator_child, source)
                                {
                                    return inner;
                                }
                            }
                        }
                    }
                } else if child.kind() == "pointer_declarator" {
                    // Handle functions returning pointers
                    if let Some(name) = self.find_identifier(&child, source) {
                        return name;
                    }
                }
            }
        }
        "unknown".to_string()
    }

    fn find_identifier(&self, node: &Node, source: &str) -> Option<String> {
        if node.kind() == "identifier" {
            return Some(get_node_text(node, source).to_string());
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if let Some(id) = self.find_identifier(&child, source) {
                    return Some(id);
                }
            }
        }

        None
    }

    /// Extract function parameters, handling nested declarators for pointer-returning functions
    fn extract_function_parameters(
        &self,
        function_node: &Node,
        source: &str,
    ) -> Option<Vec<(String, String)>> {
        // First try the standard utility
        if let Some(params) = get_function_parameters(function_node, source) {
            return Some(params);
        }

        // Handle pointer-returning functions (e.g., URL *parse_url(...))
        // The structure is: function_definition > pointer_declarator > function_declarator
        for i in 0..function_node.child_count() {
            if let Some(child) = function_node.child(i) {
                if child.kind() == "pointer_declarator" {
                    // Look for function_declarator inside
                    if let Some(params) = self.find_params_in_declarator(&child, source) {
                        return Some(params);
                    }
                }
            }
        }

        None
    }

    fn find_params_in_declarator(
        &self,
        node: &Node,
        source: &str,
    ) -> Option<Vec<(String, String)>> {
        if node.kind() == "function_declarator" {
            // Found it, extract parameters
            return self.extract_params_from_declarator(node, source);
        }

        // Recurse into children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if let Some(params) = self.find_params_in_declarator(&child, source) {
                    return Some(params);
                }
            }
        }

        None
    }

    fn extract_params_from_declarator(
        &self,
        declarator_node: &Node,
        source: &str,
    ) -> Option<Vec<(String, String)>> {
        let mut parameters = Vec::new();

        // Find parameter_list node
        for i in 0..declarator_node.child_count() {
            if let Some(child) = declarator_node.child(i) {
                if child.kind() == "parameter_list" {
                    // Extract each parameter
                    for j in 0..child.child_count() {
                        if let Some(param) = child.child(j) {
                            if param.kind() == "parameter_declaration" {
                                let param_text = get_node_text(&param, source);
                                if let Some(name) = self.extract_param_name(&param, source) {
                                    parameters.push((name, param_text.to_string()));
                                }
                            }
                        }
                    }
                }
            }
        }

        if parameters.is_empty() {
            None
        } else {
            Some(parameters)
        }
    }

    fn extract_param_name(&self, param_node: &Node, source: &str) -> Option<String> {
        // Look for identifier or declarator pattern
        for i in 0..param_node.child_count() {
            if let Some(child) = param_node.child(i) {
                if child.kind() == "identifier" {
                    return Some(get_node_text(&child, source).to_string());
                } else if matches!(
                    child.kind(),
                    "array_declarator" | "pointer_declarator" | "function_declarator"
                ) {
                    // Recursively find identifier in declarator
                    if let Some(id) = self.find_identifier(&child, source) {
                        return Some(id);
                    }
                }
            }
        }
        None
    }

    /// Check if a parameter is a debug/logging parameter (e.g., __FILE__, __func__)
    /// These are commonly passed without validation
    fn is_debug_parameter(&self, param_name: &str) -> bool {
        matches!(
            param_name.to_lowercase().as_str(),
            "file" | "filename" | "func" | "function" | "function_name" | "line" | "lineno"
        )
    }
}

//! API00-C: Functions should validate their parameters
//!
//! This rule detects functions that use parameters without validating them first.
//! This includes:
//! - Pointer parameters used without NULL checks
//! - Integer parameters used in arithmetic without overflow checks
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
use crate::analyze::context::ProjectContext;
use crate::analyze::function_summary::FunctionSummary;
use crate::analyze::null_state::NullState;
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::{get_function_parameters, get_node_text, is_pointer_type};
use std::cell::RefCell;
use std::collections::{HashMap, HashSet};
use tree_sitter::Node;

pub struct Api00C {
    function_summaries: RefCell<HashMap<String, FunctionSummary>>,
}

impl Api00C {
    pub fn new() -> Self {
        Self {
            function_summaries: RefCell::new(HashMap::new()),
        }
    }
}

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

    fn set_project_context(&self, context: &ProjectContext) {
        *self.function_summaries.borrow_mut() = context.function_summaries.clone();
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
        // Skip static functions — API00-C is about public API contracts
        if Self::is_static_function(function_node, source) {
            return;
        }

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

        // Filter for integer parameters that could overflow
        let integer_params: Vec<String> = params
            .iter()
            .filter(|(_, param_type)| self.is_integer_type(param_type))
            .map(|(name, _)| name.clone())
            .collect();

        // Get function body
        let body = match function_node.child_by_field_name("body") {
            Some(b) => b,
            None => return, // No body (declaration only)
        };

        // Check pointer parameters
        if !pointer_params.is_empty() {
            // Find validated parameters (those that appear in validation checks)
            let validated_params = self.find_validated_parameters(&body, &pointer_params, source);

            // Look up callsite null states from prescan (if available)
            let func_name = self.get_function_name(function_node, source);
            let summaries = self.function_summaries.borrow();
            let summary = summaries.get(&func_name);

            // Build param name → index mapping
            let param_indices: HashMap<&str, usize> = params
                .iter()
                .enumerate()
                .map(|(i, (name, _))| (name.as_str(), i))
                .collect();

            // Check which pointer parameters are used without validation
            for param_name in &pointer_params {
                if !validated_params.contains(param_name) {
                    // Suppress if all callers pass NotNull for this parameter
                    if let (Some(s), Some(&idx)) = (summary, param_indices.get(param_name.as_str()))
                    {
                        if let Some(&state) = s.callsite_param_null_states.get(&idx) {
                            if state == NullState::NotNull {
                                continue; // All callers pass non-null → skip
                            }
                        }
                    }

                    // Check if the parameter is actually used in the function
                    if self.is_parameter_used(&body, param_name, source) {
                        self.report_violation(
                            function_node,
                            param_name,
                            "pointer",
                            source,
                            violations,
                        );
                    }
                }
            }
        }

        // Check integer parameters for overflow validation
        if !integer_params.is_empty() {
            self.check_integer_overflow_validation(
                function_node,
                &body,
                &integer_params,
                source,
                violations,
            );
        }
    }

    /// Check if integer parameters are validated for overflow before arithmetic operations
    fn check_integer_overflow_validation(
        &self,
        function_node: &Node,
        body: &Node,
        integer_params: &[String],
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Look for arithmetic operations using integer parameters without overflow checks
        for param_name in integer_params {
            if self.has_unchecked_arithmetic(body, param_name, source) {
                self.report_violation(function_node, param_name, "integer", source, violations);
            }
        }
    }

    /// Check if an integer parameter is used in arithmetic without overflow validation
    fn has_unchecked_arithmetic(&self, body: &Node, param_name: &str, source: &str) -> bool {
        // Check if there's overflow validation before arithmetic use
        let body_text = get_node_text(body, source);

        // Remove comments from body text to avoid false positives
        let body_no_comments = self.remove_comments(&body_text);

        // Look for arithmetic operators with the parameter
        let arithmetic_patterns = [
            format!("{} +", param_name),
            format!("{} -", param_name),
            format!("{} *", param_name),
            format!("{}+", param_name),
            format!("{}-", param_name),
            format!("{}*", param_name),
            format!("+ {}", param_name),
            format!("- {}", param_name),
            format!("* {}", param_name),
            format!("+{}", param_name),
            format!("-{}", param_name),
            format!("*{}", param_name),
            format!("{} <<", param_name),
            format!("{}<<", param_name),
            format!("<< {}", param_name),
            format!("<<{}", param_name),
        ];

        let has_arithmetic = arithmetic_patterns
            .iter()
            .any(|p| body_no_comments.contains(p));

        if !has_arithmetic {
            return false;
        }

        // Check for overflow validation patterns
        let overflow_check_patterns = [
            // Check for INT_MAX/INT_MIN comparisons
            format!("{} > INT_MAX", param_name),
            format!("{} < INT_MIN", param_name),
            format!("{} >= INT_MAX", param_name),
            format!("{} <= INT_MIN", param_name),
            // Check for SIZE_MAX comparisons
            format!("{} > SIZE_MAX", param_name),
            format!("{} >= SIZE_MAX", param_name),
            format!("SIZE_MAX - {}", param_name),
            format!("SIZE_MAX -{}", param_name),
            // Check for UINT_MAX comparisons
            format!("{} > UINT_MAX", param_name),
            format!("{} >= UINT_MAX", param_name),
            // Check for division overflow check (result / divisor != dividend)
            "will overflow".to_string(),
            "overflow check".to_string(),
            // Wrapped arithmetic check
            "__builtin_add_overflow".to_string(),
            "__builtin_sub_overflow".to_string(),
            "__builtin_mul_overflow".to_string(),
        ];

        let has_overflow_check = overflow_check_patterns
            .iter()
            .any(|p| body_text.contains(p));

        // Also check for basic parameter validation (if param == 0, if param < X, etc.)
        let basic_validation_patterns = [
            format!("if ({} == 0)", param_name),
            format!("if ({}==0)", param_name),
            format!("if ({} == 0", param_name),
            format!("if (0 == {})", param_name),
            format!("if (!{})", param_name),
            format!("if ({} < ", param_name),
            format!("if ({} > ", param_name),
            format!("if ({} <= ", param_name),
            format!("if ({} >= ", param_name),
            // Handle || patterns
            format!("|| {} == 0", param_name),
            format!("||{} == 0", param_name),
            format!("{} == 0 ||", param_name),
            format!("{} == 0||", param_name),
            format!("|| {} > ", param_name),
            format!("|| {} < ", param_name),
            format!("{} > ", param_name),
            format!("{} < ", param_name),
        ];

        let has_basic_validation = basic_validation_patterns
            .iter()
            .any(|p| body_text.contains(p));

        // Return true if there's arithmetic but no overflow check AND no basic validation
        !has_overflow_check && !has_basic_validation
    }

    /// Remove C-style comments from text to avoid false positives
    fn remove_comments(&self, text: &str) -> String {
        let mut result = String::with_capacity(text.len());
        let chars: Vec<char> = text.chars().collect();
        let mut i = 0;

        while i < chars.len() {
            if i + 1 < chars.len() && chars[i] == '/' && chars[i + 1] == '*' {
                // Skip block comment
                i += 2;
                while i + 1 < chars.len() && !(chars[i] == '*' && chars[i + 1] == '/') {
                    i += 1;
                }
                i += 2; // Skip closing */
            } else if i + 1 < chars.len() && chars[i] == '/' && chars[i + 1] == '/' {
                // Skip line comment
                while i < chars.len() && chars[i] != '\n' {
                    i += 1;
                }
            } else {
                result.push(chars[i]);
                i += 1;
            }
        }

        result
    }

    /// Check if a type is an integer type (not floating point)
    fn is_integer_type(&self, type_str: &str) -> bool {
        let normalized = type_str.to_lowercase();
        // Exclude pointers
        if type_str.contains('*') {
            return false;
        }
        // Exclude floating point types
        if normalized.contains("float") || normalized.contains("double") {
            return false;
        }
        // Check for integer types
        normalized.contains("int")
            || (normalized.contains("long") && !normalized.contains("double"))
            || normalized.contains("short")
            || normalized.contains("size_t")
            || (normalized.contains("unsigned")
                && !normalized.contains("double")
                && !normalized.contains("float"))
            || (normalized.contains("signed")
                && !normalized.contains("double")
                && !normalized.contains("float"))
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

                            // Case 1: early-return / early-exit pattern
                            //   if (!ptr)        { return; }
                            //   if (ptr == NULL) { return; }
                            if self.is_early_return_pattern(&child, source) {
                                for param in validated_in_condition {
                                    validated.insert(param);
                                }
                            } else {
                                // Case 2: positive guard pattern
                                //   if (ptr != NULL) { /* all usage inside */ }
                                // The parameter is only accessed inside the guarded block,
                                // so it is safely validated even without an early return.
                                let condition_text = get_node_text(&condition, source);
                                for param in validated_in_condition {
                                    if self.is_positive_null_guard(&condition_text, &param) {
                                        validated.insert(param);
                                    }
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
                // Skip (void)param / UNUSED(param) patterns — these explicitly mark
                // a parameter as intentionally unused (e.g., callback signature match)
                if self.is_in_void_cast(node, source) {
                    return false;
                }
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

    /// Check if an identifier is inside a (void)param or UNUSED(param) cast.
    /// These patterns explicitly suppress unused-parameter warnings and indicate
    /// the parameter is intentionally not used.
    fn is_in_void_cast(&self, node: &Node, source: &str) -> bool {
        let mut current = node.parent();
        let mut depth = 0;

        while let Some(parent) = current {
            depth += 1;
            if depth > 5 {
                break;
            }

            if parent.kind() == "cast_expression" {
                // Check if the cast target type is "void"
                for i in 0..parent.child_count() {
                    if let Some(child) = parent.child(i) {
                        if child.kind() == "type_descriptor" {
                            let type_text = get_node_text(&child, source);
                            if type_text.trim() == "void" {
                                return true;
                            }
                        }
                    }
                }
            }

            // Handle UNUSED(param) macro — parsed as call_expression before preprocessing
            if parent.kind() == "call_expression" {
                if let Some(func) = parent.child_by_field_name("function") {
                    let func_name = get_node_text(&func, source);
                    if matches!(
                        func_name,
                        "UNUSED"
                            | "UNREFERENCED_PARAMETER"
                            | "UNUSED_PARAM"
                            | "UNUSED_PARAMETER"
                            | "Q_UNUSED"
                    ) {
                        return true;
                    }
                }
            }

            // Stop at expression_statement boundary
            if parent.kind() == "expression_statement" {
                break;
            }

            current = parent.parent();
        }

        false
    }

    /// Check if a function_definition has `static` storage class or a STATIC macro prefix.
    fn is_static_function(function_node: &Node, source: &str) -> bool {
        for i in 0..function_node.child_count() {
            if let Some(child) = function_node.child(i) {
                if child.kind() == "storage_class_specifier" {
                    if let Ok(text) = child.utf8_text(source.as_bytes()) {
                        if text == "static" {
                            return true;
                        }
                    }
                }
            }
        }
        // Check for STATIC macro prefix (tree-sitter sees unexpanded macro as first tokens)
        let func_text = function_node.utf8_text(source.as_bytes()).unwrap_or("");
        let first_token = func_text.split_whitespace().next().unwrap_or("");
        matches!(
            first_token,
            "STATIC" | "STATIC_FUNC" | "STATIC_INLINE" | "STATIC_NOINLINE"
        )
    }

    fn report_violation(
        &self,
        function_node: &Node,
        param_name: &str,
        param_type: &str,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Get function name
        let func_name = self.get_function_name(function_node, source);

        let (message, suggestion) = if param_type == "pointer" {
            (
                format!(
                    "Function '{}' does not validate pointer parameter '{}' before use",
                    func_name, param_name
                ),
                format!(
                    "Add validation check for '{}' at the start of the function, e.g., 'if (!{}) {{ return error_code; }}'",
                    param_name, param_name
                ),
            )
        } else {
            (
                format!(
                    "Function '{}' does not validate integer parameter '{}' for overflow before arithmetic operations",
                    func_name, param_name
                ),
                format!(
                    "Add overflow validation for '{}' before arithmetic, e.g., check against INT_MAX/INT_MIN or use __builtin_*_overflow()",
                    param_name
                ),
            )
        };

        violations.push(RuleViolation {
            rule_id: self.rule_id().to_string(),
            severity: Severity::Medium,
            message,
            file_path: String::new(),
            line: function_node.start_position().row + 1,
            column: function_node.start_position().column + 1,
            suggestion: Some(suggestion),
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

    /// Returns true if the condition text is a positive NULL guard for `param`,
    /// i.e. the if-block is only entered when the pointer is non-NULL.
    /// Examples: `ptr != NULL`, `NULL != ptr`, `ptr != 0`, `ptr` (bare truthiness)
    fn is_positive_null_guard(&self, condition_text: &str, param: &str) -> bool {
        let patterns: &[&str] = &["!= NULL", "!=NULL", "!= 0", "!=0"];
        for suffix in patterns {
            if condition_text.contains(&format!("{} {}", param, suffix))
                || condition_text.contains(&format!("{}{}", param, suffix))
            {
                return true;
            }
        }
        // NULL/0 on the left: NULL != ptr, 0 != ptr
        let prefixes: &[&str] = &["NULL !=", "NULL!=", "0 !=", "0!="];
        for prefix in prefixes {
            if condition_text.contains(&format!("{} {}", prefix, param))
                || condition_text.contains(&format!("{}{}", prefix, param))
            {
                return true;
            }
        }
        // Bare truthiness check: if (ptr) or if (ptr && ...)
        // but NOT if (!ptr) which is the early-return form already handled above
        let bare_patterns: &[&str] = &[
            &format!("({})", param),
            &format!("({} ", param),
            &format!("({} &&", param),
            &format!("({}&& ", param),
        ];
        for p in bare_patterns {
            if condition_text.contains(*p) {
                return true;
            }
        }
        false
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

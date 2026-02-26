//! MEM10-C: Define and use a pointer validation function
//!
//! Dereferencing invalid pointers leads to undefined behavior. Functions that accept
//! pointer arguments should validate them using a dedicated validation function rather
//! than performing ad-hoc NULL checks. While NULL checking is necessary, it's insufficient
//! to catch all invalid pointers. A centralized validation function provides:
//! 1. Consistent validation logic across the codebase
//! 2. A single point for platform-specific validation enhancements
//! 3. Better maintainability and testability
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! void incr(int *intptr) {
//!     if (intptr == NULL) {  // Direct NULL check
//!         /* Handle error */
//!     }
//!     (*intptr)++;
//! }
//! ```
//!
//! **Compliant:**
//! ```c
//! int valid(void *ptr) {
//!     return (ptr != NULL);  // Centralized validation
//! }
//!
//! void incr(int *intptr) {
//!     if (!valid(intptr)) {  // Use validation function
//!         /* Handle error */
//!     }
//!     (*intptr)++;
//! }
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Mem10C;

impl CertRule for Mem10C {
    fn rule_id(&self) -> &'static str {
        "MEM10-C"
    }

    fn description(&self) -> &'static str {
        "Define and use a pointer validation function"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "MEM10-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_pointer_validation(node, source, &mut violations);
        violations
    }
}

impl Mem10C {
    /// Collect the pointer parameter names of the enclosing function definition.
    /// Returns an empty set if we can't find a function definition ancestor.
    fn collect_enclosing_params<'a>(
        &self,
        node: &Node<'a>,
        source: &str,
    ) -> std::collections::HashSet<String> {
        let mut current = node.parent();
        while let Some(p) = current {
            if p.kind() == "function_definition" {
                return self.extract_pointer_param_names(&p, source);
            }
            current = p.parent();
        }
        std::collections::HashSet::new()
    }

    fn extract_pointer_param_names(
        &self,
        func_node: &Node,
        source: &str,
    ) -> std::collections::HashSet<String> {
        let mut names = std::collections::HashSet::new();
        for i in 0..func_node.child_count() {
            if let Some(child) = func_node.child(i) {
                self.collect_params_from_declarator(&child, source, &mut names);
            }
        }
        names
    }

    fn collect_params_from_declarator(
        &self,
        node: &Node,
        source: &str,
        names: &mut std::collections::HashSet<String>,
    ) {
        if node.kind() == "function_declarator" {
            if let Some(params) = node.child_by_field_name("parameters") {
                for i in 0..params.child_count() {
                    if let Some(param) = params.child(i) {
                        if param.kind() == "parameter_declaration" {
                            if let Some(decl) = param.child_by_field_name("declarator") {
                                let text = get_node_text(&decl, source);
                                // Extract the identifier from declarators like *data, data[], etc.
                                if decl.kind() == "pointer_declarator"
                                    || decl.kind() == "array_declarator"
                                {
                                    if let Some(id) = find_identifier_in_node(&decl, source) {
                                        names.insert(id);
                                    }
                                } else if decl.kind() == "identifier" {
                                    names.insert(text.to_string());
                                }
                            }
                        }
                    }
                }
            }
        } else {
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    self.collect_params_from_declarator(&child, source, names);
                }
            }
        }
    }

    fn check_pointer_validation(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Look for if statements
        if node.kind() == "if_statement" {
            if let Some(condition) = node.child_by_field_name("condition") {
                // Check if this is a direct NULL comparison
                if self.is_direct_null_check(&condition, source) {
                    // Only flag when the checked pointer is a function parameter.
                    // Inline null checks on locally-declared variables are acceptable
                    // practice; the "use a validation function" advice is primarily
                    // relevant when validating inputs at function boundaries.
                    let checked_var = extract_checked_var_name(&condition, source);
                    let params = self.collect_enclosing_params(node, source);
                    if checked_var.as_deref().is_some_and(|v| params.contains(v)) {
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            message: "Direct NULL check for pointer validation. \
                                     Define and use a dedicated pointer validation function \
                                     instead of ad-hoc NULL checks. This centralizes validation \
                                     logic and allows platform-specific enhancements."
                                .to_string(),
                            severity: self.severity(),
                            line: condition.start_position().row + 1,
                            column: condition.start_position().column + 1,
                            file_path: String::new(),
                            suggestion: Some(
                                "Create a validation function like 'int valid(void *ptr)' \
                                 and use 'if (!valid(ptr))' instead of 'if (ptr == NULL)'"
                                    .to_string(),
                            ),
                            requires_manual_review: Some(true),
                        });
                    }
                }
            }
        }

        // Recurse through children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_pointer_validation(&child, source, violations);
            }
        }
    }

    /// Check if a condition is a direct NULL comparison (e.g., ptr == NULL, ptr != NULL, !ptr)
    fn is_direct_null_check(&self, condition: &Node, source: &str) -> bool {
        let condition_text = get_node_text(condition, source);

        // Check for explicit NULL pointer comparisons only.
        // We intentionally exclude "== 0" and "!= 0" because these are very common
        // for checking integer return values (e.g., fclose() == 0, system() != 0)
        // and generate massive false positives in non-pointer contexts.
        if condition_text.contains("== NULL") || condition_text.contains("!= NULL") {
            // Make sure it's not a function call (which would be a validation function)
            // If it contains a function call, it's likely using a validation function
            if !self.appears_to_be_validation_function_call(condition, source) {
                return true;
            }
        }

        // Check for unary not operator on a pointer
        if condition.kind() == "unary_expression" {
            if let Some(operator) = condition.child_by_field_name("operator") {
                let op_text = get_node_text(&operator, source);
                if op_text == "!" {
                    if let Some(argument) = condition.child_by_field_name("argument") {
                        // If the argument is just an identifier (not a function call), it's a direct check
                        if argument.kind() == "identifier" {
                            return true;
                        }
                    }
                }
            }
        }

        false
    }

    /// Check if the condition appears to call a validation function
    fn appears_to_be_validation_function_call(&self, condition: &Node, source: &str) -> bool {
        // Look for call_expression nodes
        self.contains_call_expression(condition, source)
    }

    fn contains_call_expression(&self, node: &Node, _source: &str) -> bool {
        if node.kind() == "call_expression" {
            return true;
        }

        // Recurse through children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if self.contains_call_expression(&child, _source) {
                    return true;
                }
            }
        }

        false
    }
}

/// Extract the variable name being null-checked from a condition node.
/// Handles: `ptr == NULL`, `NULL == ptr`, `ptr != NULL`, `NULL != ptr`, `!ptr`
fn extract_checked_var_name(condition: &Node, source: &str) -> Option<String> {
    // Binary expression: ptr == NULL or ptr != NULL (or reversed)
    if condition.kind() == "binary_expression" {
        let left = condition.child_by_field_name("left")?;
        let right = condition.child_by_field_name("right")?;
        let left_text = get_node_text(&left, source);
        let right_text = get_node_text(&right, source);
        if right_text == "NULL" && left.kind() == "identifier" {
            return Some(left_text.to_string());
        }
        if left_text == "NULL" && right.kind() == "identifier" {
            return Some(right_text.to_string());
        }
        return None;
    }

    // Parenthesized expression: (ptr == NULL)
    if condition.kind() == "parenthesized_expression" {
        if let Some(inner) = condition.child(1) {
            return extract_checked_var_name(&inner, source);
        }
    }

    // Unary not: !ptr
    if condition.kind() == "unary_expression" {
        if let Some(op) = condition.child_by_field_name("operator") {
            if get_node_text(&op, source) == "!" {
                if let Some(arg) = condition.child_by_field_name("argument") {
                    if arg.kind() == "identifier" {
                        return Some(get_node_text(&arg, source).to_string());
                    }
                }
            }
        }
    }

    None
}

/// Recursively find the first identifier inside a declarator node.
fn find_identifier_in_node(node: &Node, source: &str) -> Option<String> {
    if node.kind() == "identifier" {
        return Some(get_node_text(node, source).to_string());
    }
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if let Some(id) = find_identifier_in_node(&child, source) {
                return Some(id);
            }
        }
    }
    None
}

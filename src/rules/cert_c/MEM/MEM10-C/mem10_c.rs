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

        // Check for common NULL check patterns
        if condition_text.contains("== NULL")
            || condition_text.contains("!= NULL")
            || condition_text.contains("== 0")
            || condition_text.contains("!= 0")
        {
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

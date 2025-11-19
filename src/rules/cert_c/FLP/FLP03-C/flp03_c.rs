//! FLP03-C: Detect and handle floating-point errors
//!
//! This rule addresses the detection and handling of errors occurring during
//! floating-point operations. Programmers often validate operands before operations
//! but neglect errors that occur during computation itself, which can result in
//! silent failures and unexpected arithmetic results.
//!
//! ## Floating-Point Errors to Detect:
//! - **Divide-by-zero**: Returns infinity rather than aborting
//! - **Inexact operations**: Loss of precision
//! - **Underflow**: Results too small to represent
//! - **Overflow**: Results too large for the data type
//! - **Invalid operations**: Conversions causing undefined values
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! void fpOper_noErrorChecking(void) {
//!     double a = 1e-40, b, c = 0.1;
//!     float x = 0, y;
//!     y = a;           // Inexact and underflows - no error check
//!     b = y / x;       // Divide-by-zero - no error check
//!     c = sin(30) * a; // Inexact - no error check
//! }
//! ```
//!
//! **Compliant (using fenv.h):**
//! ```c
//! #include <fenv.h>
//! #pragma STDC FENV_ACCESS ON
//!
//! void fpOper_fenv(void) {
//!     double a = 1e-40, b, c = 0.1;
//!     float x = 0, y;
//!     int fpeRaised;
//!
//!     feclearexcept(FE_ALL_EXCEPT);
//!     y = a;
//!     fpeRaised = fetestexcept(FE_ALL_EXCEPT);
//!
//!     feclearexcept(FE_ALL_EXCEPT);
//!     b = y / x;
//!     fpeRaised = fetestexcept(FE_ALL_EXCEPT);
//! }
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Flp03C;

impl Flp03C {
    pub fn new() -> Self {
        Self
    }

    /// List of floating-point error checking functions from fenv.h
    const FENV_FUNCTIONS: &'static [&'static str] = &[
        "feclearexcept",
        "fetestexcept",
        "fegetexceptflag",
        "fesetexceptflag",
        "feraiseexcept",
    ];

    /// List of Windows-specific floating-point error checking functions
    const WINDOWS_FP_FUNCTIONS: &'static [&'static str] =
        &["_clearfp", "_statusfp", "_controlfp", "_fpieee_flt"];

    /// Check if a function name is a floating-point error checking function
    fn is_fp_error_check_function(&self, name: &str) -> bool {
        Self::FENV_FUNCTIONS.contains(&name) || Self::WINDOWS_FP_FUNCTIONS.contains(&name)
    }

    /// Check if a node contains floating-point error checking calls
    fn contains_fp_error_checking(&self, node: &Node, source: &str) -> bool {
        // Check this node
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = get_node_text(&function, source);
                if self.is_fp_error_check_function(func_name) {
                    return true;
                }
            }
        }

        // Recursively check children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if self.contains_fp_error_checking(&child, source) {
                    return true;
                }
            }
        }

        false
    }

    /// Check if a binary expression involves floating-point types
    fn is_floating_point_expression(&self, node: &Node, source: &str) -> bool {
        let text = get_node_text(node, source);

        // Check for floating-point literals (contains decimal point or e notation)
        if text.contains('.') || text.contains('e') || text.contains('E') {
            return true;
        }

        // Check for floating-point type keywords
        if text.contains("float") || text.contains("double") {
            return true;
        }

        false
    }

    /// Check for floating-point division operations
    fn check_fp_division(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if node.kind() == "binary_expression" {
            // Check if this is a division operation
            let mut is_division = false;
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() == "/" {
                        is_division = true;
                        break;
                    }
                }
            }

            if is_division && self.is_floating_point_expression(node, source) {
                // Check if there's error checking in the containing function
                if let Some(containing_func) = self.find_containing_function(node) {
                    if !self.contains_fp_error_checking(&containing_func, source) {
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: self.severity(),
                            message: "Floating-point division without error checking (consider using feclearexcept/fetestexcept)".to_string(),
                            file_path: String::new(),
                            line: node.start_position().row + 1,
                            column: node.start_position().column + 1,
                            suggestion: Some(
                                "Use feclearexcept(FE_ALL_EXCEPT) before and fetestexcept(FE_ALL_EXCEPT) after floating-point operations".to_string()
                            ),
                            ..Default::default()
                        });
                    }
                }
            }
        }
    }

    /// Check for floating-point type conversions without error checking
    fn check_fp_conversion(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check for cast expressions involving floating-point types
        if node.kind() == "cast_expression" {
            if let Some(type_node) = node.child_by_field_name("type") {
                let type_text = get_node_text(&type_node, source);
                if type_text.contains("float") || type_text.contains("double") {
                    // Check if there's error checking in the containing function
                    if let Some(containing_func) = self.find_containing_function(node) {
                        if !self.contains_fp_error_checking(&containing_func, source) {
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                severity: self.severity(),
                                message: "Floating-point type conversion without error checking (may cause inexact conversion or overflow)".to_string(),
                                file_path: String::new(),
                                line: node.start_position().row + 1,
                                column: node.start_position().column + 1,
                                suggestion: Some(
                                    "Use feclearexcept/fetestexcept to detect FE_INEXACT, FE_OVERFLOW, or FE_UNDERFLOW".to_string()
                                ),
                                ..Default::default()
                            });
                        }
                    }
                }
            }
        }

        // Check for assignment to floating-point variables from other types
        if node.kind() == "assignment_expression" {
            if let (Some(left), Some(right)) = (
                node.child_by_field_name("left"),
                node.child_by_field_name("right"),
            ) {
                // Check if left side appears to be a float variable
                // This is a heuristic - ideally we'd track type information
                if self.is_floating_point_expression(&left, source)
                    || self.is_floating_point_expression(&right, source)
                {
                    // Check if there's error checking in the containing function
                    if let Some(containing_func) = self.find_containing_function(node) {
                        if !self.contains_fp_error_checking(&containing_func, source) {
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                severity: self.severity(),
                                message: format!(
                                    "Floating-point assignment without error checking (may underflow or lose precision)"
                                ),
                                file_path: String::new(),
                                line: node.start_position().row + 1,
                                column: node.start_position().column + 1,
                                suggestion: Some(
                                    "Use feclearexcept/fetestexcept to detect floating-point exceptions".to_string()
                                ),
                                ..Default::default()
                            });
                        }
                    }
                }
            }
        }
    }

    /// Find the containing function definition for a given node
    fn find_containing_function<'a>(&self, node: &Node<'a>) -> Option<Node<'a>> {
        let mut current = Some(*node);
        while let Some(n) = current {
            if n.kind() == "function_definition" {
                return Some(n);
            }
            current = n.parent();
        }
        None
    }
}

impl CertRule for Flp03C {
    fn rule_id(&self) -> &'static str {
        "FLP03-C"
    }

    fn description(&self) -> &'static str {
        "Detect and handle floating-point errors"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "FLP03-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Flp03C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check for floating-point operations without error checking
        match node.kind() {
            "binary_expression" => {
                self.check_fp_division(node, source, violations);
            }
            "cast_expression" | "assignment_expression" => {
                self.check_fp_conversion(node, source, violations);
            }
            _ => {}
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }
}

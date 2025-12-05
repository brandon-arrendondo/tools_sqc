//! FLP32-C: Prevent or detect domain and range errors in math functions
//!
//! Math functions can produce domain errors, range errors, or pole errors:
//! - Domain error: input argument outside the function's defined domain (e.g., sqrt(-1))
//! - Range error: result cannot be represented due to extreme magnitude (e.g., pow(10, 1e6))
//! - Pole error: function approaches infinity (e.g., log(0))
//!
//! ## Rationale:
//! - Domain and range errors can lead to undefined behavior or incorrect results
//! - Programs should check for these errors using errno or return value checks
//! - Critical for numerical stability and correctness
//!
//! ## Examples:
//!
//! **Non-compliant (no error checking):**
//! ```c
//! double result = sqrt(x);
//! // No check for domain error (x < 0) or NaN result
//! ```
//!
//! **Compliant (with error checking):**
//! ```c
//! errno = 0;
//! double result = sqrt(x);
//! if (errno != 0 || isnan(result)) {
//!     /* Handle error */
//! }
//! ```
//!
//! ## Detection Strategy:
//! - Find calls to math.h functions
//! - Check if errno is checked or cleared nearby
//! - Check if return value is validated (isnan, isinf, isfinite)
//! - Report if no error handling is present

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Flp32C;

impl CertRule for Flp32C {
    fn rule_id(&self) -> &'static str {
        "FLP32-C"
    }

    fn description(&self) -> &'static str {
        "Prevent or detect domain and range errors in math functions"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "FLP32-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Flp32C {
    /// List of math.h functions that can produce domain/range/pole errors
    const MATH_FUNCTIONS: &'static [&'static str] = &[
        "sqrt",
        "sqrtf",
        "sqrtl",
        "pow",
        "powf",
        "powl",
        "log",
        "logf",
        "logl",
        "log10",
        "log10f",
        "log10l",
        "log2",
        "log2f",
        "log2l",
        "exp",
        "expf",
        "expl",
        "exp2",
        "exp2f",
        "exp2l",
        "asin",
        "asinf",
        "asinl",
        "acos",
        "acosf",
        "acosl",
        "atan",
        "atanf",
        "atanl",
        "atan2",
        "atan2f",
        "atan2l",
        "sinh",
        "sinhf",
        "sinhl",
        "cosh",
        "coshf",
        "coshl",
        "tanh",
        "tanhf",
        "tanhl",
        "asinh",
        "asinhf",
        "asinhl",
        "acosh",
        "acoshf",
        "acoshl",
        "atanh",
        "atanhf",
        "atanhl",
        "hypot",
        "hypotf",
        "hypotl",
        "fmod",
        "fmodf",
        "fmodl",
        "remainder",
        "remainderf",
        "remainderl",
        "remquo",
        "remquof",
        "remquol",
    ];

    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Look for call expressions
        if node.kind() == "call_expression" {
            if let Some(func) = node.child_by_field_name("function") {
                let func_name = get_node_text(&func, source);

                // Check if it's a math function
                if Self::MATH_FUNCTIONS.contains(&func_name) {
                    // Check if there's error checking nearby
                    if !self.has_error_checking(node, source) {
                        let line = node.start_position().row + 1;

                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::Medium,
                            message: format!(
                                "Call to {}() without checking for domain/range errors",
                                func_name
                            ),
                            file_path: String::new(),
                            line,
                            column: 0,
                            suggestion: Some(
                                "Check errno or use isnan()/isinf() to detect math errors"
                                    .to_string(),
                            ),
                            ..Default::default()
                        });
                    }
                }
            }
        }

        // Recursively check children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }

    /// Check if there's error checking near the math function call
    fn has_error_checking(&self, call_node: &Node, source: &str) -> bool {
        // Get the containing scope (function or compound statement)
        let scope = self.get_containing_scope(call_node);
        let scope_node = match scope {
            Some(s) => s,
            None => return false,
        };

        // Check for errno usage or error checking functions in the same scope
        self.has_errno_check(&scope_node, source)
            || self.has_error_check_functions(&scope_node, source)
    }

    /// Get the containing function or compound statement
    fn get_containing_scope<'a>(&self, node: &Node<'a>) -> Option<Node<'a>> {
        let mut current = node.parent();

        while let Some(n) = current {
            if matches!(
                n.kind(),
                "compound_statement" | "function_definition" | "translation_unit"
            ) {
                return Some(n);
            }
            current = n.parent();
        }

        None
    }

    /// Check if errno is used in the scope
    fn has_errno_check(&self, scope: &Node, source: &str) -> bool {
        self.find_identifier_usage(scope, "errno", source)
    }

    /// Check if error checking functions are used (isnan, isinf, isfinite, fpclassify, etc.)
    fn has_error_check_functions(&self, scope: &Node, source: &str) -> bool {
        const ERROR_CHECK_FUNCS: &[&str] = &[
            "isnan",
            "isinf",
            "isfinite",
            "fpclassify",
            "isnormal",
            "signbit",
            "fetestexcept",
            "feclearexcept",
            // Domain checking functions
            "isless",
            "islessequal",
            "isgreater",
            "isgreaterequal",
            "islessgreater",
            "isunordered",
        ];

        for func in ERROR_CHECK_FUNCS {
            // Check both function calls and identifier usage (for macros)
            if self.find_function_call(scope, func, source)
                || self.find_identifier_usage(scope, func, source)
            {
                return true;
            }
        }

        false
    }

    /// Recursively search for identifier usage
    fn find_identifier_usage(&self, node: &Node, identifier: &str, source: &str) -> bool {
        if node.kind() == "identifier" {
            let text = get_node_text(node, source);
            if text == identifier {
                return true;
            }
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if self.find_identifier_usage(&child, identifier, source) {
                    return true;
                }
            }
        }

        false
    }

    /// Recursively search for function calls
    fn find_function_call(&self, node: &Node, func_name: &str, source: &str) -> bool {
        if node.kind() == "call_expression" {
            if let Some(func) = node.child_by_field_name("function") {
                let text = get_node_text(&func, source);
                if text == func_name {
                    return true;
                }
            }
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if self.find_function_call(&child, func_name, source) {
                    return true;
                }
            }
        }

        false
    }
}

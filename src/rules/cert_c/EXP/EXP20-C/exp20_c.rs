//! EXP20-C: Perform explicit tests to determine success, true and false, and equality
//!
//! Code should use explicit tests (e.g., `!= 0` instead of implicit truthiness)
//! and avoid testing for specific non-zero values when any non-zero indicates success.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! if (is_banned(usr) == 1) { ... }  // Should be != 0
//! if (!validateUser(usr)) { ... }    // Unclear what "success" means
//! ```
//!
//! **Compliant:**
//! ```c
//! if (is_banned(usr) != 0) { ... }  // Explicit non-zero test
//! if (validateUser(usr) == 0) { ... }  // Explicit success test
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Exp20C;

impl CertRule for Exp20C {
    fn rule_id(&self) -> &'static str {
        "EXP20-C"
    }

    fn description(&self) -> &'static str {
        "Perform explicit tests to determine success, true and false, and equality"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "EXP20-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.find_implicit_tests(node, source, &mut violations);
        violations
    }
}

impl Exp20C {
    /// Find implicit or misleading boolean tests
    fn find_implicit_tests(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check for == 1 comparisons with function calls
        if node.kind() == "binary_expression" {
            if let Some(operator) = node.child_by_field_name("operator") {
                let op_text = get_node_text(&operator, source);
                if op_text == "==" {
                    if let (Some(left), Some(right)) = (
                        node.child_by_field_name("left"),
                        node.child_by_field_name("right"),
                    ) {
                        // Check for func() == 1 pattern
                        let left_text = get_node_text(&left, source);
                        let right_text = get_node_text(&right, source);

                        if left.kind() == "call_expression" && right_text == "1" {
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                message: format!(
                                    "Testing '{}' for equality with 1. Functions may return values > 1. \
                                     Use '!= 0' for boolean-like tests.",
                                    left_text
                                ),
                                severity: self.severity(),
                                line: node.start_position().row + 1,
                                column: node.start_position().column + 1,
                                file_path: String::new(),
                                suggestion: Some(format!("Consider using: {} != 0", left_text)),
                                requires_manual_review: None,
                            });
                        }
                    }
                }
            }
        }

        // Check for unary ! on function calls (implicit boolean test)
        if node.kind() == "unary_expression" {
            if let Some(operator) = node.child_by_field_name("operator") {
                let op_text = get_node_text(&operator, source);
                if op_text == "!" {
                    if let Some(argument) = node.child_by_field_name("argument") {
                        if argument.kind() == "call_expression" {
                            let func_text = get_node_text(&argument, source);
                            // Only flag if it looks like a validation/checking function
                            if self.is_validation_function(&func_text) {
                                violations.push(RuleViolation {
                                    rule_id: self.rule_id().to_string(),
                                    message: format!(
                                        "Implicit boolean test on '{}'. May be unclear what constitutes success or failure.",
                                        func_text
                                    ),
                                    severity: self.severity(),
                                    line: node.start_position().row + 1,
                                    column: node.start_position().column + 1,
                                    file_path: String::new(),
                                    suggestion: Some(format!(
                                        "Use explicit test: {} == 0 or {} != 0 depending on success convention",
                                        func_text, func_text
                                    )),
                                    requires_manual_review: None,
                                });
                            }
                        }
                    }
                }
            }
        }

        // Recurse through children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.find_implicit_tests(&child, source, violations);
            }
        }
    }

    /// Check if function name suggests validation/checking or string comparison
    fn is_validation_function(&self, func_text: &str) -> bool {
        func_text.contains("validate")
            || func_text.contains("Validate")
            || func_text.contains("check")
            || func_text.contains("Check")
            || func_text.contains("verify")
            || func_text.contains("Verify")
            // String comparison functions - !strcmp() is an implicit boolean test
            || func_text.contains("strcmp")
            || func_text.contains("strncmp")
            || func_text.contains("memcmp")
            || func_text.contains("wcscmp")
            || func_text.contains("wcsncmp")
    }
}

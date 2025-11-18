//! FLP00-C: Understand the limitations of floating-point numbers
//!
//! Floating-point numbers have finite precision and are prone to rounding errors.
//! Different platforms (IEEE 754 vs IBM) and compiler optimization levels can
//! produce different results for the same floating-point operations. Direct
//! equality comparisons of floating-point values should be avoided.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! float c = a / b;
//! if (c == a / b) {  // Direct equality comparison
//!     // May fail unpredictably
//! }
//! ```
//!
//! **Compliant:**
//! ```c
//! float c = a / b;
//! float diff = fabsf(c - (a / b));
//! if (diff <= __FLT_EPSILON__) {  // Epsilon-based comparison
//!     // Robust comparison
//! }
//! ```
//!
//! **Compliant (using relative difference):**
//! ```c
//! float RelDif(float a, float b) {
//!   float c = fabsf(a);
//!   float d = fabsf(b);
//!   d = fmaxf(c, d);
//!   return d == 0.0f ? 0.0f : fabsf(a - b) / d;
//! }
//!
//! if (RelDif(c, a / b) <= __FLT_EPSILON__) {
//!     // Comparison succeeds
//! }
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Flp00C;

impl CertRule for Flp00C {
    fn rule_id(&self) -> &'static str {
        "FLP00-C"
    }

    fn description(&self) -> &'static str {
        "Understand the limitations of floating-point numbers"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "FLP00-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.find_fp_equality_comparisons(node, source, &mut violations);
        violations
    }
}

impl Flp00C {
    /// Find direct equality comparisons of floating-point values
    fn find_fp_equality_comparisons(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if node.kind() == "binary_expression" {
            // Check for == or != operators
            if let Some(operator) = node.child_by_field_name("operator") {
                let op_text = get_node_text(&operator, source);

                if op_text == "==" || op_text == "!=" {
                    // Check if either operand is floating-point
                    let left = node.child_by_field_name("left");
                    let right = node.child_by_field_name("right");

                    if let (Some(left_node), Some(right_node)) = (left, right) {
                        let left_is_fp = self.looks_like_floating_point(&left_node, source);
                        let right_is_fp = self.looks_like_floating_point(&right_node, source);

                        // Skip comparisons with exact values like 0.0
                        let left_is_zero = self.is_zero_literal(&left_node, source);
                        let right_is_zero = self.is_zero_literal(&right_node, source);

                        if (left_is_fp || right_is_fp) && !left_is_zero && !right_is_zero {
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                message: format!(
                                    "Direct {} comparison of floating-point values. \
                                     Floating-point arithmetic has finite precision and may produce \
                                     unexpected results across platforms and optimization levels.",
                                    if op_text == "==" {
                                        "equality"
                                    } else {
                                        "inequality"
                                    }
                                ),
                                severity: self.severity(),
                                line: operator.start_position().row + 1,
                                column: operator.start_position().column + 1,
                                file_path: String::new(),
                                suggestion: Some(
                                    "Use epsilon-based comparison (e.g., fabsf(a - b) <= __FLT_EPSILON__) \
                                     or a relative difference function for robust floating-point comparisons"
                                        .to_string(),
                                ),
                                requires_manual_review: Some(true),
                            });
                        }
                    }
                }
            }
        }

        // Recurse through children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.find_fp_equality_comparisons(&child, source, violations);
            }
        }
    }

    /// Check if expression looks like it involves floating-point values
    fn looks_like_floating_point(&self, node: &Node, source: &str) -> bool {
        let text = get_node_text(node, source);

        // Check for floating-point literals (contains decimal point or 'f' suffix)
        if text.contains('.') || text.ends_with('f') || text.ends_with('F') {
            return true;
        }

        // Check for floating-point type keywords in the expression
        if text.contains("float") || text.contains("double") {
            return true;
        }

        // Check for common floating-point math functions
        let fp_functions = [
            "sqrtf", "powf", "expf", "logf", "sinf", "cosf", "tanf", "fabsf", "fmaxf", "fminf",
            "sqrt", "pow", "exp", "log", "sin", "cos", "tan", "fabs", "fmax", "fmin",
        ];

        for func in &fp_functions {
            if text.contains(func) {
                return true;
            }
        }

        // Check if node is a call_expression to a floating-point function
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = get_node_text(&function, source);
                if fp_functions.contains(&func_name.as_ref()) {
                    return true;
                }
            }
        }

        // Check if it's a division operation (likely to produce floating-point)
        if node.kind() == "binary_expression" {
            if let Some(op) = node.child_by_field_name("operator") {
                if get_node_text(&op, source) == "/" {
                    return true;
                }
            }
        }

        false
    }

    /// Check if expression is a zero literal (0.0, 0.0f, etc.)
    fn is_zero_literal(&self, node: &Node, source: &str) -> bool {
        let text = get_node_text(node, source).trim();

        // Match various zero representations
        matches!(
            text,
            "0.0"
                | "0.0f"
                | "0.0F"
                | "0.00"
                | "0."
                | ".0"
                | "0.0L"
                | "0.0l"
                | "0e0"
                | "0.0e0"
                | "0.0e+0"
                | "0.0e-0"
        )
    }
}

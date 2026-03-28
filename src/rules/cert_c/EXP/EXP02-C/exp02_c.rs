//! EXP02-C: Be aware of the short-circuit behavior of the logical AND and OR operators
//!
//! This rule detects side effects (function calls, assignments, increments/decrements) in the
//! second operand of logical AND (&&) and OR (||) operators, which may not execute due to
//! short-circuit evaluation.
//!
//! ## Key Issues:
//! - The second operand of `&&` is only evaluated if the first operand is true
//! - The second operand of `||` is only evaluated if the first operand is false
//! - Side effects in the second operand may not occur, leading to logic errors
//!
//! ## Detected Violations:
//! - Function calls in the right operand of `&&` or `||`
//! - Assignment expressions in the right operand
//! - Increment/decrement operators (++, --) in the right operand
//! - Compound assignments (+=, -=, etc.) in the right operand
//!
//! ## Compliant Patterns:
//! - Side effects only in the first operand
//! - Pure boolean expressions without side effects
//! - Refactored to use separate statements for side effects

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Exp02C;

impl CertRule for Exp02C {
    fn rule_id(&self) -> &'static str {
        "EXP02-C"
    }

    fn description(&self) -> &'static str {
        "Be aware of the short-circuit behavior of the logical AND and OR operators"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "EXP02-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Exp02C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check for binary expressions that are logical AND or OR
        if node.kind() == "binary_expression" {
            if let Some(operator) = node.child_by_field_name("operator") {
                let op_text = get_node_text(&operator, source);

                // Only check logical AND (&&) and OR (||) operators
                if matches!(op_text, "&&" | "||") {
                    if let Some(right) = node.child_by_field_name("right") {
                        // Exempt null-guard && function-call pattern:
                        // `ptr != NULL && func(ptr)` is a standard guard idiom
                        if op_text == "&&" {
                            if let Some(left) = node.child_by_field_name("left") {
                                if self.is_null_guard_pattern(&left, source) {
                                    // This is intentional short-circuit guarding
                                    return;
                                }
                            }
                        }

                        // Check if the right operand has side effects
                        if self.has_side_effects(&right, source) {
                            let start_point = right.start_position();
                            let right_text = get_node_text(&right, source);

                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                severity: Severity::Low,
                                message: format!(
                                    "Side effect in right operand of '{}' operator may not execute due to short-circuit evaluation: '{}'",
                                    op_text, right_text
                                ),
                                file_path: String::new(),
                                line: start_point.row + 1,
                                column: start_point.column + 1,
                                suggestion: Some(
                                    "Move side effects to separate statements before the logical expression".to_string()
                                ),
                                ..Default::default()
                            });
                        }
                    }
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

    /// Check if the left operand of && is a null/validity guard pattern
    /// e.g., `ptr != NULL`, `ptr`, `!ptr`, `ptr != 0`
    fn is_null_guard_pattern(&self, node: &Node, source: &str) -> bool {
        let text = get_node_text(node, source);

        // Pattern: `expr != NULL` or `expr != 0` or `NULL != expr`
        if text.contains("!= NULL")
            || text.contains("!=NULL")
            || text.contains("!= 0")
            || text.contains("!=0")
            || text.contains("NULL !=")
            || text.contains("NULL!=")
        {
            return true;
        }

        // Pattern: `expr == NULL` or `expr == 0` (negative check, still a guard)
        if text.contains("== NULL")
            || text.contains("==NULL")
            || text.contains("== 0")
            || text.contains("==0")
        {
            return true;
        }

        // Pattern: bare identifier or `!identifier` (truthiness check)
        if node.kind() == "identifier" || node.kind() == "unary_expression" {
            return true;
        }

        // Pattern: parenthesized expression containing null check
        if node.kind() == "parenthesized_expression" {
            let inner_text = get_node_text(node, source);
            if inner_text.contains("NULL") || inner_text.contains("!") {
                return true;
            }
        }

        false
    }

    /// Check if a call_expression is used as a getter in a field access pattern:
    /// `func()->field` or `func().field`. The call just returns a struct/pointer
    /// and the result is read — no observable side effect.
    fn is_getter_in_field_access(&self, call_node: &Node) -> bool {
        if let Some(parent) = call_node.parent() {
            if parent.kind() == "field_expression" {
                if let Some(arg) = parent.child_by_field_name("argument") {
                    return arg.id() == call_node.id();
                }
            }
        }
        false
    }

    /// Check if a node contains side effects (function calls, assignments, increments)
    fn has_side_effects(&self, node: &Node, source: &str) -> bool {
        match node.kind() {
            // Function calls have side effects — unless they're getter patterns
            "call_expression" => {
                // Exception: func()->field is a getter pattern (returns struct pointer,
                // field is read). The function call is just to obtain a reference.
                if self.is_getter_in_field_access(node) {
                    return false;
                }
                true
            }
            // Assignment operators have side effects
            "assignment_expression" => true,
            // Increment and decrement operators have side effects
            "update_expression" => {
                // Check if it's ++ or --
                if let Some(operator) = node.child_by_field_name("operator") {
                    let op = get_node_text(&operator, source);
                    matches!(op, "++" | "--")
                } else {
                    false
                }
            }
            // Compound assignment operators (+=, -=, etc.) have side effects
            "compound_assignment_expr" => true,
            // Recursively check child nodes for side effects
            _ => {
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        if self.has_side_effects(&child, source) {
                            return true;
                        }
                    }
                }
                false
            }
        }
    }
}

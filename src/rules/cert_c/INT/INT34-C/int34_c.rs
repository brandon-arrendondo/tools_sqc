use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Int34C;

impl CertRule for Int34C {
    fn rule_id(&self) -> &'static str {
        "INT34-C"
    }

    fn description(&self) -> &'static str {
        "Do not shift an expression by a negative number of bits or by greater than or equal to the number of bits that exist in the operand"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "INT34-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Int34C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check for shift expressions
        if node.kind() == "binary_expression" {
            if let Some(op) = node.child_by_field_name("operator") {
                let op_text = get_node_text(&op, source);

                // Check for shift operators
                if matches!(op_text.trim(), "<<" | ">>") {
                    // Get the shift amount (right operand)
                    if let Some(right) = node.child_by_field_name("right") {
                        let shift_amount = get_node_text(&right, source).trim().to_string();

                        // Skip if shift amount is a literal constant (not a variable)
                        let is_constant = self.is_literal_constant(&shift_amount);

                        // Skip if both operands are function parameters AND it's a right shift
                        // (left shifts always need validation even for parameters)
                        let is_safe_param_pattern = op_text.trim() == ">>"
                            && if let Some(left) = node.child_by_field_name("left") {
                                let left_operand = get_node_text(&left, source).trim().to_string();
                                self.are_function_parameters(
                                    node,
                                    &left_operand,
                                    &shift_amount,
                                    source,
                                )
                            } else {
                                false
                            };

                        // Only check for validation if not a constant and not a safe parameter pattern
                        if !is_constant && !is_safe_param_pattern {
                            // Check if there's validation for the shift amount
                            if !self.has_shift_validation(node, &shift_amount, source) {
                                violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                message: format!(
                                    "Shift operation without validation of shift amount '{}'. \
                                     Shift amount must be non-negative and less than operand bit width",
                                    shift_amount
                                ),
                                severity: self.severity(),
                                line: node.start_position().row + 1,
                                column: node.start_position().column + 1,
                                file_path: String::new(),
                                suggestion: Some(format!(
                                    "Add validation before shift: if ({} >= 0 && {} < bit_width_of_operand)",
                                    shift_amount, shift_amount
                                )),
                                requires_manual_review: None,
                            });
                            }
                        }
                    }
                }
            }
        }

        // Recursively check children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }

    /// Check if there's validation for the shift amount
    fn has_shift_validation(&self, shift_node: &Node, shift_amount: &str, source: &str) -> bool {
        // First, check if shift is inside an else block with validation in if condition
        if self.is_in_validated_else_block(shift_node, shift_amount, source) {
            return true;
        }

        // Find the containing scope
        let mut current = shift_node.parent();
        let mut scope: Option<Node> = None;

        while let Some(node) = current {
            if matches!(
                node.kind(),
                "compound_statement" | "function_definition" | "translation_unit"
            ) {
                scope = Some(node);
                break;
            }
            current = node.parent();
        }

        if let Some(scope_node) = scope {
            // Look for validation BEFORE the shift
            return self.find_shift_validation_in_scope(
                &scope_node,
                shift_node.start_position().row,
                shift_amount,
                source,
            );
        }

        false
    }

    /// Check if shift is inside an else block with validation in if condition
    fn is_in_validated_else_block(
        &self,
        shift_node: &Node,
        shift_amount: &str,
        source: &str,
    ) -> bool {
        let mut current = shift_node.parent();

        while let Some(node) = current {
            // Check if we're in an if_statement
            if node.kind() == "if_statement" {
                // Check if shift is in the else branch
                if let Some(else_clause) = node.child_by_field_name("alternative") {
                    // Check if shift is a descendant of the else clause
                    if self.is_descendant_of(&else_clause, shift_node) {
                        // Check the if condition for validation
                        if let Some(condition) = node.child_by_field_name("condition") {
                            let cond_text = get_node_text(&condition, source);
                            if cond_text.contains(shift_amount)
                                && self.is_shift_bound_check(&cond_text, shift_amount)
                            {
                                return true;
                            }
                        }
                    }
                }
            }

            current = node.parent();
        }

        false
    }

    /// Check if target is a descendant of ancestor
    fn is_descendant_of(&self, ancestor: &Node, target: &Node) -> bool {
        if ancestor.id() == target.id() {
            return true;
        }

        let mut cursor = ancestor.walk();
        for child in ancestor.children(&mut cursor) {
            if self.is_descendant_of(&child, target) {
                return true;
            }
        }

        false
    }

    /// Find shift validation in scope before the shift operation
    fn find_shift_validation_in_scope(
        &self,
        scope: &Node,
        shift_line: usize,
        shift_amount: &str,
        source: &str,
    ) -> bool {
        let mut cursor = scope.walk();
        for child in scope.children(&mut cursor) {
            // Only check statements that come BEFORE the shift
            if child.start_position().row < shift_line {
                if child.kind() == "if_statement" {
                    if let Some(condition) = child.child_by_field_name("condition") {
                        let cond_text = get_node_text(&condition, source);

                        // Check for validation patterns:
                        // - shift_amount >= PRECISION(...)
                        // - shift_amount >= sizeof(...) * CHAR_BIT
                        // - shift_amount >= <numeric constant>
                        // - shift_amount < PRECISION(...)
                        // - shift_amount < sizeof(...) * CHAR_BIT
                        // - shift_amount < <numeric constant>
                        if cond_text.contains(shift_amount) {
                            if self.is_shift_bound_check(&cond_text, shift_amount) {
                                return true;
                            }
                        }
                    }
                }
            }

            // Recursively search in child scopes that come before the shift
            if child.start_position().row < shift_line {
                if self.find_shift_validation_in_scope(&child, shift_line, shift_amount, source) {
                    return true;
                }
            }
        }

        false
    }

    /// Check if a condition is a proper shift bound check
    fn is_shift_bound_check(&self, condition: &str, shift_amount: &str) -> bool {
        // Look for patterns that validate shift bounds:
        // - shift_amount >= PRECISION(...)
        // - shift_amount >= bit_width
        // - shift_amount < bit_width
        // - PRECISION(...) > shift_amount
        // - etc.

        // Check for PRECISION macro (indicates bit width checking)
        if condition.contains("PRECISION") && condition.contains(shift_amount) {
            return true;
        }

        // Check for sizeof(...) * CHAR_BIT pattern
        if condition.contains("sizeof")
            && condition.contains("CHAR_BIT")
            && condition.contains(shift_amount)
        {
            return true;
        }

        // Check for comparison with numeric constants representing bit widths
        // (e.g., shift_amount < 32, shift_amount < 64)
        if condition.contains(shift_amount) {
            let has_comparison = condition.contains(">=")
                || condition.contains(">")
                || condition.contains("<=")
                || condition.contains("<");
            let has_numeric = condition.chars().any(|c| c.is_ascii_digit());

            if has_comparison && has_numeric {
                return true;
            }
        }

        // Check for INT_MAX, UINT_MAX, etc.
        if (condition.contains("MAX") || condition.contains("MIN"))
            && condition.contains(shift_amount)
        {
            return true;
        }

        false
    }

    /// Check if a value is a literal constant
    fn is_literal_constant(&self, value: &str) -> bool {
        // Check if it's a numeric literal
        value.chars().all(|c| c.is_ascii_digit())
    }

    /// Check if both operands are function parameters
    fn are_function_parameters(
        &self,
        node: &Node,
        left_operand: &str,
        right_operand: &str,
        source: &str,
    ) -> bool {
        // Find the containing function
        let mut current = node.parent();
        while let Some(parent) = current {
            if parent.kind() == "function_definition" {
                // Get function parameters
                if let Some(declarator) = parent.child_by_field_name("declarator") {
                    let declarator_text = get_node_text(&declarator, source);
                    // Check if both operands appear in the parameter list
                    if declarator_text.contains(left_operand)
                        && declarator_text.contains(right_operand)
                    {
                        return true;
                    }
                }
                break;
            }
            current = parent.parent();
        }
        false
    }
}

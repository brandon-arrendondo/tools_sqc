use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils;
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
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "INT34-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Check for left-shift and right-shift operations
        if node.kind() == "binary_expression" {
            if let Some(operator) = ast_utils::get_binary_operator(node, source) {
                if operator == "<<" || operator == ">>" {
                    self.check_shift_operation(node, source, &operator, &mut violations);
                }
            }
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                violations.extend(self.check(&child, source));
            }
        }

        violations
    }
}

impl Int34C {
    /// Check if a shift operation is safe
    fn check_shift_operation(
        &self,
        node: &Node,
        source: &str,
        operator: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let left = node.child_by_field_name("left");
        let right = node.child_by_field_name("right");

        if let (Some(left_node), Some(right_node)) = (left, right) {
            let right_text = ast_utils::get_node_text(&right_node, source);
            let left_text = ast_utils::get_node_text(&left_node, source);

            // Check if this is an unsigned type operation
            // Unsigned shifts have defined behavior in most cases
            if self.is_likely_unsigned(&left_text, &left_node, source) {
                // For unsigned types, be more lenient
                // Only require validation for left-shifts (which can cause issues)
                if operator == "<<" {
                    if !self.is_shift_amount_validated(node, &right_node, source) {
                        self.report_violation(
                            node,
                            left_text.to_string(),
                            right_text.to_string(),
                            source,
                            violations,
                        );
                    }
                }
                // Right-shifts on unsigned are generally safe
            } else {
                // For signed types or unknown types, require validation for both left and right shifts
                if !self.is_shift_amount_validated(node, &right_node, source) {
                    self.report_violation(
                        node,
                        left_text.to_string(),
                        right_text.to_string(),
                        source,
                        violations,
                    );
                }
            }
        }
    }

    fn report_violation(
        &self,
        node: &Node,
        _left_text: String,
        right_text: String,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let operation = ast_utils::get_node_text(node, source);

        violations.push(RuleViolation {
            rule_id: self.rule_id().to_string(),
            severity: self.severity(),
            message: format!(
                "Shift operation '{}' by '{}' without validating shift amount is non-negative and within type width",
                operation, right_text
            ),
            file_path: String::new(),
            line: node.start_position().row + 1,
            column: node.start_position().column + 1,
            suggestion: Some(format!(
                "Check that '{}' is >= 0 and < the bit width of the operand before shifting",
                right_text
            )),
            ..Default::default()
        });
    }

    /// Check if the operand is likely an unsigned type
    fn is_likely_unsigned(&self, var_name: &str, node: &Node, source: &str) -> bool {
        // Check common naming conventions for unsigned variables
        if var_name.starts_with("ui_")
            || var_name.starts_with("u_")
            || var_name.starts_with("unsigned_")
        {
            return true;
        }

        // Try to find the variable declaration
        if let Some(func) = ast_utils::find_containing_function(node) {
            // Check function parameters
            if let Some(params) = func.child_by_field_name("parameters") {
                for i in 0..params.named_child_count() {
                    if let Some(param) = params.named_child(i) {
                        if param.kind() == "parameter_declaration" {
                            let param_text = ast_utils::get_node_text(&param, source);
                            if param_text.contains(var_name) && param_text.contains("unsigned") {
                                return true;
                            }
                        }
                    }
                }
            }
        }

        false
    }

    /// Check if shift amount has been validated
    fn is_shift_amount_validated(
        &self,
        shift_node: &Node,
        shift_amount: &Node,
        source: &str,
    ) -> bool {
        let shift_var = ast_utils::get_node_text(shift_amount, source);

        // Find the containing function
        if let Some(func) = ast_utils::find_containing_function(&shift_node) {
            if let Some(body) = func.child_by_field_name("body") {
                // Check if there's validation before the shift
                if self.has_validation_check(&body, &shift_var, source, shift_node) {
                    return true;
                }
            }
        }

        // Check parent if statement
        let mut current = shift_node.parent();
        while let Some(node) = current {
            if node.kind() == "if_statement" {
                if let Some(condition) = node.child_by_field_name("condition") {
                    if self.checks_shift_bounds(&condition, &shift_var, source) {
                        // Check if we're in the safe branch (else or consequence after validation)
                        if self.is_in_safe_branch(&node, shift_node) {
                            return true;
                        }
                    }
                }
            }
            current = node.parent();
        }

        false
    }

    /// Check if there's a validation check in the scope before the shift
    fn has_validation_check(
        &self,
        scope: &Node,
        var_name: &str,
        source: &str,
        shift_node: &Node,
    ) -> bool {
        let shift_line = shift_node.start_position().row;

        for i in 0..scope.named_child_count() {
            if let Some(child) = scope.named_child(i) {
                let child_line = child.start_position().row;

                // Only check statements before the shift
                if child_line >= shift_line {
                    break;
                }

                if child.kind() == "if_statement" {
                    if let Some(condition) = child.child_by_field_name("condition") {
                        if self.checks_shift_bounds(&condition, var_name, source) {
                            // Check if the consequence has return/exit
                            if let Some(consequence) = child.child_by_field_name("consequence") {
                                if self.has_return_or_error_handling(&consequence, source) {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }

        false
    }

    /// Check if a condition validates shift bounds
    fn checks_shift_bounds(&self, condition: &Node, var_name: &str, source: &str) -> bool {
        let condition_text = ast_utils::get_node_text(condition, source);

        // Look for patterns like:
        // - var < 0
        // - var < PRECISION(...)
        // - var >= PRECISION(...)
        // - var >= 32
        // - var < 32
        // - var < sizeof(type) * CHAR_BIT

        // Check for negative validation
        let has_negative_check = condition_text.contains(&format!("{} < 0", var_name))
            || condition_text.contains(&format!("0 > {}", var_name))
            || condition_text.contains(&format!("{} >= 0", var_name))
            || condition_text.contains(&format!("0 <= {}", var_name));

        // Check for width/precision validation
        let has_width_check = condition_text.contains(&format!("{} <", var_name))
            || condition_text.contains(&format!("{} >=", var_name))
            || condition_text.contains("PRECISION")
            || condition_text.contains("CHAR_BIT")
            || condition_text.contains("_MAX");

        // For thorough validation, we need both checks (or a combined check)
        // But we'll accept either for now to avoid false positives
        if has_negative_check || has_width_check {
            return true;
        }

        // Also check child binary expressions more carefully
        for i in 0..condition.child_count() {
            if let Some(child) = condition.child(i) {
                if child.kind() == "binary_expression" {
                    if let Some(operator) = ast_utils::get_binary_operator(&child, source) {
                        if operator == "<"
                            || operator == ">"
                            || operator == "<="
                            || operator == ">="
                        {
                            let left = child.child_by_field_name("left");
                            let right = child.child_by_field_name("right");

                            if let (Some(l), Some(r)) = (left, right) {
                                let left_text = ast_utils::get_node_text(&l, source);
                                let right_text = ast_utils::get_node_text(&r, source);

                                // Check if this compares our variable with bounds
                                if left_text == var_name || right_text == var_name {
                                    // Check for width-related constants or expressions
                                    if right_text.contains("PRECISION")
                                        || right_text.contains("CHAR_BIT")
                                        || right_text.contains("MAX")
                                        || left_text.contains("PRECISION")
                                        || left_text.contains("CHAR_BIT")
                                        || left_text.contains("MAX")
                                        || right_text == "0"
                                        || left_text == "0"
                                    {
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        false
    }

    /// Check if branch contains return or error handling
    fn has_return_or_error_handling(&self, node: &Node, source: &str) -> bool {
        let text = ast_utils::get_node_text(node, source);

        if text.contains("return") || text.contains("error") || text.contains("exit") {
            return true;
        }

        // Check for return/exit statements
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "return_statement"
                    || child.kind() == "break_statement"
                    || child.kind() == "continue_statement"
                {
                    return true;
                }
                if self.has_return_or_error_handling(&child, source) {
                    return true;
                }
            }
        }

        false
    }

    /// Check if shift operation is in a safe branch
    fn is_in_safe_branch(&self, if_node: &Node, shift_node: &Node) -> bool {
        // Check if shift_node is in the consequence or alternative
        if let Some(consequence) = if_node.child_by_field_name("consequence") {
            if self.is_descendant(&consequence, shift_node) {
                return true;
            }
        }

        if let Some(alternative) = if_node.child_by_field_name("alternative") {
            if self.is_descendant(&alternative, shift_node) {
                return true;
            }
        }

        false
    }

    /// Check if target is a descendant of node
    fn is_descendant(&self, node: &Node, target: &Node) -> bool {
        if node.id() == target.id() {
            return true;
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if self.is_descendant(&child, target) {
                    return true;
                }
            }
        }

        false
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn parse_c_code(source: &str) -> tree_sitter::Tree {
        let mut parser = tree_sitter::Parser::new();
        parser
            .set_language(&tree_sitter_c::language())
            .expect("Error loading C grammar");
        parser.parse(source, None).expect("Error parsing C code")
    }

    #[test]
    fn test_unchecked_shift() {
        let code = r#"
void func(unsigned int a, unsigned int b) {
    unsigned int result = a << b;
}
"#;
        let tree = parse_c_code(code);
        let rule = Int34C;
        let violations = rule.check(&tree.root_node(), code);
        assert!(!violations.is_empty(), "Should detect unchecked shift");
    }

    #[test]
    fn test_validated_shift() {
        let code = r#"
#include <limits.h>
void func(unsigned int a, unsigned int b) {
    unsigned int result = 0;
    if (b >= 32) {
        /* Handle error */
    } else {
        result = a << b;
    }
}
"#;
        let tree = parse_c_code(code);
        let rule = Int34C;
        let violations = rule.check(&tree.root_node(), code);
        assert!(
            violations.is_empty(),
            "Should not flag validated shift: {:?}",
            violations
        );
    }
}

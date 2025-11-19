use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils;
use tree_sitter::Node;

pub struct Int33C;

impl CertRule for Int33C {
    fn rule_id(&self) -> &'static str {
        "INT33-C"
    }

    fn description(&self) -> &'static str {
        "Ensure that division and remainder operations do not result in divide-by-zero errors"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "INT33-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Check for division or modulo operations
        if node.kind() == "binary_expression" {
            if let Some(operator) = ast_utils::get_binary_operator(node, source) {
                if operator == "/" || operator == "%" {
                    self.check_division_safety(node, source, &mut violations);
                }
            }
        }

        // Check for compound assignment operators
        if node.kind() == "assignment_expression" {
            if let Some(right) = node.child_by_field_name("right") {
                let right_text = ast_utils::get_node_text(&right, source);
                // Check for /= or %=
                if right_text.starts_with("=") {
                    // This is handled by the operator field
                } else {
                    // Check the operator itself
                    for i in 0..node.child_count() {
                        if let Some(child) = node.child(i) {
                            if child.kind() == "/=" || child.kind() == "%=" {
                                self.check_compound_assignment_safety(
                                    node,
                                    source,
                                    &mut violations,
                                );
                                break;
                            }
                            let text = ast_utils::get_node_text(&child, source);
                            if text == "/=" || text == "%=" {
                                self.check_compound_assignment_safety(
                                    node,
                                    source,
                                    &mut violations,
                                );
                                break;
                            }
                        }
                    }
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

impl Int33C {
    /// Check if a division or modulo operation is safe
    fn check_division_safety(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let Some(right) = node.child_by_field_name("right") {
            let right_text = ast_utils::get_node_text(&right, source);

            // Check for direct division by zero
            if right_text == "0" {
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: self.severity(),
                    message: "Division or modulo by zero literal".to_string(),
                    file_path: String::new(),
                    line: node.start_position().row + 1,
                    column: node.start_position().column + 1,
                    suggestion: Some(
                        "Ensure divisor is not zero before performing operation".to_string(),
                    ),
                    ..Default::default()
                });
                return;
            }

            // Check if divisor is a variable that might be zero
            // Look for preceding null check
            if right.kind() == "identifier" || right.kind() == "field_expression" {
                if !self.is_divisor_checked(node, &right, source) {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: self.severity(),
                        message: format!(
                            "Division or modulo by '{}' without checking for zero",
                            right_text
                        ),
                        file_path: String::new(),
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        suggestion: Some(format!(
                            "Check if '{}' is not zero before division",
                            right_text
                        )),
                        ..Default::default()
                    });
                }
            }
        }
    }

    /// Check if a compound assignment (/= or %=) is safe
    fn check_compound_assignment_safety(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let Some(right) = node.child_by_field_name("right") {
            let right_text = ast_utils::get_node_text(&right, source);

            // Check for direct division by zero
            if right_text == "0" {
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: self.severity(),
                    message: "Compound assignment with zero divisor".to_string(),
                    file_path: String::new(),
                    line: node.start_position().row + 1,
                    column: node.start_position().column + 1,
                    suggestion: Some(
                        "Ensure divisor is not zero before performing operation".to_string(),
                    ),
                    ..Default::default()
                });
                return;
            }

            // Check if divisor is a variable that might be zero
            if right.kind() == "identifier" || right.kind() == "field_expression" {
                if !self.is_divisor_checked(node, &right, source) {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: self.severity(),
                        message: format!(
                            "Compound assignment with '{}' without checking for zero",
                            right_text
                        ),
                        file_path: String::new(),
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        suggestion: Some(format!(
                            "Check if '{}' is not zero before division",
                            right_text
                        )),
                        ..Default::default()
                    });
                }
            }
        }
    }

    /// Check if a divisor has been validated as non-zero
    fn is_divisor_checked(&self, div_node: &Node, divisor: &Node, source: &str) -> bool {
        let divisor_name = ast_utils::get_node_text(divisor, source);

        // Find the containing function
        if let Some(func) = ast_utils::find_containing_function(&div_node) {
            if let Some(body) = func.child_by_field_name("body") {
                // Check if there's an early return or error handling for zero divisor
                if self.has_early_return_for_zero(&body, divisor_name, source, div_node) {
                    return true;
                }
            }
        }

        // Find the containing statement or block
        let mut current = div_node.parent();
        while let Some(node) = current {
            // Look for if statements that check the divisor
            if node.kind() == "if_statement" {
                if let Some(condition) = node.child_by_field_name("condition") {
                    if self.checks_for_zero(&condition, divisor_name, source) {
                        // Check if we're in the else branch or the consequence after a zero check
                        if self.is_in_safe_branch(&node, div_node) {
                            return true;
                        }
                    }
                }
            }

            current = node.parent();
        }

        false
    }

    /// Check if there's an early return or exit when divisor is zero
    fn has_early_return_for_zero(
        &self,
        scope: &Node,
        var_name: &str,
        source: &str,
        div_node: &Node,
    ) -> bool {
        // Walk through statements before the division
        let div_line = div_node.start_position().row;

        for i in 0..scope.named_child_count() {
            if let Some(child) = scope.named_child(i) {
                let child_line = child.start_position().row;

                // Only check statements before the division
                if child_line >= div_line {
                    break;
                }

                if child.kind() == "if_statement" {
                    if let Some(condition) = child.child_by_field_name("condition") {
                        if self.checks_for_zero(&condition, var_name, source) {
                            // Check if the consequence has return/exit
                            if let Some(consequence) = child.child_by_field_name("consequence") {
                                if self.has_return_or_exit(&consequence, source) {
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

    /// Check if a branch contains return or exit
    fn has_return_or_exit(&self, node: &Node, source: &str) -> bool {
        let text = ast_utils::get_node_text(node, source);

        if text.contains("return") || text.contains("exit") || text.contains("abort") {
            return true;
        }

        // Also check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "return_statement"
                    || child.kind() == "break_statement"
                    || child.kind() == "continue_statement"
                {
                    return true;
                }
                if self.has_return_or_exit(&child, source) {
                    return true;
                }
            }
        }

        false
    }

    /// Check if division node is in a safe branch (e.g., in the body after a zero check)
    fn is_in_safe_branch(&self, if_node: &Node, div_node: &Node) -> bool {
        // Check if div_node is a descendant of the if_statement's consequence
        if let Some(consequence) = if_node.child_by_field_name("consequence") {
            if self.is_descendant(&consequence, div_node) {
                return true;
            }
        }

        // Check if there's an alternative (else) branch
        if let Some(alternative) = if_node.child_by_field_name("alternative") {
            if self.is_descendant(&alternative, div_node) {
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

    /// Check if a condition expression checks for zero
    fn checks_for_zero(&self, condition: &Node, var_name: &str, source: &str) -> bool {
        let condition_text = ast_utils::get_node_text(condition, source);

        // Look for patterns like:
        // - var == 0
        // - var != 0
        // - 0 == var
        // - !var
        // - var (truthy check)

        // Simple text-based check for common patterns
        if condition_text.contains(&format!("{} == 0", var_name))
            || condition_text.contains(&format!("{} != 0", var_name))
            || condition_text.contains(&format!("0 == {}", var_name))
            || condition_text.contains(&format!("0 != {}", var_name))
            || condition_text.contains(&format!("!{}", var_name))
        {
            return true;
        }

        // Also recursively check child nodes
        for i in 0..condition.child_count() {
            if let Some(child) = condition.child(i) {
                if child.kind() == "binary_expression" {
                    if let Some(operator) = ast_utils::get_binary_operator(&child, source) {
                        if operator == "==" || operator == "!=" {
                            let left = child.child_by_field_name("left");
                            let right = child.child_by_field_name("right");

                            if let (Some(l), Some(r)) = (left, right) {
                                let left_text = ast_utils::get_node_text(&l, source);
                                let right_text = ast_utils::get_node_text(&r, source);

                                if (left_text == var_name && right_text == "0")
                                    || (right_text == var_name && left_text == "0")
                                {
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
    fn test_direct_zero_division() {
        let code = r#"
int main() {
    int x = 10;
    int result = x / 0;
    return 0;
}
"#;
        let tree = parse_c_code(code);
        let rule = Int33C;
        let violations = rule.check(&tree.root_node(), code);
        assert!(
            !violations.is_empty(),
            "Should detect direct division by zero"
        );
    }

    #[test]
    fn test_unchecked_division() {
        let code = r#"
void func(int a, int b) {
    int result = a / b;
}
"#;
        let tree = parse_c_code(code);
        let rule = Int33C;
        let violations = rule.check(&tree.root_node(), code);
        assert!(!violations.is_empty(), "Should detect unchecked division");
    }

    #[test]
    fn test_checked_division() {
        let code = r#"
void func(int a, int b) {
    if (b != 0) {
        int result = a / b;
    }
}
"#;
        let tree = parse_c_code(code);
        let rule = Int33C;
        let violations = rule.check(&tree.root_node(), code);
        assert!(violations.is_empty(), "Should not flag checked division");
    }
}

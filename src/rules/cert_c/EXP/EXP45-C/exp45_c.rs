use tree_sitter::Node;

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;

pub struct Exp45C;

impl CertRule for Exp45C {
    fn rule_id(&self) -> &'static str {
        "EXP45-C"
    }

    fn description(&self) -> &'static str {
        "Do not perform assignments in selection statements"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "EXP45-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Exp45C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check if this node is an if_statement, while_statement, or do_statement
        match node.kind() {
            "if_statement" => {
                if let Some(condition) = node.child_by_field_name("condition") {
                    self.check_condition(&condition, source, violations);
                }
            }
            "while_statement" => {
                if let Some(condition) = node.child_by_field_name("condition") {
                    self.check_condition(&condition, source, violations);
                }
            }
            "do_statement" => {
                if let Some(condition) = node.child_by_field_name("condition") {
                    self.check_condition(&condition, source, violations);
                }
            }
            _ => {}
        }

        // Recursively check children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }

    fn check_condition(&self, condition: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // If the condition is a parenthesized_expression, unwrap it
        let mut expr = *condition;
        if expr.kind() == "parenthesized_expression" {
            if let Some(child) = expr.named_child(0) {
                expr = child;
            }
        }

        // Check for assignments in the condition
        self.check_expression_for_assignment(&expr, source, violations, true);
    }

    fn check_expression_for_assignment(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        is_top_level: bool,
    ) {
        match node.kind() {
            "assignment_expression" => {
                // Found an assignment at top level of condition - this is a violation
                // unless it's wrapped in a comparison or other expression
                if is_top_level {
                    let position = node.start_position();
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: self.severity(),
                        line: position.row + 1,
                        column: position.column + 1,
                        file_path: String::new(),
                        message: format!(
                            "Assignment in selection statement condition: '{}'",
                            get_node_text(node, source)
                        ),
                        suggestion: None,
                        requires_manual_review: None,
                    });
                }
            }
            "comma_expression" => {
                // In a comma expression, check all children
                // The rightmost expression is the one that determines the condition
                let mut cursor = node.walk();
                let children: Vec<_> = node.named_children(&mut cursor).collect();

                // Check all expressions - the last one is the top-level condition
                for (i, child) in children.iter().enumerate() {
                    let is_last = i == children.len() - 1;
                    self.check_expression_for_assignment(
                        child,
                        source,
                        violations,
                        is_last && is_top_level,
                    );
                }
            }
            "binary_expression" => {
                // If this is a comparison or other binary operator wrapping an assignment,
                // the assignment is OK (e.g., (x = y) != 0)
                // So we check children with is_top_level = false
                let mut cursor = node.walk();
                for child in node.named_children(&mut cursor) {
                    self.check_expression_for_assignment(&child, source, violations, false);
                }
            }
            _ => {
                // For other expression types, recursively check children
                // with is_top_level = false since they're nested
                let mut cursor = node.walk();
                for child in node.named_children(&mut cursor) {
                    self.check_expression_for_assignment(&child, source, violations, false);
                }
            }
        }
    }
}

use crate::manifest::{RuleCategory, Severity};
use crate::prelude::RuleViolation;
use crate::rules::cert_c::CertRule;
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct DCL21C;

impl CertRule for DCL21C {
    fn rule_id(&self) -> &'static str {
        "DCL21-C"
    }

    fn cert_id(&self) -> &'static str {
        "DCL21"
    }

    fn description(&self) -> &'static str {
        "Understand the storage of compound literals"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Recursively check all nodes
        self.check_node(node, source, &mut violations);

        violations
    }
}

impl DCL21C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Look for & operator (pointer_expression with &)
        if node.kind() == "pointer_expression" {
            let text = get_node_text(node, source);
            if text.starts_with('&') {
                // Check if the operand is a cast or type initialization (compound literal pattern)
                let mut cursor = node.walk();
                for child in node.children(&mut cursor) {
                    if child.kind() == "cast_expression" || child.kind() == "compound_literal_expression" {
                        // Check if inside a loop
                        if self.is_inside_loop(node) {
                            let start = node.start_position();
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                file_path: String::new(),
                                message: "Address of compound literal taken inside a loop. Compound literals have automatic storage duration that ends at the end of the enclosing block, potentially creating dangling pointers.".to_string(),
                                line: start.row + 1,
                                column: start.column + 1,
                                severity: self.severity(),
                                suggestion: Some("Avoid taking the address of compound literals. Use a regular variable or array instead.".to_string()),
                                requires_manual_review: Some(false),
                            });
                        }
                    }
                }
            }
        }

        // Recurse into children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }
    fn is_inside_loop(&self, node: &Node) -> bool {
        let mut current = node.parent();
        while let Some(parent) = current {
            match parent.kind() {
                "for_statement" | "while_statement" | "do_statement" => return true,
                _ => current = parent.parent(),
            }
        }
        false
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_dcl21_c() {
        let rule = DCL21C;
        assert_eq!(rule.rule_id(), "DCL21-C");
        assert_eq!(rule.cert_id(), "DCL21");
    }
}

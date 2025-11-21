use crate::manifest::{RuleCategory, Severity};
use crate::prelude::RuleViolation;
use crate::rules::cert_c::CertRule;
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct DCL38C;

impl CertRule for DCL38C {
    fn rule_id(&self) -> &'static str {
        "DCL38-C"
    }

    fn cert_id(&self) -> &'static str {
        "DCL38"
    }

    fn description(&self) -> &'static str {
        "Use the correct syntax when declaring a flexible array member"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Recursively check all struct declarations for fake flexible array members
        self.check_node_recursive(node, source, &mut violations);

        violations
    }
}

impl DCL38C {
    fn check_node_recursive(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if node.kind() == "struct_specifier" {
            if let Some(violation) = self.check_struct_for_fake_flexible_array(node, source) {
                violations.push(violation);
            }
        }

        // Recurse into children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node_recursive(&child, source, violations);
        }
    }
    fn check_struct_for_fake_flexible_array(
        &self,
        node: &Node,
        source: &str,
    ) -> Option<RuleViolation> {
        let mut cursor = node.walk();

        for child in node.children(&mut cursor) {
            if child.kind() == "field_declaration_list" {
                // Get all field declarations
                let mut field_cursor = child.walk();
                let fields: Vec<_> = child
                    .children(&mut field_cursor)
                    .filter(|c| c.kind() == "field_declaration")
                    .collect();

                // Check if the last field looks like a fake flexible array
                if let Some(last_field) = fields.last() {
                    if self.is_fake_flexible_array(last_field, source) {
                        let start = last_field.start_position();
                        return Some(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            file_path: String::new(),
                            message: "Fake flexible array member detected (e.g., 'int data[1]'). Use proper flexible array member syntax 'int data[]' instead.".to_string(),
                            line: start.row + 1,
                            column: start.column + 1,
                            severity: self.severity(),
                            suggestion: Some("Change array declaration from 'type name[1]' to 'type name[]' for flexible array member".to_string()),
                            requires_manual_review: Some(false),
                        });
                    }
                }
            }
        }

        None
    }

    fn is_fake_flexible_array(&self, field_node: &Node, source: &str) -> bool {
        // Look for any array declarator with size 1 in this field
        self.has_array_size_one(field_node, source)
    }

    fn has_array_size_one(&self, node: &Node, source: &str) -> bool {
        if node.kind() == "array_declarator" {
            let mut cursor = node.walk();
            for child in node.children(&mut cursor) {
                if child.kind() == "number_literal" {
                    let text = get_node_text(&child, source);
                    if text == "1" {
                        return true;
                    }
                }
            }
        }

        // Recurse into all children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if self.has_array_size_one(&child, source) {
                return true;
            }
        }

        false
    }
}

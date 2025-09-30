use super::{CertRule, RuleViolation};
use crate::manifest::Severity;
use tree_sitter::Node;

pub struct Str31C;

impl CertRule for Str31C {
    fn rule_id(&self) -> &'static str {
        "STR31-C"
    }

    fn description(&self) -> &'static str {
        "Guarantee that storage for strings has sufficient space for character data and the null terminator"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Check for strcpy, strcat, sprintf usage without bounds checking
        if node.kind() == "call_expression" {
            if let Some(function_node) = node.child_by_field_name("function") {
                let function_name = &source[function_node.start_byte()..function_node.end_byte()];

                match function_name {
                    "strcpy" | "strcat" | "sprintf" => {
                        let start_point = node.start_position();
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::Medium,
                            message: format!("Use of unsafe string function '{}'. Consider using safer alternatives like strncpy, strncat, or snprintf.", function_name),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some(format!("Replace '{}' with safer alternative", function_name)),
                        });
                    }
                    _ => {}
                }
            }
        }

        // Check for char array declarations without sufficient size
        if node.kind() == "array_declarator" {
            if let Some(size_node) = node.child_by_field_name("size") {
                let size_text = &source[size_node.start_byte()..size_node.end_byte()];
                if let Ok(size) = size_text.parse::<i32>() {
                    if size < 2 { // At least space for one char + null terminator
                        let start_point = node.start_position();
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::Medium,
                            message: "Character array may be too small to hold meaningful string data plus null terminator".to_string(),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some("Increase array size to accommodate expected string length plus null terminator".to_string()),
                        });
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
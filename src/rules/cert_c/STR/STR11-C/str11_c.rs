use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils;
use tree_sitter::Node;

pub struct Str11C;

impl CertRule for Str11C {
    fn rule_id(&self) -> &'static str {
        "STR11-C"
    }

    fn description(&self) -> &'static str {
        "Do not specify the bound of a character array initialized with a string literal"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "STR11-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Str11C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Look for variable declarations
        if node.kind() == "declaration" {
            self.check_declaration(node, source, violations);
        }

        // Recursively check children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }

    fn check_declaration(
        &self,
        decl_node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Get the type specifier to check if it's char
        let type_text = self.get_type_from_declaration(decl_node, source);
        if !type_text.contains("char") {
            return;
        }

        // Check each init_declarator
        let mut cursor = decl_node.walk();
        for child in decl_node.children(&mut cursor) {
            if child.kind() == "init_declarator" {
                self.check_init_declarator(&child, source, violations);
            }
        }
    }

    fn get_type_from_declaration(&self, decl_node: &Node, source: &str) -> String {
        let mut type_parts = Vec::new();
        let mut cursor = decl_node.walk();
        for child in decl_node.children(&mut cursor) {
            if matches!(
                child.kind(),
                "type_qualifier" | "storage_class_specifier" | "primitive_type"
            ) {
                type_parts.push(ast_utils::get_node_text(&child, source));
            }
        }
        type_parts.join(" ")
    }

    fn check_init_declarator(
        &self,
        init_decl_node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Get the declarator (should be an array_declarator)
        let declarator = match init_decl_node.child_by_field_name("declarator") {
            Some(d) => d,
            None => return,
        };

        // Check if it's an array declarator with a size
        if declarator.kind() != "array_declarator" {
            return;
        }

        // Get the array size
        let array_size = match self.get_array_size(&declarator, source) {
            Some(size) => size,
            None => return, // No explicit size specified (allowed)
        };

        // Get the initializer
        let initializer = match init_decl_node.child_by_field_name("value") {
            Some(init) => init,
            None => return, // No initializer
        };

        // Check if initializer is a string literal
        let init_text = ast_utils::get_node_text(&initializer, source);
        if !init_text.starts_with('"') {
            return;
        }

        // Get the string literal length (without quotes)
        let string_content = init_text.trim_matches('"');
        let string_length = string_content.len();

        // Check if array size equals string length (missing null terminator)
        if array_size == string_length {
            violations.push(RuleViolation {
                rule_id: self.rule_id().to_string(),
                severity: self.severity(),
                message: format!(
                    "Character array bound [{}] matches string literal length \"{}\", \
                     missing space for null terminator. Array needs size [{}]",
                    array_size,
                    if string_content.len() > 20 {
                        format!("{}...", &string_content[..20])
                    } else {
                        string_content.to_string()
                    },
                    string_length + 1
                ),
                file_path: String::new(),
                line: declarator.start_position().row + 1,
                column: declarator.start_position().column + 1,
                suggestion: Some(
                    "Remove the array bound and let the compiler determine the correct size, or increase bound by 1"
                        .to_string(),
                ),
                ..Default::default()
            });
        }
        // Also check if array size is less than string length + 1 (too small)
        else if array_size < string_length + 1 {
            violations.push(RuleViolation {
                rule_id: self.rule_id().to_string(),
                severity: self.severity(),
                message: format!(
                    "Character array bound [{}] is too small for string literal \"{}\". \
                     String needs {} characters (including null terminator)",
                    array_size,
                    if string_content.len() > 20 {
                        format!("{}...", &string_content[..20])
                    } else {
                        string_content.to_string()
                    },
                    string_length + 1
                ),
                file_path: String::new(),
                line: declarator.start_position().row + 1,
                column: declarator.start_position().column + 1,
                suggestion: Some(format!(
                    "Remove the array bound or set it to at least [{}]",
                    string_length + 1
                )),
                ..Default::default()
            });
        }
    }

    fn get_array_size(&self, array_decl_node: &Node, source: &str) -> Option<usize> {
        // Look for the size child
        if let Some(size_node) = array_decl_node.child_by_field_name("size") {
            let size_text = ast_utils::get_node_text(&size_node, source);
            // Try to parse as integer
            size_text.trim().parse::<usize>().ok()
        } else {
            None
        }
    }
}

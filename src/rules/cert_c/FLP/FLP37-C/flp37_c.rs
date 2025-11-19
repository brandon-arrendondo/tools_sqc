use crate::manifest::{RuleCategory, Severity};
use crate::prelude::RuleViolation;
use crate::rules::cert_c::CertRule;
use crate::utility::cert_c::ast_utils::get_node_text;
use std::collections::HashMap;
use tree_sitter::Node;

pub struct FLP37C;

impl CertRule for FLP37C {
    fn rule_id(&self) -> &'static str {
        "FLP37-C"
    }

    fn cert_id(&self) -> &'static str {
        "FLP37"
    }

    fn description(&self) -> &'static str {
        "Do not use object representations to compare floating-point values"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // First pass: collect struct definitions with floating-point fields
        let mut float_structs = HashMap::new();
        self.collect_float_structs(node, source, &mut float_structs);

        // Second pass: check memcmp calls with those structs
        self.check_memcmp_calls(node, source, &float_structs, &mut violations);

        violations
    }
}

impl FLP37C {
    fn collect_float_structs(
        &self,
        node: &Node,
        source: &str,
        float_structs: &mut HashMap<String, bool>,
    ) {
        if node.kind() == "struct_specifier" {
            if let Some((struct_name, has_float)) = self.check_struct_for_float(node, source) {
                if has_float {
                    float_structs.insert(struct_name, true);
                }
            }
        }

        // Recurse
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.collect_float_structs(&child, source, float_structs);
        }
    }

    fn check_struct_for_float(&self, node: &Node, source: &str) -> Option<(String, bool)> {
        let mut struct_name = None;
        let mut has_float = false;

        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            match child.kind() {
                "type_identifier" => {
                    struct_name = Some(get_node_text(&child, source).to_string());
                }
                "field_declaration_list" => {
                    has_float = self.has_float_field(&child, source);
                }
                _ => {}
            }
        }

        if let Some(name) = struct_name {
            return Some((name, has_float));
        }

        None
    }

    fn has_float_field(&self, node: &Node, source: &str) -> bool {
        if node.kind() == "primitive_type" {
            let type_text = get_node_text(node, source);
            if type_text == "float" || type_text == "double" {
                return true;
            }
        }

        // Recurse
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if self.has_float_field(&child, source) {
                return true;
            }
        }

        false
    }

    fn check_memcmp_calls(
        &self,
        node: &Node,
        source: &str,
        float_structs: &HashMap<String, bool>,
        violations: &mut Vec<RuleViolation>,
    ) {
        if node.kind() == "call_expression" {
            let mut cursor = node.walk();
            let mut is_memcmp = false;

            for child in node.children(&mut cursor) {
                if child.kind() == "identifier" {
                    let func_name = get_node_text(&child, source);
                    if func_name == "memcmp" {
                        is_memcmp = true;
                        break;
                    }
                }
            }

            if is_memcmp {
                // Check if any argument involves a struct with float fields
                for child in node.children(&mut cursor) {
                    if child.kind() == "argument_list" {
                        if self.args_contain_float_struct(&child, source, float_structs) {
                            let start = node.start_position();
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                file_path: String::new(),
                                message: "Using memcmp() on struct containing floating-point members. Floating-point values should be compared field-by-field, not by object representation.".to_string(),
                                line: start.row + 1,
                                column: start.column + 1,
                                severity: self.severity(),
                                suggestion: Some("Compare floating-point fields individually instead of using memcmp()".to_string()),
                                requires_manual_review: Some(false),
                            });
                            break;
                        }
                    }
                }
            }
        }

        // Recurse
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_memcmp_calls(&child, source, float_structs, violations);
        }
    }

    fn args_contain_float_struct(
        &self,
        node: &Node,
        source: &str,
        float_structs: &HashMap<String, bool>,
    ) -> bool {
        // Check if this node or its children reference a struct with floats
        // Look for sizeof(struct S) patterns
        if node.kind() == "sizeof_expression" {
            let mut cursor = node.walk();
            for child in node.children(&mut cursor) {
                if child.kind() == "type_descriptor" {
                    if let Some(struct_name) = self.extract_struct_name(&child, source) {
                        if float_structs.contains_key(&struct_name) {
                            return true;
                        }
                    }
                }
            }
        }

        // Recurse
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if self.args_contain_float_struct(&child, source, float_structs) {
                return true;
            }
        }

        false
    }

    fn extract_struct_name(&self, node: &Node, source: &str) -> Option<String> {
        if node.kind() == "struct_specifier" || node.kind() == "type_identifier" {
            let mut cursor = node.walk();
            for child in node.children(&mut cursor) {
                if child.kind() == "type_identifier" {
                    return Some(get_node_text(&child, source).to_string());
                }
            }
        }

        // Recurse
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if let Some(name) = self.extract_struct_name(&child, source) {
                return Some(name);
            }
        }

        None
    }
}

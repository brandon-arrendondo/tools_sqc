use crate::manifest::{RuleCategory, Severity};
use crate::prelude::RuleViolation;
use crate::rules::cert_c::CertRule;
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct DCL19C;

impl CertRule for DCL19C {
    fn rule_id(&self) -> &'static str {
        "DCL19-C"
    }

    fn cert_id(&self) -> &'static str {
        "DCL19"
    }

    fn description(&self) -> &'static str {
        "Minimize the scope of variables and functions"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        if node.kind() == "translation_unit" {
            let mut cursor = node.walk();

            // First pass: collect all function definitions
            let mut defined_functions = std::collections::HashMap::new();
            let mut called_functions = std::collections::HashSet::new();

            for child in node.children(&mut cursor) {
                match child.kind() {
                    "declaration" => {
                        // Check file-scope variables
                        if let Some(violation) = self.check_file_scope_variable(&child, source) {
                            violations.push(violation);
                        }
                    }
                    "function_definition" => {
                        let func_name = self.get_function_name_str(&child, source);
                        let is_static = self.is_static_function(&child, source);
                        defined_functions.insert(func_name.clone(), (child, is_static));
                    }
                    _ => {}
                }
            }

            // Second pass: collect all function calls
            for child in node.children(&mut cursor) {
                self.collect_function_calls(&child, source, &mut called_functions);
            }

            // Check: if a function is defined (non-static) AND called within same file,
            // it should be static
            for (func_name, (func_node, is_static)) in &defined_functions {
                if !is_static && called_functions.contains(func_name.as_str()) {
                    let start = func_node.start_position();
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        file_path: String::new(),
                        message: format!(
                            "Function '{}' is only used within this file. It should be declared static to minimize scope.",
                            func_name
                        ),
                        line: start.row + 1,
                        column: start.column + 1,
                        severity: self.severity(),
                        suggestion: Some(format!("Add 'static' storage class to function '{}'", func_name)),
                        requires_manual_review: Some(false),
                    });
                }
            }
        }

        violations
    }
}

impl DCL19C {
    fn collect_function_calls(
        &self,
        node: &Node,
        source: &str,
        calls: &mut std::collections::HashSet<String>,
    ) {
        if node.kind() == "call_expression" {
            let mut cursor = node.walk();
            for child in node.children(&mut cursor) {
                if child.kind() == "identifier" {
                    calls.insert(get_node_text(&child, source).to_string());
                    break;
                }
            }
        }

        // Recurse into children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.collect_function_calls(&child, source, calls);
        }
    }

    fn is_static_function(&self, node: &Node, source: &str) -> bool {
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if child.kind() == "storage_class_specifier" {
                let text = get_node_text(&child, source);
                if text == "static" {
                    return true;
                }
            }
        }
        false
    }

    fn check_file_scope_variable(&self, node: &Node, source: &str) -> Option<RuleViolation> {
        // Check if this is a non-static global variable
        let mut cursor = node.walk();
        let mut is_static = false;
        let mut is_extern = false;
        let mut has_init_declarator = false;

        for child in node.children(&mut cursor) {
            match child.kind() {
                "storage_class_specifier" => {
                    let text = get_node_text(&child, source);
                    if text == "static" {
                        is_static = true;
                    } else if text == "extern" {
                        is_extern = true;
                    }
                }
                "init_declarator" => {
                    has_init_declarator = true;
                }
                _ => {}
            }
        }

        // Trigger violation if it's a non-static, non-extern global variable
        if !is_static && !is_extern && has_init_declarator {
            let start = node.start_position();
            return Some(RuleViolation {
                rule_id: self.rule_id().to_string(),
                file_path: String::new(),
                message: "File-scope variable should have minimal scope. Consider making it static within a function or limiting its scope.".to_string(),
                line: start.row + 1,
                column: start.column + 1,
                severity: self.severity(),
                suggestion: Some("Move variable to the smallest scope where it's used, or make it static inside a function".to_string()),
                requires_manual_review: Some(true),
            });
        }

        None
    }

    #[allow(dead_code)]
    fn check_file_scope_function(&self, node: &Node, source: &str) -> Option<RuleViolation> {
        // Check if this is a non-static function definition
        let mut cursor = node.walk();
        let mut is_static = false;

        for child in node.children(&mut cursor) {
            if child.kind() == "storage_class_specifier" {
                let text = get_node_text(&child, source);
                if text == "static" {
                    is_static = true;
                    break;
                }
            }
        }

        // Only flag non-static function definitions
        if !is_static {
            let start = node.start_position();

            // Get function name for better error message
            let function_name = self.get_function_name_str(node, source);

            return Some(RuleViolation {
                rule_id: self.rule_id().to_string(),
                file_path: String::new(),
                message: format!(
                    "Function '{}' has file scope without 'static'. Consider making it static if it's only used within this file.",
                    function_name
                ),
                line: start.row + 1,
                column: start.column + 1,
                severity: self.severity(),
                suggestion: Some("Add 'static' storage class if function is only used within this file".to_string()),
                requires_manual_review: Some(true),
            });
        }

        None
    }

    fn get_function_name_str(&self, node: &Node, source: &str) -> String {
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if child.kind() == "function_declarator" {
                return self.extract_function_name(&child, source);
            }
        }
        String::from("function")
    }

    fn extract_function_name(&self, node: &Node, source: &str) -> String {
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if child.kind() == "identifier" {
                return get_node_text(&child, source).to_string();
            } else if child.kind() == "pointer_declarator" || child.kind() == "function_declarator"
            {
                let name = self.extract_function_name(&child, source);
                if name != "function" {
                    return name;
                }
            }
        }
        String::from("function")
    }
}

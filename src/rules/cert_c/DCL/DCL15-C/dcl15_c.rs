//! DCL15-C: Declare file-scope objects or functions that do not need external linkage as static
//!
//! This rule detects file-scope functions and objects that lack the `static` keyword
//! and could be declared with internal linkage to avoid namespace pollution and improve
//! encapsulation.

use crate::manifest::{RuleCategory, Severity};
use crate::rules::{CertRule, RuleViolation};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Dcl15C;

impl CertRule for Dcl15C {
    fn rule_id(&self) -> &'static str {
        "DCL15-C"
    }

    fn description(&self) -> &'static str {
        "Declare file-scope objects or functions that do not need external linkage as static"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "DCL15-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_translation_unit(node, source, &mut violations);
        violations
    }
}

impl Dcl15C {
    fn check_translation_unit(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Only check children of translation_unit (file scope)
        if node.kind() == "translation_unit" {
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    self.check_file_scope_declaration(&child, source, violations);
                }
            }
        }

        // Recursively find translation_unit if we're not at root yet
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "translation_unit" {
                    self.check_translation_unit(&child, source, violations);
                }
            }
        }
    }

    fn check_file_scope_declaration(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Check function definitions at file scope
        if node.kind() == "function_definition" {
            self.check_function_definition(node, source, violations);
        }
        // Check variable declarations at file scope
        else if node.kind() == "declaration" {
            self.check_variable_declaration(node, source, violations);
        }
    }

    fn check_function_definition(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Get the function declarator to find the function name
        if let Some(declarator) = node.child_by_field_name("declarator") {
            if let Some(function_name) = self.extract_function_name(&declarator, source) {
                // Skip main() and other standard entry points
                if self.is_standard_entry_point(&function_name) {
                    return;
                }

                // Check if the function has static storage class
                if !self.has_static_storage_class(node, source) {
                    let start_point = node.start_position();
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::Low,
                        message: format!(
                            "Function '{}' should be declared static if it doesn't need external linkage",
                            function_name
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some(format!(
                            "Add 'static' storage-class specifier: static {} {}",
                            self.extract_return_type(node, source).unwrap_or("void".to_string()),
                            function_name
                        )),
                        ..Default::default()
                    });
                }
            }
        }
    }

    fn check_variable_declaration(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Check if this is a variable declaration (not a typedef, struct, etc.)
        if self.is_variable_declaration(node, source) {
            // Check if it has static storage class
            if !self.has_static_storage_class(node, source) {
                // Extract variable name
                if let Some(var_name) = self.extract_variable_name(node, source) {
                    // Skip certain patterns (like const globals that are meant to be public)
                    if !self.should_skip_variable(&var_name, node, source) {
                        let start_point = node.start_position();
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::Low,
                            message: format!(
                                "Variable '{}' should be declared static if it doesn't need external linkage",
                                var_name
                            ),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some("Add 'static' storage-class specifier".to_string()),
                            ..Default::default()
                        });
                    }
                }
            }
        }
    }

    fn has_static_storage_class(&self, node: &Node, source: &str) -> bool {
        // Look for storage_class_specifier nodes that contain "static"
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "storage_class_specifier" {
                    let text = get_node_text(&child, source);
                    if text == "static" {
                        return true;
                    }
                }
            }
        }
        false
    }

    fn extract_function_name(&self, declarator: &Node, source: &str) -> Option<String> {
        // Handle function_declarator
        if declarator.kind() == "function_declarator" {
            if let Some(inner_declarator) = declarator.child_by_field_name("declarator") {
                return self.extract_identifier(&inner_declarator, source);
            }
        }
        // Handle pointer_declarator
        else if declarator.kind() == "pointer_declarator" {
            if let Some(inner_declarator) = declarator.child_by_field_name("declarator") {
                return self.extract_function_name(&inner_declarator, source);
            }
        }
        // Handle identifier directly
        else if declarator.kind() == "identifier" {
            return Some(get_node_text(declarator, source).to_string());
        }

        None
    }

    fn extract_identifier(&self, node: &Node, source: &str) -> Option<String> {
        if node.kind() == "identifier" {
            Some(get_node_text(node, source).to_string())
        } else {
            // Recursively search for identifier
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if let Some(id) = self.extract_identifier(&child, source) {
                        return Some(id);
                    }
                }
            }
            None
        }
    }

    fn is_standard_entry_point(&self, function_name: &str) -> bool {
        // Standard entry points that should not be static
        matches!(
            function_name,
            "main" | "_start" | "WinMain" | "wWinMain" | "DllMain"
        )
    }

    fn extract_return_type(&self, function_def: &Node, source: &str) -> Option<String> {
        // Look for type node
        if let Some(type_node) = function_def.child_by_field_name("type") {
            Some(get_node_text(&type_node, source).to_string())
        } else {
            None
        }
    }

    fn is_variable_declaration(&self, node: &Node, source: &str) -> bool {
        // Check if this declaration contains a declarator (not just a type definition)
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                // Skip typedef, struct, union, enum declarations
                if child.kind() == "storage_class_specifier" {
                    let text = get_node_text(&child, source);
                    if text == "typedef" || text == "extern" {
                        return false;
                    }
                }
                // If it has an init_declarator or declarator, it's a variable declaration
                if child.kind() == "init_declarator" || child.kind() == "declarator" {
                    return true;
                }
            }
        }
        false
    }

    fn extract_variable_name(&self, declaration: &Node, source: &str) -> Option<String> {
        // Look for declarator or init_declarator
        for i in 0..declaration.child_count() {
            if let Some(child) = declaration.child(i) {
                if child.kind() == "init_declarator" {
                    if let Some(declarator) = child.child_by_field_name("declarator") {
                        return self.extract_identifier(&declarator, source);
                    }
                } else if child.kind() == "declarator" {
                    return self.extract_identifier(&child, source);
                }
            }
        }
        None
    }

    fn should_skip_variable(&self, _var_name: &str, _node: &Node, _source: &str) -> bool {
        // For now, don't skip any variables
        // Could be extended to skip certain patterns like API exports
        false
    }
}

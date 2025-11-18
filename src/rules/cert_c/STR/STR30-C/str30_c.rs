use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils;
use std::collections::HashSet;
use tree_sitter::Node;

pub struct Str30C;

impl CertRule for Str30C {
    fn rule_id(&self) -> &'static str {
        "STR30-C"
    }

    fn description(&self) -> &'static str {
        "Do not attempt to modify string literals"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "STR30-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        let mut analyzer = StringLiteralAnalyzer::new();

        // Analyze the current node and its subtree
        analyzer.analyze_node(node, source, &mut violations);

        violations
    }
}

struct StringLiteralAnalyzer {
    // Track variables that point to string literals
    string_literal_vars: HashSet<String>,
}

impl StringLiteralAnalyzer {
    fn new() -> Self {
        Self {
            string_literal_vars: HashSet::new(),
        }
    }

    fn analyze_node(&mut self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            "declaration" => {
                self.process_declaration(node, source);
            }
            "assignment_expression" => {
                self.process_assignment(node, source, violations);
            }
            "call_expression" => {
                self.check_function_call(node, source, violations);
            }
            "subscript_expression" => {
                self.check_array_modification(node, source, violations);
            }
            "pointer_expression" => {
                self.check_pointer_modification(node, source, violations);
            }
            _ => {}
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.analyze_node(&child, source, violations);
            }
        }
    }

    fn process_declaration(&mut self, node: &Node, source: &str) {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "init_declarator" {
                    if let Some(declarator) = child.child_by_field_name("declarator") {
                        // Check if this is an array declaration vs a pointer
                        let is_array = self.is_array_declarator(&declarator);
                        let var_name = self.get_variable_name(&declarator, source);

                        // Only track pointer variables pointing to string literals
                        // Arrays initialized with string literals are modifiable copies
                        if let Some(value) = child.child_by_field_name("value") {
                            if self.is_string_literal(&value, source) && !is_array {
                                // This is a pointer to a string literal
                                self.string_literal_vars.insert(var_name);
                            }
                        }
                    }
                }
            }
        }
    }

    fn is_array_declarator(&self, declarator: &Node) -> bool {
        match declarator.kind() {
            "array_declarator" => true,
            "pointer_declarator" => {
                // Check if the child is an array declarator
                for i in 0..declarator.child_count() {
                    if let Some(child) = declarator.child(i) {
                        if self.is_array_declarator(&child) {
                            return true;
                        }
                    }
                }
                false
            }
            _ => false,
        }
    }

    fn process_assignment(
        &mut self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let (Some(left), Some(right)) = (
            node.child_by_field_name("left"),
            node.child_by_field_name("right"),
        ) {
            // Check if assigning to a string literal element
            if left.kind() == "subscript_expression" {
                if let Some(array) = left.child(0) {
                    if self.is_string_literal(&array, source) {
                        self.flag_violation(
                            node,
                            "Attempting to modify a string literal through array subscript",
                            violations,
                        );
                    } else if array.kind() == "identifier" {
                        let var_name = ast_utils::get_node_text_owned(&array, source);
                        if self.string_literal_vars.contains(&var_name) {
                            self.flag_violation(
                                node,
                                &format!(
                                    "Attempting to modify string literal through variable '{}'",
                                    var_name
                                ),
                                violations,
                            );
                        }
                    }
                }
            }

            // Check if assigning through a pointer to string literal
            if left.kind() == "pointer_expression" {
                if let Some(argument) = left.child_by_field_name("argument") {
                    if self.is_string_literal(&argument, source) {
                        self.flag_violation(
                            node,
                            "Attempting to modify a string literal through pointer dereference",
                            violations,
                        );
                    }
                }
            }

            // Track new assignments
            if left.kind() == "identifier" {
                let var_name = ast_utils::get_node_text_owned(&left, source);
                if self.is_string_literal(&right, source) {
                    self.string_literal_vars.insert(var_name);
                } else {
                    // If assigning non-string-literal, remove from tracking
                    self.string_literal_vars.remove(&var_name);
                }
            }
        }
    }

    fn check_function_call(
        &mut self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let Some(function) = node.child_by_field_name("function") {
            let func_name = ast_utils::get_node_text_owned(&function, source);

            // Check for functions that modify their arguments
            if is_string_modifying_function(&func_name) {
                if let Some(arguments) = node.child_by_field_name("arguments") {
                    // Get the first argument (destination for most string functions)
                    let mut arg_index = 0;
                    for i in 0..arguments.child_count() {
                        if let Some(arg) = arguments.child(i) {
                            if arg.kind() != "," && arg.kind() != "(" && arg.kind() != ")" {
                                if arg_index == 0 {
                                    // First argument is the destination
                                    if self.is_string_literal(&arg, source) {
                                        self.flag_violation(
                                            node,
                                            &format!(
                                                "Passing string literal as destination to '{}'",
                                                func_name
                                            ),
                                            violations,
                                        );
                                    } else if arg.kind() == "identifier" {
                                        let var_name = ast_utils::get_node_text_owned(&arg, source);
                                        if self.string_literal_vars.contains(&var_name) {
                                            self.flag_violation(
                                                node,
                                                &format!("Passing pointer to string literal as destination to '{}'", func_name),
                                                violations
                                            );
                                        }
                                    }
                                    break;
                                }
                                arg_index += 1;
                            }
                        }
                    }
                }
            }
        }
    }

    fn check_array_modification(
        &mut self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Check if this subscript expression is on the left side of an assignment
        if let Some(parent) = node.parent() {
            if parent.kind() == "assignment_expression" {
                if let Some(left) = parent.child_by_field_name("left") {
                    if left.byte_range() == node.byte_range() {
                        // This subscript is being assigned to
                        if let Some(array) = node.child(0) {
                            if self.is_string_literal(&array, source) {
                                self.flag_violation(
                                    node,
                                    "Attempting to modify a string literal through array subscript",
                                    violations,
                                );
                            } else if array.kind() == "identifier" {
                                let var_name = ast_utils::get_node_text_owned(&array, source);
                                if self.string_literal_vars.contains(&var_name) {
                                    self.flag_violation(
                                        node,
                                        &format!("Attempting to modify string literal through variable '{}'", var_name),
                                        violations
                                    );
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    fn check_pointer_modification(
        &mut self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Check if this pointer expression is on the left side of an assignment
        if let Some(parent) = node.parent() {
            if parent.kind() == "assignment_expression" {
                if let Some(left) = parent.child_by_field_name("left") {
                    if left.byte_range() == node.byte_range() {
                        // This pointer dereference is being assigned to
                        if let Some(argument) = node.child_by_field_name("argument") {
                            if self.is_string_literal(&argument, source) {
                                self.flag_violation(
                                    node,
                                    "Attempting to modify a string literal through pointer dereference",
                                    violations
                                );
                            }
                        }
                    }
                }
            }
        }
    }

    fn is_string_literal(&self, node: &Node, source: &str) -> bool {
        match node.kind() {
            "string_literal" | "concatenated_string" => true,
            "cast_expression" => {
                // Check if casting a string literal
                if let Some(value) = node.child_by_field_name("value") {
                    self.is_string_literal(&value, source)
                } else {
                    false
                }
            }
            _ => false,
        }
    }

    fn get_variable_name(&self, declarator: &Node, source: &str) -> String {
        match declarator.kind() {
            "identifier" => ast_utils::get_node_text_owned(declarator, source),
            "pointer_declarator" | "array_declarator" => {
                // Look for the identifier in declarators
                for i in 0..declarator.child_count() {
                    if let Some(child) = declarator.child(i) {
                        if child.kind() == "identifier" {
                            return ast_utils::get_node_text_owned(&child, source);
                        }
                        // Recursively search in nested declarators
                        let nested_name = self.get_variable_name(&child, source);
                        if nested_name != "unknown" {
                            return nested_name;
                        }
                    }
                }
                "unknown".to_string()
            }
            _ => "unknown".to_string(),
        }
    }

    fn flag_violation(&self, node: &Node, message: &str, violations: &mut Vec<RuleViolation>) {
        let start_point = node.start_position();
        violations.push(RuleViolation {
            rule_id: "STR30-C".to_string(),
            severity: Severity::High,
            message: message.to_string(),
            file_path: String::new(),
            line: start_point.row + 1,
            column: start_point.column + 1,
            suggestion: Some("Use a modifiable array instead of a string literal".to_string()),
            ..Default::default()
        });
    }
}

fn is_string_modifying_function(func_name: &str) -> bool {
    matches!(
        func_name,
        "strcpy"
            | "strncpy"
            | "strcat"
            | "strncat"
            | "sprintf"
            | "snprintf"
            | "vsprintf"
            | "vsnprintf"
            | "gets"
            | "fgets"
            | "scanf"
            | "fscanf"
            | "sscanf"
            | "strtok"
            | "memcpy"
            | "memmove"
            | "memset"
            | "bcopy"
            | "bzero"
    )
}

// DEPRECATED: Inline tests moved to src/rules/cert_c/tests/inline/

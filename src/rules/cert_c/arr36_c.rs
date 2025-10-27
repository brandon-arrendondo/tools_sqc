use super::super::{CertRule, RuleViolation};
use crate::manifest::Severity;
use tree_sitter::Node;
use std::collections::HashMap;

pub struct Arr36C;

impl CertRule for Arr36C {
    fn rule_id(&self) -> &'static str {
        "ARR36-C"
    }

    fn description(&self) -> &'static str {
        "Do not subtract or compare two pointers that do not refer to the same array"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        let mut analyzer = PointerAnalyzer::new();

        // First pass: collect variable declarations and their types
        analyzer.collect_declarations(node, source);

        // Second pass: check for violations
        self.check_node(node, source, &analyzer, &mut violations);

        violations
    }
}

impl Arr36C {
    fn check_node(&self, node: &Node, source: &str, analyzer: &PointerAnalyzer, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            "binary_expression" => {
                self.check_binary_expression(node, source, analyzer, violations);
            }
            _ => {}
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, analyzer, violations);
            }
        }
    }

    fn check_binary_expression(&self, node: &Node, source: &str, analyzer: &PointerAnalyzer, violations: &mut Vec<RuleViolation>) {
        if let Some(operator) = get_operator(node, source) {
            match operator.as_str() {
                "-" => {
                    self.check_pointer_subtraction(node, source, analyzer, violations);
                }
                "<" | "<=" | ">" | ">=" => {
                    self.check_pointer_comparison(node, source, analyzer, violations);
                }
                _ => {}
            }
        }
    }

    fn check_pointer_subtraction(&self, node: &Node, source: &str, analyzer: &PointerAnalyzer, violations: &mut Vec<RuleViolation>) {
        if let (Some(left), Some(right)) = (node.child_by_field_name("left"), node.child_by_field_name("right")) {
            let left_info = analyzer.get_pointer_info(&left, source);
            let right_info = analyzer.get_pointer_info(&right, source);

            if let (Some(left_array), Some(right_array)) = (left_info, right_info) {
                if left_array != right_array {
                    let start_point = node.start_position();
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Pointer subtraction between pointers from different arrays: '{}' and '{}'",
                            left_array, right_array
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Ensure both pointers refer to the same array before subtraction".to_string()),
                    });
                }
            }
        }
    }

    fn check_pointer_comparison(&self, node: &Node, source: &str, analyzer: &PointerAnalyzer, violations: &mut Vec<RuleViolation>) {
        if let (Some(left), Some(right)) = (node.child_by_field_name("left"), node.child_by_field_name("right")) {
            let left_info = analyzer.get_pointer_info(&left, source);
            let right_info = analyzer.get_pointer_info(&right, source);

            if let (Some(left_array), Some(right_array)) = (left_info, right_info) {
                if left_array != right_array {
                    let start_point = node.start_position();
                    let op = get_operator(node, source).unwrap_or("?".to_string());
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Pointer comparison '{}' between pointers from different arrays: '{}' and '{}'",
                            op, left_array, right_array
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Ensure both pointers refer to the same array before comparison".to_string()),
                    });
                }
            }
        }
    }
}

struct PointerAnalyzer {
    // Maps variable names to their array base (for tracking which array they belong to)
    variable_arrays: HashMap<String, String>,
}

impl PointerAnalyzer {
    fn new() -> Self {
        Self {
            variable_arrays: HashMap::new(),
        }
    }

    fn collect_declarations(&mut self, node: &Node, source: &str) {
        match node.kind() {
            "declaration" => {
                self.process_declaration(node, source);
            }
            _ => {}
        }

        // Recursively process child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.collect_declarations(&child, source);
            }
        }
    }

    fn process_declaration(&mut self, node: &Node, source: &str) {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "init_declarator" {
                    if let Some(declarator) = child.child_by_field_name("declarator") {
                        if let Some(value) = child.child_by_field_name("value") {
                            let var_name = get_identifier_from_declarator(&declarator, source);
                            let array_base = self.extract_array_base(&value, source);
                            if !var_name.is_empty() && !array_base.is_empty() {
                                self.variable_arrays.insert(var_name, array_base);
                            }
                        }
                    }
                }
            }
        }
    }

    fn extract_array_base(&self, node: &Node, source: &str) -> String {
        match node.kind() {
            "identifier" => {
                source[node.start_byte()..node.end_byte()].to_string()
            }
            "unary_expression" => {
                // Handle &array[0] or &array
                if let Some(argument) = node.child_by_field_name("argument") {
                    match argument.kind() {
                        "identifier" => source[argument.start_byte()..argument.end_byte()].to_string(),
                        "subscript_expression" => {
                            if let Some(array) = argument.child_by_field_name("argument") {
                                source[array.start_byte()..array.end_byte()].to_string()
                            } else {
                                String::new()
                            }
                        }
                        _ => String::new(),
                    }
                } else {
                    String::new()
                }
            }
            "subscript_expression" => {
                if let Some(array) = node.child_by_field_name("argument") {
                    source[array.start_byte()..array.end_byte()].to_string()
                } else {
                    String::new()
                }
            }
            _ => String::new(),
        }
    }

    fn get_pointer_info(&self, node: &Node, source: &str) -> Option<String> {
        match node.kind() {
            "identifier" => {
                let var_name = source[node.start_byte()..node.end_byte()].to_string();
                self.variable_arrays.get(&var_name).cloned()
            }
            "unary_expression" => {
                // Handle &variable patterns
                if let Some(argument) = node.child_by_field_name("argument") {
                    match argument.kind() {
                        "identifier" => {
                            let var_name = source[argument.start_byte()..argument.end_byte()].to_string();
                            Some(var_name) // The variable itself acts as the "array"
                        }
                        "field_expression" => {
                            // Handle &struct.member
                            if let Some(argument_inner) = argument.child_by_field_name("argument") {
                                let struct_name = source[argument_inner.start_byte()..argument_inner.end_byte()].to_string();
                                Some(format!("{}_struct", struct_name))
                            } else {
                                None
                            }
                        }
                        _ => None,
                    }
                } else {
                    None
                }
            }
            "field_expression" => {
                // Handle struct.member access
                if let Some(argument) = node.child_by_field_name("argument") {
                    let struct_name = source[argument.start_byte()..argument.end_byte()].to_string();
                    Some(format!("{}_struct", struct_name))
                } else {
                    None
                }
            }
            _ => None,
        }
    }
}

fn get_operator(node: &Node, source: &str) -> Option<String> {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            let text = source[child.start_byte()..child.end_byte()].to_string();
            if matches!(text.as_str(), "-" | "<" | "<=" | ">" | ">=") {
                return Some(text);
            }
        }
    }
    None
}

fn get_identifier_from_declarator(declarator: &Node, source: &str) -> String {
    match declarator.kind() {
        "identifier" => source[declarator.start_byte()..declarator.end_byte()].to_string(),
        "pointer_declarator" | "array_declarator" => {
            for i in 0..declarator.child_count() {
                if let Some(child) = declarator.child(i) {
                    if child.kind() == "identifier" {
                        return source[child.start_byte()..child.end_byte()].to_string();
                    }
                    let nested = get_identifier_from_declarator(&child, source);
                    if !nested.is_empty() {
                        return nested;
                    }
                }
            }
            String::new()
        }
        _ => String::new(),
    }
}

#[cfg(test)]
#[path = "tests/arr36_c.rs"]
mod tests;
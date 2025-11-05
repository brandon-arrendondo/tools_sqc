use super::ast_utils;
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
                        ..Default::default()
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
                        ..Default::default()
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
            "parameter_declaration" => {
                self.process_parameter(node, source);
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
                            let var_name = ast_utils::get_identifier_from_declarator(&declarator, source);
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

    fn process_parameter(&mut self, node: &Node, source: &str) {
        // For function parameters, each parameter is treated as a distinct pointer
        // We track them using their parameter name as a unique identifier
        if let Some(declarator) = node.child_by_field_name("declarator") {
            let param_name = ast_utils::get_identifier_from_declarator(&declarator, source);
            if !param_name.is_empty() {
                // Use the parameter name itself as the "array base" to make it unique
                // This ensures parameters are only equal to themselves
                self.variable_arrays.insert(param_name.clone(), format!("param:{}", param_name));
            }
        }
    }

    fn extract_array_base(&self, node: &Node, source: &str) -> String {
        let result = match node.kind() {
            "identifier" => {
                source[node.start_byte()..node.end_byte()].to_string()
            }
            "field_expression" => {
                // Handle struct.member or union.member - capture full path
                // This ensures u.int_array and u.float_array are distinct
                source[node.start_byte()..node.end_byte()].to_string()
            }
            "cast_expression" => {
                // Handle cast expressions like (int *)malloc(...) - unwrap to get the underlying value
                if let Some(value) = node.child_by_field_name("value") {
                    self.extract_array_base(&value, source)
                } else {
                    String::new()
                }
            }
            "call_expression" => {
                // Handle function calls like malloc(), calloc(), aligned_alloc()
                // Each call is treated as a distinct allocation
                // Use byte position to make each call unique, even if they have identical text
                format!("{}@{}", &source[node.start_byte()..node.end_byte()], node.start_byte())
            }
            "compound_literal_expression" => {
                // Handle compound literals like (int[]){1, 2, 3}
                // Each compound literal creates a distinct object
                // Use byte position to make each one unique, even if they have identical content
                format!("{}@{}", &source[node.start_byte()..node.end_byte()], node.start_byte())
            }
            "string_literal" => {
                // Handle string literals like "Hello" and "World"
                // Each string literal creates a distinct array object
                // Use byte position to make each one unique, even if they have identical text
                format!("{}@{}", &source[node.start_byte()..node.end_byte()], node.start_byte())
            }
            "binary_expression" => {
                // Handle pointer arithmetic like arr + size or ptr - offset
                // The base array is determined by the left operand
                if let Some(left) = node.child_by_field_name("left") {
                    self.extract_array_base(&left, source)
                } else {
                    String::new()
                }
            }
            "pointer_expression" | "unary_expression" => {
                // Handle &array[0] or &array (pointer_expression is used by tree-sitter for &)
                if let Some(argument) = node.child_by_field_name("argument") {
                    match argument.kind() {
                        "identifier" => source[argument.start_byte()..argument.end_byte()].to_string(),
                        "field_expression" => {
                            // Handle &struct.member
                            // Extract just the struct instance part to allow comparisons between
                            // different members of the same struct (ARR36-C-EX1)
                            if let Some(base) = argument.child_by_field_name("argument") {
                                source[base.start_byte()..base.end_byte()].to_string()
                            } else {
                                source[argument.start_byte()..argument.end_byte()].to_string()
                            }
                        }
                        "subscript_expression" => {
                            // For subscript expressions, we need to find the deepest base array
                            // This handles &matrix[i][j] by extracting "matrix" rather than "matrix[i]"
                            self.extract_deepest_base(&argument, source)
                        }
                        _ => String::new(),
                    }
                } else {
                    String::new()
                }
            }
            "subscript_expression" => {
                // Handle array subscripts like arrays[0], arrays[1]
                // Use the full expression to distinguish between different sub-arrays
                // This is important for multidimensional arrays where arrays[0] and arrays[1]
                // are different arrays even though they share the same base
                source[node.start_byte()..node.end_byte()].to_string()
            }
            _ => String::new(),
        };
        result
    }

    fn extract_deepest_base(&self, node: &Node, source: &str) -> String {
        // Recursively extract the deepest base array from nested subscript expressions
        // For matrix[i][j], this returns "matrix"
        // For matrix[i], this returns "matrix"
        match node.kind() {
            "subscript_expression" => {
                if let Some(array) = node.child_by_field_name("argument") {
                    self.extract_deepest_base(&array, source)
                } else {
                    String::new()
                }
            }
            "identifier" => {
                source[node.start_byte()..node.end_byte()].to_string()
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
            "cast_expression" => {
                // Handle cast expressions like (int *)ptr - unwrap to get the underlying value
                if let Some(value) = node.child_by_field_name("value") {
                    self.get_pointer_info(&value, source)
                } else {
                    None
                }
            }
            "pointer_expression" | "unary_expression" => {
                // Handle &variable patterns (pointer_expression is used by tree-sitter for &)
                if let Some(argument) = node.child_by_field_name("argument") {
                    match argument.kind() {
                        "identifier" => {
                            let var_name = source[argument.start_byte()..argument.end_byte()].to_string();
                            Some(var_name) // The variable itself acts as the "array"
                        }
                        "field_expression" => {
                            // Handle &struct.member
                            let field_path = source[argument.start_byte()..argument.end_byte()].to_string();
                            Some(field_path)
                        }
                        _ => None,
                    }
                } else {
                    None
                }
            }
            "field_expression" => {
                // Handle struct.member or union.member access
                // Extract full path to distinguish between different members
                let var_name = source[node.start_byte()..node.end_byte()].to_string();
                self.variable_arrays.get(&var_name).cloned().or_else(|| {
                    // If not in our tracking map, use the field expression itself as the identifier
                    Some(var_name)
                })
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


#[cfg(test)]
#[path = "tests/arr36_c.rs"]
mod tests;
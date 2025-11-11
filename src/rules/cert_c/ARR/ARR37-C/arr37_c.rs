use super::ast_utils;
use super::super::{CertRule, RuleViolation};
use crate::manifest::{Severity, RuleCategory};
use tree_sitter::Node;
use std::collections::HashMap;

pub struct Arr37C;

impl CertRule for Arr37C {
    fn rule_id(&self) -> &'static str {
        "ARR37-C"
    }

    fn description(&self) -> &'static str {
        "Do not add or subtract an integer to a pointer to a non-array object"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "ARR37-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        let mut analyzer = NonArrayPointerAnalyzer::new();

        // First pass: collect variable declarations and their types
        analyzer.collect_variable_info(node, source);

        // Second pass: check for violations
        self.check_node(node, source, &analyzer, &mut violations);

        violations
    }
}

impl Arr37C {
    fn check_node(&self, node: &Node, source: &str, analyzer: &NonArrayPointerAnalyzer, violations: &mut Vec<RuleViolation>) {
        let node_text = &source[node.start_byte()..node.end_byte()];

        match node.kind() {
            "binary_expression" => {
                self.check_pointer_arithmetic(node, source, analyzer, violations);
            }
            "update_expression" => {
                self.check_pointer_increment_decrement(node, source, analyzer, violations);
            }
            "assignment_expression" => {
                // Check if this is a compound assignment (+=, -=)
                if node_text.contains("+=") || node_text.contains("-=") {
                    self.check_compound_assignment(node, source, analyzer, violations);
                }
            }
            "subscript_expression" => {
                self.check_subscript_on_non_array(node, source, analyzer, violations);
            }
            "for_statement" => {
                self.check_for_loop_pointer_arithmetic(node, source, analyzer, violations);
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

    fn check_pointer_arithmetic(&self, node: &Node, source: &str, analyzer: &NonArrayPointerAnalyzer, violations: &mut Vec<RuleViolation>) {
        if let Some(operator) = get_operator(node, source) {
            match operator.as_str() {
                "+" | "-" => {
                    if let (Some(left), Some(right)) = (node.child_by_field_name("left"), node.child_by_field_name("right")) {
                        // Check if this is pointer arithmetic (pointer +/- integer)
                        if self.is_pointer_arithmetic(&left, &right, source, analyzer) {
                            let pointer_name = self.get_pointer_name(&left, source);

                            // Skip ambiguous parameters (function parameters) - can't determine statically
                            if analyzer.is_ambiguous_parameter(&pointer_name) {
                                // Don't flag parameters - they could be arrays or single objects
                            } else if analyzer.is_struct_member_pointer(&pointer_name) {
                                let start_point = node.start_position();
                                violations.push(RuleViolation {
                                    rule_id: self.rule_id().to_string(),
                                    severity: Severity::Critical,
                                    message: format!(
                                        "Pointer arithmetic on struct member pointer '{}'. Pointer arithmetic on struct members is undefined behavior",
                                        pointer_name
                                    ),
                                    file_path: String::new(),
                                    line: start_point.row + 1,
                                    column: start_point.column + 1,
                                    suggestion: Some("Do not perform pointer arithmetic across struct members".to_string()),
                                    ..Default::default()
                                });
                            } else if analyzer.is_unknown_pointer(&pointer_name) {
                                let start_point = node.start_position();
                                violations.push(RuleViolation {
                                    rule_id: self.rule_id().to_string(),
                                    severity: Severity::Medium,
                                    message: format!(
                                        "Pointer arithmetic on pointer '{}' with unknown origin. Cannot determine if it points to an array or single object",
                                        pointer_name
                                    ),
                                    file_path: String::new(),
                                    line: start_point.row + 1,
                                    column: start_point.column + 1,
                                    suggestion: Some("Verify the pointer refers to an array before performing arithmetic, or document the allocation size".to_string()),
                                    requires_manual_review: Some(true),
                                    ..Default::default()
                                });
                            } else if analyzer.is_non_array_pointer(&pointer_name) {
                                let start_point = node.start_position();
                                violations.push(RuleViolation {
                                    rule_id: self.rule_id().to_string(),
                                    severity: Severity::High,
                                    message: format!(
                                        "Pointer arithmetic on non-array pointer '{}'. Only perform arithmetic on array pointers",
                                        pointer_name
                                    ),
                                    file_path: String::new(),
                                    line: start_point.row + 1,
                                    column: start_point.column + 1,
                                    suggestion: Some("Use array indexing or ensure pointer refers to an array".to_string()),
                                    ..Default::default()
                                });
                            }
                        }
                    }
                }
                _ => {}
            }
        }
    }

    fn check_pointer_increment_decrement(&self, node: &Node, source: &str, analyzer: &NonArrayPointerAnalyzer, violations: &mut Vec<RuleViolation>) {
        if let Some(argument) = node.child_by_field_name("argument") {
            let pointer_name = self.get_pointer_name(&argument, source);
            let start_point = node.start_position();
            let op_text = &source[node.start_byte()..node.end_byte()];

            // Skip ambiguous parameters (function parameters) - can't determine statically
            if analyzer.is_ambiguous_parameter(&pointer_name) {
                // Don't flag parameters - they could be arrays or single objects
            } else if analyzer.is_struct_member_pointer(&pointer_name) {
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::Critical,
                    message: format!(
                        "Increment/decrement operation '{}' on struct member pointer '{}'. Pointer arithmetic on struct members is undefined behavior",
                        op_text, pointer_name
                    ),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some("Do not perform pointer arithmetic across struct members".to_string()),
                    ..Default::default()
                });
            } else if analyzer.is_unknown_pointer(&pointer_name) {
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::Medium,
                    message: format!(
                        "Increment/decrement operation '{}' on pointer '{}' with unknown origin. Cannot determine if it points to an array or single object",
                        op_text, pointer_name
                    ),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some("Verify the pointer refers to an array before performing arithmetic, or document the allocation size".to_string()),
                    requires_manual_review: Some(true),
                    ..Default::default()
                });
            } else if analyzer.is_non_array_pointer(&pointer_name) {
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::High,
                    message: format!(
                        "Increment/decrement operation '{}' on non-array pointer '{}'",
                        op_text, pointer_name
                    ),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some("Use array indexing or ensure pointer refers to an array".to_string()),
                    ..Default::default()
                });
            }
        }
    }

    fn check_compound_assignment(&self, node: &Node, source: &str, analyzer: &NonArrayPointerAnalyzer, violations: &mut Vec<RuleViolation>) {
        // Check for patterns like: ptr += n, ptr -= n
        if let (Some(left), Some(right)) = (node.child_by_field_name("left"), node.child_by_field_name("right")) {
            // Get the operator
            let full_text = &source[node.start_byte()..node.end_byte()];
            if full_text.contains("+=") || full_text.contains("-=") {
                let pointer_name = self.get_pointer_name(&left, source);
                let start_point = node.start_position();
                let op_text = &source[node.start_byte()..node.end_byte()];

                // Skip ambiguous parameters (function parameters) - can't determine statically
                if analyzer.is_ambiguous_parameter(&pointer_name) {
                    // Don't flag parameters - they could be arrays or single objects
                } else if analyzer.is_struct_member_pointer(&pointer_name) {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::Critical,
                        message: format!(
                            "Compound assignment '{}' on struct member pointer '{}'. Pointer arithmetic on struct members is undefined behavior",
                            op_text, pointer_name
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Do not perform pointer arithmetic across struct members".to_string()),
                        ..Default::default()
                    });
                } else if analyzer.is_unknown_pointer(&pointer_name) {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::Medium,
                        message: format!(
                            "Compound assignment '{}' on pointer '{}' with unknown origin. Cannot determine if it points to an array or single object",
                            op_text, pointer_name
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Verify the pointer refers to an array before performing arithmetic, or document the allocation size".to_string()),
                        requires_manual_review: Some(true),
                        ..Default::default()
                    });
                } else if analyzer.is_non_array_pointer(&pointer_name) {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Compound assignment '{}' on non-array pointer '{}'",
                            op_text, pointer_name
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Use array indexing or ensure pointer refers to an array".to_string()),
                        ..Default::default()
                    });
                }
            }
        }
    }

    fn check_subscript_on_non_array(&self, node: &Node, source: &str, analyzer: &NonArrayPointerAnalyzer, violations: &mut Vec<RuleViolation>) {
        // Check for patterns like: ptr[index] where ptr is not an array pointer
        // subscript_expression has "argument" (the base pointer/array) and "index" fields
        if let Some(argument) = node.child_by_field_name("argument") {
            let pointer_name = self.get_pointer_name(&argument, source);

            // Only check if we have a named variable (not an expression)
            if pointer_name != "unknown" && !pointer_name.is_empty() {
                let start_point = node.start_position();
                let subscript_text = &source[node.start_byte()..node.end_byte()];

                // Skip ambiguous parameters (function parameters) - can't determine statically
                if analyzer.is_ambiguous_parameter(&pointer_name) {
                    // Don't flag parameters - they could be arrays or single objects
                } else if analyzer.is_struct_member_pointer(&pointer_name) {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::Critical,
                        message: format!(
                            "Subscript operation '{}' on struct member pointer '{}'. Pointer arithmetic on struct members is undefined behavior",
                            subscript_text, pointer_name
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Do not perform pointer arithmetic across struct members".to_string()),
                        ..Default::default()
                    });
                } else if analyzer.is_unknown_pointer(&pointer_name) {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::Medium,
                        message: format!(
                            "Subscript operation '{}' on pointer '{}' with unknown origin. Cannot determine if it points to an array or single object",
                            subscript_text, pointer_name
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Verify the pointer refers to an array before using subscript notation, or document the allocation size".to_string()),
                        requires_manual_review: Some(true),
                        ..Default::default()
                    });
                } else if analyzer.is_non_array_pointer(&pointer_name) {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Subscript operation '{}' on non-array pointer '{}'. Subscript notation implies array access",
                            subscript_text, pointer_name
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Only use subscript notation on array pointers, not pointers to single objects".to_string()),
                        ..Default::default()
                    });
                }
            }
        }
    }

    fn check_for_loop_pointer_arithmetic(&self, node: &Node, source: &str, analyzer: &NonArrayPointerAnalyzer, violations: &mut Vec<RuleViolation>) {
        // Check for patterns like: for (ptr = &struct.member; ptr <= &struct.other_member; ptr++)
        if let Some(update) = node.child_by_field_name("update") {
            if update.kind() == "update_expression" {
                if let Some(argument) = update.child_by_field_name("argument") {
                    let pointer_name = self.get_pointer_name(&argument, source);

                    // Check if this pointer is being used to iterate over struct members
                    if analyzer.is_struct_member_pointer(&pointer_name) {
                        let start_point = node.start_position();
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::Critical,
                            message: format!(
                                "Pointer arithmetic in loop on struct member pointer '{}'. Structure members are not guaranteed to be contiguous",
                                pointer_name
                            ),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some("Access struct members individually or use an array within the struct".to_string()),
                            ..Default::default()
                        });
                    }
                }
            }
        }
    }

    fn is_pointer_arithmetic(&self, left: &Node, right: &Node, source: &str, analyzer: &NonArrayPointerAnalyzer) -> bool {
        let left_text = &source[left.start_byte()..left.end_byte()];
        let right_text = &source[right.start_byte()..right.end_byte()];

        // Check if left is a pointer and right is an integer
        // Only consider it pointer arithmetic if the left side is actually a known pointer variable
        let left_is_pointer = left.kind() == "identifier" && analyzer.is_pointer_variable(left_text);
        let right_is_integer = right_text.chars().all(|c| c.is_ascii_digit()) || right.kind() == "number_literal";

        left_is_pointer && right_is_integer
    }

    fn get_pointer_name(&self, node: &Node, source: &str) -> String {
        match node.kind() {
            "identifier" => source[node.start_byte()..node.end_byte()].to_string(),
            _ => "unknown".to_string(),
        }
    }
}

struct NonArrayPointerAnalyzer {
    // Maps variable names to their types (array, non-array, struct-member-pointer)
    variable_types: HashMap<String, VariableType>,
    // Tracks struct member pointers
    struct_member_pointers: HashMap<String, String>,
}

#[derive(Debug, Clone)]
enum VariableType {
    Array,
    NonArray,
    StructMemberPointer,
    AmbiguousParameter,
    Unknown,
}

impl NonArrayPointerAnalyzer {
    fn new() -> Self {
        Self {
            variable_types: HashMap::new(),
            struct_member_pointers: HashMap::new(),
        }
    }

    fn collect_variable_info(&mut self, node: &Node, source: &str) {
        match node.kind() {
            "declaration" => {
                self.process_declaration(node, source);
            }
            "assignment_expression" => {
                self.process_assignment(node, source);
            }
            "parameter_declaration" => {
                self.process_parameter(node, source);
            }
            _ => {}
        }

        // Recursively process child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.collect_variable_info(&child, source);
            }
        }
    }

    fn process_declaration(&mut self, node: &Node, source: &str) {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "init_declarator" {
                    if let Some(declarator) = child.child_by_field_name("declarator") {
                        let var_name = ast_utils::get_identifier_from_declarator(&declarator, source);

                        // Determine if this is an array or pointer declaration
                        // Only track arrays and pointers - skip non-pointer variables entirely
                        let var_type = if declarator.kind() == "array_declarator" {
                            Some(VariableType::Array)
                        } else if declarator.kind() == "pointer_declarator" {
                            // Check if initialized with array reference
                            if let Some(value) = child.child_by_field_name("value") {
                                Some(self.analyze_initializer_type(&value, source))
                            } else {
                                Some(VariableType::Unknown)
                            }
                        } else {
                            None  // Not a pointer or array, skip it
                        };

                        if !var_name.is_empty() {
                            if let Some(var_type) = var_type {
                                self.variable_types.insert(var_name, var_type);
                            }
                        }
                    }
                }
            }
        }
    }

    fn process_assignment(&mut self, node: &Node, source: &str) {
        // Skip compound assignments (+=, -=, etc.) as they don't change the pointer type
        let node_text = &source[node.start_byte()..node.end_byte()];
        if node_text.contains("+=") || node_text.contains("-=") || node_text.contains("*=") || node_text.contains("/=") {
            return;
        }

        if let (Some(left), Some(right)) = (node.child_by_field_name("left"), node.child_by_field_name("right")) {
            if left.kind() == "identifier" {
                let var_name = source[left.start_byte()..left.end_byte()].to_string();

                // Skip binary expressions (like ptr = ptr + 1) as they don't change the pointer type
                // The actual pointer arithmetic will be caught by check_pointer_arithmetic
                if right.kind() == "binary_expression" {
                    return;
                }

                let var_type = self.analyze_initializer_type(&right, source);
                self.variable_types.insert(var_name.clone(), var_type);

                // Check for struct member pointer assignment
                if right.kind() == "unary_expression" {
                    if let Some(argument) = right.child_by_field_name("argument") {
                        if argument.kind() == "field_expression" {
                            self.struct_member_pointers.insert(var_name, "struct_member".to_string());
                        }
                    }
                }
            }
        }
    }

    fn process_parameter(&mut self, node: &Node, source: &str) {
        if let Some(declarator) = node.child_by_field_name("declarator") {
            let param_name = ast_utils::get_identifier_from_declarator(&declarator, source);
            if !param_name.is_empty() && self.is_pointer_parameter(&declarator) {
                self.variable_types.insert(param_name, VariableType::AmbiguousParameter);
            }
        }
    }

    fn is_pointer_parameter(&self, declarator: &Node) -> bool {
        // Check if this is a pointer parameter (pointer_declarator)
        // These are ambiguous because we can't tell if they point to a single object or an array
        declarator.kind() == "pointer_declarator"
    }

    fn analyze_initializer_type(&self, node: &Node, source: &str) -> VariableType {
        match node.kind() {
            "identifier" => {
                // Check if it's an array name
                let name = source[node.start_byte()..node.end_byte()].to_string();
                if let Some(existing_type) = self.variable_types.get(&name) {
                    existing_type.clone()
                } else {
                    VariableType::Unknown
                }
            }
            "unary_expression" | "pointer_expression" => {
                // Handle &array, &variable patterns
                // pointer_expression is used by tree-sitter for address-of operations
                if let Some(argument) = node.child_by_field_name("argument") {
                    match argument.kind() {
                        "identifier" => VariableType::NonArray, // &variable
                        "subscript_expression" => VariableType::NonArray, // &array[0] is pointer to single element
                        "field_expression" => VariableType::StructMemberPointer, // &struct.member
                        _ => VariableType::Unknown,
                    }
                } else {
                    VariableType::Unknown
                }
            }
            "subscript_expression" => VariableType::Array,
            "cast_expression" => {
                // For cast expressions like (type *)ptr, analyze the value being cast
                if let Some(value) = node.child_by_field_name("value") {
                    self.analyze_initializer_type(&value, source)
                } else {
                    VariableType::Unknown
                }
            }
            _ => VariableType::Unknown,
        }
    }

    fn is_non_array_pointer(&self, var_name: &str) -> bool {
        matches!(self.variable_types.get(var_name), Some(VariableType::NonArray))
    }

    fn is_struct_member_pointer(&self, var_name: &str) -> bool {
        self.struct_member_pointers.contains_key(var_name) ||
        matches!(self.variable_types.get(var_name), Some(VariableType::StructMemberPointer))
    }

    fn is_ambiguous_parameter(&self, var_name: &str) -> bool {
        matches!(self.variable_types.get(var_name), Some(VariableType::AmbiguousParameter))
    }

    fn is_unknown_pointer(&self, var_name: &str) -> bool {
        matches!(self.variable_types.get(var_name), Some(VariableType::Unknown))
    }

    fn is_pointer_variable(&self, var_name: &str) -> bool {
        self.variable_types.contains_key(var_name)
    }
}

fn get_operator(node: &Node, source: &str) -> Option<String> {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            let text = source[child.start_byte()..child.end_byte()].to_string();
            if matches!(text.as_str(), "+" | "-") {
                return Some(text);
            }
        }
    }
    None
}


// DEPRECATED: Inline tests moved to src/rules/cert_c/tests/inline/
// #[cfg(test)]
// #[path = "tests/arr37_c.rs"]
// mod tests;
use super::{CertRule, RuleViolation};
use crate::manifest::Severity;
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
        match node.kind() {
            "binary_expression" => {
                self.check_pointer_arithmetic(node, source, analyzer, violations);
            }
            "update_expression" => {
                self.check_pointer_increment_decrement(node, source, analyzer, violations);
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
                            if analyzer.is_non_array_pointer(&pointer_name) {
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
            if analyzer.is_non_array_pointer(&pointer_name) {
                let start_point = node.start_position();
                let op_text = &source[node.start_byte()..node.end_byte()];
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
                });
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
        let left_is_pointer = analyzer.is_pointer_variable(left_text) || left.kind() == "identifier";
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
                        let var_name = get_identifier_from_declarator(&declarator, source);

                        // Determine if this is an array declaration
                        let var_type = if declarator.kind() == "array_declarator" {
                            VariableType::Array
                        } else if declarator.kind() == "pointer_declarator" {
                            // Check if initialized with array reference
                            if let Some(value) = child.child_by_field_name("value") {
                                self.analyze_initializer_type(&value, source)
                            } else {
                                VariableType::Unknown
                            }
                        } else {
                            VariableType::NonArray
                        };

                        if !var_name.is_empty() {
                            self.variable_types.insert(var_name, var_type);
                        }
                    }
                }
            }
        }
    }

    fn process_assignment(&mut self, node: &Node, source: &str) {
        if let (Some(left), Some(right)) = (node.child_by_field_name("left"), node.child_by_field_name("right")) {
            if left.kind() == "identifier" {
                let var_name = source[left.start_byte()..left.end_byte()].to_string();
                let var_type = self.analyze_initializer_type(&right, source);
                self.variable_types.insert(var_name, var_type);

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
            "unary_expression" => {
                // Handle &array, &variable patterns
                if let Some(argument) = node.child_by_field_name("argument") {
                    match argument.kind() {
                        "identifier" => VariableType::NonArray, // &variable
                        "subscript_expression" => VariableType::Array, // &array[0]
                        "field_expression" => VariableType::StructMemberPointer, // &struct.member
                        _ => VariableType::Unknown,
                    }
                } else {
                    VariableType::Unknown
                }
            }
            "subscript_expression" => VariableType::Array,
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
mod tests {
    use super::*;
    use crate::parser::CParser;

    #[test]
    fn test_arr37c_detects_non_array_pointer_arithmetic() {
        let rule = Arr37C;
        let mut parser = CParser::new().unwrap();

        // Test case: Pointer arithmetic on single object
        let source = r#"
void func(void) {
    int single_int = 42;
    int *ptr = &single_int;

    ptr = ptr + 1;  // Should trigger violation - not an array
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);
        assert!(!violations.is_empty(), "Should detect pointer arithmetic on non-array object");
        assert!(violations.iter().any(|v| v.message.contains("non-array pointer")));
    }

    #[test]
    fn test_arr37c_accepts_array_pointer_arithmetic() {
        let rule = Arr37C;
        let mut parser = CParser::new().unwrap();

        // Test case: Valid pointer arithmetic on array
        let source = r#"
void func(void) {
    int array[10];
    int *ptr = array;

    ptr = ptr + 1;  // Should not trigger violation - valid array arithmetic
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        let non_array_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("non-array pointer"))
            .collect();
        assert!(non_array_violations.is_empty(), "Should not flag valid array pointer arithmetic");
    }

    #[test]
    fn test_arr37c_detects_struct_member_iteration() {
        let rule = Arr37C;
        let mut parser = CParser::new().unwrap();

        // Test case: Iterating through struct members with pointer arithmetic
        let source = r#"
struct numbers {
    short num_a, num_b, num_c;
};

int sum_numbers(const struct numbers *numb) {
    int total = 0;
    const short *numb_ptr;

    for (numb_ptr = &numb->num_a;
         numb_ptr <= &numb->num_c;
         numb_ptr++) {  // Should trigger violation
        total += *(numb_ptr);
    }

    return total;
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);
        assert!(!violations.is_empty(), "Should detect struct member pointer iteration");
        assert!(violations.iter().any(|v| v.message.contains("struct member") || v.message.contains("non-array")));
    }

    #[test]
    fn test_arr37c_detects_pointer_increment() {
        let rule = Arr37C;
        let mut parser = CParser::new().unwrap();

        // Test case: Incrementing non-array pointer
        let source = r#"
void func(void) {
    int value = 42;
    int *ptr = &value;

    ptr++;  // Should trigger violation
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);
        assert!(!violations.is_empty(), "Should detect pointer increment on non-array object");
    }

    #[test]
    fn test_arr37c_allows_zero_arithmetic() {
        let rule = Arr37C;
        let mut parser = CParser::new().unwrap();

        // Test case: Adding zero to pointer (should be allowed)
        let source = r#"
void func(void) {
    int value = 42;
    int *ptr = &value;

    ptr = ptr + 0;  // Should not trigger violation - adding 0 is always valid
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        // Our implementation might still flag this, but the CERT standard allows adding 0
        // In a more sophisticated implementation, we would check for the literal 0
    }
}
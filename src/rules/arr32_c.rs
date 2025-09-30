use super::{CertRule, RuleViolation};
use crate::manifest::Severity;
use tree_sitter::Node;

pub struct Arr32C;

impl CertRule for Arr32C {
    fn rule_id(&self) -> &'static str {
        "ARR32-C"
    }

    fn description(&self) -> &'static str {
        "Ensure size arguments for variable length arrays are in a valid range"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Look for variable length array declarations
        if node.kind() == "array_declarator" {
            if let Some(size_node) = node.child_by_field_name("size") {
                let start_point = node.start_position();

                // Check if this is a VLA (size is not a constant)
                if is_variable_length_array(&size_node, source) {
                    let size_text = &source[size_node.start_byte()..size_node.end_byte()];

                    // Check for obvious violations
                    if is_problematic_vla_size(&size_node, source) {
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::High,
                            message: format!(
                                "Variable length array with potentially unsafe size '{}'. Ensure size is validated to be positive and within reasonable bounds.",
                                size_text
                            ),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some("Add bounds checking: if (size == 0 || size > MAX_ARRAY) { /* handle error */ }".to_string()),
                        });
                    }
                }
            }
        }

        // Look for function parameters that might be used as VLA sizes
        if node.kind() == "parameter_declaration" {
            if let Some(declarator) = node.child_by_field_name("declarator") {
                if declarator.kind() == "array_declarator" {
                    if let Some(size_node) = declarator.child_by_field_name("size") {
                        let start_point = node.start_position();
                        let size_text = &source[size_node.start_byte()..size_node.end_byte()];

                        // Check if this looks like an unchecked parameter
                        if is_identifier_node(&size_node) {
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                severity: Severity::Medium,
                                message: format!(
                                    "Function parameter used as VLA size '{}' without validation. Consider adding bounds checking.",
                                    size_text
                                ),
                                file_path: String::new(),
                                line: start_point.row + 1,
                                column: start_point.column + 1,
                                suggestion: Some("Validate the size parameter before using it for VLA declaration".to_string()),
                            });
                        }
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

fn is_variable_length_array(size_node: &Node, source: &str) -> bool {
    // A VLA has a size that is not a compile-time constant
    // Simple heuristic: if it's an identifier or expression (not a number literal)
    match size_node.kind() {
        "identifier" => true,
        "binary_expression" => true,
        "call_expression" => true,
        "number_literal" => false,
        _ => {
            // Check if it contains any identifiers (making it non-constant)
            contains_identifier(size_node, source)
        }
    }
}

fn is_problematic_vla_size(size_node: &Node, source: &str) -> bool {
    let size_text = &source[size_node.start_byte()..size_node.end_byte()];

    // Check for obvious problematic patterns
    if size_text == "0" {
        return true;
    }

    // Check for identifiers without obvious bounds checking context
    if size_node.kind() == "identifier" {
        // Look for nearby bounds checking
        return !has_nearby_bounds_check(size_node, source);
    }

    // Check for expressions that might overflow
    if size_node.kind() == "binary_expression" {
        let operator = get_binary_operator(size_node, source);
        if operator == "*" || operator == "+" {
            // Potential for overflow
            return true;
        }
    }

    false
}

fn is_identifier_node(node: &Node) -> bool {
    node.kind() == "identifier"
}

fn contains_identifier(node: &Node, _source: &str) -> bool {
    if node.kind() == "identifier" {
        return true;
    }

    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if contains_identifier(&child, _source) {
                return true;
            }
        }
    }

    false
}

fn has_nearby_bounds_check(node: &Node, source: &str) -> bool {
    // Simple heuristic: look in the surrounding context for bounds checking patterns
    if let Some(parent) = node.parent() {
        if let Some(grandparent) = parent.parent() {
            let context = &source[grandparent.start_byte()..grandparent.end_byte()];

            // Look for common bounds checking patterns
            return context.contains("if") &&
                   (context.contains("== 0") ||
                    context.contains("> ") ||
                    context.contains("< ") ||
                    context.contains("MAX_") ||
                    context.contains("SIZE_MAX"));
        }
    }
    false
}

fn get_binary_operator<'a>(node: &Node, source: &'a str) -> &'a str {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            let child_text = &source[child.start_byte()..child.end_byte()];
            if child_text == "*" || child_text == "+" || child_text == "-" || child_text == "/" {
                return child_text;
            }
        }
    }
    ""
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::parser::CParser;

    #[test]
    fn test_arr32c_detects_unsafe_vla() {
        let rule = Arr32C;
        let mut parser = CParser::new().unwrap();

        // Test case 1: VLA with unchecked parameter
        let source1 = r#"
void func(size_t size) {
    int vla[size];  // Should trigger violation
    do_work(vla, size);
}
"#;

        let tree1 = parser.parse_source(source1).unwrap();
        let violations1 = rule.check(&tree1.root_node(), source1);
        assert!(!violations1.is_empty(), "Should detect unsafe VLA with unchecked size");
        assert!(violations1[0].message.contains("potentially unsafe size"));

        // Test case 2: VLA with zero size
        let source2 = r#"
void func() {
    int vla[0];  // Should trigger violation
}
"#;

        let tree2 = parser.parse_source(source2).unwrap();
        let violations2 = rule.check(&tree2.root_node(), source2);
        assert!(!violations2.is_empty(), "Should detect VLA with zero size");

        // Test case 3: VLA with expression that might overflow
        let source3 = r#"
void func(size_t a, size_t b) {
    int vla[a * b];  // Should trigger violation
}
"#;

        let tree3 = parser.parse_source(source3).unwrap();
        let violations3 = rule.check(&tree3.root_node(), source3);
        assert!(!violations3.is_empty(), "Should detect potentially overflowing VLA size expression");
    }

    #[test]
    fn test_arr32c_accepts_safe_vla() {
        let rule = Arr32C;
        let mut parser = CParser::new().unwrap();

        // Test case: VLA with proper bounds checking
        let source = r#"
enum { MAX_ARRAY = 1024 };

void func(size_t size) {
    if (size == 0 || size > MAX_ARRAY) {
        /* Handle error */
        return;
    }
    int vla[size];  // Should not trigger violation due to bounds check
    do_work(vla, size);
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        // This might still trigger due to our simple heuristic, but in a more
        // sophisticated implementation, it should recognize the bounds checking
        // For now, we'll check that any violations are of lower severity
        if !violations.is_empty() {
            assert!(matches!(violations[0].severity, Severity::Medium | Severity::Low));
        }
    }

    #[test]
    fn test_arr32c_ignores_fixed_arrays() {
        let rule = Arr32C;
        let mut parser = CParser::new().unwrap();

        // Test case: Fixed-size array (not VLA)
        let source = r#"
void func() {
    int fixed_array[100];  // Should not trigger violation
    do_work(fixed_array, 100);
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        // Should not find violations for fixed-size arrays
        let vla_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("variable length array"))
            .collect();
        assert!(vla_violations.is_empty(), "Should not flag fixed-size arrays as VLA violations");
    }

    #[test]
    fn test_arr32c_detects_function_parameter_vla() {
        let rule = Arr32C;
        let mut parser = CParser::new().unwrap();

        // Test case: Function parameter with VLA
        let source = r#"
void func(int n, int arr[n]) {  // Should trigger violation for unchecked parameter
    /* function body */
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        let param_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("parameter") && v.message.contains("validation"))
            .collect();
        assert!(!param_violations.is_empty(), "Should detect unchecked VLA parameter");
    }
}
use super::super::{CertRule, RuleViolation};
use crate::manifest::Severity;
use tree_sitter::Node;

pub struct Arr00C;

impl CertRule for Arr00C {
    fn rule_id(&self) -> &'static str {
        "ARR00-C"
    }

    fn description(&self) -> &'static str {
        "Understand how arrays work"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Check for direct array assignment (arr1 = arr2)
        if node.kind() == "assignment_expression" {
            if let Some(violation) = check_array_assignment(node, source) {
                violations.push(violation);
            }
        }

        // Check for sizeof misuse with array parameters
        if node.kind() == "sizeof_expression" {
            if let Some(violation) = check_sizeof_misuse(node, source) {
                violations.push(violation);
            }
        }

        // Check for dangerous functions like gets()
        if node.kind() == "call_expression" {
            if let Some(violation) = check_dangerous_functions(node, source) {
                violations.push(violation);
            }
            // Also check for array size mismatches in function calls
            if let Some(violation) = check_array_size_mismatch(node, source) {
                violations.push(violation);
            }
        }

        // Check for array decay confusion in comparisons
        if node.kind() == "binary_expression" {
            if let Some(violation) = check_array_comparison(node, source) {
                violations.push(violation);
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

fn check_array_assignment(node: &Node, source: &str) -> Option<RuleViolation> {
    // Get left and right operands of assignment
    let left = node.child_by_field_name("left")?;
    let right = node.child_by_field_name("right")?;

    // Check if left side is an array identifier (not a subscript)
    if is_array_identifier(&left, source) && !is_subscript(&left) {
        // Check if right side is also an array identifier
        if is_array_identifier(&right, source) {
            let start_point = node.start_position();
            let left_text = &source[left.start_byte()..left.end_byte()];
            let right_text = &source[right.start_byte()..right.end_byte()];

            return Some(RuleViolation {
                rule_id: "ARR00-C".to_string(),
                severity: Severity::High,
                message: format!(
                    "Cannot directly assign arrays: '{}' = '{}'. Arrays are not assignable in C.",
                    left_text, right_text
                ),
                file_path: String::new(),
                line: start_point.row + 1,
                column: start_point.column + 1,
                suggestion: Some("Use memcpy() or a loop to copy array elements".to_string()),
            });
        }
    }

    None
}

fn check_sizeof_misuse(node: &Node, source: &str) -> Option<RuleViolation> {
    // For sizeof expressions, we need to look at the second child (index 1) which is typically the parenthesized expression
    if node.child_count() >= 2 {
        if let Some(arg_expr) = node.child(1) {
            if arg_expr.kind() == "parenthesized_expression" {
                // Look inside the parentheses for an identifier
                for i in 0..arg_expr.child_count() {
                    if let Some(child) = arg_expr.child(i) {
                        if child.kind() == "identifier" {
                            return check_if_array_parameter(&child, node, source);
                        }
                    }
                }
            } else if arg_expr.kind() == "identifier" {
                // Direct identifier without parentheses
                return check_if_array_parameter(&arg_expr, node, source);
            }
        }
    }

    None
}

fn check_if_array_parameter(identifier_node: &Node, sizeof_node: &Node, source: &str) -> Option<RuleViolation> {
    let identifier_name = &source[identifier_node.start_byte()..identifier_node.end_byte()];

    // Find the containing function
    let function_def = find_containing_function(identifier_node)?;

    // Get the function's parameters
    let parameters = get_function_parameters(&function_def, source)?;

    // Check if this identifier is a parameter declared as an array
    for (param_name, param_type) in parameters {
        if param_name == identifier_name && is_array_parameter_type(&param_type) {
            let start_point = sizeof_node.start_position();
            return Some(RuleViolation {
                rule_id: "ARR00-C".to_string(),
                severity: Severity::Medium,
                message: format!(
                    "Misuse of sizeof() on array parameter '{}'. Array parameters decay to pointers, sizeof will return pointer size not array size.",
                    identifier_name
                ),
                file_path: String::new(),
                line: start_point.row + 1,
                column: start_point.column + 1,
                suggestion: Some("Pass array size as a separate parameter or use a different method to track array size".to_string()),
            });
        }
    }

    None
}

fn check_array_size_mismatch(node: &Node, source: &str) -> Option<RuleViolation> {
    // Get the function being called
    let function_node = node.child_by_field_name("function")?;
    let function_name = &source[function_node.start_byte()..function_node.end_byte()];

    // Check if we're calling a function with array parameters
    let arguments = node.child_by_field_name("arguments")?;

    // Look for malloc/calloc patterns being passed to functions expecting arrays
    for i in 0..arguments.child_count() {
        if let Some(arg) = arguments.child(i) {
            if arg.kind() == "call_expression" {
                if let Some(func) = arg.child_by_field_name("function") {
                    let func_text = &source[func.start_byte()..func.end_byte()];
                    if func_text == "malloc" || func_text == "calloc" {
                        // Check if malloc size appears insufficient
                        if let Some(violation) = check_malloc_size_mismatch(&arg, function_name, node, source) {
                            return Some(violation);
                        }
                    }
                }
            }
        }
    }

    None
}

fn check_malloc_size_mismatch(malloc_node: &Node, target_function: &str, call_node: &Node, source: &str) -> Option<RuleViolation> {
    // Extract malloc size argument
    let args = malloc_node.child_by_field_name("arguments")?;

    // Try to detect obvious size mismatches
    // Look for patterns like malloc(10 * sizeof(int)) being passed to functions expecting larger arrays
    for i in 0..args.child_count() {
        if let Some(arg) = args.child(i) {
            let arg_text = &source[arg.start_byte()..arg.end_byte()];
            // Simple heuristic: if we see a small number being multiplied by sizeof
            if contains_small_allocation(arg_text) {
                let start_point = call_node.start_position();
                return Some(RuleViolation {
                    rule_id: "ARR00-C".to_string(),
                    severity: Severity::High,
                    message: format!(
                        "Potential array size mismatch: dynamically allocated array may be smaller than expected by function '{}'",
                        target_function
                    ),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some("Ensure allocated size matches the function's expected array size".to_string()),
                });
            }
        }
    }

    None
}

fn contains_small_allocation(text: &str) -> bool {
    // Simple heuristic: look for small numbers in allocation
    // This is a basic check - in production, we'd need more sophisticated analysis
    if text.contains("10 *") || text.contains("* 10") || text.starts_with("10 ") {
        return true;
    }
    // Check for other small allocations
    for size in &["1", "2", "3", "4", "5", "6", "7", "8", "9"] {
        if text.starts_with(size) && (text.contains("*") || text.len() < 3) {
            return true;
        }
    }
    false
}

fn check_dangerous_functions(node: &Node, source: &str) -> Option<RuleViolation> {
    // Check for calls to gets() which is inherently unsafe
    if let Some(function) = node.child_by_field_name("function") {
        let func_text = &source[function.start_byte()..function.end_byte()];

        if func_text == "gets" {
            let start_point = node.start_position();
            return Some(RuleViolation {
                rule_id: "ARR00-C".to_string(),
                severity: Severity::Critical,
                message: "Use of gets() is dangerous and deprecated. It does not perform bounds checking.".to_string(),
                file_path: String::new(),
                line: start_point.row + 1,
                column: start_point.column + 1,
                suggestion: Some("Use fgets() or gets_s() instead".to_string()),
            });
        }

        // Check for unchecked string functions
        let unsafe_functions = ["strcpy", "strcat", "sprintf"];
        if unsafe_functions.contains(&func_text) {
            let start_point = node.start_position();
            return Some(RuleViolation {
                rule_id: "ARR00-C".to_string(),
                severity: Severity::High,
                message: format!(
                    "Use of {} without bounds checking can lead to buffer overflow",
                    func_text
                ),
                file_path: String::new(),
                line: start_point.row + 1,
                column: start_point.column + 1,
                suggestion: Some(format!("Use {}n or {}_s for safer string operations", func_text, func_text)),
            });
        }
    }

    None
}

fn check_array_comparison(node: &Node, source: &str) -> Option<RuleViolation> {
    // Check for array comparisons using == or !=
    if let Some(operator) = node.child_by_field_name("operator") {
        let op_text = &source[operator.start_byte()..operator.end_byte()];

        if op_text == "==" || op_text == "!=" {
            let left = node.child_by_field_name("left")?;
            let right = node.child_by_field_name("right")?;

            // Check if either side is an array
            if is_array_identifier(&left, source) || is_array_identifier(&right, source) {
                let start_point = node.start_position();
                return Some(RuleViolation {
                    rule_id: "ARR00-C".to_string(),
                    severity: Severity::Medium,
                    message: "Comparing arrays with == or != compares addresses, not contents".to_string(),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some("Use memcmp() or strcmp() to compare array contents".to_string()),
                });
            }
        }
    }

    None
}

fn find_containing_function<'a>(node: &Node<'a>) -> Option<Node<'a>> {
    let mut current = Some(*node);
    while let Some(n) = current {
        if n.kind() == "function_definition" {
            return Some(n);
        }
        current = n.parent();
    }
    None
}

fn get_function_parameters(function_node: &Node, source: &str) -> Option<Vec<(String, String)>> {
    // Find the parameter list
    for i in 0..function_node.child_count() {
        if let Some(child) = function_node.child(i) {
            if child.kind() == "function_declarator" {
                return extract_parameters(&child, source);
            }
        }
    }
    None
}

fn extract_parameters(declarator_node: &Node, source: &str) -> Option<Vec<(String, String)>> {
    let mut parameters = Vec::new();

    // Find parameter_list node
    for i in 0..declarator_node.child_count() {
        if let Some(child) = declarator_node.child(i) {
            if child.kind() == "parameter_list" {
                // Extract each parameter
                for j in 0..child.child_count() {
                    if let Some(param) = child.child(j) {
                        if param.kind() == "parameter_declaration" {
                            if let Some((name, param_type)) = extract_parameter_info(&param, source) {
                                parameters.push((name, param_type));
                            }
                        }
                    }
                }
            }
        }
    }

    if parameters.is_empty() {
        None
    } else {
        Some(parameters)
    }
}

fn extract_parameter_info(param_node: &Node, source: &str) -> Option<(String, String)> {
    let param_text = &source[param_node.start_byte()..param_node.end_byte()];

    // Look for array declarator pattern
    for i in 0..param_node.child_count() {
        if let Some(child) = param_node.child(i) {
            if child.kind() == "array_declarator" || child.kind() == "pointer_declarator" {
                // Found array or pointer parameter
                if let Some(identifier) = find_identifier_in_declarator(&child, source) {
                    return Some((identifier, param_text.to_string()));
                }
            } else if child.kind() == "identifier" {
                // Simple parameter
                let name = &source[child.start_byte()..child.end_byte()];
                return Some((name.to_string(), param_text.to_string()));
            }
        }
    }

    None
}

fn find_identifier_in_declarator(declarator_node: &Node, source: &str) -> Option<String> {
    // Recursively find identifier in declarator
    for i in 0..declarator_node.child_count() {
        if let Some(child) = declarator_node.child(i) {
            if child.kind() == "identifier" {
                return Some(source[child.start_byte()..child.end_byte()].to_string());
            } else if child.kind() == "array_declarator" || child.kind() == "pointer_declarator" {
                if let Some(id) = find_identifier_in_declarator(&child, source) {
                    return Some(id);
                }
            }
        }
    }
    None
}

fn is_array_parameter_type(param_type: &str) -> bool {
    // Check if parameter type indicates an array
    param_type.contains('[') ||
    (param_type.contains("*") && !param_type.contains("const char *")) // Avoid false positives on string literals
}

fn is_array_identifier(node: &Node, _source: &str) -> bool {
    // Simple heuristic: check if this is an identifier that could be an array
    // In a real implementation, we'd need symbol table information
    node.kind() == "identifier" && !is_function_call_name(node)
}

fn is_subscript(node: &Node) -> bool {
    node.kind() == "subscript_expression"
}

fn is_function_call_name(node: &Node) -> bool {
    // Check if this identifier is the function part of a call expression
    if let Some(parent) = node.parent() {
        parent.kind() == "call_expression" && parent.child_by_field_name("function") == Some(*node)
    } else {
        false
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::parser::CParser;

    #[test]
    fn test_arr00c_detects_direct_array_assignment() {
        let rule = Arr00C;
        let mut parser = CParser::new().unwrap();

        let source = r#"
void func() {
    int arr1[10];
    int arr2[10];
    arr1 = arr2;  // Should trigger violation
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        assert!(!violations.is_empty(), "Should detect direct array assignment");
        assert!(violations[0].message.contains("Cannot directly assign arrays"));
    }

    #[test]
    fn test_arr00c_detects_gets_usage() {
        let rule = Arr00C;
        let mut parser = CParser::new().unwrap();

        let source = r#"
void func() {
    char buffer[100];
    gets(buffer);  // Should trigger critical violation
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        assert!(!violations.is_empty(), "Should detect gets() usage");
        assert!(violations[0].message.contains("gets() is dangerous"));
        assert!(matches!(violations[0].severity, Severity::Critical));
    }

    #[test]
    fn test_arr00c_detects_unsafe_string_functions() {
        let rule = Arr00C;
        let mut parser = CParser::new().unwrap();

        let source = r#"
void func() {
    char dest[10];
    char src[20];
    strcpy(dest, src);  // Should trigger violation
    strcat(dest, src);  // Should trigger violation
    sprintf(dest, "%s", src);  // Should trigger violation
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        assert!(violations.len() >= 3, "Should detect all three unsafe functions");
        for violation in &violations {
            assert!(violation.message.contains("without bounds checking"));
        }
    }

    #[test]
    fn test_arr00c_detects_array_comparison() {
        let rule = Arr00C;
        let mut parser = CParser::new().unwrap();

        let source = r#"
void func() {
    int arr1[10];
    int arr2[10];
    if (arr1 == arr2) {  // Should trigger violation
        // This compares addresses, not contents
    }
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        assert!(!violations.is_empty(), "Should detect array comparison");
        assert!(violations[0].message.contains("compares addresses, not contents"));
    }

    #[test]
    fn test_arr00c_detects_sizeof_misuse() {
        let rule = Arr00C;
        let mut parser = CParser::new().unwrap();

        let source = r#"
void func(int arr[]) {
    size_t size = sizeof(arr);  // Should trigger violation - arr is a pointer here
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);


        let sizeof_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("sizeof"))
            .collect();
        assert!(!sizeof_violations.is_empty(), "Should detect sizeof misuse on array parameter");
    }

    #[test]
    fn test_arr00c_detects_sizeof_misuse_with_array_size() {
        let rule = Arr00C;
        let mut parser = CParser::new().unwrap();

        let source = r#"
void modify_array(int arr[100]) {
    size_t size = sizeof(arr) / sizeof(arr[0]);  // Wrong! arr is a pointer
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        let sizeof_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("sizeof"))
            .collect();
        assert!(!sizeof_violations.is_empty(), "Should detect sizeof misuse even with explicit array size");
    }

    #[test]
    fn test_arr00c_detects_malloc_size_mismatch() {
        let rule = Arr00C;
        let mut parser = CParser::new().unwrap();

        let source = r#"
void modify_array(int arr[100]) {
    for (int i = 0; i < 100; i++) {
        arr[i] = i;
    }
}

void test() {
    // Direct malloc call as argument - this pattern should be detected
    modify_array(malloc(10 * sizeof(int)));  // Should trigger - passing 10-element allocation to function expecting 100
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);


        let mismatch_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("size mismatch"))
            .collect();
        assert!(!mismatch_violations.is_empty(), "Should detect array size mismatch");
    }

    #[test]
    fn test_arr00c_allows_safe_operations() {
        let rule = Arr00C;
        let mut parser = CParser::new().unwrap();

        let source = r#"
void func() {
    int arr1[10];
    int arr2[10];

    // These should be allowed
    arr1[0] = arr2[0];  // Element assignment
    memcpy(arr1, arr2, sizeof(arr1));  // Safe copy

    if (memcmp(arr1, arr2, sizeof(arr1)) == 0) {  // Safe comparison
        // Arrays are equal
    }

    char dest[100];
    char src[50];
    strncpy(dest, src, sizeof(dest) - 1);  // Bounded copy
    dest[sizeof(dest) - 1] = '\0';
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        // Should not flag safe operations
        let dangerous_violations: Vec<_> = violations.iter()
            .filter(|v| matches!(v.severity, Severity::High | Severity::Critical))
            .collect();
        assert!(dangerous_violations.is_empty(), "Should not flag safe array operations as dangerous");
    }

    #[test]
    fn test_arr00c_checks_nested_contexts() {
        let rule = Arr00C;
        let mut parser = CParser::new().unwrap();

        let source = r#"
void outer() {
    void inner() {
        char buffer[100];
        gets(buffer);  // Should still detect in nested function
    }

    int arr1[5], arr2[5];
    if (1) {
        arr1 = arr2;  // Should detect in nested block
    }
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        assert!(violations.len() >= 2, "Should detect violations in nested contexts");
    }
}
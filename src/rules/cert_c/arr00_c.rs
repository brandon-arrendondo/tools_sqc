//! ARR00-C: Understand how arrays work
//!
//! This rule checks for common misunderstandings about how arrays work in C:
//! - Direct array assignment (arrays are not assignable)
//! - Array comparison with == or != (compares addresses, not contents)
//! - sizeof() misuse on array parameters (arrays decay to pointers)
//! - Variable Length Arrays (VLAs) with zero, negative, or unvalidated sizes
//! - Use of gets() which has no bounds checking mechanism and is always unsafe
//! - Using unvalidated user input as loop bounds for array access
//! - Using uninitialized variables as loop bounds for array access
//! - Pointer arithmetic that obviously exceeds array bounds
//!
//! Note: Other unsafe functions (strcpy, etc.) are better checked by ARR38-C

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

        match node.kind() {
            "assignment_expression" => {
                // Check for direct array assignment (arr1 = arr2)
                if let Some(violation) = check_array_assignment(node, source) {
                    violations.push(violation);
                }
            }
            "sizeof_expression" => {
                // Check for sizeof misuse with array parameters
                if let Some(violation) = check_sizeof_misuse(node, source) {
                    violations.push(violation);
                }
            }
            "binary_expression" => {
                // Check for array comparison with == or !=
                if let Some(violation) = check_array_comparison(node, source) {
                    violations.push(violation);
                }
                // Also check for pointer arithmetic that exceeds bounds
                if let Some(violation) = check_pointer_arithmetic(node, source) {
                    violations.push(violation);
                }
            }
            "declaration" => {
                // Check for VLA with zero or invalid size
                if let Some(violation) = check_vla_declaration(node, source) {
                    violations.push(violation);
                }
            }
            "call_expression" => {
                // Check for dangerous functions like gets(), strcpy(), etc.
                if let Some(violation) = check_dangerous_functions(node, source) {
                    violations.push(violation);
                }
            }
            "for_statement" => {
                // Check for loops with unvalidated bounds accessing arrays
                if let Some(violation) = check_loop_array_access(node, source) {
                    violations.push(violation);
                }
            }
            _ => {}
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

// ============================================================================
// Core Rule Checks
// ============================================================================

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

fn check_vla_declaration(node: &Node, source: &str) -> Option<RuleViolation> {
    // Look for array_declarator in the declaration
    let mut declarator = None;
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if child.kind() == "array_declarator" {
                declarator = Some(child);
                break;
            } else if child.kind() == "init_declarator" {
                // Check inside init_declarator for array_declarator
                for j in 0..child.child_count() {
                    if let Some(inner) = child.child(j) {
                        if inner.kind() == "array_declarator" {
                            declarator = Some(inner);
                            break;
                        }
                    }
                }
            }
        }
    }

    let declarator = declarator?;

    // Get the size expression from the array declarator
    // array_declarator has structure: identifier [ size ]
    let mut size_node = None;
    let mut found_open_bracket = false;
    for i in 0..declarator.child_count() {
        if let Some(child) = declarator.child(i) {
            if child.kind() == "[" {
                found_open_bracket = true;
                continue;
            }
            // After '[', the next non-']' node is the size
            if found_open_bracket && child.kind() != "]" {
                size_node = Some(child);
                break;
            }
        }
    }

    let size_node = size_node?;
    let size_text = &source[size_node.start_byte()..size_node.end_byte()];

    // Check if size is a variable (VLA) - not a number literal
    let is_vla = size_node.kind() == "identifier" ||
                 size_node.kind() == "call_expression" ||
                 size_node.kind() == "binary_expression" ||
                 (size_node.kind() != "number_literal" && !size_text.chars().all(|c| c.is_numeric()));

    if !is_vla {
        // Check if it's a constant 0
        if size_text == "0" {
            let start_point = declarator.start_position();
            return Some(RuleViolation {
                rule_id: "ARR00-C".to_string(),
                severity: Severity::High,
                message: "Array declared with size 0. Zero-length arrays have undefined behavior.".to_string(),
                file_path: String::new(),
                line: start_point.row + 1,
                column: start_point.column + 1,
                suggestion: Some("Use a positive constant size or validate variable size before declaration".to_string()),
            });
        }
        return None; // Constant non-zero size is OK
    }

    // For VLAs with variable size, check if the size variable was validated
    // This is a heuristic check - we look for the size identifier
    if size_node.kind() == "identifier" {
        let size_var_name = size_text;

        // Check if this appears to be an unvalidated parameter or variable
        // Look for assignment of 0 or validation checks in the surrounding context
        if let Some(violation) = check_vla_size_validation(node, size_var_name, source, &declarator) {
            return Some(violation);
        }
    }

    None
}

fn check_vla_size_validation(decl_node: &Node, size_var: &str, source: &str, declarator: &Node) -> Option<RuleViolation> {
    // Look backwards in the source to find if size_var was assigned 0 or is unvalidated
    // First, try to find the containing function
    let function_node = find_containing_function(decl_node)?;

    // Get all variable declarations and assignments before this VLA declaration
    let vla_position = decl_node.start_byte();

    // Simple heuristic: check if we can find "size_var = 0" before the VLA
    let function_start = function_node.start_byte();
    let preceding_text = &source[function_start..vla_position];

    // Check for direct assignment of 0
    if preceding_text.contains(&format!("{} = 0", size_var)) ||
       preceding_text.contains(&format!("{}=0", size_var)) {
        let start_point = declarator.start_position();
        return Some(RuleViolation {
            rule_id: "ARR00-C".to_string(),
            severity: Severity::High,
            message: format!(
                "Variable Length Array declared with size '{}' which is assigned 0. VLAs must have positive size.",
                size_var
            ),
            file_path: String::new(),
            line: start_point.row + 1,
            column: start_point.column + 1,
            suggestion: Some("Validate that the size is positive before declaring the VLA".to_string()),
        });
    }

    // Check if it's a function parameter without validation
    // This is a simplified check - in production, we'd need more sophisticated analysis
    if is_function_parameter(&function_node, size_var, source) {
        // Check if there's a validation before the VLA
        if !has_size_validation_before(preceding_text, size_var) {
            let start_point = declarator.start_position();
            return Some(RuleViolation {
                rule_id: "ARR00-C".to_string(),
                severity: Severity::Medium,
                message: format!(
                    "Variable Length Array declared with unvalidated parameter '{}'. Size could be zero or negative.",
                    size_var
                ),
                file_path: String::new(),
                line: start_point.row + 1,
                column: start_point.column + 1,
                suggestion: Some("Add validation: if (size <= 0 || size > MAX_SIZE) return;".to_string()),
            });
        }
    }

    None
}

fn is_function_parameter(function_node: &Node, var_name: &str, source: &str) -> bool {
    // Find parameter list in function
    for i in 0..function_node.child_count() {
        if let Some(child) = function_node.child(i) {
            if child.kind() == "function_declarator" {
                for j in 0..child.child_count() {
                    if let Some(param_list) = child.child(j) {
                        if param_list.kind() == "parameter_list" {
                            let param_text = &source[param_list.start_byte()..param_list.end_byte()];
                            if param_text.contains(var_name) {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    false
}

fn has_size_validation_before(text: &str, size_var: &str) -> bool {
    // Check for common validation patterns
    // if (size == 0), if (size <= 0), if (size < 1), etc.
    // Also check for compound conditions with || or &&

    // Simple patterns
    let simple_patterns = [
        format!("{} == 0", size_var),
        format!("{} <= 0", size_var),
        format!("{} < 1", size_var),
        format!("0 == {}", size_var),
        format!("{}==0", size_var),
        format!("{}<=0", size_var),
        format!("{}<1", size_var),
    ];

    simple_patterns.iter().any(|pattern| text.contains(pattern))
}

fn check_dangerous_functions(node: &Node, source: &str) -> Option<RuleViolation> {
    // Check for calls to functions that demonstrate misunderstanding of array bounds
    let function = node.child_by_field_name("function")?;
    let func_text = &source[function.start_byte()..function.end_byte()];

    // gets() is inherently dangerous - ALWAYS indicates misunderstanding
    // There is NO safe way to use gets() as it has no bounds checking mechanism
    if func_text == "gets" {
        let start_point = node.start_position();
        return Some(RuleViolation {
            rule_id: "ARR00-C".to_string(),
            severity: Severity::Critical,
            message: "Use of gets() demonstrates misunderstanding of array bounds. It is deprecated and has no safe usage.".to_string(),
            file_path: String::new(),
            line: start_point.row + 1,
            column: start_point.column + 1,
            suggestion: Some("Use fgets(buffer, sizeof(buffer), stdin) which respects buffer size".to_string()),
        });
    }

    None
}

fn check_loop_array_access(node: &Node, source: &str) -> Option<RuleViolation> {
    // Check for loops that use unvalidated/uninitialized variables as bounds when accessing arrays
    // Patterns:
    // 1. for (int i = 0; i < user_input; i++) { array[i] = ...; }
    // 2. for (int i = 0; i < uninitialized_var; i++) { array[i] = ...; }

    // Get the loop condition to find the bound variable
    let condition = node.child_by_field_name("condition")?;
    let bound_var = extract_loop_bound_variable(&condition, source)?;

    // Get the loop body
    let body = node.child_by_field_name("body")?;

    // Check if body contains array access
    let has_array_access = contains_array_access(&body);
    if !has_array_access {
        return None;
    }

    // Look backwards in the function to see if bound_var was populated from user input or is uninitialized
    let function_node = find_containing_function(node)?;
    let loop_position = node.start_byte();
    let function_start = function_node.start_byte();
    let preceding_text = &source[function_start..loop_position];

    // Check for scanf/fscanf reading into the bound variable
    if is_user_input_variable(&bound_var, preceding_text) {
        // Check if there's validation before the loop
        if !has_validation_before_loop(&bound_var, preceding_text, loop_position, source) {
            let start_point = node.start_position();
            return Some(RuleViolation {
                rule_id: "ARR00-C".to_string(),
                severity: Severity::High,
                message: format!(
                    "Loop uses unvalidated user input '{}' as bound for array access. This can cause out-of-bounds access.",
                    bound_var
                ),
                file_path: String::new(),
                line: start_point.row + 1,
                column: start_point.column + 1,
                suggestion: Some(format!(
                    "Validate '{}' against array size before using in loop: if ({} < 0 || {} > ARRAY_SIZE) {{ /* error */ }}",
                    bound_var, bound_var, bound_var
                )),
            });
        }
    }
    // Check if the variable is uninitialized
    else if is_uninitialized_variable(&bound_var, preceding_text) {
        let start_point = node.start_position();
        return Some(RuleViolation {
            rule_id: "ARR00-C".to_string(),
            severity: Severity::High,
            message: format!(
                "Loop uses uninitialized variable '{}' as bound for array access. This has indeterminate value and can cause out-of-bounds access.",
                bound_var
            ),
            file_path: String::new(),
            line: start_point.row + 1,
            column: start_point.column + 1,
            suggestion: Some(format!(
                "Initialize '{}' to a valid value before using it in the loop",
                bound_var
            )),
        });
    }

    None
}

fn extract_loop_bound_variable(condition: &Node, source: &str) -> Option<String> {
    // For condition like "i < count", extract "count"
    // Handle binary expressions: i < var, i <= var, var > i, etc.
    if condition.kind() == "binary_expression" {
        let left = condition.child_by_field_name("left")?;
        let right = condition.child_by_field_name("right")?;

        // Check right side first (most common: i < bound)
        if right.kind() == "identifier" {
            return Some(source[right.start_byte()..right.end_byte()].to_string());
        }
        // Check left side (less common: bound > i)
        if left.kind() == "identifier" {
            let text = &source[left.start_byte()..left.end_byte()];
            // Avoid returning the loop variable itself
            if text != "i" && text != "j" && text != "k" {
                return Some(text.to_string());
            }
        }
    }
    None
}

fn contains_array_access(node: &Node) -> bool {
    // Check if this node or any child is a subscript_expression (array access)
    if node.kind() == "subscript_expression" {
        return true;
    }

    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if contains_array_access(&child) {
                return true;
            }
        }
    }

    false
}

fn is_user_input_variable(var_name: &str, preceding_text: &str) -> bool {
    // Check if variable was populated by scanf, fscanf, fgets, or other input functions
    let input_patterns = [
        format!("scanf(\"%d\", &{})", var_name),
        format!("scanf(\"%d\",&{})", var_name),
        format!("scanf ( \"%d\" , &{} )", var_name),
        format!("fscanf(stdin, \"%d\", &{})", var_name),
        format!("scanf(\"%u\", &{})", var_name),
    ];

    // Simple check: does scanf read into this variable?
    input_patterns.iter().any(|pattern| preceding_text.contains(pattern)) ||
    preceding_text.contains(&format!("scanf")) && preceding_text.contains(&format!("&{}", var_name))
}

fn has_validation_before_loop(var_name: &str, preceding_text: &str, loop_pos: usize, source: &str) -> bool {
    // Check if there's validation of var_name between scanf and the loop
    // Look for patterns like: if (count > MAX) or if (count < 0)

    // Find where scanf populated the variable
    if let Some(scanf_pos) = preceding_text.rfind("scanf") {
        let between_scanf_and_loop = &source[scanf_pos..loop_pos];

        // Look for validation patterns
        let validation_patterns = [
            format!("if ({} >", var_name),
            format!("if ({} <", var_name),
            format!("if ({} >=", var_name),
            format!("if ({} <=", var_name),
            format!("if (0 >{}", var_name),
            format!("if (0 <{}", var_name),
        ];

        return validation_patterns.iter().any(|p| between_scanf_and_loop.contains(p));
    }

    false
}

fn is_uninitialized_variable(var_name: &str, preceding_text: &str) -> bool {
    // Check if variable is declared but never initialized
    // Look for patterns like: "int size;" without "size =" before the loop

    // Check if variable is declared (simple heuristic)
    let declaration_patterns = [
        format!("int {};", var_name),
        format!("int {} ;", var_name),
        format!("size_t {};", var_name),
        format!("unsigned {}", var_name),
        format!("long {}", var_name),
    ];

    let is_declared = declaration_patterns.iter().any(|p| preceding_text.contains(p)) ||
                      (preceding_text.contains("int") && preceding_text.contains(var_name) &&
                       !preceding_text.contains(&format!("{}[", var_name))); // Not an array declaration

    if !is_declared {
        return false;
    }

    // Check if it's been assigned a value
    let assignment_patterns = [
        format!("{} =", var_name),
        format!("{}=", var_name),
        format!("&{}", var_name), // scanf with &var means it's initialized by input
    ];

    let is_initialized = assignment_patterns.iter().any(|p| preceding_text.contains(p));

    // Variable is uninitialized if declared but not initialized
    !is_initialized
}

fn check_pointer_arithmetic(node: &Node, source: &str) -> Option<RuleViolation> {
    // Check for pointer arithmetic that obviously exceeds array bounds
    // Pattern: ptr = arr + offset  where offset > array_size

    // binary_expression nodes have "left", "operator", and "right" fields
    let operator_node = node.child_by_field_name("operator")?;
    let operator = &source[operator_node.start_byte()..operator_node.end_byte()];

    // Only check addition (+ creates a pointer offset)
    if operator != "+" {
        return None;
    }

    let left = node.child_by_field_name("left")?;
    let right = node.child_by_field_name("right")?;

    // Right side should be a number literal (constant offset)
    if right.kind() != "number_literal" {
        return None;
    }

    // Get the array name and offset
    let array_name = &source[left.start_byte()..left.end_byte()];
    let offset_text = &source[right.start_byte()..right.end_byte()];

    // Parse the offset as a number
    let offset: usize = match offset_text.parse() {
        Ok(n) => n,
        Err(_) => return None, // Not a constant offset
    };

    // Find the array declaration in the function body to get its size
    let function_node = find_containing_function(node)?;

    // Find the compound_statement (function body) within the function_definition
    let mut body_start = function_node.start_byte();
    for i in 0..function_node.child_count() {
        if let Some(child) = function_node.child(i) {
            if child.kind() == "compound_statement" {
                // Skip the opening brace '{'
                body_start = child.start_byte() + 1;
                break;
            }
        }
    }

    let ptr_position = node.start_byte();
    let preceding_text = &source[body_start..ptr_position];

    // Look for array declaration: type array_name[SIZE]
    if let Some(array_size) = find_array_size(array_name, preceding_text) {
        // Check if offset exceeds the array size
        // Note: arr + size (one past the end) is technically allowed but shouldn't be dereferenced
        if offset > array_size {
            let start_point = node.start_position();
            return Some(RuleViolation {
                rule_id: "ARR00-C".to_string(),
                severity: Severity::High,
                message: format!(
                    "Pointer arithmetic '{}' goes {} elements past the end of array '{}[{}]'. This exceeds array bounds.",
                    &source[node.start_byte()..node.end_byte()],
                    offset - array_size,
                    array_name,
                    array_size
                ),
                file_path: String::new(),
                line: start_point.row + 1,
                column: start_point.column + 1,
                suggestion: Some(format!(
                    "Ensure pointer arithmetic stays within array bounds (0 to {})",
                    array_size
                )),
            });
        }
    }

    None
}

fn find_array_size(array_name: &str, preceding_text: &str) -> Option<usize> {
    // Look for array declaration patterns: type array_name[SIZE] = ...
    // Examples: int arr[5], char buf[100]

    // Search for "array_name[" pattern to find the array declaration
    let pattern = format!("{}[", array_name);
    if let Some(pos) = preceding_text.rfind(&pattern) {
        // Look for the closing bracket
        let after_name = &preceding_text[pos..];
        if let Some(bracket_start) = after_name.find('[') {
            if let Some(bracket_end) = after_name.find(']') {
                if bracket_end > bracket_start {
                    let size_text = &after_name[bracket_start + 1..bracket_end].trim();
                    // Try to parse as a number
                    if let Ok(size) = size_text.parse::<usize>() {
                        return Some(size);
                    }
                }
            }
        }
    }

    None
}

// ============================================================================
// Array Comparison Checks
// ============================================================================

fn check_array_comparison(node: &Node, source: &str) -> Option<RuleViolation> {
    // Check for array comparisons using == or !=
    let operator = node.child_by_field_name("operator")?;
    let op_text = &source[operator.start_byte()..operator.end_byte()];

    if op_text != "==" && op_text != "!=" {
        return None;
    }

    let left = node.child_by_field_name("left")?;
    let right = node.child_by_field_name("right")?;

    // Check if either side is an array (heuristic check)
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

    None
}

// ============================================================================
// AST Traversal Helpers
// ============================================================================

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

// ============================================================================
// Type and Node Classification Helpers
// ============================================================================

fn is_array_parameter_type(param_type: &str) -> bool {
    // Check if parameter type indicates an array
    // Note: This is a heuristic check without full type information
    param_type.contains('[') ||
    (param_type.contains("*") && !param_type.contains("const char *")) // Avoid false positives on string literals
}

fn is_array_identifier(node: &Node, _source: &str) -> bool {
    // Heuristic check if identifier could be an array
    // Limitation: Without symbol table, we cannot definitively determine array types
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

        // Should not flag safe operations (no High/Critical violations expected)
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
    int arr1[5], arr2[5];
    if (1) {
        arr1 = arr2;  // Should detect in nested block
    }
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        assert!(!violations.is_empty(), "Should detect violations in nested contexts");
    }

    #[test]
    fn test_arr00c_detects_zero_size_vla() {
        let rule = Arr00C;
        let mut parser = CParser::new().unwrap();

        let source = r#"
int main() {
    int size = 0;
    int vla[size];  // Should trigger violation - VLA with size 0

    vla[0] = 100;

    return 0;
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        assert!(!violations.is_empty(), "Should detect VLA with zero size");
        let vla_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("size") || v.message.contains("0"))
            .collect();
        assert!(!vla_violations.is_empty(), "Should detect VLA size issue");
    }

    #[test]
    fn test_arr00c_detects_unvalidated_vla() {
        let rule = Arr00C;
        let mut parser = CParser::new().unwrap();

        let source = r#"
void create_vla(int size) {
    int vla[size];  // Should trigger violation - unvalidated parameter

    for (int i = 0; i < size; i++) {
        vla[i] = i;
    }
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        assert!(!violations.is_empty(), "Should detect unvalidated VLA parameter");
        let vla_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("unvalidated") || v.message.contains("parameter"))
            .collect();
        assert!(!vla_violations.is_empty(), "Should detect unvalidated VLA");
    }

    #[test]
    fn test_arr00c_allows_validated_vla() {
        let rule = Arr00C;
        let mut parser = CParser::new().unwrap();

        let source = r#"
void process_vla(int n) {
    if (n <= 0 || n > 1000) {
        return;
    }

    int vla[n];  // Should be OK - size is validated

    for (int i = 0; i < n; i++) {
        vla[i] = i;
    }
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        // Should not flag validated VLA
        let vla_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("VLA") || v.message.contains("Variable Length"))
            .collect();
        assert!(vla_violations.is_empty(), "Should not flag validated VLA");
    }

    #[test]
    fn test_arr00c_detects_gets_usage() {
        let rule = Arr00C;
        let mut parser = CParser::new().unwrap();

        let source = r#"
#include <stdio.h>

int main() {
    char buffer[50];

    printf("Enter input: ");
    gets(buffer);  // Should trigger critical violation

    printf("You entered: %s\n", buffer);

    return 0;
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        assert!(!violations.is_empty(), "Should detect gets() usage");
        let gets_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("gets"))
            .collect();
        assert!(!gets_violations.is_empty(), "Should detect gets() as dangerous");
        assert!(matches!(gets_violations[0].severity, Severity::Critical));
    }

    #[test]
    fn test_arr00c_allows_safe_and_validated_string_operations() {
        let rule = Arr00C;
        let mut parser = CParser::new().unwrap();

        let source = r#"
#include <stdio.h>
#include <string.h>

void func() {
    char dest[100];
    char src[50];

    // Safe bounded operations - these are OK
    strncpy(dest, src, sizeof(dest) - 1);
    dest[sizeof(dest) - 1] = '\0';

    strncat(dest, src, sizeof(dest) - strlen(dest) - 1);

    snprintf(dest, sizeof(dest), "%s", src);

    fgets(dest, sizeof(dest), stdin);

    // Validated strcpy - shows understanding of arrays
    if (strlen(src) < sizeof(dest)) {
        strcpy(dest, src);
    }
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        // Should not flag safe/validated operations
        // (strcpy/strcat/sprintf with validation shows understanding - covered by ARR38-C)
        let dangerous_violations: Vec<_> = violations.iter()
            .filter(|v| matches!(v.severity, Severity::High | Severity::Critical))
            .collect();
        assert!(dangerous_violations.is_empty(), "Should not flag safe or validated string operations");
    }

    #[test]
    fn test_arr00c_detects_unvalidated_input_loop() {
        let rule = Arr00C;
        let mut parser = CParser::new().unwrap();

        let source = r#"
#include <stdio.h>

int main() {
    int data[100];
    int count;

    printf("How many numbers? ");
    scanf("%d", &count);

    for (int i = 0; i < count; i++) {
        scanf("%d", &data[i]);
    }

    return 0;
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        assert!(!violations.is_empty(), "Should detect unvalidated user input in loop");
        let input_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("unvalidated") && v.message.contains("count"))
            .collect();
        assert!(!input_violations.is_empty(), "Should detect 'count' as unvalidated");
    }

    #[test]
    fn test_arr00c_allows_validated_input_loop() {
        let rule = Arr00C;
        let mut parser = CParser::new().unwrap();

        let source = r#"
#include <stdio.h>

#define MAX_SIZE 100

int main() {
    int data[MAX_SIZE];
    int count;

    printf("How many numbers? ");
    scanf("%d", &count);

    if (count < 0 || count > MAX_SIZE) {
        printf("Invalid count\n");
        return 1;
    }

    for (int i = 0; i < count; i++) {
        scanf("%d", &data[i]);
    }

    return 0;
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        // Should not flag validated input
        let input_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("unvalidated"))
            .collect();
        assert!(input_violations.is_empty(), "Should not flag validated user input");
    }

    #[test]
    fn test_arr00c_detects_uninitialized_loop_bound() {
        let rule = Arr00C;
        let mut parser = CParser::new().unwrap();

        let source = r#"
#include <stdio.h>

int main() {
    int size;
    int arr[10];

    for (int i = 0; i < size; i++) {
        arr[i] = i;
    }

    return 0;
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        assert!(!violations.is_empty(), "Should detect uninitialized variable in loop");
        let uninitialized_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("uninitialized") && v.message.contains("size"))
            .collect();
        assert!(!uninitialized_violations.is_empty(), "Should detect 'size' as uninitialized");
    }

    #[test]
    fn test_arr00c_allows_initialized_loop_bound() {
        let rule = Arr00C;
        let mut parser = CParser::new().unwrap();

        let source = r#"
#include <stdio.h>

int main() {
    int size = 10;
    int arr[10];

    for (int i = 0; i < size; i++) {
        arr[i] = i;
    }

    return 0;
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        // Should not flag initialized variable
        let uninitialized_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("uninitialized"))
            .collect();
        assert!(uninitialized_violations.is_empty(), "Should not flag initialized variable");
    }

    #[test]
    fn test_arr00c_detects_pointer_past_end() {
        let rule = Arr00C;
        let mut parser = CParser::new().unwrap();

        let source = r#"
#include <stdio.h>

int main() {
    int arr[5] = {1, 2, 3, 4, 5};
    int *ptr = arr;

    ptr = arr + 10;  // Should trigger - way past end
    *ptr = 100;

    return 0;
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        assert!(!violations.is_empty(), "Should detect pointer past array end");
        let pointer_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("past the end") || v.message.contains("exceeds"))
            .collect();
        assert!(!pointer_violations.is_empty(), "Should detect pointer arithmetic violation");
    }

    #[test]
    fn test_arr00c_allows_valid_pointer_arithmetic() {
        let rule = Arr00C;
        let mut parser = CParser::new().unwrap();

        let source = r#"
#include <stdio.h>

int main() {
    int arr[10] = {0};
    int *ptr;

    // Valid pointer arithmetic within bounds
    ptr = arr + 5;
    *ptr = 42;

    // One past the end is allowed (but shouldn't dereference)
    ptr = arr + 10;

    return 0;
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        // Should not flag valid pointer arithmetic (arr + 5 for arr[10])
        // Note: arr + 10 is one-past-the-end which is allowed (just can't dereference)
        let pointer_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("past the end") || v.message.contains("exceeds"))
            .collect();
        assert!(pointer_violations.is_empty(), "Should not flag valid pointer arithmetic");
    }
}
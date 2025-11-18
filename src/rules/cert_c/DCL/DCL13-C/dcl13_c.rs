use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils;
use tree_sitter::Node;

pub struct Dcl13C;

impl CertRule for Dcl13C {
    fn rule_id(&self) -> &'static str {
        "DCL13-C"
    }

    fn description(&self) -> &'static str {
        "Declare function parameters that are pointers to values not changed by the function as const"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "DCL13-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Check all function definitions and declarations
        check_functions_recursively(node, source, &mut violations, self.rule_id());

        violations
    }
}

/// Recursively check all function definitions and declarations in the AST
fn check_functions_recursively(
    node: &Node,
    source: &str,
    violations: &mut Vec<RuleViolation>,
    rule_id: &str,
) {
    if node.kind() == "function_definition" {
        check_function_definition(node, source, violations, rule_id);
    } else if node.kind() == "declaration" {
        // Check for function declarations (prototypes)
        check_function_declaration(node, source, violations, rule_id);
    }

    // Recursively check children
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            check_functions_recursively(&child, source, violations, rule_id);
        }
    }
}

/// Check a function definition for const-correctness of pointer parameters
fn check_function_definition(
    func_node: &Node,
    source: &str,
    violations: &mut Vec<RuleViolation>,
    rule_id: &str,
) {
    // Extract function parameters with their const-qualification status
    let params = extract_function_parameters(func_node, source);

    // Find the function body
    let body = find_compound_statement(func_node);

    // For each pointer parameter, check if it's modified in the function body
    for (param_name, is_const, is_pointer, line, col) in params {
        if !is_pointer {
            continue; // Only check pointer parameters
        }

        // Check if this parameter is modified in the function body
        let is_modified = if let Some(body_node) = body {
            is_pointer_param_modified(&body_node, &param_name, source)
        } else {
            false // No body, assume not modified
        };

        if is_modified {
            // Case 1: Pointer parameter is being modified through dereference
            // This is a violation because it creates side effects visible outside the function
            violations.push(RuleViolation {
                rule_id: rule_id.to_string(),
                severity: Severity::Low,
                message: format!(
                    "Function modifies value through pointer parameter '{}' - consider using const or avoiding modification",
                    param_name
                ),
                file_path: String::new(),
                line,
                column: col,
                suggestion: Some(format!(
                    "Declare parameter as 'const <type> *{}' to prevent modifications, or document this as an output parameter",
                    param_name
                )),
                ..Default::default()
            });
        } else if !is_const {
            // Case 2: non-const pointer parameter that is never modified (should be const)
            violations.push(RuleViolation {
                rule_id: rule_id.to_string(),
                severity: Severity::Low,
                message: format!(
                    "Pointer parameter '{}' is not modified and should be declared const",
                    param_name
                ),
                file_path: String::new(),
                line,
                column: col,
                suggestion: Some(format!(
                    "Declare parameter as 'const <type> *{}'",
                    param_name
                )),
                ..Default::default()
            });
        }
    }
}

/// Check a function declaration for const-correctness of pointer parameters
fn check_function_declaration(
    decl_node: &Node,
    source: &str,
    violations: &mut Vec<RuleViolation>,
    rule_id: &str,
) {
    // Look for function declarators in the declaration
    for i in 0..decl_node.child_count() {
        if let Some(child) = decl_node.child(i) {
            if child.kind() == "function_declarator" || is_function_declarator(&child) {
                // For function declarations (no body), we can only check basic patterns
                // We'll flag non-const pointer parameters as potential issues
                let params = extract_params_from_declarator(&child, source);

                for (param_name, is_const, is_pointer, line, col) in params {
                    if is_pointer && !is_const && !param_name.is_empty() {
                        // Only flag if it's a clear case where const would be appropriate
                        // (e.g., second parameter of strcat-like functions)
                        // For declarations without bodies, we can't analyze usage,
                        // so we'll be conservative and only flag obvious cases

                        // Check if this looks like a read-only parameter by naming convention
                        // (src, source, input, etc.) or position (second param in string functions)
                        if is_likely_readonly_param(&param_name) {
                            violations.push(RuleViolation {
                                rule_id: rule_id.to_string(),
                                severity: Severity::Low,
                                message: format!(
                                    "Pointer parameter '{}' should likely be declared const",
                                    param_name
                                ),
                                file_path: String::new(),
                                line,
                                column: col,
                                suggestion: Some(format!(
                                    "Consider declaring parameter as 'const <type> *{}'",
                                    param_name
                                )),
                                ..Default::default()
                            });
                        }
                    }
                }
            }
        }
    }
}

/// Check if a parameter name suggests it should be read-only
fn is_likely_readonly_param(name: &str) -> bool {
    let lowercase = name.to_lowercase();
    lowercase.starts_with("src")
        || lowercase.starts_with("source")
        || lowercase.starts_with("input")
        || lowercase.starts_with("in_")
        || lowercase.contains("read")
        || name.ends_with("2") // Common convention for second string parameter (e.g., s2 in strcat)
}

/// Check if a node is a function declarator (recursively checking pointer/array decorators)
fn is_function_declarator(node: &Node) -> bool {
    if node.kind() == "function_declarator" {
        return true;
    }

    // Check children for nested declarators
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if is_function_declarator(&child) {
                return true;
            }
        }
    }

    false
}

/// Extract parameters from a function declarator
fn extract_params_from_declarator(
    declarator: &Node,
    source: &str,
) -> Vec<(String, bool, bool, usize, usize)> {
    let mut params = Vec::new();

    // Find parameter_list
    if let Some(param_list) = find_parameter_list(declarator) {
        for i in 0..param_list.child_count() {
            if let Some(param) = param_list.child(i) {
                if param.kind() == "parameter_declaration" {
                    if let Some((name, is_const, is_pointer, line, col)) =
                        analyze_parameter(&param, source)
                    {
                        params.push((name, is_const, is_pointer, line, col));
                    }
                }
            }
        }
    }

    params
}

/// Find parameter_list in a function declarator
fn find_parameter_list<'a>(declarator: &Node<'a>) -> Option<Node<'a>> {
    for i in 0..declarator.child_count() {
        if let Some(child) = declarator.child(i) {
            if child.kind() == "parameter_list" {
                return Some(child);
            }
            // Recursively search in nested declarators
            if let Some(found) = find_parameter_list(&child) {
                return Some(found);
            }
        }
    }
    None
}

/// Check if a pointer parameter is modified in the function body
fn is_pointer_param_modified(body: &Node, param_name: &str, source: &str) -> bool {
    // Look for assignment expressions that modify *param_name
    check_node_for_pointer_modification(body, param_name, source)
}

/// Recursively check if a node contains modifications to the dereferenced pointer
fn check_node_for_pointer_modification(node: &Node, param_name: &str, source: &str) -> bool {
    // Check if this is an assignment to *param_name
    if node.kind() == "assignment_expression" {
        if let Some(left) = node.child_by_field_name("left") {
            // Check if left side is a pointer dereference of our parameter
            if is_pointer_dereference_of_param(&left, param_name, source) {
                return true;
            }
        }
    }

    // Check for increment/decrement of *param_name
    if node.kind() == "update_expression" {
        if let Some(argument) = node.child_by_field_name("argument") {
            if is_pointer_dereference_of_param(&argument, param_name, source) {
                return true;
            }
        }
    }

    // Recursively check children
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if check_node_for_pointer_modification(&child, param_name, source) {
                return true;
            }
        }
    }

    false
}

/// Check if a node is a pointer dereference of a specific parameter (e.g., *x)
fn is_pointer_dereference_of_param(node: &Node, param_name: &str, source: &str) -> bool {
    if node.kind() == "pointer_expression" {
        // Get the argument of the dereference
        if let Some(argument) = node.child_by_field_name("argument") {
            let text = ast_utils::get_node_text(&argument, source);
            return text == param_name;
        }
    }
    false
}

/// Extract function parameters with const-qualification and pointer status
///
/// Returns: Vec<(parameter_name, is_const, is_pointer, line_number, column_number)>
fn extract_function_parameters(
    func_node: &Node,
    source: &str,
) -> Vec<(String, bool, bool, usize, usize)> {
    let mut parameters = Vec::new();

    // Find the function_declarator
    for i in 0..func_node.child_count() {
        if let Some(child) = func_node.child(i) {
            if child.kind() == "function_declarator"
                || is_pointer_or_array_with_func_declarator(&child)
            {
                if let Some(param_list) = find_parameter_list(&child) {
                    // Extract each parameter
                    for j in 0..param_list.child_count() {
                        if let Some(param) = param_list.child(j) {
                            if param.kind() == "parameter_declaration" {
                                if let Some((name, is_const, is_pointer, line, col)) =
                                    analyze_parameter(&param, source)
                                {
                                    parameters.push((name, is_const, is_pointer, line, col));
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    parameters
}

/// Check if node is a pointer/array declarator that contains a function declarator
fn is_pointer_or_array_with_func_declarator(node: &Node) -> bool {
    if node.kind() == "pointer_declarator" || node.kind() == "array_declarator" {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "function_declarator"
                    || is_pointer_or_array_with_func_declarator(&child)
                {
                    return true;
                }
            }
        }
    }
    false
}

/// Analyze a parameter declaration to extract name, const status, and pointer status
///
/// Returns: Some((name, is_const, is_pointer, line, col)) or None
fn analyze_parameter(param: &Node, source: &str) -> Option<(String, bool, bool, usize, usize)> {
    let mut is_const = false;
    let mut is_pointer = false;
    let mut param_name = String::new();
    let mut line = 0;
    let mut col = 0;

    // Check for type qualifiers (const)
    for i in 0..param.child_count() {
        if let Some(child) = param.child(i) {
            match child.kind() {
                "type_qualifier" => {
                    let text = ast_utils::get_node_text(&child, source);
                    if text == "const" {
                        is_const = true;
                    }
                }
                "pointer_declarator" => {
                    is_pointer = true;
                    param_name = ast_utils::get_identifier_from_declarator(&child, source);
                    if !param_name.is_empty() {
                        let pos = child.start_position();
                        line = pos.row + 1;
                        col = pos.column + 1;
                    }
                }
                "array_declarator" => {
                    is_pointer = true; // Arrays decay to pointers in function parameters
                    param_name = ast_utils::get_identifier_from_declarator(&child, source);
                    if !param_name.is_empty() {
                        let pos = child.start_position();
                        line = pos.row + 1;
                        col = pos.column + 1;
                    }
                }
                "identifier" => {
                    // Direct identifier (might be non-pointer parameter)
                    if param_name.is_empty() {
                        param_name = ast_utils::get_node_text(&child, source).to_string();
                        let pos = child.start_position();
                        line = pos.row + 1;
                        col = pos.column + 1;
                    }
                }
                "primitive_type" | "type_identifier" | "struct_specifier" | "union_specifier"
                | "enum_specifier" => {
                    // Type specifier - check if followed by pointer
                    if i + 1 < param.child_count() {
                        if let Some(next) = param.child(i + 1) {
                            if next.kind() == "*" || next.kind() == "abstract_pointer_declarator" {
                                is_pointer = true;
                            }
                        }
                    }
                }
                _ => {}
            }
        }
    }

    if !param_name.is_empty() {
        Some((param_name, is_const, is_pointer, line, col))
    } else {
        None
    }
}

/// Find the compound_statement (body) of a function
fn find_compound_statement<'a>(func_node: &Node<'a>) -> Option<Node<'a>> {
    for i in 0..func_node.child_count() {
        if let Some(child) = func_node.child(i) {
            if child.kind() == "compound_statement" {
                return Some(child);
            }
        }
    }
    None
}

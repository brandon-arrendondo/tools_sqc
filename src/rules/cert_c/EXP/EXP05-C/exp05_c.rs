use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Exp05C;

impl CertRule for Exp05C {
    fn rule_id(&self) -> &'static str {
        "EXP05-C"
    }

    fn description(&self) -> &'static str {
        "Do not cast away a const qualification"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "EXP05-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Check for explicit casts that remove const qualification
        if node.kind() == "cast_expression" {
            if let Some(type_node) = node.child_by_field_name("type") {
                if let Some(value_node) = node.child_by_field_name("value") {
                    // Get the target type (what we're casting to)
                    let target_type = get_node_text(&type_node, source);

                    // Check if we're casting away const
                    // Target type should be a non-const pointer
                    if is_pointer_type(&target_type) && !target_type.contains("const") {
                        // Check if the value refers to a const-qualified variable
                        if is_value_const_qualified(&value_node, node, source) {
                            report_violation(node, source, &mut violations);
                        }
                    }
                }
            }
        }

        // Check for implicit const removal in function calls
        if node.kind() == "call_expression" {
            if let Some(function_node) = node.child_by_field_name("function") {
                let function_name = get_node_text(&function_node, source);

                // Check known functions that take non-const pointers
                if is_modifying_function(function_name) {
                    if let Some(arguments) = node.child_by_field_name("arguments") {
                        check_function_arguments(&arguments, node, source, &mut violations);
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

/// Check if a type is a pointer type
fn is_pointer_type(type_str: &str) -> bool {
    type_str.contains('*')
}

/// Check if a value refers to a const-qualified variable/parameter
fn is_value_const_qualified(value_node: &Node, context: &Node, source: &str) -> bool {
    match value_node.kind() {
        "identifier" => {
            let id_name = get_node_text(value_node, source);
            // Search for the declaration in the function scope
            find_const_declaration(id_name, context, source)
        }
        _ => false,
    }
}

/// Search for a const declaration of the given identifier
fn find_const_declaration(id_name: &str, context: &Node, source: &str) -> bool {
    // Find the containing function
    let mut current = Some(*context);
    while let Some(node) = current {
        if node.kind() == "function_definition" {
            // Search parameters for const qualification
            if let Some(declarator) = node.child_by_field_name("declarator") {
                if check_params_for_const(&declarator, id_name, source) {
                    return true;
                }
            }

            // Search variable declarations in the function body
            if let Some(body) = node.child_by_field_name("body") {
                if check_body_for_const(&body, id_name, source) {
                    return true;
                }
            }

            break;
        }
        current = node.parent();
    }

    // Also check for global const declarations
    if let Some(root) = get_translation_unit(context) {
        check_body_for_const(&root, id_name, source)
    } else {
        false
    }
}

/// Get the translation unit (root) node
fn get_translation_unit<'a>(node: &Node<'a>) -> Option<Node<'a>> {
    let mut current = Some(*node);
    while let Some(n) = current {
        if n.kind() == "translation_unit" {
            return Some(n);
        }
        current = n.parent();
    }
    None
}

/// Check function parameters for const qualification
fn check_params_for_const(declarator: &Node, id_name: &str, source: &str) -> bool {
    for i in 0..declarator.child_count() {
        if let Some(child) = declarator.child(i) {
            if child.kind() == "parameter_list" {
                return check_param_list_for_const(&child, id_name, source);
            }
            // Recursively search in nested declarators
            if check_params_for_const(&child, id_name, source) {
                return true;
            }
        }
    }
    false
}

/// Check parameter list for const qualification
fn check_param_list_for_const(param_list: &Node, id_name: &str, source: &str) -> bool {
    for i in 0..param_list.child_count() {
        if let Some(param) = param_list.child(i) {
            if param.kind() == "parameter_declaration" {
                let param_text = get_node_text(&param, source);
                // Check if this parameter has const and matches the identifier
                if param_text.contains("const") && param_text.contains(id_name) {
                    return true;
                }
            }
        }
    }
    false
}

/// Check function body or scope for const declarations
fn check_body_for_const(body: &Node, id_name: &str, source: &str) -> bool {
    for i in 0..body.child_count() {
        if let Some(child) = body.child(i) {
            if child.kind() == "declaration" {
                let decl_text = get_node_text(&child, source);
                // Check if this declaration has const and matches the identifier
                if decl_text.contains("const") && decl_text.contains(id_name) {
                    return true;
                }
            }
            // Recursively search nested scopes, but NOT into other function
            // definitions — those have their own scope and their const parameters
            // don't apply here
            if child.kind() != "function_definition" {
                if check_body_for_const(&child, id_name, source) {
                    return true;
                }
            }
        }
    }
    false
}

/// Check if a function name is a known function that modifies its arguments
fn is_modifying_function(name: &str) -> bool {
    matches!(
        name,
        "memset"
            | "memcpy"
            | "memmove"
            | "strcpy"
            | "strncpy"
            | "strcat"
            | "strncat"
            | "sprintf"
            | "snprintf"
            | "gets"
            | "fgets"
            | "scanf"
            | "fscanf"
            | "sscanf"
    )
}

/// Check function arguments for const-qualified values
fn check_function_arguments(
    arguments: &Node,
    context: &Node,
    source: &str,
    violations: &mut Vec<RuleViolation>,
) {
    for i in 0..arguments.child_count() {
        if let Some(arg) = arguments.child(i) {
            // Skip punctuation like commas and parentheses
            if arg.kind() == "," || arg.kind() == "(" || arg.kind() == ")" {
                continue;
            }

            // Check if this argument is a const-qualified identifier
            if is_const_qualified_argument(&arg, context, source) {
                report_violation(&arg, source, violations);
                return; // Only report once per function call
            }
        }
    }
}

/// Check if an argument is const-qualified
fn is_const_qualified_argument(node: &Node, context: &Node, source: &str) -> bool {
    match node.kind() {
        "identifier" => {
            let id_name = get_node_text(node, source);
            find_const_declaration(id_name, context, source)
        }
        // Field expressions (e.g., ptr->buffer): the base pointer may be const-qualified
        // but the member itself may not be. We can't determine member const-qualification
        // without struct definitions, so skip these to avoid false positives.
        "field_expression" => false,
        _ => {
            // Recursively check for identifiers in the argument
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if is_const_qualified_argument(&child, context, source) {
                        return true;
                    }
                }
            }
            false
        }
    }
}

/// Report a violation for casting away const
fn report_violation(node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
    let start_point = node.start_position();
    let node_text = get_node_text(node, source);

    violations.push(RuleViolation {
        rule_id: "EXP05-C".to_string(),
        severity: Severity::Medium,
        message: format!("Do not cast away const qualification: '{}'", node_text),
        file_path: String::new(),
        line: start_point.row + 1,
        column: start_point.column + 1,
        suggestion: Some(
            "Ensure const-qualified objects are not modified through cast-away pointers"
                .to_string(),
        ),
        ..Default::default()
    });
}

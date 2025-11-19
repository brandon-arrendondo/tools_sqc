use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils;
use tree_sitter::Node;

pub struct Dcl17C;

impl CertRule for Dcl17C {
    fn rule_id(&self) -> &'static str {
        "DCL17-C"
    }

    fn description(&self) -> &'static str {
        "Beware of miscompiled volatile-qualified variables"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "DCL17-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Check for direct access to volatile variables
        // Direct access can be miscompiled; should use function wrappers
        if is_direct_volatile_access(node, source) {
            let var_name = ast_utils::get_node_text(node, source);
            let start_point = node.start_position();

            violations.push(RuleViolation {
                rule_id: self.rule_id().to_string(),
                severity: self.severity(),
                message: format!(
                    "Direct access to volatile variable '{}' may be miscompiled. Use function wrappers for volatile accesses",
                    var_name
                ),
                file_path: String::new(),
                line: start_point.row + 1,
                column: start_point.column + 1,
                suggestion: Some(
                    "Wrap volatile accesses in functions: vol_read(volatile T *p) { return *p; }"
                        .to_string(),
                ),
                ..Default::default()
            });
        }

        violations
    }
}

/// Checks if a node represents a direct access to a volatile variable
/// Direct accesses should be wrapped in function calls to avoid compiler bugs
fn is_direct_volatile_access(node: &Node, source: &str) -> bool {
    // Check if this is an identifier in an expression context
    if node.kind() != "identifier" {
        return false;
    }

    // Get the variable name
    let var_name = ast_utils::get_node_text(node, source);

    // Check if parent context suggests this is a direct access
    let parent = match node.parent() {
        Some(p) => p,
        None => return false,
    };

    // If the identifier is part of a function call, it's likely wrapped (compliant)
    if is_within_function_call(node) {
        return false;
    }

    // If the identifier is being addressed (&var), it might be for a wrapper function
    if parent.kind() == "unary_expression" {
        let operator = parent.child_by_field_name("operator");
        if let Some(op) = operator {
            let op_text = ast_utils::get_node_text(&op, source);
            if op_text == "&" {
                return false; // Taking address for function wrapper
            }
        }
    }

    // Check if this variable is declared as volatile
    if !is_variable_volatile(&var_name, node, source) {
        return false;
    }

    // Check if this is a direct read or write context
    is_direct_access_context(node)
}

/// Checks if a node is within a function call (indicates wrapped access)
fn is_within_function_call(node: &Node) -> bool {
    let mut current = node.parent();
    while let Some(parent) = current {
        if parent.kind() == "call_expression" {
            // Check if node is the function name being called
            if let Some(func) = parent.child_by_field_name("function") {
                if func.id() == node.id() {
                    return false; // This is the function name, not an argument
                }
            }
            return true; // Node is within function call arguments
        }
        current = parent.parent();
    }
    false
}

/// Checks if a variable is declared with volatile qualifier
fn is_variable_volatile(var_name: &str, node: &Node, source: &str) -> bool {
    // Walk up the tree to find the translation unit (root)
    let mut current = Some(*node);
    while let Some(n) = current {
        if n.kind() == "translation_unit" {
            return find_volatile_declaration(&n, var_name, source);
        }
        current = n.parent();
    }
    false
}

/// Searches for a volatile declaration of the given variable
fn find_volatile_declaration(root: &Node, var_name: &str, source: &str) -> bool {
    // Search for declarations in the translation unit
    let mut cursor = root.walk();
    let mut stack = vec![*root];

    while let Some(node) = stack.pop() {
        if node.kind() == "declaration" {
            if has_volatile_qualifier(&node, source) {
                // Check if this declaration includes our variable
                if declaration_includes_variable(&node, var_name, source) {
                    return true;
                }
            }
        }

        // Add children to stack for traversal
        cursor.reset(node);
        if cursor.goto_first_child() {
            loop {
                stack.push(cursor.node());
                if !cursor.goto_next_sibling() {
                    break;
                }
            }
        }
    }
    false
}

/// Checks if a declaration has the volatile qualifier
fn has_volatile_qualifier(decl_node: &Node, source: &str) -> bool {
    let mut cursor = decl_node.walk();
    cursor.reset(*decl_node);

    if cursor.goto_first_child() {
        loop {
            let child = cursor.node();
            if child.kind() == "type_qualifier" {
                let text = ast_utils::get_node_text(&child, source);
                if text == "volatile" {
                    return true;
                }
            }
            if !cursor.goto_next_sibling() {
                break;
            }
        }
    }
    false
}

/// Checks if a declaration node declares the given variable
fn declaration_includes_variable(decl_node: &Node, var_name: &str, source: &str) -> bool {
    let mut cursor = decl_node.walk();
    cursor.reset(*decl_node);

    if cursor.goto_first_child() {
        loop {
            let child = cursor.node();
            if child.kind() == "init_declarator" || child.kind() == "identifier" {
                let text = extract_variable_name(&child, source);
                if text == var_name {
                    return true;
                }
            }
            if !cursor.goto_next_sibling() {
                break;
            }
        }
    }
    false
}

/// Extracts the variable name from a declarator node
fn extract_variable_name<'a>(node: &Node, source: &'a str) -> &'a str {
    if node.kind() == "identifier" {
        return ast_utils::get_node_text(node, source);
    }

    // For init_declarator, find the identifier child
    let mut cursor = node.walk();
    cursor.reset(*node);

    if cursor.goto_first_child() {
        loop {
            let child = cursor.node();
            if child.kind() == "identifier" {
                return ast_utils::get_node_text(&child, source);
            }
            if child.kind() == "init_declarator" || child.kind() == "pointer_declarator" {
                let result = extract_variable_name(&child, source);
                if !result.is_empty() {
                    return result;
                }
            }
            if !cursor.goto_next_sibling() {
                break;
            }
        }
    }
    ""
}

/// Checks if the node is in a direct access context (assignment, comparison, etc.)
fn is_direct_access_context(node: &Node) -> bool {
    let parent = match node.parent() {
        Some(p) => p,
        None => return false,
    };

    match parent.kind() {
        // Direct assignment contexts
        "assignment_expression" | "update_expression" => true,
        // Direct comparison or arithmetic
        "binary_expression" | "relational_expression" => true,
        // Direct initialization
        "init_declarator" => true,
        // For loop increment/condition
        "for_statement" => {
            // Check if this is in the initializer, condition, or increment
            if let Some(init) = parent.child_by_field_name("initializer") {
                if is_ancestor(&init, node) {
                    return true;
                }
            }
            if let Some(cond) = parent.child_by_field_name("condition") {
                if is_ancestor(&cond, node) {
                    return true;
                }
            }
            if let Some(update) = parent.child_by_field_name("update") {
                if is_ancestor(&update, node) {
                    return true;
                }
            }
            false
        }
        _ => false,
    }
}

/// Checks if ancestor is an ancestor of descendant in the AST
fn is_ancestor(ancestor: &Node, descendant: &Node) -> bool {
    let mut current = Some(*descendant);
    while let Some(node) = current {
        if node.id() == ancestor.id() {
            return true;
        }
        current = node.parent();
    }
    false
}

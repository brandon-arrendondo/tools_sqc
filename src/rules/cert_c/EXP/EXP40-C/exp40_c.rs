use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Exp40C;

impl CertRule for Exp40C {
    fn rule_id(&self) -> &'static str {
        "EXP40-C"
    }

    fn description(&self) -> &'static str {
        "Do not modify constant objects"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "EXP40-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Check for assignments that might remove const qualification
        match node.kind() {
            "assignment_expression" => {
                check_assignment(node, source, &mut violations);
            }
            "init_declarator" => {
                check_init_declarator(node, source, &mut violations);
            }
            "pointer_declarator" => {
                check_pointer_assignment(node, source, &mut violations);
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

/// Check if an assignment removes const qualification
fn check_assignment(node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
    if let (Some(left), Some(right)) = (
        node.child_by_field_name("left"),
        node.child_by_field_name("right"),
    ) {
        let left_text = get_node_text(&left, source);
        let right_text = get_node_text(&right, source);

        // Check if we're assigning through a pointer that removes const
        // Pattern: *ptr = value where ptr might point to const data
        if left_text.starts_with('*') {
            // Get the pointer variable
            let _ptr_var = left_text.trim_start_matches('*').trim();

            // If the RHS contains a const object or const pointer, flag it
            if is_potentially_const_violating(&right, source) {
                report_violation(
                    node,
                    source,
                    violations,
                    &format!(
                        "Potential modification of const object through pointer: '{}' = '{}'",
                        left_text, right_text
                    ),
                );
            }
        }

        // Check for assigning &non_const_ptr to const_ptr_ptr (const T ** = &(T *))
        // Pattern: ipp = &ip where ipp is const int ** and ip is int *
        if left.kind() == "identifier" && right.kind() == "pointer_expression" {
            if let Some(op) = right.child_by_field_name("operator") {
                if get_node_text(&op, source) == "&" {
                    // Check if this identifier was declared with const ** pattern
                    if is_const_pointer_to_pointer_var(&left_text, node, source) {
                        report_violation(
                            node,
                            source,
                            violations,
                            &format!(
                                "Assigning address of non-const pointer to const-qualified pointer-to-pointer: {} = {}",
                                left_text, right_text
                            ),
                        );
                    }
                }
            }
        }
    }
}

/// Check if a variable name was declared as const T **
fn is_const_pointer_to_pointer_var(var_name: &str, node: &Node, source: &str) -> bool {
    // Find the translation_unit (root)
    let mut root = *node;
    while let Some(parent) = root.parent() {
        root = parent;
    }

    // Search for declarations of this variable
    find_const_ptr_ptr_decl(&root, var_name, source)
}

fn find_const_ptr_ptr_decl(node: &Node, var_name: &str, source: &str) -> bool {
    if node.kind() == "declaration" {
        let decl_text = get_node_text(node, source);
        // Check for patterns like "const int **varname" or "const T **varname"
        if decl_text.contains("const") && decl_text.contains("**") && decl_text.contains(var_name) {
            // Make sure const is before ** (not after like int * const *)
            if let Some(const_pos) = decl_text.find("const") {
                if let Some(ptr_ptr_pos) = decl_text.find("**") {
                    if const_pos < ptr_ptr_pos {
                        return true;
                    }
                }
            }
        }
    }

    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if find_const_ptr_ptr_decl(&child, var_name, source) {
                return true;
            }
        }
    }
    false
}

/// Check init_declarator for const removal in initialization
fn check_init_declarator(node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
    // Get declarator and value
    if let Some(declarator) = node.child_by_field_name("declarator") {
        if let Some(value) = node.child_by_field_name("value") {
            // Check if this is a pointer declaration
            if is_pointer_declarator(&declarator) {
                let decl_text = get_node_text(&declarator, source);
                let value_text = get_node_text(&value, source);

                // Check if const is in the parent declaration's type specifiers
                let parent_has_const = node
                    .parent()
                    .filter(|p| p.kind() == "declaration")
                    .is_some_and(|decl| {
                        // Check type specifiers in the declaration for const
                        let mut cursor = decl.walk();
                        for child in decl.children(&mut cursor) {
                            if child.kind() == "type_qualifier" {
                                let q = get_node_text(&child, source);
                                if q == "const" {
                                    return true;
                                }
                            }
                        }
                        false
                    });

                // If the declarator is a non-const pointer but the value is const, flag it
                // Skip if the parent declaration already has const qualifier
                if !parent_has_const
                    && !contains_const_keyword(&declarator, source)
                    && is_const_qualified(&value, source)
                {
                    report_violation(
                        node,
                        source,
                        violations,
                        &format!(
                            "Pointer to const assigned to non-const pointer without cast: {} = {}",
                            decl_text, value_text
                        ),
                    );
                }
            }
        }
    }
}

/// Check pointer assignments in declarations
fn check_pointer_assignment(node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
    let node_text = get_node_text(node, source);

    // Look for patterns like "int **ipp" where const qualification might be violated
    // This is a simplified check - a full implementation would need type tracking
    if node_text.contains("**") && node.parent().is_some() {
        if let Some(parent) = node.parent() {
            if parent.kind() == "init_declarator" {
                if let Some(value) = parent.child_by_field_name("value") {
                    let value_text = get_node_text(&value, source);
                    // Check if we're creating a pointer-to-pointer that could bypass const
                    if value_text.contains('&') && !contains_const_in_pointer_chain(node, source) {
                        // This could be a const bypass - check more carefully
                        check_pointer_to_pointer_const(&parent, source, violations);
                    }
                }
            }
        }
    }
}

/// Check if a node is a pointer declarator
fn is_pointer_declarator(node: &Node) -> bool {
    node.kind() == "pointer_declarator" || (node.kind() == "declarator" && has_pointer_child(node))
}

/// Check if node has a pointer child
fn has_pointer_child(node: &Node) -> bool {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if child.kind() == "pointer_declarator" || child.kind() == "*" {
                return true;
            }
            if has_pointer_child(&child) {
                return true;
            }
        }
    }
    false
}

/// Check if a node or its ancestors contain const keyword
fn contains_const_keyword(node: &Node, source: &str) -> bool {
    // Check current node
    let text = get_node_text(node, source);
    if text.contains("const") {
        return true;
    }

    // Check children
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if child.kind() == "type_qualifier" {
                let qualifier_text = get_node_text(&child, source);
                if qualifier_text == "const" {
                    return true;
                }
            }
            if contains_const_keyword(&child, source) {
                return true;
            }
        }
    }

    false
}

/// Check if a value is const-qualified
fn is_const_qualified(node: &Node, source: &str) -> bool {
    match node.kind() {
        "pointer_expression" => {
            // &const_var
            if let Some(argument) = node.child_by_field_name("argument") {
                return is_const_qualified(&argument, source);
            }
        }
        "identifier" => {
            // We'd need symbol table to know if identifier is const
            // For now, check if we can find const in nearby context
            return false;
        }
        _ => {
            // Check if node has const in its type
            return contains_const_keyword(node, source);
        }
    }
    false
}

/// Check if expression might be violating const
fn is_potentially_const_violating(node: &Node, source: &str) -> bool {
    // Look for address-of expressions (&var) - only flag if argument is const
    if node.kind() == "pointer_expression" {
        if let Some(operator) = node.child_by_field_name("operator") {
            let op_text = get_node_text(&operator, source);
            if op_text == "&" {
                // Only flag if taking address of const-qualified object
                if let Some(argument) = node.child_by_field_name("argument") {
                    return contains_const_keyword(&argument, source);
                }
            }
        }
    }

    // Check for const keyword directly in expression
    contains_const_keyword(node, source)
}

/// Check if pointer declaration chain contains const
fn contains_const_in_pointer_chain(node: &Node, source: &str) -> bool {
    let mut current = *node;

    // Walk up to declaration
    while let Some(parent) = current.parent() {
        if parent.kind() == "declaration" {
            return contains_const_keyword(&parent, source);
        }
        current = parent;
    }

    false
}

/// Check pointer-to-pointer const bypass patterns
fn check_pointer_to_pointer_const(node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
    // Pattern from wiki: const int **ipp; int *ip; ipp = &ip;
    // This allows *ipp to be assigned a const int*, then *ip can modify it

    if let Some(declarator) = node.child_by_field_name("declarator") {
        let decl_text = get_node_text(&declarator, source);
        if let Some(value) = node.child_by_field_name("value") {
            let value_text = get_node_text(&value, source);

            // Check if we have const ** pattern assigned from non-const *
            // This is a simplified heuristic
            if decl_text.contains("**") {
                if value_text.contains('&') && !value_text.contains("const") {
                    report_violation(
                        node,
                        source,
                        violations,
                        &format!(
                            "Pointer-to-pointer assignment may allow const circumvention: {} = {}",
                            decl_text, value_text
                        ),
                    );
                }
            }
        }
    }
}

/// Report a violation
fn report_violation(node: &Node, source: &str, violations: &mut Vec<RuleViolation>, message: &str) {
    let start_point = node.start_position();
    let node_text = get_node_text(node, source);

    violations.push(RuleViolation {
        rule_id: "EXP40-C".to_string(),
        severity: Severity::Low,
        message: format!("{}: '{}'", message, node_text),
        file_path: String::new(),
        line: start_point.row + 1,
        column: start_point.column + 1,
        suggestion: Some(
            "Either remove const qualifier if the object should be modifiable, or use explicit casts to show intentional const removal"
                .to_string(),
        ),
        ..Default::default()
    });
}

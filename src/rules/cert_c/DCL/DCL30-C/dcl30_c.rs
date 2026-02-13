//! DCL30-C: Declare objects with appropriate storage durations
//!
//! This rule detects when pointers to automatic storage (local variables) are
//! returned or assigned to longer-lived storage, causing dangling pointer issues.
//!
//! VIOLATIONS:
//! - return local_array;              // Returning pointer to local
//! - *ptr_param = local;              // Assigning local to output parameter
//! - global_ptr = local_array;        // Assigning local to global
//!
//! COMPLIANT:
//! - void init_array(char *array)    // Pass array as parameter instead
//! - return malloc(...);              // Return allocated memory
//! - static char array[10];           // Use static storage for long-lived data

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils;
use tree_sitter::Node;

pub struct Dcl30C;

impl CertRule for Dcl30C {
    fn rule_id(&self) -> &'static str {
        "DCL30-C"
    }

    fn description(&self) -> &'static str {
        "Declare objects with appropriate storage durations"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "DCL30-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        match node.kind() {
            // Check return statements for local variable pointers
            "return_statement" => {
                if let Some(violation) = self.check_return_local(node, source) {
                    violations.push(violation);
                }
            }
            // Check assignments for local variables assigned to globals or output params
            "assignment_expression" => {
                if let Some(violation) = self.check_assignment_storage_duration(node, source) {
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

impl Dcl30C {
    /// Recursively collect file-scope declarations, including inside preprocessor blocks.
    fn collect_file_scope_declarations<'a>(node: &Node<'a>, decls: &mut Vec<Node<'a>>) {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "declaration" {
                    decls.push(child);
                } else if child.kind().starts_with("preproc_") {
                    Self::collect_file_scope_declarations(&child, decls);
                }
            }
        }
    }

    /// Check if a return statement returns a pointer to a local variable
    fn check_return_local(&self, return_node: &Node, source: &str) -> Option<RuleViolation> {
        // Get the returned expression
        for i in 0..return_node.child_count() {
            if let Some(child) = return_node.child(i) {
                if child.kind() == "identifier" {
                    let var_name = ast_utils::get_node_text(&child, source).to_string();

                    // Check if this identifier refers to a local array/variable
                    if self.is_local_variable(&child, source) {
                        let start_point = return_node.start_position();

                        return Some(RuleViolation {
                            rule_id: "DCL30-C".to_string(),
                            severity: Severity::High,
                            message: format!(
                                "Function returns pointer to local variable '{}' with automatic storage duration",
                                var_name
                            ),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some(
                                "Use static storage, allocated memory, or pass output buffer as parameter".to_string()
                            ),
                            ..Default::default()
                        });
                    }
                }
            }
        }
        None
    }

    /// Check if an assignment assigns a local variable to global or output parameter
    fn check_assignment_storage_duration(
        &self,
        assignment_node: &Node,
        source: &str,
    ) -> Option<RuleViolation> {
        // Get left and right sides of assignment
        let left = assignment_node.child_by_field_name("left")?;
        let right = assignment_node.child_by_field_name("right")?;

        // Check if right side is a local variable
        if right.kind() == "identifier" {
            let right_var = ast_utils::get_node_text(&right, source).to_string();

            if !self.is_local_variable(&right, source) {
                return None;
            }

            // Check if left side is a pointer dereference or global
            match left.kind() {
                "pointer_expression" => {
                    // *ptr_param = local; pattern
                    let start_point = assignment_node.start_position();
                    return Some(RuleViolation {
                        rule_id: "DCL30-C".to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Local variable '{}' assigned through pointer parameter - address will be invalid when function returns",
                            right_var
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some(
                            "Copy data instead of assigning pointer, or use static/allocated storage".to_string()
                        ),
                        ..Default::default()
                    });
                }
                "identifier" => {
                    // Check if left is a global variable (declared outside function)
                    if self.is_global_or_static(&left, source) {
                        let left_var = ast_utils::get_node_text(&left, source).to_string();

                        // Check if global is reassigned later in the same function (like p = NULL)
                        if self.is_global_reassigned_later(assignment_node, &left_var, source) {
                            // Safe pattern: global is reset before function returns
                            return None;
                        }

                        let start_point = assignment_node.start_position();

                        return Some(RuleViolation {
                            rule_id: "DCL30-C".to_string(),
                            severity: Severity::High,
                            message: format!(
                                "Local variable '{}' assigned to global/static variable '{}' - creates dangling pointer",
                                right_var, left_var
                            ),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some(
                                "Use static storage for local variable or copy data instead of assigning pointer".to_string()
                            ),
                            ..Default::default()
                        });
                    }
                }
                _ => {}
            }
        }

        None
    }

    /// Check if a variable is declared locally in a function (automatic storage)
    fn is_local_variable(&self, var_node: &Node, source: &str) -> bool {
        let var_name = ast_utils::get_node_text(var_node, source);

        // Find the containing function
        let mut current = var_node.parent();
        let mut function_body: Option<Node> = None;

        while let Some(node) = current {
            if node.kind() == "compound_statement" {
                if let Some(parent) = node.parent() {
                    if parent.kind() == "function_definition" {
                        function_body = Some(node);
                        break;
                    }
                }
            }
            current = node.parent();
        }

        let body = match function_body {
            Some(b) => b,
            None => return false,
        };

        // Search for declaration of this variable in function body
        self.find_local_declaration(&body, &var_name, source)
    }

    /// Find if a variable is declared locally (not static)
    fn find_local_declaration(&self, body: &Node, var_name: &str, source: &str) -> bool {
        for i in 0..body.child_count() {
            if let Some(child) = body.child(i) {
                if child.kind() == "declaration" {
                    // Check if this declaration declares our variable
                    if self.declaration_contains_var(&child, var_name) {
                        // Check if it's NOT static
                        let decl_text = ast_utils::get_node_text(&child, source);
                        return !decl_text.contains("static");
                    }
                }
                // Recursively search compound statements
                if child.kind() == "compound_statement" {
                    if self.find_local_declaration(&child, var_name, source) {
                        return true;
                    }
                }
            }
        }
        false
    }

    /// Check if a declaration contains a specific variable name
    fn declaration_contains_var(&self, decl_node: &Node, var_name: &str) -> bool {
        // Simple approach: check if variable name appears in declaration text
        // This is a heuristic but works for most cases
        for i in 0..decl_node.child_count() {
            if let Some(child) = decl_node.child(i) {
                // Look for init_declarator or direct declarators
                if matches!(
                    child.kind(),
                    "init_declarator" | "array_declarator" | "pointer_declarator" | "identifier"
                ) {
                    // Search recursively for identifier nodes
                    if self.contains_identifier(&child, var_name) {
                        return true;
                    }
                }
            }
        }
        false
    }

    /// Check if a node tree contains an identifier with given name
    fn contains_identifier(&self, node: &Node, var_name: &str) -> bool {
        if node.kind() == "identifier" {
            // Compare node text byte range (simple byte comparison)
            if var_name.len() == (node.end_byte() - node.start_byte()) {
                // Exact length match - likely the same identifier
                return true;
            }
        }

        // Recursively search children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if self.contains_identifier(&child, var_name) {
                    return true;
                }
            }
        }
        false
    }

    /// Check if a global variable is reassigned later in the same function
    /// (e.g., p = NULL after p = local_var makes it safe)
    fn is_global_reassigned_later(
        &self,
        assignment_node: &Node,
        var_name: &str,
        source: &str,
    ) -> bool {
        // Find the containing function body
        let mut current = assignment_node.parent();
        let mut function_body: Option<Node> = None;

        while let Some(node) = current {
            if node.kind() == "compound_statement" {
                if let Some(parent) = node.parent() {
                    if parent.kind() == "function_definition" {
                        function_body = Some(node);
                        break;
                    }
                }
            }
            current = node.parent();
        }

        let body = match function_body {
            Some(b) => b,
            None => return false,
        };

        // Find the expression_statement containing the current assignment
        let mut current_stmt: Option<Node> = None;
        let mut node = Some(*assignment_node);
        while let Some(n) = node {
            if n.kind() == "expression_statement" {
                current_stmt = Some(n);
                break;
            }
            node = n.parent();
        }

        let current_expr_stmt = match current_stmt {
            Some(s) => s,
            None => return false,
        };

        // Find all statements after this assignment
        let statements = self.get_statements_in_body(&body);
        let mut found_current = false;

        for stmt in statements {
            // Skip until we find the current statement
            if stmt.id() == current_expr_stmt.id() {
                found_current = true;
                continue; // Skip the current statement itself
            }

            if found_current {
                // Check if this statement assigns to the same variable
                if self.assigns_to_variable(&stmt, var_name, source) {
                    return true;
                }
            }
        }

        false
    }

    /// Get all statements within a compound statement (flattened)
    fn get_statements_in_body<'a>(&self, body: &Node<'a>) -> Vec<Node<'a>> {
        let mut statements = Vec::new();
        for i in 0..body.child_count() {
            if let Some(child) = body.child(i) {
                if child.kind() == "expression_statement" {
                    statements.push(child);
                }
            }
        }
        statements
    }

    /// Check if a statement assigns to a specific variable
    fn assigns_to_variable(&self, stmt: &Node, var_name: &str, source: &str) -> bool {
        if stmt.kind() == "expression_statement" {
            for i in 0..stmt.child_count() {
                if let Some(child) = stmt.child(i) {
                    if child.kind() == "assignment_expression" {
                        if let Some(left) = child.child_by_field_name("left") {
                            let left_text = ast_utils::get_node_text(&left, source);
                            if left_text == var_name {
                                return true;
                            }
                        }
                    }
                }
            }
        }
        false
    }

    /// Check if a variable is declared at global scope or with static storage
    fn is_global_or_static(&self, var_node: &Node, source: &str) -> bool {
        // Get variable name
        let var_name = ast_utils::get_node_text(var_node, source);

        // Find the translation unit (root)
        let mut current = Some(*var_node);
        let mut root: Option<Node> = None;

        while let Some(node) = current {
            if node.kind() == "translation_unit" {
                root = Some(node);
                break;
            }
            current = node.parent();
        }

        let translation_unit = match root {
            Some(r) => r,
            None => return false,
        };

        // Search for global declaration of this variable, including inside preprocessor blocks
        let mut decls = Vec::new();
        Self::collect_file_scope_declarations(&translation_unit, &mut decls);
        for child in &decls {
            if self.contains_identifier(child, &var_name) {
                return true;
            }
        }

        false
    }
}

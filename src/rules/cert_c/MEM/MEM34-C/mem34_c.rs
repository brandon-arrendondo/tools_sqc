// SPDX-License-Identifier: MIT
// Copyright (c) 2024 Ryan Urchick

//! MEM34-C: Only free memory allocated dynamically
//!
//! This rule detects attempts to free or reallocate memory that was not dynamically allocated.
//!
//! Violations:
//! - Calling free() on a string literal pointer
//! - Calling free() on a stack-allocated variable
//! - Calling realloc() on a stack-allocated array
//! - Calling realloc() on a pointer to stack memory
//!
//! Compliant:
//! - Only free() memory returned from malloc/calloc/realloc
//! - Only realloc() memory that was previously dynamically allocated

use tree_sitter::Node;

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;

/// MEM34-C: Only free memory allocated dynamically
pub struct Mem34C;

impl CertRule for Mem34C {
    fn rule_id(&self) -> &'static str {
        "MEM34-C"
    }

    fn description(&self) -> &'static str {
        "Only free memory allocated dynamically"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "MEM34-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Mem34C {
    /// Recursively check nodes for MEM34-C violations
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check for free() and realloc() calls
        if node.kind() == "call_expression" {
            if let Some(func_node) = node.child_by_field_name("function") {
                let func_name = get_node_text(&func_node, source);
                let func_name_str = func_name.trim();

                match func_name_str {
                    "free" => {
                        // Check if the argument to free() is dynamically allocated
                        if let Some(args) = node.child_by_field_name("arguments") {
                            if let Some(var_name) =
                                self.get_first_argument_identifier(&args, source)
                            {
                                // Flag if we find a problem:
                                // 1. Variable is assigned a string literal
                                // 2. Variable is never dynamically allocated
                                let has_literal_assignment =
                                    self.has_string_literal_assignment(&var_name, node, source);
                                let has_dynamic_allocation =
                                    self.is_dynamically_allocated(&var_name, node, source);

                                if has_literal_assignment || !has_dynamic_allocation {
                                    let position = node.start_position();
                                    violations.push(RuleViolation {
                                        rule_id: self.rule_id().to_string(),
                                        severity: self.severity(),
                                        line: position.row + 1,
                                        column: position.column + 1,
                                        file_path: String::new(),
                                        message: format!(
                                            "Attempting to free '{}' which may not be dynamically allocated. \
                                            Only free memory allocated with malloc(), calloc(), or realloc().",
                                            var_name
                                        ),
                                        suggestion: Some(
                                            "Ensure the pointer was allocated with malloc(), calloc(), or realloc() \
                                            before calling free().".to_string()
                                        ),
                                        requires_manual_review: None,
                                    });
                                }
                            }
                        }
                    }
                    "realloc" => {
                        // Check if the first argument to realloc() is dynamically allocated
                        if let Some(args) = node.child_by_field_name("arguments") {
                            if let Some(var_name) =
                                self.get_first_argument_identifier(&args, source)
                            {
                                // Check if this variable is a stack array
                                if self.is_stack_allocated(&var_name, node, source) {
                                    let position = node.start_position();
                                    violations.push(RuleViolation {
                                        rule_id: self.rule_id().to_string(),
                                        severity: self.severity(),
                                        line: position.row + 1,
                                        column: position.column + 1,
                                        file_path: String::new(),
                                        message: format!(
                                            "Attempting to realloc '{}' which appears to be stack-allocated. \
                                            Only realloc memory previously allocated with malloc(), calloc(), or realloc().",
                                            var_name
                                        ),
                                        suggestion: Some(
                                            "Use malloc() to allocate dynamic memory first, then use realloc() \
                                            to resize if needed.".to_string()
                                        ),
                                        requires_manual_review: None,
                                    });
                                }
                            }
                        }
                    }
                    _ => {}
                }
            }
        }

        // Recursively check children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }

    /// Get the first argument identifier from an argument list
    fn get_first_argument_identifier(&self, args_node: &Node, source: &str) -> Option<String> {
        let mut cursor = args_node.walk();
        for child in args_node.children(&mut cursor) {
            if child.kind() != "(" && child.kind() != ")" && child.kind() != "," {
                return self.extract_identifier(&child, source);
            }
        }
        None
    }

    /// Extract identifier name from an expression
    fn extract_identifier(&self, node: &Node, source: &str) -> Option<String> {
        match node.kind() {
            "identifier" => Some(get_node_text(node, source).trim().to_string()),
            "cast_expression" => {
                // Look for identifier in the value being cast
                if let Some(value) = node.child_by_field_name("value") {
                    return self.extract_identifier(&value, source);
                }
                None
            }
            "parenthesized_expression" => {
                // Look inside parentheses
                let mut cursor = node.walk();
                for child in node.children(&mut cursor) {
                    if child.kind() != "(" && child.kind() != ")" {
                        return self.extract_identifier(&child, source);
                    }
                }
                None
            }
            _ => None,
        }
    }

    /// Check if a variable was dynamically allocated
    ///
    /// This checks if the variable was assigned from malloc/calloc/realloc
    fn is_dynamically_allocated(&self, var_name: &str, node: &Node, source: &str) -> bool {
        // Walk up to the containing function
        let mut parent = node.parent();
        while let Some(p) = parent {
            if p.kind() == "function_definition" {
                // Search for assignments to this variable
                return self.search_for_dynamic_allocation(&p, var_name, source);
            }
            parent = p.parent();
        }
        false
    }

    /// Search for dynamic allocation (malloc/calloc/realloc) assignments
    fn search_for_dynamic_allocation(&self, node: &Node, var_name: &str, source: &str) -> bool {
        // Check for assignments: var_name = malloc(...)
        if node.kind() == "assignment_expression" || node.kind() == "init_declarator" {
            // Check if left side matches our variable
            if let Some(left) = node
                .child_by_field_name("left")
                .or_else(|| node.child_by_field_name("declarator"))
            {
                if let Some(left_name) = self.extract_identifier(&left, source) {
                    if left_name == var_name {
                        // Check if right side is malloc/calloc/realloc
                        if let Some(right) = node
                            .child_by_field_name("right")
                            .or_else(|| node.child_by_field_name("value"))
                        {
                            if self.is_dynamic_allocation_call(&right, source) {
                                return true;
                            }
                        }
                    }
                }
            }
        }

        // Recursively search children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if self.search_for_dynamic_allocation(&child, var_name, source) {
                return true;
            }
        }

        false
    }

    /// Check if a node is a malloc/calloc/realloc call
    fn is_dynamic_allocation_call(&self, node: &Node, source: &str) -> bool {
        if node.kind() == "call_expression" {
            if let Some(func) = node.child_by_field_name("function") {
                let func_name = get_node_text(&func, source);
                let func_name_str = func_name.trim();
                return func_name_str == "malloc"
                    || func_name_str == "calloc"
                    || func_name_str == "realloc";
            }
        }

        // Check inside cast expressions
        if node.kind() == "cast_expression" {
            if let Some(value) = node.child_by_field_name("value") {
                return self.is_dynamic_allocation_call(&value, source);
            }
        }

        false
    }

    /// Check if a variable is stack-allocated (array declaration)
    fn is_stack_allocated(&self, var_name: &str, node: &Node, source: &str) -> bool {
        // Walk up to find the function scope
        let mut parent = node.parent();
        while let Some(p) = parent {
            if p.kind() == "function_definition" {
                // Search for array declarations
                return self.search_for_stack_array(&p, var_name, source);
            }
            parent = p.parent();
        }
        false
    }

    /// Search for stack array declarations: type name[size]
    fn search_for_stack_array(&self, node: &Node, var_name: &str, source: &str) -> bool {
        // Look for declarations with array declarators
        if node.kind() == "declaration" {
            let mut cursor = node.walk();
            for child in node.children(&mut cursor) {
                // Check init_declarator nodes (e.g., int arr[10] = {...})
                if child.kind() == "init_declarator" {
                    if let Some(declarator) = child.child_by_field_name("declarator") {
                        if self.is_array_declarator(&declarator, var_name, source) {
                            return true;
                        }
                    }
                }
                // Also check direct array_declarator nodes (e.g., char buf[256])
                if self.is_array_declarator(&child, var_name, source) {
                    return true;
                }
            }
        }

        // Recursively search children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if self.search_for_stack_array(&child, var_name, source) {
                return true;
            }
        }

        false
    }

    /// Check if a declarator is an array declarator: name[size]
    fn is_array_declarator(&self, node: &Node, var_name: &str, source: &str) -> bool {
        if node.kind() == "array_declarator" {
            // Check if the declarator name matches
            if let Some(declarator) = node.child_by_field_name("declarator") {
                if let Some(decl_name) = self.extract_identifier(&declarator, source) {
                    return decl_name == var_name;
                }
            }
        }
        false
    }

    /// Check if a variable has been assigned a string literal
    fn has_string_literal_assignment(&self, var_name: &str, node: &Node, source: &str) -> bool {
        // Walk up to the containing function
        let mut parent = node.parent();
        while let Some(p) = parent {
            if p.kind() == "function_definition" {
                return self.search_for_literal_assignment(&p, var_name, source);
            }
            parent = p.parent();
        }
        false
    }

    /// Search for assignments of string literals to a variable
    fn search_for_literal_assignment(&self, node: &Node, var_name: &str, source: &str) -> bool {
        // Check for assignments: var_name = "string literal"
        if node.kind() == "assignment_expression" {
            if let Some(left) = node.child_by_field_name("left") {
                if let Some(left_name) = self.extract_identifier(&left, source) {
                    if left_name == var_name {
                        // Check if right side is a string literal
                        if let Some(right) = node.child_by_field_name("right") {
                            if right.kind() == "string_literal" {
                                return true;
                            }
                        }
                    }
                }
            }
        }

        // Recursively search children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if self.search_for_literal_assignment(&child, var_name, source) {
                return true;
            }
        }

        false
    }
}

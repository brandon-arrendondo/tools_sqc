//! INT07-C: Use only explicitly signed or unsigned char type for numeric values
//!
//! The plain `char` type has implementation-defined signedness, making it unsuitable
//! for numeric operations. Use explicit `signed char` or `unsigned char` for numeric values.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! char c = 200;
//! int i = 1000;
//! printf("i/c = %d\n", i/c);  // Unpredictable: 5 (unsigned) or -17 (signed)
//! ```
//!
//! **Compliant:**
//! ```c
//! unsigned char c = 200;
//! int i = 1000;
//! printf("i/c = %d\n", i/c);  // Predictable: 5
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use std::collections::HashMap;
use tree_sitter::Node;

pub struct Int07C;

impl CertRule for Int07C {
    fn rule_id(&self) -> &'static str {
        "INT07-C"
    }

    fn description(&self) -> &'static str {
        "Use only explicitly signed or unsigned char type for numeric values"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "INT07-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Track plain char variables (name -> declaration location)
        let mut plain_char_vars: HashMap<String, (usize, usize)> = HashMap::new();

        // Find all plain char variable declarations
        self.find_plain_char_vars(node, source, &mut plain_char_vars);

        // Find uses of plain char variables in numeric contexts
        self.find_numeric_uses(node, source, &plain_char_vars, &mut violations);

        violations
    }
}

impl Int07C {
    /// Find plain char variable declarations (not signed char or unsigned char)
    fn find_plain_char_vars(
        &self,
        node: &Node,
        source: &str,
        plain_char_vars: &mut HashMap<String, (usize, usize)>,
    ) {
        if node.kind() == "declaration" {
            let decl_text = get_node_text(node, source);

            // Check if this is a char declaration (not signed char or unsigned char)
            if self.is_plain_char_declaration(&decl_text) {
                if let Some(var_name) = self.extract_var_name(node, source) {
                    plain_char_vars.insert(
                        var_name,
                        (
                            node.start_position().row + 1,
                            node.start_position().column + 1,
                        ),
                    );
                }
            }
        }

        // Check function parameters
        if node.kind() == "parameter_declaration" {
            let param_text = get_node_text(node, source);

            if self.is_plain_char_declaration(&param_text) {
                if let Some(var_name) = self.extract_param_name(node, source) {
                    plain_char_vars.insert(
                        var_name,
                        (
                            node.start_position().row + 1,
                            node.start_position().column + 1,
                        ),
                    );
                }
            }
        }

        // Recurse through children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.find_plain_char_vars(&child, source, plain_char_vars);
            }
        }
    }

    /// Check if declaration text represents a plain char (not signed/unsigned char)
    fn is_plain_char_declaration(&self, decl_text: &str) -> bool {
        // Must contain "char"
        if !decl_text.contains("char") {
            return false;
        }

        // Must NOT contain "signed" or "unsigned" before "char"
        if decl_text.contains("signed") || decl_text.contains("unsigned") {
            return false;
        }

        // Check for patterns like "char x" or "char *x"
        // Avoid false positives like "character" or variable names containing "char"
        let normalized = decl_text.replace('\t', " ");
        let patterns = [
            " char ", " char*", " char[", "\tchar ", "\tchar*", "\tchar[",
        ];

        // Also check if it starts with "char " (at beginning of declaration)
        if normalized.trim().starts_with("char ") || normalized.trim().starts_with("char*") {
            return true;
        }

        patterns.iter().any(|p| normalized.contains(p))
    }

    /// Find uses of plain char variables in numeric contexts
    fn find_numeric_uses(
        &self,
        node: &Node,
        source: &str,
        plain_char_vars: &HashMap<String, (usize, usize)>,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Check binary expressions (arithmetic and comparison)
        if node.kind() == "binary_expression" {
            if let Some(operator) = node.child_by_field_name("operator") {
                let op_text = get_node_text(&operator, source);

                // Arithmetic operators: +, -, *, /, %
                // Comparison operators: <, <=, >, >=, ==, !=
                let is_numeric_op = matches!(
                    op_text,
                    "+" | "-" | "*" | "/" | "%" | "<" | "<=" | ">" | ">=" | "==" | "!="
                );

                if is_numeric_op {
                    // Check left and right operands
                    if let Some(left) = node.child_by_field_name("left") {
                        self.check_operand_for_violation(
                            &left,
                            source,
                            plain_char_vars,
                            violations,
                        );
                    }
                    if let Some(right) = node.child_by_field_name("right") {
                        self.check_operand_for_violation(
                            &right,
                            source,
                            plain_char_vars,
                            violations,
                        );
                    }
                }
            }
        }

        // Check assignment expressions with numeric values
        if node.kind() == "assignment_expression" {
            if let Some(right) = node.child_by_field_name("right") {
                // If right side is a numeric literal, check left side
                if self.is_numeric_literal(&right, source) {
                    if let Some(left) = node.child_by_field_name("left") {
                        self.check_operand_for_violation(
                            &left,
                            source,
                            plain_char_vars,
                            violations,
                        );
                    }
                }
            }
        }

        // Check unary operations (++, --, unary -, unary +)
        if node.kind() == "unary_expression" || node.kind() == "update_expression" {
            if let Some(argument) = node.child_by_field_name("argument") {
                self.check_operand_for_violation(&argument, source, plain_char_vars, violations);
            }
        }

        // Recurse through children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.find_numeric_uses(&child, source, plain_char_vars, violations);
            }
        }
    }

    /// Check if an operand is a plain char variable and report violation
    fn check_operand_for_violation(
        &self,
        operand: &Node,
        source: &str,
        plain_char_vars: &HashMap<String, (usize, usize)>,
        violations: &mut Vec<RuleViolation>,
    ) {
        let operand_text = get_node_text(operand, source);

        // Check if this operand is a plain char variable
        if plain_char_vars.contains_key(operand_text) {
            violations.push(RuleViolation {
                rule_id: self.rule_id().to_string(),
                message: format!(
                    "Variable '{}' of type char used in numeric operation. \
                     Use explicit 'signed char' or 'unsigned char' for numeric values.",
                    operand_text
                ),
                severity: self.severity(),
                line: operand.start_position().row + 1,
                column: operand.start_position().column + 1,
                file_path: String::new(),
                suggestion: Some(format!(
                    "Change declaration of '{}' from 'char' to 'signed char' or 'unsigned char'",
                    operand_text
                )),
                requires_manual_review: None,
            });
        }

        // Also check if operand itself is an identifier
        if operand.kind() == "identifier" {
            // Already handled above
        } else {
            // Recursively check children (for complex expressions)
            for i in 0..operand.child_count() {
                if let Some(child) = operand.child(i) {
                    if child.kind() == "identifier" {
                        let child_text = get_node_text(&child, source);
                        if plain_char_vars.contains_key(child_text) {
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                message: format!(
                                    "Variable '{}' of type char used in numeric operation. \
                                     Use explicit 'signed char' or 'unsigned char' for numeric values.",
                                    child_text
                                ),
                                severity: self.severity(),
                                line: child.start_position().row + 1,
                                column: child.start_position().column + 1,
                                file_path: String::new(),
                                suggestion: Some(format!(
                                    "Change declaration of '{}' from 'char' to 'signed char' or 'unsigned char'",
                                    child_text
                                )),
                                requires_manual_review: None,
                            });
                        }
                    }
                }
            }
        }
    }

    /// Check if a node represents a numeric literal
    fn is_numeric_literal(&self, node: &Node, source: &str) -> bool {
        if node.kind() == "number_literal" {
            return true;
        }

        // Also check for negative numeric literals (unary -)
        if node.kind() == "unary_expression" {
            if let Some(operator) = node.child_by_field_name("operator") {
                let op_text = get_node_text(&operator, source);
                if op_text == "-" || op_text == "+" {
                    if let Some(argument) = node.child_by_field_name("argument") {
                        return self.is_numeric_literal(&argument, source);
                    }
                }
            }
        }

        false
    }

    /// Extract variable name from declaration
    fn extract_var_name(&self, decl: &Node, source: &str) -> Option<String> {
        for i in 0..decl.child_count() {
            if let Some(child) = decl.child(i) {
                if child.kind() == "init_declarator" {
                    return self.find_identifier(&child, source);
                } else if child.kind() == "identifier" {
                    return Some(get_node_text(&child, source).to_string());
                }
            }
        }
        None
    }

    /// Extract parameter name from parameter declaration
    fn extract_param_name(&self, param: &Node, source: &str) -> Option<String> {
        for i in 0..param.child_count() {
            if let Some(child) = param.child(i) {
                if child.kind() == "identifier" {
                    return Some(get_node_text(&child, source).to_string());
                }
            }
        }
        None
    }

    /// Find identifier in node tree
    fn find_identifier(&self, node: &Node, source: &str) -> Option<String> {
        if node.kind() == "identifier" {
            return Some(get_node_text(node, source).to_string());
        }
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if let Some(name) = self.find_identifier(&child, source) {
                    return Some(name);
                }
            }
        }
        None
    }
}

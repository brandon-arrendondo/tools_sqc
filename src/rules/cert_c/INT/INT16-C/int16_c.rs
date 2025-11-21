//! INT16-C: Do not make assumptions about representation of signed integers
//!
//! The C Standard permits three different representations for signed integers:
//! - Two's complement
//! - One's complement
//! - Sign and magnitude
//!
//! Bitwise operations on signed integers produce implementation-defined results.
//! Always use unsigned integers for bitwise operations.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! int value;
//! if (value & 0x1 != 0) {  // Bitwise operation on signed int
//!     // Check if odd - fails on one's complement
//! }
//! ```
//!
//! **Compliant:**
//! ```c
//! // Option 1: Use modulo operator
//! if (value % 2 != 0) {
//!     // Correct way to check if odd
//! }
//!
//! // Option 2: Use unsigned integers
//! unsigned int value;
//! if (value & 0x1 != 0) {
//!     // Bitwise operations are safe on unsigned types
//! }
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use std::collections::HashMap;
use tree_sitter::Node;

pub struct Int16C;

impl CertRule for Int16C {
    fn rule_id(&self) -> &'static str {
        "INT16-C"
    }

    fn description(&self) -> &'static str {
        "Do not make assumptions about representation of signed integers"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "INT16-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Track signed integer variables (name -> declaration location)
        let mut signed_int_vars: HashMap<String, (usize, usize)> = HashMap::new();

        // Find all signed integer variable declarations
        self.find_signed_int_vars(node, source, &mut signed_int_vars);

        // Find bitwise operations on signed integer variables
        self.find_bitwise_operations(node, source, &signed_int_vars, &mut violations);

        violations
    }
}

impl Int16C {
    /// Find signed integer variable declarations (int, short, long, signed char)
    fn find_signed_int_vars(
        &self,
        node: &Node,
        source: &str,
        signed_int_vars: &mut HashMap<String, (usize, usize)>,
    ) {
        if node.kind() == "declaration" {
            let decl_text = get_node_text(node, source);

            // Check if this is a signed integer declaration
            if self.is_signed_int_declaration(&decl_text) {
                if let Some(var_name) = self.extract_var_name(node, source) {
                    signed_int_vars.insert(
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

            if self.is_signed_int_declaration(&param_text) {
                if let Some(var_name) = self.extract_param_name(node, source) {
                    signed_int_vars.insert(
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
                self.find_signed_int_vars(&child, source, signed_int_vars);
            }
        }
    }

    /// Check if declaration text represents a signed integer type
    fn is_signed_int_declaration(&self, decl_text: &str) -> bool {
        // Unsigned integers are safe - do not track
        if decl_text.contains("unsigned") {
            return false;
        }

        // Check for signed integer types
        // Note: plain 'int', 'short', 'long' are signed by default
        let has_signed_type = decl_text.contains(" int ")
            || decl_text.contains(" int*")
            || decl_text.contains(" int[")
            || decl_text.contains("\\tint ")
            || decl_text.contains("\\tint*")
            || decl_text.contains("\\tint[")
            || decl_text.trim().starts_with("int ")
            || decl_text.trim().starts_with("int*")
            || decl_text.trim().starts_with("int[")
            || decl_text.contains(" short ")
            || decl_text.contains(" short*")
            || decl_text.contains(" long ")
            || decl_text.contains(" long*")
            || decl_text.contains("signed char")
            || decl_text.contains("signed int");

        has_signed_type
    }

    /// Find bitwise operations involving signed integer variables
    fn find_bitwise_operations(
        &self,
        node: &Node,
        source: &str,
        signed_int_vars: &HashMap<String, (usize, usize)>,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Check binary bitwise operations: &, |, ^, <<, >>
        if node.kind() == "binary_expression" {
            if let Some(operator) = node.child_by_field_name("operator") {
                let op_text = get_node_text(&operator, source);

                // Bitwise operators
                let is_bitwise_op =
                    matches!(op_text, "&" | "|" | "^" | "<<" | ">>" | "&=" | "|=" | "^=");

                if is_bitwise_op {
                    // Check left and right operands
                    if let Some(left) = node.child_by_field_name("left") {
                        self.check_operand_for_violation(
                            &left,
                            source,
                            signed_int_vars,
                            violations,
                            op_text,
                        );
                    }
                    if let Some(right) = node.child_by_field_name("right") {
                        self.check_operand_for_violation(
                            &right,
                            source,
                            signed_int_vars,
                            violations,
                            op_text,
                        );
                    }
                }
            }
        }

        // Check unary bitwise NOT operation: ~
        if node.kind() == "unary_expression" {
            if let Some(operator) = node.child_by_field_name("operator") {
                let op_text = get_node_text(&operator, source);

                if op_text == "~" {
                    if let Some(argument) = node.child_by_field_name("argument") {
                        self.check_operand_for_violation(
                            &argument,
                            source,
                            signed_int_vars,
                            violations,
                            "~",
                        );
                    }
                }
            }
        }

        // Recurse through children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.find_bitwise_operations(&child, source, signed_int_vars, violations);
            }
        }
    }

    /// Check if an operand is a signed integer variable and report violation
    fn check_operand_for_violation(
        &self,
        operand: &Node,
        source: &str,
        signed_int_vars: &HashMap<String, (usize, usize)>,
        violations: &mut Vec<RuleViolation>,
        operator: &str,
    ) {
        let operand_text = get_node_text(operand, source);

        // Check if this operand is a signed integer variable
        if signed_int_vars.contains_key(operand_text) {
            violations.push(RuleViolation {
                rule_id: self.rule_id().to_string(),
                message: format!(
                    "Bitwise operation '{}' on signed integer variable '{}'. \
                     Signed integer representation is implementation-defined. \
                     Use unsigned integers for bitwise operations or use arithmetic operators instead.",
                    operator, operand_text
                ),
                severity: self.severity(),
                line: operand.start_position().row + 1,
                column: operand.start_position().column + 1,
                file_path: String::new(),
                suggestion: Some(format!(
                    "Change '{}' to 'unsigned int {}' or avoid bitwise operations on signed integers",
                    operand_text, operand_text
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
                        if signed_int_vars.contains_key(child_text) {
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                message: format!(
                                    "Bitwise operation '{}' on signed integer variable '{}'. \
                                     Signed integer representation is implementation-defined. \
                                     Use unsigned integers for bitwise operations or use arithmetic operators instead.",
                                    operator, child_text
                                ),
                                severity: self.severity(),
                                line: child.start_position().row + 1,
                                column: child.start_position().column + 1,
                                file_path: String::new(),
                                suggestion: Some(format!(
                                    "Change '{}' to 'unsigned int {}' or avoid bitwise operations on signed integers",
                                    child_text, child_text
                                )),
                                requires_manual_review: None,
                            });
                        }
                    }
                }
            }
        }
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

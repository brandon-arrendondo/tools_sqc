//! INT14-C: Avoid performing bitwise and arithmetic operations on the same data
//!
//! This rule addresses performing bitwise and arithmetic operations on the same data.
//! Bitwise operations are frequently used on arithmetic values as premature optimization,
//! which reduces code readability and obscures programmer intent.
//!
//! ## Bitwise operators:
//! - Unary: `~` (bitwise NOT)
//! - Binary: `<<` `>>` (shift), `&` `|` `^` (bitwise AND/OR/XOR)
//!
//! ## Non-compliant examples:
//!
//! **Left shift misuse:**
//! ```c
//! int compute(int x) {
//!     int y = x << 2;  // Bitwise shift
//!     x += y + 1;      // Arithmetic operation
//!     return x;        // Computes 5x + 1 via bit manipulation
//! }
//! ```
//!
//! **Right shift division:**
//! ```c
//! int compute(int x) {
//!     x >>= 2;  // Bitwise shift (implementation-dependent for negatives)
//!     return x; // Attempts division by 4
//! }
//! ```
//!
//! ## Compliant solutions:
//! ```c
//! int compute(int x) {
//!     return 5 * x + 1;  // Clear mathematical intent
//! }
//!
//! int compute(int x) {
//!     return x / 4;  // Explicit division
//! }
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use std::collections::{HashMap, HashSet};
use tree_sitter::Node;

pub struct Int14C;

#[derive(Debug, Clone, PartialEq, Eq, Hash)]
enum OperationType {
    Bitwise,
    Arithmetic,
}

impl Int14C {
    pub fn new() -> Self {
        Self
    }

    /// Check if an operator is a bitwise operator
    fn is_bitwise_operator(&self, op: &str) -> bool {
        matches!(op, "~" | "<<" | ">>" | "&" | "|" | "^")
    }

    /// Check if an operator is an arithmetic operator
    fn is_arithmetic_operator(&self, op: &str) -> bool {
        matches!(op, "+" | "-" | "*" | "/" | "%")
    }

    /// Extract variable names used in an expression
    fn extract_variables(&self, node: &Node, source: &str, vars: &mut HashSet<String>) {
        if node.kind() == "identifier" {
            let var_name = get_node_text(node, source).to_string();
            vars.insert(var_name);
        }

        // Recursively check children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.extract_variables(&child, source, vars);
            }
        }
    }

    /// Get the operator from a binary expression
    fn get_operator(&self, node: &Node, source: &str) -> Option<String> {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                let kind = child.kind();
                if kind == "+"
                    || kind == "-"
                    || kind == "*"
                    || kind == "/"
                    || kind == "%"
                    || kind == "<<"
                    || kind == ">>"
                    || kind == "&"
                    || kind == "|"
                    || kind == "^"
                {
                    return Some(kind.to_string());
                }
                // Handle named operators
                let text = get_node_text(&child, source);
                if self.is_bitwise_operator(text) || self.is_arithmetic_operator(text) {
                    return Some(text.to_string());
                }
            }
        }
        None
    }

    /// Check a function definition for mixed bitwise/arithmetic operations
    fn check_function(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Track which variables have been used with which operation types
        let mut variable_operations: HashMap<String, HashSet<OperationType>> = HashMap::new();

        // Track line numbers for first occurrence
        let mut variable_locations: HashMap<String, (usize, usize)> = HashMap::new();

        // Recursively analyze all operations in the function
        self.analyze_operations(
            node,
            source,
            &mut variable_operations,
            &mut variable_locations,
        );

        // Check for variables that have both bitwise and arithmetic operations
        for (var_name, operations) in variable_operations.iter() {
            if operations.contains(&OperationType::Bitwise)
                && operations.contains(&OperationType::Arithmetic)
            {
                let (line, column) = variable_locations.get(var_name).unwrap_or(&(0, 0));
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: self.severity(),
                    message: format!(
                        "Variable '{}' used with both bitwise and arithmetic operations (reduces code readability)",
                        var_name
                    ),
                    file_path: String::new(),
                    line: *line,
                    column: *column,
                    suggestion: Some(
                        "Use separate variables for bitwise and arithmetic operations, or refactor to use only arithmetic operators".to_string()
                    ),
                    ..Default::default()
                });
            }
        }
    }

    /// Recursively analyze operations on variables
    fn analyze_operations(
        &self,
        node: &Node,
        source: &str,
        variable_operations: &mut HashMap<String, HashSet<OperationType>>,
        variable_locations: &mut HashMap<String, (usize, usize)>,
    ) {
        // Check binary expressions
        if node.kind() == "binary_expression" {
            if let Some(op) = self.get_operator(node, source) {
                let op_type = if self.is_bitwise_operator(&op) {
                    Some(OperationType::Bitwise)
                } else if self.is_arithmetic_operator(&op) {
                    Some(OperationType::Arithmetic)
                } else {
                    None
                };

                if let Some(op_type) = op_type {
                    // Extract variables from this expression
                    let mut vars = HashSet::new();
                    self.extract_variables(node, source, &mut vars);

                    // Record operation type for each variable
                    for var in vars {
                        variable_operations
                            .entry(var.clone())
                            .or_insert_with(HashSet::new)
                            .insert(op_type.clone());

                        // Record first location if not already recorded
                        if !variable_locations.contains_key(&var) {
                            let line = node.start_position().row + 1;
                            let column = node.start_position().column + 1;
                            variable_locations.insert(var, (line, column));
                        }
                    }
                }
            }
        }

        // Check compound assignment expressions (e.g., x += 1, x <<= 2)
        if node.kind() == "assignment_expression" {
            if let Some(op_node) = node.child(1) {
                let op_text = get_node_text(&op_node, source);

                // Check for compound assignment operators
                let op_type = if op_text == "+="
                    || op_text == "-="
                    || op_text == "*="
                    || op_text == "/="
                    || op_text == "%="
                {
                    Some(OperationType::Arithmetic)
                } else if op_text == "<<="
                    || op_text == ">>="
                    || op_text == "&="
                    || op_text == "|="
                    || op_text == "^="
                {
                    Some(OperationType::Bitwise)
                } else {
                    None
                };

                if let Some(op_type) = op_type {
                    // Extract the left-hand variable
                    if let Some(left) = node.child_by_field_name("left") {
                        if left.kind() == "identifier" {
                            let var = get_node_text(&left, source).to_string();
                            variable_operations
                                .entry(var.clone())
                                .or_insert_with(HashSet::new)
                                .insert(op_type);

                            // Record first location if not already recorded
                            if !variable_locations.contains_key(&var) {
                                let line = node.start_position().row + 1;
                                let column = node.start_position().column + 1;
                                variable_locations.insert(var, (line, column));
                            }
                        }
                    }
                }
            }
        }

        // Check unary expressions (e.g., ~x)
        if node.kind() == "unary_expression" {
            if let Some(op_node) = node.child(0) {
                if op_node.kind() == "~" {
                    // Extract variables from the operand
                    let mut vars = HashSet::new();
                    if let Some(operand) = node.child(1) {
                        self.extract_variables(&operand, source, &mut vars);

                        for var in vars {
                            variable_operations
                                .entry(var.clone())
                                .or_insert_with(HashSet::new)
                                .insert(OperationType::Bitwise);

                            // Record first location if not already recorded
                            if !variable_locations.contains_key(&var) {
                                let line = node.start_position().row + 1;
                                let column = node.start_position().column + 1;
                                variable_locations.insert(var, (line, column));
                            }
                        }
                    }
                }
            }
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.analyze_operations(&child, source, variable_operations, variable_locations);
            }
        }
    }
}

impl CertRule for Int14C {
    fn rule_id(&self) -> &'static str {
        "INT14-C"
    }

    fn description(&self) -> &'static str {
        "Avoid performing bitwise and arithmetic operations on the same data"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "INT14-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Check function definitions
        if node.kind() == "function_definition" {
            self.check_function(node, source, &mut violations);
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

use super::{CertRule, RuleViolation};
use crate::manifest::Severity;
use tree_sitter::Node;

pub struct Arr30C;

impl CertRule for Arr30C {
    fn rule_id(&self) -> &'static str {
        "ARR30-C"
    }

    fn description(&self) -> &'static str {
        "Do not form or use out-of-bounds pointers or array subscripts"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        if node.kind() == "subscript_expression" {
            let start_point = node.start_position();

            // Extract the index expression from the subscript
            if let Some(index_node) = get_subscript_index(node) {
                let index_text = &source[index_node.start_byte()..index_node.end_byte()];

                // Check if this array access has proper bounds checking
                if !has_bounds_check(node, index_text, source) {
                    let array_text = if let Some(array_node) = get_subscript_array(node) {
                        &source[array_node.start_byte()..array_node.end_byte()]
                    } else {
                        "array"
                    };

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!("Potential out-of-bounds array access: {}[{}]. Ensure bounds checking is performed.", array_text, index_text),
                        file_path: String::new(), // Will be filled by caller
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Add bounds checking before array access or use loop with proper bounds".to_string()),
                    });
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

fn get_subscript_array<'a>(node: &'a Node<'a>) -> Option<Node<'a>> {
    // The array is typically the first child of a subscript_expression
    node.child(0)
}

fn get_subscript_index<'a>(node: &'a Node<'a>) -> Option<Node<'a>> {
    // The index is typically between '[' and ']' - usually the second child
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            // Skip '[' and ']' symbols, look for the actual index expression
            if child.kind() != "[" && child.kind() != "]" && i > 0 {
                return Some(child);
            }
        }
    }
    None
}

fn has_bounds_check(subscript_node: &Node, index_text: &str, source: &str) -> bool {
    // Check multiple patterns for bounds checking:
    // 1. Loop-based bounds checking
    // 2. Conditional bounds checking
    // 3. Function parameter bounds checking

    // First check for loop-based bounds checking
    if has_loop_bounds_check(subscript_node, index_text, source) {
        return true;
    }

    // Check for conditional bounds checking
    if has_conditional_bounds_check(subscript_node, index_text, source) {
        return true;
    }

    // Check for function parameter bounds checking
    if has_function_bounds_check(subscript_node, index_text, source) {
        return true;
    }

    false
}

fn has_loop_bounds_check(subscript_node: &Node, index_text: &str, source: &str) -> bool {
    // Traverse up the AST to find containing for_statement nodes
    let mut current = subscript_node.parent();

    while let Some(node) = current {
        if node.kind() == "for_statement" {
            return check_for_loop_bounds(&node, index_text, source);
        }
        current = node.parent();
    }
    false
}

fn check_for_loop_bounds(for_node: &Node, index_text: &str, source: &str) -> bool {
    // Parse the for loop structure: for (init; condition; update)
    // We need to check if:
    // 1. The index variable matches the loop iterator
    // 2. The loop condition properly constrains the iterator

    // Find the condition part of the for loop
    for i in 0..for_node.child_count() {
        if let Some(child) = for_node.child(i) {
            if child.kind() == "binary_expression" || child.kind() == "comparison_expression" {
                let condition_text = &source[child.start_byte()..child.end_byte()];

                // Check if the condition constrains our index variable properly
                if condition_contains_safe_bounds(condition_text, index_text) {
                    return true;
                }
            }
        }
    }

    // Also check nested expressions in the condition
    for i in 0..for_node.child_count() {
        if let Some(child) = for_node.child(i) {
            if child.kind() == "parenthesized_expression" {
                // Look for condition inside parentheses
                for j in 0..child.child_count() {
                    if let Some(grandchild) = child.child(j) {
                        if grandchild.kind() == "binary_expression" || grandchild.kind() == "comparison_expression" {
                            let condition_text = &source[grandchild.start_byte()..grandchild.end_byte()];
                            if condition_contains_safe_bounds(condition_text, index_text) {
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

fn condition_contains_safe_bounds(condition_text: &str, index_text: &str) -> bool {
    // Check for safe boundary conditions like:
    // i < size, i < length, i < array_size, etc.
    // Unsafe conditions like i <= size are considered violations (off-by-one)

    let trimmed_index = index_text.trim();

    // Look for patterns like "i < size" (safe)
    if condition_text.contains(&format!("{} <", trimmed_index)) {
        // Make sure it's not "<=" which would be unsafe
        return !condition_text.contains(&format!("{} <=", trimmed_index));
    }

    // Look for patterns like "size > i" (safe)
    if condition_text.contains(&format!("> {}", trimmed_index)) {
        return !condition_text.contains(&format!(">= {}", trimmed_index));
    }
    false
}

fn has_conditional_bounds_check(subscript_node: &Node, index_text: &str, source: &str) -> bool {
    // Check for explicit if conditions that check bounds
    let mut current = subscript_node.parent();

    while let Some(node) = current {
        if node.kind() == "if_statement" {
            // Look for the condition part
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() == "parenthesized_expression" || child.kind() == "binary_expression" {
                        let condition_text = &source[child.start_byte()..child.end_byte()];
                        if condition_contains_safe_bounds(condition_text, index_text) {
                            return true;
                        }
                    }
                }
            }
        }
        current = node.parent();
    }

    false
}

fn has_function_bounds_check(subscript_node: &Node, _index_text: &str, source: &str) -> bool {
    // Check if this is inside a function that receives bounds as parameters
    // This is a simplified check - in practice, this would need more sophisticated analysis
    let mut current = subscript_node.parent();

    while let Some(node) = current {
        if node.kind() == "function_definition" {
            // Check if function parameters include size/length parameters
            let function_text = &source[node.start_byte()..node.end_byte()];
            if function_text.contains("size") || function_text.contains("length") || function_text.contains("count") {
                return true;
            }
        }
        current = node.parent();
    }

    false
}
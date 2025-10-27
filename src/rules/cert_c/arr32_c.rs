use super::super::{CertRule, RuleViolation};
use crate::manifest::Severity;
use tree_sitter::Node;

pub struct Arr32C;

impl CertRule for Arr32C {
    fn rule_id(&self) -> &'static str {
        "ARR32-C"
    }

    fn description(&self) -> &'static str {
        "Ensure size arguments for variable length arrays are in a valid range"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Look for variable length array declarations
        if node.kind() == "array_declarator" {
            if let Some(size_node) = node.child_by_field_name("size") {
                let start_point = node.start_position();

                // Check if this is a VLA (size is not a constant)
                if is_variable_length_array(&size_node, source) {
                    let size_text = &source[size_node.start_byte()..size_node.end_byte()];

                    // Check for obvious violations
                    if is_problematic_vla_size(&size_node, source) {
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::High,
                            message: format!(
                                "Variable length array with potentially unsafe size '{}'. Ensure size is validated to be positive and within reasonable bounds.",
                                size_text
                            ),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some("Add bounds checking: if (size == 0 || size > MAX_ARRAY) { /* handle error */ }".to_string()),
                        });
                    }
                }
            }
        }

        // Look for function parameters that might be used as VLA sizes
        if node.kind() == "parameter_declaration" {
            if let Some(declarator) = node.child_by_field_name("declarator") {
                if declarator.kind() == "array_declarator" {
                    if let Some(size_node) = declarator.child_by_field_name("size") {
                        let start_point = node.start_position();
                        let size_text = &source[size_node.start_byte()..size_node.end_byte()];

                        // Check if this looks like an unchecked parameter
                        if is_identifier_node(&size_node) {
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                severity: Severity::Medium,
                                message: format!(
                                    "Function parameter used as VLA size '{}' without validation. Consider adding bounds checking.",
                                    size_text
                                ),
                                file_path: String::new(),
                                line: start_point.row + 1,
                                column: start_point.column + 1,
                                suggestion: Some("Validate the size parameter before using it for VLA declaration".to_string()),
                            });
                        }
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

fn is_variable_length_array(size_node: &Node, source: &str) -> bool {
    // A VLA has a size that is not a compile-time constant
    // Simple heuristic: if it's an identifier or expression (not a number literal)
    match size_node.kind() {
        "identifier" => true,
        "binary_expression" => true,
        "call_expression" => true,
        "number_literal" => false,
        _ => {
            // Check if it contains any identifiers (making it non-constant)
            contains_identifier(size_node, source)
        }
    }
}

fn is_problematic_vla_size(size_node: &Node, source: &str) -> bool {
    let size_text = &source[size_node.start_byte()..size_node.end_byte()];

    // Check for obvious problematic patterns
    if size_text == "0" {
        return true;
    }

    // Check for identifiers without obvious bounds checking context
    if size_node.kind() == "identifier" {
        // Look for nearby bounds checking
        return !has_nearby_bounds_check(size_node, source);
    }

    // Check for expressions that might overflow
    if size_node.kind() == "binary_expression" {
        let operator = get_binary_operator(size_node, source);
        if operator == "*" || operator == "+" {
            // Potential for overflow
            return true;
        }
    }

    false
}

fn is_identifier_node(node: &Node) -> bool {
    node.kind() == "identifier"
}

fn contains_identifier(node: &Node, _source: &str) -> bool {
    if node.kind() == "identifier" {
        return true;
    }

    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if contains_identifier(&child, _source) {
                return true;
            }
        }
    }

    false
}

fn has_nearby_bounds_check(node: &Node, source: &str) -> bool {
    // Simple heuristic: look in the surrounding context for bounds checking patterns
    if let Some(parent) = node.parent() {
        if let Some(grandparent) = parent.parent() {
            let context = &source[grandparent.start_byte()..grandparent.end_byte()];

            // Look for common bounds checking patterns
            return context.contains("if") &&
                   (context.contains("== 0") ||
                    context.contains("> ") ||
                    context.contains("< ") ||
                    context.contains("MAX_") ||
                    context.contains("SIZE_MAX"));
        }
    }
    false
}

fn get_binary_operator<'a>(node: &Node, source: &'a str) -> &'a str {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            let child_text = &source[child.start_byte()..child.end_byte()];
            if child_text == "*" || child_text == "+" || child_text == "-" || child_text == "/" {
                return child_text;
            }
        }
    }
    ""
}

#[cfg(test)]
#[path = "tests/arr32_c.rs"]
mod tests;
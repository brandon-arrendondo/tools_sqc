use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Exp07C;

impl CertRule for Exp07C {
    fn rule_id(&self) -> &'static str {
        "EXP07-C"
    }

    fn description(&self) -> &'static str {
        "Do not diminish the benefits of constants by assuming their values in expressions"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "EXP07-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Check for binary expressions with shift operators that use magic numbers
        if node.kind() == "binary_expression" {
            if let Some(operator_node) = node.child_by_field_name("operator") {
                let operator = get_node_text(&operator_node, source);

                // Check for shift operators
                if operator == "<<" || operator == ">>" {
                    // Check if the right operand (shift amount) is a numeric literal
                    if let Some(right) = node.child_by_field_name("right") {
                        if is_numeric_literal(&right, source) {
                            // Byte-boundary shifts (8, 16, 24, 32, ...) are standard
                            // serialization/packing idioms, not magic number assumptions.
                            let shift_text = get_node_text(&right, source).trim();
                            if let Ok(shift_val) = shift_text.parse::<u32>() {
                                if shift_val > 0 && shift_val % 8 == 0 {
                                    // Skip — byte extraction/packing pattern
                                } else {
                                    report_violation(node, source, &mut violations);
                                }
                            } else {
                                report_violation(node, source, &mut violations);
                            }
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

/// Check if a node is a numeric literal
fn is_numeric_literal(node: &Node, source: &str) -> bool {
    match node.kind() {
        "number_literal" => true,
        _ => {
            // Also check if it's an expression that evaluates to a number
            let text = get_node_text(node, source).trim();
            text.chars().all(|c| c.is_ascii_digit())
        }
    }
}

/// Check if there's a comment near this node that suggests a constant assumption
/// For example: "/* BUFSIZ = 512 = 2^9 */"
fn has_constant_assumption_comment(node: &Node, source: &str) -> bool {
    // Get the line containing this node
    let start_byte = node.start_byte();
    let end_byte = node.end_byte();

    // Search for the start of the line
    let line_start = source[..start_byte]
        .rfind('\n')
        .map(|pos| pos + 1)
        .unwrap_or(0);

    // Search for the end of the line
    let line_end = source[end_byte..]
        .find('\n')
        .map(|pos| end_byte + pos)
        .unwrap_or(source.len());

    // Get the full line
    let line = &source[line_start..line_end];

    // Check if the line contains a comment with typical constant assumption patterns
    // Pattern: "/* NAME = value = expression */" or similar
    if line.contains("/*") && line.contains("*/") {
        // Extract comment content
        if let Some(comment_start) = line.find("/*") {
            if let Some(comment_end) = line.find("*/") {
                let comment = &line[comment_start + 2..comment_end];

                // Look for patterns like "CONSTANT = number" or "= 2^n"
                if comment.contains('=')
                    && (comment.contains('^')
                        || comment
                            .chars()
                            .any(|c| c.is_ascii_uppercase() && c.is_alphabetic()))
                {
                    return true;
                }
            }
        }
    }

    false
}

/// Report a violation for assuming constant values in expressions
fn report_violation(node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
    let start_point = node.start_position();
    let node_text = get_node_text(node, source);

    violations.push(RuleViolation {
        rule_id: "EXP07-C".to_string(),
        severity: Severity::Low,
        message: format!(
            "Do not assume constant values in expressions: '{}'",
            node_text
        ),
        file_path: String::new(),
        line: start_point.row + 1,
        column: start_point.column + 1,
        suggestion: Some(
            "Use the constant identifier directly instead of assuming its numeric value"
                .to_string(),
        ),
        ..Default::default()
    });
}

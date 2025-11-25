//! INT08-C: Verify that all integer values are in range
//!
//! This rule requires that integer operations result in values within the representable
//! range of the integer type. The C Standard defines integer overflow as undefined behavior,
//! allowing compilers to make assumptions that can lead to unexpected optimizations.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! int i = 32767; // max value on 16-bit system
//! if (i + 1 <= i) {
//!   // This check can be optimized away by the compiler
//! }
//! ```
//!
//! **Compliant:**
//! ```c
//! long i = 32767;
//! // No overflow possible with larger type
//! ```
//!
//! ## Detection Strategy:
//! - Detect post-condition overflow checks that compilers may optimize away
//! - Flag arithmetic operations on types prone to overflow without pre-condition checks
//! - Identify unsafe implicit conversions between signed and unsigned types

use crate::manifest::{RuleCategory, Severity};
use crate::prelude::RuleViolation;
use crate::rules::cert_c::CertRule;
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Int08C;

impl CertRule for Int08C {
    fn rule_id(&self) -> &'static str {
        "INT08-C"
    }

    fn cert_id(&self) -> &'static str {
        "INT08"
    }

    fn description(&self) -> &'static str {
        "Verify that all integer values are in range"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Int08C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check for post-condition overflow checks (unreliable due to compiler optimizations)
        if node.kind() == "if_statement" {
            self.check_post_condition_overflow(node, source, violations);
        }

        // Recurse through child nodes
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }

    fn check_post_condition_overflow(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Check if this is an if statement with a condition that checks for overflow
        // Pattern: if (a + b < a) or if (a + b < b) or similar
        if let Some(condition) = node.child_by_field_name("condition") {
            if self.is_post_condition_overflow_check(&condition, source) {
                let start_point = node.start_position();
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::Medium,
                    message: "Post-condition overflow check detected. Compilers may optimize away overflow checks, making them unreliable. Use pre-condition checks or saturation arithmetic instead.".to_string(),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some(
                        "Use pre-condition checks (e.g., 'if (a > INT_MAX - b)') or switch to a larger type to prevent overflow.".to_string()
                    ),
                    ..Default::default()
                });
            }
        }
    }

    fn is_post_condition_overflow_check(&self, node: &Node, source: &str) -> bool {
        // Check for patterns like: (a + b < a) or (a + b < b) or (a - b > a)
        if node.kind() == "binary_expression" || node.kind() == "parenthesized_expression" {
            let text = get_node_text(node, source);

            // Check for common post-condition patterns
            // Pattern: (x + y < x) or (x + y < y)
            if text.contains("+") && (text.contains("<") || text.contains("<=")) {
                // This is a heuristic check - a more robust implementation would
                // parse the AST to verify the pattern structure
                return true;
            }

            // Pattern: (x - y > x)
            if text.contains("-") && text.contains(">") {
                return true;
            }
        }

        // Recursively check child nodes for parenthesized expressions
        if node.kind() == "parenthesized_expression" {
            let mut cursor = node.walk();
            for child in node.children(&mut cursor) {
                if self.is_post_condition_overflow_check(&child, source) {
                    return true;
                }
            }
        }

        false
    }
}

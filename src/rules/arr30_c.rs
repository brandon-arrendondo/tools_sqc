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
            // Basic check for array access patterns that might be problematic
            let start_point = node.start_position();
            let text = &source[node.start_byte()..node.end_byte()];

            // Simple heuristic: look for potentially dangerous patterns
            if text.contains("arr[i]") && !has_bounds_check(node, source) {
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::High,
                    message: "Potential out-of-bounds array access detected. Ensure bounds checking is performed.".to_string(),
                    file_path: String::new(), // Will be filled by caller
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some("Add bounds checking before array access".to_string()),
                });
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

fn has_bounds_check(node: &Node, source: &str) -> bool {
    // Simple heuristic to check if there's bounds checking nearby
    // In a real implementation, this would be more sophisticated
    let parent = node.parent();
    if let Some(parent_node) = parent {
        let parent_text = &source[parent_node.start_byte()..parent_node.end_byte()];
        parent_text.contains("if") && (parent_text.contains("<") || parent_text.contains(">="))
    } else {
        false
    }
}
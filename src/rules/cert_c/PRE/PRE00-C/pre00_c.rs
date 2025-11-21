use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Pre00C;

impl CertRule for Pre00C {
    fn rule_id(&self) -> &'static str {
        "PRE00-C"
    }

    fn description(&self) -> &'static str {
        "Prefer inline or static functions to function-like macros"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        self.rule_id()
    }

    fn check(&self, root: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(root, source, &mut violations);
        violations
    }
}

impl Pre00C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Look for function-like macros
        // Tree-sitter C uses "preproc_function_def" for #define MACRO(args) ...
        if node.kind() == "preproc_function_def" || node.kind() == "preproc_def" {
            let text = get_node_text(node, source);
            // For preproc_function_def, it's always a function-like macro
            // For preproc_def, check if it looks like a function-like macro
            let is_function_like =
                node.kind() == "preproc_function_def" || (text.contains('(') && text.contains(')'));

            if is_function_like {
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: self.severity(),
                    message: "Function-like macro detected; prefer inline or static functions for type safety".to_string(),
                    file_path: String::new(),
                    line: node.start_position().row + 1,
                    column: node.start_position().column + 1,
                    suggestion: None,
                    requires_manual_review: None,
                });
            }
        }

        // Recurse
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }
}

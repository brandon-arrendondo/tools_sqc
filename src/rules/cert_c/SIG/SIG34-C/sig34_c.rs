use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Sig34C;

impl CertRule for Sig34C {
    fn rule_id(&self) -> &'static str {
        "SIG34-C"
    }

    fn description(&self) -> &'static str {
        "Do not call signal() from within interruptible signal handlers"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        self.rule_id()
    }

    fn check(&self, root: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.find_signal_handlers(root, source, &mut violations);
        violations
    }
}

impl Sig34C {
    fn find_signal_handlers(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Look for function definitions that might be signal handlers
        if node.kind() == "function_definition" {
            if let Some(declarator) = node.child_by_field_name("declarator") {
                let func_text = get_node_text(&declarator, source);
                // Check if this looks like a signal handler (takes int parameter)
                if func_text.contains("int")
                    || func_text.contains("signum")
                    || func_text.contains("sig")
                {
                    // Check for signal() calls within this function
                    if let Some(body) = node.child_by_field_name("body") {
                        self.check_for_signal_calls(&body, source, violations);
                    }
                }
            }
        }

        // Recurse
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.find_signal_handlers(&child, source, violations);
        }
    }

    fn check_for_signal_calls(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if node.kind() == "call_expression" {
            if let Some(func_node) = node.child_by_field_name("function") {
                let func_name = get_node_text(&func_node, source);
                if func_name == "signal" {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: self.severity(),
                        message: "signal() called from within signal handler; this is not safe"
                            .to_string(),
                        file_path: String::new(),
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        suggestion: None,
                        requires_manual_review: None,
                    });
                }
            }
        }

        // Recurse
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_for_signal_calls(&child, source, violations);
        }
    }
}

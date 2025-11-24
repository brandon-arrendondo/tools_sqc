use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Fio15C;

impl CertRule for Fio15C {
    fn rule_id(&self) -> &'static str {
        "FIO15-C"
    }

    fn description(&self) -> &'static str {
        "Ensure that file operations are performed in a secure directory"
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
        self.check_node(root, source, &mut violations);
        violations
    }
}

impl Fio15C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Look for fopen calls with paths in potentially insecure directories
        if node.kind() == "call_expression" {
            if let Some(func_node) = node.child_by_field_name("function") {
                let func_name = get_node_text(&func_node, source);
                if func_name == "fopen" {
                    if let Some(args) = node.child_by_field_name("arguments") {
                        if let Some(first_arg) = args.named_child(0) {
                            let path = get_node_text(&first_arg, source);
                            // Check for operations in /tmp or other world-writable directories
                            if path.contains("/tmp")
                                && !self.has_security_check_before(node, source)
                            {
                                violations.push(RuleViolation {
                                    rule_id: self.rule_id().to_string(),
                                    severity: self.severity(),
                                    message: format!(
                                        "File operation in potentially insecure directory {}",
                                        path
                                    ),
                                    file_path: String::new(),
                                    line: node.start_position().row + 1,
                                    column: node.start_position().column + 1,
                                    suggestion: None,
                                    requires_manual_review: None,
                                });
                            }
                        }
                    }
                }
            }
        }

        // Recurse
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }

    fn has_security_check_before(&self, _node: &Node, source: &str) -> bool {
        // Simple heuristic: check if source contains security-related function calls
        source.contains("is_secure_directory") || source.contains("stat(")
    }
}

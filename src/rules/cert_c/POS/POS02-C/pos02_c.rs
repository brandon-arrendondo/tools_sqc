use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use tree_sitter::Node;

pub struct Pos02C;

impl CertRule for Pos02C {
    fn rule_id(&self) -> &'static str {
        "POS02-C"
    }

    fn description(&self) -> &'static str {
        "Follow the principle of least privilege - drop privileges after setup"
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

impl Pos02C {
    fn check_node<'a>(&self, node: &Node<'a>, source: &str, violations: &mut Vec<RuleViolation>) {
        // Look for privileged operations without privilege dropping
        if node.kind() == "call_expression" {
            if let Some(func) = node.child_by_field_name("function") {
                let func_name = &source[func.start_byte()..func.end_byte()];

                // Check for privileged operations
                if self.is_privileged_operation(func_name) {
                    // Check if privilege dropping exists after this call
                    if !self.has_privilege_drop_after(node, source) {
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: self.severity(),
                            message: format!(
                                "Privileged operation '{}' without subsequent privilege dropping",
                                func_name
                            ),
                            file_path: String::new(),
                            line: node.start_position().row + 1,
                            column: node.start_position().column + 1,
                            suggestion: Some(
                                "Call setuid/setgid to drop privileges after setup".to_string(),
                            ),
                            requires_manual_review: Some(true),
                            ..Default::default()
                        });
                    }
                }
            }
        }

        // Recursively check child nodes
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }

    fn is_privileged_operation(&self, func_name: &str) -> bool {
        // Operations that often require elevated privileges
        matches!(
            func_name,
            "socket"
                | "bind"
                | "listen"
                | "setsockopt"
                | "chroot"
                | "chown"
                | "chmod"
                | "mount"
                | "umount"
        )
    }

    fn has_privilege_drop_after(&self, node: &Node, source: &str) -> bool {
        // Get the function containing this call
        let mut current = node.parent();
        while let Some(parent) = current {
            if parent.kind() == "function_definition" {
                // Search for setuid/setgid/seteuid/setegid calls
                let text = &source[parent.start_byte()..parent.end_byte()];
                return text.contains("setuid")
                    || text.contains("setgid")
                    || text.contains("seteuid")
                    || text.contains("setegid");
            }
            current = parent.parent();
        }
        false
    }
}

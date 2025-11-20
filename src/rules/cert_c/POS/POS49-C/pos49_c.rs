use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use tree_sitter::Node;

pub struct Pos49C;

impl CertRule for Pos49C {
    fn rule_id(&self) -> &'static str {
        "POS49-C"
    }

    fn description(&self) -> &'static str {
        "Do not access shared bit-fields from multiple threads without mutex protection"
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

        // Simple heuristic: if source has bit fields (":") and threading patterns, flag it
        if self.has_bit_fields(source) && self.has_potential_thread_access(source) {
            self.check_node(root, source, &mut violations);
        }

        violations
    }
}

impl Pos49C {
    fn has_bit_fields(&self, source: &str) -> bool {
        // Look for bit field pattern in struct: "unsigned int name : width"
        source.contains("unsigned") && source.contains(':') && source.contains("struct")
    }

    fn check_node<'a>(&self, node: &Node<'a>, source: &str, violations: &mut Vec<RuleViolation>) {
        // Look for field_declaration nodes that contain ":"
        if node.kind() == "field_declaration" {
            let text = &source[node.start_byte()..node.end_byte()];
            if text.contains(':') && !text.contains("::") {
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: self.severity(),
                    message: "Bit-field in struct may be accessed from multiple threads without mutex protection".to_string(),
                    file_path: String::new(),
                    line: node.start_position().row + 1,
                    column: node.start_position().column + 1,
                    suggestion: Some("Use separate bytes/integers or protect with mutex".to_string()),
                    requires_manual_review: Some(true),
                    ..Default::default()
                });
            }
        }

        // Recursively check child nodes
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }

    fn has_potential_thread_access(&self, source: &str) -> bool {
        // Heuristic: look for pthread or multiple functions suggesting threads
        source.contains("pthread_create") ||
        source.contains("pthread_t") ||
        source.contains("thread") ||
        // Check for multiple functions that might be thread targets
        (source.matches("void ").count() >= 2 && source.contains("flags"))
    }
}

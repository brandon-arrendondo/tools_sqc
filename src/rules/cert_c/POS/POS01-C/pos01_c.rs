use crate::manifest::{RuleCategory, Severity};
use crate::prelude::RuleViolation;
use crate::rules::cert_c::CertRule;
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct POS01C;

impl CertRule for POS01C {
    fn rule_id(&self) -> &'static str {
        "POS01-C"
    }

    fn cert_id(&self) -> &'static str {
        "POS01"
    }

    fn description(&self) -> &'static str {
        "Check for the existence of links when dealing with files"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Check both inside functions and at top-level (translation_unit)
        if node.kind() == "translation_unit" {
            // For top-level code, check if there's any lstat() in the entire file
            let has_lstat = self.subtree_has_lstat(node, source);
            self.check_open_calls_recursive(node, source, has_lstat, &mut violations);
        } else {
            // Check functions individually
            self.check_functions(node, source, &mut violations);
        }

        violations
    }
}

impl POS01C {
    fn check_functions(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if node.kind() == "function_definition" {
            // Check this function for open() without proper protection
            let has_lstat = self.subtree_has_lstat(node, source);
            self.check_open_calls_recursive(node, source, has_lstat, violations);
        }

        // Recurse
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_functions(&child, source, violations);
        }
    }

    fn subtree_has_lstat(&self, node: &Node, source: &str) -> bool {
        if node.kind() == "call_expression" {
            let mut cursor = node.walk();
            for child in node.children(&mut cursor) {
                if child.kind() == "identifier" {
                    let func_name = get_node_text(&child, source);
                    if func_name == "lstat" {
                        return true;
                    }
                }
            }
        }

        // Recurse
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if self.subtree_has_lstat(&child, source) {
                return true;
            }
        }

        false
    }

    fn check_open_calls_recursive(&self, node: &Node, source: &str, has_lstat: bool, violations: &mut Vec<RuleViolation>) {
        if node.kind() == "call_expression" {
            // Check if this is an open() call
            let mut cursor = node.walk();
            let mut is_open = false;
            let mut has_nofollow = false;

            for child in node.children(&mut cursor) {
                if child.kind() == "identifier" {
                    let func_name = get_node_text(&child, source);
                    if func_name == "open" {
                        is_open = true;
                    }
                }
                
                // Check arguments for O_NOFOLLOW
                if child.kind() == "argument_list" {
                    has_nofollow = self.has_nofollow_flag(&child, source);
                }
            }

            if is_open && !has_nofollow && !has_lstat {
                // Flag open() without O_NOFOLLOW and without lstat() validation
                let start = node.start_position();
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    file_path: String::new(),
                    message: "open() called without O_NOFOLLOW flag and without lstat() validation. This may be vulnerable to symlink attacks.".to_string(),
                    line: start.row + 1,
                    column: start.column + 1,
                    severity: self.severity(),
                    suggestion: Some("Use O_NOFOLLOW flag in open() call, or validate with lstat() before opening".to_string()),
                    requires_manual_review: Some(false),
                });
            }
        }

        // Recurse
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_open_calls_recursive(&child, source, has_lstat, violations);
        }
    }

    fn has_nofollow_flag(&self, node: &Node, source: &str) -> bool {
        // Check if any argument contains O_NOFOLLOW
        let text = get_node_text(node, source);
        text.contains("O_NOFOLLOW")
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_pos01_c() {
        let rule = POS01C;
        assert_eq!(rule.rule_id(), "POS01-C");
        assert_eq!(rule.cert_id(), "POS01");
    }
}

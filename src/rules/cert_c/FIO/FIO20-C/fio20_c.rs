//! FIO20-C: Avoid unintentional truncation when using fgets() or fgetws()
//!
//! fgets() reads at most n-1 characters. If input is longer, it's truncated
//! without warning. Code should check for newline to detect truncation.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! char buf[10];
//! fgets(buf, sizeof(buf), stdin);  // No truncation check
//! process(buf);
//! ```
//!
//! **Compliant:**
//! ```c
//! char buf[10];
//! fgets(buf, sizeof(buf), stdin);
//! if (strchr(buf, '\n') == NULL) {
//!     // Handle truncation
//! }
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Fio20C;

impl CertRule for Fio20C {
    fn rule_id(&self) -> &'static str {
        "FIO20-C"
    }

    fn description(&self) -> &'static str {
        "Avoid unintentional truncation when using fgets() or fgetws()"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "FIO20-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_fgets_truncation(node, source, &mut violations);
        violations
    }
}

impl Fio20C {
    /// Check for fgets/fgetws without truncation handling
    fn check_fgets_truncation(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = get_node_text(&function, source);
                if func_name == "fgets" || func_name == "fgetws" {
                    // Check if followed by newline check
                    if !self.has_newline_check(node, source) {
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            message: format!(
                                "{}() without truncation check. Input may be silently truncated.",
                                func_name
                            ),
                            severity: self.severity(),
                            line: node.start_position().row + 1,
                            column: node.start_position().column + 1,
                            file_path: String::new(),
                            suggestion: Some(
                                "Check for newline character to detect truncation: \
                                 if (strchr(buf, '\\n') == NULL) { /* handle truncation */ }"
                                    .to_string(),
                            ),
                            requires_manual_review: Some(true),
                        });
                    }

                    // Check for small buffer sizes
                    if let Some(size) = self.get_buffer_size(node, source) {
                        if size < 32 {
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                message: format!(
                                    "{}() with small buffer size ({}). High risk of truncation.",
                                    func_name, size
                                ),
                                severity: self.severity(),
                                line: node.start_position().row + 1,
                                column: node.start_position().column + 1,
                                file_path: String::new(),
                                suggestion: Some(
                                    "Consider larger buffer for user input, or validate input length"
                                        .to_string(),
                                ),
                                requires_manual_review: None,
                            });
                        }
                    }
                }
            }
        }

        // Recurse
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_fgets_truncation(&child, source, violations);
            }
        }
    }

    /// Check if there's a newline check after fgets
    fn has_newline_check(&self, node: &Node, source: &str) -> bool {
        // Look for strchr or memchr with '\n' in nearby code
        if let Some(parent) = node.parent() {
            if let Some(grandparent) = parent.parent() {
                let code_block = get_node_text(&grandparent, source);
                // Simple heuristic: check if there's a newline check nearby
                if code_block.contains("strchr") && code_block.contains("'\\n'") {
                    return true;
                }
                if code_block.contains("memchr") && code_block.contains("'\\n'") {
                    return true;
                }
                // Also check for direct newline comparison
                if code_block.contains("[") && code_block.contains("'\\n'") {
                    return true;
                }
            }
        }
        false
    }

    /// Get buffer size from fgets call if it's a literal
    fn get_buffer_size(&self, node: &Node, source: &str) -> Option<usize> {
        if let Some(args) = node.child_by_field_name("arguments") {
            let mut arg_count = 0;
            for i in 0..args.child_count() {
                if let Some(child) = args.child(i) {
                    if child.kind() != "(" && child.kind() != ")" && child.kind() != "," {
                        arg_count += 1;
                        if arg_count == 2 {
                            // Second argument is size
                            let size_text = get_node_text(&child, source).trim();
                            if let Ok(size) = size_text.parse::<usize>() {
                                return Some(size);
                            }
                        }
                    }
                }
            }
        }
        None
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_fgets_without_check() {
        let code = r#"
            void func(void) {
                char buf[10];
                fgets(buf, 10, stdin);
                printf("%s", buf);
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();

        let rule = Fio20C;
        let violations = rule.check(&root, code);

        assert!(
            !violations.is_empty(),
            "Should detect fgets without truncation check"
        );
    }

    #[test]
    fn test_fgets_with_newline_check() {
        let code = r#"
            void func(void) {
                char buf[100];
                fgets(buf, sizeof(buf), stdin);
                if (strchr(buf, '\n') == NULL) {
                    // Handle truncation
                }
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();

        let rule = Fio20C;
        let violations = rule.check(&root, code);

        // Should not flag when there's a newline check
        let no_truncation_warning = violations
            .iter()
            .all(|v| !v.message.contains("truncation check"));
        assert!(
            no_truncation_warning,
            "Should not flag fgets with newline check: {:?}",
            violations
        );
    }

    #[test]
    fn test_small_buffer() {
        let code = r#"
            void func(void) {
                char buf[8];
                fgets(buf, 8, stdin);
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();

        let rule = Fio20C;
        let violations = rule.check(&root, code);

        // Should flag small buffer size
        let has_small_buffer = violations
            .iter()
            .any(|v| v.message.contains("small buffer"));
        assert!(
            has_small_buffer,
            "Should detect small buffer size: {:?}",
            violations
        );
    }
}

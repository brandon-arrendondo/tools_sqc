//! ERR06-C: Understand the termination behavior of assert() and abort()
//!
//! The assert() macro calls abort(), which means cleanup functions registered
//! with atexit() are not called. When cleanup handlers are registered, use
//! explicit error checking instead of assert().
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! if (atexit(cleanup) != 0) { /* Handle error */ }
//! // ...
//! assert(condition);  // Bypasses atexit cleanup if assertion fails
//! ```
//!
//! **Compliant:**
//! ```c
//! if (atexit(cleanup) != 0) { /* Handle error */ }
//! // ...
//! if (!condition) {
//!     exit(EXIT_FAILURE);  // Calls atexit cleanup handlers
//! }
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Err06C;

impl CertRule for Err06C {
    fn rule_id(&self) -> &'static str {
        "ERR06-C"
    }

    fn description(&self) -> &'static str {
        "Understand the termination behavior of assert() and abort()"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "ERR06-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Check if atexit is used anywhere in the file
        let has_atexit = self.find_atexit_calls(node, source);

        if has_atexit {
            // Find all assert() calls and flag them
            self.find_assert_calls(node, source, &mut violations);
        }

        violations
    }
}

impl Err06C {
    /// Check if atexit() or at_quick_exit() is called anywhere in the tree
    fn find_atexit_calls(&self, node: &Node, source: &str) -> bool {
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = get_node_text(&function, source);
                if func_name == "atexit" || func_name == "at_quick_exit" {
                    return true;
                }
            }
        }

        // Recurse through children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if self.find_atexit_calls(&child, source) {
                    return true;
                }
            }
        }

        false
    }

    /// Find all assert() calls and create violations
    fn find_assert_calls(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = get_node_text(&function, source);
                if func_name == "assert" {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        message: "assert() used when atexit() cleanup handlers are registered. \
                                 assert() calls abort() which bypasses atexit cleanup."
                            .to_string(),
                        severity: self.severity(),
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        file_path: String::new(),
                        suggestion: Some(
                            "Replace assert() with explicit error checking: \
                             if (!condition) { exit(EXIT_FAILURE); }"
                                .to_string(),
                        ),
                        requires_manual_review: None,
                    });
                }
            }
        }

        // Recurse through children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.find_assert_calls(&child, source, violations);
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_assert_with_atexit() {
        let code = r#"
            void cleanup(void) { }
            int main(void) {
                if (atexit(cleanup) != 0) { }
                assert(1 == 1);
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();

        let rule = Err06C;
        let violations = rule.check(&root, code);

        assert!(
            !violations.is_empty(),
            "Should detect assert with atexit registered"
        );
    }

    #[test]
    fn test_exit_with_atexit() {
        let code = r#"
            void cleanup(void) { }
            int main(void) {
                if (atexit(cleanup) != 0) { }
                if (0) {
                    exit(EXIT_FAILURE);
                }
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();

        let rule = Err06C;
        let violations = rule.check(&root, code);

        assert!(
            violations.is_empty(),
            "Should not flag exit() with atexit: {:?}",
            violations
        );
    }

    #[test]
    fn test_assert_without_atexit() {
        let code = r#"
            int main(void) {
                assert(1 == 1);
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();

        let rule = Err06C;
        let violations = rule.check(&root, code);

        assert!(
            violations.is_empty(),
            "Should not flag assert without atexit: {:?}",
            violations
        );
    }
}

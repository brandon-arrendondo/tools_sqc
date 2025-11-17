//! API04-C: Provide a consistent and usable error-checking mechanism
//!
//! Functions should provide consistent and usable error-checking mechanisms.
//! Complex or inconsistent interfaces are sometimes ignored by programmers,
//! resulting in code that is not properly error checked.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! // strlcpy has inconsistent error checking - returns length, awkward comparison
//! if (strlcpy(dest, src, sizeof(dest)) >= sizeof(dest)) {
//!     // Handle error
//! }
//! ```
//!
//! **Compliant:**
//! ```c
//! // strcpy_m provides consistent error checking - returns errno_t
//! errno_t result = strcpy_m(dest, src);
//! if (result != 0) {
//!     // Handle error
//! }
//! ```

use crate::rules::{CertRule, RuleViolation};
use crate::manifest::{Severity, RuleCategory};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Api04C;

impl CertRule for Api04C {
    fn rule_id(&self) -> &'static str {
        "API04-C"
    }

    fn description(&self) -> &'static str {
        "Provide a consistent and usable error-checking mechanism"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "API04-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Api04C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check for strlcpy() and strlcat() function calls
        // These functions have inconsistent error-checking mechanisms
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = get_node_text(&function, source);

                if func_name == "strlcpy" || func_name == "strlcat" {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: self.severity(),
                        message: format!(
                            "Use of '{}' function with inconsistent error-checking mechanism",
                            func_name
                        ),
                        file_path: String::new(),
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        suggestion: Some(format!(
                            "Consider using functions with clearer return values like strcpy_s() or strcpy_m() \
                            that return errno_t for consistent error checking"
                        )),
                        ..Default::default()
                    });
                }
            }
        }

        // Recurse through children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_strlcpy_detected() {
        let source = r#"
            char dest[10];
            if (strlcpy(dest, src, sizeof(dest)) >= sizeof(dest)) {
                // error
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(source, None).unwrap();
        let root_node = tree.root_node();

        let rule = Api04C;
        let violations = rule.check(&root_node, source);

        assert!(!violations.is_empty(), "Should detect strlcpy violation");
        assert_eq!(violations[0].rule_id, "API04-C");
    }

    #[test]
    fn test_strcpy_m_allowed() {
        let source = r#"
            errno_t result = strcpy_m(dest, src);
            if (result != 0) {
                // error
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(source, None).unwrap();
        let root_node = tree.root_node();

        let rule = Api04C;
        let violations = rule.check(&root_node, source);

        assert!(violations.is_empty(), "strcpy_m should not trigger violation");
    }
}

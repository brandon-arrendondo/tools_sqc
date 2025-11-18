//! ARR02-C: Explicitly specify array bounds, even if implicitly defined by an initializer
//!
//! This rule detects array declarations where the bounds are implicitly defined
//! by the initializer rather than explicitly specified. While C allows omitting
//! array bounds when an initializer is present, explicit bounds improve code
//! clarity and prevent errors.
//!
//! # Violation Patterns
//!
//! ```c
//! int numbers[] = {1, 2, 3, 4, 5};  // VIOLATION: implicit bounds
//! char text[] = "Hello";            // VIOLATION: implicit bounds
//! int sparse[] = {[0] = 1, [5] = 42, [10] = 100};  // VIOLATION: designated initializers
//! ```
//!
//! # Compliant Solutions
//!
//! ```c
//! int numbers[5] = {1, 2, 3, 4, 5};  // OK: explicit bounds
//! char text[6] = "Hello";            // OK: explicit bounds
//! int sparse[11] = {[0] = 1, [5] = 42, [10] = 100};  // OK: explicit bounds
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Arr02C;

impl CertRule for Arr02C {
    fn rule_id(&self) -> &'static str {
        "ARR02-C"
    }

    fn description(&self) -> &'static str {
        "Explicitly specify array bounds, even if implicitly defined by an initializer"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "ARR02-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_declarations(node, source, &mut violations);
        violations
    }
}

impl Arr02C {
    /// Check all declarations for implicit array bounds
    fn check_declarations(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Only check declaration nodes
        if node.kind() == "declaration" {
            // Check if this declaration has an initializer
            if let Some(declarator) = node.child_by_field_name("declarator") {
                self.check_declarator(&declarator, source, violations);
            }
        }

        // Recursively check all child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_declarations(&child, source, violations);
            }
        }
    }

    /// Check a declarator for implicit array bounds
    fn check_declarator(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            "init_declarator" => {
                // This has an initializer, check the declarator part
                if let Some(declarator) = node.child_by_field_name("declarator") {
                    self.check_array_declarator(&declarator, source, violations);
                }
            }
            "array_declarator" => {
                self.check_array_declarator(node, source, violations);
            }
            _ => {
                // Recursively check children
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        self.check_declarator(&child, source, violations);
                    }
                }
            }
        }
    }

    /// Check if an array declarator has implicit bounds
    fn check_array_declarator(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if node.kind() == "array_declarator" {
            // Check if the size field is missing (implicit bounds)
            if node.child_by_field_name("size").is_none() {
                // Check if there's an empty bracket pair [] with no size
                let has_brackets = node
                    .children(&mut node.walk())
                    .any(|child| child.kind() == "[" || child.kind() == "]");

                if has_brackets {
                    let line = node.start_position().row + 1;
                    let column = node.start_position().column + 1;
                    let text = get_node_text(node, source);

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: self.severity(),
                        file_path: String::new(),
                        line,
                        column,
                        message: format!(
                            "Array declaration '{}' has implicit bounds; specify explicit size",
                            text
                        ),
                        suggestion: Some(
                            "Explicitly specify array bounds even when using an initializer"
                                .to_string(),
                        ),
                        requires_manual_review: None,
                    });
                }
            }

            // Also check nested declarators (for multi-dimensional arrays)
            if let Some(declarator) = node.child_by_field_name("declarator") {
                self.check_array_declarator(&declarator, source, violations);
            }
        }

        // Check children for nested array declarators
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "array_declarator" {
                    self.check_array_declarator(&child, source, violations);
                }
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_rule_metadata() {
        let rule = Arr02C;
        assert_eq!(rule.rule_id(), "ARR02-C");
        assert_eq!(rule.cert_id(), "ARR02-C");
        assert_eq!(
            rule.description(),
            "Explicitly specify array bounds, even if implicitly defined by an initializer"
        );
        assert_eq!(rule.severity(), Severity::Medium);
    }
}

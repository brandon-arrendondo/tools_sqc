//! FLP30-C: Do not use floating-point variables as loop counters
//!
//! Floating-point numbers cannot represent all decimal values exactly.
//! Using them as loop counters can cause loops to iterate an unexpected
//! number of times or create infinite loops.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! for (float x = 0.1f; x <= 1.0f; x += 0.1f) {
//!     // May iterate 9 or 10 times
//! }
//! ```
//!
//! **Compliant:**
//! ```c
//! for (size_t count = 1; count <= 10; ++count) {
//!     float x = count / 10.0f;
//!     // Iterates exactly 10 times
//! }
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Flp30C;

impl CertRule for Flp30C {
    fn rule_id(&self) -> &'static str {
        "FLP30-C"
    }

    fn description(&self) -> &'static str {
        "Do not use floating-point variables as loop counters"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "FLP30-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.find_floating_point_loops(node, source, &mut violations);
        violations
    }
}

impl Flp30C {
    /// Find for loops with floating-point loop counters
    fn find_floating_point_loops(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if node.kind() == "for_statement" {
            // Check the initialization part of the for loop
            if let Some(init) = node.child_by_field_name("initializer") {
                if let Some(fp_var) = self.has_floating_point_init(&init, source) {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        message: format!(
                            "Floating-point variable '{}' used as loop counter. \
                             Floating-point arithmetic may cause unexpected iteration counts.",
                            fp_var
                        ),
                        severity: self.severity(),
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        file_path: String::new(),
                        suggestion: Some(
                            "Use an integer counter and derive floating-point values inside the loop"
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
                self.find_floating_point_loops(&child, source, violations);
            }
        }
    }

    /// Check if for loop initialization declares or assigns a floating-point variable
    fn has_floating_point_init(&self, init: &Node, source: &str) -> Option<String> {
        // Check for declaration (e.g., "float x = 0.1f")
        if init.kind() == "declaration" {
            // Look for float or double type specifier
            if self.has_floating_point_type(init, source) {
                // Extract variable name
                return self.extract_declared_var_name(init, source);
            }
        }

        // Check for assignment expression (e.g., "x = 0.1f")
        if init.kind() == "assignment_expression" {
            let init_text = get_node_text(init, source);
            // Check if assigning a floating-point literal
            if init_text.contains(".") && (init_text.contains("f") || init_text.contains("F")) {
                return self.extract_assigned_var_name(init, source);
            }
        }

        None
    }

    /// Check if a declaration has float or double type
    fn has_floating_point_type(&self, decl: &Node, source: &str) -> bool {
        let decl_text = get_node_text(decl, source);
        decl_text.contains("float ") || decl_text.contains("double ")
    }

    /// Extract variable name from declaration
    fn extract_declared_var_name(&self, decl: &Node, source: &str) -> Option<String> {
        for i in 0..decl.child_count() {
            if let Some(child) = decl.child(i) {
                if child.kind() == "init_declarator" {
                    return self.find_identifier(&child, source);
                }
                if child.kind() == "identifier" {
                    return Some(get_node_text(&child, source).to_string());
                }
            }
        }
        None
    }

    /// Extract variable name from assignment expression
    fn extract_assigned_var_name(&self, assignment: &Node, source: &str) -> Option<String> {
        if let Some(left) = assignment.child_by_field_name("left") {
            return Some(get_node_text(&left, source).to_string());
        }
        None
    }

    /// Find identifier in node tree
    fn find_identifier(&self, node: &Node, source: &str) -> Option<String> {
        if node.kind() == "identifier" {
            return Some(get_node_text(node, source).to_string());
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if let Some(name) = self.find_identifier(&child, source) {
                    return Some(name);
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
    fn test_float_loop_counter() {
        let code = r#"
            void func(void) {
                for (float x = 0.1f; x <= 1.0f; x += 0.1f) {
                }
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();

        let rule = Flp30C;
        let violations = rule.check(&root, code);

        assert!(!violations.is_empty(), "Should detect float loop counter");
    }

    #[test]
    fn test_double_loop_counter() {
        let code = r#"
            void func(void) {
                for (double d = 0.0; d < 10.0; d += 0.5) {
                }
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();

        let rule = Flp30C;
        let violations = rule.check(&root, code);

        assert!(!violations.is_empty(), "Should detect double loop counter");
    }

    #[test]
    fn test_integer_loop_counter() {
        let code = r#"
            void func(void) {
                for (size_t count = 1; count <= 10; ++count) {
                    float x = count / 10.0f;
                }
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();

        let rule = Flp30C;
        let violations = rule.check(&root, code);

        assert!(
            violations.is_empty(),
            "Should not flag integer loop counter: {:?}",
            violations
        );
    }

    #[test]
    fn test_int_loop_counter() {
        let code = r#"
            void func(void) {
                for (int i = 0; i < 10; i++) {
                    double d = i * 0.5;
                }
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();

        let rule = Flp30C;
        let violations = rule.check(&root, code);

        assert!(
            violations.is_empty(),
            "Should not flag int loop counter: {:?}",
            violations
        );
    }
}

//! INT01-C: Use size_t or rsize_t for all integer values representing the size of an object
//!
//! Variables that hold object sizes should be size_t, not int or other types,
//! to avoid overflow and ensure sufficient precision.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! void copy(size_t n) {
//!     int i;  // Wrong type for size
//!     for (i = 0; i < n; ++i) { ... }  // int compared with size_t
//! }
//! ```
//!
//! **Compliant:**
//! ```c
//! void copy(size_t n) {
//!     size_t i;  // Correct type
//!     for (i = 0; i < n; ++i) { ... }
//! }
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use std::collections::{HashMap, HashSet};
use tree_sitter::Node;

pub struct Int01C;

impl CertRule for Int01C {
    fn rule_id(&self) -> &'static str {
        "INT01-C"
    }

    fn description(&self) -> &'static str {
        "Use size_t or rsize_t for all integer values representing the size of an object"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "INT01-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Track size_t parameters and variables
        let mut size_t_vars: HashSet<String> = HashSet::new();
        // Track int variables
        let mut int_vars: HashMap<String, (usize, usize)> = HashMap::new();

        // Find size_t variables (parameters and locals)
        self.find_size_t_vars(node, source, &mut size_t_vars);

        // Find int variables
        self.find_int_vars(node, source, &mut int_vars);

        // Find comparisons between int and size_t
        self.find_int_size_t_comparisons(node, source, &size_t_vars, &int_vars, &mut violations);

        violations
    }
}

impl Int01C {
    /// Find size_t variables (parameters and declarations)
    fn find_size_t_vars(&self, node: &Node, source: &str, size_t_vars: &mut HashSet<String>) {
        // Check function parameters
        if node.kind() == "parameter_declaration" {
            let decl_text = get_node_text(node, source);
            if decl_text.contains("size_t") {
                if let Some(var_name) = self.extract_param_name(node, source) {
                    size_t_vars.insert(var_name);
                }
            }
        }

        // Check variable declarations
        if node.kind() == "declaration" {
            let decl_text = get_node_text(node, source);
            if decl_text.contains("size_t ") {
                if let Some(var_name) = self.extract_var_name(node, source) {
                    size_t_vars.insert(var_name);
                }
            }
        }

        // Recurse
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.find_size_t_vars(&child, source, size_t_vars);
            }
        }
    }

    /// Find int variable declarations
    fn find_int_vars(
        &self,
        node: &Node,
        source: &str,
        int_vars: &mut HashMap<String, (usize, usize)>,
    ) {
        if node.kind() == "declaration" {
            let decl_text = get_node_text(node, source);
            // Check for int but not unsigned int, not size_t, not uint*
            if decl_text.trim().starts_with("int ")
                || decl_text.contains(" int ")
                || decl_text.contains("\tint ")
            {
                if !decl_text.contains("unsigned")
                    && !decl_text.contains("size_t")
                    && !decl_text.contains("uint")
                {
                    if let Some(var_name) = self.extract_var_name(node, source) {
                        int_vars.insert(
                            var_name,
                            (
                                node.start_position().row + 1,
                                node.start_position().column + 1,
                            ),
                        );
                    }
                }
            }
        }

        // Recurse
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.find_int_vars(&child, source, int_vars);
            }
        }
    }

    /// Find comparisons between int variables and size_t variables
    fn find_int_size_t_comparisons(
        &self,
        node: &Node,
        source: &str,
        size_t_vars: &HashSet<String>,
        int_vars: &HashMap<String, (usize, usize)>,
        violations: &mut Vec<RuleViolation>,
    ) {
        if node.kind() == "binary_expression" {
            if let Some(operator) = node.child_by_field_name("operator") {
                let op_text = get_node_text(&operator, source);
                // Check comparison operators
                if op_text == "<" || op_text == "<=" || op_text == ">" || op_text == ">=" {
                    if let (Some(left), Some(right)) = (
                        node.child_by_field_name("left"),
                        node.child_by_field_name("right"),
                    ) {
                        let left_text = get_node_text(&left, source);
                        let right_text = get_node_text(&right, source);

                        // Check if int var is compared with size_t var
                        if (int_vars.contains_key(left_text) && size_t_vars.contains(right_text))
                            || (int_vars.contains_key(right_text)
                                && size_t_vars.contains(left_text))
                        {
                            let int_var = if int_vars.contains_key(left_text) {
                                left_text
                            } else {
                                right_text
                            };
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                message: format!(
                                    "Variable '{}' of type int compared with size_t. \
                                     Use size_t for variables representing object sizes.",
                                    int_var
                                ),
                                severity: self.severity(),
                                line: node.start_position().row + 1,
                                column: node.start_position().column + 1,
                                file_path: String::new(),
                                suggestion: Some(format!(
                                    "Change declaration of '{}' from int to size_t",
                                    int_var
                                )),
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
                self.find_int_size_t_comparisons(&child, source, size_t_vars, int_vars, violations);
            }
        }
    }

    /// Extract parameter name
    fn extract_param_name(&self, param: &Node, source: &str) -> Option<String> {
        for i in 0..param.child_count() {
            if let Some(child) = param.child(i) {
                if child.kind() == "identifier" {
                    return Some(get_node_text(&child, source).to_string());
                }
            }
        }
        None
    }

    /// Extract variable name from declaration
    fn extract_var_name(&self, decl: &Node, source: &str) -> Option<String> {
        for i in 0..decl.child_count() {
            if let Some(child) = decl.child(i) {
                if child.kind() == "init_declarator" || child.kind() == "identifier" {
                    return self.find_identifier(&child, source);
                }
            }
        }
        None
    }

    /// Find identifier in node
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
    fn test_int_compared_with_size_t() {
        let code = r#"
            char *copy(size_t n) {
                int i;
                for (i = 0; i < n; ++i) { }
                return 0;
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();

        let rule = Int01C;
        let violations = rule.check(&root, code);

        assert!(
            !violations.is_empty(),
            "Should detect int compared with size_t"
        );
    }

    #[test]
    fn test_size_t_counter() {
        let code = r#"
            char *copy(size_t n) {
                size_t i;
                for (i = 0; i < n; ++i) { }
                return 0;
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();

        let rule = Int01C;
        let violations = rule.check(&root, code);

        assert!(
            violations.is_empty(),
            "Should not flag size_t counter: {:?}",
            violations
        );
    }
}

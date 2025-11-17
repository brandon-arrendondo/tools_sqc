//! EXP43-C: Avoid undefined behavior when using restrict-qualified pointers
//!
//! Restrict-qualified pointers must not alias each other. Assigning one restrict
//! pointer to another or passing overlapping memory to restrict parameters causes UB.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! int *restrict a;
//! int *restrict b;
//! a = b;  // Undefined behavior - restrict pointers aliasing
//! ```
//!
//! **Compliant:**
//! ```c
//! int *restrict a;
//! int *b;
//! a = b;  // OK - b is not restrict-qualified
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use std::collections::HashSet;
use tree_sitter::Node;

pub struct Exp43C;

impl CertRule for Exp43C {
    fn rule_id(&self) -> &'static str {
        "EXP43-C"
    }

    fn description(&self) -> &'static str {
        "Avoid undefined behavior when using restrict-qualified pointers"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "EXP43-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Track restrict-qualified pointer variables
        let mut restrict_vars: HashSet<String> = HashSet::new();

        // First pass: find restrict pointer declarations
        self.find_restrict_declarations(node, source, &mut restrict_vars);

        // Second pass: find assignments between restrict pointers
        self.find_restrict_assignments(node, source, &restrict_vars, &mut violations);

        // Third pass: find function calls with potentially overlapping restrict params
        self.find_overlapping_restrict_calls(node, source, &mut violations);

        violations
    }
}

impl Exp43C {
    /// Find restrict-qualified pointer declarations
    fn find_restrict_declarations(
        &self,
        node: &Node,
        source: &str,
        restrict_vars: &mut HashSet<String>,
    ) {
        if node.kind() == "declaration" {
            let decl_text = get_node_text(node, source);
            if decl_text.contains("restrict") {
                // Extract variable name
                if let Some(var_name) = self.extract_var_name(node, source) {
                    restrict_vars.insert(var_name);
                }
            }
        }

        // Recurse
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.find_restrict_declarations(&child, source, restrict_vars);
            }
        }
    }

    /// Find assignments between restrict-qualified pointers
    fn find_restrict_assignments(
        &self,
        node: &Node,
        source: &str,
        restrict_vars: &HashSet<String>,
        violations: &mut Vec<RuleViolation>,
    ) {
        if node.kind() == "assignment_expression" {
            if let (Some(left), Some(right)) = (
                node.child_by_field_name("left"),
                node.child_by_field_name("right"),
            ) {
                let left_text = get_node_text(&left, source);
                let right_text = get_node_text(&right, source);

                // Check if both sides are restrict-qualified variables
                if restrict_vars.contains(left_text) && restrict_vars.contains(right_text) {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        message: format!(
                            "Assignment from restrict pointer '{}' to restrict pointer '{}'. \
                             This causes undefined behavior due to pointer aliasing.",
                            right_text, left_text
                        ),
                        severity: self.severity(),
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        file_path: String::new(),
                        suggestion: Some(
                            "Use non-restrict pointers for aliased references".to_string(),
                        ),
                        requires_manual_review: None,
                    });
                }
            }
        }

        // Recurse
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.find_restrict_assignments(&child, source, restrict_vars, violations);
            }
        }
    }

    /// Find function calls with potentially overlapping restrict parameters
    fn find_overlapping_restrict_calls(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if node.kind() == "call_expression" {
            if let Some(args) = node.child_by_field_name("arguments") {
                let args_text = get_node_text(&args, source);
                // Check for common overlap patterns: same array with different offsets
                // e.g., f(50, d + 1, d) or memcpy(a, a+1, n)
                if self.has_overlapping_array_args(&args_text) {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        message: "Function call with potentially overlapping memory regions. \
                             If parameters are restrict-qualified, this causes undefined behavior."
                            .to_string(),
                        severity: self.severity(),
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        file_path: String::new(),
                        suggestion: Some(
                            "Ensure memory regions do not overlap when using restrict pointers"
                                .to_string(),
                        ),
                        requires_manual_review: None,
                    });
                }
            }
        }

        // Recurse
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.find_overlapping_restrict_calls(&child, source, violations);
            }
        }
    }

    /// Check if arguments contain overlapping array references
    fn has_overlapping_array_args(&self, args_text: &str) -> bool {
        // Simple heuristic: look for patterns like "d + 1, d" or "a, a+1"
        let parts: Vec<&str> = args_text.split(',').collect();
        if parts.len() >= 2 {
            for i in 0..parts.len() {
                for j in 0..parts.len() {
                    if i != j {
                        let a = parts[i].trim();
                        let b = parts[j].trim();
                        // Check if one is base and other is base + offset
                        if (a.contains('+') || a.contains('-'))
                            && !b.contains('+')
                            && !b.contains('-')
                        {
                            // Extract base from "base + offset"
                            let base = a
                                .split(|c| c == '+' || c == '-')
                                .next()
                                .unwrap_or("")
                                .trim();
                            if base == b || b.starts_with(base) {
                                return true;
                            }
                        }
                    }
                }
            }
        }
        false
    }

    /// Extract variable name from declaration
    fn extract_var_name(&self, decl: &Node, source: &str) -> Option<String> {
        for i in 0..decl.child_count() {
            if let Some(child) = decl.child(i) {
                if child.kind() == "init_declarator" || child.kind() == "pointer_declarator" {
                    return self.find_identifier(&child, source);
                }
                if child.kind() == "identifier" {
                    return Some(get_node_text(&child, source).to_string());
                }
            }
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
    fn test_restrict_pointer_assignment() {
        let code = r#"
            int *restrict a;
            int *restrict b;
            int main(void) {
                a = b;
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();

        let rule = Exp43C;
        let violations = rule.check(&root, code);

        assert!(
            !violations.is_empty(),
            "Should detect restrict pointer assignment"
        );
    }

    #[test]
    fn test_overlapping_array_call() {
        let code = r#"
            void f(int n, int *restrict p, const int *restrict q);
            void g(void) {
                extern int d[100];
                f(50, d + 1, d);
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();

        let rule = Exp43C;
        let violations = rule.check(&root, code);

        assert!(
            !violations.is_empty(),
            "Should detect overlapping restrict parameters"
        );
    }

    #[test]
    fn test_non_overlapping_call() {
        let code = r#"
            void f(int n, int *restrict p, const int *restrict q);
            void g(void) {
                int a[50];
                int b[50];
                f(50, a, b);
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();

        let rule = Exp43C;
        let violations = rule.check(&root, code);

        assert!(
            violations.is_empty(),
            "Should not flag non-overlapping arrays: {:?}",
            violations
        );
    }
}

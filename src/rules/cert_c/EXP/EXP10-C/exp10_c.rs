//! EXP10-C: Do not depend on the order of evaluation of subexpressions or the order
//! in which side effects take place
//!
//! The order of evaluation of subexpressions is unspecified in C. Multiple function
//! calls with side effects in the same expression can lead to undefined behavior.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! int x = f(1) + f(2);  // Order of f(1) and f(2) is unspecified
//! ```
//!
//! **Compliant:**
//! ```c
//! int x = f(1);
//! x += f(2);  // Side effects are sequenced
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Exp10C;

impl CertRule for Exp10C {
    fn rule_id(&self) -> &'static str {
        "EXP10-C"
    }

    fn description(&self) -> &'static str {
        "Do not depend on the order of evaluation of subexpressions or the order in which side effects take place"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "EXP10-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.find_unsequenced_side_effects(node, source, &mut violations);
        violations
    }
}

impl Exp10C {
    /// Find expressions with multiple function calls that could have unsequenced side effects
    fn find_unsequenced_side_effects(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Check binary expressions for multiple function calls
        if node.kind() == "binary_expression" {
            let call_count = self.count_function_calls(node);
            if call_count >= 2 {
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    message: format!(
                        "Expression contains {} function calls with potentially unsequenced side effects. \
                         Order of evaluation is unspecified.",
                        call_count
                    ),
                    severity: self.severity(),
                    line: node.start_position().row + 1,
                    column: node.start_position().column + 1,
                    file_path: String::new(),
                    suggestion: Some(
                        "Separate function calls into distinct statements to ensure defined evaluation order"
                            .to_string(),
                    ),
                    requires_manual_review: None,
                });
            }
        }

        // Check function call expressions that themselves contain multiple function calls
        if node.kind() == "call_expression" {
            if let Some(args) = node.child_by_field_name("arguments") {
                let call_count = self.count_function_calls(&args);
                if call_count >= 2 {
                    let node_text = get_node_text(node, source);
                    // Only flag if this looks like the complex pattern with array subscript
                    if node_text.contains("[") && node_text.contains("]") {
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            message: format!(
                                "Function call with {} nested function calls. \
                                 Order of evaluation is unspecified.",
                                call_count
                            ),
                            severity: self.severity(),
                            line: node.start_position().row + 1,
                            column: node.start_position().column + 1,
                            file_path: String::new(),
                            suggestion: Some(
                                "Store function results in temporary variables before use"
                                    .to_string(),
                            ),
                            requires_manual_review: None,
                        });
                    }
                }
            }
        }

        // Also check for complex pointer subscript patterns
        if node.kind() == "subscript_expression" {
            let total_calls = self.count_function_calls(node);
            if total_calls >= 2 {
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    message: format!(
                        "Subscript expression with {} function calls. \
                         Order of evaluation is unspecified.",
                        total_calls
                    ),
                    severity: self.severity(),
                    line: node.start_position().row + 1,
                    column: node.start_position().column + 1,
                    file_path: String::new(),
                    suggestion: Some(
                        "Store intermediate results in temporary variables".to_string(),
                    ),
                    requires_manual_review: None,
                });
            }
        }

        // Recurse through children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.find_unsequenced_side_effects(&child, source, violations);
            }
        }
    }

    /// Count the number of function calls in a subtree
    fn count_function_calls(&self, node: &Node) -> usize {
        let mut count = 0;

        if node.kind() == "call_expression" {
            count += 1;
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                count += self.count_function_calls(&child);
            }
        }

        count
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_multiple_function_calls_in_binary_expr() {
        let code = r#"
            int g;
            int f(int i) { g = i; return i; }
            int main(void) {
                int x = f(1) + f(2);
                return 0;
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();

        let rule = Exp10C;
        let violations = rule.check(&root, code);

        assert!(
            !violations.is_empty(),
            "Should detect multiple function calls in binary expression"
        );
    }

    #[test]
    fn test_separated_function_calls() {
        let code = r#"
            int g;
            int f(int i) { g = i; return i; }
            int main(void) {
                int x = f(1);
                x += f(2);
                return 0;
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();

        let rule = Exp10C;
        let violations = rule.check(&root, code);

        assert!(
            violations.is_empty(),
            "Should not flag separated function calls: {:?}",
            violations
        );
    }

    #[test]
    fn test_single_function_call() {
        let code = r#"
            int main(void) {
                int x = f(1) + 2;
                return 0;
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();

        let rule = Exp10C;
        let violations = rule.check(&root, code);

        assert!(
            violations.is_empty(),
            "Should not flag single function call: {:?}",
            violations
        );
    }
}

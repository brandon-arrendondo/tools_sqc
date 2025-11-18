//! API07-C: Enforce type safety
//!
//! Functions should guarantee that any object returned by the function, or any
//! modified value referenced by a pointer argument, is a valid object of the
//! function return type or argument type. This rule specifically targets
//! functions like `strncpy()` that may not guarantee null-termination.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! char a[NTBS_SIZE];
//! char *source;
//! // strncpy doesn't guarantee null termination
//! strncpy(a, source, 5);
//! ```
//!
//! **Compliant:**
//! ```c
//! char a[NTBS_SIZE];
//! char *source;
//! // strncpy_s guarantees null termination (C11 Annex K)
//! errno_t err = strncpy_s(a, sizeof(a), source, 5);
//! ```

use crate::manifest::{RuleCategory, Severity};
use crate::rules::{CertRule, RuleViolation};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Api07C;

impl CertRule for Api07C {
    fn rule_id(&self) -> &'static str {
        "API07-C"
    }

    fn description(&self) -> &'static str {
        "Enforce type safety"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "API07-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Api07C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check for strncpy() calls
        if node.kind() == "call_expression" {
            self.check_strncpy_call(node, source, violations);
        }

        // Recurse through children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }

    fn check_strncpy_call(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(function_node) = node.child_by_field_name("function") {
            let function_name = get_node_text(&function_node, source);

            // Check if this is a strncpy call
            if function_name == "strncpy" {
                let start_point = node.start_position();
                let call_text = get_node_text(node, source);

                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::Medium,
                    message: format!(
                        "Use of strncpy() '{}' does not guarantee null-termination, violating type safety",
                        call_text
                    ),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some(
                        "Use strncpy_s() (C11 Annex K) or manually null-terminate the destination buffer".to_string()
                    ),
                    ..Default::default()
                });
            }
        }
    }
}

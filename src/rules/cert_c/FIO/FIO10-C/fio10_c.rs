//! FIO10-C: Take care when using the rename() function
//!
//! The behavior of rename() when the destination file exists is implementation-defined.
//! On POSIX systems, the destination is removed. On Windows, rename() fails.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! rename(src, dst);  // No error checking, destination may exist
//! ```
//!
//! **Compliant:**
//! ```c
//! if (rename(src, dst) != 0) {
//!     // Handle error - destination may exist or other failure
//! }
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Fio10C;

impl CertRule for Fio10C {
    fn rule_id(&self) -> &'static str {
        "FIO10-C"
    }

    fn description(&self) -> &'static str {
        "Take care when using the rename() function"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "FIO10-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.find_unchecked_rename(node, source, &mut violations);
        violations
    }
}

impl Fio10C {
    /// Find rename() calls without error checking
    fn find_unchecked_rename(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Look for expression_statement containing rename() call
        if node.kind() == "expression_statement" {
            if let Some(expr) = node.child(0) {
                if self.is_rename_call(&expr, source) {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        message: "rename() called without checking return value. \
                                  Behavior when destination exists is implementation-defined."
                            .to_string(),
                        severity: self.severity(),
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        file_path: String::new(),
                        suggestion: Some(
                            "Check rename() return value (0 = success, non-zero = error) \
                             and handle errors appropriately"
                                .to_string(),
                        ),
                        requires_manual_review: None,
                    });
                }
            }
        }

        // Also check for TOCTOU: access() or stat() before rename()
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = get_node_text(&function, source);
                if func_name == "access" || func_name == "stat" || func_name == "lstat" {
                    // Check if this is followed by rename() - potential TOCTOU
                    if self.is_followed_by_rename(node, source) {
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            message: format!(
                                "{}() followed by rename() creates TOCTOU race condition",
                                func_name
                            ),
                            severity: self.severity(),
                            line: node.start_position().row + 1,
                            column: node.start_position().column + 1,
                            file_path: String::new(),
                            suggestion: Some(
                                "Avoid check-then-rename pattern. Instead, try rename() and \
                                 handle errors, or use POSIX link()+unlink() for atomicity"
                                    .to_string(),
                            ),
                            requires_manual_review: None,
                        });
                    }
                }
            }
        }

        // Recurse
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.find_unchecked_rename(&child, source, violations);
            }
        }
    }

    /// Check if node is a rename() call
    fn is_rename_call(&self, node: &Node, source: &str) -> bool {
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                return get_node_text(&function, source) == "rename";
            }
        }
        false
    }

    /// Check if access/stat is followed by rename in same block
    fn is_followed_by_rename(&self, node: &Node, source: &str) -> bool {
        // Get parent to find sibling statements
        if let Some(parent) = node.parent() {
            if let Some(grandparent) = parent.parent() {
                let mut found_current = false;
                for i in 0..grandparent.child_count() {
                    if let Some(child) = grandparent.child(i) {
                        if found_current {
                            // Look for rename in subsequent siblings
                            if self.contains_rename(&child, source) {
                                return true;
                            }
                        }
                        if child.id() == parent.id() {
                            found_current = true;
                        }
                    }
                }
            }
        }
        false
    }

    /// Check if node or its children contain rename() call
    fn contains_rename(&self, node: &Node, source: &str) -> bool {
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                if get_node_text(&function, source) == "rename" {
                    return true;
                }
            }
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if self.contains_rename(&child, source) {
                    return true;
                }
            }
        }
        false
    }
}

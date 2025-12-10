//! MEM01-C: Store a new value in pointers immediately after free()
//!
//! This rule detects calls to free() where the pointer is not set to NULL
//! immediately after, potentially leading to double-free vulnerabilities.
//!
//! VIOLATIONS:
//! - free(ptr) not immediately followed by ptr = NULL
//!
//! COMPLIANT:
//! - free(ptr) immediately followed by ptr = NULL

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Mem01C;

impl CertRule for Mem01C {
    fn rule_id(&self) -> &'static str {
        "MEM01-C"
    }

    fn description(&self) -> &'static str {
        "Store a new value in pointers immediately after free()"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "MEM01-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Mem01C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Look for compound statements (blocks) and check for free() calls
        if node.kind() == "compound_statement" {
            self.check_compound_statement(node, source, violations);
        }

        // Recurse into children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }

    fn check_compound_statement(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Collect all expression statements in order
        let mut statements: Vec<Node> = Vec::new();
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "expression_statement" {
                    statements.push(child);
                }
            }
        }

        // Check each free() call to see if it's followed by ptr = NULL
        for (idx, stmt) in statements.iter().enumerate() {
            if let Some(free_ptr) = self.get_free_pointer(stmt, source) {
                // Check if the next statement is ptr = NULL
                let has_null_assignment = if idx + 1 < statements.len() {
                    self.is_null_assignment(&statements[idx + 1], source, &free_ptr)
                } else {
                    false
                };

                if !has_null_assignment {
                    let pos = stmt.start_position();
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Pointer '{}' not set to NULL after free(); may cause double-free",
                            free_ptr
                        ),
                        file_path: String::new(),
                        line: pos.row + 1,
                        column: pos.column + 1,
                        suggestion: Some(format!(
                            "Add '{} = NULL;' immediately after free({})",
                            free_ptr, free_ptr
                        )),
                        ..Default::default()
                    });
                }
            }
        }
    }

    fn get_free_pointer(&self, stmt: &Node, source: &str) -> Option<String> {
        // Look for call_expression with free()
        for i in 0..stmt.child_count() {
            if let Some(child) = stmt.child(i) {
                if child.kind() == "call_expression" {
                    if let Some(func) = child.child_by_field_name("function") {
                        let func_name = get_node_text(&func, source);
                        if func_name == "free" {
                            // Get the argument (the pointer being freed)
                            if let Some(args) = child.child_by_field_name("arguments") {
                                for j in 0..args.child_count() {
                                    if let Some(arg) = args.child(j) {
                                        if arg.kind() != "("
                                            && arg.kind() != ")"
                                            && arg.kind() != ","
                                        {
                                            return Some(get_node_text(&arg, source).to_string());
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        None
    }

    fn is_null_assignment(&self, stmt: &Node, source: &str, ptr_name: &str) -> bool {
        // Check if this statement is ptr = NULL
        for i in 0..stmt.child_count() {
            if let Some(child) = stmt.child(i) {
                if child.kind() == "assignment_expression" {
                    if let (Some(left), Some(right)) = (
                        child.child_by_field_name("left"),
                        child.child_by_field_name("right"),
                    ) {
                        let left_name = get_node_text(&left, source);
                        let right_val = get_node_text(&right, source);
                        if left_name == ptr_name && (right_val == "NULL" || right_val == "0") {
                            return true;
                        }
                    }
                }
            }
        }
        false
    }
}

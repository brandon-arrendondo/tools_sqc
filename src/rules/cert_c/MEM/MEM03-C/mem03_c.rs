//! MEM03-C: Clear sensitive information stored in reusable resources
//!
//! This rule detects free() or realloc() calls on memory that may contain
//! sensitive data without first clearing the memory with memset/memset_s.
//!
//! VIOLATIONS:
//! - free(ptr) without prior memset/memset_s/explicit_bzero to clear memory
//! - realloc(ptr, size) - old memory may not be cleared before being freed
//!
//! COMPLIANT:
//! - memset/memset_s/explicit_bzero called before free()
//! - Using calloc() for new allocations (initializes to zero)

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use std::collections::HashSet;
use tree_sitter::Node;

pub struct Mem03C;

const CLEAR_FUNCS: &[&str] = &[
    "memset",
    "memset_s",
    "explicit_bzero",
    "bzero",
    "SecureZeroMemory",
];

impl CertRule for Mem03C {
    fn rule_id(&self) -> &'static str {
        "MEM03-C"
    }

    fn description(&self) -> &'static str {
        "Clear sensitive information stored in reusable resources"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "MEM03-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Mem03C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check function definitions for free/realloc patterns
        if node.kind() == "function_definition" {
            self.check_function(node, source, violations);
        }

        // Recurse
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }

    fn check_function(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Get compound statement (function body)
        if let Some(body) = node.child_by_field_name("body") {
            // Track which pointers have been cleared
            let mut cleared_ptrs: HashSet<String> = HashSet::new();

            self.analyze_block(&body, source, violations, &mut cleared_ptrs);
        }
    }

    fn analyze_block(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        cleared_ptrs: &mut HashSet<String>,
    ) {
        // Process statements in order
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.process_statement(&child, source, violations, cleared_ptrs);
            }
        }
    }

    fn process_statement(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        cleared_ptrs: &mut HashSet<String>,
    ) {
        let kind = node.kind();

        // Check for memset/memset_s calls (clear operations)
        if kind == "expression_statement" {
            if let Some(ptr) = self.get_clear_call_ptr(node, source) {
                cleared_ptrs.insert(ptr);
            }
        }

        // Check for free() calls without prior clearing
        if kind == "expression_statement" {
            if let Some(ptr) = self.get_free_ptr(node, source) {
                if !cleared_ptrs.contains(&ptr) {
                    let pos = node.start_position();
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::Medium,
                        message: format!(
                            "Pointer '{}' freed without clearing sensitive data first",
                            ptr
                        ),
                        file_path: String::new(),
                        line: pos.row + 1,
                        column: pos.column + 1,
                        suggestion: Some(format!(
                            "Add 'memset({}, 0, size);' before free({}) to clear sensitive data",
                            ptr, ptr
                        )),
                        ..Default::default()
                    });
                }
            }
        }

        // Check for realloc() calls (always a potential issue)
        if kind == "expression_statement" || kind == "declaration" || kind == "init_declarator" {
            if let Some(ptr) = self.get_realloc_ptr(node, source) {
                if !cleared_ptrs.contains(&ptr) {
                    let pos = node.start_position();
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::Medium,
                        message: format!(
                            "realloc() on '{}' may leak sensitive data from old memory",
                            ptr
                        ),
                        file_path: String::new(),
                        line: pos.row + 1,
                        column: pos.column + 1,
                        suggestion: Some(
                            "Consider allocating new memory, copying, clearing old, then freeing"
                                .to_string(),
                        ),
                        ..Default::default()
                    });
                }
            }
        }

        // Recurse into nested blocks
        if kind == "compound_statement" {
            self.analyze_block(node, source, violations, cleared_ptrs);
        } else if kind == "if_statement" || kind == "while_statement" || kind == "for_statement" {
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() == "compound_statement" {
                        // Create a copy for nested scope
                        let mut nested_cleared = cleared_ptrs.clone();
                        self.analyze_block(&child, source, violations, &mut nested_cleared);
                    }
                }
            }
        }
    }

    fn get_clear_call_ptr(&self, node: &Node, source: &str) -> Option<String> {
        // Look for memset/memset_s/explicit_bzero calls
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "call_expression" {
                    if let Some(func) = child.child_by_field_name("function") {
                        let func_name = get_node_text(&func, source);
                        if CLEAR_FUNCS.contains(&func_name) {
                            // Get the first argument (pointer being cleared)
                            if let Some(args) = child.child_by_field_name("arguments") {
                                for j in 0..args.child_count() {
                                    if let Some(arg) = args.child(j) {
                                        let arg_kind = arg.kind();
                                        if arg_kind != "(" && arg_kind != ")" && arg_kind != "," {
                                            // Extract the base pointer from the argument
                                            return Some(self.extract_base_ptr(&arg, source));
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

    fn extract_base_ptr(&self, node: &Node, source: &str) -> String {
        // Handle cast_expression like (volatile char *)ptr
        if node.kind() == "cast_expression" {
            // Find the value being cast (typically the last child that's not type_descriptor)
            for i in (0..node.child_count()).rev() {
                if let Some(child) = node.child(i) {
                    let kind = child.kind();
                    if kind != "type_descriptor" && kind != "(" && kind != ")" {
                        return self.extract_base_ptr(&child, source);
                    }
                }
            }
        }
        // Base case: identifier or other expression
        get_node_text(node, source).to_string()
    }

    fn get_free_ptr(&self, node: &Node, source: &str) -> Option<String> {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "call_expression" {
                    if let Some(func) = child.child_by_field_name("function") {
                        let func_name = get_node_text(&func, source);
                        if func_name == "free" {
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

    fn get_realloc_ptr(&self, node: &Node, source: &str) -> Option<String> {
        // Find realloc call and return the pointer being reallocated
        self.find_realloc_in_node(node, source)
    }

    fn find_realloc_in_node(&self, node: &Node, source: &str) -> Option<String> {
        if node.kind() == "call_expression" {
            if let Some(func) = node.child_by_field_name("function") {
                let func_name = get_node_text(&func, source);
                if func_name == "realloc" {
                    // Get first argument (pointer being reallocated)
                    if let Some(args) = node.child_by_field_name("arguments") {
                        for j in 0..args.child_count() {
                            if let Some(arg) = args.child(j) {
                                if arg.kind() != "(" && arg.kind() != ")" && arg.kind() != "," {
                                    return Some(get_node_text(&arg, source).to_string());
                                }
                            }
                        }
                    }
                }
            }
        }

        // Recurse
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if let Some(ptr) = self.find_realloc_in_node(&child, source) {
                    return Some(ptr);
                }
            }
        }
        None
    }
}

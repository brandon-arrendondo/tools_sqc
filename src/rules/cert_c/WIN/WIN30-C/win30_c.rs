//! WIN30-C: Properly pair allocation and deallocation functions
//!
//! Windows provides several memory allocation APIs that must be paired with
//! their corresponding deallocation functions. Mispairing causes memory
//! corruption or out-of-bounds access.
//!
//! ## Non-compliant example:
//!
//! ```c
//! // FormatMessage allocates with LocalAlloc
//! FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER, ...);
//! GlobalFree(buf);  // WRONG - should be LocalFree
//!
//! // malloc must pair with free, not LocalFree
//! char *p = malloc(100);
//! LocalFree(p);  // WRONG - should be free
//! ```
//!
//! ## Compliant solution:
//!
//! ```c
//! // Use LocalFree for FormatMessage allocations
//! FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER, ...);
//! LocalFree(buf);  // Correct
//!
//! // Use free for malloc
//! char *p = malloc(100);
//! free(p);  // Correct
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Win30C;

impl Win30C {
    pub fn new() -> Self {
        Self
    }

    /// Check for potentially mismatched allocation/deallocation pairs
    fn check_deallocation_call(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if node.kind() != "call_expression" {
            return;
        }

        if let Some(function_node) = node.child_by_field_name("function") {
            let function_name = get_node_text(&function_node, source);

            match &function_name[..] {
                "free"
                | "LocalFree"
                | "GlobalFree"
                | "VirtualFree"
                | "VirtualFreeEx"
                | "HeapFree"
                | "FreeUserPhysicalPages" => {
                    self.check_deallocation_usage(&function_name, node, source, violations);
                }
                _ => {}
            }
        }
    }

    /// Provide guidance on proper pairing for deallocators
    fn check_deallocation_usage(
        &self,
        deallocator: &str,
        node: &Node,
        _source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let guidance = match deallocator {
            "free" => {
                "free() should only be used for memory allocated with malloc() or realloc(). Do not use it for LocalAlloc(), GlobalAlloc(), VirtualAlloc(), or HeapAlloc() allocations."
            }
            "LocalFree" => {
                "LocalFree() should only be used for memory allocated with LocalAlloc(), LocalReAlloc(), or FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER). Do not use it for malloc(), GlobalAlloc(), VirtualAlloc(), or HeapAlloc() allocations."
            }
            "GlobalFree" => {
                "GlobalFree() should only be used for memory allocated with GlobalAlloc() or GlobalReAlloc(). Do not use it for malloc(), LocalAlloc(), VirtualAlloc(), or HeapAlloc() allocations."
            }
            "VirtualFree" | "VirtualFreeEx" => {
                "VirtualFree() should only be used for memory allocated with VirtualAlloc(), VirtualAllocEx(), or VirtualAllocExNuma(). Do not use it for malloc(), LocalAlloc(), GlobalAlloc(), or HeapAlloc() allocations."
            }
            "HeapFree" => {
                "HeapFree() should only be used for memory allocated with HeapAlloc() or HeapReAlloc(). Do not use it for malloc(), LocalAlloc(), GlobalAlloc(), or VirtualAlloc() allocations."
            }
            "FreeUserPhysicalPages" => {
                "FreeUserPhysicalPages() should only be used for memory allocated with AllocateUserPhysicalPages() or AllocateUserPhysicalPagesNuma()."
            }
            _ => return,
        };

        violations.push(RuleViolation {
            rule_id: self.rule_id().to_string(),
            severity: Severity::Low,
            message: format!(
                "Using {} for deallocation. Ensure allocation/deallocation functions are properly paired. {}",
                deallocator, guidance
            ),
            file_path: String::new(),
            line: node.start_position().row + 1,
            column: node.start_position().column + 1,
            suggestion: Some(
                format!("Verify that {} is the correct deallocation function for this allocation. Windows memory APIs require specific pairings (malloc/free, LocalAlloc/LocalFree, GlobalAlloc/GlobalFree, etc.).", deallocator)
            ),
            ..Default::default()
        });
    }

    /// Check for FormatMessage with FORMAT_MESSAGE_ALLOCATE_BUFFER
    fn check_format_message_call(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if node.kind() != "call_expression" {
            return;
        }

        if let Some(function_node) = node.child_by_field_name("function") {
            let function_name = get_node_text(&function_node, source);

            if &function_name[..] == "FormatMessage" {
                // Check if first argument contains FORMAT_MESSAGE_ALLOCATE_BUFFER
                if let Some(args_node) = node.child_by_field_name("arguments") {
                    if let Some(first_arg) = self.get_first_argument(&args_node) {
                        let arg_text = get_node_text(&first_arg, source);

                        if arg_text.contains("FORMAT_MESSAGE_ALLOCATE_BUFFER") {
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                severity: Severity::Medium,
                                message: format!(
                                    "FormatMessage() called with FORMAT_MESSAGE_ALLOCATE_BUFFER. The allocated buffer MUST be freed with LocalFree(), not free(), GlobalFree(), or other deallocators."
                                ),
                                file_path: String::new(),
                                line: node.start_position().row + 1,
                                column: node.start_position().column + 1,
                                suggestion: Some(
                                    "Per Microsoft documentation, buffers allocated by FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER) must be freed with LocalFree()."
                                        .to_string(),
                                ),
                                ..Default::default()
                            });
                        }
                    }
                }
            }
        }
    }

    /// Get the first argument from an argument list node
    fn get_first_argument<'a>(&self, args_node: &Node<'a>) -> Option<Node<'a>> {
        for i in 0..args_node.child_count() {
            if let Some(child) = args_node.child(i) {
                // Skip '(' and ')' and ',' tokens
                if child.kind() != "(" && child.kind() != ")" && child.kind() != "," {
                    return Some(child);
                }
            }
        }
        None
    }
}

impl CertRule for Win30C {
    fn rule_id(&self) -> &'static str {
        "WIN30-C"
    }

    fn description(&self) -> &'static str {
        "Properly pair allocation and deallocation functions"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "WIN30-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Win30C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // First pass: collect FormatMessage calls with FORMAT_MESSAGE_ALLOCATE_BUFFER
        // and check if wrong deallocator is used
        self.check_format_message_mismatch(node, source, violations);

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }

    /// Check for FormatMessage with FORMAT_MESSAGE_ALLOCATE_BUFFER followed by wrong deallocator
    fn check_format_message_mismatch(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Find the root scope
        let scope = self.find_scope(node);
        if scope.is_none() {
            return;
        }
        let scope = scope.unwrap();

        // Check if this scope has FormatMessage with FORMAT_MESSAGE_ALLOCATE_BUFFER
        let has_format_message_alloc = self.contains_format_message_alloc(&scope, source);

        if has_format_message_alloc {
            // Check if GlobalFree is used (wrong for FormatMessage)
            if self.contains_global_free(&scope, source) {
                // Find the GlobalFree call and report violation
                self.report_global_free_violation(&scope, source, violations);
            }
        }
    }

    fn find_scope<'a>(&self, node: &Node<'a>) -> Option<Node<'a>> {
        let mut current = *node;
        while let Some(parent) = current.parent() {
            if parent.kind() == "compound_statement" || parent.kind() == "translation_unit" {
                return Some(parent);
            }
            current = parent;
        }
        Some(current)
    }

    fn contains_format_message_alloc(&self, node: &Node, source: &str) -> bool {
        if node.kind() == "call_expression" {
            if let Some(function_node) = node.child_by_field_name("function") {
                let function_name = get_node_text(&function_node, source);
                if &function_name[..] == "FormatMessage" {
                    if let Some(args_node) = node.child_by_field_name("arguments") {
                        if let Some(first_arg) = self.get_first_argument(&args_node) {
                            let arg_text = get_node_text(&first_arg, source);
                            if arg_text.contains("FORMAT_MESSAGE_ALLOCATE_BUFFER") {
                                return true;
                            }
                        }
                    }
                }
            }
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if self.contains_format_message_alloc(&child, source) {
                    return true;
                }
            }
        }
        false
    }

    fn contains_global_free(&self, node: &Node, source: &str) -> bool {
        if node.kind() == "call_expression" {
            if let Some(function_node) = node.child_by_field_name("function") {
                let function_name = get_node_text(&function_node, source);
                if &function_name[..] == "GlobalFree" {
                    return true;
                }
            }
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if self.contains_global_free(&child, source) {
                    return true;
                }
            }
        }
        false
    }

    fn report_global_free_violation(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if node.kind() == "call_expression" {
            if let Some(function_node) = node.child_by_field_name("function") {
                let function_name = get_node_text(&function_node, source);
                if &function_name[..] == "GlobalFree" {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: self.severity(),
                        message: "Using GlobalFree() for buffer allocated by FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER). Use LocalFree() instead.".to_string(),
                        file_path: String::new(),
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        suggestion: Some(
                            "Change GlobalFree() to LocalFree(). Per Microsoft documentation, buffers allocated by FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER) must be freed with LocalFree().".to_string()
                        ),
                        ..Default::default()
                    });
                    return;
                }
            }
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.report_global_free_violation(&child, source, violations);
            }
        }
    }
}

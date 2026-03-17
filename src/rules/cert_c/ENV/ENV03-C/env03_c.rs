//! ENV03-C: Sanitize the environment when invoking external programs
//!
//! This rule detects when external programs are invoked via system() or popen()
//! without first sanitizing the environment (clearing or setting PATH/IFS).
//!
//! Resolves macro aliases (#define SYSTEM system) via project context.
//!
//! Sanitization is checked per function scope: clearenv()/setenv()/putenv()
//! must appear in the same function as the system()/popen() call.
//!
//! Violation pattern:
//!   system("/bin/ls");  // No clearenv() or setenv("PATH"/"IFS") in this function
//!
//! Compliant patterns:
//!   clearenv();
//!   setenv("PATH", "/bin", 1);
//!   setenv("IFS", " \t\n", 1);
//!   system("/bin/ls");  // Environment sanitized in same function

use crate::analyze::const_eval;
use crate::analyze::context::ProjectContext;
use crate::manifest::{RuleCategory, Severity};
use crate::rules::{CertRule, RuleViolation};
use crate::utility::cert_c::ast_utils::get_node_text;
use std::cell::RefCell;
use std::collections::HashMap;
use tree_sitter::Node;

pub struct Env03C {
    project_aliases: RefCell<HashMap<String, String>>,
    current_aliases: RefCell<HashMap<String, String>>,
}

impl Env03C {
    pub fn new() -> Self {
        Self {
            project_aliases: RefCell::new(HashMap::new()),
            current_aliases: RefCell::new(HashMap::new()),
        }
    }
}

impl Default for Env03C {
    fn default() -> Self {
        Self::new()
    }
}

impl CertRule for Env03C {
    fn rule_id(&self) -> &'static str {
        "ENV03-C"
    }

    fn description(&self) -> &'static str {
        "Sanitize the environment when invoking external programs"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "ENV03-C"
    }

    fn set_project_context(&self, context: &ProjectContext) {
        *self.project_aliases.borrow_mut() = context.macro_aliases.clone();
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        // Merge project-level aliases with per-file aliases
        let mut aliases = self.project_aliases.borrow().clone();
        aliases.extend(const_eval::collect_macro_aliases(node, source));
        *self.current_aliases.borrow_mut() = aliases;

        let mut violations = Vec::new();
        self.check_all_calls(node, source, &mut violations);
        violations
    }
}

impl Env03C {
    /// Resolve a function name through macro aliases.
    fn resolve_name(&self, name: &str) -> String {
        let aliases = self.current_aliases.borrow();
        if let Some(target) = aliases.get(name) {
            target.clone()
        } else {
            name.to_string()
        }
    }

    /// Find all system()/popen() calls and check if their containing scope
    /// has environment sanitization.
    fn check_all_calls(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = get_node_text(&function, source);
                let resolved = self.resolve_name(&func_name);

                if resolved == "system" || resolved == "popen" {
                    // Find containing function, or root for bare code
                    let scope = self.find_containing_function_or_root(node);
                    let scope_node = scope.unwrap_or(*node);

                    // Check if that scope has sanitization
                    let mut has_sanitization = false;
                    self.check_for_sanitization(&scope_node, source, &mut has_sanitization);

                    if !has_sanitization {
                        let start_point = node.start_position();
                        let call_text = get_node_text(node, source);

                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::High,
                            message: format!(
                                "External program invocation '{}' without environment sanitization. \
                                The environment should be sanitized before invoking external programs \
                                to prevent environment variable manipulation attacks.",
                                call_text
                            ),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some(
                                "Call clearenv() to clear the environment, then use setenv() to set \
                                PATH and IFS to known safe values before invoking external programs."
                                    .to_string(),
                            ),
                            ..Default::default()
                        });
                    }
                }
            }
        }

        // Recurse
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_all_calls(&child, source, violations);
            }
        }
    }

    /// Find the containing function_definition, or the translation_unit root.
    fn find_containing_function_or_root<'a>(&self, node: &Node<'a>) -> Option<Node<'a>> {
        let mut current = *node;
        let mut root = *node;
        while let Some(parent) = current.parent() {
            if parent.kind() == "function_definition" {
                return Some(parent);
            }
            root = parent;
            current = parent;
        }
        // No function found — return the root (translation_unit)
        Some(root)
    }

    fn check_for_sanitization(&self, node: &Node, source: &str, has_sanitization: &mut bool) {
        if *has_sanitization {
            return; // Already found
        }

        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = get_node_text(&function, source);

                // clearenv() clears the entire environment
                if func_name == "clearenv" {
                    *has_sanitization = true;
                    return;
                }

                // setenv("PATH", ...) or setenv("IFS", ...) sanitizes those variables
                if func_name == "setenv" {
                    if let Some(args) = node.child_by_field_name("arguments") {
                        // Get first argument
                        for i in 0..args.child_count() {
                            if let Some(arg) = args.child(i) {
                                if arg.kind() == "string_literal" {
                                    let arg_text = get_node_text(&arg, source);
                                    // Check for PATH or IFS
                                    if arg_text.contains("PATH") || arg_text.contains("IFS") {
                                        *has_sanitization = true;
                                        return;
                                    }
                                }
                                break; // Only check first argument
                            }
                        }
                    }
                }

                // putenv() with PATH= or IFS=
                if func_name == "putenv" {
                    if let Some(args) = node.child_by_field_name("arguments") {
                        for i in 0..args.child_count() {
                            if let Some(arg) = args.child(i) {
                                if arg.kind() == "string_literal" {
                                    let arg_text = get_node_text(&arg, source);
                                    if arg_text.contains("PATH=") || arg_text.contains("IFS=") {
                                        *has_sanitization = true;
                                        return;
                                    }
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }

        // Recurse
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_for_sanitization(&child, source, has_sanitization);
            }
        }
    }
}

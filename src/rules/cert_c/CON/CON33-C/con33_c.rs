use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Con33C;

/// Non-thread-safe library functions and their thread-safe alternatives
/// Format: (unsafe_function, thread_safe_alternative, description)
const NON_THREAD_SAFE_FUNCTIONS: &[(&str, &str, &str)] = &[
    ("strerror", "strerror_r", "POSIX"),
    ("strtok", "strtok_r", "POSIX"),
    ("asctime", "asctime_r or strftime", "POSIX"),
    ("ctime", "ctime_r or strftime", "POSIX"),
    ("localtime", "localtime_r", "POSIX"),
    ("gmtime", "gmtime_r", "POSIX"),
    ("tmpnam", "tmpnam_r or mkstemp", "POSIX"),
    (
        "rand",
        "rand_r",
        "POSIX (or use a thread-safe random generator)",
    ),
    ("getenv", "secure alternative or mutex protection", ""),
    ("setlocale", "mutex protection", ""),
];

impl CertRule for Con33C {
    fn rule_id(&self) -> &'static str {
        "CON33-C"
    }

    fn description(&self) -> &'static str {
        "Avoid race conditions when using library functions"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "CON33-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Recursively check for non-thread-safe function calls
        violations.extend(self.check_node(*node, source));

        violations
    }
}

impl Con33C {
    /// Recursively check nodes for non-thread-safe function calls
    fn check_node(&self, node: Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Check if this is a function call expression
        if node.kind() == "call_expression" {
            if let Some(violation) = self.check_function_call(node, source) {
                violations.push(violation);
            }
        }

        // Recursively check all children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            violations.extend(self.check_node(child, source));
        }

        violations
    }

    /// Check if a call_expression is calling a non-thread-safe function
    fn check_function_call(&self, call_node: Node, source: &str) -> Option<RuleViolation> {
        // Get the function being called
        let function_node = call_node.child_by_field_name("function")?;
        let function_name = get_node_text(&function_node, source);

        // Check if this function is in the non-thread-safe list
        for (unsafe_func, safe_alt, context) in NON_THREAD_SAFE_FUNCTIONS {
            if function_name == *unsafe_func {
                let message = if context.is_empty() {
                    format!(
                        "Use of non-thread-safe function '{}'. Consider using '{}' instead to avoid race conditions in multithreaded code",
                        unsafe_func, safe_alt
                    )
                } else {
                    format!(
                        "Use of non-thread-safe function '{}'. Consider using '{}' ({}) instead to avoid race conditions in multithreaded code",
                        unsafe_func, safe_alt, context
                    )
                };

                let suggestion = format!("Use {} instead", safe_alt);

                return Some(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: self.severity(),
                    message,
                    file_path: String::new(),
                    line: call_node.start_position().row + 1,
                    column: call_node.start_position().column + 1,
                    suggestion: Some(suggestion),
                    ..Default::default()
                });
            }
        }

        None
    }
}

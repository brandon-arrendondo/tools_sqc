use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils;
use tree_sitter::Node;

pub struct Err07C;

impl CertRule for Err07C {
    fn rule_id(&self) -> &'static str {
        "ERR07-C"
    }

    fn description(&self) -> &'static str {
        "Prefer functions that support error checking over equivalent functions that don't"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "ERR07-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Recursively check all function calls in the AST
        check_function_calls(node, source, &mut violations, self.rule_id());

        violations
    }
}

/// Recursively check all function calls for unsafe functions
fn check_function_calls(
    node: &Node,
    source: &str,
    violations: &mut Vec<RuleViolation>,
    rule_id: &str,
) {
    if node.kind() == "call_expression" {
        check_call_expression(node, source, violations, rule_id);
    }

    // Recursively check children
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            check_function_calls(&child, source, violations, rule_id);
        }
    }
}

/// Check a specific call expression for unsafe function usage
fn check_call_expression(
    call_node: &Node,
    source: &str,
    violations: &mut Vec<RuleViolation>,
    rule_id: &str,
) {
    // Get the function name
    if let Some(function) = call_node.child_by_field_name("function") {
        let func_name = ast_utils::get_node_text(&function, source);

        // Check if this is one of the unsafe functions
        if let Some((preferred, reason)) = get_preferred_alternative(&func_name) {
            let pos = call_node.start_position();
            violations.push(RuleViolation {
                rule_id: rule_id.to_string(),
                severity: Severity::Medium,
                message: format!(
                    "Use of '{}' which lacks error checking - prefer '{}'",
                    func_name, preferred
                ),
                file_path: String::new(),
                line: pos.row + 1,
                column: pos.column + 1,
                suggestion: Some(format!(
                    "Replace '{}' with '{}': {}",
                    func_name, preferred, reason
                )),
                ..Default::default()
            });
        }
    }
}

/// Get the preferred alternative for an unsafe function
///
/// Returns: Some((preferred_function, reason)) or None if function is safe
fn get_preferred_alternative(func_name: &str) -> Option<(&'static str, &'static str)> {
    match func_name {
        "atoi" => Some((
            "strtol",
            "strtol provides error indication and prevents undefined behavior on overflow",
        )),
        "atol" => Some((
            "strtol",
            "strtol provides error indication and prevents undefined behavior on overflow",
        )),
        "atoll" => Some((
            "strtoll",
            "strtoll provides error indication and prevents undefined behavior on overflow",
        )),
        "atof" => Some((
            "strtod",
            "strtod provides error indication and prevents undefined behavior on error",
        )),
        "rewind" => Some((
            "fseek",
            "fseek returns a success/failure indication, rewind fails silently",
        )),
        "setbuf" => Some((
            "setvbuf",
            "setvbuf returns an error value if operation fails, setbuf fails silently",
        )),
        "ctime" => Some((
            "asctime/localtime",
            "ctime has undefined behavior if localtime fails",
        )),
        _ => None,
    }
}

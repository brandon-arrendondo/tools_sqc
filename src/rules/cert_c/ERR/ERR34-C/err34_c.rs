use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils;
use tree_sitter::Node;

pub struct Err34C;

impl CertRule for Err34C {
    fn rule_id(&self) -> &'static str {
        "ERR34-C"
    }

    fn description(&self) -> &'static str {
        "Detect errors when converting a string to a number"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "ERR34-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Look for call expressions
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = ast_utils::get_node_text(&function, source);

                // Check for unsafe conversion functions
                if is_unsafe_conversion_function(func_name) {
                    let start_point = node.start_position();
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: self.severity(),
                        message: format!(
                            "Use of '{}()' for string-to-number conversion lacks error detection. Use strtol/strtoul family with error checking",
                            func_name
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some(format!(
                            "Replace {}() with strtol/strtoul family and check errno, end pointer, and range",
                            func_name
                        )),
                        ..Default::default()
                    });
                }
            }
        }

        // Recursively check children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            violations.extend(self.check(&child, source));
        }

        violations
    }
}

/// Checks if a function is an unsafe string-to-number conversion function
fn is_unsafe_conversion_function(func_name: &str) -> bool {
    matches!(
        func_name,
        // atoi family - no error reporting
        "atoi" | "atol" | "atoll" | "atof" |
        // scanf family - misses overflow errors
        "sscanf" | "scanf" | "fscanf" | "vsscanf" | "vscanf" | "vfscanf"
    )
}

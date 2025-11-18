use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils;
use tree_sitter::Node;

pub struct Dcl00C;

impl CertRule for Dcl00C {
    fn rule_id(&self) -> &'static str {
        "DCL00-C"
    }

    fn description(&self) -> &'static str {
        "Const-qualify immutable objects"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "DCL00-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Look for variable declarations that could be const
        if node.kind() == "declaration" {
            if let Some(declarator_node) = find_declarator(node) {
                if let Some(init_declarator) = declarator_node.parent() {
                    if init_declarator.kind() == "init_declarator" {
                        // Check if variable has an initializer but no const qualifier
                        if has_initializer(&init_declarator) && !has_const_qualifier(node, source) {
                            let var_name =
                                ast_utils::get_identifier_from_declarator(&declarator_node, source);
                            let start_point = node.start_position();

                            // Check if this is a candidate for const qualification
                            if is_const_candidate(node, &var_name, source) {
                                violations.push(RuleViolation {
                                    rule_id: self.rule_id().to_string(),
                                    severity: Severity::Medium,
                                    message: format!(
                                        "Variable '{}' is initialized but never modified, consider const-qualifying it",
                                        var_name
                                    ),
                                    file_path: String::new(),
                                    line: start_point.row + 1,
                                    column: start_point.column + 1,
                                    suggestion: Some(format!("Add 'const' qualifier: const {} = ...", var_name)),
                                ..Default::default()
                                });
                            }
                        }
                    }
                }
            }
        }

        // Look for string literals assigned to non-const pointers
        if node.kind() == "init_declarator" {
            if let Some(declarator) = node.child_by_field_name("declarator") {
                if let Some(value) = node.child_by_field_name("value") {
                    if is_string_literal(&value, source) && is_pointer_declarator(&declarator) {
                        if !has_const_in_pointer_type(node, source) {
                            let var_name =
                                ast_utils::get_identifier_from_declarator(&declarator, source);
                            let start_point = node.start_position();

                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                severity: Severity::High,
                                message: format!(
                                    "String literal assigned to non-const pointer '{}'. String literals are immutable",
                                    var_name
                                ),
                                file_path: String::new(),
                                line: start_point.row + 1,
                                column: start_point.column + 1,
                                suggestion: Some("Use 'const char*' for string literals".to_string()),
                            ..Default::default()
                            });
                        }
                    }
                }
            }
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                violations.extend(self.check(&child, source));
            }
        }

        violations
    }
}

fn find_declarator<'a>(node: &'a Node<'a>) -> Option<Node<'a>> {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if child.kind() == "init_declarator" {
                return child.child_by_field_name("declarator");
            } else if child.kind().contains("declarator") {
                return Some(child);
            }
        }
    }
    None
}

fn has_initializer(node: &Node) -> bool {
    node.child_by_field_name("value").is_some()
}

fn has_const_qualifier(node: &Node, source: &str) -> bool {
    let text = &source[node.start_byte()..node.end_byte()];
    text.contains("const")
}

fn is_const_candidate(node: &Node, var_name: &str, source: &str) -> bool {
    // Simple heuristic: check if it looks like a constant value
    let decl_text = &source[node.start_byte()..node.end_byte()];

    // Look for obvious constants
    if decl_text.contains("3.14") || // pi
       decl_text.contains("2.71") || // e
       decl_text.contains("\"") ||   // string literals
       var_name.to_uppercase() == *var_name || // ALL_CAPS naming
       var_name.starts_with("k") || // kConstant naming
       var_name.contains("_MAX") ||
       var_name.contains("_MIN") ||
       var_name.contains("_SIZE")
    {
        return true;
    }

    // Check for numeric literals that look like constants
    if decl_text.matches(char::is_numeric).count() > 0 {
        // Has numbers, might be a constant
        return true;
    }

    false
}

fn is_string_literal(node: &Node, source: &str) -> bool {
    if node.kind() == "string_literal" {
        return true;
    }

    let text = &source[node.start_byte()..node.end_byte()];
    text.starts_with('"') && text.ends_with('"')
}

fn is_pointer_declarator(node: &Node) -> bool {
    node.kind() == "pointer_declarator" || node.to_sexp().contains("pointer_declarator")
}

fn has_const_in_pointer_type(node: &Node, source: &str) -> bool {
    // Look for const in the declaration
    let text = &source[node.start_byte()..node.end_byte()];
    text.contains("const char") || text.contains("char const")
}

// DEPRECATED: Inline tests moved to src/rules/cert_c/tests/inline/

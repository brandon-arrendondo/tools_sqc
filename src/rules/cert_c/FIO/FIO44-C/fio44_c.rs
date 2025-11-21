use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Fio44C;

impl CertRule for Fio44C {
    fn rule_id(&self) -> &'static str {
        "FIO44-C"
    }

    fn description(&self) -> &'static str {
        "Only use values for fsetpos() that are returned from fgetpos()"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        self.rule_id()
    }

    fn check(&self, root: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Track fpos_t variables and whether they were initialized by fgetpos
        let mut fpos_vars = std::collections::HashMap::new();
        self.track_fpos_vars(root, source, &mut fpos_vars);

        // Check fsetpos calls
        self.check_fsetpos_calls(root, source, &fpos_vars, &mut violations);

        violations
    }
}

impl Fio44C {
    fn track_fpos_vars(
        &self,
        node: &Node,
        source: &str,
        fpos_vars: &mut std::collections::HashMap<String, bool>,
    ) {
        // Look for fpos_t declarations
        if node.kind() == "declaration" {
            let decl_text = get_node_text(node, source);
            if decl_text.contains("fpos_t") {
                // Extract variable name (simple heuristic)
                if let Some(declarator) = node.child_by_field_name("declarator") {
                    if let Some(ident) = self.get_identifier(&declarator, source) {
                        fpos_vars.insert(ident.to_string(), false);
                    }
                }
            }
        }

        // Look for fgetpos calls
        if node.kind() == "call_expression" {
            if let Some(func_node) = node.child_by_field_name("function") {
                if get_node_text(&func_node, source) == "fgetpos" {
                    // Mark the fpos_t variable as initialized by fgetpos
                    if let Some(args) = node.child_by_field_name("arguments") {
                        if let Some(second_arg) = args.named_child(1) {
                            let arg_text = get_node_text(&second_arg, source);
                            let var_name = arg_text.trim_start_matches('&').trim();
                            if let Some(initialized) = fpos_vars.get_mut(var_name) {
                                *initialized = true;
                            }
                        }
                    }
                }
            }
        }

        // Recurse
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.track_fpos_vars(&child, source, fpos_vars);
        }
    }

    fn check_fsetpos_calls(
        &self,
        node: &Node,
        source: &str,
        fpos_vars: &std::collections::HashMap<String, bool>,
        violations: &mut Vec<RuleViolation>,
    ) {
        if node.kind() == "call_expression" {
            if let Some(func_node) = node.child_by_field_name("function") {
                if get_node_text(&func_node, source) == "fsetpos" {
                    if let Some(args) = node.child_by_field_name("arguments") {
                        if let Some(second_arg) = args.named_child(1) {
                            let arg_text = get_node_text(&second_arg, source);
                            let var_name = arg_text.trim_start_matches('&').trim();

                            // Check if this fpos_t was initialized by fgetpos
                            if let Some(&initialized) = fpos_vars.get(var_name) {
                                if !initialized {
                                    violations.push(RuleViolation {
                                        rule_id: self.rule_id().to_string(),
                                        severity: self.severity(),
                                        message: "fsetpos() called with value not obtained from fgetpos()".to_string(),
                                        file_path: String::new(),
                                        line: node.start_position().row + 1,
                                        column: node.start_position().column + 1,
                                        suggestion: None,
                                        requires_manual_review: None,
                                    });
                                }
                            } else {
                                // Variable not tracked, might be manually initialized
                                if arg_text.contains("__pos") || arg_text.contains('.') {
                                    violations.push(RuleViolation {
                                        rule_id: self.rule_id().to_string(),
                                        severity: self.severity(),
                                        message: "fsetpos() called with manually initialized fpos_t value".to_string(),
                                        file_path: String::new(),
                                        line: node.start_position().row + 1,
                                        column: node.start_position().column + 1,
                                        suggestion: None,
                                        requires_manual_review: None,
                                    });
                                }
                            }
                        }
                    }
                }
            }
        }

        // Recurse
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_fsetpos_calls(&child, source, fpos_vars, violations);
        }
    }

    fn get_identifier<'a>(&self, node: &Node<'a>, source: &'a str) -> Option<&'a str> {
        if node.kind() == "identifier" {
            return Some(get_node_text(node, source));
        }

        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if let Some(ident) = self.get_identifier(&child, source) {
                return Some(ident);
            }
        }
        None
    }
}

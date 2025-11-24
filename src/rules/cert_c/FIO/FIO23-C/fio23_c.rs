use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Fio23C;

impl CertRule for Fio23C {
    fn rule_id(&self) -> &'static str {
        "FIO23-C"
    }

    fn description(&self) -> &'static str {
        "Do not exit with unflushed data in stdout or stderr. Call fclose() or fflush() before exit/return."
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        self.rule_id()
    }

    fn check(&self, root: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Check if stdout/stderr are closed before return in main
        if self.has_main_return_without_fclose(root, source) {
            // Find the return statement in main
            if let Some((line, column)) = self.find_main_return(root) {
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: self.severity(),
                    message:
                        "main() returns without calling fclose(stdout) - data may be unflushed"
                            .to_string(),
                    file_path: String::new(),
                    line,
                    column,
                    suggestion: Some(
                        "Call fclose(stdout) before return to ensure data is flushed".to_string(),
                    ),
                    requires_manual_review: Some(false),
                    ..Default::default()
                });
            }
        }

        // Check for printf in atexit handlers without prior fclose
        self.check_atexit_handlers(root, source, &mut violations);

        violations
    }
}

impl Fio23C {
    fn has_main_return_without_fclose(&self, root: &Node, source: &str) -> bool {
        // Find main function
        if let Some(main_func) = self.find_main_function(root, source) {
            // Check if there's fclose(stdout) before return
            if let Some(body) = main_func.child_by_field_name("body") {
                return !self.has_fclose_stdout(&body, source);
            }
        }
        false
    }

    fn find_main_function<'a>(&self, node: &Node<'a>, source: &str) -> Option<Node<'a>> {
        if node.kind() == "function_definition" {
            if let Some(declarator) = node.child_by_field_name("declarator") {
                if let Some(name_node) = self.find_identifier_in_declarator(&declarator) {
                    if get_node_text(&name_node, source) == "main" {
                        return Some(*node);
                    }
                }
            }
        }

        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if let Some(result) = self.find_main_function(&child, source) {
                return Some(result);
            }
        }
        None
    }

    fn find_identifier_in_declarator<'a>(&self, declarator: &Node<'a>) -> Option<Node<'a>> {
        if declarator.kind() == "identifier" {
            return Some(*declarator);
        }

        let mut cursor = declarator.walk();
        for child in declarator.children(&mut cursor) {
            if child.kind() == "identifier" {
                return Some(child);
            }
            if let Some(result) = self.find_identifier_in_declarator(&child) {
                return Some(result);
            }
        }
        None
    }

    fn has_fclose_stdout(&self, body: &Node, source: &str) -> bool {
        self.check_fclose_stdout_recursive(body, source)
    }

    fn check_fclose_stdout_recursive(&self, node: &Node, source: &str) -> bool {
        // Look for fclose(stdout)
        if node.kind() == "call_expression" {
            if let Some(func) = node.child_by_field_name("function") {
                if get_node_text(&func, source) == "fclose" {
                    if let Some(args) = node.child_by_field_name("arguments") {
                        let args_text = get_node_text(&args, source);
                        if args_text.contains("stdout") {
                            return true;
                        }
                    }
                }
            }
        }

        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if self.check_fclose_stdout_recursive(&child, source) {
                return true;
            }
        }
        false
    }

    fn find_main_return(&self, root: &Node) -> Option<(usize, usize)> {
        if let Some(main_func) = self.find_main_function_node(root) {
            if let Some(body) = main_func.child_by_field_name("body") {
                if let Some(return_stmt) = self.find_return_statement(&body) {
                    let line = return_stmt.start_position().row + 1;
                    let column = return_stmt.start_position().column + 1;
                    return Some((line, column));
                }
            }
        }
        None
    }

    fn find_main_function_node<'a>(&self, node: &Node<'a>) -> Option<Node<'a>> {
        if node.kind() == "function_definition" {
            if let Some(declarator) = node.child_by_field_name("declarator") {
                if self.is_main_declarator(&declarator) {
                    return Some(*node);
                }
            }
        }

        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if let Some(result) = self.find_main_function_node(&child) {
                return Some(result);
            }
        }
        None
    }

    fn is_main_declarator(&self, declarator: &Node) -> bool {
        if declarator.kind() == "identifier" {
            return true; // Simplified check
        }

        let mut cursor = declarator.walk();
        for child in declarator.children(&mut cursor) {
            if self.is_main_declarator(&child) {
                return true;
            }
        }
        false
    }

    fn find_return_statement<'a>(&self, node: &Node<'a>) -> Option<Node<'a>> {
        if node.kind() == "return_statement" {
            return Some(*node);
        }

        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if let Some(result) = self.find_return_statement(&child) {
                return Some(result);
            }
        }
        None
    }

    fn check_atexit_handlers(
        &self,
        root: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // First check if main has fclose(stdout)
        let main_has_fclose = if let Some(main_func) = self.find_main_function(root, source) {
            if let Some(body) = main_func.child_by_field_name("body") {
                self.has_fclose_stdout(&body, source)
            } else {
                false
            }
        } else {
            false
        };

        // Only flag if main closes stdout AND there are atexit handlers with printf
        if !main_has_fclose {
            return;
        }

        // Find atexit() calls and track registered functions
        let atexit_funcs = self.find_atexit_functions(root, source);

        for func_name in atexit_funcs {
            // Find the function definition
            if let Some(func_node) = self.find_function_by_name(root, &func_name, source) {
                // Check if it has printf/fprintf without fclose before
                if self.has_printf_in_function(&func_node, source) {
                    let line = func_node.start_position().row + 1;
                    let column = func_node.start_position().column + 1;

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: self.severity(),
                        message: format!("atexit handler '{}' contains printf after fclose(stdout) in main - output may be lost", func_name),
                        file_path: String::new(),
                        line,
                        column,
                        suggestion: Some("Do not call fclose(stdout) before atexit handlers that print".to_string()),
                        requires_manual_review: Some(true),
                        ..Default::default()
                    });
                }
            }
        }
    }

    fn find_atexit_functions(&self, node: &Node, source: &str) -> Vec<String> {
        let mut funcs = Vec::new();
        self.collect_atexit_functions(node, source, &mut funcs);
        funcs
    }

    fn collect_atexit_functions(&self, node: &Node, source: &str, funcs: &mut Vec<String>) {
        if node.kind() == "call_expression" {
            if let Some(func) = node.child_by_field_name("function") {
                if get_node_text(&func, source) == "atexit" {
                    if let Some(args) = node.child_by_field_name("arguments") {
                        if let Some(arg) = self.get_first_arg(&args) {
                            let func_name = get_node_text(&arg, source).to_string();
                            funcs.push(func_name);
                        }
                    }
                }
            }
        }

        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.collect_atexit_functions(&child, source, funcs);
        }
    }

    fn get_first_arg<'a>(&self, arguments: &Node<'a>) -> Option<Node<'a>> {
        let mut cursor = arguments.walk();
        for child in arguments.children(&mut cursor) {
            if child.kind() != "," && child.kind() != "(" && child.kind() != ")" {
                return Some(child);
            }
        }
        None
    }

    fn find_function_by_name<'a>(
        &self,
        node: &Node<'a>,
        name: &str,
        source: &str,
    ) -> Option<Node<'a>> {
        if node.kind() == "function_definition" {
            if let Some(declarator) = node.child_by_field_name("declarator") {
                if let Some(id) = self.find_identifier_in_declarator(&declarator) {
                    if get_node_text(&id, source) == name {
                        return Some(*node);
                    }
                }
            }
        }

        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if let Some(result) = self.find_function_by_name(&child, name, source) {
                return Some(result);
            }
        }
        None
    }

    fn has_print_without_prior_fclose(&self, func_node: &Node, source: &str) -> bool {
        // This method is no longer used but kept for completeness
        self.has_printf_in_function(func_node, source)
    }

    fn has_printf_in_function(&self, node: &Node, source: &str) -> bool {
        if node.kind() == "call_expression" {
            if let Some(func) = node.child_by_field_name("function") {
                let func_name = get_node_text(&func, source);
                if func_name == "printf" || func_name == "fprintf" || func_name == "puts" {
                    return true;
                }
            }
        }

        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if self.has_printf_in_function(&child, source) {
                return true;
            }
        }
        false
    }
}

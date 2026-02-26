use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

/// MSC41-C: Never hard code sensitive information
///
/// Detects hard-coded sensitive data like passwords, API keys, and encryption keys
/// that should not be embedded directly in source code.
pub struct Msc41C;

impl Msc41C {
    pub fn new() -> Self {
        Self
    }

    /// Check if a function name suggests it deals with sensitive information
    fn is_sensitive_function(name: &str) -> bool {
        let sensitive_keywords = [
            "auth",
            "password",
            "passwd",
            "pwd",
            "credential",
            "login",
            "key",
            "secret",
            "token",
            "encrypt",
            "decrypt",
            "crypto",
            "cipher",
            "connect",
            "database",
            "db",
        ];

        let name_lower = name.to_lowercase();
        sensitive_keywords
            .iter()
            .any(|keyword| name_lower.contains(keyword))
    }

    /// Check if a variable name suggests it stores sensitive information
    fn is_sensitive_variable_name(name: &str) -> bool {
        let sensitive_patterns = [
            "password",
            "passwd",
            "pwd",
            "pass",
            "key",
            "secret",
            "token",
            "credential",
            "apikey",
            "api_key",
            "auth",
            "pin",
            "code",
            "salt",
        ];

        let name_lower = name.to_lowercase();
        sensitive_patterns
            .iter()
            .any(|pattern| name_lower.contains(pattern))
    }

    /// Check if a string literal looks like sensitive data
    fn looks_like_sensitive_data(text: &str) -> bool {
        // Remove quotes
        let content = text.trim_matches('"').trim_matches('\'');

        // Empty strings are not sensitive
        if content.is_empty() {
            return false;
        }

        // Very short strings (< 3 chars) are likely not passwords
        if content.len() < 3 {
            return false;
        }

        // Common non-sensitive strings to exclude
        let non_sensitive = [
            "test",
            "example",
            "localhost",
            "127.0.0.1",
            "utf-8",
            "utf8",
            "http",
            "https",
            "file",
            "path",
            "name",
            "user",
            "admin",
            "root",
            "guest",
            "public",
            "private",
            "default",
        ];

        let content_lower = content.to_lowercase();
        if non_sensitive
            .iter()
            .any(|s| content_lower.contains(s) && content.len() < 20)
        {
            return false;
        }

        // If it contains mix of letters, numbers, or special chars, might be sensitive
        let has_letters = content.chars().any(|c| c.is_alphabetic());
        let has_digits = content.chars().any(|c| c.is_numeric());
        let has_special = content
            .chars()
            .any(|c| !c.is_alphanumeric() && !c.is_whitespace());

        // Passwords/keys typically have mixed character types
        (has_special || has_digits) && has_letters || (has_digits && has_special)
    }

    /// Check call expressions for hard-coded sensitive data
    fn check_call_expression(node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if node.kind() != "call_expression" {
            return;
        }

        // Get function name
        let mut func_name = String::new();
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if child.kind() == "identifier" {
                func_name = get_node_text(&child, source).to_string();
                break;
            }
        }

        if func_name.is_empty() || !Self::is_sensitive_function(&func_name) {
            return;
        }

        // Check arguments for string literals
        cursor = node.walk();
        for child in node.children(&mut cursor) {
            if child.kind() == "argument_list" {
                let mut arg_cursor = child.walk();
                for arg in child.children(&mut arg_cursor) {
                    Self::check_node_for_string_literals(
                        &arg,
                        source,
                        violations,
                        Some(&func_name),
                    );
                }
            }
        }
    }

    /// Check for string literals in a node and its children
    fn check_node_for_string_literals(
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        context_func: Option<&str>,
    ) {
        if node.kind() == "string_literal" {
            let text = get_node_text(node, source);

            // If we have a context function (sensitive function), flag any non-empty string literal
            // Otherwise, use heuristics to determine if it looks like sensitive data
            let should_flag = if context_func.is_some() {
                // In sensitive function context, flag any non-empty string
                let content = text.trim_matches('"').trim_matches('\'');
                !content.is_empty()
            } else {
                // Outside sensitive function context, use heuristics
                Self::looks_like_sensitive_data(text)
            };

            if should_flag {
                let context = if let Some(func) = context_func {
                    format!(" passed to `{}`", func)
                } else {
                    String::new()
                };

                violations.push(RuleViolation {
                    rule_id: "MSC41-C".to_string(),
                    severity: Severity::High,
                    message: format!(
                        "Hard-coded sensitive information detected{}. Sensitive data like passwords, keys, or tokens should not be embedded in source code. Code: {}",
                        context,
                        text.trim()
                    ),
                    file_path: String::new(),
                    line: node.start_position().row + 1,
                    column: node.start_position().column + 1,
                    suggestion: Some(
                        "Consider reading sensitive data from environment variables, configuration files, or user input at runtime.".to_string()
                    ),
                    requires_manual_review: None,
                });
            }
        }

        // Recursively check children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            Self::check_node_for_string_literals(&child, source, violations, context_func);
        }
    }

    /// Check variable declarations/initializations for hard-coded sensitive data
    fn check_declaration(node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if node.kind() != "declaration" && node.kind() != "init_declarator" {
            return;
        }

        // Get variable name and check if it suggests sensitive data
        let mut var_name = String::new();
        let mut has_string_init = false;
        let mut init_node: Option<Node> = None;

        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if child.kind() == "init_declarator" {
                let mut init_cursor = child.walk();
                for init_child in child.children(&mut init_cursor) {
                    if init_child.kind() == "pointer_declarator"
                        || init_child.kind() == "identifier"
                    {
                        // Extract variable name
                        let decl_text = get_node_text(&init_child, source);
                        var_name = decl_text
                            .trim_start_matches('*')
                            .trim()
                            .split(|c: char| !c.is_alphanumeric() && c != '_')
                            .next()
                            .unwrap_or("")
                            .to_string();
                    } else if init_child.kind() == "string_literal" {
                        has_string_init = true;
                        init_node = Some(init_child);
                    }
                }
            }
        }

        // If variable name suggests sensitive data and it's initialized with a string
        if !var_name.is_empty() && Self::is_sensitive_variable_name(&var_name) && has_string_init {
            if let Some(string_node) = init_node {
                let text = get_node_text(&string_node, source);
                violations.push(RuleViolation {
                    rule_id: "MSC41-C".to_string(),
                    severity: Severity::High,
                    message: format!(
                        "Variable `{}` appears to store sensitive information and is initialized with a hard-coded string literal. Sensitive data should not be embedded in source code. Code: {}",
                        var_name,
                        text.trim()
                    ),
                    file_path: String::new(),
                    line: string_node.start_position().row + 1,
                    column: string_node.start_position().column + 1,
                    suggestion: Some(
                        "Consider reading sensitive data from environment variables, configuration files, or user input at runtime.".to_string()
                    ),
                    requires_manual_review: None,
                });
            }
        }
    }

    /// Recursively check all nodes
    fn check_node(node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check this node
        Self::check_call_expression(node, source, violations);
        Self::check_declaration(node, source, violations);

        // Recursively check children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            Self::check_node(&child, source, violations);
        }
    }
}

impl CertRule for Msc41C {
    fn rule_id(&self) -> &'static str {
        "MSC41-C"
    }

    fn description(&self) -> &'static str {
        "Never hard code sensitive information"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "MSC41-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        Self::check_node(node, source, &mut violations);
        violations
    }
}

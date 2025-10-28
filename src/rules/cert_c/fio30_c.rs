//! FIO30-C: Exclude user input from format strings
//!
//! This rule detects when user-controlled input is used as a format string
//! in functions like printf, sprintf, fprintf, etc. Using user input as
//! format strings can lead to format string vulnerabilities.
//!
//! VIOLATIONS:
//! - printf(user_input)           // User input as format string
//! - sprintf(buf, argv[1], data)  // Command line argument as format string
//! - fprintf(file, getenv("FMT")) // Environment variable as format string
//!
//! COMPLIANT:
//! - printf("%s", user_input)     // User input as data argument
//! - sprintf(buf, "Data: %s", user_input)  // Literal format with user data
//! - printf("Hello, World!")      // Literal format string
//!
//! The rule tracks data flow to identify user input sources including:
//! - Command line arguments (argv)
//! - Input functions (fgets, scanf, getenv, etc.)
//! - Variables assigned from user input sources

use super::ast_utils;
use super::super::{CertRule, RuleViolation};
use crate::manifest::Severity;
use tree_sitter::Node;
use std::collections::HashSet;

pub struct Fio30C;

impl CertRule for Fio30C {
    fn rule_id(&self) -> &'static str {
        "FIO30-C"
    }

    fn description(&self) -> &'static str {
        "Exclude user input from format strings"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Analyze each function for format string vulnerabilities
        if node.kind() == "function_definition" {
            let mut analyzer = FormatStringAnalyzer::new();
            analyzer.analyze_function(node, source, &mut violations);
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

struct FormatStringAnalyzer {
    // Track variables that may contain user input
    user_input_vars: HashSet<String>,
    // Track variables that are safe (known constants)
    safe_vars: HashSet<String>,
}

impl FormatStringAnalyzer {
    fn new() -> Self {
        Self {
            user_input_vars: HashSet::new(),
            safe_vars: HashSet::new(),
        }
    }

    fn analyze_function(&mut self, func_node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // First, check if this is main() function and mark argv as user input
        if let Some(declarator) = func_node.child_by_field_name("declarator") {
            let func_name = self.get_function_name(&declarator, source);
            if func_name == "main" {
                self.mark_main_parameters(&declarator, source);
            }
        }

        if let Some(body) = func_node.child_by_field_name("body") {
            self.analyze_node(&body, source, violations);
        }
    }

    fn mark_main_parameters(&mut self, declarator: &Node, source: &str) {
        // Look for main function parameters (argc, argv)
        if let Some(params) = declarator.child_by_field_name("parameters") {
            let mut param_count = 0;
            for i in 0..params.child_count() {
                if let Some(param) = params.child(i) {
                    if param.kind() == "parameter_declaration" {
                        if param_count == 1 {
                            // Second parameter is argv
                            if let Some(declarator) = param.child_by_field_name("declarator") {
                                let param_name = self.get_variable_name(&declarator, source);
                                self.user_input_vars.insert(param_name);
                            }
                        }
                        param_count += 1;
                    }
                }
            }
        }
    }

    fn analyze_node(&mut self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            "declaration" => {
                self.process_declaration(node, source);
            }
            "assignment_expression" => {
                self.process_assignment(node, source);
            }
            "call_expression" => {
                self.check_format_string_call(node, source, violations);
            }
            _ => {}
        }

        // Recursively process child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.analyze_node(&child, source, violations);
            }
        }
    }

    fn process_declaration(&mut self, node: &Node, source: &str) {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "init_declarator" {
                    if let Some(declarator) = child.child_by_field_name("declarator") {
                        let var_name = self.get_variable_name(&declarator, source);

                        if let Some(value) = child.child_by_field_name("value") {
                            if self.is_user_input_source(&value, source) {
                                self.user_input_vars.insert(var_name);
                            } else if self.is_safe_value(&value, source) {
                                self.safe_vars.insert(var_name);
                            } else if value.kind() == "identifier" {
                                let source_var = ast_utils::get_node_text_owned(&value, source);
                                if self.user_input_vars.contains(&source_var) {
                                    self.user_input_vars.insert(var_name);
                                } else if self.safe_vars.contains(&source_var) {
                                    self.safe_vars.insert(var_name);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    fn process_assignment(&mut self, node: &Node, source: &str) {
        if let (Some(left), Some(right)) = (
            node.child_by_field_name("left"),
            node.child_by_field_name("right")
        ) {
            if left.kind() == "identifier" {
                let var_name = ast_utils::get_node_text_owned(&left, source);

                if self.is_user_input_source(&right, source) {
                    self.user_input_vars.insert(var_name.clone());
                    self.safe_vars.remove(&var_name);
                } else if self.is_safe_value(&right, source) {
                    self.safe_vars.insert(var_name.clone());
                    self.user_input_vars.remove(&var_name);
                } else if right.kind() == "identifier" {
                    let source_var = ast_utils::get_node_text_owned(&right, source);
                    if self.user_input_vars.contains(&source_var) {
                        self.user_input_vars.insert(var_name.clone());
                        self.safe_vars.remove(&var_name);
                    } else if self.safe_vars.contains(&source_var) {
                        self.safe_vars.insert(var_name.clone());
                        self.user_input_vars.remove(&var_name);
                    }
                }
            }
        }
    }

    fn check_format_string_call(&mut self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(function) = node.child_by_field_name("function") {
            let func_name = ast_utils::get_node_text_owned(&function, source);

            if self.is_format_string_function(&func_name) {
                if let Some(arguments) = node.child_by_field_name("arguments") {
                    // Get the format string argument (usually first argument)
                    let format_arg_index = self.get_format_arg_index(&func_name);
                    let mut current_arg = 0;
                    let mut format_string_node = None;

                    for i in 0..arguments.child_count() {
                        if let Some(arg) = arguments.child(i) {
                            // Skip punctuation and whitespace
                            if matches!(arg.kind(), "," | "(" | ")") {
                                continue;
                            }

                            if current_arg == format_arg_index {
                                format_string_node = Some(arg);
                                break;
                            }
                            current_arg += 1;
                        }
                    }

                    if let Some(format_arg) = format_string_node {
                        // Special handling for sizeof expressions which are safe
                        if format_arg.kind() == "sizeof_expression" {
                            return; // sizeof expressions don't represent format strings
                        }

                        if self.is_potentially_unsafe_format_string(&format_arg, source) {
                            let start_point = format_arg.start_position();
                            let arg_text = &source[format_arg.start_byte()..format_arg.end_byte()];

                            violations.push(RuleViolation {
                                rule_id: "FIO30-C".to_string(),
                                severity: Severity::Critical,
                                message: format!(
                                    "User input used as format string in '{}()' call: {}",
                                    func_name, arg_text
                                ),
                                file_path: String::new(),
                                line: start_point.row + 1,
                                column: start_point.column + 1,
                                suggestion: Some(format!(
                                    "Use a literal format string: {}(\"%s\", user_input) instead of {}(user_input)",
                                    func_name, func_name
                                )),
                            });
                        }
                    }
                }
            }
        }
    }

    fn is_format_string_function(&self, func_name: &str) -> bool {
        matches!(func_name,
            "printf" | "fprintf" | "sprintf" | "snprintf" |
            "vprintf" | "vfprintf" | "vsprintf" | "vsnprintf" |
            "scanf" | "fscanf" | "sscanf" |
            "syslog" | "err" | "errx" | "warn" | "warnx"
        )
    }

    fn get_format_arg_index(&self, func_name: &str) -> usize {
        match func_name {
            "snprintf" | "vsnprintf" => 2, // Third argument is format string (buffer, size, format, ...)
            "sprintf" | "vsprintf" | "sscanf" | "fprintf" | "fscanf" | "syslog" => 1, // Second argument is format string
            _ => 0, // First argument is format string (printf, scanf, etc.)
        }
    }

    fn is_user_input_source(&self, node: &Node, source: &str) -> bool {
        match node.kind() {
            "call_expression" => {
                if let Some(function) = node.child_by_field_name("function") {
                    let func_name = ast_utils::get_node_text_owned(&function, source);
                    return matches!(func_name.as_str(),
                        "fgets" | "gets" | "getline" | "getdelim" | "fgetc" | "getc" | "getchar" |
                        "fread" | "read" | "recv" | "recvfrom" | "recvmsg" |
                        "getenv" | "getpwnam" | "getpwuid" | "getgrnam" | "getgrgid"
                    );
                }
                false
            }
            "subscript_expression" => {
                // Check if this is accessing argv or environment variables
                if let Some(array) = node.child(0) {
                    if array.kind() == "identifier" {
                        let array_name = ast_utils::get_node_text_owned(&array, source);
                        return self.user_input_vars.contains(&array_name) || array_name == "argv";
                    }
                }
                false
            }
            "identifier" => {
                let var_name = ast_utils::get_node_text_owned(node, source);
                self.user_input_vars.contains(&var_name)
            }
            _ => false
        }
    }

    fn is_safe_value(&self, node: &Node, source: &str) -> bool {
        match node.kind() {
            "string_literal" | "concatenated_string" => true,
            "number_literal" => true,
            "char_literal" => true,
            "identifier" => {
                let var_name = ast_utils::get_node_text_owned(node, source);
                self.safe_vars.contains(&var_name)
            }
            _ => false
        }
    }

    /// Determines if a node represents a potentially unsafe format string.
    ///
    /// Safe format strings include:
    /// - String literals ("format %s")
    /// - Concatenated string literals
    /// - Variables known to contain only literal strings
    ///
    /// Unsafe format strings include:
    /// - User input variables
    /// - Function calls returning user-controlled data
    /// - Array subscripts (especially argv[])
    /// - Unknown or untracked variables
    fn is_potentially_unsafe_format_string(&self, node: &Node, source: &str) -> bool {
        // Debug: Log the actual node kind to understand what we're dealing with
        #[cfg(debug_assertions)]
        eprintln!("DEBUG FIO30-C: Checking node kind: '{}' with text: '{}'",
                 node.kind(),
                 &source[node.start_byte()..node.end_byte()]);

        match node.kind() {
            "string_literal" | "concatenated_string" | "string_content" => {
                // Literal strings are always safe as format strings
                false
            }
            "identifier" => {
                let var_name = ast_utils::get_node_text_owned(node, source);
                // Unsafe if it's user input and not in safe vars
                self.user_input_vars.contains(&var_name) ||
                (!self.safe_vars.contains(&var_name) && self.could_be_user_input(&var_name))
            }
            "subscript_expression" => {
                // Array access could be user input (e.g., argv[1])
                if let Some(array) = node.child(0) {
                    if array.kind() == "identifier" {
                        let array_name = ast_utils::get_node_text_owned(&array, source);
                        return self.user_input_vars.contains(&array_name) || array_name == "argv";
                    }
                }
                true // Conservative: assume array access could be unsafe
            }
            "call_expression" => {
                // Function calls that return strings could be unsafe
                if let Some(function) = node.child_by_field_name("function") {
                    let func_name = ast_utils::get_node_text_owned(&function, source);
                    // Functions that typically return user-controlled data
                    return matches!(func_name.as_str(),
                        "fgets" | "gets" | "getline" | "getenv" | "getpwnam" | "readline"
                    );
                }
                true // Conservative: assume unknown function calls could be unsafe
            }
            "binary_expression" | "conditional_expression" | "cast_expression" => {
                // These could involve string operations, need deeper inspection
                // For now, check if any child is potentially unsafe
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        if self.is_potentially_unsafe_format_string(&child, source) {
                            return true;
                        }
                    }
                }
                false
            }
            _ => {
                // IMPORTANT: Check if this might be a string literal wrapped in another node
                // Some AST structures wrap string literals in additional nodes
                if node.child_count() == 1 {
                    if let Some(child) = node.child(0) {
                        if child.kind() == "string_literal" || child.kind() == "string_content" {
                            return false; // It's a wrapped string literal, which is safe
                        }
                        // Recursively check the single child
                        return self.is_potentially_unsafe_format_string(&child, source);
                    }
                }

                // For safety, check the actual text content
                let text = &source[node.start_byte()..node.end_byte()];
                if text.starts_with('"') && text.ends_with('"') {
                    // It looks like a string literal
                    return false;
                }

                // Conservative: assume unknown expressions could be unsafe
                true
            }
        }
    }

    fn could_be_user_input(&self, var_name: &str) -> bool {
        // Heuristic: variables with certain names are likely to contain user input
        let name_lower = var_name.to_lowercase();
        name_lower.contains("input") ||
        name_lower.contains("user") ||
        name_lower.contains("argv") ||
        name_lower.contains("arg") ||
        name_lower.contains("buf") ||
        name_lower.contains("buffer") ||
        name_lower.contains("line") ||
        name_lower.contains("cmd") ||
        name_lower.contains("command")
    }

    fn get_function_name(&self, declarator: &Node, source: &str) -> String {
        match declarator.kind() {
            "identifier" => ast_utils::get_node_text_owned(declarator, source),
            "function_declarator" => {
                if let Some(declarator) = declarator.child_by_field_name("declarator") {
                    self.get_function_name(&declarator, source)
                } else {
                    "unknown".to_string()
                }
            }
            _ => {
                for i in 0..declarator.child_count() {
                    if let Some(child) = declarator.child(i) {
                        if child.kind() == "identifier" {
                            return ast_utils::get_node_text_owned(&child, source);
                        }
                        let nested_name = self.get_function_name(&child, source);
                        if nested_name != "unknown" {
                            return nested_name;
                        }
                    }
                }
                "unknown".to_string()
            }
        }
    }

    fn get_variable_name(&self, declarator: &Node, source: &str) -> String {
        match declarator.kind() {
            "identifier" => ast_utils::get_node_text_owned(declarator, source),
            "pointer_declarator" | "array_declarator" => {
                for i in 0..declarator.child_count() {
                    if let Some(child) = declarator.child(i) {
                        if child.kind() == "identifier" {
                            return ast_utils::get_node_text_owned(&child, source);
                        }
                        let nested_name = self.get_variable_name(&child, source);
                        if nested_name != "unknown" {
                            return nested_name;
                        }
                    }
                }
                "unknown".to_string()
            }
            _ => "unknown".to_string()
        }
    }
}

#[cfg(test)]
#[path = "tests/fio30_c.rs"]
mod tests;
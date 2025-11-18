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

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils;
use std::collections::HashSet;
use tree_sitter::Node;

pub struct Fio30C;

impl CertRule for Fio30C {
    fn rule_id(&self) -> &'static str {
        "FIO30-C"
    }

    fn description(&self) -> &'static str {
        "Exclude user input from format strings"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "FIO30-C"
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
    // Track function parameters (which should be treated as potentially tainted for format strings)
    function_parameters: HashSet<String>,
}

impl FormatStringAnalyzer {
    fn new() -> Self {
        Self {
            user_input_vars: HashSet::new(),
            safe_vars: HashSet::new(),
            function_parameters: HashSet::new(),
        }
    }

    fn analyze_function(
        &mut self,
        func_node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // First, check if this is main() function and mark argv as user input
        if let Some(declarator) = func_node.child_by_field_name("declarator") {
            let func_name = self.get_function_name(&declarator, source);
            if func_name == "main" {
                self.mark_main_parameters(&declarator, source);
            }
            // Mark all function parameters (they could be tainted from callers)
            self.mark_function_parameters(&declarator, source);
        }

        if let Some(body) = func_node.child_by_field_name("body") {
            self.analyze_node(&body, source, violations);
        }
    }

    fn mark_function_parameters(&mut self, declarator: &Node, source: &str) {
        // Look for function parameters and track them
        if let Some(params) = declarator.child_by_field_name("parameters") {
            for i in 0..params.child_count() {
                if let Some(param) = params.child(i) {
                    if param.kind() == "parameter_declaration" {
                        if let Some(param_declarator) = param.child_by_field_name("declarator") {
                            let param_name = self.get_variable_name(&param_declarator, source);
                            // Mark as function parameter (potentially tainted)
                            self.function_parameters.insert(param_name);
                        }
                    }
                }
            }
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
                // Check for string manipulation functions that propagate taint
                self.process_string_manipulation_call(node, source);
                // Check for format string vulnerabilities
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
            node.child_by_field_name("right"),
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
                } else if right.kind() == "call_expression" {
                    // Check if call returns tainted data
                    if self.call_returns_tainted_data(&right, source) {
                        self.user_input_vars.insert(var_name.clone());
                        self.safe_vars.remove(&var_name);
                    }
                } else if self.expression_contains_taint(&right, source) {
                    // Check if expression contains any tainted data
                    self.user_input_vars.insert(var_name.clone());
                    self.safe_vars.remove(&var_name);
                }
            }
        }
    }

    /// Process string manipulation calls like strcpy, strcat, sprintf that propagate taint
    fn process_string_manipulation_call(&mut self, node: &Node, source: &str) {
        if let Some(function) = node.child_by_field_name("function") {
            let func_name = ast_utils::get_node_text_owned(&function, source);

            // Handle functions that write user input to their first argument
            if matches!(
                func_name.as_str(),
                "fgets" | "gets" | "getline" | "fread" | "read"
            ) {
                if let Some(arguments) = node.child_by_field_name("arguments") {
                    let args = self.extract_arguments(&arguments, source);
                    if !args.is_empty() {
                        // First argument receives user input
                        if let Some(dest_name) = self.get_base_variable(&args[0], source) {
                            self.user_input_vars.insert(dest_name.clone());
                            self.safe_vars.remove(&dest_name);
                        }
                    }
                }
            }

            // Handle scanf family - all arguments after format string receive user input
            if matches!(func_name.as_str(), "scanf" | "fscanf" | "sscanf") {
                if let Some(arguments) = node.child_by_field_name("arguments") {
                    let args = self.extract_arguments(&arguments, source);
                    // scanf/fscanf: first arg is format, rest are pointers to receive input
                    // sscanf: first two args are string and format, rest are pointers
                    let start_index = if func_name == "sscanf" { 2 } else { 1 };
                    for i in start_index..args.len() {
                        if let Some(dest_name) = self.get_base_variable(&args[i], source) {
                            self.user_input_vars.insert(dest_name.clone());
                            self.safe_vars.remove(&dest_name);
                        }
                    }
                }
            }

            // Handle strcpy, strcat, sprintf, snprintf - first arg gets tainted if source is tainted
            if matches!(
                func_name.as_str(),
                "strcpy" | "strcat" | "sprintf" | "snprintf" | "strncpy" | "strncat"
            ) {
                if let Some(arguments) = node.child_by_field_name("arguments") {
                    let args = self.extract_arguments(&arguments, source);

                    if !args.is_empty() {
                        let dest_arg = &args[0];

                        // Check if any source arguments are tainted
                        let mut any_source_tainted = false;
                        for i in 1..args.len() {
                            if self.is_tainted_argument(&args[i], source) {
                                any_source_tainted = true;
                                break;
                            }
                        }

                        if any_source_tainted {
                            // Mark destination as tainted
                            if let Some(dest_name) = self.get_base_variable(dest_arg, source) {
                                self.user_input_vars.insert(dest_name.clone());
                                self.safe_vars.remove(&dest_name);
                            }
                        }
                    }
                }
            }
        }
    }

    /// Check if an argument expression contains tainted data
    fn is_tainted_argument(&self, arg: &Node, source: &str) -> bool {
        match arg.kind() {
            "identifier" => {
                let var_name = ast_utils::get_node_text_owned(arg, source);
                self.user_input_vars.contains(&var_name)
                    || self.function_parameters.contains(&var_name)
            }
            "subscript_expression" => {
                if let Some(array) = arg.child(0) {
                    if array.kind() == "identifier" {
                        let array_name = ast_utils::get_node_text_owned(&array, source);
                        return self.user_input_vars.contains(&array_name) || array_name == "argv";
                    }
                }
                false
            }
            "call_expression" => self.is_user_input_source(arg, source),
            _ => {
                // Recursively check children
                for i in 0..arg.child_count() {
                    if let Some(child) = arg.child(i) {
                        if self.is_tainted_argument(&child, source) {
                            return true;
                        }
                    }
                }
                false
            }
        }
    }

    /// Extract base variable name from an expression (handles array access, pointer deref)
    fn get_base_variable(&self, node: &Node, source: &str) -> Option<String> {
        match node.kind() {
            "identifier" => Some(ast_utils::get_node_text_owned(node, source)),
            "subscript_expression" | "pointer_expression" | "unary_expression" => {
                // Handle &var, *var, var[index], etc.
                if let Some(base) = node.child(0) {
                    // Skip operators like '&', '*'
                    if base.kind() == "&" || base.kind() == "*" {
                        if let Some(actual_base) = node.child(1) {
                            return self.get_base_variable(&actual_base, source);
                        }
                    }
                    self.get_base_variable(&base, source)
                } else if let Some(arg) = node.child_by_field_name("argument") {
                    // For unary expressions with named 'argument' field
                    self.get_base_variable(&arg, source)
                } else {
                    None
                }
            }
            _ => {
                // For any other node type, try to find an identifier child
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        if child.kind() == "identifier" {
                            return Some(ast_utils::get_node_text_owned(&child, source));
                        }
                    }
                }
                None
            }
        }
    }

    /// Extract arguments from an argument list
    fn extract_arguments<'a>(&self, arguments: &'a Node, source: &str) -> Vec<Node<'a>> {
        let mut args = Vec::new();
        for i in 0..arguments.child_count() {
            if let Some(arg) = arguments.child(i) {
                if !matches!(arg.kind(), "," | "(" | ")") {
                    args.push(arg);
                }
            }
        }
        args
    }

    /// Check if a call expression returns tainted data
    fn call_returns_tainted_data(&self, call_node: &Node, source: &str) -> bool {
        if let Some(function) = call_node.child_by_field_name("function") {
            let func_name = ast_utils::get_node_text_owned(&function, source);

            // User input functions
            if matches!(
                func_name.as_str(),
                "fgets"
                    | "gets"
                    | "getline"
                    | "getdelim"
                    | "fgetc"
                    | "getc"
                    | "getchar"
                    | "fread"
                    | "read"
                    | "recv"
                    | "recvfrom"
                    | "recvmsg"
                    | "getenv"
                    | "getpwnam"
                    | "getpwuid"
                    | "getgrnam"
                    | "getgrgid"
            ) {
                return true;
            }

            // Check if any arguments are tainted (function might return based on tainted input)
            if let Some(arguments) = call_node.child_by_field_name("arguments") {
                for arg in self.extract_arguments(&arguments, source) {
                    if self.is_tainted_argument(&arg, source) {
                        return true;
                    }
                }
            }
        }
        false
    }

    /// Check if an expression contains any tainted data
    fn expression_contains_taint(&self, expr: &Node, source: &str) -> bool {
        match expr.kind() {
            "identifier" => {
                let var_name = ast_utils::get_node_text_owned(expr, source);
                self.user_input_vars.contains(&var_name)
            }
            "subscript_expression" => {
                if let Some(array) = expr.child(0) {
                    if array.kind() == "identifier" {
                        let array_name = ast_utils::get_node_text_owned(&array, source);
                        if self.user_input_vars.contains(&array_name) || array_name == "argv" {
                            return true;
                        }
                    }
                }
                // Also check if index is tainted
                if let Some(index) = expr.child(1) {
                    if self.expression_contains_taint(&index, source) {
                        return true;
                    }
                }
                false
            }
            "call_expression" => self.call_returns_tainted_data(expr, source),
            _ => {
                // Recursively check children
                for i in 0..expr.child_count() {
                    if let Some(child) = expr.child(i) {
                        if self.expression_contains_taint(&child, source) {
                            return true;
                        }
                    }
                }
                false
            }
        }
    }

    fn check_format_string_call(
        &mut self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
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
                                ..Default::default()
                            });
                        }
                    }
                }
            }
        }
    }

    fn is_format_string_function(&self, func_name: &str) -> bool {
        matches!(
            func_name,
            "printf"
                | "fprintf"
                | "sprintf"
                | "snprintf"
                | "dprintf"
                | "vprintf"
                | "vfprintf"
                | "vsprintf"
                | "vsnprintf"
                | "vdprintf"
                | "scanf"
                | "fscanf"
                | "sscanf"
                | "syslog"
                | "err"
                | "errx"
                | "warn"
                | "warnx"
                | "error"
        )
    }

    fn get_format_arg_index(&self, func_name: &str) -> usize {
        match func_name {
            "snprintf" | "vsnprintf" => 2, // Third argument is format string (buffer, size, format, ...)
            "sprintf" | "vsprintf" | "sscanf" | "fprintf" | "fscanf" | "vfprintf" | "syslog"
            | "dprintf" | "vdprintf" => 1, // Second argument is format string
            // BSD/POSIX err/errx have an initial exit/status code, then format string
            "err" | "errx" => 1,
            // warn/warnx take the format string as first argument
            "warn" | "warnx" => 0,
            // GNU error(int status, int errnum, const char *format, ...) => format at index 2
            "error" => 2,
            _ => 0, // First argument is format string (printf, scanf, vprintf, etc.)
        }
    }

    fn is_user_input_source(&self, node: &Node, source: &str) -> bool {
        match node.kind() {
            "call_expression" => {
                if let Some(function) = node.child_by_field_name("function") {
                    let func_name = ast_utils::get_node_text_owned(&function, source);
                    return matches!(
                        func_name.as_str(),
                        "fgets"
                            | "gets"
                            | "getline"
                            | "getdelim"
                            | "fgetc"
                            | "getc"
                            | "getchar"
                            | "fread"
                            | "read"
                            | "recv"
                            | "recvfrom"
                            | "recvmsg"
                            | "getenv"
                            | "getpwnam"
                            | "getpwuid"
                            | "getgrnam"
                            | "getgrgid"
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
            _ => false,
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
            _ => false,
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
    /// - Function parameters (conservative approach)
    /// - Unknown or untracked variables matching suspicious patterns
    fn is_potentially_unsafe_format_string(&self, node: &Node, source: &str) -> bool {
        match node.kind() {
            "string_literal" | "concatenated_string" | "string_content" => {
                // Literal strings are always safe as format strings
                false
            }
            "identifier" => {
                let var_name = ast_utils::get_node_text_owned(node, source);
                // Unsafe if it's user input
                if self.user_input_vars.contains(&var_name) {
                    return true;
                }
                // Explicitly safe variables are allowed
                if self.safe_vars.contains(&var_name) {
                    return false;
                }
                // Function parameters should be treated as potentially tainted for format strings
                // This is conservative but prevents vulnerabilities
                if self.function_parameters.contains(&var_name) {
                    return true;
                }
                // For other variables, use heuristic
                self.could_be_user_input(&var_name)
            }
            "subscript_expression" => {
                // Array access could be user input (e.g., argv[1])
                if let Some(array) = node.child_by_field_name("argument") {
                    if array.kind() == "identifier" {
                        let array_name = ast_utils::get_node_text_owned(&array, source);
                        if self.user_input_vars.contains(&array_name) || array_name == "argv" {
                            return true;
                        }
                    }
                }
                // Also check if the index is tainted (e.g., formats[tainted_index])
                if let Some(index) = node.child_by_field_name("index") {
                    if self.expression_contains_taint(&index, source) {
                        return true; // Tainted index means we can't trust the result
                    }
                }
                false // Safe if array and index are both untainted
            }
            "call_expression" => {
                // Function calls that return strings could be unsafe
                if let Some(function) = node.child_by_field_name("function") {
                    let func_name = ast_utils::get_node_text_owned(&function, source);
                    // Functions that typically return user-controlled data
                    return matches!(
                        func_name.as_str(),
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
        // Balance between catching vulnerabilities and avoiding false positives
        let name_lower = var_name.to_lowercase();

        // Strong indicators of user input
        if name_lower.contains("user_input")
            || name_lower.contains("user_data")
            || name_lower.contains("argv")
            || name_lower.contains("getenv")
            || name_lower.contains("stdin")
        {
            return true;
        }

        // Exclude common CONST pattern names (uppercase with _ pattern suggests const)
        // But keep lowercase variants suspicious (e.g., global_format vs INFO_FORMAT)
        if var_name.chars().filter(|c| c.is_uppercase()).count() > var_name.len() / 2
            && var_name.contains('_')
        {
            // Looks like a CONST_STYLE_NAME, probably safe
            return false;
        }

        // Check for suspicious patterns in non-const-style names
        name_lower.contains("user")
            || (name_lower.contains("input") && !name_lower.starts_with("max_input"))
            || name_lower.contains("cmd")
            || name_lower.contains("command")
            || (name_lower.contains("format") && !var_name.chars().next().unwrap_or('a').is_uppercase())
            || (name_lower.contains("buf") && name_lower.contains("user"))
            || (name_lower.contains("msg") && (name_lower.contains("user") || name_lower.contains("error")))
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
            _ => "unknown".to_string(),
        }
    }
}

// DEPRECATED: Inline tests moved to src/rules/cert_c/tests/inline/
// #[cfg(test)]
// #[path = "tests/fio30_c.rs"]
// mod tests;

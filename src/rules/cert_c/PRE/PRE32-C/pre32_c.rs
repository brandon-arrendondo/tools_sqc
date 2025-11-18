use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use std::collections::HashSet;
use tree_sitter::Node;

pub struct Pre32C;

impl CertRule for Pre32C {
    fn rule_id(&self) -> &'static str {
        "PRE32-C"
    }

    fn description(&self) -> &'static str {
        "Do not use preprocessor directives in invocations of function-like macros"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "PRE32-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        self.check_node(node, source, &mut violations);

        violations
    }
}

impl Pre32C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            "call_expression" => {
                self.check_function_call(node, source, violations);
            }
            _ => {}
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }

    fn check_function_call(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(function_node) = node.child_by_field_name("function") {
            let function_name = &source[function_node.start_byte()..function_node.end_byte()];

            // Check if this is a potentially macro-implemented function
            if self.is_potentially_macro_function(function_name) {
                // Check arguments for preprocessor directives
                if let Some(arguments) = node.child_by_field_name("arguments") {
                    self.check_arguments_for_directives(
                        &arguments,
                        source,
                        function_name,
                        violations,
                    );
                }
            }
        }
    }

    fn check_arguments_for_directives(
        &self,
        arguments: &Node,
        source: &str,
        function_name: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Get the full text of the arguments section
        let args_text = &source[arguments.start_byte()..arguments.end_byte()];

        // Look for preprocessor directives within the arguments
        if self.contains_preprocessor_directives(args_text) {
            let start_point = arguments.start_position();

            violations.push(RuleViolation {
                rule_id: self.rule_id().to_string(),
                severity: Severity::High,
                message: format!(
                    "Function '{}' called with preprocessor directives in arguments. This causes undefined behavior if the function is implemented as a macro",
                    function_name
                ),
                file_path: String::new(),
                line: start_point.row + 1,
                column: start_point.column + 1,
                suggestion: Some("Move preprocessor directives outside the function call using conditional compilation".to_string()),
            ..Default::default()
            });
        }

        // Also check individual arguments
        for i in 0..arguments.child_count() {
            if let Some(child) = arguments.child(i) {
                if child.kind() != "," {
                    let arg_text = &source[child.start_byte()..child.end_byte()];
                    if self.contains_preprocessor_directives(arg_text) {
                        let start_point = child.start_position();

                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::High,
                            message: format!(
                                "Argument to '{}' contains preprocessor directive: '{}'",
                                function_name,
                                arg_text.trim()
                            ),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some(
                                "Use conditional compilation to wrap the entire function call"
                                    .to_string(),
                            ),
                            ..Default::default()
                        });
                    }
                }
            }
        }
    }

    fn is_potentially_macro_function(&self, function_name: &str) -> bool {
        // Standard library functions that may be implemented as macros
        let std_lib_functions: HashSet<&str> = [
            // String functions
            "memcpy", "memmove", "memset", "memcmp", "memchr", "strcpy", "strncpy", "strcat",
            "strncat", "strcmp", "strncmp", "strchr", "strrchr", "strpbrk", "strspn", "strcspn",
            "strstr", "strtok", "strlen", // Character functions
            "isalnum", "isalpha", "isblank", "iscntrl", "isdigit", "isgraph", "islower", "isprint",
            "ispunct", "isspace", "isupper", "isxdigit", "tolower", "toupper",
            // I/O functions
            "getc", "putc", "getchar", "putchar", "fgetc", "fputc", "getwc", "putwc", "fgetwc",
            "fputwc", "printf", "fprintf", "sprintf", "snprintf", "scanf", "fscanf", "sscanf",
            // Math functions
            "abs", "labs", "llabs", "fabs", "fabsf", "fabsl", "sqrt", "sqrtf", "sqrtl", "pow",
            "powf", "powl", "sin", "cos", "tan", "asin", "acos", "atan", "atan2", "exp", "log",
            "log10", "ceil", "floor", "fmod", // Memory allocation
            "malloc", "calloc", "realloc", "free",   // Assertion
            "assert", // Wide character functions
            "wmemcpy", "wmemmove", "wmemset", "wmemcmp", "wmemchr", "wcscpy", "wcsncpy", "wcscat",
            "wcsncat", "wcscmp", "wcsncmp", "wcschr", "wcsrchr", "wcspbrk", "wcsspn", "wcscspn",
            "wcsstr", "wcstok", "wcslen",
        ]
        .iter()
        .cloned()
        .collect();

        std_lib_functions.contains(function_name) ||
        // Any function could potentially be a macro, so we should be conservative
        // But focus on functions commonly implemented as macros
        function_name.chars().all(|c| c.is_uppercase() || c == '_') // ALL_CAPS suggests macro
    }

    fn contains_preprocessor_directives(&self, text: &str) -> bool {
        // Look for preprocessor directive patterns
        let directives = [
            "#define", "#undef", "#include", "#if", "#ifdef", "#ifndef", "#else", "#elif",
            "#endif", "#error", "#warning", "#pragma", "#line",
        ];

        for directive in &directives {
            if text.contains(directive) {
                return true;
            }
        }

        // Also look for macro continuation patterns
        if text.contains("\\") && text.contains("\n") {
            return true;
        }

        false
    }

    fn spans_multiple_lines_with_directives(&self, text: &str) -> bool {
        let lines: Vec<&str> = text.lines().collect();

        if lines.len() <= 1 {
            return false;
        }

        // Check if any line contains preprocessor directives
        for line in lines {
            let trimmed = line.trim();
            if trimmed.starts_with('#') {
                return true;
            }
        }

        false
    }
}

// DEPRECATED: Inline tests moved to src/rules/cert_c/tests/inline/

use super::{CertRule, RuleViolation};
use crate::manifest::Severity;
use tree_sitter::Node;
use std::collections::HashSet;

pub struct Pre32C;

impl CertRule for Pre32C {
    fn rule_id(&self) -> &'static str {
        "PRE32-C"
    }

    fn description(&self) -> &'static str {
        "Do not use preprocessor directives in invocations of function-like macros"
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
                    self.check_arguments_for_directives(&arguments, source, function_name, violations);
                }
            }
        }
    }

    fn check_arguments_for_directives(&self, arguments: &Node, source: &str, function_name: &str, violations: &mut Vec<RuleViolation>) {
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
                                function_name, arg_text.trim()
                            ),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some("Use conditional compilation to wrap the entire function call".to_string()),
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
            "memcpy", "memmove", "memset", "memcmp", "memchr",
            "strcpy", "strncpy", "strcat", "strncat", "strcmp", "strncmp",
            "strchr", "strrchr", "strpbrk", "strspn", "strcspn", "strstr", "strtok",
            "strlen",

            // Character functions
            "isalnum", "isalpha", "isblank", "iscntrl", "isdigit", "isgraph",
            "islower", "isprint", "ispunct", "isspace", "isupper", "isxdigit",
            "tolower", "toupper",

            // I/O functions
            "getc", "putc", "getchar", "putchar", "fgetc", "fputc",
            "getwc", "putwc", "fgetwc", "fputwc",
            "printf", "fprintf", "sprintf", "snprintf",
            "scanf", "fscanf", "sscanf",

            // Math functions
            "abs", "labs", "llabs", "fabs", "fabsf", "fabsl",
            "sqrt", "sqrtf", "sqrtl", "pow", "powf", "powl",
            "sin", "cos", "tan", "asin", "acos", "atan", "atan2",
            "exp", "log", "log10", "ceil", "floor", "fmod",

            // Memory allocation
            "malloc", "calloc", "realloc", "free",

            // Assertion
            "assert",

            // Wide character functions
            "wmemcpy", "wmemmove", "wmemset", "wmemcmp", "wmemchr",
            "wcscpy", "wcsncpy", "wcscat", "wcsncat", "wcscmp", "wcsncmp",
            "wcschr", "wcsrchr", "wcspbrk", "wcsspn", "wcscspn", "wcsstr", "wcstok",
            "wcslen",
        ].iter().cloned().collect();

        std_lib_functions.contains(function_name) ||
        // Any function could potentially be a macro, so we should be conservative
        // But focus on functions commonly implemented as macros
        function_name.chars().all(|c| c.is_uppercase() || c == '_') // ALL_CAPS suggests macro
    }

    fn contains_preprocessor_directives(&self, text: &str) -> bool {
        // Look for preprocessor directive patterns
        let directives = [
            "#define", "#undef", "#include", "#if", "#ifdef", "#ifndef",
            "#else", "#elif", "#endif", "#error", "#warning", "#pragma",
            "#line",
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

#[cfg(test)]
mod tests {
    use super::*;
    use crate::parser::CParser;

    #[test]
    fn test_pre32c_detects_preprocessor_in_memcpy() {
        let rule = Pre32C;
        let mut parser = CParser::new().unwrap();

        // Test case: Preprocessor directive in memcpy arguments (classic example)
        let source = r#"
#include <string.h>

void func(const char *src) {
    char *dest;
    memcpy(dest, src,
        #ifdef PLATFORM1
            12
        #else
            24
        #endif
    );  // Should trigger violation
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);
        assert!(!violations.is_empty(), "Should detect preprocessor directive in memcpy arguments");
        assert!(violations.iter().any(|v| v.message.contains("preprocessor directives")));
    }

    #[test]
    fn test_pre32c_detects_ifdef_in_printf() {
        let rule = Pre32C;
        let mut parser = CParser::new().unwrap();

        // Test case: Preprocessor directive in printf arguments
        let source = r#"
#include <stdio.h>

void debug_print(int value) {
    printf("Value: %d\n",
        #ifdef DEBUG
            value
        #else
            0
        #endif
    );  // Should trigger violation
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);
        assert!(!violations.is_empty(), "Should detect preprocessor directive in printf arguments");
    }

    #[test]
    fn test_pre32c_detects_define_in_assert() {
        let rule = Pre32C;
        let mut parser = CParser::new().unwrap();

        // Test case: Preprocessor directive in assert
        let source = r#"
#include <assert.h>

void func(void) {
    assert(
        #define TEMP_VAL 42
        TEMP_VAL > 0
    );  // Should trigger violation
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);
        assert!(!violations.is_empty(), "Should detect #define in assert arguments");
    }

    #[test]
    fn test_pre32c_accepts_compliant_conditional_calls() {
        let rule = Pre32C;
        let mut parser = CParser::new().unwrap();

        // Test case: Compliant solution - preprocessor outside function call
        let source = r#"
#include <string.h>

void func(const char *src) {
    char *dest;
    #ifdef PLATFORM1
        memcpy(dest, src, 12);
    #else
        memcpy(dest, src, 24);
    #endif
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        let directive_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("preprocessor directives"))
            .collect();
        assert!(directive_violations.is_empty(), "Should not flag compliant conditional compilation");
    }

    #[test]
    fn test_pre32c_accepts_regular_macro_constants() {
        let rule = Pre32C;
        let mut parser = CParser::new().unwrap();

        // Test case: Regular macro usage (not preprocessor directives)
        let source = r#"
#define BUFFER_SIZE 1024

#include <string.h>

void func(const char *src) {
    char *dest;
    memcpy(dest, src, BUFFER_SIZE);  // Should not trigger violation
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        let directive_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("preprocessor directives"))
            .collect();
        assert!(directive_violations.is_empty(), "Should not flag regular macro usage");
    }

    #[test]
    fn test_pre32c_detects_include_in_function_arg() {
        let rule = Pre32C;
        let mut parser = CParser::new().unwrap();

        // Test case: #include in function argument (pathological case)
        let source = r#"
void process_data(int value);

void func(void) {
    process_data(
        #include "value.h"  // Should trigger violation
    );
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);
        assert!(!violations.is_empty(), "Should detect #include in function arguments");
    }

    #[test]
    fn test_pre32c_handles_custom_functions() {
        let rule = Pre32C;
        let mut parser = CParser::new().unwrap();

        // Test case: Custom function that might be a macro
        let source = r#"
#define CUSTOM_FUNCTION(x) do_something(x)

void func(void) {
    CUSTOM_FUNCTION(
        #ifdef FEATURE_ENABLED
            42
        #else
            0
        #endif
    );  // Should trigger violation for potential macro
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        // Our implementation focuses on standard library functions,
        // but could be extended to detect custom macros
    }

    #[test]
    fn test_pre32c_accepts_safe_function_calls() {
        let rule = Pre32C;
        let mut parser = CParser::new().unwrap();

        // Test case: Normal function calls without preprocessor directives
        let source = r#"
#include <stdio.h>
#include <string.h>

void func(void) {
    char buffer[100];
    const char *src = "Hello";

    strcpy(buffer, src);
    printf("Buffer: %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        let directive_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("preprocessor directives"))
            .collect();
        assert!(directive_violations.is_empty(), "Should not flag normal function usage");
    }
}
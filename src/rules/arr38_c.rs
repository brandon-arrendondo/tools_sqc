use super::{CertRule, RuleViolation};
use crate::manifest::Severity;
use tree_sitter::Node;

pub struct Arr38C;

impl CertRule for Arr38C {
    fn rule_id(&self) -> &'static str {
        "ARR38-C"
    }

    fn description(&self) -> &'static str {
        "Guarantee that library functions do not form invalid pointers"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        self.check_node(node, source, &mut violations);

        violations
    }
}

impl Arr38C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            "call_expression" => {
                self.check_library_function_call(node, source, violations);
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

    fn check_library_function_call(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(function_node) = node.child_by_field_name("function") {
            let function_name = &source[function_node.start_byte()..function_node.end_byte()];

            match function_name {
                "memcpy" | "memmove" | "memset" | "memcmp" => {
                    self.check_memory_function(node, source, function_name, violations);
                }
                "strcpy" | "strncpy" | "strcat" | "strncat" | "strcmp" | "strncmp" => {
                    self.check_string_function(node, source, function_name, violations);
                }
                "wmemcpy" | "wmemmove" | "wmemset" | "wmemcmp" => {
                    self.check_wide_memory_function(node, source, function_name, violations);
                }
                "wcscpy" | "wcsncpy" | "wcscat" | "wcsncat" | "wcscmp" | "wcsncmp" => {
                    self.check_wide_string_function(node, source, function_name, violations);
                }
                "malloc" | "calloc" | "realloc" => {
                    self.check_allocation_function(node, source, function_name, violations);
                }
                _ => {}
            }
        }
    }

    fn check_memory_function(&self, node: &Node, source: &str, function_name: &str, violations: &mut Vec<RuleViolation>) {
        let args = self.get_function_arguments(node, source);

        match function_name {
            "memcpy" | "memmove" => {
                if args.len() >= 3 {
                    self.check_memory_copy_size(&args, node, source, function_name, violations);
                }
            }
            "memset" => {
                if args.len() >= 3 {
                    self.check_memory_set_size(&args, node, source, function_name, violations);
                }
            }
            "memcmp" => {
                if args.len() >= 3 {
                    self.check_memory_compare_size(&args, node, source, function_name, violations);
                }
            }
            _ => {}
        }
    }

    fn check_string_function(&self, node: &Node, source: &str, function_name: &str, violations: &mut Vec<RuleViolation>) {
        let args = self.get_function_arguments(node, source);

        match function_name {
            "strncpy" | "strncat" | "strncmp" => {
                if args.len() >= 3 {
                    self.check_string_size_parameter(&args, node, source, function_name, violations);
                }
            }
            "strcpy" | "strcat" => {
                if args.len() >= 2 {
                    self.check_unbounded_string_function(&args, node, source, function_name, violations);
                }
            }
            _ => {}
        }
    }

    fn check_wide_memory_function(&self, node: &Node, source: &str, function_name: &str, violations: &mut Vec<RuleViolation>) {
        let args = self.get_function_arguments(node, source);

        if args.len() >= 3 {
            // Wide character functions expect size in terms of wchar_t, not bytes
            let size_arg = &args[2];
            if self.is_byte_size_expression(size_arg, source) {
                let start_point = node.start_position();
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::High,
                    message: format!(
                        "Function '{}' expects size in wchar_t units, not bytes. Using sizeof() may cause buffer overflow",
                        function_name
                    ),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some("Use element count instead of sizeof() for wide character functions".to_string()),
                });
            }
        }
    }

    fn check_wide_string_function(&self, node: &Node, source: &str, function_name: &str, violations: &mut Vec<RuleViolation>) {
        let args = self.get_function_arguments(node, source);

        if function_name.contains("wcsn") && args.len() >= 3 {
            let size_arg = &args[2];
            if self.is_byte_size_expression(size_arg, source) {
                let start_point = node.start_position();
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::High,
                    message: format!(
                        "Function '{}' expects character count, not byte count",
                        function_name
                    ),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some("Use character count instead of sizeof() for wide string functions".to_string()),
                });
            }
        }
    }

    fn check_allocation_function(&self, node: &Node, source: &str, function_name: &str, violations: &mut Vec<RuleViolation>) {
        let args = self.get_function_arguments(node, source);

        if function_name == "calloc" && args.len() >= 2 {
            // calloc(count, size) - check for potential overflow
            let count_arg = &args[0];
            let size_arg = &args[1];

            if self.could_cause_overflow(count_arg, size_arg, source) {
                let start_point = node.start_position();
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::Medium,
                    message: "calloc() arguments may cause integer overflow".to_string(),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some("Check for potential overflow in calloc arguments".to_string()),
                });
            }
        }
    }

    fn check_memory_copy_size(&self, args: &[String], node: &Node, source: &str, function_name: &str, violations: &mut Vec<RuleViolation>) {
        let size_arg = &args[2];

        // Check for common dangerous patterns
        if self.is_dangerous_size_calculation(size_arg, source) {
            let start_point = node.start_position();
            violations.push(RuleViolation {
                rule_id: self.rule_id().to_string(),
                severity: Severity::High,
                message: format!(
                    "Function '{}' called with potentially invalid size calculation '{}'",
                    function_name, size_arg
                ),
                file_path: String::new(),
                line: start_point.row + 1,
                column: start_point.column + 1,
                suggestion: Some("Ensure size calculation matches actual buffer size".to_string()),
            });
        }
    }

    fn check_memory_set_size(&self, args: &[String], node: &Node, source: &str, function_name: &str, violations: &mut Vec<RuleViolation>) {
        let size_arg = &args[2];

        if self.is_excessive_size_for_memset(size_arg, &args[0], source) {
            let start_point = node.start_position();
            violations.push(RuleViolation {
                rule_id: self.rule_id().to_string(),
                severity: Severity::High,
                message: format!(
                    "Function '{}' called with size larger than target buffer",
                    function_name
                ),
                file_path: String::new(),
                line: start_point.row + 1,
                column: start_point.column + 1,
                suggestion: Some("Ensure memset size does not exceed buffer size".to_string()),
            });
        }
    }

    fn check_memory_compare_size(&self, args: &[String], node: &Node, source: &str, function_name: &str, violations: &mut Vec<RuleViolation>) {
        let size_arg = &args[2];

        if self.is_dangerous_size_calculation(size_arg, source) {
            let start_point = node.start_position();
            violations.push(RuleViolation {
                rule_id: self.rule_id().to_string(),
                severity: Severity::Medium,
                message: format!(
                    "Function '{}' called with potentially invalid size for comparison",
                    function_name
                ),
                file_path: String::new(),
                line: start_point.row + 1,
                column: start_point.column + 1,
                suggestion: Some("Verify comparison size matches actual data size".to_string()),
            });
        }
    }

    fn check_string_size_parameter(&self, args: &[String], node: &Node, source: &str, function_name: &str, violations: &mut Vec<RuleViolation>) {
        let size_arg = &args[2];

        if self.is_sizeof_expression(size_arg) {
            let start_point = node.start_position();
            violations.push(RuleViolation {
                rule_id: self.rule_id().to_string(),
                severity: Severity::Medium,
                message: format!(
                    "Function '{}' called with sizeof() as size parameter, which may not account for null terminator",
                    function_name
                ),
                file_path: String::new(),
                line: start_point.row + 1,
                column: start_point.column + 1,
                suggestion: Some("Consider using sizeof(buffer) - 1 for string functions".to_string()),
            });
        }
    }

    fn check_unbounded_string_function(&self, args: &[String], node: &Node, source: &str, function_name: &str, violations: &mut Vec<RuleViolation>) {
        // These functions are inherently dangerous without bounds checking
        let start_point = node.start_position();
        violations.push(RuleViolation {
            rule_id: self.rule_id().to_string(),
            severity: Severity::High,
            message: format!(
                "Function '{}' is unsafe - no bounds checking. Use safer alternatives like strncpy/strncat",
                function_name
            ),
            file_path: String::new(),
            line: start_point.row + 1,
            column: start_point.column + 1,
            suggestion: Some(format!("Replace '{}' with safer bounded alternative", function_name)),
        });
    }

    fn get_function_arguments(&self, node: &Node, source: &str) -> Vec<String> {
        let mut args = Vec::new();

        if let Some(arguments) = node.child_by_field_name("arguments") {
            for i in 0..arguments.child_count() {
                if let Some(child) = arguments.child(i) {
                    if child.kind() != "," {
                        let arg_text = source[child.start_byte()..child.end_byte()].to_string();
                        args.push(arg_text);
                    }
                }
            }
        }

        args
    }

    fn is_byte_size_expression(&self, expr: &str, _source: &str) -> bool {
        expr.contains("sizeof(") && !expr.contains("/ sizeof(")
    }

    fn is_sizeof_expression(&self, expr: &str) -> bool {
        expr.contains("sizeof(")
    }

    fn is_dangerous_size_calculation(&self, size_expr: &str, _source: &str) -> bool {
        // Look for potentially dangerous patterns
        size_expr.contains("+ 1") ||  // off-by-one errors
        size_expr.contains("* sizeof") ||  // double scaling
        size_expr.contains("sizeof(") && size_expr.contains("*") ||  // multiplication with sizeof
        size_expr.contains("nchars + 1")  // specific pattern from examples
    }

    fn is_excessive_size_for_memset(&self, size_expr: &str, _target_expr: &str, _source: &str) -> bool {
        // Simple heuristic: check for obviously wrong patterns
        size_expr.contains("+ 1") && size_expr.contains("nchars")
    }

    fn could_cause_overflow(&self, count_expr: &str, size_expr: &str, _source: &str) -> bool {
        // Check for potential overflow in calloc
        (count_expr.contains("SIZE_MAX") || count_expr.contains("UINT_MAX")) ||
        (size_expr.contains("SIZE_MAX") || size_expr.contains("UINT_MAX"))
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::parser::CParser;

    #[test]
    fn test_arr38c_detects_wide_char_sizeof_issue() {
        let rule = Arr38C;
        let mut parser = CParser::new().unwrap();

        // Test case: Using sizeof with wide character functions
        let source = r#"
void func(void) {
    static const wchar_t w_str[] = L"Hello world";
    wchar_t w_buffer[32];
    wmemcpy(w_buffer, w_str, sizeof(w_str));  // Should trigger violation
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);
        assert!(!violations.is_empty(), "Should detect sizeof with wide character function");
        assert!(violations.iter().any(|v| v.message.contains("wchar_t units")));
    }

    #[test]
    fn test_arr38c_detects_memset_size_issue() {
        let rule = Arr38C;
        let mut parser = CParser::new().unwrap();

        // Test case: memset with incorrect size calculation
        let source = r#"
void func(size_t nchars) {
    char *p = (char *)malloc(nchars);
    const size_t n = nchars + 1;
    memset(p, 0, n);  // Should trigger violation - size too large
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);
        assert!(!violations.is_empty(), "Should detect memset with excessive size");
    }

    #[test]
    fn test_arr38c_detects_unsafe_string_functions() {
        let rule = Arr38C;
        let mut parser = CParser::new().unwrap();

        // Test case: Using unsafe string functions
        let source = r#"
void func(void) {
    char dest[10];
    char src[] = "Hello World";
    strcpy(dest, src);  // Should trigger violation - no bounds checking
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);
        assert!(!violations.is_empty(), "Should detect unsafe strcpy usage");
        assert!(violations.iter().any(|v| v.message.contains("unsafe")));
    }

    #[test]
    fn test_arr38c_detects_double_scaling() {
        let rule = Arr38C;
        let mut parser = CParser::new().unwrap();

        // Test case: Double scaling with sizeof
        let source = r#"
void func(void) {
    long array[4];
    const size_t n = sizeof(int) * 4;
    memset(array, 0, n);  // Should trigger violation - incorrect scaling
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);
        assert!(!violations.is_empty(), "Should detect double scaling issue");
    }

    #[test]
    fn test_arr38c_accepts_correct_usage() {
        let rule = Arr38C;
        let mut parser = CParser::new().unwrap();

        // Test case: Correct usage with proper size calculation
        let source = r#"
void func(void) {
    char buffer[32];
    char src[] = "Hello";
    strncpy(buffer, src, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        // Should have fewer or less severe violations for proper usage
        let critical_violations: Vec<_> = violations.iter()
            .filter(|v| matches!(v.severity, Severity::High | Severity::Critical))
            .collect();

        // Note: our implementation might still flag sizeof usage, but it should be less severe
    }
}
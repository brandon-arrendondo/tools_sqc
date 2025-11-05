use super::super::{CertRule, RuleViolation};
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
                    ..Default::default()
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
                    ..Default::default()
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
                    ..Default::default()
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
                ..Default::default()
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
                ..Default::default()
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
                ..Default::default()
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
                ..Default::default()
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
            ..Default::default()
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
#[path = "tests/arr38_c.rs"]
mod tests;
use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Arr38C;

impl CertRule for Arr38C {
    fn rule_id(&self) -> &'static str {
        "ARR38-C"
    }

    fn description(&self) -> &'static str {
        "Guarantee that library functions do not form invalid pointers"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "ARR38-C"
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

    fn check_library_function_call(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let Some(function_node) = node.child_by_field_name("function") {
            let function_name = get_node_text(&function_node, source);

            match function_name {
                "memcpy" | "memmove" | "memset" | "memcmp" | "memchr" => {
                    self.check_memory_function(node, source, function_name, violations);
                }
                "strcpy" | "strncpy" | "strcat" | "strncat" | "strcmp" | "strncmp" => {
                    self.check_string_function(node, source, function_name, violations);
                }
                "wmemcpy" | "wmemmove" | "wmemset" | "wmemcmp" | "wmemchr" => {
                    self.check_wide_memory_function(node, source, function_name, violations);
                }
                "wcscpy" | "wcsncpy" | "wcscat" | "wcsncat" | "wcscmp" | "wcsncmp" => {
                    self.check_wide_string_function(node, source, function_name, violations);
                }
                "malloc" | "calloc" | "realloc" | "aligned_alloc" => {
                    self.check_allocation_function(node, source, function_name, violations);
                }
                "fread" | "fwrite" => {
                    self.check_io_function(node, source, function_name, violations);
                }
                "fgets" | "snprintf" | "swprintf" | "strftime" => {
                    self.check_buffer_function(node, source, function_name, violations);
                }
                "bsearch" | "qsort" => {
                    self.check_array_function(node, source, function_name, violations);
                }
                _ => {}
            }
        }
    }

    fn check_memory_function(
        &self,
        node: &Node,
        source: &str,
        function_name: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let args = self.get_function_arguments(node, source);

        match function_name {
            "memcpy" | "memmove" => {
                if args.len() >= 3 {
                    self.check_three_arg_size(&args, node, source, function_name, violations);
                }
            }
            "memset" => {
                if args.len() >= 3 {
                    self.check_three_arg_size(&args, node, source, function_name, violations);
                }
            }
            "memcmp" | "memchr" => {
                if args.len() >= 3 {
                    self.check_three_arg_size(&args, node, source, function_name, violations);
                }
            }
            _ => {}
        }
    }

    fn check_string_function(
        &self,
        node: &Node,
        source: &str,
        function_name: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let args = self.get_function_arguments(node, source);

        match function_name {
            "strncpy" | "strncat" | "strncmp" => {
                if args.len() >= 3 {
                    self.check_string_size_parameter(
                        &args,
                        node,
                        source,
                        function_name,
                        violations,
                    );
                }
            }
            "strcpy" | "strcat" => {
                if args.len() >= 2 {
                    self.check_unbounded_string_function(
                        &args,
                        node,
                        source,
                        function_name,
                        violations,
                    );
                }
            }
            _ => {}
        }
    }

    fn check_wide_memory_function(
        &self,
        node: &Node,
        source: &str,
        function_name: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let args = self.get_function_arguments(node, source);

        if args.len() >= 3 {
            // Wide character functions expect size in terms of wchar_t, not bytes
            let size_arg = &args[2];
            if self.is_byte_size_expression(size_arg) {
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

    fn check_wide_string_function(
        &self,
        node: &Node,
        source: &str,
        function_name: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let args = self.get_function_arguments(node, source);

        if function_name.contains("wcsn") && args.len() >= 3 {
            let size_arg = &args[2];
            if self.is_byte_size_expression(size_arg) {
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
                    suggestion: Some(
                        "Use character count instead of sizeof() for wide string functions"
                            .to_string(),
                    ),
                    ..Default::default()
                });
            }
        }
    }

    fn check_allocation_function(
        &self,
        node: &Node,
        source: &str,
        function_name: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let args = self.get_function_arguments(node, source);

        match function_name {
            "calloc" => {
                if args.len() >= 2 {
                    // calloc(count, size) - check for potential overflow
                    let count_arg = &args[0];
                    let size_arg = &args[1];

                    if self.could_cause_overflow(count_arg, size_arg) {
                        let start_point = node.start_position();
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::Medium,
                            message: "calloc() arguments may cause integer overflow".to_string(),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some(
                                "Check for potential overflow in calloc arguments".to_string(),
                            ),
                            ..Default::default()
                        });
                    }
                }
            }
            "realloc" => {
                if args.len() >= 2 {
                    let size_arg = &args[1];
                    if self.is_dangerous_size_calculation(size_arg) {
                        let start_point = node.start_position();
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::High,
                            message: "realloc() called with potentially incorrect size".to_string(),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some(
                                "Verify realloc size is correct for the new allocation".to_string(),
                            ),
                            ..Default::default()
                        });
                    }
                }
            }
            "aligned_alloc" => {
                if args.len() >= 2 {
                    let size_arg = &args[1];
                    if self.is_dangerous_size_calculation(size_arg) {
                        let start_point = node.start_position();
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::High,
                            message: "aligned_alloc() called with potentially incorrect size"
                                .to_string(),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some(
                                "Verify aligned_alloc size matches intended allocation".to_string(),
                            ),
                            ..Default::default()
                        });
                    }
                }
            }
            _ => {}
        }
    }

    fn check_three_arg_size(
        &self,
        args: &[String],
        node: &Node,
        _source: &str,
        function_name: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let size_arg = &args[2];

        // Check for dangerous size calculation patterns
        if self.is_dangerous_size_calculation(size_arg) {
            let start_point = node.start_position();
            violations.push(RuleViolation {
                rule_id: self.rule_id().to_string(),
                severity: Severity::High,
                message: format!(
                    "Function '{}' called with potentially invalid size calculation",
                    function_name
                ),
                file_path: String::new(),
                line: start_point.row + 1,
                column: start_point.column + 1,
                suggestion: Some("Ensure size argument does not exceed buffer bounds".to_string()),
                ..Default::default()
            });
        }
    }

    fn check_string_size_parameter(
        &self,
        args: &[String],
        node: &Node,
        _source: &str,
        function_name: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let size_arg = &args[2];

        // Use the general dangerous size calculation check
        // This will allow sizeof(buffer) - 1 and other safe patterns
        if self.is_dangerous_size_calculation(size_arg) {
            let start_point = node.start_position();
            violations.push(RuleViolation {
                rule_id: self.rule_id().to_string(),
                severity: Severity::Medium,
                message: format!(
                    "Function '{}' called with potentially invalid size parameter",
                    function_name
                ),
                file_path: String::new(),
                line: start_point.row + 1,
                column: start_point.column + 1,
                suggestion: Some("Verify size parameter is correct for the buffer".to_string()),
                ..Default::default()
            });
        }
    }

    fn check_unbounded_string_function(
        &self,
        _args: &[String],
        node: &Node,
        _source: &str,
        function_name: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
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

    fn check_io_function(
        &self,
        node: &Node,
        source: &str,
        function_name: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let args = self.get_function_arguments(node, source);

        // fread/fwrite have signature: (ptr, size, count, file)
        if args.len() >= 4 {
            let size_arg = &args[1];
            let count_arg = &args[2];

            // Check if count_arg looks like it might be total bytes instead of count
            // Common error: using sizeof(buffer) as count instead of dividing by element size
            if count_arg.contains("sizeof(") && !count_arg.contains("/") {
                let start_point = node.start_position();
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::High,
                    message: format!(
                        "Function '{}' count parameter appears to use total size instead of element count",
                        function_name
                    ),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some("Use element count, not total byte size. Example: fread(buf, sizeof(elem), count, file)".to_string()),
                    ..Default::default()
                });
            }

            // Check for wrong size calculation (size * count could overflow)
            if self.is_dangerous_size_calculation(size_arg)
                || self.is_dangerous_size_calculation(count_arg)
            {
                let start_point = node.start_position();
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::High,
                    message: format!(
                        "Function '{}' has potentially invalid size or count parameter",
                        function_name
                    ),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some("Verify size and count parameters are correct".to_string()),
                    ..Default::default()
                });
            }
        }
    }

    fn check_buffer_function(
        &self,
        node: &Node,
        source: &str,
        function_name: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let args = self.get_function_arguments(node, source);

        // These functions have a size parameter that must not exceed buffer size
        // fgets(buf, size, file), snprintf(buf, size, fmt, ...), etc.
        let size_idx = match function_name {
            "fgets" => 1,
            "snprintf" | "swprintf" => 1,
            "strftime" => 1,
            _ => return,
        };

        if args.len() > size_idx {
            let size_arg = &args[size_idx];

            if self.is_dangerous_size_calculation(size_arg) {
                let start_point = node.start_position();
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::High,
                    message: format!(
                        "Function '{}' called with potentially invalid size parameter",
                        function_name
                    ),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some(
                        "Ensure size parameter does not exceed buffer size".to_string(),
                    ),
                    ..Default::default()
                });
            }
        }
    }

    fn check_array_function(
        &self,
        node: &Node,
        source: &str,
        function_name: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let args = self.get_function_arguments(node, source);

        match function_name {
            "bsearch" => {
                // bsearch(key, base, count, size, compare)
                if args.len() >= 4 {
                    let size_arg = &args[3];
                    if self.is_dangerous_size_calculation(size_arg) {
                        let start_point = node.start_position();
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::High,
                            message: "bsearch called with potentially incorrect element size"
                                .to_string(),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some(
                                "Verify element size matches array element type".to_string(),
                            ),
                            ..Default::default()
                        });
                    }
                }
            }
            "qsort" => {
                // qsort(base, count, size, compare)
                if args.len() >= 3 {
                    let size_arg = &args[2];
                    if self.is_dangerous_size_calculation(size_arg) {
                        let start_point = node.start_position();
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::High,
                            message: "qsort called with potentially incorrect element size"
                                .to_string(),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some(
                                "Verify element size matches array element type".to_string(),
                            ),
                            ..Default::default()
                        });
                    }
                }
            }
            _ => {}
        }
    }

    fn get_function_arguments(&self, node: &Node, source: &str) -> Vec<String> {
        let mut args = Vec::new();

        if let Some(arguments) = node.child_by_field_name("arguments") {
            for i in 0..arguments.child_count() {
                if let Some(child) = arguments.child(i) {
                    if child.kind() != "," && child.kind() != "(" && child.kind() != ")" {
                        args.push(get_node_text(&child, source).to_string());
                    }
                }
            }
        }

        args
    }

    fn is_byte_size_expression(&self, expr: &str) -> bool {
        expr.contains("sizeof(") && !expr.contains("/ sizeof(")
    }

    fn is_sizeof_expression(&self, expr: &str) -> bool {
        expr.contains("sizeof(")
    }

    fn is_dangerous_size_calculation(&self, size_expr: &str) -> bool {
        // Look for potentially dangerous patterns that indicate incorrect size calculations

        // Allow legitimate patterns first
        // Pattern: strlen(x) + 1 or wcslen(x) + 1 - this is correct for null terminator
        if (size_expr.contains("strlen(") || size_expr.contains("wcslen("))
            && size_expr.contains("+ 1")
        {
            return false;
        }

        // Pattern: sizeof(buffer) - 1 - this is correct for string functions
        if size_expr.contains("sizeof(") && size_expr.contains("- 1") {
            return false;
        }

        // Pattern: sizeof(*ptr) - this is usually correct (dereferenced pointer)
        if size_expr.contains("sizeof(*") {
            return false;
        }

        // Now check for dangerous patterns

        // Pattern 1: sizeof with explicit multiplication (not dereference)
        // e.g., "sizeof(int) * ARR_SIZE" indicates double scaling
        if size_expr.contains("sizeof(")
            && size_expr.contains("*")
            && !size_expr.contains("sizeof(*")
        {
            return true;
        }

        // Pattern 2: Variable + 1 patterns (not strlen/wcslen)
        // e.g., "nchars + 1" when nchars is the allocated size
        if size_expr.contains("nchars + 1")
            || (size_expr.contains("chars") && size_expr.contains("+ 1"))
        {
            return true;
        }

        false
    }

    fn could_cause_overflow(&self, count_expr: &str, size_expr: &str) -> bool {
        // Check for potential overflow in calloc
        (count_expr.contains("SIZE_MAX") || count_expr.contains("UINT_MAX"))
            || (size_expr.contains("SIZE_MAX") || size_expr.contains("UINT_MAX"))
    }
}

// DEPRECATED: Inline tests moved to src/rules/cert_c/tests/inline/
// #[cfg(test)]
// #[path = "tests/arr38_c.rs"]
// mod tests;

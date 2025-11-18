use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use tree_sitter::Node;

pub struct Arr39C;

impl CertRule for Arr39C {
    fn rule_id(&self) -> &'static str {
        "ARR39-C"
    }

    fn description(&self) -> &'static str {
        "Do not add or subtract a scaled integer to a pointer"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "ARR39-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        self.check_node(node, source, &mut violations);

        violations
    }
}

impl Arr39C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            "binary_expression" => {
                self.check_pointer_arithmetic(node, source, violations);
            }
            "assignment_expression" => {
                self.check_assignment_arithmetic(node, source, violations);
            }
            "call_expression" => {
                self.check_function_call_scaling(node, source, violations);
            }
            "while_statement" | "for_statement" => {
                self.check_loop_pointer_arithmetic(node, source, violations);
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

    fn check_pointer_arithmetic(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let Some(operator) = self.get_operator(node, source) {
            if operator == "+" || operator == "-" {
                if let (Some(left), Some(right)) = (
                    node.child_by_field_name("left"),
                    node.child_by_field_name("right"),
                ) {
                    // Check if this is pointer + scaled_integer or pointer - scaled_integer
                    if self.is_pointer_scaled_arithmetic(&left, &right, source) {
                        let start_point = node.start_position();
                        let expr_text = &source[node.start_byte()..node.end_byte()];

                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::High,
                            message: format!(
                                "Scaled integer arithmetic with pointer: '{}'. This results in double scaling",
                                expr_text
                            ),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some("Remove sizeof() or use unscaled integer arithmetic".to_string()),
                        ..Default::default()
                        });
                    }
                }
            }
        }
    }

    fn check_assignment_arithmetic(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let Some(operator) = self.get_assignment_operator(node, source) {
            if operator == "+=" || operator == "-=" {
                if let (Some(left), Some(right)) = (
                    node.child_by_field_name("left"),
                    node.child_by_field_name("right"),
                ) {
                    // Check if this is pointer += scaled_integer
                    if self.is_scaled_integer_expression(&right, source)
                        && self.looks_like_pointer(&left, source)
                    {
                        let start_point = node.start_position();
                        let expr_text = &source[node.start_byte()..node.end_byte()];

                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::High,
                            message: format!(
                                "Scaled integer assignment to pointer: '{}'. This results in double scaling",
                                expr_text
                            ),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some("Use unscaled integer increment without sizeof()".to_string()),
                        ..Default::default()
                        });
                    }
                }
            }
        }
    }

    fn check_function_call_scaling(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let Some(function_node) = node.child_by_field_name("function") {
            let function_name = &source[function_node.start_byte()..function_node.end_byte()];

            // Check specific functions that commonly have scaling issues
            match function_name {
                "fgetws" | "fputws" => {
                    self.check_wide_string_function_scaling(
                        node,
                        source,
                        function_name,
                        violations,
                    );
                }
                "memset" | "memcpy" | "memmove" => {
                    self.check_memory_function_scaling(node, source, function_name, violations);
                }
                _ => {}
            }
        }
    }

    fn check_loop_pointer_arithmetic(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Look for patterns like: while (ptr < (buf + sizeof(buf)))
        let node_text = &source[node.start_byte()..node.end_byte()];

        if node_text.contains("sizeof(") && (node_text.contains(" + ") || node_text.contains(" - "))
        {
            // Look for pointer comparison with sizeof scaling
            if let Some(condition) = self.find_loop_condition(node) {
                if self.has_scaled_pointer_comparison(&condition, source) {
                    let start_point = condition.start_position();

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: "Loop condition uses scaled pointer arithmetic with sizeof(), causing double scaling".to_string(),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Use array element count instead of sizeof() in pointer arithmetic".to_string()),
                    ..Default::default()
                    });
                }
            }
        }
    }

    fn check_wide_string_function_scaling(
        &self,
        node: &Node,
        source: &str,
        function_name: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let args = self.get_function_arguments(node, source);

        for (i, arg) in args.iter().enumerate() {
            if arg.contains("wcslen(") && arg.contains("sizeof(wchar_t)") {
                let start_point = node.start_position();
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::High,
                    message: format!(
                        "Function '{}' argument {} uses scaled arithmetic: '{}'. wcslen already returns character count",
                        function_name, i + 1, arg
                    ),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some("Remove '* sizeof(wchar_t)' multiplication".to_string()),
                ..Default::default()
                });
            }
        }
    }

    fn check_memory_function_scaling(
        &self,
        node: &Node,
        source: &str,
        function_name: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let Some(arguments) = node.child_by_field_name("arguments") {
            // Look for patterns in the first argument (destination pointer arithmetic)
            for i in 0..arguments.child_count() {
                if let Some(arg) = arguments.child(i) {
                    if arg.kind() != "," {
                        let arg_text = &source[arg.start_byte()..arg.end_byte()];

                        // Check for scaled offset patterns like: struct_ptr + offsetof(...) * sizeof(...)
                        if self.is_scaled_offset_pattern(arg_text) {
                            let start_point = arg.start_position();
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                severity: Severity::High,
                                message: format!(
                                    "Function '{}' called with scaled offset. offsetof() result is already scaled",
                                    function_name
                                ),
                                file_path: String::new(),
                                line: start_point.row + 1,
                                column: start_point.column + 1,
                                suggestion: Some("Use char* pointer to avoid scaling or remove extra scaling".to_string()),
                            ..Default::default()
                            });
                        }
                        break; // Only check first argument for destination pointer
                    }
                }
            }
        }
    }

    fn is_pointer_scaled_arithmetic(&self, left: &Node, right: &Node, source: &str) -> bool {
        let _left_text = &source[left.start_byte()..left.end_byte()];
        let _right_text = &source[right.start_byte()..right.end_byte()];

        // Check if left is likely a pointer and right contains scaling
        let left_is_pointer = self.looks_like_pointer_node(left, source);
        let right_is_scaled = self.is_scaled_integer_expression(&right, source);

        left_is_pointer && right_is_scaled
    }

    fn is_scaled_integer_expression(&self, node: &Node, source: &str) -> bool {
        let text = &source[node.start_byte()..node.end_byte()];

        // Common scaled integer patterns
        text.contains("sizeof(")
            || text.contains("offsetof(")
            || (text.contains("*") && (text.contains("sizeof") || text.contains("wcslen")))
            || text.contains("wcslen(") && text.contains("sizeof(wchar_t)")
    }

    fn looks_like_pointer(&self, node: &Node, source: &str) -> bool {
        let text = &source[node.start_byte()..node.end_byte()];

        // Simple heuristics for pointer identification
        text.ends_with("_ptr")
            || text.ends_with("*")
            || text.contains("buf")
            || text.contains("array")
            || text.contains("ptr")
    }

    fn looks_like_pointer_node(&self, node: &Node, source: &str) -> bool {
        match node.kind() {
            "identifier" => {
                let _text = &source[node.start_byte()..node.end_byte()];
                self.looks_like_pointer(node, source)
            }
            "binary_expression" => {
                // Could be pointer arithmetic
                true
            }
            _ => false,
        }
    }

    fn is_scaled_offset_pattern(&self, text: &str) -> bool {
        (text.contains("offsetof(") && text.contains("*"))
            || (text.contains("offsetof(") && text.contains("sizeof("))
    }

    fn has_scaled_pointer_comparison(&self, node: &Node, source: &str) -> bool {
        let text = &source[node.start_byte()..node.end_byte()];

        // Look for patterns like: ptr < (buf + sizeof(buf))
        text.contains("sizeof(")
            && (text.contains(" < ")
                || text.contains(" <= ")
                || text.contains(" > ")
                || text.contains(" >= "))
            && (text.contains(" + ") || text.contains(" - "))
    }

    fn find_loop_condition<'a>(&self, node: &'a Node<'a>) -> Option<Node<'a>> {
        // Find condition in while or for loop
        match node.kind() {
            "while_statement" => node.child_by_field_name("condition"),
            "for_statement" => node.child_by_field_name("condition"),
            _ => None,
        }
    }

    fn get_operator(&self, node: &Node, source: &str) -> Option<String> {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                let text = &source[child.start_byte()..child.end_byte()];
                if matches!(text, "+" | "-" | "*" | "/") {
                    return Some(text.to_string());
                }
            }
        }
        None
    }

    fn get_assignment_operator(&self, node: &Node, source: &str) -> Option<String> {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                let text = &source[child.start_byte()..child.end_byte()];
                if matches!(text, "+=" | "-=" | "*=" | "/=") {
                    return Some(text.to_string());
                }
            }
        }
        None
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
}

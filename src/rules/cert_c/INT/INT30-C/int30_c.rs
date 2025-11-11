use super::super::{CertRule, RuleViolation};
use crate::manifest::{Severity, RuleCategory};
use tree_sitter::Node;

pub struct Int30C;

impl CertRule for Int30C {
    fn rule_id(&self) -> &'static str {
        "INT30-C"
    }

    fn description(&self) -> &'static str {
        "Ensure that unsigned integer operations do not wrap"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "INT30-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        self.check_node(node, source, &mut violations);

        violations
    }
}

impl Int30C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            "binary_expression" => {
                self.check_binary_operation(node, source, violations);
            }
            "assignment_expression" => {
                self.check_assignment_operation(node, source, violations);
            }
            "call_expression" => {
                self.check_function_call(node, source, violations);
            }
            "update_expression" => {
                self.check_increment_decrement(node, source, violations);
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

    fn check_binary_operation(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(operator) = self.get_operator(node, source) {
            match operator.as_str() {
                "+" => self.check_addition(node, source, violations),
                "-" => self.check_subtraction(node, source, violations),
                "*" => self.check_multiplication(node, source, violations),
                "<<" => self.check_left_shift(node, source, violations),
                _ => {}
            }
        }
    }

    fn check_assignment_operation(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(operator) = self.get_assignment_operator(node, source) {
            match operator.as_str() {
                "+=" => self.check_compound_addition(node, source, violations),
                "-=" => self.check_compound_subtraction(node, source, violations),
                "*=" => self.check_compound_multiplication(node, source, violations),
                "<<=" => self.check_compound_left_shift(node, source, violations),
                _ => {}
            }
        }
    }

    fn check_addition(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let (Some(left), Some(right)) = (node.child_by_field_name("left"), node.child_by_field_name("right")) {
            let left_type = self.infer_type(&left, source);
            let right_type = self.infer_type(&right, source);

            if self.is_unsigned_type(&left_type) || self.is_unsigned_type(&right_type) {
                if !self.has_overflow_check_addition(node, source) {
                    let start_point = node.start_position();
                    let expr_text = &source[node.start_byte()..node.end_byte()];

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Unsigned integer addition '{}' may wrap without overflow checking",
                            expr_text
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Add overflow check: if (UINT_MAX - a < b) { /* handle error */ }".to_string()),
                    ..Default::default()
                    });
                }
            }
        }
    }

    fn check_subtraction(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let (Some(left), Some(right)) = (node.child_by_field_name("left"), node.child_by_field_name("right")) {
            let left_type = self.infer_type(&left, source);
            let right_type = self.infer_type(&right, source);

            if self.is_unsigned_type(&left_type) || self.is_unsigned_type(&right_type) {
                if !self.has_overflow_check_subtraction(node, source) {
                    let start_point = node.start_position();
                    let expr_text = &source[node.start_byte()..node.end_byte()];

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Unsigned integer subtraction '{}' may wrap without underflow checking",
                            expr_text
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Add underflow check: if (a < b) { /* handle error */ }".to_string()),
                    ..Default::default()
                    });
                }
            }
        }
    }

    fn check_multiplication(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let (Some(left), Some(right)) = (node.child_by_field_name("left"), node.child_by_field_name("right")) {
            let left_type = self.infer_type(&left, source);
            let right_type = self.infer_type(&right, source);

            if self.is_unsigned_type(&left_type) || self.is_unsigned_type(&right_type) {
                if !self.has_overflow_check_multiplication(node, source) {
                    let start_point = node.start_position();
                    let expr_text = &source[node.start_byte()..node.end_byte()];

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Unsigned integer multiplication '{}' may wrap without overflow checking",
                            expr_text
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Add overflow check: if (a > UINT_MAX / b) { /* handle error */ }".to_string()),
                    ..Default::default()
                    });
                }
            }
        }
    }

    fn check_left_shift(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let (Some(left), Some(right)) = (node.child_by_field_name("left"), node.child_by_field_name("right")) {
            let left_type = self.infer_type(&left, source);

            if self.is_unsigned_type(&left_type) {
                if !self.has_shift_overflow_check(node, source) {
                    let start_point = node.start_position();
                    let expr_text = &source[node.start_byte()..node.end_byte()];

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Unsigned integer left shift '{}' may cause overflow without checking",
                            expr_text
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Add shift overflow check before shifting".to_string()),
                    ..Default::default()
                    });
                }
            }
        }
    }

    fn check_compound_addition(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(left) = node.child_by_field_name("left") {
            let left_type = self.infer_type(&left, source);

            if self.is_unsigned_type(&left_type) {
                if !self.has_overflow_check_compound(node, source) {
                    let start_point = node.start_position();
                    let expr_text = &source[node.start_byte()..node.end_byte()];

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Unsigned integer compound addition '{}' may wrap without overflow checking",
                            expr_text
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Add overflow check before compound assignment".to_string()),
                    ..Default::default()
                    });
                }
            }
        }
    }

    fn check_compound_subtraction(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(left) = node.child_by_field_name("left") {
            let left_type = self.infer_type(&left, source);

            if self.is_unsigned_type(&left_type) {
                if !self.has_overflow_check_compound(node, source) {
                    let start_point = node.start_position();
                    let expr_text = &source[node.start_byte()..node.end_byte()];

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Unsigned integer compound subtraction '{}' may wrap without underflow checking",
                            expr_text
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Add underflow check before compound assignment".to_string()),
                    ..Default::default()
                    });
                }
            }
        }
    }

    fn check_compound_multiplication(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(left) = node.child_by_field_name("left") {
            let left_type = self.infer_type(&left, source);

            if self.is_unsigned_type(&left_type) {
                if !self.has_overflow_check_compound(node, source) {
                    let start_point = node.start_position();
                    let expr_text = &source[node.start_byte()..node.end_byte()];

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Unsigned integer compound multiplication '{}' may wrap without overflow checking",
                            expr_text
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Add overflow check before compound assignment".to_string()),
                    ..Default::default()
                    });
                }
            }
        }
    }

    fn check_compound_left_shift(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(left) = node.child_by_field_name("left") {
            let left_type = self.infer_type(&left, source);

            if self.is_unsigned_type(&left_type) {
                if !self.has_overflow_check_compound(node, source) {
                    let start_point = node.start_position();
                    let expr_text = &source[node.start_byte()..node.end_byte()];

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Unsigned integer compound left shift '{}' may cause overflow without checking",
                            expr_text
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Add shift overflow check before compound assignment".to_string()),
                    ..Default::default()
                    });
                }
            }
        }
    }

    fn check_increment_decrement(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(argument) = node.child_by_field_name("argument") {
            let arg_type = self.infer_type(&argument, source);

            if self.is_unsigned_type(&arg_type) {
                let operator = self.get_update_operator(node, source);
                if operator == "++" || operator == "--" {
                    if !self.has_overflow_check_update(node, source) {
                        let start_point = node.start_position();
                        let expr_text = &source[node.start_byte()..node.end_byte()];

                        let message = if operator == "++" {
                            format!("Unsigned integer increment '{}' may wrap at maximum value", expr_text)
                        } else {
                            format!("Unsigned integer decrement '{}' may wrap at zero", expr_text)
                        };

                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::Medium,
                            message,
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some("Add bounds checking before increment/decrement".to_string()),
                        ..Default::default()
                        });
                    }
                }
            }
        }
    }

    fn check_function_call(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(function_node) = node.child_by_field_name("function") {
            let function_name = &source[function_node.start_byte()..function_node.end_byte()];

            match function_name {
                "malloc" | "calloc" | "realloc" => {
                    self.check_allocation_overflow(node, source, function_name, violations);
                }
                _ => {}
            }
        }
    }

    fn check_allocation_overflow(&self, node: &Node, source: &str, function_name: &str, violations: &mut Vec<RuleViolation>) {
        let args = self.get_function_arguments(node, source);

        match function_name {
            "malloc" => {
                if !args.is_empty() && self.contains_multiplication(&args[0]) {
                    self.flag_allocation_overflow(node, source, function_name, &args[0], violations);
                }
            }
            "calloc" => {
                if args.len() >= 2 {
                    // calloc(count, size) - multiplication is implicit
                    if !self.has_calloc_overflow_check(node, source) {
                        let start_point = node.start_position();
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::High,
                            message: format!(
                                "calloc({}, {}) may cause integer overflow in size calculation",
                                args[0], args[1]
                            ),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some("Check for overflow: if (count > SIZE_MAX / size) { /* handle error */ }".to_string()),
                        ..Default::default()
                        });
                    }
                }
            }
            "realloc" => {
                if !args.is_empty() && self.contains_multiplication(&args[1]) {
                    self.flag_allocation_overflow(node, source, function_name, &args[1], violations);
                }
            }
            _ => {}
        }
    }

    fn flag_allocation_overflow(&self, node: &Node, source: &str, function_name: &str, size_arg: &str, violations: &mut Vec<RuleViolation>) {
        let start_point = node.start_position();
        violations.push(RuleViolation {
            rule_id: self.rule_id().to_string(),
            severity: Severity::High,
            message: format!(
                "{}() called with multiplication that may cause integer overflow: '{}'",
                function_name, size_arg
            ),
            file_path: String::new(),
            line: start_point.row + 1,
            column: start_point.column + 1,
            suggestion: Some("Add overflow check before allocation".to_string()),
        ..Default::default()
        });
    }

    fn infer_type(&self, node: &Node, source: &str) -> String {
        // Simple type inference based on patterns
        let text = &source[node.start_byte()..node.end_byte()];

        // Look for explicit unsigned indicators
        if text.contains("unsigned") || text.contains("size_t") || text.contains("uint") {
            return "unsigned".to_string();
        }

        // Look for unsigned literals
        if text.ends_with("u") || text.ends_with("U") {
            return "unsigned".to_string();
        }

        // Look for unsigned constants
        if text.contains("UINT_MAX") || text.contains("SIZE_MAX") {
            return "unsigned".to_string();
        }

        // Default assumption based on common patterns
        if text.chars().all(|c| c.is_ascii_digit()) {
            // Plain number - could be either, assume signed for conservatism
            return "int".to_string();
        }

        // Variable names with common unsigned patterns
        if text.starts_with("u") || text.contains("size") || text.contains("len") {
            return "unsigned".to_string();
        }

        "unknown".to_string()
    }

    fn is_unsigned_type(&self, type_str: &str) -> bool {
        type_str.contains("unsigned") || type_str == "size_t" || type_str.contains("uint")
    }

    fn has_overflow_check_addition(&self, node: &Node, source: &str) -> bool {
        // Look for UINT_MAX - a < b pattern in surrounding context
        self.has_surrounding_check(node, source, &["UINT_MAX", "SIZE_MAX", "- ", " < "])
    }

    fn has_overflow_check_subtraction(&self, node: &Node, source: &str) -> bool {
        // Look for a < b pattern in surrounding context
        self.has_surrounding_check(node, source, &[" < ", " >= "])
    }

    fn has_overflow_check_multiplication(&self, node: &Node, source: &str) -> bool {
        // Look for a > MAX / b pattern in surrounding context
        self.has_surrounding_check(node, source, &["UINT_MAX", "SIZE_MAX", " / ", " > "])
    }

    fn has_shift_overflow_check(&self, node: &Node, source: &str) -> bool {
        // Look for shift amount validation
        self.has_surrounding_check(node, source, &[" < ", "sizeof", "* 8"])
    }

    fn has_overflow_check_compound(&self, node: &Node, source: &str) -> bool {
        // Look for any overflow checking pattern
        self.has_surrounding_check(node, source, &["if", "UINT_MAX", "SIZE_MAX"])
    }

    fn has_overflow_check_update(&self, node: &Node, source: &str) -> bool {
        // Look for bounds checking around increment/decrement
        self.has_surrounding_check(node, source, &["if", "UINT_MAX", "== 0"])
    }

    fn has_calloc_overflow_check(&self, node: &Node, source: &str) -> bool {
        // Look for calloc-specific overflow checking
        self.has_surrounding_check(node, source, &["SIZE_MAX", " / ", " > "])
    }

    fn has_surrounding_check(&self, node: &Node, source: &str, patterns: &[&str]) -> bool {
        // Simple heuristic: look in parent contexts for overflow checking patterns
        if let Some(parent) = node.parent() {
            if let Some(grandparent) = parent.parent() {
                let context = &source[grandparent.start_byte()..grandparent.end_byte()];
                return patterns.iter().all(|pattern| context.contains(pattern));
            }
        }
        false
    }

    fn contains_multiplication(&self, expr: &str) -> bool {
        expr.contains('*') && !expr.contains("/*") && !expr.contains("*/")
    }

    fn get_operator(&self, node: &Node, source: &str) -> Option<String> {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                let text = &source[child.start_byte()..child.end_byte()];
                if matches!(text, "+" | "-" | "*" | "/" | "<<" | ">>") {
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
                if matches!(text, "+=" | "-=" | "*=" | "/=" | "<<=" | ">>=") {
                    return Some(text.to_string());
                }
            }
        }
        None
    }

    fn get_update_operator(&self, node: &Node, source: &str) -> String {
        let text = &source[node.start_byte()..node.end_byte()];
        if text.contains("++") {
            "++".to_string()
        } else if text.contains("--") {
            "--".to_string()
        } else {
            "unknown".to_string()
        }
    }

    fn get_function_arguments(&self, node: &Node, source: &str) -> Vec<String> {
        let mut args = Vec::new();

        if let Some(arguments) = node.child_by_field_name("arguments") {
            for i in 0..arguments.child_count() {
                if let Some(child) = arguments.child(i) {
                    if child.kind() != "," {
                        let arg_text = source[child.start_byte()..child.end_byte()].to_string();
                        args.push(arg_text.trim().to_string());
                    }
                }
            }
        }

        args
    }
}

// DEPRECATED: Inline tests moved to src/rules/cert_c/tests/inline/
// #[cfg(test)]
// #[path = "tests/int30_c.rs"]
// mod tests;
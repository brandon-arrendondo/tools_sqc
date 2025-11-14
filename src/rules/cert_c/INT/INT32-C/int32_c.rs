use super::super::{CertRule, RuleViolation};
use crate::manifest::{Severity, RuleCategory};
use tree_sitter::Node;

pub struct Int32C;

impl CertRule for Int32C {
    fn rule_id(&self) -> &'static str {
        "INT32-C"
    }

    fn description(&self) -> &'static str {
        "Ensure that operations on signed integers do not result in overflow"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "INT32-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Int32C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            "binary_expression" => {
                self.check_binary_operation(node, source, violations);
            }
            "assignment_expression" => {
                self.check_assignment_operation(node, source, violations);
            }
            "unary_expression" => {
                self.check_unary_operation(node, source, violations);
            }
            "update_expression" => {
                self.check_increment_decrement(node, source, violations);
            }
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

    fn check_binary_operation(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(operator) = self.get_operator(node, source) {
            match operator.as_str() {
                "+" => self.check_addition(node, source, violations),
                "-" => self.check_subtraction(node, source, violations),
                "*" => self.check_multiplication(node, source, violations),
                "/" => self.check_division(node, source, violations),
                "%" => self.check_modulo(node, source, violations),
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
                "/=" => self.check_compound_division(node, source, violations),
                "%=" => self.check_compound_modulo(node, source, violations),
                "<<=" => self.check_compound_left_shift(node, source, violations),
                _ => {}
            }
        }
    }

    fn check_unary_operation(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(operator) = self.get_unary_operator(node, source) {
            if operator == "-" {
                self.check_negation(node, source, violations);
            }
        }
    }

    fn check_addition(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let (Some(left), Some(right)) = (node.child_by_field_name("left"), node.child_by_field_name("right")) {
            let left_type = self.infer_type(&left, source);
            let right_type = self.infer_type(&right, source);

            if self.is_signed_type(&left_type) || self.is_signed_type(&right_type) {
                // Skip if this operation is part of an overflow check comparison
                if self.is_part_of_comparison(node, source) {
                    return;
                }

                if !self.has_overflow_check_addition(node, source) {
                    let start_point = node.start_position();
                    let expr_text = &source[node.start_byte()..node.end_byte()];

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Signed integer addition '{}' may overflow without proper checking",
                            expr_text
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Add overflow check: if ((b > 0 && a > INT_MAX - b) || (b < 0 && a < INT_MIN - b)) { /* handle error */ }".to_string()),
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

            if self.is_signed_type(&left_type) || self.is_signed_type(&right_type) {
                // Skip if this operation is part of an overflow check comparison
                if self.is_part_of_comparison(node, source) {
                    return;
                }

                // Skip if both operands are constants (compiler handles these)
                let left_text = &source[left.start_byte()..left.end_byte()];
                let right_text = &source[right.start_byte()..right.end_byte()];
                if self.is_constant_expression(left_text) && self.is_constant_expression(right_text) {
                    return; // Safe - constant expression
                }

                if !self.has_overflow_check_subtraction(node, source) {
                    let start_point = node.start_position();
                    let expr_text = &source[node.start_byte()..node.end_byte()];

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Signed integer subtraction '{}' may overflow without proper checking",
                            expr_text
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Add overflow check: if ((b < 0 && a > INT_MAX + b) || (b > 0 && a < INT_MIN + b)) { /* handle error */ }".to_string()),
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

            if self.is_signed_type(&left_type) || self.is_signed_type(&right_type) {
                // Skip if this operation is part of an overflow check comparison
                if self.is_part_of_comparison(node, source) {
                    return;
                }

                // Skip if using wider type (cast to long long before multiplication)
                let left_text = &source[left.start_byte()..left.end_byte()];
                let right_text = &source[right.start_byte()..right.end_byte()];
                if (left_text.contains("long long") || right_text.contains("long long")) ||
                   (left_text.starts_with("(signed long long)") || left_text.starts_with("(long long)") ||
                    right_text.starts_with("(signed long long)") || right_text.starts_with("(long long)")) {
                    return; // Safe - using wider type
                }

                if !self.has_overflow_check_multiplication(node, source) {
                    let start_point = node.start_position();
                    let expr_text = &source[node.start_byte()..node.end_byte()];

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Signed integer multiplication '{}' may overflow without proper checking",
                            expr_text
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Add overflow check using complex multiplication overflow detection".to_string()),
                    ..Default::default()
                    });
                }
            }
        }
    }

    fn check_division(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let (Some(left), Some(right)) = (node.child_by_field_name("left"), node.child_by_field_name("right")) {
            let left_text = &source[left.start_byte()..left.end_byte()];
            let right_text = &source[right.start_byte()..right.end_byte()];
            let left_type = self.infer_type(&left, source);
            let right_type = self.infer_type(&right, source);

            // Check for signed integer division
            // INT_MIN / -1 causes overflow because -INT_MIN cannot be represented
            if self.is_signed_type(&left_type) || self.is_signed_type(&right_type) {
                // Skip if this division is part of an overflow check comparison
                if self.is_part_of_comparison(node, source) {
                    return;
                }

                // Check if there's explicit INT_MIN/-1 pattern OR generic signed division without checks
                let has_explicit_risk = right_text.trim() == "-1" || right_text.contains("-1") ||
                                       left_text.contains("INT_MIN") || left_text.contains("LONG_MIN") ||
                                       self.could_be_int_min(&left, source);

                // Also flag generic signed division of variables (could be INT_MIN / -1 at runtime)
                let is_variable_division = left.kind() == "identifier" && right.kind() == "identifier";

                if (has_explicit_risk || is_variable_division) && !self.has_division_overflow_check(node, source) {
                    let start_point = node.start_position();
                    let expr_text = &source[node.start_byte()..node.end_byte()];

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Signed integer division '{}' may overflow (INT_MIN / -1)",
                            expr_text
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Add check: if (dividend == INT_MIN && divisor == -1) { /* handle error */ }".to_string()),
                    ..Default::default()
                    });
                }
            }
        }
    }

    fn check_modulo(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let (Some(left), Some(right)) = (node.child_by_field_name("left"), node.child_by_field_name("right")) {
            let left_text = &source[left.start_byte()..left.end_byte()];
            let right_text = &source[right.start_byte()..right.end_byte()];
            let left_type = self.infer_type(&left, source);
            let right_type = self.infer_type(&right, source);

            // Check for signed integer modulo
            // INT_MIN % -1 causes overflow
            if self.is_signed_type(&left_type) || self.is_signed_type(&right_type) {
                let has_explicit_risk = (left_text.contains("INT_MIN") || left_text.contains("LONG_MIN") ||
                                        self.could_be_int_min(&left, source)) &&
                                       (right_text == "-1" || right_text.contains("-1"));

                // Also flag generic signed modulo of variables (could be INT_MIN % -1 at runtime)
                let is_variable_modulo = left.kind() == "identifier" && right.kind() == "identifier";

                if (has_explicit_risk || is_variable_modulo) && !self.has_modulo_overflow_check(node, source) {
                    let start_point = node.start_position();
                    let expr_text = &source[node.start_byte()..node.end_byte()];

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Signed integer modulo '{}' may overflow (INT_MIN % -1)",
                            expr_text
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Add check: if (dividend == INT_MIN && divisor == -1) { /* handle error */ }".to_string()),
                    ..Default::default()
                    });
                }
            }
        }
    }

    fn check_negation(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(argument) = node.child_by_field_name("argument") {
            let _arg_text = &source[argument.start_byte()..argument.end_byte()];
            let arg_type = self.infer_type(&argument, source);

            // Check for negation of signed integers, especially -INT_MIN which causes overflow
            if self.is_signed_type(&arg_type) {
                if !self.has_negation_overflow_check(node, source) {
                    let start_point = node.start_position();
                    let expr_text = &source[node.start_byte()..node.end_byte()];

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Signed integer negation '{}' may overflow (-INT_MIN)",
                            expr_text
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Add check: if (value == INT_MIN) { /* handle error */ }".to_string()),
                    ..Default::default()
                    });
                }
            }
        }
    }

    fn check_left_shift(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let (Some(left), Some(_right)) = (node.child_by_field_name("left"), node.child_by_field_name("right")) {
            let left_type = self.infer_type(&left, source);

            if self.is_signed_type(&left_type) {
                if !self.has_shift_overflow_check(node, source) {
                    let start_point = node.start_position();
                    let expr_text = &source[node.start_byte()..node.end_byte()];

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Signed integer left shift '{}' may overflow or exhibit undefined behavior",
                            expr_text
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Validate shift amount and check for overflow before shifting".to_string()),
                    ..Default::default()
                    });
                }
            }
        }
    }

    fn check_compound_addition(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(left) = node.child_by_field_name("left") {
            let left_type = self.infer_type(&left, source);

            if self.is_signed_type(&left_type) {
                if !self.has_overflow_check_compound(node, source) {
                    let start_point = node.start_position();
                    let expr_text = &source[node.start_byte()..node.end_byte()];

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Signed integer compound addition '{}' may overflow without checking",
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

            if self.is_signed_type(&left_type) {
                if !self.has_overflow_check_compound(node, source) {
                    let start_point = node.start_position();
                    let expr_text = &source[node.start_byte()..node.end_byte()];

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Signed integer compound subtraction '{}' may overflow without checking",
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

    fn check_compound_multiplication(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(left) = node.child_by_field_name("left") {
            let left_type = self.infer_type(&left, source);

            if self.is_signed_type(&left_type) {
                if !self.has_overflow_check_compound(node, source) {
                    let start_point = node.start_position();
                    let expr_text = &source[node.start_byte()..node.end_byte()];

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Signed integer compound multiplication '{}' may overflow without checking",
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

    fn check_compound_division(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let (Some(left), Some(right)) = (node.child_by_field_name("left"), node.child_by_field_name("right")) {
            let left_text = &source[left.start_byte()..left.end_byte()];
            let right_text = &source[right.start_byte()..right.end_byte()];

            if (left_text.contains("INT_MIN") || self.could_be_int_min(&left, source)) &&
               (right_text == "-1" || right_text.contains("-1")) {
                if !self.has_overflow_check_compound(node, source) {
                    let start_point = node.start_position();
                    let expr_text = &source[node.start_byte()..node.end_byte()];

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Signed integer compound division '{}' may overflow (INT_MIN /= -1)",
                            expr_text
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Add check before assignment: if (left == INT_MIN && right == -1) { /* handle error */ }".to_string()),
                    ..Default::default()
                    });
                }
            }
        }
    }

    fn check_compound_modulo(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let (Some(left), Some(right)) = (node.child_by_field_name("left"), node.child_by_field_name("right")) {
            let left_text = &source[left.start_byte()..left.end_byte()];
            let right_text = &source[right.start_byte()..right.end_byte()];

            if (left_text.contains("INT_MIN") || self.could_be_int_min(&left, source)) &&
               (right_text == "-1" || right_text.contains("-1")) {
                if !self.has_overflow_check_compound(node, source) {
                    let start_point = node.start_position();
                    let expr_text = &source[node.start_byte()..node.end_byte()];

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Signed integer compound modulo '{}' may overflow (INT_MIN %= -1)",
                            expr_text
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Add check before assignment: if (left == INT_MIN && right == -1) { /* handle error */ }".to_string()),
                    ..Default::default()
                    });
                }
            }
        }
    }

    fn check_compound_left_shift(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(left) = node.child_by_field_name("left") {
            let left_type = self.infer_type(&left, source);

            if self.is_signed_type(&left_type) {
                if !self.has_overflow_check_compound(node, source) {
                    let start_point = node.start_position();
                    let expr_text = &source[node.start_byte()..node.end_byte()];

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Signed integer compound left shift '{}' may overflow or exhibit undefined behavior",
                            expr_text
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Validate shift amount and check for overflow before assignment".to_string()),
                    ..Default::default()
                    });
                }
            }
        }
    }

    fn check_increment_decrement(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(argument) = node.child_by_field_name("argument") {
            let arg_type = self.infer_type(&argument, source);

            if self.is_signed_type(&arg_type) {
                // Skip if this is part of a safe for loop (bounded, starting from small values)
                if self.is_in_safe_for_loop(node, source) {
                    return;
                }

                let operator = self.get_update_operator(node, source);
                if operator == "++" || operator == "--" {
                    if !self.has_overflow_check_update(node, source) {
                        let start_point = node.start_position();
                        let expr_text = &source[node.start_byte()..node.end_byte()];

                        let message = if operator == "++" {
                            format!("Signed integer increment '{}' may overflow at INT_MAX", expr_text)
                        } else {
                            format!("Signed integer decrement '{}' may overflow at INT_MIN", expr_text)
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

            // Check for functions that commonly receive arithmetic expressions that might overflow
            match function_name {
                "malloc" | "calloc" | "realloc" => {
                    self.check_allocation_overflow(node, source, function_name, violations);
                }
                "memcpy" | "memmove" | "memset" => {
                    self.check_memory_function_overflow(node, source, function_name, violations);
                }
                "abs" | "labs" | "llabs" => {
                    self.check_abs_overflow(node, source, function_name, violations);
                }
                _ => {}
            }
        }
    }

    fn check_allocation_overflow(&self, node: &Node, source: &str, function_name: &str, violations: &mut Vec<RuleViolation>) {
        let args = self.get_function_arguments(node, source);

        for (i, arg) in args.iter().enumerate() {
            if self.contains_arithmetic(arg) && !self.has_allocation_overflow_check(node, source) {
                let start_point = node.start_position();
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::High,
                    message: format!(
                        "{}() argument {} contains arithmetic that may overflow: '{}'",
                        function_name, i + 1, arg
                    ),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some("Validate arithmetic operations before passing to allocation functions".to_string()),
                ..Default::default()
                });
            }
        }
    }

    fn check_memory_function_overflow(&self, node: &Node, source: &str, function_name: &str, violations: &mut Vec<RuleViolation>) {
        let args = self.get_function_arguments(node, source);

        // Check size arguments for arithmetic that might overflow
        let size_arg_indices = match function_name {
            "memcpy" | "memmove" => vec![2], // Third argument is size
            "memset" => vec![2],             // Third argument is size
            _ => vec![],
        };

        for &idx in &size_arg_indices {
            if let Some(arg) = args.get(idx) {
                if self.contains_arithmetic(arg) && !self.has_memory_function_overflow_check(node, source) {
                    let start_point = node.start_position();
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "{}() size argument contains arithmetic that may overflow: '{}'",
                            function_name, arg
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Validate size calculations before passing to memory functions".to_string()),
                    ..Default::default()
                    });
                }
            }
        }
    }

    fn check_abs_overflow(&self, node: &Node, source: &str, function_name: &str, violations: &mut Vec<RuleViolation>) {
        // abs(INT_MIN), labs(LONG_MIN), llabs(LLONG_MIN) all cause overflow
        // because the absolute value of the minimum signed integer cannot be represented
        if !self.has_abs_overflow_check(node, source) {
            let start_point = node.start_position();
            let expr_text = &source[node.start_byte()..node.end_byte()];

            violations.push(RuleViolation {
                rule_id: self.rule_id().to_string(),
                severity: Severity::High,
                message: format!(
                    "{}() may overflow when called with most negative value (e.g., INT_MIN): '{}'",
                    function_name, expr_text
                ),
                file_path: String::new(),
                line: start_point.row + 1,
                column: start_point.column + 1,
                suggestion: Some("Check if argument equals INT_MIN/LONG_MIN/LLONG_MIN before calling abs/labs/llabs".to_string()),
            ..Default::default()
            });
        }
    }

    fn infer_type(&self, node: &Node, source: &str) -> String {
        let text = &source[node.start_byte()..node.end_byte()];

        // Look for explicit unsigned type indicators first
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

        // Look for explicit signed type indicators
        if text.contains("signed") || text.contains("int") || text.contains("short") || text.contains("long") {
            return "signed".to_string();
        }

        // Look for signed integer constants
        if text.contains("INT_MAX") || text.contains("INT_MIN") {
            return "signed".to_string();
        }

        // Plain numbers without unsigned suffix are typically signed
        if text.chars().all(|c| c.is_ascii_digit() || c == '-') {
            return "signed".to_string();
        }

        // If this is just a variable name, look for it in function parameters or declarations
        if text.chars().all(|c| c.is_ascii_alphabetic() || c == '_') {
            if let Some(declared_type) = self.find_variable_declaration(node, source, text) {
                return declared_type;
            }
        }

        // Variable names that suggest unsigned integers
        if text.starts_with("u") || text.contains("size") || text.contains("len") {
            return "unsigned".to_string();
        }

        // Variable names that suggest signed integers
        if text.starts_with("i") || text.contains("signed") || text.contains("count") || text.contains("index") {
            return "signed".to_string();
        }

        // For simple variable names, default to signed since "int" is signed by default in C
        "signed".to_string()
    }

    fn find_variable_declaration(&self, node: &Node, source: &str, var_name: &str) -> Option<String> {
        // Look for the function that contains this node
        let mut current = node.parent();
        while let Some(parent) = current {
            if parent.kind() == "function_definition" {
                // Look in function parameters
                if let Some(params) = parent.child_by_field_name("parameters") {
                    let params_text = &source[params.start_byte()..params.end_byte()];
                    if params_text.contains("unsigned") && params_text.contains(var_name) {
                        return Some("unsigned".to_string());
                    }
                    if params_text.contains("signed") || params_text.contains("int") {
                        if params_text.contains(var_name) && !params_text.contains("unsigned") {
                            return Some("signed".to_string());
                        }
                    }
                }
                break;
            }
            current = parent.parent();
        }

        // Look in local declarations (simplified)
        current = node.parent();
        while let Some(parent) = current {
            if parent.kind() == "declaration" {
                let decl_text = &source[parent.start_byte()..parent.end_byte()];
                if decl_text.contains(var_name) {
                    if decl_text.contains("unsigned") {
                        return Some("unsigned".to_string());
                    }
                    if decl_text.contains("signed") || decl_text.contains("int") {
                        return Some("signed".to_string());
                    }
                }
            }
            current = parent.parent();
        }

        None
    }

    fn is_signed_type(&self, type_str: &str) -> bool {
        type_str == "signed" || type_str == "int" ||
        (type_str != "unsigned" && type_str != "size_t" && !type_str.contains("uint"))
    }

    fn could_be_int_min(&self, node: &Node, source: &str) -> bool {
        let text = &source[node.start_byte()..node.end_byte()];
        text.contains("INT_MIN") ||
        (text.starts_with("min") && (text.contains("val") || text.contains("num")))
    }

    fn contains_arithmetic(&self, expr: &str) -> bool {
        expr.contains('+') || expr.contains('-') || expr.contains('*') || expr.contains('/')
    }

    fn is_constant_expression(&self, expr: &str) -> bool {
        // Check if expression is a constant (literal number or named constant like INT_MAX)
        let trimmed = expr.trim();

        // Numeric literals
        if trimmed.chars().all(|c| c.is_ascii_digit() || c == '-' || c == '+') {
            return true;
        }

        // Named constants
        if trimmed.contains("INT_MAX") || trimmed.contains("INT_MIN") ||
           trimmed.contains("LONG_MAX") || trimmed.contains("LONG_MIN") ||
           trimmed.contains("LLONG_MAX") || trimmed.contains("LLONG_MIN") ||
           trimmed.contains("UINT_MAX") || trimmed.contains("SIZE_MAX") {
            return true;
        }

        false
    }

    fn is_in_safe_for_loop(&self, node: &Node, source: &str) -> bool {
        // Walk up the tree to see if this node is in a for_statement
        // Only consider it safe if the loop has clear small bounds
        let mut current = node.parent();
        while let Some(parent) = current {
            if parent.kind() == "for_statement" {
                // Check if this is a typical safe for loop (starting from 0 or small value)
                // by looking at the initializer
                if let Some(initializer) = parent.child_by_field_name("initializer") {
                    // Convert initializer to text and check if it's safe
                    // Safe patterns: i = 0, i = 1, etc. (small constants)
                    // Unsafe patterns: i = INT_MAX - 2, etc.
                    let init_text = &source[initializer.start_byte()..initializer.end_byte()];
                    if init_text.contains("INT_MAX") || init_text.contains("LONG_MAX") ||
                       init_text.contains("INT_MIN") || init_text.contains("LONG_MIN") {
                        return false; // Unsafe - loop starts near limits
                    }
                    return true; // Safe - typical for loop
                }
            }
            // Stop at function boundary
            if parent.kind() == "function_definition" {
                break;
            }
            current = parent.parent();
        }
        false
    }

    fn has_overflow_check_addition(&self, node: &Node, source: &str) -> bool {
        // First check the immediate surrounding context (parent/grandparent)
        if self.has_surrounding_check(node, source, &["INT_MAX", "INT_MIN", " - ", " > ", " < "]) {
            return true;
        }

        // Then check the broader function context for overflow checks
        self.has_function_level_overflow_check(node, source, &["INT_MAX", "INT_MIN", " - ", " > ", " < "])
    }

    fn has_overflow_check_subtraction(&self, node: &Node, source: &str) -> bool {
        // First check the immediate surrounding context (parent/grandparent)
        if self.has_surrounding_check(node, source, &["INT_MAX", "INT_MIN", " + ", " > ", " < "]) {
            return true;
        }

        // Then check the broader function context for overflow checks
        self.has_function_level_overflow_check(node, source, &["INT_MAX", "INT_MIN", " + ", " > ", " < "])
    }

    fn has_overflow_check_multiplication(&self, node: &Node, source: &str) -> bool {
        // Check for multiplication overflow patterns:
        // 1. if (a > INT_MAX / b) - division-based check
        // 2. Complex checks with INT_MAX/INT_MIN and division
        if self.has_surrounding_check(node, source, &["INT_MAX", " / "]) ||
           self.has_surrounding_check(node, source, &["INT_MIN", " / "]) ||
           self.has_surrounding_check(node, source, &["LONG_MAX", " / "]) ||
           self.has_surrounding_check(node, source, &["LONG_MIN", " / "]) {
            return true;
        }

        // Function-level checks
        if self.has_function_level_patterns_any(node, source, &["INT_MAX", " / "]) ||
           self.has_function_level_patterns_any(node, source, &["INT_MIN", " / "]) ||
           self.has_function_level_patterns_any(node, source, &["LONG_MAX", " / "]) ||
           self.has_function_level_patterns_any(node, source, &["LONG_MIN", " / "]) {
            return true;
        }

        false
    }

    fn has_division_overflow_check(&self, node: &Node, source: &str) -> bool {
        // Check for INT_MIN/-1 or LONG_MIN/-1 or LLONG_MIN/-1 division overflow checks
        if self.has_surrounding_check(node, source, &["INT_MIN", " == ", " -1"]) ||
           self.has_surrounding_check(node, source, &["LONG_MIN", " == ", " -1"]) ||
           self.has_surrounding_check(node, source, &["LLONG_MIN", " == ", " -1"]) {
            return true;
        }

        self.has_function_level_patterns_any(node, source, &["INT_MIN", " == ", " -1"]) ||
        self.has_function_level_patterns_any(node, source, &["LONG_MIN", " == ", " -1"]) ||
        self.has_function_level_patterns_any(node, source, &["LLONG_MIN", " == ", " -1"])
    }

    fn has_modulo_overflow_check(&self, node: &Node, source: &str) -> bool {
        // Check for INT_MIN%- 1 or LONG_MIN%-1 or LLONG_MIN%-1 modulo overflow checks
        if self.has_surrounding_check(node, source, &["INT_MIN", " == ", " -1"]) ||
           self.has_surrounding_check(node, source, &["LONG_MIN", " == ", " -1"]) ||
           self.has_surrounding_check(node, source, &["LLONG_MIN", " == ", " -1"]) {
            return true;
        }

        self.has_function_level_patterns_any(node, source, &["INT_MIN", " == ", " -1"]) ||
        self.has_function_level_patterns_any(node, source, &["LONG_MIN", " == ", " -1"]) ||
        self.has_function_level_patterns_any(node, source, &["LLONG_MIN", " == ", " -1"])
    }

    fn has_negation_overflow_check(&self, node: &Node, source: &str) -> bool {
        // Check for negation of INT_MIN, LONG_MIN, or LLONG_MIN
        if self.has_surrounding_check(node, source, &["INT_MIN", " == "]) ||
           self.has_surrounding_check(node, source, &["LONG_MIN", " == "]) ||
           self.has_surrounding_check(node, source, &["LLONG_MIN", " == "]) {
            return true;
        }

        self.has_function_level_patterns_any(node, source, &["INT_MIN", " == "]) ||
        self.has_function_level_patterns_any(node, source, &["LONG_MIN", " == "]) ||
        self.has_function_level_patterns_any(node, source, &["LLONG_MIN", " == "])
    }

    fn has_shift_overflow_check(&self, node: &Node, source: &str) -> bool {
        // Check for left shift overflow patterns:
        // Complete check requires BOTH:
        // 1. Shift amount validation AND value range check
        // 2. Value range check: a > (INT_MAX >> b) or similar with LONG_MAX

        // Check for value range pattern (most comprehensive)
        if self.has_surrounding_check(node, source, &["LONG_MAX", " >> "]) ||
           self.has_surrounding_check(node, source, &["INT_MAX", " >> "]) {
            return true;
        }

        if self.has_function_level_patterns_any(node, source, &["LONG_MAX", " >> "]) ||
           self.has_function_level_patterns_any(node, source, &["INT_MAX", " >> "]) {
            return true;
        }

        // Only accept PRECISION if it's combined with value range checks
        // (PRECISION alone is insufficient - see wiki_noncompliant_6)
        if (self.has_surrounding_check(node, source, &["PRECISION"]) ||
            self.has_function_level_patterns_any(node, source, &["PRECISION"])) &&
           (self.has_function_level_patterns_any(node, source, &[" >> "]) ||
            self.has_function_level_patterns_any(node, source, &[" < ", "sizeof"])) {
            return false; // PRECISION + shift amount check only is insufficient
        }

        // sizeof-based complete checks
        self.has_surrounding_check(node, source, &[" < ", "sizeof", "* 8"])
    }

    fn has_overflow_check_compound(&self, node: &Node, source: &str) -> bool {
        if self.has_surrounding_check(node, source, &["if", "INT_MAX", "INT_MIN"]) {
            return true;
        }
        self.has_function_level_overflow_check(node, source, &["if", "INT_MAX", "INT_MIN"])
    }

    fn has_overflow_check_update(&self, node: &Node, source: &str) -> bool {
        // For increment: check if value == INT_MAX
        // For decrement: check if value == INT_MIN
        // We need to detect EITHER check, not both
        if self.has_surrounding_check(node, source, &["if", "INT_MAX", " == "]) ||
           self.has_surrounding_check(node, source, &["if", "INT_MIN", " == "]) {
            return true;
        }

        self.has_function_level_patterns_any(node, source, &["if", "INT_MAX", " == "]) ||
        self.has_function_level_patterns_any(node, source, &["if", "INT_MIN", " == "])
    }

    fn has_allocation_overflow_check(&self, node: &Node, source: &str) -> bool {
        self.has_surrounding_check(node, source, &["SIZE_MAX", " / ", " > ", "if"])
    }

    fn has_memory_function_overflow_check(&self, node: &Node, source: &str) -> bool {
        self.has_surrounding_check(node, source, &["SIZE_MAX", " > ", "if"])
    }

    fn has_abs_overflow_check(&self, node: &Node, source: &str) -> bool {
        // Check if there's a check for INT_MIN/LONG_MIN/LLONG_MIN before the abs call
        if self.has_surrounding_check(node, source, &["INT_MIN", "if"]) ||
           self.has_surrounding_check(node, source, &["LONG_MIN", "if"]) ||
           self.has_surrounding_check(node, source, &["LLONG_MIN", "if"]) {
            return true;
        }
        self.has_function_level_overflow_check(node, source, &["INT_MIN", "if"]) ||
        self.has_function_level_overflow_check(node, source, &["LONG_MIN", "if"]) ||
        self.has_function_level_overflow_check(node, source, &["LLONG_MIN", "if"])
    }

    /// Check if a binary operation is part of a comparison expression (used in overflow checking)
    fn is_part_of_comparison(&self, node: &Node, source: &str) -> bool {
        let mut current = node.parent();

        while let Some(parent) = current {
            // If we find a binary_expression parent, check if it's a comparison operator
            if parent.kind() == "binary_expression" {
                // Get the operator
                for i in 0..parent.child_count() {
                    if let Some(child) = parent.child(i) {
                        let text = &source[child.start_byte()..child.end_byte()];
                        // Check if this is a comparison operator
                        if matches!(text, ">" | "<" | ">=" | "<=" | "==" | "!=") {
                            return true;
                        }
                    }
                }
            }

            // Stop at statement boundaries to avoid going too far up the tree
            if matches!(parent.kind(),
                "expression_statement" | "return_statement" | "declaration" |
                "function_definition" | "compound_statement") {
                break;
            }

            current = parent.parent();
        }

        false
    }

    /// Check if the function containing this node has overflow checking code (all patterns must match)
    fn has_function_level_overflow_check(&self, node: &Node, source: &str, patterns: &[&str]) -> bool {
        // Find the containing function
        let mut current = node.parent();
        while let Some(parent) = current {
            if parent.kind() == "function_definition" {
                // Search the entire function body for the patterns
                let func_text = &source[parent.start_byte()..parent.end_byte()];
                return patterns.iter().all(|pattern| func_text.contains(pattern));
            }
            current = parent.parent();
        }
        false
    }

    /// Check if the function containing this node has overflow checking code (any pattern can match)
    fn has_function_level_patterns_any(&self, node: &Node, source: &str, patterns: &[&str]) -> bool {
        // Find the containing function
        let mut current = node.parent();
        while let Some(parent) = current {
            if parent.kind() == "function_definition" {
                // Search the entire function body for the patterns
                let func_text = &source[parent.start_byte()..parent.end_byte()];
                return patterns.iter().all(|pattern| func_text.contains(pattern));
            }
            current = parent.parent();
        }
        false
    }

    fn has_surrounding_check(&self, node: &Node, source: &str, patterns: &[&str]) -> bool {
        if let Some(parent) = node.parent() {
            if let Some(grandparent) = parent.parent() {
                let context = &source[grandparent.start_byte()..grandparent.end_byte()];
                return patterns.iter().all(|pattern| context.contains(pattern));
            }
        }
        false
    }

    fn get_operator(&self, node: &Node, source: &str) -> Option<String> {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                let text = &source[child.start_byte()..child.end_byte()];
                if matches!(text, "+" | "-" | "*" | "/" | "%" | "<<" | ">>") {
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
                if matches!(text, "+=" | "-=" | "*=" | "/=" | "%=" | "<<=" | ">>=") {
                    return Some(text.to_string());
                }
            }
        }
        None
    }

    fn get_unary_operator(&self, node: &Node, source: &str) -> Option<String> {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                let text = &source[child.start_byte()..child.end_byte()];
                if matches!(text, "-" | "+" | "!" | "~") {
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
// #[path = "tests/int32_c.rs"]
// mod tests;

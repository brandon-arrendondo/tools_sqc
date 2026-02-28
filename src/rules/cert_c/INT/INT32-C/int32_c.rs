use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use std::collections::HashMap;
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
        let type_map = self.collect_variable_types(node, source);
        self.check_node(node, source, &mut violations, &type_map);
        violations
    }
}

impl Int32C {
    fn check_node(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        type_map: &HashMap<String, String>,
    ) {
        // Skip nodes inside compile-time contexts (cannot overflow at runtime)
        if self.is_in_compile_time_context(node) {
            return;
        }

        match node.kind() {
            "binary_expression" => {
                self.check_binary_operation(node, source, violations, type_map);
            }
            "assignment_expression" => {
                self.check_assignment_operation(node, source, violations, type_map);
            }
            "unary_expression" => {
                self.check_unary_operation(node, source, violations, type_map);
            }
            "update_expression" => {
                self.check_increment_decrement(node, source, violations, type_map);
            }
            "call_expression" => {
                self.check_function_call(node, source, violations);
            }
            _ => {}
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations, type_map);
            }
        }
    }

    /// Check if this node is inside a compile-time context where overflow cannot occur at runtime.
    /// Covers: sizeof(), _Static_assert, enum value definitions, array size declarators.
    fn is_in_compile_time_context(&self, node: &Node) -> bool {
        let mut current = node.parent();
        while let Some(parent) = current {
            match parent.kind() {
                "sizeof_expression" => return true,
                "static_assert_declaration" => return true,
                "enumerator" => return true,
                "array_declarator" => {
                    // Array size expressions like int arr[N + 1] are compile-time
                    return true;
                }
                "function_definition" => break,
                _ => {}
            }
            current = parent.parent();
        }
        false
    }

    fn check_binary_operation(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        type_map: &HashMap<String, String>,
    ) {
        if let Some(operator) = self.get_operator(node, source) {
            match operator.as_str() {
                "+" => self.check_addition(node, source, violations, type_map),
                "-" => self.check_subtraction(node, source, violations, type_map),
                "*" => self.check_multiplication(node, source, violations, type_map),
                "/" => self.check_division(node, source, violations, type_map),
                "%" => self.check_modulo(node, source, violations, type_map),
                "<<" => self.check_left_shift(node, source, violations, type_map),
                _ => {}
            }
        }
    }

    fn check_assignment_operation(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        type_map: &HashMap<String, String>,
    ) {
        if let Some(operator) = self.get_assignment_operator(node, source) {
            match operator.as_str() {
                "+=" => self.check_compound_addition(node, source, violations, type_map),
                "-=" => self.check_compound_subtraction(node, source, violations, type_map),
                "*=" => self.check_compound_multiplication(node, source, violations, type_map),
                "/=" => self.check_compound_division(node, source, violations),
                "%=" => self.check_compound_modulo(node, source, violations),
                "<<=" => self.check_compound_left_shift(node, source, violations, type_map),
                _ => {}
            }
        }
    }

    fn check_unary_operation(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        type_map: &HashMap<String, String>,
    ) {
        if let Some(operator) = self.get_unary_operator(node, source) {
            if operator == "-" {
                self.check_negation(node, source, violations, type_map);
            }
        }
    }

    fn check_addition(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        type_map: &HashMap<String, String>,
    ) {
        if let (Some(left), Some(right)) = (
            node.child_by_field_name("left"),
            node.child_by_field_name("right"),
        ) {
            let left_type = self.infer_type(&left, source, type_map);
            let right_type = self.infer_type(&right, source, type_map);

            // Skip if any operand is a non-integer type (char, float, pointer, etc.)
            if left_type == "not_applicable" || right_type == "not_applicable" {
                return;
            }

            // Skip if either operand is unsigned — unsigned wrap is INT30-C, not INT32-C
            if left_type == "unsigned" || right_type == "unsigned" {
                return;
            }

            if self.is_signed_type(&left_type) || self.is_signed_type(&right_type) {
                // Skip if this operation is part of an overflow check comparison
                if self.is_part_of_comparison(node, source) {
                    return;
                }

                // Skip if using wider type (cast to long long before addition)
                let left_text = get_node_text(&left, source);
                let right_text = get_node_text(&right, source);
                if self.has_wider_cast(left_text, right_text) {
                    return;
                }

                // Skip if inside a block guarded by a type-limit bounds check
                let op_names = self.extract_operand_names(node, source);
                if self.is_inside_bounds_checked_block(node, source, &op_names) {
                    return;
                }

                if !self.has_overflow_check_addition(node, source) {
                    let start_point = node.start_position();
                    let expr_text = get_node_text(node, source);

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

    fn check_subtraction(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        type_map: &HashMap<String, String>,
    ) {
        if let (Some(left), Some(right)) = (
            node.child_by_field_name("left"),
            node.child_by_field_name("right"),
        ) {
            let left_type = self.infer_type(&left, source, type_map);
            let right_type = self.infer_type(&right, source, type_map);

            // Skip if any operand is a non-integer type (char, float, pointer, etc.)
            if left_type == "not_applicable" || right_type == "not_applicable" {
                return;
            }

            // Skip if either operand is unsigned — unsigned wrap is INT30-C, not INT32-C
            if left_type == "unsigned" || right_type == "unsigned" {
                return;
            }

            if self.is_signed_type(&left_type) || self.is_signed_type(&right_type) {
                // Skip if this operation is part of an overflow check comparison
                if self.is_part_of_comparison(node, source) {
                    return;
                }

                // Skip if both operands are constants (compiler handles these)
                let left_text = get_node_text(&left, source);
                let right_text = get_node_text(&right, source);
                if self.is_constant_expression(left_text) && self.is_constant_expression(right_text)
                {
                    return; // Safe - constant expression
                }

                // Skip if using wider type (cast to long long before subtraction)
                if self.has_wider_cast(left_text, right_text) {
                    return;
                }

                // Skip if inside a block guarded by a type-limit bounds check
                let op_names = self.extract_operand_names(node, source);
                if self.is_inside_bounds_checked_block(node, source, &op_names) {
                    return;
                }

                if !self.has_overflow_check_subtraction(node, source) {
                    let start_point = node.start_position();
                    let expr_text = get_node_text(node, source);

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

    fn check_multiplication(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        type_map: &HashMap<String, String>,
    ) {
        if let (Some(left), Some(right)) = (
            node.child_by_field_name("left"),
            node.child_by_field_name("right"),
        ) {
            let left_type = self.infer_type(&left, source, type_map);
            let right_type = self.infer_type(&right, source, type_map);

            // Skip if any operand is a non-integer type (char, float, pointer, etc.)
            if left_type == "not_applicable" || right_type == "not_applicable" {
                return;
            }

            // Skip if either operand is unsigned — unsigned wrap is INT30-C, not INT32-C
            if left_type == "unsigned" || right_type == "unsigned" {
                return;
            }

            if self.is_signed_type(&left_type) || self.is_signed_type(&right_type) {
                // Skip if this operation is part of an overflow check comparison
                if self.is_part_of_comparison(node, source) {
                    return;
                }

                // Skip if using wider type (cast to long long before multiplication)
                let left_text = get_node_text(&left, source);
                let right_text = get_node_text(&right, source);
                if self.has_wider_cast(left_text, right_text) {
                    return; // Safe - using wider type
                }

                // Skip if operands are bounded for-loop variables
                if self.is_in_bounded_for_loop(node, source) {
                    return;
                }

                // Skip if inside a block guarded by a type-limit bounds check
                let op_names = self.extract_operand_names(node, source);
                if self.is_inside_bounds_checked_block(node, source, &op_names) {
                    return;
                }

                if !self.has_overflow_check_multiplication(node, source) {
                    let start_point = node.start_position();
                    let expr_text = get_node_text(node, source);

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

    fn check_division(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        type_map: &HashMap<String, String>,
    ) {
        if let (Some(left), Some(right)) = (
            node.child_by_field_name("left"),
            node.child_by_field_name("right"),
        ) {
            let left_text = get_node_text(&left, source);
            let right_text = get_node_text(&right, source);
            let left_type = self.infer_type(&left, source, type_map);
            let right_type = self.infer_type(&right, source, type_map);

            // Skip if any operand is a non-integer type (char, float, pointer, etc.)
            if left_type == "not_applicable" || right_type == "not_applicable" {
                return;
            }

            // Skip if either operand is unsigned — unsigned wrap is INT30-C, not INT32-C
            if left_type == "unsigned" || right_type == "unsigned" {
                return;
            }

            // Check for signed integer division
            // INT_MIN / -1 causes overflow because -INT_MIN cannot be represented
            if self.is_signed_type(&left_type) || self.is_signed_type(&right_type) {
                // Skip if this division is part of an overflow check comparison
                if self.is_part_of_comparison(node, source) {
                    return;
                }

                // Check if there's explicit INT_MIN/-1 pattern OR generic signed division without checks
                let has_explicit_risk = right_text.trim() == "-1"
                    || right_text.contains("-1")
                    || left_text.contains("INT_MIN")
                    || left_text.contains("LONG_MIN")
                    || self.could_be_int_min(&left, source);

                // Also flag generic signed division of variables (could be INT_MIN / -1 at runtime)
                // but skip if the right operand (divisor) is unsigned — can't be -1
                let is_variable_division = left.kind() == "identifier"
                    && right.kind() == "identifier"
                    && !self.is_unsigned_type(&right_type);

                if (has_explicit_risk || is_variable_division)
                    && !self.has_division_overflow_check(node, source)
                {
                    let start_point = node.start_position();
                    let expr_text = get_node_text(node, source);

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

    fn check_modulo(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        type_map: &HashMap<String, String>,
    ) {
        if let (Some(left), Some(right)) = (
            node.child_by_field_name("left"),
            node.child_by_field_name("right"),
        ) {
            let left_text = get_node_text(&left, source);
            let right_text = get_node_text(&right, source);
            let left_type = self.infer_type(&left, source, type_map);
            let right_type = self.infer_type(&right, source, type_map);

            // Skip if any operand is a non-integer type (char, float, pointer, etc.)
            if left_type == "not_applicable" || right_type == "not_applicable" {
                return;
            }

            // Skip if either operand is unsigned — unsigned wrap is INT30-C, not INT32-C
            if left_type == "unsigned" || right_type == "unsigned" {
                return;
            }

            // Check for signed integer modulo
            // INT_MIN % -1 causes overflow
            if self.is_signed_type(&left_type) || self.is_signed_type(&right_type) {
                let has_explicit_risk = (left_text.contains("INT_MIN")
                    || left_text.contains("LONG_MIN")
                    || self.could_be_int_min(&left, source))
                    && (right_text == "-1" || right_text.contains("-1"));

                // Also flag generic signed modulo of variables (could be INT_MIN % -1 at runtime)
                // but skip if the right operand (divisor) is unsigned — can't be -1
                let is_variable_modulo = left.kind() == "identifier"
                    && right.kind() == "identifier"
                    && !self.is_unsigned_type(&right_type);

                if (has_explicit_risk || is_variable_modulo)
                    && !self.has_modulo_overflow_check(node, source)
                {
                    let start_point = node.start_position();
                    let expr_text = get_node_text(node, source);

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

    fn check_negation(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        type_map: &HashMap<String, String>,
    ) {
        if let Some(argument) = node.child_by_field_name("argument") {
            let _arg_text = get_node_text(&argument, source);
            let arg_type = self.infer_type(&argument, source, type_map);

            // Skip if operand is a non-integer type (char, float, pointer, etc.)
            if arg_type == "not_applicable" {
                return;
            }

            // Check for negation of signed integers, especially -INT_MIN which causes overflow
            if self.is_signed_type(&arg_type) && !self.has_negation_overflow_check(node, source) {
                let start_point = node.start_position();
                let expr_text = get_node_text(node, source);

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
                    suggestion: Some(
                        "Add check: if (value == INT_MIN) { /* handle error */ }".to_string(),
                    ),
                    ..Default::default()
                });
            }
        }
    }

    fn check_left_shift(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        type_map: &HashMap<String, String>,
    ) {
        if let (Some(left), Some(_right)) = (
            node.child_by_field_name("left"),
            node.child_by_field_name("right"),
        ) {
            let left_type = self.infer_type(&left, source, type_map);

            // Skip if operand is a non-integer type (char, float, pointer, etc.)
            if left_type == "not_applicable" {
                return;
            }

            if self.is_signed_type(&left_type) {
                // Skip if inside a block guarded by a type-limit bounds check
                let op_names = self.extract_operand_names(node, source);
                if self.is_inside_bounds_checked_block(node, source, &op_names) {
                    return;
                }

                if !self.has_shift_overflow_check(node, source) {
                    let start_point = node.start_position();
                    let expr_text = get_node_text(node, source);

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

    fn check_compound_addition(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        type_map: &HashMap<String, String>,
    ) {
        if let Some(left) = node.child_by_field_name("left") {
            let left_type = self.infer_type(&left, source, type_map);

            // Skip if operand is a non-integer type (char, float, pointer, etc.)
            if left_type == "not_applicable" {
                return;
            }

            if self.is_signed_type(&left_type) {
                // Skip if inside a block guarded by a type-limit bounds check
                let op_names = self.extract_operand_names(node, source);
                if self.is_inside_bounds_checked_block(node, source, &op_names) {
                    return;
                }

                if !self.has_overflow_check_compound(node, source) {
                    let start_point = node.start_position();
                    let expr_text = get_node_text(node, source);

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
                        suggestion: Some(
                            "Add overflow check before compound assignment".to_string(),
                        ),
                        ..Default::default()
                    });
                }
            }
        }
    }

    fn check_compound_subtraction(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        type_map: &HashMap<String, String>,
    ) {
        if let Some(left) = node.child_by_field_name("left") {
            let left_type = self.infer_type(&left, source, type_map);

            // Skip if operand is a non-integer type (char, float, pointer, etc.)
            if left_type == "not_applicable" {
                return;
            }

            if self.is_signed_type(&left_type) {
                // Skip if inside a block guarded by a type-limit bounds check
                let op_names = self.extract_operand_names(node, source);
                if self.is_inside_bounds_checked_block(node, source, &op_names) {
                    return;
                }

                if !self.has_overflow_check_compound(node, source) {
                    let start_point = node.start_position();
                    let expr_text = get_node_text(node, source);

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

    fn check_compound_multiplication(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        type_map: &HashMap<String, String>,
    ) {
        if let Some(left) = node.child_by_field_name("left") {
            let left_type = self.infer_type(&left, source, type_map);

            // Skip if operand is a non-integer type (char, float, pointer, etc.)
            if left_type == "not_applicable" {
                return;
            }

            if self.is_signed_type(&left_type) {
                // Skip if inside a block guarded by a type-limit bounds check
                let op_names = self.extract_operand_names(node, source);
                if self.is_inside_bounds_checked_block(node, source, &op_names) {
                    return;
                }

                if !self.has_overflow_check_compound(node, source) {
                    let start_point = node.start_position();
                    let expr_text = get_node_text(node, source);

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

    fn check_compound_division(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let (Some(left), Some(right)) = (
            node.child_by_field_name("left"),
            node.child_by_field_name("right"),
        ) {
            let left_text = get_node_text(&left, source);
            let right_text = get_node_text(&right, source);

            if (left_text.contains("INT_MIN") || self.could_be_int_min(&left, source))
                && (right_text == "-1" || right_text.contains("-1"))
                && !self.has_overflow_check_compound(node, source)
            {
                let start_point = node.start_position();
                let expr_text = get_node_text(node, source);

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

    fn check_compound_modulo(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let (Some(left), Some(right)) = (
            node.child_by_field_name("left"),
            node.child_by_field_name("right"),
        ) {
            let left_text = get_node_text(&left, source);
            let right_text = get_node_text(&right, source);

            if (left_text.contains("INT_MIN") || self.could_be_int_min(&left, source))
                && (right_text == "-1" || right_text.contains("-1"))
                && !self.has_overflow_check_compound(node, source)
            {
                let start_point = node.start_position();
                let expr_text = get_node_text(node, source);

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

    fn check_compound_left_shift(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        type_map: &HashMap<String, String>,
    ) {
        if let Some(left) = node.child_by_field_name("left") {
            let left_type = self.infer_type(&left, source, type_map);

            // Skip if operand is a non-integer type (char, float, pointer, etc.)
            if left_type == "not_applicable" {
                return;
            }

            if self.is_signed_type(&left_type) && !self.has_overflow_check_compound(node, source) {
                let start_point = node.start_position();
                let expr_text = get_node_text(node, source);

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

    fn check_increment_decrement(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        type_map: &HashMap<String, String>,
    ) {
        if let Some(argument) = node.child_by_field_name("argument") {
            let arg_type = self.infer_type(&argument, source, type_map);

            // Skip if operand is a non-integer type (char, float, pointer, etc.)
            if arg_type == "not_applicable" {
                return;
            }

            if self.is_signed_type(&arg_type) {
                // Skip if this is part of a safe for loop (bounded, starting from small values)
                if self.is_in_safe_for_loop(node, source) {
                    return;
                }

                // Skip if inside a block guarded by a type-limit bounds check
                let op_names = self.extract_operand_names(node, source);
                if self.is_inside_bounds_checked_block(node, source, &op_names) {
                    return;
                }

                let operator = self.get_update_operator(node, source);
                if (operator == "++" || operator == "--")
                    && !self.has_overflow_check_update(node, source)
                {
                    let start_point = node.start_position();
                    let expr_text = get_node_text(node, source);

                    let message = if operator == "++" {
                        format!(
                            "Signed integer increment '{}' may overflow at INT_MAX",
                            expr_text
                        )
                    } else {
                        format!(
                            "Signed integer decrement '{}' may overflow at INT_MIN",
                            expr_text
                        )
                    };

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::Medium,
                        message,
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some(
                            "Add bounds checking before increment/decrement".to_string(),
                        ),
                        ..Default::default()
                    });
                }
            }
        }
    }

    fn check_function_call(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(function_node) = node.child_by_field_name("function") {
            let function_name = get_node_text(&function_node, source);

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

    fn check_allocation_overflow(
        &self,
        node: &Node,
        source: &str,
        function_name: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let args = self.get_function_arguments(node, source);

        for (i, arg) in args.iter().enumerate() {
            if self.contains_arithmetic(arg) && !self.has_allocation_overflow_check(node, source) {
                let start_point = node.start_position();
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::High,
                    message: format!(
                        "{}() argument {} contains arithmetic that may overflow: '{}'",
                        function_name,
                        i + 1,
                        arg
                    ),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some(
                        "Validate arithmetic operations before passing to allocation functions"
                            .to_string(),
                    ),
                    ..Default::default()
                });
            }
        }
    }

    fn check_memory_function_overflow(
        &self,
        node: &Node,
        source: &str,
        function_name: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let args = self.get_function_arguments(node, source);

        // Check size arguments for arithmetic that might overflow
        let size_arg_indices = match function_name {
            "memcpy" | "memmove" => vec![2], // Third argument is size
            "memset" => vec![2],             // Third argument is size
            _ => vec![],
        };

        for &idx in &size_arg_indices {
            if let Some(arg) = args.get(idx) {
                if self.contains_arithmetic(arg)
                    && !self.has_memory_function_overflow_check(node, source)
                {
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
                        suggestion: Some(
                            "Validate size calculations before passing to memory functions"
                                .to_string(),
                        ),
                        ..Default::default()
                    });
                }
            }
        }
    }

    fn check_abs_overflow(
        &self,
        node: &Node,
        source: &str,
        function_name: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // abs(INT_MIN), labs(LONG_MIN), llabs(LLONG_MIN) all cause overflow
        // because the absolute value of the minimum signed integer cannot be represented
        if !self.has_abs_overflow_check(node, source) {
            let start_point = node.start_position();
            let expr_text = get_node_text(node, source);

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

    /// Collect variable types from function parameters and local declarations.
    /// Walks the entire AST to find all function_definition nodes and collects
    /// types from their parameters and body declarations.
    fn collect_variable_types(&self, node: &Node, source: &str) -> HashMap<String, String> {
        let mut type_map = HashMap::new();

        if node.kind() == "function_definition" {
            // Collect from function parameters
            if let Some(declarator) = node.child_by_field_name("declarator") {
                self.collect_params_from_declarator(&declarator, source, &mut type_map);
            }
            // Collect from local declarations in the function body
            if let Some(body) = node.child_by_field_name("body") {
                self.collect_local_declarations(&body, source, &mut type_map);
            }
        }

        // Recurse into children to find nested function_definitions
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                let child_map = self.collect_variable_types(&child, source);
                type_map.extend(child_map);
            }
        }

        type_map
    }

    fn collect_params_from_declarator(
        &self,
        node: &Node,
        source: &str,
        type_map: &mut HashMap<String, String>,
    ) {
        if node.kind() == "function_declarator" {
            if let Some(params) = node.child_by_field_name("parameters") {
                for i in 0..params.child_count() {
                    if let Some(param) = params.child(i) {
                        if param.kind() == "parameter_declaration" {
                            self.extract_type_and_name(&param, source, type_map);
                        }
                    }
                }
            }
        }
        // Recurse to find nested function_declarator (e.g. pointer declarators)
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.collect_params_from_declarator(&child, source, type_map);
            }
        }
    }

    fn collect_local_declarations(
        &self,
        node: &Node,
        source: &str,
        type_map: &mut HashMap<String, String>,
    ) {
        if node.kind() == "declaration" {
            self.extract_type_and_name(node, source, type_map);
        }
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.collect_local_declarations(&child, source, type_map);
            }
        }
    }

    fn extract_type_and_name(
        &self,
        node: &Node,
        source: &str,
        type_map: &mut HashMap<String, String>,
    ) {
        let mut type_text = String::new();

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "primitive_type" | "sized_type_specifier" | "type_identifier" => {
                        type_text = get_node_text(&child, source).to_string();
                    }
                    _ => {}
                }
            }
        }

        if type_text.is_empty() {
            return;
        }

        // Extract variable names from declarators
        if let Some(declarator) = node.child_by_field_name("declarator") {
            if let Some(name) = Self::extract_identifier_name(&declarator, source) {
                type_map.insert(name, type_text.clone());
            }
        }

        // Handle init_declarator lists (e.g. `int a, b;`)
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "init_declarator" {
                    if let Some(decl) = child.child_by_field_name("declarator") {
                        if let Some(name) = Self::extract_identifier_name(&decl, source) {
                            type_map.insert(name, type_text.clone());
                        }
                    }
                }
            }
        }
    }

    fn extract_identifier_name(node: &Node, source: &str) -> Option<String> {
        match node.kind() {
            "identifier" => Some(get_node_text(node, source).to_string()),
            "pointer_declarator" | "array_declarator" | "parenthesized_declarator" => {
                if let Some(inner) = node.child_by_field_name("declarator") {
                    Self::extract_identifier_name(&inner, source)
                } else {
                    None
                }
            }
            _ => {
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        if child.kind() == "identifier" {
                            return Some(get_node_text(&child, source).to_string());
                        }
                    }
                }
                None
            }
        }
    }

    fn infer_type(&self, node: &Node, source: &str, type_map: &HashMap<String, String>) -> String {
        let text = get_node_text(node, source);

        // Check the type map FIRST — most reliable source of type info.
        // Must come before text heuristics because variable names like "index"
        // contain "int" as a substring, causing false signed classification.
        if node.kind() == "identifier" {
            if let Some(declared_type) = type_map.get(text) {
                if self.is_unsigned_type(declared_type) {
                    return "unsigned".to_string();
                }
                // Only return signed if the type is clearly an integer type
                if declared_type.contains("int")
                    || declared_type.contains("short")
                    || declared_type.contains("long")
                    || declared_type == "signed"
                {
                    return "signed".to_string();
                }
                // Non-integer types (float, double, char, pointers, structs) — not applicable to INT32-C
                return "not_applicable".to_string();
            }
        }

        // Look for explicit unsigned type indicators
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

        // Field expressions (e.g., self->capacity) without type evidence
        // should not be assumed signed — return not_applicable
        if node.kind() == "field_expression" {
            return "not_applicable".to_string();
        }

        // Look for explicit signed type indicators (only for non-identifier nodes
        // like type specifiers in casts/declarations — identifiers checked above)
        if node.kind() != "identifier"
            && (text.contains("signed")
                || text.contains("int")
                || text.contains("short")
                || text.contains("long"))
        {
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

        // Fall back to old heuristic for variable names not in the type map
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
        if text.starts_with("i")
            || text.contains("signed")
            || text.contains("count")
            || text.contains("index")
        {
            return "signed".to_string();
        }

        // For variables NOT in the type map, default to unknown instead of signed
        // This prevents false positives on variables whose type we can't determine
        "unknown".to_string()
    }

    fn find_variable_declaration(
        &self,
        node: &Node,
        source: &str,
        var_name: &str,
    ) -> Option<String> {
        // Look for the function that contains this node
        let mut current = node.parent();
        while let Some(parent) = current {
            if parent.kind() == "function_definition" {
                // Look in function parameters
                if let Some(params) = parent.child_by_field_name("parameters") {
                    let params_text = get_node_text(&params, source);
                    if params_text.contains("unsigned") && params_text.contains(var_name) {
                        return Some("unsigned".to_string());
                    }
                    if (params_text.contains("signed") || params_text.contains("int"))
                        && params_text.contains(var_name)
                        && !params_text.contains("unsigned")
                    {
                        return Some("signed".to_string());
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
                let decl_text = get_node_text(&parent, source);
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
        type_str == "signed" || type_str == "int"
    }

    fn is_unsigned_type(&self, type_str: &str) -> bool {
        type_str == "unsigned"
            || type_str == "size_t"
            || type_str.contains("uint")
            || type_str.starts_with("unsigned ")
            || type_str == "SIZE_MAX"
    }

    fn could_be_int_min(&self, node: &Node, source: &str) -> bool {
        let text = get_node_text(node, source);
        text.contains("INT_MIN")
            || (text.starts_with("min") && (text.contains("val") || text.contains("num")))
    }

    /// Check if either operand has a wider-type cast (long long), making overflow impossible.
    fn has_wider_cast(&self, left_text: &str, right_text: &str) -> bool {
        let has_ll = |text: &str| {
            text.contains("long long")
                || text.starts_with("(signed long long)")
                || text.starts_with("(long long)")
                || text.starts_with("(int64_t)")
                || text.starts_with("(int_least64_t)")
        };
        has_ll(left_text) || has_ll(right_text)
    }

    /// Extract operand identifier names from a binary expression node.
    /// Returns a vec of variable names found in the left/right operands.
    fn extract_operand_names(&self, node: &Node, source: &str) -> Vec<String> {
        let mut names = Vec::new();
        if let Some(left) = node.child_by_field_name("left") {
            Self::collect_identifiers(&left, source, &mut names);
        }
        if let Some(right) = node.child_by_field_name("right") {
            Self::collect_identifiers(&right, source, &mut names);
        }
        // For unary/update expressions, check argument
        if let Some(arg) = node.child_by_field_name("argument") {
            Self::collect_identifiers(&arg, source, &mut names);
        }
        names
    }

    fn collect_identifiers(node: &Node, source: &str, names: &mut Vec<String>) {
        if node.kind() == "identifier" {
            let name = get_node_text(node, source).to_string();
            if !names.contains(&name) {
                names.push(name);
            }
        }
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                Self::collect_identifiers(&child, source, names);
            }
        }
    }

    fn contains_arithmetic(&self, expr: &str) -> bool {
        expr.contains('+') || expr.contains('-') || expr.contains('*') || expr.contains('/')
    }

    fn is_constant_expression(&self, expr: &str) -> bool {
        // Check if expression is a constant (literal number or named constant like INT_MAX)
        let trimmed = expr.trim();

        // Numeric literals
        if trimmed
            .chars()
            .all(|c| c.is_ascii_digit() || c == '-' || c == '+')
        {
            return true;
        }

        // Named constants
        if trimmed.contains("INT_MAX")
            || trimmed.contains("INT_MIN")
            || trimmed.contains("LONG_MAX")
            || trimmed.contains("LONG_MIN")
            || trimmed.contains("LLONG_MAX")
            || trimmed.contains("LLONG_MIN")
            || trimmed.contains("UINT_MAX")
            || trimmed.contains("SIZE_MAX")
        {
            return true;
        }

        false
    }

    /// Check if a binary expression is inside a for-loop with a small constant bound,
    /// making overflow impossible (e.g., `i * i` where `i < 100`).
    fn is_in_bounded_for_loop(&self, node: &Node, source: &str) -> bool {
        let mut current = node.parent();
        while let Some(parent) = current {
            if parent.kind() == "for_statement" {
                let for_text = get_node_text(&parent, source);
                // Check that the loop doesn't involve near-limit values
                if for_text.contains("INT_MAX")
                    || for_text.contains("LONG_MAX")
                    || for_text.contains("INT_MIN")
                    || for_text.contains("LONG_MIN")
                {
                    return false;
                }
                // Heuristic: if the for-loop condition contains a small numeric bound
                // (< 4 digits), the loop variable won't overflow in typical arithmetic
                if let Some(condition) = parent.child_by_field_name("condition") {
                    let cond_text = get_node_text(&condition, source);
                    // Look for patterns like "i < 100" or "i <= 999"
                    let bound_re = cond_text
                        .split(|c: char| !c.is_ascii_digit())
                        .filter(|s| !s.is_empty())
                        .any(|num| {
                            num.len() <= 4
                                && num.parse::<i64>().is_ok_and(|n| (0..=10000).contains(&n))
                        });
                    if bound_re {
                        return true;
                    }
                }
                return false;
            }
            if parent.kind() == "function_definition" {
                break;
            }
            current = parent.parent();
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
                    let init_text = get_node_text(&initializer, source);
                    if init_text.contains("INT_MAX")
                        || init_text.contains("LONG_MAX")
                        || init_text.contains("INT_MIN")
                        || init_text.contains("LONG_MIN")
                    {
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
        // First check the immediate surrounding context (parent/grandparent) — strict pattern
        if self.has_surrounding_check(node, source, &["INT_MAX", "INT_MIN", " - ", " > ", " < "]) {
            return true;
        }

        // Check for any signed limit macro with a comparison operator in the function.
        // Relaxed from requiring ALL of [INT_MAX, INT_MIN, " - ", " > ", " < "] to
        // requiring any limit macro + comparison — handles CHAR_MAX, SHRT_MAX, etc.
        // The scoped check (guard_keywords + operand match) ensures precision.
        self.has_function_level_overflow_check(node, source, &[" > "])
            || self.has_function_level_overflow_check(node, source, &[" < "])
            || self.has_function_level_overflow_check(node, source, &[" >= "])
            || self.has_function_level_overflow_check(node, source, &[" <= "])
    }

    fn has_overflow_check_subtraction(&self, node: &Node, source: &str) -> bool {
        // First check the immediate surrounding context (parent/grandparent)
        if self.has_surrounding_check(node, source, &["INT_MAX", "INT_MIN", " + ", " > ", " < "]) {
            return true;
        }

        // Then check the broader function context for overflow checks
        self.has_function_level_overflow_check(
            node,
            source,
            &["INT_MAX", "INT_MIN", " + ", " > ", " < "],
        )
    }

    fn has_overflow_check_multiplication(&self, node: &Node, source: &str) -> bool {
        // Check for multiplication overflow patterns:
        // 1. if (a > INT_MAX / b) - division-based check
        // 2. Complex checks with INT_MAX/INT_MIN and division
        if self.has_surrounding_check(node, source, &["INT_MAX", " / "])
            || self.has_surrounding_check(node, source, &["INT_MIN", " / "])
            || self.has_surrounding_check(node, source, &["LONG_MAX", " / "])
            || self.has_surrounding_check(node, source, &["LONG_MIN", " / "])
        {
            return true;
        }

        // Function-level checks
        if self.has_function_level_patterns_any(node, source, &["INT_MAX", " / "])
            || self.has_function_level_patterns_any(node, source, &["INT_MIN", " / "])
            || self.has_function_level_patterns_any(node, source, &["LONG_MAX", " / "])
            || self.has_function_level_patterns_any(node, source, &["LONG_MIN", " / "])
        {
            return true;
        }

        false
    }

    fn has_division_overflow_check(&self, node: &Node, source: &str) -> bool {
        // Check for INT_MIN/-1 or LONG_MIN/-1 or LLONG_MIN/-1 division overflow checks
        if self.has_surrounding_check(node, source, &["INT_MIN", " == ", " -1"])
            || self.has_surrounding_check(node, source, &["LONG_MIN", " == ", " -1"])
            || self.has_surrounding_check(node, source, &["LLONG_MIN", " == ", " -1"])
        {
            return true;
        }

        self.has_function_level_patterns_any(node, source, &["INT_MIN", " == ", " -1"])
            || self.has_function_level_patterns_any(node, source, &["LONG_MIN", " == ", " -1"])
            || self.has_function_level_patterns_any(node, source, &["LLONG_MIN", " == ", " -1"])
    }

    fn has_modulo_overflow_check(&self, node: &Node, source: &str) -> bool {
        // Check for INT_MIN%- 1 or LONG_MIN%-1 or LLONG_MIN%-1 modulo overflow checks
        if self.has_surrounding_check(node, source, &["INT_MIN", " == ", " -1"])
            || self.has_surrounding_check(node, source, &["LONG_MIN", " == ", " -1"])
            || self.has_surrounding_check(node, source, &["LLONG_MIN", " == ", " -1"])
        {
            return true;
        }

        self.has_function_level_patterns_any(node, source, &["INT_MIN", " == ", " -1"])
            || self.has_function_level_patterns_any(node, source, &["LONG_MIN", " == ", " -1"])
            || self.has_function_level_patterns_any(node, source, &["LLONG_MIN", " == ", " -1"])
    }

    fn has_negation_overflow_check(&self, node: &Node, source: &str) -> bool {
        // Check for negation of INT_MIN, LONG_MIN, or LLONG_MIN
        if self.has_surrounding_check(node, source, &["INT_MIN", " == "])
            || self.has_surrounding_check(node, source, &["LONG_MIN", " == "])
            || self.has_surrounding_check(node, source, &["LLONG_MIN", " == "])
        {
            return true;
        }

        self.has_function_level_patterns_any(node, source, &["INT_MIN", " == "])
            || self.has_function_level_patterns_any(node, source, &["LONG_MIN", " == "])
            || self.has_function_level_patterns_any(node, source, &["LLONG_MIN", " == "])
    }

    fn has_shift_overflow_check(&self, node: &Node, source: &str) -> bool {
        // Check for left shift overflow patterns:
        // Complete check requires BOTH:
        // 1. Shift amount validation AND value range check
        // 2. Value range check: a > (INT_MAX >> b) or similar with LONG_MAX

        // Check for value range pattern (most comprehensive)
        if self.has_surrounding_check(node, source, &["LONG_MAX", " >> "])
            || self.has_surrounding_check(node, source, &["INT_MAX", " >> "])
        {
            return true;
        }

        if self.has_function_level_patterns_any(node, source, &["LONG_MAX", " >> "])
            || self.has_function_level_patterns_any(node, source, &["INT_MAX", " >> "])
        {
            return true;
        }

        // Only accept PRECISION if it's combined with value range checks
        // (PRECISION alone is insufficient - see wiki_noncompliant_6)
        if (self.has_surrounding_check(node, source, &["PRECISION"])
            || self.has_function_level_patterns_any(node, source, &["PRECISION"]))
            && (self.has_function_level_patterns_any(node, source, &[" >> "])
                || self.has_function_level_patterns_any(node, source, &[" < ", "sizeof"]))
        {
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
        if self.has_surrounding_check(node, source, &["if", "INT_MAX", " == "])
            || self.has_surrounding_check(node, source, &["if", "INT_MIN", " == "])
        {
            return true;
        }

        self.has_function_level_patterns_any(node, source, &["if", "INT_MAX", " == "])
            || self.has_function_level_patterns_any(node, source, &["if", "INT_MIN", " == "])
    }

    fn has_allocation_overflow_check(&self, node: &Node, source: &str) -> bool {
        self.has_surrounding_check(node, source, &["SIZE_MAX", " / ", " > ", "if"])
    }

    fn has_memory_function_overflow_check(&self, node: &Node, source: &str) -> bool {
        self.has_surrounding_check(node, source, &["SIZE_MAX", " > ", "if"])
    }

    fn has_abs_overflow_check(&self, node: &Node, source: &str) -> bool {
        // Check if there's a check for INT_MIN/LONG_MIN/LLONG_MIN before the abs call
        if self.has_surrounding_check(node, source, &["INT_MIN", "if"])
            || self.has_surrounding_check(node, source, &["LONG_MIN", "if"])
            || self.has_surrounding_check(node, source, &["LLONG_MIN", "if"])
        {
            return true;
        }
        self.has_function_level_overflow_check(node, source, &["INT_MIN", "if"])
            || self.has_function_level_overflow_check(node, source, &["LONG_MIN", "if"])
            || self.has_function_level_overflow_check(node, source, &["LLONG_MIN", "if"])
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
                        let text = get_node_text(&child, source);
                        // Check if this is a comparison operator
                        if matches!(text, ">" | "<" | ">=" | "<=" | "==" | "!=") {
                            return true;
                        }
                    }
                }
            }

            // Stop at statement boundaries to avoid going too far up the tree
            if matches!(
                parent.kind(),
                "expression_statement"
                    | "return_statement"
                    | "declaration"
                    | "function_definition"
                    | "compound_statement"
            ) {
                break;
            }

            current = parent.parent();
        }

        false
    }

    /// Check if the function containing this node has overflow checking code (all patterns must match).
    /// Scoped to operands: requires at least one operand name from the flagged expression to appear
    /// near the overflow guard pattern, preventing one variable's check from suppressing another's.
    fn has_function_level_overflow_check(
        &self,
        node: &Node,
        source: &str,
        patterns: &[&str],
    ) -> bool {
        let operand_names = self.extract_operand_names(node, source);
        self.has_function_level_overflow_check_scoped(node, source, patterns, &operand_names)
    }

    /// Check if the function containing this node has overflow checking code (all patterns must match).
    /// Variant used when operand names are not applicable (e.g., compound assignments).
    fn has_function_level_patterns_any(
        &self,
        node: &Node,
        source: &str,
        patterns: &[&str],
    ) -> bool {
        let operand_names = self.extract_operand_names(node, source);
        self.has_function_level_overflow_check_scoped(node, source, patterns, &operand_names)
    }

    /// Core operand-scoped function-level overflow check.
    /// Checks that:
    /// 1. All the given patterns exist in the function (e.g., "INT_MAX", " - ")
    /// 2. At least one operand from the flagged expression appears in a line that
    ///    also contains at least one of the overflow-guard keywords (INT_MAX, INT_MIN, etc.)
    fn has_function_level_overflow_check_scoped(
        &self,
        node: &Node,
        source: &str,
        patterns: &[&str],
        operand_names: &[String],
    ) -> bool {
        // Find the containing function
        let mut current = node.parent();
        while let Some(parent) = current {
            if parent.kind() == "function_definition" {
                let func_text = get_node_text(&parent, source);

                // First: do the patterns even exist in this function?
                if !patterns.iter().all(|p| func_text.contains(p)) {
                    return false;
                }

                // If we have no operand names to scope against, fall back to
                // the old behavior (patterns found anywhere in the function)
                if operand_names.is_empty() {
                    return true;
                }

                // The overflow-guard keywords we look for near operand names
                let guard_keywords = [
                    "INT_MAX",
                    "INT_MIN",
                    "LONG_MAX",
                    "LONG_MIN",
                    "LLONG_MAX",
                    "LLONG_MIN",
                    "UINT_MAX",
                    "SIZE_MAX",
                    "CHAR_MAX",
                    "CHAR_MIN",
                    "SCHAR_MAX",
                    "SCHAR_MIN",
                    "SHRT_MAX",
                    "SHRT_MIN",
                ];

                // Search for lines that contain an overflow guard keyword AND
                // at least one operand name from the flagged expression.
                for line in func_text.lines() {
                    let trimmed = line.trim();
                    let has_guard = guard_keywords.iter().any(|kw| trimmed.contains(kw));
                    if !has_guard {
                        continue;
                    }
                    if operand_names
                        .iter()
                        .any(|name| self.contains_word(trimmed, name))
                    {
                        return true;
                    }
                }

                return false;
            }
            current = parent.parent();
        }
        false
    }

    /// Check if `text` contains `word` as a whole word (not a substring of another identifier).
    fn contains_word(&self, text: &str, word: &str) -> bool {
        if word.is_empty() {
            return false;
        }
        let mut start = 0;
        while let Some(pos) = text[start..].find(word) {
            let abs_pos = start + pos;
            let before_ok = abs_pos == 0
                || !text.as_bytes()[abs_pos - 1].is_ascii_alphanumeric()
                    && text.as_bytes()[abs_pos - 1] != b'_';
            let after_pos = abs_pos + word.len();
            let after_ok = after_pos >= text.len()
                || !text.as_bytes()[after_pos].is_ascii_alphanumeric()
                    && text.as_bytes()[after_pos] != b'_';
            if before_ok && after_ok {
                return true;
            }
            start = abs_pos + 1;
        }
        false
    }

    /// Check if the arithmetic node is inside a block guarded by a type-limit bounds check.
    /// Walks up ancestors (up to 15 levels) looking for an `if_statement` whose condition
    /// references a signed type-limit macro AND at least one operand of the arithmetic.
    fn is_inside_bounds_checked_block(
        &self,
        node: &Node,
        source: &str,
        op_names: &[String],
    ) -> bool {
        const SIGNED_LIMIT_MACROS: &[&str] = &[
            "INT_MAX",
            "INT_MIN",
            "LONG_MAX",
            "LONG_MIN",
            "LLONG_MAX",
            "LLONG_MIN",
            "CHAR_MAX",
            "CHAR_MIN",
            "SCHAR_MAX",
            "SCHAR_MIN",
            "SHRT_MAX",
            "SHRT_MIN",
        ];

        let mut current = *node;
        let mut depth = 0;
        while let Some(parent) = current.parent() {
            depth += 1;
            if depth > 15 {
                break;
            }
            if parent.kind() == "function_definition" {
                break;
            }
            if parent.kind() == "if_statement" {
                if let Some(condition) = parent.child_by_field_name("condition") {
                    let cond_text = get_node_text(&condition, source);
                    let has_limit = SIGNED_LIMIT_MACROS
                        .iter()
                        .any(|m| self.contains_word(cond_text, m));
                    if has_limit {
                        if op_names.is_empty() {
                            return true;
                        }
                        if op_names
                            .iter()
                            .any(|name| self.contains_word(cond_text, name))
                        {
                            return true;
                        }
                    }
                }
            }
            current = parent;
        }
        false
    }

    fn has_surrounding_check(&self, node: &Node, source: &str, patterns: &[&str]) -> bool {
        if let Some(parent) = node.parent() {
            if let Some(grandparent) = parent.parent() {
                let context = get_node_text(&grandparent, source);
                return patterns.iter().all(|pattern| context.contains(pattern));
            }
        }
        false
    }

    fn get_operator(&self, node: &Node, source: &str) -> Option<String> {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                let text = get_node_text(&child, source);
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
                let text = get_node_text(&child, source);
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
                let text = get_node_text(&child, source);
                if matches!(text, "-" | "+" | "!" | "~") {
                    return Some(text.to_string());
                }
            }
        }
        None
    }

    fn get_update_operator(&self, node: &Node, source: &str) -> String {
        let text = get_node_text(node, source);
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
                        let arg_text = get_node_text(&child, source).to_string();
                        args.push(arg_text.trim().to_string());
                    }
                }
            }
        }

        args
    }
}

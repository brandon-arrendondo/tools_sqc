use super::super::{CertRule, RuleViolation};
use crate::analyze::const_eval::{self, MacroConstantMap};
use crate::analyze::context::ProjectContext;
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use std::cell::RefCell;
use std::collections::HashMap;
use tree_sitter::Node;

pub struct Int30C {
    project_macros: RefCell<MacroConstantMap>,
    current_macros: RefCell<MacroConstantMap>,
}

impl Int30C {
    pub fn new() -> Self {
        Self {
            project_macros: RefCell::new(MacroConstantMap::new()),
            current_macros: RefCell::new(MacroConstantMap::new()),
        }
    }
}

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

    fn set_project_context(&self, context: &ProjectContext) {
        *self.project_macros.borrow_mut() = context.macro_constants.clone();
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        let type_map = self.collect_variable_types(node, source);

        // Merge project-level macros with per-file macros
        let mut macros = self.project_macros.borrow().clone();
        macros.extend(const_eval::collect_macro_constants(node, source));
        *self.current_macros.borrow_mut() = macros;

        self.check_node(node, source, &mut violations, &type_map);

        violations
    }
}

impl Int30C {
    /// For compound assignments (`x op= y`), check if `x op y` provably fits
    /// in an unsigned integer of `bits` width using constant evaluation.
    /// Note: only resolves the RHS — the LHS is the mutation target and its
    /// initial assignment doesn't reflect its current value (especially in loops).
    fn compound_expr_fits_unsigned(&self, node: &Node, source: &str, op: &str, bits: u32) -> bool {
        let macros = self.current_macros.borrow();
        if let (Some(left), Some(right)) = (
            node.child_by_field_name("left"),
            node.child_by_field_name("right"),
        ) {
            let loop_ranges = const_eval::extract_loop_var_ranges(node, source, &macros);
            let mut var_ranges = loop_ranges.clone();
            // Only resolve RHS identifiers — LHS is the mutation target
            const_eval::resolve_identifiers_in_expr(
                &right,
                source,
                &macros,
                &loop_ranges,
                &mut var_ranges,
            );
            // LHS: only use loop ranges and macros (not local assignments)
            let lr = const_eval::try_evaluate_range(&left, source, &macros, &loop_ranges);
            let rr = const_eval::try_evaluate_range(&right, source, &macros, &var_ranges);
            if let (Some(lr), Some(rr)) = (lr, rr) {
                let result = match op {
                    "+" => lr.add(&rr),
                    "-" => lr.sub(&rr),
                    "*" => lr.mul(&rr),
                    "<<" => lr.shl(&rr),
                    _ => None,
                };
                if let Some(range) = result {
                    return range.fits_in_unsigned(bits);
                }
            }
        }
        false
    }

    fn check_node(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        type_map: &HashMap<String, String>,
    ) {
        match node.kind() {
            "binary_expression" => {
                self.check_binary_operation(node, source, violations, type_map);
            }
            "assignment_expression" => {
                self.check_assignment_operation(node, source, violations, type_map);
            }
            "call_expression" => {
                self.check_function_call(node, source, violations);
            }
            "update_expression" => {
                self.check_increment_decrement(node, source, violations, type_map);
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
                "<<=" => self.check_compound_left_shift(node, source, violations, type_map),
                _ => {}
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

            // Skip pointer arithmetic: ptr + n, n + ptr
            // "not_applicable" means a non-integer type (pointer, float, struct).
            // Adding an unsigned offset to a pointer is valid C pointer arithmetic and
            // is NOT an unsigned integer overflow — that is ARR39-C's domain.
            if left_type == "not_applicable" || right_type == "not_applicable" {
                // Recurse into children only
                return;
            }

            if (self.is_unsigned_type(&left_type) || self.is_unsigned_type(&right_type))
                && !self.has_overflow_check_addition(node, source)
            {
                // Check for var + 1 or 1 + var bounded by enclosing loop condition
                if self.is_add_one_bounded_by_loop(node, source) {
                    return;
                }

                // Skip if constant evaluation proves the result fits in 32-bit unsigned
                if const_eval::expression_fits_in_unsigned(
                    node,
                    source,
                    &self.current_macros.borrow(),
                    32,
                ) {
                    return;
                }

                let start_point = node.start_position();
                let expr_text = get_node_text(node, source);

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
                    suggestion: Some(
                        "Add overflow check: if (UINT_MAX - a < b) { /* handle error */ }"
                            .to_string(),
                    ),
                    ..Default::default()
                });
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

            // Skip pointer arithmetic: ptr - n, ptr - ptr2
            if left_type == "not_applicable" || right_type == "not_applicable" {
                return;
            }

            if (self.is_unsigned_type(&left_type) || self.is_unsigned_type(&right_type))
                && !self.has_overflow_check_subtraction(node, source)
            {
                // Skip if constant evaluation proves the result fits in 32-bit unsigned
                if const_eval::expression_fits_in_unsigned(
                    node,
                    source,
                    &self.current_macros.borrow(),
                    32,
                ) {
                    return;
                }

                let start_point = node.start_position();
                let expr_text = get_node_text(node, source);

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
                    suggestion: Some(
                        "Add underflow check: if (a < b) { /* handle error */ }".to_string(),
                    ),
                    ..Default::default()
                });
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

            if (self.is_unsigned_type(&left_type) || self.is_unsigned_type(&right_type))
                && !self.has_overflow_check_multiplication(node, source)
            {
                // Skip if constant evaluation proves the result fits in 32-bit unsigned
                if const_eval::expression_fits_in_unsigned(
                    node,
                    source,
                    &self.current_macros.borrow(),
                    32,
                ) {
                    return;
                }

                let start_point = node.start_position();
                let expr_text = get_node_text(node, source);

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
                    suggestion: Some(
                        "Add overflow check: if (a > UINT_MAX / b) { /* handle error */ }"
                            .to_string(),
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

            if self.is_unsigned_type(&left_type) && !self.has_shift_overflow_check(node, source) {
                // Skip if constant evaluation proves the result fits in 32-bit unsigned
                if const_eval::expression_fits_in_unsigned(
                    node,
                    source,
                    &self.current_macros.borrow(),
                    32,
                ) {
                    return;
                }

                let start_point = node.start_position();
                let expr_text = get_node_text(node, source);

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

    fn check_compound_addition(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        type_map: &HashMap<String, String>,
    ) {
        if let Some(left) = node.child_by_field_name("left") {
            let left_type = self.infer_type(&left, source, type_map);

            // Skip pointer arithmetic: ptr += n
            if left_type == "not_applicable" {
                return;
            }

            if self.is_unsigned_type(&left_type) && !self.has_overflow_check_compound(node, source)
            {
                // Check for var += 1 bounded by enclosing loop condition (var < limit)
                if let Some(right) = node.child_by_field_name("right") {
                    let right_text = get_node_text(&right, source);
                    if right_text.trim() == "1" {
                        let var_name = get_node_text(&left, source);
                        if self.is_bounded_by_loop_condition(node, var_name.trim(), source) {
                            return;
                        }
                    }
                }

                // Skip if constant evaluation proves the result fits in 32-bit unsigned
                if self.compound_expr_fits_unsigned(node, source, "+", 32) {
                    return;
                }

                let start_point = node.start_position();
                let expr_text = get_node_text(node, source);

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

    fn check_compound_subtraction(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        type_map: &HashMap<String, String>,
    ) {
        if let Some(left) = node.child_by_field_name("left") {
            let left_type = self.infer_type(&left, source, type_map);

            // Skip pointer arithmetic: ptr -= n
            if left_type == "not_applicable" {
                return;
            }

            if self.is_unsigned_type(&left_type) && !self.has_overflow_check_compound(node, source)
            {
                // Check for var -= 1 with positive guard (var > expr implies var >= 1)
                if let Some(right) = node.child_by_field_name("right") {
                    let right_text = get_node_text(&right, source);
                    if right_text.trim() == "1" {
                        let var_name = get_node_text(&left, source);
                        if self.is_guarded_by_gt_zero(node, var_name.trim(), source) {
                            return;
                        }
                    }
                }

                // Skip if constant evaluation proves the result fits in 32-bit unsigned
                if self.compound_expr_fits_unsigned(node, source, "-", 32) {
                    return;
                }

                let start_point = node.start_position();
                let expr_text = get_node_text(node, source);

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

    fn check_compound_multiplication(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        type_map: &HashMap<String, String>,
    ) {
        if let Some(left) = node.child_by_field_name("left") {
            let left_type = self.infer_type(&left, source, type_map);

            if self.is_unsigned_type(&left_type) && !self.has_overflow_check_compound(node, source)
            {
                // Skip if constant evaluation proves the result fits in 32-bit unsigned
                if self.compound_expr_fits_unsigned(node, source, "*", 32) {
                    return;
                }

                let start_point = node.start_position();
                let expr_text = get_node_text(node, source);

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

    fn check_compound_left_shift(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        type_map: &HashMap<String, String>,
    ) {
        if let Some(left) = node.child_by_field_name("left") {
            let left_type = self.infer_type(&left, source, type_map);

            if self.is_unsigned_type(&left_type) && !self.has_overflow_check_compound(node, source)
            {
                // Skip if constant evaluation proves the result fits in 32-bit unsigned
                if self.compound_expr_fits_unsigned(node, source, "<<", 32) {
                    return;
                }

                let start_point = node.start_position();
                let expr_text = get_node_text(node, source);

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

    fn check_increment_decrement(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        type_map: &HashMap<String, String>,
    ) {
        if let Some(argument) = node.child_by_field_name("argument") {
            let arg_type = self.infer_type(&argument, source, type_map);

            if self.is_unsigned_type(&arg_type) {
                let operator = self.get_update_operator(node, source);
                if operator == "++" || operator == "--" {
                    // Skip increments/decrements in for-loop update clauses — the loop
                    // condition bounds the counter, making wrap impossible in practice.
                    if self.is_in_for_loop_update(node) {
                        return;
                    }
                    // Skip decrements guarded by `var > 0` or `0 < var` (FP-011).
                    if operator == "--" {
                        let var_name = get_node_text(&argument, source);
                        if self.is_guarded_by_gt_zero(node, var_name, source) {
                            return;
                        }
                    }
                    // Skip increments bounded by enclosing loop condition (var < limit).
                    if operator == "++" {
                        let var_name = get_node_text(&argument, source);
                        if self.is_bounded_by_loop_condition(node, var_name.trim(), source) {
                            return;
                        }
                    }
                    if !self.has_overflow_check_update(node, source) {
                        let start_point = node.start_position();
                        let expr_text = get_node_text(node, source);

                        let message = if operator == "++" {
                            format!(
                                "Unsigned integer increment '{}' may wrap at maximum value",
                                expr_text
                            )
                        } else {
                            format!(
                                "Unsigned integer decrement '{}' may wrap at zero",
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

    fn check_allocation_overflow(
        &self,
        node: &Node,
        source: &str,
        function_name: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let args = self.get_function_arguments(node, source);

        match function_name {
            "malloc" => {
                if !args.is_empty() && self.contains_multiplication(&args[0]) {
                    self.flag_allocation_overflow(
                        node,
                        source,
                        function_name,
                        &args[0],
                        violations,
                    );
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
                    self.flag_allocation_overflow(
                        node,
                        source,
                        function_name,
                        &args[1],
                        violations,
                    );
                }
            }
            _ => {}
        }
    }

    fn flag_allocation_overflow(
        &self,
        node: &Node,
        _source: &str,
        function_name: &str,
        size_arg: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
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

    fn infer_type(&self, node: &Node, source: &str, type_map: &HashMap<String, String>) -> String {
        let text = get_node_text(node, source);

        // Look for explicit unsigned indicators in the text
        if text.contains("unsigned") || text.contains("size_t") || text.contains("uint") {
            return "unsigned".to_string();
        }

        // Look for unsigned literals (suffix U or u)
        if text.ends_with("u")
            || text.ends_with("U")
            || text.ends_with("UL")
            || text.ends_with("ul")
        {
            return "unsigned".to_string();
        }

        // Look for unsigned constants
        if text.contains("UINT_MAX") || text.contains("SIZE_MAX") {
            return "unsigned".to_string();
        }

        // sizeof() always returns size_t (unsigned)
        if node.kind() == "sizeof_expression" {
            return "unsigned".to_string();
        }

        // Plain number literals — assume signed for conservatism
        if text.chars().all(|c| c.is_ascii_digit()) {
            return "int".to_string();
        }

        // Check identifiers against the type map (most reliable)
        if node.kind() == "identifier" {
            if let Some(declared_type) = type_map.get(text) {
                if self.is_unsigned_type(declared_type) {
                    return "unsigned".to_string();
                }
                // Non-integer types (float, double, char, pointers, structs) — not applicable
                if !declared_type.contains("int")
                    && !declared_type.contains("short")
                    && !declared_type.contains("long")
                    && declared_type != "signed"
                {
                    return "not_applicable".to_string();
                }
                return "int".to_string();
            }
        }

        // For pointer expressions, strip the '*' and check
        if node.kind() == "pointer_expression" {
            let var_name = text.trim_start_matches('*').trim();
            if let Some(declared_type) = type_map.get(var_name) {
                if self.is_unsigned_type(declared_type) {
                    return "unsigned".to_string();
                }
                return "int".to_string();
            }
        }

        // Fallback: check variable declaration in function text for unmapped variables
        if node.kind() == "identifier" || node.kind() == "pointer_expression" {
            let var_name = text.trim_start_matches('*').trim();
            if self.is_variable_declared_unsigned(node, source, var_name) {
                return "unsigned".to_string();
            }
        }

        "unknown".to_string()
    }

    /// Check if a variable is declared as unsigned in the containing function
    fn is_variable_declared_unsigned(&self, node: &Node, source: &str, var_name: &str) -> bool {
        // Find containing function
        let func = self.find_containing_function(node);
        if func.is_none() {
            return false;
        }
        let func = func.unwrap();
        let func_text = get_node_text(&func, source);

        // Check for parameter declarations like "unsigned int var_name" or "unsigned int *var_name"
        if func_text.contains(&format!("unsigned int {}", var_name))
            || func_text.contains(&format!("unsigned int *{}", var_name))
            || func_text.contains(&format!("unsigned long {}", var_name))
            || func_text.contains(&format!("size_t {}", var_name))
            || func_text.contains(&format!("uint32_t {}", var_name))
            || func_text.contains(&format!("uint64_t {}", var_name))
        {
            return true;
        }

        false
    }

    /// Find the containing function definition
    fn find_containing_function<'a>(&self, node: &Node<'a>) -> Option<Node<'a>> {
        let mut current = *node;
        while let Some(parent) = current.parent() {
            if parent.kind() == "function_definition" {
                return Some(parent);
            }
            current = parent;
        }
        None
    }

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

    fn is_unsigned_type(&self, type_str: &str) -> bool {
        type_str.contains("unsigned") || type_str == "size_t" || type_str.contains("uint")
    }

    fn has_overflow_check_addition(&self, node: &Node, source: &str) -> bool {
        // Look for UINT_MAX - a < b pattern (precondition) or result < a (postcondition)
        self.has_function_context_check(node, source, &["UINT_MAX", " - ", " < "])
            || self.has_function_context_check(node, source, &["SIZE_MAX", " - ", " < "])
            || self.has_function_context_check(node, source, &["== UINT_MAX"])
            || self.has_postcondition_check(node, source)
            || self.uses_wider_type(node, source)
            || self.is_inside_checked_block(node, source)
    }

    fn has_overflow_check_subtraction(&self, node: &Node, source: &str) -> bool {
        // Look for if (a < b) or postcondition if (result > a) pattern
        // Note: We need an actual if check before or after the subtraction
        self.has_subtraction_precondition(node, source)
            || self.has_postcondition_check(node, source)
            || self.is_inside_checked_block(node, source)
            || self.is_subtract_one_guarded(node, source)
    }

    fn has_overflow_check_multiplication(&self, node: &Node, source: &str) -> bool {
        // Look for a > MAX / b pattern in containing function
        self.has_function_context_check(node, source, &["UINT_MAX", " / "])
            || self.has_function_context_check(node, source, &["SIZE_MAX", " / "])
            || self.has_preceding_overflow_check(node, source)
            || self.uses_wider_type(node, source)
            || self.is_inside_checked_block(node, source)
    }

    /// Check if there's an overflow check in the code preceding this node
    fn has_preceding_overflow_check(&self, node: &Node, source: &str) -> bool {
        // Get text before this node in the translation unit
        let node_start = node.start_byte();
        if node_start > 0 {
            let preceding_text = &source[..node_start];
            // Look for SIZE_MAX/UINT_MAX division check patterns
            if (preceding_text.contains("SIZE_MAX /") || preceding_text.contains("UINT_MAX /"))
                && preceding_text.contains("if")
            {
                return true;
            }
        }
        false
    }

    fn has_shift_overflow_check(&self, node: &Node, source: &str) -> bool {
        // Look for shift amount validation
        self.has_function_context_check(node, source, &["sizeof"])
            || self.is_inside_checked_block(node, source)
    }

    fn has_overflow_check_compound(&self, node: &Node, source: &str) -> bool {
        // Look for any overflow checking pattern
        self.has_function_context_check(node, source, &["if", "UINT_MAX"])
            || self.has_function_context_check(node, source, &["if", "SIZE_MAX"])
            || self.is_inside_checked_block(node, source)
    }

    fn has_overflow_check_update(&self, node: &Node, source: &str) -> bool {
        // Look for bounds checking around increment/decrement - must be explicit UINT_MAX or == 0
        self.has_function_context_check(node, source, &["if", "UINT_MAX"])
            || self.has_function_context_check(node, source, &["if", "== 0"])
            || self.is_inside_checked_block(node, source)
    }

    fn has_calloc_overflow_check(&self, node: &Node, source: &str) -> bool {
        // Look for calloc-specific overflow checking
        self.has_function_context_check(node, source, &["SIZE_MAX", " / "])
            || self.is_inside_checked_block(node, source)
    }

    fn has_function_context_check(&self, node: &Node, source: &str, patterns: &[&str]) -> bool {
        // Look in the containing function for overflow checking patterns
        if let Some(func) = self.find_containing_function(node) {
            let func_text = get_node_text(&func, source);
            return patterns.iter().all(|pattern| func_text.contains(pattern));
        }
        false
    }

    /// Check for subtraction precondition (if (a < b) before subtraction)
    fn has_subtraction_precondition(&self, node: &Node, source: &str) -> bool {
        // Look for if statement before the subtraction that compares the operands
        if let Some(func) = self.find_containing_function(node) {
            let func_text = get_node_text(&func, source);
            // Look for typical precondition pattern
            if func_text.contains("if (ui_a < ui_b)")
                || func_text.contains("if (a < b)")
                || func_text.contains("if(ui_a < ui_b)")
                || func_text.contains("if(a < b)")
            {
                return true;
            }
        }
        false
    }

    /// Check for postcondition check (if (result < original) or if (result > original))
    fn has_postcondition_check(&self, node: &Node, source: &str) -> bool {
        if let Some(func) = self.find_containing_function(node) {
            let func_text = get_node_text(&func, source);
            // Look for postcondition patterns like "if (usum < ui_a)" or "if (udiff > ui_a)"
            if func_text.contains("if (usum < ")
                || func_text.contains("if (udiff > ")
                || func_text.contains("if(usum < ")
                || func_text.contains("if(udiff > ")
                || func_text.contains("if (result < ")
                || func_text.contains("if (result > ")
            {
                return true;
            }
        }
        false
    }

    /// Check if operation uses wider type casting for safety
    fn uses_wider_type(&self, node: &Node, source: &str) -> bool {
        // Check parent for cast to wider type
        if let Some(parent) = node.parent() {
            let parent_text = get_node_text(&parent, source);
            if parent_text.contains("(uint64_t)")
                || parent_text.contains("(unsigned long long)")
                || parent_text.contains("(int64_t)")
            {
                return true;
            }
        }
        // Also check if operands are cast to wider type
        let node_text = get_node_text(node, source);
        if node_text.contains("(uint64_t)") || node_text.contains("(unsigned long long)") {
            return true;
        }
        false
    }

    /// Extract operand identifier names from a binary/assignment/update expression node.
    fn extract_operand_names(&self, node: &Node, source: &str) -> Vec<String> {
        let mut names = Vec::new();
        if let Some(left) = node.child_by_field_name("left") {
            Self::collect_identifiers(&left, source, &mut names);
        }
        if let Some(right) = node.child_by_field_name("right") {
            Self::collect_identifiers(&right, source, &mut names);
        }
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

    /// Check if the operation is inside an if-else block guarded by an unsigned type-limit macro.
    /// Operand-aware: the if-condition must reference at least one operand of the arithmetic.
    fn is_inside_checked_block(&self, node: &Node, source: &str) -> bool {
        const UNSIGNED_LIMIT_MACROS: &[&str] = &[
            "UINT_MAX",
            "SIZE_MAX",
            "UINT32_MAX",
            "UCHAR_MAX",
            "USHRT_MAX",
            "UINT8_MAX",
            "UINT16_MAX",
            "UINT64_MAX",
            "ULONG_MAX",
            "ULLONG_MAX",
        ];

        let op_names = self.extract_operand_names(node, source);
        let mut current = *node;
        while let Some(parent) = current.parent() {
            if parent.kind() == "if_statement" {
                if let Some(condition) = parent.child_by_field_name("condition") {
                    let cond_text = get_node_text(&condition, source);
                    let has_limit = UNSIGNED_LIMIT_MACROS
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
            // Stop at function boundary
            if parent.kind() == "function_definition" {
                break;
            }
            current = parent;
        }
        false
    }

    #[allow(dead_code)]
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
        let text = get_node_text(node, source);
        if text.contains("++") {
            "++".to_string()
        } else if text.contains("--") {
            "--".to_string()
        } else {
            "unknown".to_string()
        }
    }

    /// Check if a decrement/subtraction is inside a block guarded by a positive-value condition.
    /// Patterns: `if (var > 0)`, `while (var > expr)`, `for (...; var > expr; ...)`.
    /// For unsigned types, `var > expr` implies `var >= 1`, making `var--` or `var - 1` safe.
    fn is_guarded_by_gt_zero(&self, node: &Node, var_name: &str, source: &str) -> bool {
        let mut current = *node;
        while let Some(parent) = current.parent() {
            if matches!(
                parent.kind(),
                "if_statement" | "while_statement" | "for_statement"
            ) {
                if let Some(condition) = parent.child_by_field_name("condition") {
                    let cond_text = get_node_text(&condition, source);
                    if self.condition_implies_positive(cond_text, var_name) {
                        return true;
                    }
                }
            }
            if parent.kind() == "function_definition" {
                break;
            }
            current = parent;
        }
        false
    }

    /// Check if a condition text implies var_name > 0 (i.e., var is positive).
    /// Handles compound conditions (&&/||) via substring matching.
    /// Recognizes `var > expr` (any lower bound, not just zero) since for unsigned
    /// types, `var > expr` always implies `var >= 1`.
    fn condition_implies_positive(&self, cond_text: &str, var_name: &str) -> bool {
        let cond = cond_text.trim();
        let cond = if cond.starts_with('(') && cond.ends_with(')') {
            &cond[1..cond.len() - 1]
        } else {
            cond
        };
        let cond = cond.trim();

        // Quick check: does the condition mention the variable as a whole word?
        if !self.contains_word(cond, var_name) {
            return false;
        }

        // Pattern 1: var > expr (strict GT — implies var >= 1 for unsigned)
        // Covers: var > 0, var > idx, var > some_expr, etc.
        let gt_pat = format!("{} > ", var_name);
        if cond.contains(&gt_pat) {
            return true;
        }

        // Pattern 2: var != 0 or 0 != var (explicit non-zero check)
        let neq_zero = format!("{} != 0", var_name);
        let zero_neq = format!("0 != {}", var_name);
        if cond.contains(&neq_zero) || cond.contains(&zero_neq) {
            return true;
        }

        // Pattern 3: expr < var (reverse of var > expr — strict LT)
        let lt_rev = format!("< {}", var_name);
        if cond.contains(&lt_rev) {
            for (pos, _) in cond.match_indices(&lt_rev) {
                // Exclude << (shift) and <= (less-than-or-equal)
                let prev = if pos > 0 { cond.as_bytes()[pos - 1] } else { 0 };
                if prev != b'<' && prev != b'=' {
                    return true;
                }
            }
        }

        false
    }

    /// For "var - 1" subtraction: if var is guarded by a positive-value condition,
    /// then var >= 1 and var - 1 >= 0, so no unsigned wrap.
    fn is_subtract_one_guarded(&self, node: &Node, source: &str) -> bool {
        if let Some(right) = node.child_by_field_name("right") {
            let right_text = get_node_text(&right, source);
            if right_text.trim() == "1" {
                if let Some(left) = node.child_by_field_name("left") {
                    let var_name = get_node_text(&left, source);
                    return self.is_guarded_by_gt_zero(node, var_name.trim(), source);
                }
            }
        }
        false
    }

    /// Check if a node is inside the update clause of a for-loop.
    /// For-loop update increments (i++) are bounded by the loop condition,
    /// making unsigned wrap impossible in normal usage.
    fn is_in_for_loop_update(&self, node: &Node) -> bool {
        let mut current = Some(*node);
        while let Some(n) = current {
            if let Some(parent) = n.parent() {
                if parent.kind() == "for_statement" {
                    // Check if this node is in the update clause
                    // The for_statement fields: initializer, condition, update, body
                    if let Some(update) = parent.child_by_field_name("update") {
                        if self.node_contains(&update, &n) {
                            return true;
                        }
                    }
                }
                current = Some(parent);
            } else {
                break;
            }
        }
        false
    }

    /// Check if parent node contains child (by byte range).
    fn node_contains(&self, parent: &Node, child: &Node) -> bool {
        child.start_byte() >= parent.start_byte() && child.end_byte() <= parent.end_byte()
    }

    /// For binary "var + 1" or "1 + var": if var is bounded by an enclosing loop
    /// condition (var < limit), then var + 1 <= limit <= UINT_MAX, so no wrap.
    fn is_add_one_bounded_by_loop(&self, node: &Node, source: &str) -> bool {
        if let (Some(left), Some(right)) = (
            node.child_by_field_name("left"),
            node.child_by_field_name("right"),
        ) {
            let left_text = get_node_text(&left, source);
            let right_text = get_node_text(&right, source);

            // Check "var + 1" pattern
            if right_text.trim() == "1" {
                return self.is_bounded_by_loop_condition(node, left_text.trim(), source);
            }
            // Check "1 + var" pattern
            if left_text.trim() == "1" {
                return self.is_bounded_by_loop_condition(node, right_text.trim(), source);
            }
        }
        false
    }

    /// Check if var_name is bounded by an enclosing loop condition.
    /// Detects `while (var < limit)` and `for (...; var < limit; ...)` patterns.
    /// Inside the loop body, var < limit, so var + 1 <= limit <= UINT_MAX.
    fn is_bounded_by_loop_condition(&self, node: &Node, var_name: &str, source: &str) -> bool {
        let mut current = *node;
        while let Some(parent) = current.parent() {
            if matches!(parent.kind(), "while_statement" | "for_statement") {
                if let Some(condition) = parent.child_by_field_name("condition") {
                    let cond_text = get_node_text(&condition, source);
                    if self.condition_implies_upper_bound(&cond_text, var_name) {
                        return true;
                    }
                }
            }
            if parent.kind() == "function_definition" {
                break;
            }
            current = parent;
        }
        false
    }

    /// Check if a condition implies var < some_limit (upper bound).
    /// Recognizes: `var < expr`, `var <= expr`, `expr > var`, `expr >= var`.
    fn condition_implies_upper_bound(&self, cond_text: &str, var_name: &str) -> bool {
        let cond = cond_text.trim();
        let cond = if cond.starts_with('(') && cond.ends_with(')') {
            &cond[1..cond.len() - 1]
        } else {
            cond
        };
        let cond = cond.trim();

        if !self.contains_word(cond, var_name) {
            return false;
        }

        // Pattern 1: var < expr or var <= expr
        let lt_pat = format!("{} < ", var_name);
        let le_pat = format!("{} <= ", var_name);
        if cond.contains(&lt_pat) || cond.contains(&le_pat) {
            return true;
        }

        // Pattern 2: expr > var or expr >= var
        let gt_rev = format!("> {}", var_name);
        if cond.contains(&gt_rev) {
            // Exclude >> (shift)
            for (pos, _) in cond.match_indices(&gt_rev) {
                let prev = if pos > 0 { cond.as_bytes()[pos - 1] } else { 0 };
                if prev != b'>' {
                    return true;
                }
            }
        }

        false
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

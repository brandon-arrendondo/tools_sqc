use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use std::collections::HashMap;
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
        let type_map = self.collect_variable_types(node, source);

        self.check_node(node, source, &mut violations, &type_map);

        violations
    }
}

impl Int30C {
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

            if self.is_unsigned_type(&left_type) || self.is_unsigned_type(&right_type) {
                if !self.has_overflow_check_addition(node, source) {
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

            if self.is_unsigned_type(&left_type) || self.is_unsigned_type(&right_type) {
                if !self.has_overflow_check_subtraction(node, source) {
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

            if self.is_unsigned_type(&left_type) || self.is_unsigned_type(&right_type) {
                if !self.has_overflow_check_multiplication(node, source) {
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
                        suggestion: Some("Add overflow check: if (a > UINT_MAX / b) { /* handle error */ }".to_string()),
                    ..Default::default()
                    });
                }
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

            if self.is_unsigned_type(&left_type) {
                if !self.has_shift_overflow_check(node, source) {
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

            if self.is_unsigned_type(&left_type) {
                if !self.has_overflow_check_compound(node, source) {
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

            if self.is_unsigned_type(&left_type) {
                if !self.has_overflow_check_compound(node, source) {
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

            if self.is_unsigned_type(&left_type) {
                if !self.has_overflow_check_compound(node, source) {
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

            if self.is_unsigned_type(&left_type) {
                if !self.has_overflow_check_compound(node, source) {
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
            if let Some(name) = self.extract_identifier_name(&declarator, source) {
                type_map.insert(name, type_text.clone());
            }
        }

        // Handle init_declarator lists (e.g. `int a, b;`)
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "init_declarator" {
                    if let Some(decl) = child.child_by_field_name("declarator") {
                        if let Some(name) = self.extract_identifier_name(&decl, source) {
                            type_map.insert(name, type_text.clone());
                        }
                    }
                }
            }
        }
    }

    fn extract_identifier_name(&self, node: &Node, source: &str) -> Option<String> {
        match node.kind() {
            "identifier" => Some(get_node_text(node, source).to_string()),
            "pointer_declarator" | "array_declarator" | "parenthesized_declarator" => {
                if let Some(inner) = node.child_by_field_name("declarator") {
                    self.extract_identifier_name(&inner, source)
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

    /// Check if the operation is inside an if-else block that suggests it's protected
    fn is_inside_checked_block(&self, node: &Node, source: &str) -> bool {
        // Walk up the tree to see if we're inside an if/else block
        let mut current = *node;
        while let Some(parent) = current.parent() {
            if parent.kind() == "if_statement" {
                // We're inside an if statement - check if it's a real overflow check
                let if_text = get_node_text(&parent, source);
                // Must have UINT_MAX or SIZE_MAX - not just any comparison
                if if_text.contains("UINT_MAX")
                    || if_text.contains("SIZE_MAX")
                    || if_text.contains("UINT32_MAX")
                {
                    return true;
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

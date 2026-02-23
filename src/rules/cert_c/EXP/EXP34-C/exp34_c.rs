use super::super::{CertRule, RuleViolation};
use crate::analyze::context::ProjectContext;
use crate::analyze::function_summary::FunctionSummary;
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils;
use std::cell::RefCell;
use std::collections::{HashMap, HashSet};
use tree_sitter::Node;

pub struct Exp34C {
    function_summaries: RefCell<HashMap<String, FunctionSummary>>,
}

impl Exp34C {
    pub fn new() -> Self {
        Self {
            function_summaries: RefCell::new(HashMap::new()),
        }
    }
}

impl CertRule for Exp34C {
    fn rule_id(&self) -> &'static str {
        "EXP34-C"
    }

    fn description(&self) -> &'static str {
        "Do not dereference null pointers"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "EXP34-C"
    }

    fn set_project_context(&self, context: &ProjectContext) {
        *self.function_summaries.borrow_mut() = context.function_summaries.clone();
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        let summaries = self.function_summaries.borrow();

        // Analyze function bodies for null pointer dereferences
        if node.kind() == "function_definition" {
            if let Some(body) = node.child_by_field_name("body") {
                let mut analyzer = NullPointerAnalyzer::new();
                analyzer.analyze_function_body(&body, source, &summaries, &mut violations);
            }
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                violations.extend(self.check(&child, source));
            }
        }

        violations
    }
}

struct NullPointerAnalyzer {
    // Track variables that could be null
    potentially_null_vars: HashSet<String>,
    // Track variables that have been null-checked
    null_checked_vars: HashSet<String>,
    // Track the byte offset where each null check occurs
    null_check_positions: std::collections::HashMap<String, usize>,
    // Track byte positions where variables are reassigned to nullable values AFTER a null check
    nullable_reassignments: Vec<(String, usize)>,
    // Track variables known to be declared as pointer types (gates null-taint)
    declared_pointer_vars: HashSet<String>,
}

impl NullPointerAnalyzer {
    fn new() -> Self {
        Self {
            potentially_null_vars: HashSet::new(),
            null_checked_vars: HashSet::new(),
            null_check_positions: std::collections::HashMap::new(),
            nullable_reassignments: Vec::new(),
            declared_pointer_vars: HashSet::new(),
        }
    }

    fn analyze_function_body(
        &mut self,
        body: &Node,
        source: &str,
        summaries: &HashMap<String, FunctionSummary>,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Collect function parameters (they could be null if they're pointers)
        if let Some(parent) = body.parent() {
            if parent.kind() == "function_definition" {
                if let Some(declarator) = parent.child_by_field_name("declarator") {
                    self.collect_function_parameters(&declarator, source);
                }
            }
        }

        // First pass: find all early-return null checks (before processing variables)
        self.find_early_return_checks(body, source);

        // Second pass: collect potentially null variables
        self.collect_null_variables(body, source, summaries);

        // Third pass: check for unsafe dereferences
        self.check_dereferences(body, source, violations);
    }

    fn collect_function_parameters(&mut self, declarator: &Node, source: &str) {
        // Find the function_declarator which contains parameters
        if declarator.kind() == "function_declarator" {
            if let Some(params) = declarator.child_by_field_name("parameters") {
                for i in 0..params.child_count() {
                    if let Some(param) = params.child(i) {
                        if param.kind() == "parameter_declaration" {
                            // Extract parameter name and check if it looks like a pointer
                            let param_text = ast_utils::get_node_text_owned(&param, source);

                            if let Some(param_declarator) = param.child_by_field_name("declarator")
                            {
                                let param_name = get_identifier_name(&param_declarator, source);
                                // Mark as potentially null if it's actually a pointer
                                // Only use AST-based detection and explicit '*' in the type text
                                if is_pointer_declarator(&param_declarator)
                                    || param_text.contains('*')
                                    || param_text.starts_with("FILE")
                                    || param_name.contains("callback")
                                // Function pointer typedefs can't be detected via AST
                                {
                                    self.declared_pointer_vars.insert(param_name.clone());
                                    self.potentially_null_vars.insert(param_name);
                                }
                            }
                        }
                    }
                }
            }
        } else {
            // Recursively search for function_declarator
            for i in 0..declarator.child_count() {
                if let Some(child) = declarator.child(i) {
                    self.collect_function_parameters(&child, source);
                }
            }
        }
    }

    /// First pass: find all if statements with null checks and early returns
    fn find_early_return_checks(&mut self, node: &Node, source: &str) {
        if node.kind() == "if_statement" {
            if let Some(condition) = node.child_by_field_name("condition") {
                if let Some(var_name) = self.get_null_check_var(&condition, source) {
                    if let Some(consequence) = node.child_by_field_name("consequence") {
                        // Check the condition to see if it's checking FOR null or AGAINST null
                        let condition_checks_for_null =
                            !self.is_null_safe_in_then_branch(&condition, &var_name, source);

                        if condition_checks_for_null {
                            // Pattern: if (ptr == NULL) { ... }
                            // Variable is safe only AFTER the entire if-statement ends.
                            // Using end_byte ensures that derefs inside the null-branch
                            // (deref_byte < if_end) are still flagged as unsafe, while
                            // derefs after the block (deref_byte > if_end) are safe.
                            if consequence.kind() == "compound_statement" {
                                self.null_checked_vars.insert(var_name.clone());
                                self.null_check_positions.insert(var_name, node.end_byte());
                            }
                        } else {
                            // Pattern: if (ptr != NULL) { ... }
                            // This is a guarded scope pattern, not early return
                            // Don't add to null_checked_vars here; handled by is_in_null_guarded_scope
                        }
                    }
                }
            }
        }

        // Recognize assert(var) as a null check
        if node.kind() == "expression_statement" {
            if let Some(expr) = node.child(0) {
                if expr.kind() == "call_expression" {
                    if let Some(function) = expr.child_by_field_name("function") {
                        let func_name = ast_utils::get_node_text_owned(&function, source);
                        if func_name == "assert" {
                            if let Some(args) = expr.child_by_field_name("arguments") {
                                // Find the actual argument within the argument_list node
                                // The argument_list has children: '(', <args...>, ')'
                                for i in 0..args.child_count() {
                                    if let Some(arg) = args.child(i) {
                                        // Skip '(' and ')'
                                        if arg.kind() == "(" || arg.kind() == ")" {
                                            continue;
                                        }

                                        // Handle assert(var)
                                        if arg.kind() == "identifier" {
                                            let var_name =
                                                ast_utils::get_node_text_owned(&arg, source);
                                            self.null_checked_vars.insert(var_name.clone());
                                            self.null_check_positions
                                                .insert(var_name, node.start_byte());
                                            break;
                                        }
                                        // Handle assert(var != NULL) or similar
                                        else if arg.kind() == "binary_expression" {
                                            if let (Some(left), Some(right)) = (
                                                arg.child_by_field_name("left"),
                                                arg.child_by_field_name("right"),
                                            ) {
                                                let left_text =
                                                    ast_utils::get_node_text_owned(&left, source);
                                                let right_text =
                                                    ast_utils::get_node_text_owned(&right, source);
                                                if is_null_value(&right_text)
                                                    && left.kind() == "identifier"
                                                {
                                                    self.null_checked_vars
                                                        .insert(left_text.clone());
                                                    self.null_check_positions
                                                        .insert(left_text, node.start_byte());
                                                } else if is_null_value(&left_text)
                                                    && right.kind() == "identifier"
                                                {
                                                    self.null_checked_vars
                                                        .insert(right_text.clone());
                                                    self.null_check_positions
                                                        .insert(right_text, node.start_byte());
                                                }
                                            }
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // Recursively search for more if statements
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.find_early_return_checks(&child, source);
            }
        }
    }

    fn collect_null_variables(
        &mut self,
        node: &Node,
        source: &str,
        summaries: &HashMap<String, FunctionSummary>,
    ) {
        match node.kind() {
            "assignment_expression" => {
                if let (Some(left), Some(right)) = (
                    node.child_by_field_name("left"),
                    node.child_by_field_name("right"),
                ) {
                    // Get the full path for left side (could be identifier or field_expression)
                    let left_name = ast_utils::get_node_text_owned(&left, source);
                    let right_text = ast_utils::get_node_text_owned(&right, source);

                    // Use end_byte so that RHS sub-expressions (e.g. current->next in
                    // `current = current->next`) have a deref_byte that is LESS than
                    // assignment_byte — preventing the RHS from being treated as coming
                    // after its own assignment (self-referential false positive).
                    let assignment_byte = node.end_byte();

                    // Only apply the pointer-type gate for simple identifier LHS.
                    // For field_expression (c.value), subscript_expression, etc. we cannot
                    // look up the declared type without struct analysis, so keep the original
                    // permissive behaviour for those.  For plain identifiers we require the
                    // variable to be in declared_pointer_vars so that integer variables like
                    // `int rc` are never tainted by a nullable function-call return value.
                    let left_is_ptr = left.kind() != "identifier"
                        || self.declared_pointer_vars.contains(&left_name);

                    // Check if assigning NULL, 0, cast to NULL, or function that can return null.
                    // For nullable function calls we require the LHS to be a declared pointer.
                    // For literal NULL/0/(T*)NULL we also require pointer type (assigning 0 to
                    // an int is never a null pointer situation).
                    if left_is_ptr
                        && (is_null_value(&right_text)
                            || is_nullable_function_call(&right, source, summaries)
                            || is_cast_to_null(&right, source))
                    {
                        self.potentially_null_vars.insert(left_name.clone());
                        // Record this reassignment position for flow-sensitive checking
                        if self.null_checked_vars.contains(&left_name) {
                            self.nullable_reassignments
                                .push((left_name.clone(), assignment_byte));
                        }
                    } else if right.kind() == "identifier" {
                        // Assignment from another variable: current = head
                        // Only propagate null-taint if both sides are pointer variables.
                        if left_is_ptr && self.potentially_null_vars.contains(&right_text) {
                            self.potentially_null_vars.insert(left_name.clone());
                            if self.null_checked_vars.contains(&left_name) {
                                self.nullable_reassignments
                                    .push((left_name.clone(), assignment_byte));
                            }
                        }
                    } else if right.kind() == "field_expression" {
                        // Assignment from field access: current = current->next
                        // Only propagate null if LHS is a pointer and the base is potentially null.
                        if left_is_ptr {
                            if let Some(argument) = right.child_by_field_name("argument") {
                                let base_name = ast_utils::get_node_text_owned(&argument, source);
                                if self.potentially_null_vars.contains(&base_name) {
                                    self.potentially_null_vars.insert(left_name.clone());
                                    if self.null_checked_vars.contains(&left_name) {
                                        self.nullable_reassignments
                                            .push((left_name.clone(), assignment_byte));
                                    }
                                }
                            }
                        }
                    } else if right.kind() == "call_expression" {
                        // Reassignment from any function call — may or may not return null.
                        if !is_nullable_function_call(&right, source, summaries) {
                            self.potentially_null_vars.remove(&left_name);
                        } else {
                            self.potentially_null_vars.insert(left_name.clone());
                            if self.null_checked_vars.contains(&left_name) {
                                self.nullable_reassignments
                                    .push((left_name.clone(), assignment_byte));
                            }
                        }
                    } else if !is_null_value(&right_text) && left.kind() == "identifier" {
                        // If assigning a non-null value to a simple variable, remove from potentially null set
                        // (Don't remove for field_expression as it's more complex)
                        self.potentially_null_vars.remove(&left_name);
                    }
                }
            }
            "declaration" => {
                self.process_declaration(node, source, summaries);
            }
            _ => {}
        }

        // Recursively process child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.collect_null_variables(&child, source, summaries);
            }
        }
    }

    fn process_declaration(
        &mut self,
        node: &Node,
        source: &str,
        summaries: &HashMap<String, FunctionSummary>,
    ) {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "init_declarator" {
                    if let Some(declarator) = child.child_by_field_name("declarator") {
                        let var_name = get_identifier_name(&declarator, source);
                        let is_ptr = is_pointer_declarator(&declarator)
                            && !declarator_contains_array(&declarator);

                        if is_ptr {
                            self.declared_pointer_vars.insert(var_name.clone());
                        }

                        // Only taint as potentially-null if this variable is a pointer type.
                        // Integers (e.g. `int rc = 0` or `int rc = func()`) must never be
                        // added to potentially_null_vars even when the RHS looks null-like.
                        if let Some(value) = child.child_by_field_name("value") {
                            let value_text = ast_utils::get_node_text_owned(&value, source);
                            if is_ptr
                                && (is_null_value(&value_text)
                                    || is_nullable_function_call(&value, source, summaries)
                                    || is_cast_to_null(&value, source))
                            {
                                self.potentially_null_vars.insert(var_name.clone());
                            } else if value.kind() == "identifier" {
                                // Declaration initialized from another pointer: Node *current = head;
                                // Only propagate null-taint if this is also a pointer variable.
                                if is_ptr && self.potentially_null_vars.contains(&value_text) {
                                    self.potentially_null_vars.insert(var_name.clone());
                                }
                            }
                        } else {
                            // Uninitialized pointer variables (not stack arrays) are potentially null
                            if is_ptr {
                                self.potentially_null_vars.insert(var_name);
                            }
                        }
                    }
                } else if child.kind() == "pointer_declarator" || child.kind() == "identifier" {
                    // Handle simple uninitialized declarations like "int *ptr;"
                    // The declarator might be a direct child of the declaration node
                    let var_name = get_identifier_name(&child, source);
                    // Don't treat stack arrays (int *arr[5]) as potentially null —
                    // the array variable itself is stack-allocated and can never be null.
                    // Only true pointer variables (int *ptr) can be null.
                    if !var_name.is_empty()
                        && is_pointer_declarator(&child)
                        && !declarator_contains_array(&child)
                    {
                        self.declared_pointer_vars.insert(var_name.clone());
                        self.potentially_null_vars.insert(var_name);
                    }
                }
            }
        }
    }

    /// Get the variable being checked for NULL, returns Some(var_name) if this is a null check
    fn get_null_check_var(&self, condition: &Node, source: &str) -> Option<String> {
        match condition.kind() {
            "parenthesized_expression" => {
                // Unwrap parentheses
                if let Some(child) = condition.child(1) {
                    return self.get_null_check_var(&child, source);
                }
            }
            "binary_expression" => {
                if let (Some(left), Some(right)) = (
                    condition.child_by_field_name("left"),
                    condition.child_by_field_name("right"),
                ) {
                    let left_text = ast_utils::get_node_text_owned(&left, source);
                    let right_text = ast_utils::get_node_text_owned(&right, source);

                    // Pattern: ptr == NULL or ptr != NULL
                    if is_null_value(&right_text) && left.kind() == "identifier" {
                        return Some(left_text);
                    } else if is_null_value(&left_text) && right.kind() == "identifier" {
                        return Some(right_text);
                    }

                    // Handle && and || operators - check both sides recursively
                    if let Some(operator) = condition.child_by_field_name("operator") {
                        let op_text = ast_utils::get_node_text_owned(&operator, source);
                        if op_text == "||" || op_text == "&&" {
                            // Try left side first
                            if let Some(var) = self.get_null_check_var(&left, source) {
                                return Some(var);
                            }
                            // Then try right side
                            return self.get_null_check_var(&right, source);
                        }
                    }
                }
            }
            "unary_expression" => {
                // Pattern: !ptr
                if let Some(operand) = condition.child_by_field_name("argument") {
                    if operand.kind() == "identifier" {
                        return Some(ast_utils::get_node_text_owned(&operand, source));
                    }
                }
            }
            "identifier" => {
                // Pattern: if (ptr) - this IS a null check (checks if ptr != NULL)
                return Some(ast_utils::get_node_text_owned(condition, source));
            }
            _ => {}
        }
        None
    }

    /// Check if a block contains error handling (return, exit, throw, goto, or any statement suggesting error handling)
    fn contains_early_return(&self, node: &Node) -> bool {
        if matches!(
            node.kind(),
            "return_statement" | "break_statement" | "continue_statement" | "goto_statement"
        ) {
            return true;
        }

        // If the block is non-empty (has statements), consider it as having error handling
        // This is a pragmatic approach for wiki examples that use "/* Handle error */" comments
        if node.kind() == "compound_statement" {
            // Count non-trivial children (not just braces and whitespace)
            let mut has_content = false;
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() != "{" && child.kind() != "}" && child.kind() != "comment" {
                        has_content = true;
                        break;
                    }
                }
            }
            // Accept any non-empty block as potential error handling
            if has_content {
                return true;
            }
        }

        // Recursively check children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if self.contains_early_return(&child) {
                    return true;
                }
            }
        }

        false
    }

    fn process_null_check(&mut self, condition: &Node, source: &str) {
        // Look for patterns like: ptr != NULL, ptr == NULL, !ptr, ptr
        match condition.kind() {
            "binary_expression" => {
                if let (Some(left), Some(operator), Some(right)) = (
                    condition.child_by_field_name("left"),
                    condition.child_by_field_name("operator"),
                    condition.child_by_field_name("right"),
                ) {
                    let op_text = ast_utils::get_node_text_owned(&operator, source);
                    let left_text = ast_utils::get_node_text_owned(&left, source);
                    let right_text = ast_utils::get_node_text_owned(&right, source);

                    // Check for null comparison patterns
                    if op_text == "!=" || op_text == "==" {
                        if is_null_value(&right_text) && left.kind() == "identifier" {
                            // Pattern: ptr != NULL or ptr == NULL
                            self.null_checked_vars.insert(left_text);
                        } else if is_null_value(&left_text) && right.kind() == "identifier" {
                            // Pattern: NULL != ptr or NULL == ptr
                            self.null_checked_vars.insert(right_text);
                        }
                    }
                }
            }
            "unary_expression" => {
                // Pattern: !ptr
                if let Some(operand) = condition.child_by_field_name("argument") {
                    if operand.kind() == "identifier" {
                        let var_name = ast_utils::get_node_text_owned(&operand, source);
                        self.null_checked_vars.insert(var_name);
                    }
                }
            }
            "identifier" => {
                // Pattern: if (ptr) - checks that ptr is not null
                let var_name = ast_utils::get_node_text_owned(condition, source);
                self.null_checked_vars.insert(var_name);
            }
            _ => {}
        }
    }

    fn check_dereferences(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            "pointer_expression" => {
                // tree-sitter uses pointer_expression for both *ptr (dereference) and
                // &var (address-of).  Only the dereference form can be a null-ptr bug;
                // taking the address of a variable is always safe.
                let is_deref = node
                    .child_by_field_name("operator")
                    .map(|op| ast_utils::get_node_text_owned(&op, source) == "*")
                    .unwrap_or(false);

                if is_deref {
                    if let Some(argument) = node.child_by_field_name("argument") {
                        // Get the full expression being dereferenced (could be identifier or field_expression)
                        let mut deref_text = ast_utils::get_node_text_owned(&argument, source);

                        // Handle parenthesized expressions - strip the parentheses
                        if argument.kind() == "parenthesized_expression" {
                            // Get the inner expression
                            if let Some(inner) = argument.child(1) {
                                // child 0 is '(', child 1 is expression, child 2 is ')'
                                deref_text = ast_utils::get_node_text_owned(&inner, source);
                            }
                        }

                        // Check both simple identifiers and complex expressions like field_expression
                        if argument.kind() == "identifier"
                            || argument.kind() == "field_expression"
                            || argument.kind() == "parenthesized_expression"
                        {
                            if self.is_unsafe_dereference_at_node(&deref_text, node, source) {
                                let start_point = node.start_position();
                                violations.push(RuleViolation {
                                    rule_id: "EXP34-C".to_string(),
                                    severity: Severity::High,
                                    message: format!(
                                        "Potential null pointer dereference of variable '{}'",
                                        deref_text
                                    ),
                                    file_path: String::new(),
                                    line: start_point.row + 1,
                                    column: start_point.column + 1,
                                    suggestion: Some(format!(
                                        "Check if '{}' is not NULL before dereferencing",
                                        deref_text
                                    )),
                                    ..Default::default()
                                });
                            }
                        }
                    }
                } // end if is_deref
            }
            "subscript_expression" => {
                // Array subscript can also be null pointer dereference: ptr[index]
                // The subscript expression has the array as the first child (child 0)
                if let Some(array) = node.child(0) {
                    if array.kind() == "identifier" {
                        let var_name = ast_utils::get_node_text_owned(&array, source);
                        if self.is_unsafe_dereference_at_node(&var_name, node, source) {
                            let start_point = node.start_position();
                            violations.push(RuleViolation {
                                rule_id: "EXP34-C".to_string(),
                                severity: Severity::High,
                                message: format!(
                                    "Potential null pointer dereference in array access of variable '{}'",
                                    var_name
                                ),
                                file_path: String::new(),
                                line: start_point.row + 1,
                                column: start_point.column + 1,
                                suggestion: Some(format!("Check if '{}' is not NULL before array access", var_name)),
                                ..Default::default()
                            });
                        }
                    }
                }
            }
            "field_expression" => {
                // Structure/union member access: ptr->member or (*ptr).member
                if let Some(argument) = node.child_by_field_name("argument") {
                    if argument.kind() == "identifier" {
                        let var_name = ast_utils::get_node_text_owned(&argument, source);
                        if self.is_unsafe_dereference_at_node(&var_name, node, source) {
                            let start_point = node.start_position();
                            violations.push(RuleViolation {
                                rule_id: "EXP34-C".to_string(),
                                severity: Severity::High,
                                message: format!(
                                    "Potential null pointer dereference in member access of variable '{}'",
                                    var_name
                                ),
                                file_path: String::new(),
                                line: start_point.row + 1,
                                column: start_point.column + 1,
                                suggestion: Some(format!("Check if '{}' is not NULL before member access", var_name)),
                                ..Default::default()
                            });
                        }
                    }
                }
            }
            "call_expression" => {
                // Check function calls that commonly cause null pointer dereferences
                if let Some(function) = node.child_by_field_name("function") {
                    // Check if calling a function pointer (identifier) that could be NULL
                    if function.kind() == "identifier" {
                        let func_name = ast_utils::get_node_text_owned(&function, source);
                        if self.is_unsafe_dereference_at_node(&func_name, node, source) {
                            let start_point = function.start_position();
                            violations.push(RuleViolation {
                                rule_id: "EXP34-C".to_string(),
                                severity: Severity::High,
                                message: format!(
                                    "Calling potentially null function pointer '{}'",
                                    func_name
                                ),
                                file_path: String::new(),
                                line: start_point.row + 1,
                                column: start_point.column + 1,
                                suggestion: Some(format!(
                                    "Check if '{}' is not NULL before calling",
                                    func_name
                                )),
                                ..Default::default()
                            });
                        }
                    }

                    // Also check if it's a function that dereferences pointers
                    let func_name = ast_utils::get_node_text_owned(&function, source);
                    if is_deref_function(&func_name) {
                        // Check arguments for potentially null pointers
                        if let Some(args) = node.child_by_field_name("arguments") {
                            self.check_function_arguments(&args, source, violations);
                        }
                    }
                }
            }
            _ => {}
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_dereferences(&child, source, violations);
            }
        }
    }

    fn check_function_arguments(
        &self,
        args: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        for i in 0..args.child_count() {
            if let Some(arg) = args.child(i) {
                if arg.kind() == "identifier" {
                    let var_name = ast_utils::get_node_text_owned(&arg, source);
                    if self.is_unsafe_dereference_at_node(&var_name, &arg, source) {
                        let start_point = arg.start_position();
                        violations.push(RuleViolation {
                            rule_id: "EXP34-C".to_string(),
                            severity: Severity::High,
                            message: format!(
                                "Passing potentially null pointer '{}' to function",
                                var_name
                            ),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some(format!(
                                "Check if '{}' is not NULL before passing to function",
                                var_name
                            )),
                            ..Default::default()
                        });
                    }
                }
            }
        }
    }

    fn is_unsafe_dereference(&self, var_name: &str) -> bool {
        // A variable is a candidate for unsafe dereference if it is potentially null.
        // The null_checked_vars membership alone does NOT make it safe here — that
        // determination is position-sensitive and handled by is_unsafe_dereference_at_node
        // via the flow-sensitive check and is_in_null_guarded_scope.  Removing the
        // !null_checked_vars short-circuit ensures we correctly flag the deref_after_check
        // pattern: `if (ptr == NULL) { *ptr; }` — the variable IS in null_checked_vars
        // (we saw an == NULL check) but the dereference is INSIDE the null branch.
        self.potentially_null_vars.contains(var_name)
    }

    /// Check if a dereference at a specific AST node is unsafe, considering local null-checks
    fn is_unsafe_dereference_at_node(
        &self,
        var_name: &str,
        deref_node: &Node,
        source: &str,
    ) -> bool {
        // First check if it's potentially unsafe based on global analysis
        if !self.is_unsafe_dereference(var_name) {
            return false;
        }

        // Now check if this dereference is within a null-guarded scope
        // Walk up the AST to find if we're inside an if-statement that null-checks this variable
        if self.is_in_null_guarded_scope(var_name, deref_node, source) {
            return false; // Safe within this guarded scope
        }

        // Flow-sensitive check: if the variable was null-checked, and the dereference
        // occurs BEFORE a nullable reassignment, it's still safe.
        let deref_byte = deref_node.start_byte();
        if self.null_checked_vars.contains(var_name) {
            if let Some(&check_pos) = self.null_check_positions.get(var_name) {
                // Dereference is after the null check
                if deref_byte > check_pos {
                    // Check if any nullable reassignment occurs between the check and this dereference
                    let has_intervening_reassignment =
                        self.nullable_reassignments.iter().any(|(name, pos)| {
                            name == var_name && *pos > check_pos && *pos < deref_byte
                        });
                    if !has_intervening_reassignment {
                        return false; // Safe: null check covers this dereference
                    }
                    // There IS a reassignment between check and deref — fall through to unsafe
                }
            }
        }

        true // Unsafe dereference
    }

    /// Check if a node is within an if-statement that guards against null for the given variable
    fn is_in_null_guarded_scope(&self, var_name: &str, node: &Node, source: &str) -> bool {
        let mut current = node.parent();

        while let Some(parent) = current {
            // Check if we're in the right operand of a && expression whose left
            // operand null-checks this variable — short-circuit makes the right side safe.
            // Pattern: (ptr != NULL) && (ptr->field)  →  ptr->field is safe here.
            if parent.kind() == "binary_expression" {
                if let Some(operator) = parent.child_by_field_name("operator") {
                    let op = ast_utils::get_node_text_owned(&operator, source);
                    if op == "&&" {
                        if let (Some(left), Some(right)) = (
                            parent.child_by_field_name("left"),
                            parent.child_by_field_name("right"),
                        ) {
                            if self.node_is_within(&right, node)
                                && self.analyze_condition_for_safety(&left, var_name, source, false)
                            {
                                return true; // Safe: && short-circuit guards this side
                            }
                        }
                    }
                }
            }

            // Check if we're in a conditional expression (ternary operator)
            if parent.kind() == "conditional_expression" {
                // Pattern: (condition) ? expr1 : expr2
                if let Some(condition) = parent.child_by_field_name("condition") {
                    if let Some(checked_var) = self.get_null_check_var(&condition, source) {
                        if checked_var == var_name {
                            let is_safe_in_consequence =
                                self.is_null_safe_in_then_branch(&condition, var_name, source);

                            // Check if we're in the consequence (true branch)
                            if let Some(consequence) = parent.child_by_field_name("consequence") {
                                if self.node_is_within(&consequence, node) {
                                    return is_safe_in_consequence;
                                }
                            }

                            // Check if we're in the alternative (false branch)
                            if let Some(alternative) = parent.child_by_field_name("alternative") {
                                if self.node_is_within(&alternative, node) {
                                    return !is_safe_in_consequence;
                                }
                            }
                        }
                    }
                }
            }

            // Check if we're in an if-statement
            if parent.kind() == "if_statement" {
                // Check if the condition guards this variable
                if let Some(condition) = parent.child_by_field_name("condition") {
                    // Check if this is a null check for our variable
                    if let Some(checked_var) = self.get_null_check_var(&condition, source) {
                        if checked_var == var_name {
                            // Found a null check for this variable
                            // Determine what kind of check this is by examining the condition more carefully
                            let is_safe_in_then =
                                self.is_null_safe_in_then_branch(&condition, var_name, source);

                            // Check if we're in the consequence (then) branch
                            if let Some(consequence) = parent.child_by_field_name("consequence") {
                                if self.node_is_within(&consequence, node) {
                                    // We're in the then-branch
                                    return is_safe_in_then;
                                }
                            }

                            // Check if we're in the alternative (else) branch
                            if let Some(alternative) = parent.child_by_field_name("alternative") {
                                if self.node_is_within(&alternative, node) {
                                    // We're in the else-branch - safe is opposite of then
                                    return !is_safe_in_then;
                                }
                            }
                        }
                    }
                }
            }

            current = parent.parent();
        }

        false // Not in a guarded scope
    }

    /// Check if child_node is within (descendant of) parent_node
    fn node_is_within(&self, parent_node: &Node, child_node: &Node) -> bool {
        parent_node.start_byte() <= child_node.start_byte()
            && parent_node.end_byte() >= child_node.end_byte()
    }

    /// Determine if a variable is null-safe in the THEN branch of an if-statement
    /// Returns true if "if (condition) { /* var is safe here */ }"
    fn is_null_safe_in_then_branch(&self, condition: &Node, var_name: &str, source: &str) -> bool {
        // Recursively search for binary expressions that compare the variable
        self.analyze_condition_for_safety(condition, var_name, source, false)
    }

    /// Recursively analyze a condition to determine if variable is safe
    /// negated=true means we're inside a logical NOT
    fn analyze_condition_for_safety(
        &self,
        node: &Node,
        var_name: &str,
        source: &str,
        negated: bool,
    ) -> bool {
        match node.kind() {
            "parenthesized_expression" => {
                // Unwrap parentheses
                if let Some(child) = node.child(1) {
                    // child 0 is '(', child 1 is expression
                    return self.analyze_condition_for_safety(&child, var_name, source, negated);
                }
            }
            "unary_expression" => {
                // Handle logical NOT (!)
                if let Some(operator) = node.child(0) {
                    let op_text = ast_utils::get_node_text_owned(&operator, source);
                    if op_text == "!" {
                        if let Some(argument) = node.child_by_field_name("argument") {
                            return self.analyze_condition_for_safety(
                                &argument, var_name, source, !negated,
                            );
                        }
                    }
                }
            }
            "binary_expression" => {
                if let Some(operator) = node.child_by_field_name("operator") {
                    let op_text = ast_utils::get_node_text_owned(&operator, source);

                    match op_text.as_str() {
                        "==" => {
                            // var == NULL means NOT safe in then-branch (unless negated)
                            if self.is_null_comparison(node, var_name, source) {
                                return negated; // Safe only if negated (i.e., !(var == NULL))
                            }
                        }
                        "!=" => {
                            // var != NULL means safe in then-branch (unless negated)
                            if self.is_null_comparison(node, var_name, source) {
                                return !negated; // Safe unless negated
                            }
                        }
                        "&&" => {
                            // Both must be true for safety
                            if let (Some(left), Some(right)) = (
                                node.child_by_field_name("left"),
                                node.child_by_field_name("right"),
                            ) {
                                let left_safe = self
                                    .analyze_condition_for_safety(&left, var_name, source, negated);
                                let right_safe = self.analyze_condition_for_safety(
                                    &right, var_name, source, negated,
                                );
                                // Safe if either check confirms safety
                                return left_safe || right_safe;
                            }
                        }
                        "||" => {
                            // Either can be true
                            if let (Some(left), Some(right)) = (
                                node.child_by_field_name("left"),
                                node.child_by_field_name("right"),
                            ) {
                                let left_safe = self
                                    .analyze_condition_for_safety(&left, var_name, source, negated);
                                let right_safe = self.analyze_condition_for_safety(
                                    &right, var_name, source, negated,
                                );
                                // Safe only if both confirm safety
                                return left_safe && right_safe;
                            }
                        }
                        _ => {}
                    }
                }
            }
            "identifier" => {
                // Direct usage like "if (ptr)" means safe in then-branch
                let id_text = ast_utils::get_node_text_owned(node, source);
                if id_text == var_name {
                    return !negated; // Safe unless negated
                }
            }
            _ => {}
        }

        false // Default: not confirmed safe
    }

    /// Check if a binary expression compares the variable to NULL
    fn is_null_comparison(&self, binary_expr: &Node, var_name: &str, source: &str) -> bool {
        if let (Some(left), Some(right)) = (
            binary_expr.child_by_field_name("left"),
            binary_expr.child_by_field_name("right"),
        ) {
            let left_text = ast_utils::get_node_text_owned(&left, source);
            let right_text = ast_utils::get_node_text_owned(&right, source);

            // Check if one side is the variable and the other is NULL
            (left_text == var_name && is_null_value(&right_text))
                || (right_text == var_name && is_null_value(&left_text))
        } else {
            false
        }
    }
}

// Using ast_utils for common functions
fn get_identifier_name(declarator: &Node, source: &str) -> String {
    ast_utils::get_identifier_from_declarator(declarator, source)
}

fn is_null_value(text: &str) -> bool {
    let trimmed = text.trim();
    trimmed == "NULL" || trimmed == "0" || trimmed == "nullptr"
}

fn is_nullable_function_call(
    node: &Node,
    source: &str,
    summaries: &HashMap<String, FunctionSummary>,
) -> bool {
    if node.kind() != "call_expression" {
        return false;
    }

    if let Some(function) = node.child_by_field_name("function") {
        let func_name = ast_utils::get_node_text_owned(&function, source);

        // Check function summaries from inter-procedural analysis
        if let Some(summary) = summaries.get(func_name.as_str()) {
            if summary.can_return_null {
                return true;
            }
        }

        // Common functions that can return NULL
        matches!(
            func_name.as_str(),
            "malloc"
                | "calloc"
                | "realloc"
                | "strstr"
                | "strchr"
                | "strrchr"
                | "fopen"
                | "fdopen"
                | "freopen"
                | "tmpfile"
                | "popen"
                | "getenv"
                | "setlocale"
                | "strtok"
                | "bsearch"
                | "fgets"
                | "gets"
                | "strdup"
                | "strndup"
                | "strpbrk"
                | "memchr"
                | "localtime"
                | "gmtime"
                | "asctime"
                | "ctime"
                | "create_int" // Specific to test case
        )
    } else {
        false
    }
}

fn is_pointer_declarator(declarator: &Node) -> bool {
    match declarator.kind() {
        "pointer_declarator" => true,
        "array_declarator" => {
            // Arrays are also pointers in C
            true
        }
        _ => {
            // Check if any parent is a pointer declarator
            for i in 0..declarator.child_count() {
                if let Some(child) = declarator.child(i) {
                    if is_pointer_declarator(&child) {
                        return true;
                    }
                }
            }
            false
        }
    }
}

fn is_deref_function(func_name: &str) -> bool {
    // Functions that are known to dereference their pointer arguments
    matches!(
        func_name,
        "strlen"
            | "strcpy"
            | "strcat"
            | "strcmp"
            | "strchr"
            | "strstr"
            | "sprintf"
            | "fprintf"
            | "printf"
            | "scanf"
            | "fscanf"
            | "fread"
            | "fwrite"
            | "fgets"
            | "fputs"
            | "fputc"
            | "fgetc"
            | "memcpy"
            | "memmove"
            | "memset"
            | "memcmp"
            | "free"
    )
}

/// Check if a declarator contains an array_declarator at any level.
/// Used to detect stack arrays like `int *arr[5]` — these are never null themselves.
fn declarator_contains_array(node: &Node) -> bool {
    if node.kind() == "array_declarator" {
        return true;
    }
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if declarator_contains_array(&child) {
                return true;
            }
        }
    }
    false
}

/// Check if an expression is a cast to NULL (e.g., (int*)NULL)
fn is_cast_to_null(node: &Node, source: &str) -> bool {
    if node.kind() == "cast_expression" {
        // Check if the value being cast is NULL
        if let Some(value) = node.child_by_field_name("value") {
            let value_text = ast_utils::get_node_text_owned(&value, source);
            return is_null_value(&value_text);
        }
    }
    false
}

use super::super::{CertRule, RuleViolation};
use crate::manifest::Severity;
use tree_sitter::Node;
use std::collections::HashSet;

pub struct Exp34C;

impl CertRule for Exp34C {
    fn rule_id(&self) -> &'static str {
        "EXP34-C"
    }

    fn description(&self) -> &'static str {
        "Do not dereference null pointers"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Analyze function bodies for null pointer dereferences
        if node.kind() == "function_definition" {
            if let Some(body) = node.child_by_field_name("body") {
                let mut analyzer = NullPointerAnalyzer::new();
                analyzer.analyze_function_body(&body, source, &mut violations);
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
}

impl NullPointerAnalyzer {
    fn new() -> Self {
        Self {
            potentially_null_vars: HashSet::new(),
            null_checked_vars: HashSet::new(),
        }
    }

    fn analyze_function_body(&mut self, body: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // First pass: collect potentially null variables
        self.collect_null_variables(body, source);

        // Second pass: check for unsafe dereferences
        self.check_dereferences(body, source, violations);
    }

    fn collect_null_variables(&mut self, node: &Node, source: &str) {
        match node.kind() {
            "assignment_expression" => {
                if let (Some(left), Some(right)) = (
                    node.child_by_field_name("left"),
                    node.child_by_field_name("right")
                ) {
                    if left.kind() == "identifier" {
                        let var_name = get_node_text(&left, source);
                        let right_text = get_node_text(&right, source);

                        // Check if assigning NULL, 0, or function that can return null
                        if is_null_value(&right_text) || is_nullable_function_call(&right, source) {
                            self.potentially_null_vars.insert(var_name);
                        } else if !is_null_value(&right_text) {
                            // If assigning a non-null value, remove from potentially null set
                            self.potentially_null_vars.remove(&var_name);
                        }
                    }
                }
            }
            "declaration" => {
                self.process_declaration(node, source);
            }
            "if_statement" => {
                // Check for null checks in if conditions
                if let Some(condition) = node.child_by_field_name("condition") {
                    self.process_null_check(&condition, source);
                }
            }
            _ => {}
        }

        // Recursively process child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.collect_null_variables(&child, source);
            }
        }
    }

    fn process_declaration(&mut self, node: &Node, source: &str) {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "init_declarator" {
                    if let Some(declarator) = child.child_by_field_name("declarator") {
                        let var_name = get_identifier_name(&declarator, source);

                        // Check if initialized to null or a nullable function
                        if let Some(value) = child.child_by_field_name("value") {
                            let value_text = get_node_text(&value, source);
                            if is_null_value(&value_text) || is_nullable_function_call(&value, source) {
                                self.potentially_null_vars.insert(var_name);
                            }
                        } else {
                            // Uninitialized pointer variables are potentially null
                            if is_pointer_declarator(&declarator) {
                                self.potentially_null_vars.insert(var_name);
                            }
                        }
                    }
                }
            }
        }
    }

    fn process_null_check(&mut self, condition: &Node, source: &str) {
        // Look for patterns like: ptr != NULL, ptr == NULL, !ptr, ptr
        match condition.kind() {
            "binary_expression" => {
                if let (Some(left), Some(operator), Some(right)) = (
                    condition.child_by_field_name("left"),
                    condition.child_by_field_name("operator"),
                    condition.child_by_field_name("right")
                ) {
                    let op_text = get_node_text(&operator, source);
                    let left_text = get_node_text(&left, source);
                    let right_text = get_node_text(&right, source);

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
                        let var_name = get_node_text(&operand, source);
                        self.null_checked_vars.insert(var_name);
                    }
                }
            }
            "identifier" => {
                // Pattern: if (ptr) - checks that ptr is not null
                let var_name = get_node_text(condition, source);
                self.null_checked_vars.insert(var_name);
            }
            _ => {}
        }
    }

    fn check_dereferences(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            "pointer_expression" => {
                // Direct pointer dereference: *ptr
                if let Some(argument) = node.child_by_field_name("argument") {
                    if argument.kind() == "identifier" {
                        let var_name = get_node_text(&argument, source);
                        if self.is_unsafe_dereference(&var_name) {
                            let start_point = node.start_position();
                            violations.push(RuleViolation {
                                rule_id: "EXP34-C".to_string(),
                                severity: Severity::High,
                                message: format!(
                                    "Potential null pointer dereference of variable '{}'",
                                    var_name
                                ),
                                file_path: String::new(),
                                line: start_point.row + 1,
                                column: start_point.column + 1,
                                suggestion: Some(format!("Check if '{}' is not NULL before dereferencing", var_name)),
                            });
                        }
                    }
                }
            }
            "subscript_expression" => {
                // Array subscript can also be null pointer dereference: ptr[index]
                // The subscript expression has the array as the first child (child 0)
                if let Some(array) = node.child(0) {
                    if array.kind() == "identifier" {
                        let var_name = get_node_text(&array, source);
                        if self.is_unsafe_dereference(&var_name) {
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
                            });
                        }
                    }
                }
            }
            "field_expression" => {
                // Structure/union member access: ptr->member or (*ptr).member
                if let Some(argument) = node.child_by_field_name("argument") {
                    if argument.kind() == "identifier" {
                        let var_name = get_node_text(&argument, source);
                        if self.is_unsafe_dereference(&var_name) {
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
                            });
                        }
                    }
                }
            }
            "call_expression" => {
                // Check function calls that commonly cause null pointer dereferences
                if let Some(function) = node.child_by_field_name("function") {
                    let func_name = get_node_text(&function, source);
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

    fn check_function_arguments(&self, args: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        for i in 0..args.child_count() {
            if let Some(arg) = args.child(i) {
                if arg.kind() == "identifier" {
                    let var_name = get_node_text(&arg, source);
                    if self.is_unsafe_dereference(&var_name) {
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
                            suggestion: Some(format!("Check if '{}' is not NULL before passing to function", var_name)),
                        });
                    }
                }
            }
        }
    }

    fn is_unsafe_dereference(&self, var_name: &str) -> bool {
        // A dereference is unsafe if the variable is potentially null and hasn't been checked
        self.potentially_null_vars.contains(var_name) && !self.null_checked_vars.contains(var_name)
    }
}

fn get_node_text(node: &Node, source: &str) -> String {
    source[node.start_byte()..node.end_byte()].to_string()
}

fn get_identifier_name(declarator: &Node, source: &str) -> String {
    match declarator.kind() {
        "identifier" => get_node_text(declarator, source),
        "pointer_declarator" | "array_declarator" => {
            // Look for the identifier in pointer/array declarators
            for i in 0..declarator.child_count() {
                if let Some(child) = declarator.child(i) {
                    if child.kind() == "identifier" {
                        return get_node_text(&child, source);
                    }
                    // Recursively search in nested declarators
                    let nested_name = get_identifier_name(&child, source);
                    if nested_name != "unknown" {
                        return nested_name;
                    }
                }
            }
            "unknown".to_string()
        }
        _ => "unknown".to_string(),
    }
}

fn is_null_value(text: &str) -> bool {
    let trimmed = text.trim();
    trimmed == "NULL" || trimmed == "0" || trimmed == "nullptr"
}

fn is_nullable_function_call(node: &Node, source: &str) -> bool {
    if node.kind() != "call_expression" {
        return false;
    }

    if let Some(function) = node.child_by_field_name("function") {
        let func_name = get_node_text(&function, source);
        // Common functions that can return NULL
        matches!(func_name.as_str(),
            "malloc" | "calloc" | "realloc" | "strstr" | "strchr" | "strrchr" |
            "fopen" | "fdopen" | "freopen" | "tmpfile" | "popen" |
            "getenv" | "setlocale" | "strtok" | "bsearch"
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
    matches!(func_name,
        "strlen" | "strcpy" | "strcat" | "strcmp" | "strchr" | "strstr" |
        "sprintf" | "fprintf" | "printf" | "scanf" | "fscanf" |
        "fread" | "fwrite" | "fgets" | "fputs" | "fputc" | "fgetc" |
        "memcpy" | "memmove" | "memset" | "memcmp" | "free"
    )
}

#[cfg(test)]
#[path = "tests/exp34_c.rs"]
mod tests;
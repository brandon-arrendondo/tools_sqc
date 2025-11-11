use super::super::{CertRule, RuleViolation};
use crate::manifest::{Severity, RuleCategory};
use tree_sitter::Node;
use std::collections::{HashMap, HashSet};

pub struct Mem30C;

impl CertRule for Mem30C {
    fn rule_id(&self) -> &'static str {
        "MEM30-C"
    }

    fn description(&self) -> &'static str {
        "Do not access freed memory"
    }

    fn severity(&self) -> Severity {
        Severity::Critical
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "MEM30-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        let mut analyzer = MemoryAnalyzer::new();

        // First pass: collect memory operations
        analyzer.collect_memory_operations(node, source);

        // Second pass: analyze for violations
        analyzer.analyze_violations(node, source, &mut violations);

        violations
    }
}

#[derive(Debug, Clone)]
enum MemoryState {
    Allocated,
    Freed,
    Unknown,
}

#[derive(Debug, Clone)]
struct MemoryOperation {
    variable: String,
    operation: String, // "malloc", "free", "realloc", "access"
    line: usize,
    node_start: usize,
    node_end: usize,
}

struct MemoryAnalyzer {
    memory_operations: Vec<MemoryOperation>,
    variable_states: HashMap<String, MemoryState>,
}

impl MemoryAnalyzer {
    fn new() -> Self {
        Self {
            memory_operations: Vec::new(),
            variable_states: HashMap::new(),
        }
    }

    fn collect_memory_operations(&mut self, node: &Node, source: &str) {
        match node.kind() {
            "call_expression" => {
                self.process_function_call(node, source);
            }
            "assignment_expression" => {
                self.process_assignment(node, source);
            }
            _ => {}
        }

        // Recursively process child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.collect_memory_operations(&child, source);
            }
        }
    }

    fn process_function_call(&mut self, node: &Node, source: &str) {
        if let Some(function_node) = node.child_by_field_name("function") {
            let function_name = &source[function_node.start_byte()..function_node.end_byte()];

            match function_name {
                "free" => {
                    self.process_free_call(node, source);
                }
                "malloc" | "calloc" | "realloc" => {
                    self.process_allocation_call(node, source, function_name);
                }
                "strcpy" | "strcat" | "memcpy" | "memmove" | "sprintf" | "printf" => {
                    self.process_memory_access_function(node, source, function_name);
                }
                _ => {
                    // Check if any arguments might be freed memory
                    self.check_function_arguments_for_freed_memory(node, source, function_name);
                }
            }
        }
    }

    fn process_free_call(&mut self, node: &Node, source: &str) {
        if let Some(arguments) = node.child_by_field_name("arguments") {
            for i in 0..arguments.child_count() {
                if let Some(arg) = arguments.child(i) {
                    if arg.kind() != "," {
                        let var_name = self.extract_variable_name(&arg, source);
                        if !var_name.is_empty() {
                            let start_point = node.start_position();
                            self.memory_operations.push(MemoryOperation {
                                variable: var_name.clone(),
                                operation: "free".to_string(),
                                line: start_point.row + 1,
                                node_start: node.start_byte(),
                                node_end: node.end_byte(),
                            });
                            self.variable_states.insert(var_name, MemoryState::Freed);
                        }
                    }
                }
            }
        }
    }

    fn process_allocation_call(&mut self, node: &Node, source: &str, function_name: &str) {
        // Look for assignment to track allocated memory
        if let Some(parent) = node.parent() {
            if parent.kind() == "assignment_expression" {
                if let Some(left) = parent.child_by_field_name("left") {
                    let var_name = self.extract_variable_name(&left, source);
                    if !var_name.is_empty() {
                        let start_point = node.start_position();
                        self.memory_operations.push(MemoryOperation {
                            variable: var_name.clone(),
                            operation: function_name.to_string(),
                            line: start_point.row + 1,
                            node_start: node.start_byte(),
                            node_end: node.end_byte(),
                        });
                        self.variable_states.insert(var_name, MemoryState::Allocated);
                    }
                }
            }
        }
    }

    fn process_assignment(&mut self, node: &Node, source: &str) {
        // Check for assignments that might involve freed memory
        if let (Some(left), Some(right)) = (node.child_by_field_name("left"), node.child_by_field_name("right")) {
            let left_var = self.extract_variable_name(&left, source);
            let right_var = self.extract_variable_name(&right, source);

            // Check if assigning from a freed pointer
            if !right_var.is_empty() {
                if let Some(MemoryState::Freed) = self.variable_states.get(&right_var) {
                    let start_point = node.start_position();
                    self.memory_operations.push(MemoryOperation {
                        variable: right_var.clone(),
                        operation: "access".to_string(),
                        line: start_point.row + 1,
                        node_start: node.start_byte(),
                        node_end: node.end_byte(),
                    });
                }
            }
        }
    }

    fn process_memory_access_function(&mut self, node: &Node, source: &str, function_name: &str) {
        // Check if function arguments reference freed memory
        if let Some(arguments) = node.child_by_field_name("arguments") {
            for i in 0..arguments.child_count() {
                if let Some(arg) = arguments.child(i) {
                    if arg.kind() != "," {
                        let var_name = self.extract_variable_name(&arg, source);
                        if !var_name.is_empty() {
                            if let Some(MemoryState::Freed) = self.variable_states.get(&var_name) {
                                let start_point = node.start_position();
                                self.memory_operations.push(MemoryOperation {
                                    variable: var_name.clone(),
                                    operation: "access".to_string(),
                                    line: start_point.row + 1,
                                    node_start: node.start_byte(),
                                    node_end: node.end_byte(),
                                });
                            }
                        }
                    }
                }
            }
        }
    }

    fn check_function_arguments_for_freed_memory(&mut self, node: &Node, source: &str, function_name: &str) {
        if let Some(arguments) = node.child_by_field_name("arguments") {
            for i in 0..arguments.child_count() {
                if let Some(arg) = arguments.child(i) {
                    if arg.kind() != "," {
                        let var_name = self.extract_variable_name(&arg, source);
                        if !var_name.is_empty() {
                            if let Some(MemoryState::Freed) = self.variable_states.get(&var_name) {
                                let start_point = node.start_position();
                                self.memory_operations.push(MemoryOperation {
                                    variable: var_name.clone(),
                                    operation: "access".to_string(),
                                    line: start_point.row + 1,
                                    node_start: node.start_byte(),
                                    node_end: node.end_byte(),
                                });
                            }
                        }
                    }
                }
            }
        }
    }

    fn analyze_violations(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Look for specific violation patterns
        self.check_use_after_free(violations, source);
        self.check_double_free(violations, source);
        self.check_realloc_misuse(node, source, violations);
        self.check_loop_free_patterns(node, source, violations);
    }

    fn check_use_after_free(&self, violations: &mut Vec<RuleViolation>, source: &str) {
        let mut freed_vars: HashSet<String> = HashSet::new();

        for op in &self.memory_operations {
            match op.operation.as_str() {
                "free" => {
                    freed_vars.insert(op.variable.clone());
                }
                "access" => {
                    if freed_vars.contains(&op.variable) {
                        violations.push(RuleViolation {
                            rule_id: "MEM30-C".to_string(),
                            severity: Severity::Critical,
                            message: format!(
                                "Use-after-free: variable '{}' accessed after being freed",
                                op.variable
                            ),
                            file_path: String::new(),
                            line: op.line,
                            column: 1,
                            suggestion: Some("Do not access memory after freeing it. Set pointer to NULL after free().".to_string()),
                        ..Default::default()
                        });
                    }
                }
                "malloc" | "calloc" | "realloc" => {
                    // Reset freed status if reallocated
                    freed_vars.remove(&op.variable);
                }
                _ => {}
            }
        }
    }

    fn check_double_free(&self, violations: &mut Vec<RuleViolation>, source: &str) {
        let mut freed_vars: HashSet<String> = HashSet::new();

        for op in &self.memory_operations {
            match op.operation.as_str() {
                "free" => {
                    if freed_vars.contains(&op.variable) {
                        violations.push(RuleViolation {
                            rule_id: "MEM30-C".to_string(),
                            severity: Severity::Critical,
                            message: format!(
                                "Double-free: variable '{}' freed multiple times",
                                op.variable
                            ),
                            file_path: String::new(),
                            line: op.line,
                            column: 1,
                            suggestion: Some("Set pointer to NULL after freeing to prevent double-free.".to_string()),
                        ..Default::default()
                        });
                    } else {
                        freed_vars.insert(op.variable.clone());
                    }
                }
                "malloc" | "calloc" | "realloc" => {
                    // Reset freed status if reallocated
                    freed_vars.remove(&op.variable);
                }
                _ => {}
            }
        }
    }

    fn check_realloc_misuse(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        self.check_realloc_pattern(node, source, violations);
    }

    fn check_realloc_pattern(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            "call_expression" => {
                if let Some(function_node) = node.child_by_field_name("function") {
                    let function_name = &source[function_node.start_byte()..function_node.end_byte()];

                    if function_name == "realloc" {
                        // Look for dangerous realloc patterns
                        if let Some(parent) = node.parent() {
                            if parent.kind() == "assignment_expression" {
                                if let Some(left) = parent.child_by_field_name("left") {
                                    let realloc_target = self.extract_variable_name(&left, source);

                                    // Check if realloc is assigned back to the same variable
                                    if let Some(arguments) = node.child_by_field_name("arguments") {
                                        if let Some(first_arg) = arguments.child(0) {
                                            if first_arg.kind() != "," {
                                                let source_var = self.extract_variable_name(&first_arg, source);
                                                if realloc_target == source_var {
                                                    let start_point = node.start_position();
                                                    violations.push(RuleViolation {
                                                        rule_id: "MEM30-C".to_string(),
                                                        severity: Severity::High,
                                                        message: format!(
                                                            "Dangerous realloc pattern: '{}' assigned back to same variable, may cause memory leak on failure",
                                                            realloc_target
                                                        ),
                                                        file_path: String::new(),
                                                        line: start_point.row + 1,
                                                        column: start_point.column + 1,
                                                        suggestion: Some("Use temporary variable for realloc result and check for NULL before assignment".to_string()),
                                                    ..Default::default()
                                                    });
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            _ => {}
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_realloc_pattern(&child, source, violations);
            }
        }
    }

    fn check_loop_free_patterns(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Look for linked list free patterns
        self.check_linked_list_free(node, source, violations);
    }

    fn check_linked_list_free(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if node.kind() == "for_statement" || node.kind() == "while_statement" {
            let loop_text = &source[node.start_byte()..node.end_byte()];

            // Look for patterns like: p = p->next after free(p)
            if loop_text.contains("free(") && loop_text.contains("->next") {
                // This is a heuristic check for the classic linked list free error
                if self.has_dangerous_loop_free_pattern(loop_text) {
                    let start_point = node.start_position();
                    violations.push(RuleViolation {
                        rule_id: "MEM30-C".to_string(),
                        severity: Severity::High,
                        message: "Potential use-after-free in loop: accessing freed pointer's members".to_string(),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some("Save pointer->next before freeing pointer".to_string()),
                    ..Default::default()
                    });
                }
            }
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_linked_list_free(&child, source, violations);
            }
        }
    }

    fn has_dangerous_loop_free_pattern(&self, loop_text: &str) -> bool {
        // Look for pattern: p = p->next; free(p) or free(p); ... p->next
        let lines: Vec<&str> = loop_text.lines().collect();

        for i in 0..lines.len() {
            let line = lines[i].trim();
            if line.contains("free(") {
                // Check if there's a ->next access in the same loop
                for j in 0..lines.len() {
                    if i != j && lines[j].contains("->next") {
                        return true;
                    }
                }
            }
        }

        false
    }

    fn extract_variable_name(&self, node: &Node, source: &str) -> String {
        match node.kind() {
            "identifier" => {
                source[node.start_byte()..node.end_byte()].to_string()
            }
            "pointer_expression" => {
                // Handle *ptr
                if let Some(argument) = node.child_by_field_name("argument") {
                    self.extract_variable_name(&argument, source)
                } else {
                    String::new()
                }
            }
            "field_expression" => {
                // Handle ptr->field, just return the base
                if let Some(argument) = node.child_by_field_name("argument") {
                    self.extract_variable_name(&argument, source)
                } else {
                    String::new()
                }
            }
            "subscript_expression" => {
                // Handle ptr[index], just return the base
                if let Some(argument) = node.child_by_field_name("argument") {
                    self.extract_variable_name(&argument, source)
                } else {
                    String::new()
                }
            }
            _ => String::new()
        }
    }
}

// DEPRECATED: Inline tests moved to src/rules/cert_c/tests/inline/
// #[cfg(test)]
// #[path = "tests/mem30_c.rs"]
// mod tests;

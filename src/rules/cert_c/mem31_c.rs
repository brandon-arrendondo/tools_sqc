use super::ast_utils;
use super::super::{CertRule, RuleViolation};
use crate::manifest::Severity;
use tree_sitter::Node;
use std::collections::{HashMap, HashSet};

pub struct Mem31C;

impl CertRule for Mem31C {
    fn rule_id(&self) -> &'static str {
        "MEM31-C"
    }

    fn description(&self) -> &'static str {
        "Free dynamically allocated memory when no longer needed"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Analyze each function independently for memory leaks
        if node.kind() == "function_definition" {
            let mut analyzer = MemoryLeakAnalyzer::new();
            analyzer.analyze_function(node, source, &mut violations);
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

struct MemoryLeakAnalyzer {
    // Track allocated memory by variable name
    allocated_memory: HashMap<String, AllocInfo>,
    // Track freed memory
    freed_memory: HashSet<String>,
    // Track variables that are returned or stored globally
    escaped_memory: HashSet<String>,
}

#[derive(Debug, Clone)]
struct AllocInfo {
    line: usize,
    column: usize,
    alloc_type: String,
}

impl MemoryLeakAnalyzer {
    fn new() -> Self {
        Self {
            allocated_memory: HashMap::new(),
            freed_memory: HashSet::new(),
            escaped_memory: HashSet::new(),
        }
    }

    fn analyze_function(&mut self, func_node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(body) = func_node.child_by_field_name("body") {
            // First pass: collect all memory operations
            self.analyze_node(&body, source);

            // Second pass: check for leaks
            self.detect_leaks(violations);
        }
    }

    fn analyze_node(&mut self, node: &Node, source: &str) {
        match node.kind() {
            "declaration" | "expression_statement" => {
                self.process_statement(node, source);
            }
            "assignment_expression" => {
                self.process_assignment(node, source);
            }
            "call_expression" => {
                self.process_call(node, source);
            }
            "return_statement" => {
                self.process_return(node, source);
            }
            "if_statement" | "while_statement" | "for_statement" | "do_statement" => {
                // Process control flow statements recursively
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        self.analyze_node(&child, source);
                    }
                }
            }
            "compound_statement" => {
                // Process compound statements recursively
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        self.analyze_node(&child, source);
                    }
                }
            }
            _ => {
                // Recursively process other nodes
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        self.analyze_node(&child, source);
                    }
                }
            }
        }
    }

    fn process_statement(&mut self, node: &Node, source: &str) {
        // Look for declarations with malloc/calloc/realloc
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "init_declarator" {
                    if let Some(declarator) = child.child_by_field_name("declarator") {
                        let var_name = self.get_variable_name(&declarator, source);

                        if let Some(value) = child.child_by_field_name("value") {
                            if self.is_allocation_call(&value, source) {
                                let pos = value.start_position();
                                let alloc_type = self.get_allocation_type(&value, source);
                                self.allocated_memory.insert(var_name.clone(), AllocInfo {
                                    line: pos.row + 1,
                                    column: pos.column + 1,
                                    alloc_type,
                                });
                            }
                        }
                    }
                } else {
                    self.analyze_node(&child, source);
                }
            }
        }
    }

    fn process_assignment(&mut self, node: &Node, source: &str) {
        if let (Some(left), Some(right)) = (
            node.child_by_field_name("left"),
            node.child_by_field_name("right")
        ) {
            if left.kind() == "identifier" {
                let var_name = ast_utils::get_node_text_owned(&left, source);

                // Check if this variable was previously allocated
                let was_allocated = self.allocated_memory.contains_key(&var_name);

                // Check if assigning result of allocation
                if self.is_allocation_call(&right, source) {
                    // If the variable was already allocated and not freed, it's a leak
                    if was_allocated && !self.freed_memory.contains(&var_name) {
                        // The old allocation is now leaked - we need to create a unique identifier for it
                        // Since we can't track the old allocation separately, we'll generate a violation now
                        if let Some(old_alloc) = self.allocated_memory.get(&var_name) {
                            // We'll mark this as leaked by creating a unique name for the old allocation
                            let leaked_name = format!("{}@{}:{}", var_name, old_alloc.line, old_alloc.column);
                            self.allocated_memory.insert(leaked_name, old_alloc.clone());
                        }
                    }

                    let pos = right.start_position();
                    let alloc_type = self.get_allocation_type(&right, source);
                    self.allocated_memory.insert(var_name.clone(), AllocInfo {
                        line: pos.row + 1,
                        column: pos.column + 1,
                        alloc_type,
                    });
                } else if right.kind() == "identifier" {
                    // Check if assigning allocated pointer to another variable
                    let right_var = ast_utils::get_node_text_owned(&right, source);
                    if self.allocated_memory.contains_key(&right_var) {
                        // Transfer ownership
                        if let Some(alloc_info) = self.allocated_memory.get(&right_var).cloned() {
                            self.allocated_memory.insert(var_name, alloc_info);
                            // The original variable still holds the allocation until freed
                        }
                    }
                } else if right.kind() == "null" || ast_utils::get_node_text_owned(&right, source) == "NULL" {
                    // Setting to NULL doesn't free memory, potential leak if not freed before
                    // If the variable was allocated and not freed, it's a leak
                    if was_allocated && !self.freed_memory.contains(&var_name) {
                        if let Some(old_alloc) = self.allocated_memory.get(&var_name) {
                            let leaked_name = format!("{}@{}:{}", var_name, old_alloc.line, old_alloc.column);
                            self.allocated_memory.insert(leaked_name, old_alloc.clone());
                        }
                    }
                }
            }
        }
    }

    fn process_call(&mut self, node: &Node, source: &str) {
        if let Some(function) = node.child_by_field_name("function") {
            let func_name = ast_utils::get_node_text_owned(&function, source);

            if func_name == "free" {
                // Process free() call
                if let Some(arguments) = node.child_by_field_name("arguments") {
                    for i in 0..arguments.child_count() {
                        if let Some(arg) = arguments.child(i) {
                            if arg.kind() == "identifier" {
                                let var_name = ast_utils::get_node_text_owned(&arg, source);
                                self.freed_memory.insert(var_name.clone());
                                // Also mark any aliases as freed
                                let vars_to_free: Vec<String> = self.allocated_memory
                                    .iter()
                                    .filter_map(|(k, v)| {
                                        if let Some(original) = self.allocated_memory.get(&var_name) {
                                            if v.line == original.line && v.column == original.column {
                                                Some(k.clone())
                                            } else {
                                                None
                                            }
                                        } else {
                                            None
                                        }
                                    })
                                    .collect();

                                for v in vars_to_free {
                                    self.freed_memory.insert(v);
                                }
                            }
                        }
                    }
                }
            } else if func_name == "realloc" {
                // realloc can be used to free memory (when new size is 0) or reallocate
                if let Some(arguments) = node.child_by_field_name("arguments") {
                    let mut arg_count = 0;
                    let mut first_arg = String::new();

                    for i in 0..arguments.child_count() {
                        if let Some(arg) = arguments.child(i) {
                            if arg.kind() != "," && arg.kind() != "(" && arg.kind() != ")" {
                                if arg_count == 0 && arg.kind() == "identifier" {
                                    first_arg = ast_utils::get_node_text_owned(&arg, source);
                                }
                                arg_count += 1;
                            }
                        }
                    }

                    if !first_arg.is_empty() {
                        // realloc frees the old memory and allocates new
                        self.freed_memory.insert(first_arg.clone());
                    }
                }
            } else {
                // Check if passing allocated memory to a function (might transfer ownership)
                // For now, we'll be conservative and not mark it as escaped
                // unless it's a known ownership-transferring function
            }
        }
    }

    fn process_return(&mut self, node: &Node, source: &str) {
        // If returning allocated memory, it escapes and shouldn't be considered a leak
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "identifier" {
                    let var_name = ast_utils::get_node_text_owned(&child, source);
                    if self.allocated_memory.contains_key(&var_name) {
                        self.escaped_memory.insert(var_name);
                    }
                } else if self.is_allocation_call(&child, source) {
                    // Direct return of allocation is not a leak
                    // We don't track it since it escapes immediately
                }
            }
        }
    }

    fn is_allocation_call(&self, node: &Node, source: &str) -> bool {
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = ast_utils::get_node_text_owned(&function, source);
                return matches!(func_name.as_str(), "malloc" | "calloc" | "realloc" | "strdup" | "strndup");
            }
        }
        false
    }

    fn get_allocation_type(&self, node: &Node, source: &str) -> String {
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                return ast_utils::get_node_text_owned(&function, source);
            }
        }
        "unknown".to_string()
    }

    fn get_variable_name(&self, declarator: &Node, source: &str) -> String {
        match declarator.kind() {
            "identifier" => ast_utils::get_node_text_owned(declarator, source),
            "pointer_declarator" | "array_declarator" => {
                for i in 0..declarator.child_count() {
                    if let Some(child) = declarator.child(i) {
                        if child.kind() == "identifier" {
                            return ast_utils::get_node_text_owned(&child, source);
                        }
                        let nested_name = self.get_variable_name(&child, source);
                        if nested_name != "unknown" {
                            return nested_name;
                        }
                    }
                }
                "unknown".to_string()
            }
            _ => "unknown".to_string()
        }
    }

    fn detect_leaks(&self, violations: &mut Vec<RuleViolation>) {
        for (var_name, alloc_info) in &self.allocated_memory {
            if !self.freed_memory.contains(var_name) && !self.escaped_memory.contains(var_name) {
                violations.push(RuleViolation {
                    rule_id: "MEM31-C".to_string(),
                    severity: Severity::High,
                    message: format!(
                        "Memory allocated with '{}' for variable '{}' is not freed",
                        alloc_info.alloc_type, var_name
                    ),
                    file_path: String::new(),
                    line: alloc_info.line,
                    column: alloc_info.column,
                    suggestion: Some(format!("Add 'free({})' before the variable goes out of scope", var_name)),
                });
            }
        }
    }
}

#[cfg(test)]
#[path = "tests/mem31_c.rs"]
mod tests;
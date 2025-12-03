use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils;
use std::collections::{HashMap, HashSet};
use tree_sitter::Node;

pub struct Mem31C;

impl CertRule for Mem31C {
    fn rule_id(&self) -> &'static str {
        "MEM31-C"
    }

    fn description(&self) -> &'static str {
        "Free dynamically allocated memory when no longer needed"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "MEM31-C"
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
    // Track freed memory: var_name -> (line, column) of free call
    freed_memory: HashMap<String, (usize, usize)>,
    // Track variables that are returned or stored globally
    escaped_memory: HashSet<String>,
    // Collect double-free violations during analysis
    double_free_violations: Vec<RuleViolation>,
    // Track if we're inside a loop (for double-free detection)
    in_loop: bool,
    // Track loop nesting depth for proper double-free detection
    loop_depth: usize,
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
            freed_memory: HashMap::new(),
            escaped_memory: HashSet::new(),
            double_free_violations: Vec::new(),
            in_loop: false,
            loop_depth: 0,
        }
    }

    fn analyze_function(
        &mut self,
        func_node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let Some(body) = func_node.child_by_field_name("body") {
            // First pass: collect all memory operations and detect double-frees
            self.analyze_node(&body, source);

            // Add double-free violations found during analysis
            violations.append(&mut self.double_free_violations);

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
            "while_statement" | "for_statement" | "do_statement" => {
                // Track loop nesting for double-free detection
                self.in_loop = true;
                self.loop_depth += 1;
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        self.analyze_node(&child, source);
                    }
                }
                self.loop_depth -= 1;
                if self.loop_depth == 0 {
                    self.in_loop = false;
                }
            }
            "if_statement" => {
                // For if statements, use branch-aware analysis
                // Save freed state before processing each branch
                let saved_freed = self.freed_memory.clone();

                // Check if this if-block contains a return statement
                let has_return = self.block_has_return(node);

                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        self.analyze_node(&child, source);
                    }
                }

                // If this if-block has a return, frees inside it are branch-local
                // Restore previous state to avoid false double-free on other branches
                if has_return && !self.in_loop {
                    self.freed_memory = saved_freed;
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
                                self.allocated_memory.insert(
                                    var_name.clone(),
                                    AllocInfo {
                                        line: pos.row + 1,
                                        column: pos.column + 1,
                                        alloc_type,
                                    },
                                );
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
            node.child_by_field_name("right"),
        ) {
            // Handle field expressions on the left - memory escapes through struct assignment
            // e.g., list->head = new_node  (new_node escapes through list)
            if left.kind() == "field_expression" || left.kind() == "subscript_expression" {
                // If right side is an allocated variable, mark it as escaped
                if right.kind() == "identifier" {
                    let right_var = ast_utils::get_node_text_owned(&right, source);
                    if self.allocated_memory.contains_key(&right_var) {
                        self.escaped_memory.insert(right_var);
                    }
                }
                return;
            }

            // Handle identifiers for allocation tracking
            let var_name = if left.kind() == "identifier" {
                ast_utils::get_node_text_owned(&left, source)
            } else {
                return;
            };

            // Check if this variable was previously allocated
            let was_allocated = self.allocated_memory.contains_key(&var_name);

            // Check if assigning result of allocation
            if self.is_allocation_call(&right, source) {
                // If the variable was already allocated and not freed, it's a leak
                if was_allocated && !self.freed_memory.contains_key(&var_name) {
                    // The old allocation is now leaked - we need to create a unique identifier for it
                    // Since we can't track the old allocation separately, we'll generate a violation now
                    if let Some(old_alloc) = self.allocated_memory.get(&var_name) {
                        // We'll mark this as leaked by creating a unique name for the old allocation
                        let leaked_name =
                            format!("{}@{}:{}", var_name, old_alloc.line, old_alloc.column);
                        self.allocated_memory.insert(leaked_name, old_alloc.clone());
                    }
                }

                // New allocation clears freed status (variable now points to valid memory)
                self.freed_memory.remove(&var_name);

                let pos = right.start_position();
                let alloc_type = self.get_allocation_type(&right, source);
                self.allocated_memory.insert(
                    var_name.clone(),
                    AllocInfo {
                        line: pos.row + 1,
                        column: pos.column + 1,
                        alloc_type,
                    },
                );
            } else if right.kind() == "identifier" {
                // Check if assigning allocated pointer to another variable
                let right_var = ast_utils::get_node_text_owned(&right, source);

                // Assignment of one pointer to another clears the freed status
                // (e.g., buffer = temp after realloc)
                self.freed_memory.remove(&var_name);

                if self.allocated_memory.contains_key(&right_var) {
                    // Transfer ownership
                    if let Some(alloc_info) = self.allocated_memory.get(&right_var).cloned() {
                        self.allocated_memory.insert(var_name, alloc_info);
                        // The original variable still holds the allocation until freed
                    }
                }
            } else if right.kind() == "null"
                || ast_utils::get_node_text_owned(&right, source) == "NULL"
            {
                // Setting to NULL doesn't free memory, potential leak if not freed before
                // If the variable was allocated and not freed, it's a leak
                if was_allocated && !self.freed_memory.contains_key(&var_name) {
                    if let Some(old_alloc) = self.allocated_memory.get(&var_name) {
                        let leaked_name =
                            format!("{}@{}:{}", var_name, old_alloc.line, old_alloc.column);
                        self.allocated_memory.insert(leaked_name, old_alloc.clone());
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
                            // Handle identifiers and field expressions
                            let var_name = if arg.kind() == "identifier" {
                                ast_utils::get_node_text_owned(&arg, source)
                            } else if arg.kind() == "field_expression" {
                                // For field expressions like "container->data", track as full expression
                                ast_utils::get_node_text_owned(&arg, source)
                            } else {
                                continue;
                            };

                            if !var_name.is_empty() {
                                let free_pos = node.start_position();

                                // Check for double-free: if already freed, report violation
                                if self.freed_memory.contains_key(&var_name) {
                                    self.double_free_violations.push(RuleViolation {
                                        rule_id: "MEM31-C".to_string(),
                                        severity: Severity::High,
                                        message: format!(
                                            "Double free detected: '{}' was already freed",
                                            var_name
                                        ),
                                        file_path: String::new(),
                                        line: free_pos.row + 1,
                                        column: free_pos.column + 1,
                                        suggestion: Some(format!(
                                            "Remove this duplicate free() call or set '{}' = NULL after first free",
                                            var_name
                                        )),
                                        ..Default::default()
                                    });
                                }

                                // Mark as freed
                                self.freed_memory.insert(
                                    var_name.clone(),
                                    (free_pos.row + 1, free_pos.column + 1),
                                );

                                // Also mark any aliases as freed
                                let vars_to_free: Vec<String> = self
                                    .allocated_memory
                                    .iter()
                                    .filter_map(|(k, v)| {
                                        if let Some(original) = self.allocated_memory.get(&var_name)
                                        {
                                            if v.line == original.line
                                                && v.column == original.column
                                            {
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
                                    self.freed_memory
                                        .insert(v, (free_pos.row + 1, free_pos.column + 1));
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
                    let free_pos = node.start_position();

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
                        self.freed_memory
                            .insert(first_arg.clone(), (free_pos.row + 1, free_pos.column + 1));
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
        // Handle cast expressions like (char *)malloc(...)
        if node.kind() == "cast_expression" {
            if let Some(value) = node.child_by_field_name("value") {
                return self.is_allocation_call(&value, source);
            }
        }

        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = ast_utils::get_node_text_owned(&function, source);
                return matches!(
                    func_name.as_str(),
                    "malloc" | "calloc" | "realloc" | "strdup" | "strndup"
                );
            }
        }
        false
    }

    fn get_allocation_type(&self, node: &Node, source: &str) -> String {
        // Handle cast expressions like (char *)malloc(...)
        if node.kind() == "cast_expression" {
            if let Some(value) = node.child_by_field_name("value") {
                return self.get_allocation_type(&value, source);
            }
        }

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
            _ => "unknown".to_string(),
        }
    }

    fn detect_leaks(&self, violations: &mut Vec<RuleViolation>) {
        for (var_name, alloc_info) in &self.allocated_memory {
            if !self.freed_memory.contains_key(var_name) && !self.escaped_memory.contains(var_name)
            {
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
                    suggestion: Some(format!(
                        "Add 'free({})' before the variable goes out of scope",
                        var_name
                    )),
                    ..Default::default()
                });
            }
        }
    }

    /// Check if a node contains a return statement
    fn block_has_return(&self, node: &Node) -> bool {
        if node.kind() == "return_statement" {
            return true;
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if self.block_has_return(&child) {
                    return true;
                }
            }
        }

        false
    }
}

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
            escaped_memory: HashSet::new(),
        }
    }

    fn analyze_function(
        &mut self,
        func_node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let Some(body) = func_node.child_by_field_name("body") {
            // First pass: collect all allocations
            self.collect_allocations(&body, source);

            // Second pass: analyze control flow paths for leaks
            self.analyze_paths(&body, source, violations);
        }
    }

    fn collect_allocations(&mut self, node: &Node, source: &str) {
        match node.kind() {
            "declaration" => {
                self.process_declaration(node, source);
            }
            "assignment_expression" => {
                self.process_assignment_alloc(node, source);
            }
            _ => {
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        self.collect_allocations(&child, source);
                    }
                }
            }
        }
    }

    fn process_declaration(&mut self, node: &Node, source: &str) {
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
                                    var_name,
                                    AllocInfo {
                                        line: pos.row + 1,
                                        column: pos.column + 1,
                                        alloc_type,
                                    },
                                );
                            }
                        }
                    }
                }
            }
        }
    }

    fn process_assignment_alloc(&mut self, node: &Node, source: &str) {
        if let (Some(left), Some(right)) = (
            node.child_by_field_name("left"),
            node.child_by_field_name("right"),
        ) {
            if left.kind() == "identifier" {
                let var_name = ast_utils::get_node_text_owned(&left, source);

                if self.is_allocation_call(&right, source) {
                    let pos = right.start_position();
                    let alloc_type = self.get_allocation_type(&right, source);
                    self.allocated_memory.insert(
                        var_name,
                        AllocInfo {
                            line: pos.row + 1,
                            column: pos.column + 1,
                            alloc_type,
                        },
                    );
                }
            }
        }
    }

    fn analyze_paths(&mut self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        let mut path_state = PathState::new();
        self.analyze_statement(node, source, &mut path_state, violations);

        // Check for leaks at implicit function exit (when function ends without explicit return)
        // But only if the function doesn't end with a return/exit statement
        if !self.ends_with_exit(node, source) {
            self.detect_leaks_at_exit(&path_state, violations);
        }
    }

    fn analyze_statement(
        &mut self,
        node: &Node,
        source: &str,
        path_state: &mut PathState,
        violations: &mut Vec<RuleViolation>,
    ) {
        match node.kind() {
            "return_statement" => {
                self.process_return(node, source, path_state);
                self.detect_leaks_at_exit(path_state, violations);
            }
            "expression_statement" => {
                // Process expression statements (e.g., free(buffer);)
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        if child.kind() == "call_expression" {
                            self.process_call(&child, source, path_state);
                            // Check if this is an exit/abort call
                            if let Some(function) = child.child_by_field_name("function") {
                                let func_name = ast_utils::get_node_text_owned(&function, source);
                                if matches!(
                                    func_name.as_str(),
                                    "exit" | "abort" | "_exit" | "_Exit"
                                ) {
                                    self.detect_leaks_at_exit(path_state, violations);
                                }
                            }
                        } else {
                            self.analyze_statement(&child, source, path_state, violations);
                        }
                    }
                }
            }
            "call_expression" => {
                self.process_call(node, source, path_state);
                // Check if this is an exit/abort call
                if let Some(function) = node.child_by_field_name("function") {
                    let func_name = ast_utils::get_node_text_owned(&function, source);
                    if matches!(func_name.as_str(), "exit" | "abort" | "_exit" | "_Exit") {
                        self.detect_leaks_at_exit(path_state, violations);
                    }
                }
            }
            "if_statement" => {
                self.process_if_statement(node, source, path_state, violations);
            }
            "while_statement" | "for_statement" | "do_statement" => {
                // For loops, analyze the body
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        self.analyze_statement(&child, source, path_state, violations);
                    }
                }
            }
            "compound_statement" => {
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        self.analyze_statement(&child, source, path_state, violations);
                    }
                }
            }
            _ => {
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        self.analyze_statement(&child, source, path_state, violations);
                    }
                }
            }
        }
    }

    fn process_if_statement(
        &mut self,
        node: &Node,
        source: &str,
        path_state: &mut PathState,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Save current path state
        let state_before = path_state.clone();

        // Analyze consequence (then branch)
        if let Some(consequence) = node.child_by_field_name("consequence") {
            let mut then_state = state_before.clone();
            self.analyze_statement(&consequence, source, &mut then_state, violations);

            // Check if then branch has an early return/exit
            let then_has_exit = self.has_exit_statement(&consequence, source);

            // Analyze alternative (else branch)
            if let Some(alternative) = node.child_by_field_name("alternative") {
                let mut else_state = state_before.clone();
                self.analyze_statement(&alternative, source, &mut else_state, violations);

                let else_has_exit = self.has_exit_statement(&alternative, source);

                // If then branch exits, continue with else state
                if then_has_exit && !else_has_exit {
                    *path_state = else_state;
                }
                // If else branch exits, continue with then state
                else if else_has_exit && !then_has_exit {
                    *path_state = then_state;
                }
                // If both exit, detect leaks on both paths
                else if then_has_exit && else_has_exit {
                    // Both paths exit - nothing continues
                }
                // If neither exits, we need to merge states
                // Check if memory is freed in only one branch
                else {
                    self.detect_partial_free(&then_state, &else_state, violations);
                    // Merge states - memory is freed only if freed in both
                    let freed_in_both: HashSet<String> = then_state
                        .freed_memory
                        .intersection(&else_state.freed_memory)
                        .cloned()
                        .collect();
                    path_state.freed_memory = freed_in_both;
                }
            } else {
                // No else branch
                if then_has_exit {
                    // Then branch exits, continue with original state
                    *path_state = state_before;
                } else {
                    // Check for leaks when memory is freed only in then branch
                    if !then_state
                        .freed_memory
                        .is_subset(&state_before.freed_memory)
                    {
                        // Memory freed in then but not guaranteed in all paths
                        self.detect_partial_free(&then_state, &state_before, violations);
                    }
                    // Continue with then state (optimistic)
                    *path_state = then_state;
                }
            }
        }
    }

    fn detect_partial_free(
        &self,
        state1: &PathState,
        state2: &PathState,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Find variables freed in one path but not the other
        let freed_in_one: HashSet<_> = state1
            .freed_memory
            .symmetric_difference(&state2.freed_memory)
            .collect();

        for var_name in freed_in_one {
            if let Some(alloc_info) = self.allocated_memory.get(var_name.as_str()) {
                if !self.escaped_memory.contains(var_name.as_str()) {
                    violations.push(RuleViolation {
                        rule_id: "MEM31-C".to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Memory allocated with '{}' for variable '{}' may not be freed on all execution paths",
                            alloc_info.alloc_type, var_name
                        ),
                        file_path: String::new(),
                        line: alloc_info.line,
                        column: alloc_info.column,
                        suggestion: Some(format!(
                            "Ensure 'free({})' is called on all execution paths",
                            var_name
                        )),
                        ..Default::default()
                    });
                }
            }
        }
    }

    fn has_exit_statement(&self, node: &Node, source: &str) -> bool {
        if node.kind() == "return_statement" {
            return true;
        }

        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = ast_utils::get_node_text_owned(&function, source);
                if matches!(
                    func_name.as_str(),
                    "exit" | "abort" | "_exit" | "_Exit" | "longjmp"
                ) {
                    return true;
                }
            }
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if self.has_exit_statement(&child, source) {
                    return true;
                }
            }
        }

        false
    }

    fn ends_with_exit(&self, node: &Node, source: &str) -> bool {
        // Check if the last statement in a compound block is a return/exit
        if node.kind() == "compound_statement" {
            let mut last_statement: Option<Node> = None;

            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    // Skip punctuation like { and }
                    if child.kind() != "{" && child.kind() != "}" {
                        last_statement = Some(child);
                    }
                }
            }

            if let Some(last_stmt) = last_statement {
                if last_stmt.kind() == "return_statement" {
                    return true;
                }

                if last_stmt.kind() == "expression_statement" {
                    for i in 0..last_stmt.child_count() {
                        if let Some(child) = last_stmt.child(i) {
                            if child.kind() == "call_expression" {
                                if let Some(function) = child.child_by_field_name("function") {
                                    let func_name =
                                        ast_utils::get_node_text_owned(&function, source);
                                    if matches!(
                                        func_name.as_str(),
                                        "exit" | "abort" | "_exit" | "_Exit"
                                    ) {
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        false
    }

    fn process_call(&self, node: &Node, source: &str, path_state: &mut PathState) {
        if let Some(function) = node.child_by_field_name("function") {
            let func_name = ast_utils::get_node_text_owned(&function, source);

            if func_name == "free" {
                if let Some(arguments) = node.child_by_field_name("arguments") {
                    for i in 0..arguments.child_count() {
                        if let Some(arg) = arguments.child(i) {
                            if arg.kind() == "identifier" {
                                let var_name = ast_utils::get_node_text_owned(&arg, source);
                                path_state.freed_memory.insert(var_name);
                            }
                        }
                    }
                }
            } else if func_name == "realloc" {
                if let Some(arguments) = node.child_by_field_name("arguments") {
                    let mut first_arg = String::new();
                    for i in 0..arguments.child_count() {
                        if let Some(arg) = arguments.child(i) {
                            if arg.kind() == "identifier" && first_arg.is_empty() {
                                first_arg = ast_utils::get_node_text_owned(&arg, source);
                                path_state.freed_memory.insert(first_arg.clone());
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    fn process_return(&mut self, node: &Node, source: &str, _path_state: &PathState) {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "identifier" {
                    let var_name = ast_utils::get_node_text_owned(&child, source);
                    if self.allocated_memory.contains_key(&var_name) {
                        self.escaped_memory.insert(var_name);
                    }
                } else if self.is_allocation_call(&child, source) {
                    // Direct return of allocation is not a leak
                }
            }
        }
    }

    fn detect_leaks_at_exit(&self, path_state: &PathState, violations: &mut Vec<RuleViolation>) {
        for (var_name, alloc_info) in &self.allocated_memory {
            if !path_state.freed_memory.contains(var_name)
                && !self.escaped_memory.contains(var_name)
            {
                violations.push(RuleViolation {
                    rule_id: "MEM31-C".to_string(),
                    severity: Severity::High,
                    message: format!(
                        "Memory allocated with '{}' for variable '{}' is not freed before function exit",
                        alloc_info.alloc_type, var_name
                    ),
                    file_path: String::new(),
                    line: alloc_info.line,
                    column: alloc_info.column,
                    suggestion: Some(format!(
                        "Add 'free({})' before the function exits",
                        var_name
                    )),
                    ..Default::default()
                });
            }
        }
    }

    fn is_allocation_call(&self, node: &Node, source: &str) -> bool {
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
}

#[derive(Clone)]
struct PathState {
    freed_memory: HashSet<String>,
}

impl PathState {
    fn new() -> Self {
        Self {
            freed_memory: HashSet::new(),
        }
    }
}

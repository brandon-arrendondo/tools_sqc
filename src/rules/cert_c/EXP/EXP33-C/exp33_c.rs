use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use std::collections::{HashMap, HashSet};
use tree_sitter::Node;

pub struct Exp33C;

impl CertRule for Exp33C {
    fn rule_id(&self) -> &'static str {
        "EXP33-C"
    }

    fn description(&self) -> &'static str {
        "Do not read uninitialized memory"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "EXP33-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // For translation unit, collect file-scope static/thread-local variables
        // and analyze all functions with access to those
        if node.kind() == "translation_unit" {
            let mut file_scope_vars: HashMap<String, VarState> = HashMap::new();

            // Pre-pass: scan all function definitions for interprocedural analysis
            let mut interprocedural_analyzer = UninitializedVariableAnalyzer::new();
            interprocedural_analyzer.scan_function_definitions(node, source);

            // First pass: collect file-scope static/thread-local declarations
            let mut file_scope_decls = Vec::new();
            Exp33C::collect_file_scope_declarations(node, &mut file_scope_decls);
            for child in &file_scope_decls {
                let decl_text = get_node_text(child, source);
                if decl_text.contains("static ")
                    || decl_text.contains("_Thread_local")
                    || decl_text.contains("__thread")
                {
                    // Check if it has an initializer
                    if !decl_text.contains('=') && !decl_text.contains('{') {
                        // Extract variable name
                        if let Some(var_name) =
                            Exp33C::extract_var_name_from_declaration(child, source)
                        {
                            file_scope_vars.insert(var_name, VarState::StaticUninitialized);
                        }
                    }
                }
            }

            // Second pass: analyze each function with file-scope vars
            let mut func_defs = Vec::new();
            Exp33C::collect_function_definitions(node, &mut func_defs);
            if !file_scope_vars.is_empty() {
                for func_def in &func_defs {
                    if let Some(body) = func_def.child_by_field_name("body") {
                        let mut analyzer = UninitializedVariableAnalyzer::new();
                        // Copy interprocedural analysis results
                        analyzer.realloc_wrapper_functions =
                            interprocedural_analyzer.realloc_wrapper_functions.clone();
                        analyzer.conditionally_init_functions = interprocedural_analyzer
                            .conditionally_init_functions
                            .clone();
                        // Add file-scope vars
                        for (name, state) in &file_scope_vars {
                            analyzer.var_states.insert(name.clone(), state.clone());
                        }
                        analyzer.collect_all_info(&body, source);
                        analyzer.check_usage(&body, source, &mut violations);
                    }
                }
            }

            // Third pass: analyze each function for local uninitialized variable usage
            for func_def in &func_defs {
                if let Some(body) = func_def.child_by_field_name("body") {
                    let mut analyzer = UninitializedVariableAnalyzer::new();
                    // Copy interprocedural analysis results
                    analyzer.realloc_wrapper_functions =
                        interprocedural_analyzer.realloc_wrapper_functions.clone();
                    analyzer.conditionally_init_functions = interprocedural_analyzer
                        .conditionally_init_functions
                        .clone();

                    // Two-pass analysis:
                    // Pass 1: Collect all declarations and track which get initialized
                    analyzer.collect_all_info(&body, source);

                    // Check for goto patterns that skip initializations
                    analyzer.check_goto_pattern(&body, source, &mut violations);

                    // Check for conditional initialization patterns
                    analyzer.check_conditional_init_pattern(&body, source);

                    // Pass 2: Check for reads of uninitialized variables
                    analyzer.check_usage(&body, source, &mut violations);
                }
            }

            // Don't recurse into children for translation_unit - we've handled everything
            return violations;
        }

        // Analyze function bodies for uninitialized variable usage
        if node.kind() == "function_definition" {
            if let Some(body) = node.child_by_field_name("body") {
                let mut analyzer = UninitializedVariableAnalyzer::new();

                // Two-pass analysis:
                // Pass 1: Collect all declarations and track which get initialized
                analyzer.collect_all_info(&body, source);

                // Check for goto patterns that skip initializations
                analyzer.check_goto_pattern(&body, source, &mut violations);

                // Check for conditional initialization patterns
                analyzer.check_conditional_init_pattern(&body, source);

                // Pass 2: Check for reads of uninitialized variables
                analyzer.check_usage(&body, source, &mut violations);
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

impl Exp33C {
    /// Recursively collect all `function_definition` nodes, including those nested
    /// inside preprocessor conditional blocks (`#ifdef`, `#ifndef`, `#if`, etc.).
    fn collect_function_definitions<'a>(node: &Node<'a>, funcs: &mut Vec<Node<'a>>) {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "function_definition" {
                    funcs.push(child);
                } else if child.kind().starts_with("preproc_") {
                    Self::collect_function_definitions(&child, funcs);
                }
            }
        }
    }

    /// Recursively collect all `declaration` nodes at file scope, including those
    /// nested inside preprocessor conditional blocks.
    fn collect_file_scope_declarations<'a>(node: &Node<'a>, decls: &mut Vec<Node<'a>>) {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "declaration" {
                    decls.push(child);
                } else if child.kind().starts_with("preproc_") {
                    Self::collect_file_scope_declarations(&child, decls);
                }
            }
        }
    }

    fn extract_var_name_from_declaration(decl: &Node, source: &str) -> Option<String> {
        for i in 0..decl.child_count() {
            if let Some(child) = decl.child(i) {
                match child.kind() {
                    "init_declarator" => {
                        if let Some(declarator) = child.child_by_field_name("declarator") {
                            return Self::get_declarator_name(&declarator, source);
                        }
                    }
                    "identifier" => {
                        return Some(get_node_text(&child, source).to_string());
                    }
                    "pointer_declarator" | "array_declarator" => {
                        return Self::get_declarator_name(&child, source);
                    }
                    _ => {}
                }
            }
        }
        None
    }

    fn get_declarator_name(declarator: &Node, source: &str) -> Option<String> {
        match declarator.kind() {
            "identifier" => Some(get_node_text(declarator, source).to_string()),
            "pointer_declarator" | "array_declarator" => {
                for i in 0..declarator.child_count() {
                    if let Some(child) = declarator.child(i) {
                        if child.kind() == "identifier" {
                            return Some(get_node_text(&child, source).to_string());
                        }
                        if let Some(name) = Self::get_declarator_name(&child, source) {
                            return Some(name);
                        }
                    }
                }
                None
            }
            _ => None,
        }
    }
}

#[allow(dead_code)]
#[derive(Clone, Debug, PartialEq)]
enum VarState {
    Uninitialized,
    Initialized,
    ConditionallyInitialized, // Initialized only in some paths
    MallocUninitialized,
    MallocInitialized,
    StaticUninitialized, // static/thread-local without explicit initializer
}

#[allow(dead_code)]
struct UninitializedVariableAnalyzer {
    var_states: HashMap<String, VarState>,
    malloc_pointers: HashSet<String>,
    initializing_functions: HashSet<String>,
    reported: HashSet<String>, // Track reported variables to avoid duplicates
    has_unconditional_init: HashSet<String>, // Variables with at least one unconditional assignment
    initially_uninitialized: HashSet<String>, // Variables that started without initializer
    unsigned_char_vars: HashSet<String>, // Variables of unsigned char type (EXP33-C exception)
    array_vars: HashSet<String>, // Variables declared as arrays (decay to pointers in calls)
    realloc_wrapper_functions: HashSet<String>, // Functions that return realloc results
    conditionally_init_functions: HashSet<String>, // Functions that conditionally init pointer params
}

impl UninitializedVariableAnalyzer {
    fn new() -> Self {
        let initializing_functions: HashSet<String> = [
            "memset",
            "memcpy",
            "memmove",
            "strcpy",
            "strncpy",
            "sprintf",
            "snprintf",
            "fgets",
            "fread",
            "read",
            "recv",
            "scanf",
            "fscanf",
            "sscanf",
            "gets",
            "bzero",
            "strcat",
            "strncat",
            // POSIX/system functions that initialize via output pointers
            "gettimeofday",
            "getaddrinfo",
            "stat",
            "fstat",
            "lstat",
            "getrusage",
            "getsockname",
            "getpeername",
            "clock_gettime",
            "pthread_attr_init",
            "pthread_mutex_init",
            "pthread_cond_init",
            "regcomp",
            "regexec",
            "sigaction",
            "sigemptyset",
            "sigfillset",
            "mbrlen",
            "mbrtowc",
            "mbsrtowcs",
            "wcrtomb",
            "wcsrtombs",
        ]
        .iter()
        .map(|s| s.to_string())
        .collect();

        Self {
            var_states: HashMap::new(),
            malloc_pointers: HashSet::new(),
            initializing_functions,
            reported: HashSet::new(),
            has_unconditional_init: HashSet::new(),
            initially_uninitialized: HashSet::new(),
            unsigned_char_vars: HashSet::new(),
            array_vars: HashSet::new(),
            realloc_wrapper_functions: HashSet::new(),
            conditionally_init_functions: HashSet::new(),
        }
    }

    /// Scan the translation unit for function definitions that wrap realloc
    /// or conditionally initialize pointer parameters
    fn scan_function_definitions(&mut self, translation_unit: &Node, source: &str) {
        let mut func_defs = Vec::new();
        Exp33C::collect_function_definitions(translation_unit, &mut func_defs);
        for func_def in &func_defs {
            self.analyze_function_def(func_def, source);
        }
    }

    fn analyze_function_def(&mut self, func_def: &Node, source: &str) {
        // Get function name from declarator
        let func_name = if let Some(declarator) = func_def.child_by_field_name("declarator") {
            Self::get_function_name(&declarator, source)
        } else {
            return;
        };

        if func_name.is_empty() {
            return;
        }

        if let Some(body) = func_def.child_by_field_name("body") {
            // Check if function returns realloc result
            if self.function_returns_realloc(&body, source) {
                self.realloc_wrapper_functions.insert(func_name.clone());
            }

            // Check if function has pointer parameters that are conditionally initialized
            if let Some(declarator) = func_def.child_by_field_name("declarator") {
                if self.has_conditional_pointer_init(&declarator, &body, source) {
                    self.conditionally_init_functions.insert(func_name);
                }
            }
        }
    }

    fn get_function_name(declarator: &Node, source: &str) -> String {
        match declarator.kind() {
            "identifier" => get_node_text(declarator, source).to_string(),
            "function_declarator" => {
                if let Some(inner) = declarator.child_by_field_name("declarator") {
                    Self::get_function_name(&inner, source)
                } else {
                    String::new()
                }
            }
            "pointer_declarator" => {
                for i in 0..declarator.child_count() {
                    if let Some(child) = declarator.child(i) {
                        let name = Self::get_function_name(&child, source);
                        if !name.is_empty() {
                            return name;
                        }
                    }
                }
                String::new()
            }
            _ => String::new(),
        }
    }

    /// Check if a function body returns a realloc result without initializing the new memory
    fn function_returns_realloc(&self, body: &Node, source: &str) -> bool {
        // First, find all variables assigned from realloc
        let mut realloc_vars: HashSet<String> = HashSet::new();
        Self::collect_realloc_vars(body, source, &mut realloc_vars);

        if realloc_vars.is_empty() {
            return false;
        }

        // Check if any realloc variable is initialized with memset before return
        // If there's a memset call on the realloc'd memory, it's NOT a pure realloc wrapper
        let body_text = get_node_text(body, source);
        if body_text.contains("memset(") {
            // The function initializes the memory - not a pure realloc wrapper
            return false;
        }

        // Then check if any return statement returns a realloc result or a realloc variable
        Self::find_realloc_return(body, source, &realloc_vars)
    }

    fn collect_realloc_vars(node: &Node, source: &str, realloc_vars: &mut HashSet<String>) {
        // Look for assignments like: var = realloc(...) or var = (type *)realloc(...)
        if node.kind() == "declaration" || node.kind() == "assignment_expression" {
            let text = get_node_text(node, source);
            if text.contains("realloc(") {
                // Extract variable name from left side
                if node.kind() == "assignment_expression" {
                    if let Some(left) = node.child_by_field_name("left") {
                        if left.kind() == "identifier" {
                            realloc_vars.insert(get_node_text(&left, source).to_string());
                        }
                    }
                } else if node.kind() == "declaration" {
                    // Extract from init_declarator
                    for i in 0..node.child_count() {
                        if let Some(child) = node.child(i) {
                            if child.kind() == "init_declarator" {
                                if let Some(declarator) = child.child_by_field_name("declarator") {
                                    let var_name = Self::get_var_name(&declarator, source);
                                    if var_name != "unknown" {
                                        realloc_vars.insert(var_name);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                Self::collect_realloc_vars(&child, source, realloc_vars);
            }
        }
    }

    fn find_realloc_return(node: &Node, source: &str, realloc_vars: &HashSet<String>) -> bool {
        if node.kind() == "return_statement" {
            let return_text = get_node_text(node, source);
            // Check if return value directly involves realloc
            if return_text.contains("realloc(") {
                return true;
            }
            // Check if returning a variable that was assigned from realloc
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() == "identifier" {
                        let var_name = get_node_text(&child, source);
                        if realloc_vars.contains(&var_name.to_string()) {
                            return true;
                        }
                    }
                }
            }
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if Self::find_realloc_return(&child, source, realloc_vars) {
                    return true;
                }
            }
        }
        false
    }

    /// Check if function has pointer parameters that are only conditionally initialized
    fn has_conditional_pointer_init(&self, declarator: &Node, body: &Node, source: &str) -> bool {
        // Get pointer parameter names
        let pointer_params = self.get_pointer_params(declarator, source);
        if pointer_params.is_empty() {
            return false;
        }

        // Check if any pointer param is written to only conditionally (if/else-if without else)
        for param in &pointer_params {
            if self.param_is_conditionally_initialized(param, body, source) {
                return true;
            }
        }
        false
    }

    fn get_pointer_params(&self, declarator: &Node, source: &str) -> Vec<String> {
        let mut params = Vec::new();

        if declarator.kind() == "function_declarator" {
            if let Some(params_node) = declarator.child_by_field_name("parameters") {
                Self::collect_pointer_params(&params_node, source, &mut params);
            }
        } else {
            for i in 0..declarator.child_count() {
                if let Some(child) = declarator.child(i) {
                    params.extend(self.get_pointer_params(&child, source));
                }
            }
        }
        params
    }

    fn collect_pointer_params(node: &Node, source: &str, params: &mut Vec<String>) {
        if node.kind() == "parameter_declaration" {
            // Check if this is a pointer parameter
            let param_text = get_node_text(node, source);
            if param_text.contains('*') {
                // Extract parameter name
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        if child.kind() == "pointer_declarator" || child.kind() == "identifier" {
                            let name = Self::get_var_name(&child, source);
                            if name != "unknown" {
                                params.push(name);
                            }
                        }
                    }
                }
            }
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                Self::collect_pointer_params(&child, source, params);
            }
        }
    }

    fn param_is_conditionally_initialized(&self, param: &str, body: &Node, source: &str) -> bool {
        // Find all writes to *param
        let writes = self.find_all_pointer_writes(param, body, source);
        if writes.is_empty() {
            return false; // No writes at all - not relevant
        }

        // Check if ALL writes are inside incomplete conditionals (if/else-if without else)
        writes
            .iter()
            .all(|pos| self.is_inside_incomplete_conditional(*pos, body, source))
    }

    fn find_all_pointer_writes(&self, param: &str, node: &Node, source: &str) -> Vec<usize> {
        let mut writes = Vec::new();
        Self::collect_pointer_writes(param, node, source, &mut writes);
        writes
    }

    fn collect_pointer_writes(param: &str, node: &Node, source: &str, writes: &mut Vec<usize>) {
        if node.kind() == "assignment_expression" {
            if let Some(left) = node.child_by_field_name("left") {
                // Check for *param = value
                if left.kind() == "pointer_expression" {
                    let left_text = get_node_text(&left, source);
                    if left_text.starts_with('*') {
                        if let Some(arg) = left.child_by_field_name("argument") {
                            if get_node_text(&arg, source) == param {
                                writes.push(node.start_byte());
                            }
                        }
                    }
                }
            }
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                Self::collect_pointer_writes(param, &child, source, writes);
            }
        }
    }

    /// Pass 1: Collect all declarations, assignments, and function calls that initialize
    fn collect_all_info(&mut self, node: &Node, source: &str) {
        match node.kind() {
            "declaration" => {
                self.process_declaration(node, source);
            }
            "assignment_expression" => {
                self.process_assignment(node, source);
            }
            "call_expression" => {
                self.process_init_call(node, source);
            }
            "update_expression" => {
                // i++ or ++i doesn't initialize, it reads
            }
            _ => {}
        }

        // Recursively process children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.collect_all_info(&child, source);
            }
        }
    }

    /// Check for conditional initialization patterns (if/else-if without else, switch without default)
    fn check_conditional_init_pattern(&mut self, node: &Node, source: &str) {
        // Find variables that are initialized only in conditional branches
        // and mark them as ConditionallyInitialized

        for (var_name, _state) in self.var_states.clone() {
            if !self.initially_uninitialized.contains(&var_name) {
                continue; // Skip variables that were initialized at declaration
            }

            // Check if this variable is assigned only inside conditionals
            let assignments = self.find_all_assignments(&var_name, node, source);
            // Also check for function-call-based initializations
            let func_inits = self.find_all_init_func_calls(&var_name, node, source);

            // Combine all initialization positions, deduplicating by byte offset so that
            // a single call site (e.g. fgets which pushes twice for an identifier arg)
            // counts as one initialization.
            let mut seen: HashSet<usize> = HashSet::new();
            let all_inits: Vec<usize> = assignments
                .into_iter()
                .chain(func_inits.into_iter())
                .filter(|pos| seen.insert(*pos))
                .collect();
            if all_inits.is_empty() {
                continue; // No initializations found
            }

            // Check if ALL initializations are inside incomplete conditionals
            let all_conditional = all_inits
                .iter()
                .all(|pos| self.is_inside_incomplete_conditional(*pos, node, source));

            if all_conditional {
                if all_inits.len() >= 2 {
                    // 2+ initializations across conditional branches suggests exhaustive
                    // coverage (e.g. if/else-if chains where all enum values are handled).
                    // Downgrade to ConditionallyInitialized rather than Uninitialized to
                    // avoid false positives; check_usage does not flag this state.
                    self.var_states
                        .insert(var_name, VarState::ConditionallyInitialized);
                } else {
                    self.var_states.insert(var_name, VarState::Uninitialized);
                }
            }
        }
    }

    /// Find all positions where a variable is initialized via function call
    fn find_all_init_func_calls(&self, var_name: &str, node: &Node, source: &str) -> Vec<usize> {
        let mut positions = Vec::new();
        self.collect_init_func_calls_for_var(var_name, node, source, &mut positions);
        positions
    }

    fn collect_init_func_calls_for_var(
        &self,
        var_name: &str,
        node: &Node,
        source: &str,
        positions: &mut Vec<usize>,
    ) {
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = get_node_text(&function, source).to_string();
                if self.initializing_functions.contains(&func_name) {
                    if let Some(args) = node.child_by_field_name("arguments") {
                        let output_indices = self.get_output_arg_indices(&func_name);
                        let mut arg_idx = 0;
                        for i in 0..args.child_count() {
                            if let Some(arg) = args.child(i) {
                                if arg.kind() != "(" && arg.kind() != ")" && arg.kind() != "," {
                                    if output_indices.contains(&arg_idx) {
                                        let extracted_var = self.extract_var_from_arg(&arg, source);
                                        if extracted_var == var_name {
                                            positions.push(node.start_byte());
                                        }
                                        // Also check for direct identifier
                                        if arg.kind() == "identifier" {
                                            let arg_name = get_node_text(&arg, source);
                                            if arg_name == var_name {
                                                positions.push(node.start_byte());
                                            }
                                        }
                                    }
                                    arg_idx += 1;
                                }
                            }
                        }
                    }
                }
            }
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.collect_init_func_calls_for_var(var_name, &child, source, positions);
            }
        }
    }

    /// Find all assignment positions for a variable
    fn find_all_assignments(&self, var_name: &str, node: &Node, source: &str) -> Vec<usize> {
        let mut positions = Vec::new();
        Self::collect_assignments_for_var(var_name, node, source, &mut positions);
        positions
    }

    fn collect_assignments_for_var(
        var_name: &str,
        node: &Node,
        source: &str,
        positions: &mut Vec<usize>,
    ) {
        if node.kind() == "assignment_expression" {
            if let Some(left) = node.child_by_field_name("left") {
                if left.kind() == "identifier" {
                    let name = get_node_text(&left, source);
                    if name == var_name {
                        positions.push(node.start_byte());
                    }
                }
            }
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                Self::collect_assignments_for_var(var_name, &child, source, positions);
            }
        }
    }

    /// Check if a position is inside an incomplete conditional (if without else, switch without default)
    fn is_inside_incomplete_conditional(&self, pos: usize, node: &Node, source: &str) -> bool {
        // Check ALL enclosing conditionals, not just the innermost
        // A variable is conditionally initialized if ANY enclosing conditional is incomplete
        // and the position is in the body (not the condition) of that incomplete conditional
        let conditionals = self.find_all_enclosing_conditionals(pos, node);

        for conditional in conditionals {
            match conditional.kind() {
                "if_statement" => {
                    // Check if position is in the condition vs the body
                    // The condition always executes, so we only care about the body
                    if !Self::is_in_if_condition(pos, &conditional) {
                        // Position is in the body - check if this if has else
                        if !Self::if_chain_has_else(&conditional) {
                            return true; // In body of if without else
                        }
                    }
                }
                "switch_statement" => {
                    // Check if switch has a default case
                    if !self.switch_has_default(&conditional, source) {
                        return true;
                    }
                }
                _ => {}
            }
        }
        false
    }

    /// Find all enclosing conditionals (from innermost to outermost)
    fn find_all_enclosing_conditionals<'a>(&self, pos: usize, node: &Node<'a>) -> Vec<Node<'a>> {
        let mut result = Vec::new();
        Self::collect_enclosing_conditionals(pos, node, &mut result);
        result
    }

    fn collect_enclosing_conditionals<'a>(pos: usize, node: &Node<'a>, result: &mut Vec<Node<'a>>) {
        // Check if current node contains the position
        if pos < node.start_byte() || pos >= node.end_byte() {
            return;
        }

        // If this node is a conditional, add it
        if node.kind() == "if_statement" || node.kind() == "switch_statement" {
            result.push(*node);
        }

        // Recursively check children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                Self::collect_enclosing_conditionals(pos, &child, result);
            }
        }
    }

    /// Check if position is inside the condition clause of an if statement (not the body)
    fn is_in_if_condition(pos: usize, if_node: &Node) -> bool {
        // The condition is the parenthesized_expression child
        if let Some(condition) = if_node.child_by_field_name("condition") {
            return pos >= condition.start_byte() && pos < condition.end_byte();
        }
        false
    }

    #[allow(dead_code)]
    fn find_enclosing_conditional<'a>(pos: usize, node: &Node<'a>) -> Option<Node<'a>> {
        // Check if current node contains the position
        if pos < node.start_byte() || pos >= node.end_byte() {
            return None;
        }

        // Check children first (to find innermost)
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if let Some(found) = Self::find_enclosing_conditional(pos, &child) {
                    return Some(found);
                }
            }
        }

        // Then check if this node is a conditional
        if node.kind() == "if_statement" || node.kind() == "switch_statement" {
            return Some(*node);
        }

        None
    }

    fn if_chain_has_else(if_node: &Node) -> bool {
        // An if statement has: condition, consequence, and optionally alternative
        // The alternative can be another if_statement (else if), else_clause, or a compound_statement (else)
        if let Some(alt) = if_node.child_by_field_name("alternative") {
            if alt.kind() == "if_statement" {
                // It's an else-if directly, check recursively
                return Self::if_chain_has_else(&alt);
            } else if alt.kind() == "else_clause" {
                // Wrapped in else_clause - check if it contains an if_statement (else if)
                for i in 0..alt.child_count() {
                    if let Some(child) = alt.child(i) {
                        if child.kind() == "if_statement" {
                            // It's an else-if, check recursively
                            return Self::if_chain_has_else(&child);
                        }
                    }
                }
                // else_clause without if_statement = plain else
                return true;
            } else {
                // It's a plain else clause (compound_statement, etc.)
                return true;
            }
        }
        false
    }

    fn switch_has_default(&self, switch_node: &Node, source: &str) -> bool {
        // Look for "default:" in the switch body
        if let Some(body) = switch_node.child_by_field_name("body") {
            let body_text = get_node_text(&body, source);
            return body_text.contains("default:");
        }
        false
    }

    /// Check if function contains goto that could skip initializations
    fn check_goto_pattern(
        &mut self,
        node: &Node,
        source: &str,
        _violations: &mut Vec<RuleViolation>,
    ) {
        // Find goto statements, labels, declarations with initializers, and assignments
        let mut gotos: Vec<(String, usize)> = Vec::new(); // (target label, goto position)
        let mut labels: HashMap<String, usize> = HashMap::new(); // label name -> position
        let mut decls_with_init: Vec<(String, usize)> = Vec::new(); // (var name, position)
        let mut assignments: Vec<(String, usize)> = Vec::new(); // (var name, assignment position)

        Self::collect_goto_info(
            node,
            source,
            &mut gotos,
            &mut labels,
            &mut decls_with_init,
            &mut assignments,
        );

        // Check if any goto can skip an initialization (declaration with initializer)
        for (goto_target, goto_pos) in &gotos {
            if let Some(&label_pos) = labels.get(goto_target) {
                // goto at goto_pos jumps to label at label_pos
                // Check if any declaration with initializer is between them (but could be skipped)
                for (var_name, decl_pos) in &decls_with_init {
                    // If goto is before the declaration, and label is after
                    // Then the goto can skip the initialization
                    if *goto_pos < *decl_pos && *decl_pos < label_pos {
                        // This declaration can be skipped!
                        if self.var_states.contains_key(var_name) {
                            self.var_states
                                .insert(var_name.clone(), VarState::Uninitialized);
                        }
                    }
                }
            }
        }

        // Check if any goto can reach a label without going through an assignment
        // Only for variables that were declared without initializer
        for (var_name, state) in self.var_states.clone() {
            // Only check variables that:
            // 1. Are currently marked as Initialized (via later assignment)
            // 2. Were initially declared without initializer
            if state == VarState::Initialized && self.initially_uninitialized.contains(&var_name) {
                // Check if this variable was assigned (not initialized at declaration)
                // and if there's a path via goto that skips the assignment
                let var_assignments: Vec<usize> = assignments
                    .iter()
                    .filter(|(name, _)| name == &var_name)
                    .map(|(_, pos)| *pos)
                    .collect();

                if var_assignments.is_empty() {
                    continue; // No assignments found
                }

                // Check each goto - if it can reach a label without going through any assignment
                for (goto_target, goto_pos) in &gotos {
                    if let Some(&label_pos) = labels.get(goto_target) {
                        // Check if this goto can reach the label without passing through any assignment
                        let skips_all_assignments = var_assignments.iter().all(|&assign_pos| {
                            // The goto skips this assignment if:
                            // - goto is before assignment AND label is before assignment (goto jumps over)
                            // - OR assignment is before goto (goto doesn't pass through it)
                            *goto_pos < assign_pos && label_pos <= assign_pos
                                || assign_pos < *goto_pos
                        });

                        if skips_all_assignments && label_pos > *goto_pos {
                            // This goto reaches the label without initializing the variable
                            self.var_states
                                .insert(var_name.clone(), VarState::Uninitialized);
                            break;
                        }
                    }
                }
            }
        }
    }

    fn collect_goto_info(
        node: &Node,
        source: &str,
        gotos: &mut Vec<(String, usize)>,
        labels: &mut HashMap<String, usize>,
        decls_with_init: &mut Vec<(String, usize)>,
        assignments: &mut Vec<(String, usize)>,
    ) {
        match node.kind() {
            "goto_statement" => {
                // Extract target label - can be statement_identifier or identifier
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        if child.kind() == "statement_identifier" || child.kind() == "identifier" {
                            let label_name = get_node_text(&child, source).to_string();
                            gotos.push((label_name, node.start_byte()));
                            break;
                        }
                    }
                }
            }
            "labeled_statement" => {
                // Extract label name - the first child should be the label
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        // Try multiple possible node kinds for label
                        if child.kind() == "statement_identifier" || child.kind() == "identifier" {
                            let label_name = get_node_text(&child, source).to_string();
                            // Remove trailing colon if present
                            let clean_label = label_name.trim_end_matches(':').to_string();
                            labels.insert(clean_label, node.start_byte());
                            break;
                        }
                    }
                }
            }
            "declaration" => {
                let decl_text = get_node_text(node, source);
                // Check if it has an initializer
                if decl_text.contains('=') {
                    // Extract variable name
                    for i in 0..node.child_count() {
                        if let Some(child) = node.child(i) {
                            if child.kind() == "init_declarator" {
                                if let Some(declarator) = child.child_by_field_name("declarator") {
                                    let var_name = Self::get_var_name(&declarator, source);
                                    if var_name != "unknown" {
                                        decls_with_init.push((var_name, node.start_byte()));
                                    }
                                }
                            }
                        }
                    }
                }
            }
            "assignment_expression" => {
                // Track assignments for goto path analysis
                if let Some(left) = node.child_by_field_name("left") {
                    if left.kind() == "identifier" {
                        let var_name = get_node_text(&left, source).to_string();
                        assignments.push((var_name, node.start_byte()));
                    }
                }
            }
            _ => {}
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                Self::collect_goto_info(
                    &child,
                    source,
                    gotos,
                    labels,
                    decls_with_init,
                    assignments,
                );
            }
        }
    }

    fn process_declaration(&mut self, node: &Node, source: &str) {
        // Check if this is an array or struct with initializer like {0} or {1, 2, 3}
        let decl_text = get_node_text(node, source);

        // Check for static or _Thread_local specifier
        let is_static_or_thread_local = decl_text.contains("static ")
            || decl_text.contains("_Thread_local")
            || decl_text.contains("__thread");

        // Check for unsigned char type (EXP33-C exception)
        let is_unsigned_char = decl_text.contains("unsigned char");

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "init_declarator" {
                    if let Some(declarator) = child.child_by_field_name("declarator") {
                        let var_name = Self::get_var_name(&declarator, source);

                        // Track array variables
                        if declarator.kind() == "array_declarator" {
                            self.array_vars.insert(var_name.clone());
                        }

                        if let Some(value) = child.child_by_field_name("value") {
                            let value_text = get_node_text(&value, source);

                            if value_text.contains("malloc(") && !value_text.contains("calloc(") {
                                self.var_states
                                    .insert(var_name.clone(), VarState::MallocUninitialized);
                                self.malloc_pointers.insert(var_name);
                            } else if value_text.contains("calloc(") {
                                self.var_states
                                    .insert(var_name.clone(), VarState::MallocInitialized);
                                self.malloc_pointers.insert(var_name);
                            } else {
                                // Has initializer = initialized
                                self.var_states.insert(var_name, VarState::Initialized);
                            }
                        } else {
                            // No initializer - track as initially uninitialized
                            self.initially_uninitialized.insert(var_name.clone());
                            // Track unsigned char variables (EXP33-C exception)
                            if is_unsigned_char {
                                self.unsigned_char_vars.insert(var_name.clone());
                            }
                            if is_static_or_thread_local {
                                // Static/thread-local without explicit init - flag it
                                self.var_states
                                    .insert(var_name, VarState::StaticUninitialized);
                            } else {
                                self.var_states.insert(var_name, VarState::Uninitialized);
                            }
                        }
                    }
                } else if child.kind() == "pointer_declarator"
                    || child.kind() == "array_declarator"
                    || child.kind() == "identifier"
                {
                    // Direct declarator without init_declarator wrapper
                    // Check if the whole declaration has an initializer
                    if !decl_text.contains('=') && !decl_text.contains('{') {
                        let var_name = Self::get_var_name(&child, source);
                        if var_name != "unknown" {
                            // Track array variables
                            if child.kind() == "array_declarator" {
                                self.array_vars.insert(var_name.clone());
                            }
                            // Track as initially uninitialized
                            self.initially_uninitialized.insert(var_name.clone());
                            // Track unsigned char variables (EXP33-C exception)
                            if is_unsigned_char {
                                self.unsigned_char_vars.insert(var_name.clone());
                            }
                            if is_static_or_thread_local {
                                self.var_states
                                    .insert(var_name, VarState::StaticUninitialized);
                            } else {
                                self.var_states.insert(var_name, VarState::Uninitialized);
                            }
                        }
                    }
                }
            }
        }
    }

    fn process_assignment(&mut self, node: &Node, source: &str) {
        if let Some(left) = node.child_by_field_name("left") {
            let left_text = get_node_text(&left, source).to_string();

            if left.kind() == "subscript_expression" {
                if let Some(array) = left.child_by_field_name("argument") {
                    let array_name = get_node_text(&array, source).to_string();
                    if self.var_states.contains_key(&array_name) {
                        // Partial init for malloc'd memory
                        if self.malloc_pointers.contains(&array_name) {
                            self.var_states
                                .insert(array_name, VarState::MallocInitialized);
                        } else {
                            self.var_states.insert(array_name, VarState::Initialized);
                        }
                    }
                }
            } else if left.kind() == "identifier" {
                if let Some(right) = node.child_by_field_name("right") {
                    let right_text = get_node_text(&right, source);
                    if right_text.contains("malloc(") && !right_text.contains("calloc(") {
                        self.var_states
                            .insert(left_text.clone(), VarState::MallocUninitialized);
                        self.malloc_pointers.insert(left_text);
                    } else if right_text.contains("calloc(") {
                        self.var_states
                            .insert(left_text.clone(), VarState::MallocInitialized);
                        self.malloc_pointers.insert(left_text);
                    } else if right_text.contains("realloc(") {
                        // realloc can extend memory, and the extended portion is uninitialized
                        // Treat realloc'd memory as potentially uninitialized
                        self.var_states
                            .insert(left_text.clone(), VarState::MallocUninitialized);
                        self.malloc_pointers.insert(left_text);
                    } else if self.is_call_to_realloc_wrapper(&right, source) {
                        // Call to a function that wraps realloc - treat like realloc
                        self.var_states
                            .insert(left_text.clone(), VarState::MallocUninitialized);
                        self.malloc_pointers.insert(left_text);
                    } else {
                        // Don't upgrade static variables - they remain flagged
                        if let Some(current) = self.var_states.get(&left_text) {
                            if *current != VarState::StaticUninitialized {
                                self.var_states.insert(left_text, VarState::Initialized);
                            }
                        } else {
                            self.var_states.insert(left_text, VarState::Initialized);
                        }
                    }
                }
            } else if left.kind() == "pointer_expression" {
                if let Some(arg) = left.child_by_field_name("argument") {
                    let ptr_name = get_node_text(&arg, source).to_string();
                    if self.malloc_pointers.contains(&ptr_name) {
                        self.var_states
                            .insert(ptr_name, VarState::MallocInitialized);
                    }
                }
            } else if left.kind() == "field_expression" {
                // struct.field = value or ptr->field = value or arr[i].field = value
                // Recursively extract the base variable and mark it as initialized.
                // This handles: p.a = 1 (marks p), arr[0].a = 0 (marks arr).
                let base_name = Self::extract_base_pointer(&left, source);
                if !base_name.is_empty() {
                    if self.malloc_pointers.contains(&base_name) {
                        self.var_states
                            .insert(base_name, VarState::MallocInitialized);
                    } else if self.var_states.contains_key(&base_name) {
                        if let Some(current) = self.var_states.get(&base_name) {
                            if *current != VarState::StaticUninitialized {
                                self.var_states.insert(base_name, VarState::Initialized);
                            }
                        }
                    }
                }
            }
        }
    }

    fn process_init_call(&mut self, node: &Node, source: &str) {
        if let Some(function) = node.child_by_field_name("function") {
            let func_name = get_node_text(&function, source).to_string();

            if let Some(args) = node.child_by_field_name("arguments") {
                // For known initializing functions, mark their output pointers as initialized
                if self.initializing_functions.contains(&func_name) {
                    // Determine which argument(s) are output pointers based on the function
                    let output_arg_indices = self.get_output_arg_indices(&func_name);

                    let mut arg_idx = 0;
                    for i in 0..args.child_count() {
                        if let Some(arg) = args.child(i) {
                            if arg.kind() != "(" && arg.kind() != ")" && arg.kind() != "," {
                                if output_arg_indices.contains(&arg_idx) {
                                    // This is an output argument - mark as initialized
                                    let var_name = self.extract_var_from_arg(&arg, source);
                                    if !var_name.is_empty() {
                                        if self.malloc_pointers.contains(&var_name) {
                                            self.var_states
                                                .insert(var_name, VarState::MallocInitialized);
                                        } else if self.var_states.contains_key(&var_name) {
                                            self.var_states.insert(var_name, VarState::Initialized);
                                        }
                                    }
                                }
                                arg_idx += 1;
                            }
                        }
                    }
                } else if !self.is_non_initializing_function(&func_name)
                    && !self.conditionally_init_functions.contains(&func_name)
                {
                    // For unknown functions (that aren't known to read from pointers
                    // AND aren't detected as conditionally initializing),
                    // treat any &var argument or array/pointer variable passed directly
                    // as potentially initialized. Arrays decay to pointers, so
                    // func(buf) is equivalent to func(&buf[0]).
                    for i in 0..args.child_count() {
                        if let Some(arg) = args.child(i) {
                            if arg.kind() == "pointer_expression" {
                                let arg_text = get_node_text(&arg, source);
                                if arg_text.starts_with('&') {
                                    let var_name = self.extract_var_from_arg(&arg, source);
                                    if !var_name.is_empty()
                                        && self.var_states.contains_key(&var_name)
                                    {
                                        self.var_states.insert(var_name, VarState::Initialized);
                                    }
                                }
                            } else if arg.kind() == "identifier" {
                                // Only mark array variables as initialized when passed
                                // by name — arrays decay to pointers and the function
                                // may write into the buffer. Scalar variables passed by
                                // value cannot be modified by the callee.
                                let var_name = get_node_text(&arg, source).to_string();
                                if self.array_vars.contains(&var_name)
                                    && self.var_states.contains_key(&var_name)
                                    && self.initially_uninitialized.contains(&var_name)
                                {
                                    self.var_states.insert(var_name, VarState::Initialized);
                                }
                            }
                        }
                    }
                }

                // Handle va_start specially - initializes the va_list
                if func_name == "va_start" {
                    for i in 0..args.child_count() {
                        if let Some(arg) = args.child(i) {
                            if arg.kind() != "(" && arg.kind() != ")" && arg.kind() != "," {
                                let var_name = get_node_text(&arg, source).to_string();
                                if self.var_states.contains_key(&var_name) {
                                    self.var_states.insert(var_name, VarState::Initialized);
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    /// Check if a function is known to READ from pointer arguments rather than initialize them
    fn is_non_initializing_function(&self, func_name: &str) -> bool {
        matches!(
            func_name,
            // Wide character functions that read mbstate_t
            "mbrlen" | "mbrtowc" | "mbsrtowcs" | "wcrtomb" | "wcsrtombs" |
            // Regex functions that read compiled pattern
            "regexec" |
            // Comparison functions that read from pointers
            "memcmp" | "strcmp" | "strncmp" | "wmemcmp" | "wcscmp" | "wcsncmp" |
            // Print functions that read from pointers (not sprintf/snprintf - they init first arg)
            "printf" | "fprintf" | "vprintf" | "vfprintf" |
            "puts" | "fputs" | "putchar" | "fputc" |
            // String length/search functions that read
            "strlen" | "wcslen" | "strchr" | "strrchr" | "strstr" | "strpbrk" |
            // Hash/checksum functions that read data
            "crc32" | "md5" | "sha1" | "sha256" |
            // Functions that read data for output
            "fwrite" | "write" | "send" | "sendto"
        )
    }

    /// Check if a node is a call expression to a realloc wrapper function
    fn is_call_to_realloc_wrapper(&self, node: &Node, source: &str) -> bool {
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = get_node_text(&function, source).to_string();
                return self.realloc_wrapper_functions.contains(&func_name);
            }
        }
        // Handle cast expressions: (int *)resize_array(...)
        if node.kind() == "cast_expression" {
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if self.is_call_to_realloc_wrapper(&child, source) {
                        return true;
                    }
                }
            }
        }
        false
    }

    /// Get which argument indices are output parameters for known functions
    fn get_output_arg_indices(&self, func_name: &str) -> Vec<usize> {
        match func_name {
            // First argument is output for most string/memory functions
            "memset" | "memcpy" | "memmove" | "strcpy" | "strncpy" | "sprintf" | "snprintf"
            | "strcat" | "strncat" | "bzero" => vec![0],
            // First argument is output for gets/fgets
            "fgets" | "gets" => vec![0],
            // These read into the first argument
            "fread" | "read" | "recv" => vec![0],
            // scanf family - variables are output (but we can't easily track format args)
            "scanf" | "fscanf" | "sscanf" => vec![], // Too complex to handle
            // POSIX functions with output pointers - first arg is usually output
            "gettimeofday" => vec![0],
            "getaddrinfo" => vec![3], // 4th arg (results) is output
            "stat" | "fstat" | "lstat" => vec![1], // 2nd arg (struct stat *) is output
            "getrusage" => vec![1],
            "getsockname" | "getpeername" => vec![1, 2],
            "clock_gettime" => vec![1],
            // pthread init functions
            "pthread_attr_init" | "pthread_mutex_init" | "pthread_cond_init" => vec![0],
            // Signal functions
            "sigaction" => vec![2], // 3rd arg (old action) is output
            "sigemptyset" | "sigfillset" => vec![0],
            // regex
            "regcomp" => vec![0],
            // These are NOT pure output - they may read from the state
            "mbrlen" | "mbrtowc" | "mbsrtowcs" | "wcrtomb" | "wcsrtombs" => vec![],
            "regexec" => vec![], // Reads compiled regex
            _ => vec![0],        // Default: first arg is output
        }
    }

    /// Extract variable name from an argument (handles &var, var, etc.)
    fn extract_var_from_arg(&self, arg: &Node, source: &str) -> String {
        if arg.kind() == "pointer_expression" {
            let arg_text = get_node_text(arg, source);
            if arg_text.starts_with('&') {
                if let Some(inner) = arg.child_by_field_name("argument") {
                    if inner.kind() == "identifier" {
                        return get_node_text(&inner, source).to_string();
                    }
                }
            }
        } else if arg.kind() == "identifier" {
            return get_node_text(arg, source).to_string();
        }
        String::new()
    }

    /// Pass 2: Check for reads of uninitialized variables
    fn check_usage(&mut self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            "identifier" => {
                self.check_identifier_read(node, source, violations);
            }
            "pointer_expression" => {
                self.check_pointer_deref(node, source, violations);
            }
            "subscript_expression" => {
                self.check_subscript_read(node, source, violations);
            }
            "field_expression" => {
                self.check_field_read(node, source, violations);
            }
            _ => {}
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_usage(&child, source, violations);
            }
        }
    }

    fn check_field_read(&mut self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check for field access on malloc'd struct: ptr->field or (*ptr).field
        if let Some(arg) = node.child_by_field_name("argument") {
            let base_name = if arg.kind() == "identifier" {
                get_node_text(&arg, source).to_string()
            } else if arg.kind() == "pointer_expression" {
                // (*ptr).field pattern
                if let Some(inner) = arg.child_by_field_name("argument") {
                    get_node_text(&inner, source).to_string()
                } else {
                    return;
                }
            } else {
                return;
            };

            if self.reported.contains(&base_name) {
                return;
            }

            if let Some(state) = self.var_states.get(&base_name) {
                if *state == VarState::MallocUninitialized
                    && self.is_field_read_context(node, source)
                {
                    let start = node.start_position();
                    violations.push(RuleViolation {
                        rule_id: "EXP33-C".to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Reading from uninitialized malloc'd struct through '{}'",
                            base_name
                        ),
                        file_path: String::new(),
                        line: start.row + 1,
                        column: start.column + 1,
                        suggestion: Some(
                            "Use calloc() or initialize struct fields before reading".to_string(),
                        ),
                        ..Default::default()
                    });
                    self.reported.insert(base_name);
                }
            }
        }
    }

    fn is_field_read_context(&self, node: &Node, _source: &str) -> bool {
        if let Some(parent) = node.parent() {
            match parent.kind() {
                "assignment_expression" => {
                    if let Some(left) = parent.child_by_field_name("left") {
                        return node.start_byte() != left.start_byte();
                    }
                    true
                }
                "init_declarator" => false,
                _ => true,
            }
        } else {
            true
        }
    }

    fn check_identifier_read(
        &mut self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let var_name = get_node_text(node, source).to_string();

        // Skip if already reported
        if self.reported.contains(&var_name) {
            return;
        }

        // Skip unsigned char variables (EXP33-C exception)
        if self.unsigned_char_vars.contains(&var_name) {
            return;
        }

        // Check if this is an uninitialized variable
        if let Some(state) = self.var_states.get(&var_name) {
            let is_uninit = matches!(
                state,
                VarState::Uninitialized | VarState::StaticUninitialized
            );

            if is_uninit {
                // Check if this is a read context (not assignment target, not declaration)
                if self.is_read_context_for_identifier(node, source) {
                    let start = node.start_position();
                    let msg = if *state == VarState::StaticUninitialized {
                        format!(
                            "Reading static/thread-local variable '{}' without explicit initialization",
                            var_name
                        )
                    } else {
                        format!("Reading uninitialized variable '{}'", var_name)
                    };
                    violations.push(RuleViolation {
                        rule_id: "EXP33-C".to_string(),
                        severity: Severity::High,
                        message: msg,
                        file_path: String::new(),
                        line: start.row + 1,
                        column: start.column + 1,
                        suggestion: Some("Initialize the variable before use".to_string()),
                        ..Default::default()
                    });
                    self.reported.insert(var_name);
                }
            }
        }
    }

    fn is_read_context_for_identifier(&self, node: &Node, source: &str) -> bool {
        if let Some(parent) = node.parent() {
            match parent.kind() {
                // Left side of assignment is not a read
                "assignment_expression" => {
                    if let Some(left) = parent.child_by_field_name("left") {
                        return node.start_byte() != left.start_byte();
                    }
                    true
                }
                // Declaration is not a read
                "init_declarator"
                | "declaration"
                | "declarator"
                | "pointer_declarator"
                | "array_declarator"
                | "parameter_declaration" => false,
                // sizeof doesn't read the value
                "sizeof_expression" => false,
                // Address-of (&var) doesn't read the value
                "unary_expression" => {
                    let parent_text = get_node_text(&parent, source);
                    !parent_text.starts_with('&')
                }
                // Function call argument is a read
                "argument_list" => {
                    // Check if it's passed as a pointer for initialization
                    if let Some(gp) = parent.parent() {
                        if gp.kind() == "call_expression" {
                            if let Some(func) = gp.child_by_field_name("function") {
                                let func_name = get_node_text(&func, source);
                                if self.initializing_functions.contains(func_name) {
                                    // Check if this is the first (destination) argument
                                    for i in 0..parent.child_count() {
                                        if let Some(arg) = parent.child(i) {
                                            if arg.kind() != "("
                                                && arg.kind() != ")"
                                                && arg.kind() != ","
                                            {
                                                if node.start_byte() == arg.start_byte() {
                                                    return false; // First arg of init func
                                                }
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    true
                }
                // field_expression: identifier is base of struct/pointer access.
                // Not a read if the field_expression itself is the LHS of an assignment
                // (e.g., `myUnion.field = x` — writing to the field, not reading myUnion).
                "field_expression" => {
                    if let Some(gp) = parent.parent() {
                        if gp.kind() == "assignment_expression" {
                            if let Some(left) = gp.child_by_field_name("left") {
                                if parent.start_byte() == left.start_byte() {
                                    return false; // e.g. myUnion.field = x
                                }
                            }
                        }
                    }
                    true
                }
                // These are read contexts
                "binary_expression"
                | "return_statement"
                | "condition_clause"
                | "parenthesized_expression"
                | "call_expression"
                | "subscript_expression"
                | "update_expression" => true,
                _ => true,
            }
        } else {
            true
        }
    }

    fn check_pointer_deref(
        &mut self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let node_text = get_node_text(node, source);

        // Only check dereferences (*ptr), not address-of (&var)
        if !node_text.starts_with('*') {
            return;
        }

        if let Some(arg) = node.child_by_field_name("argument") {
            let ptr_name = get_node_text(&arg, source).to_string();

            if self.reported.contains(&ptr_name) {
                return;
            }

            if let Some(state) = self.var_states.get(&ptr_name) {
                match state {
                    VarState::Uninitialized => {
                        let start = node.start_position();
                        violations.push(RuleViolation {
                            rule_id: "EXP33-C".to_string(),
                            severity: Severity::High,
                            message: format!("Dereferencing uninitialized pointer '{}'", ptr_name),
                            file_path: String::new(),
                            line: start.row + 1,
                            column: start.column + 1,
                            suggestion: Some(
                                "Initialize the pointer before dereferencing".to_string(),
                            ),
                            ..Default::default()
                        });
                        self.reported.insert(ptr_name);
                    }
                    VarState::MallocUninitialized => {
                        // Check if this is a read context
                        if self.is_deref_read_context(node, source) {
                            let start = node.start_position();
                            violations.push(RuleViolation {
                                rule_id: "EXP33-C".to_string(),
                                severity: Severity::High,
                                message: format!(
                                    "Reading from uninitialized malloc'd memory through '{}'",
                                    ptr_name
                                ),
                                file_path: String::new(),
                                line: start.row + 1,
                                column: start.column + 1,
                                suggestion: Some(
                                    "Use calloc() or initialize memory before reading".to_string(),
                                ),
                                ..Default::default()
                            });
                            self.reported.insert(ptr_name);
                        }
                    }
                    _ => {}
                }
            }
        }
    }

    fn is_deref_read_context(&self, node: &Node, _source: &str) -> bool {
        if let Some(parent) = node.parent() {
            match parent.kind() {
                "assignment_expression" => {
                    if let Some(left) = parent.child_by_field_name("left") {
                        return node.start_byte() != left.start_byte();
                    }
                    true
                }
                "init_declarator" => false,
                _ => true,
            }
        } else {
            true
        }
    }

    fn check_subscript_read(
        &mut self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let Some(array) = node.child_by_field_name("argument") {
            // Handle ptr[i], arr->field[i], or (*ptr)[i] patterns
            let base_name = Self::extract_base_pointer(&array, source);

            if base_name.is_empty() || self.reported.contains(&base_name) {
                return;
            }

            // Special case: Flexible array member access pattern (ptr->field[i])
            // If the subscript argument is a field_expression and the base is malloc'd,
            // the flexible array portion may be uninitialized even if some fields were assigned
            let is_flex_array_access =
                array.kind() == "field_expression" && self.malloc_pointers.contains(&base_name);

            if let Some(state) = self.var_states.get(&base_name) {
                let is_uninit = *state == VarState::MallocUninitialized
                    || (is_flex_array_access && *state == VarState::MallocInitialized);

                if is_uninit && self.is_subscript_read_context(node, source) {
                    let start = node.start_position();
                    let msg = if is_flex_array_access {
                        format!(
                            "Reading from potentially uninitialized flexible array member through '{}'",
                            base_name
                        )
                    } else {
                        format!(
                            "Reading from uninitialized malloc'd memory through '{}'",
                            base_name
                        )
                    };
                    violations.push(RuleViolation {
                        rule_id: "EXP33-C".to_string(),
                        severity: Severity::High,
                        message: msg,
                        file_path: String::new(),
                        line: start.row + 1,
                        column: start.column + 1,
                        suggestion: Some(
                            "Use calloc() or initialize memory before reading".to_string(),
                        ),
                        ..Default::default()
                    });
                    self.reported.insert(base_name);
                }
            }
        }
    }

    /// Extract the base pointer from expressions like arr, arr->field, (*ptr), etc.
    fn extract_base_pointer(node: &Node, source: &str) -> String {
        match node.kind() {
            "identifier" => get_node_text(node, source).to_string(),
            "field_expression" => {
                // arr->field or obj.field - get the base
                if let Some(arg) = node.child_by_field_name("argument") {
                    Self::extract_base_pointer(&arg, source)
                } else {
                    String::new()
                }
            }
            "pointer_expression" => {
                // (*ptr) or &var
                if let Some(arg) = node.child_by_field_name("argument") {
                    Self::extract_base_pointer(&arg, source)
                } else {
                    String::new()
                }
            }
            "parenthesized_expression" => {
                // (ptr)
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        if child.kind() != "(" && child.kind() != ")" {
                            return Self::extract_base_pointer(&child, source);
                        }
                    }
                }
                String::new()
            }
            _ => String::new(),
        }
    }

    fn is_subscript_read_context(&self, node: &Node, source: &str) -> bool {
        if let Some(parent) = node.parent() {
            match parent.kind() {
                "assignment_expression" => {
                    if let Some(left) = parent.child_by_field_name("left") {
                        return node.start_byte() != left.start_byte();
                    }
                    true
                }
                "init_declarator" => false,
                // subscript inside field_expression LHS: data[0].field = x
                // The subscript is not being read — it's the write target's base.
                "field_expression" => {
                    if let Some(gp) = parent.parent() {
                        if gp.kind() == "assignment_expression" {
                            if let Some(left) = gp.child_by_field_name("left") {
                                if parent.start_byte() == left.start_byte() {
                                    return false; // e.g. data[i].field = x
                                }
                            }
                        }
                    }
                    true
                }
                "call_expression" => {
                    if let Some(func) = parent.child_by_field_name("function") {
                        let func_name = get_node_text(&func, source).to_string();
                        if self.initializing_functions.contains(&func_name) {
                            if let Some(args) = parent.child_by_field_name("arguments") {
                                for i in 0..args.child_count() {
                                    if let Some(arg) = args.child(i) {
                                        if arg.kind() != "("
                                            && arg.kind() != ")"
                                            && arg.kind() != ","
                                        {
                                            if node.start_byte() == arg.start_byte() {
                                                return false;
                                            }
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    true
                }
                _ => true,
            }
        } else {
            true
        }
    }

    fn get_var_name(declarator: &Node, source: &str) -> String {
        match declarator.kind() {
            "identifier" => get_node_text(declarator, source).to_string(),
            "pointer_declarator" | "array_declarator" => {
                for i in 0..declarator.child_count() {
                    if let Some(child) = declarator.child(i) {
                        if child.kind() == "identifier" {
                            return get_node_text(&child, source).to_string();
                        }
                        let nested = Self::get_var_name(&child, source);
                        if nested != "unknown" {
                            return nested;
                        }
                    }
                }
                "unknown".to_string()
            }
            _ => "unknown".to_string(),
        }
    }
}

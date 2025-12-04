use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use std::collections::{HashMap, HashSet};
use tree_sitter::Node;

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

        // Single pass: analyze the AST for use-after-free patterns
        analyzer.analyze_node(node, source, &mut violations);

        violations
    }
}

struct MemoryAnalyzer {
    // Track which variables are currently freed
    freed_vars: HashSet<String>,
    // Track aliases: if alias = ptr, then aliases["alias"] = "ptr"
    aliases: HashMap<String, String>,
    // Track which variables have been set to NULL after free
    nullified_vars: HashSet<String>,
    // Track realloc old pointers that have been updated to new pointer
    realloc_updated: HashSet<String>,
    // Track realloc relationships: realloc_map[old_ptr] = new_ptr
    // When we see new_ptr = realloc(old_ptr, ...), old_ptr becomes potentially invalid
    realloc_invalidated: HashSet<String>,
    // Track union members - when one member is freed, all are freed
    union_members: HashMap<String, HashSet<String>>,
}

impl MemoryAnalyzer {
    fn new() -> Self {
        Self {
            freed_vars: HashSet::new(),
            aliases: HashMap::new(),
            nullified_vars: HashSet::new(),
            realloc_updated: HashSet::new(),
            realloc_invalidated: HashSet::new(),
            union_members: HashMap::new(),
        }
    }

    /// Main analysis entry point - recursively analyze the AST
    fn analyze_node(&mut self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            "function_definition" => {
                // Analyze each function with fresh state to avoid cross-function pollution
                let mut func_analyzer = MemoryAnalyzer::new();
                func_analyzer.analyze_function(node, source, violations);
                return; // Don't recurse further - function handled completely
            }
            _ => {}
        }

        // Recursively process child nodes (top-level traversal)
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.analyze_node(&child, source, violations);
            }
        }
    }

    /// Analyze a single function with isolated state
    fn analyze_function(&mut self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        self.analyze_function_body(node, source, violations);
    }

    /// Analyze nodes within a function
    fn analyze_function_body(
        &mut self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        match node.kind() {
            "if_statement" => {
                // Handle if-else with branch-sensitive analysis
                self.analyze_if_statement(node, source, violations);
                return; // Don't recurse - handled by analyze_if_statement
            }
            "call_expression" => {
                self.process_call_expression(node, source, violations);
            }
            "assignment_expression" => {
                self.process_assignment(node, source, violations);
            }
            "init_declarator" => {
                self.process_init_declarator(node, source, violations);
            }
            "pointer_expression" => {
                // Check for dereference of freed memory (*ptr)
                self.check_pointer_dereference(node, source, violations);
            }
            "subscript_expression" => {
                // Check for array access on freed memory (arr[i])
                self.check_subscript_access(node, source, violations);
                // Don't recurse into subscript - we already checked the argument
                // This prevents double-checking field expressions that are subscript arguments
                return;
            }
            "binary_expression" => {
                // Check for pointer arithmetic on freed memory (ptr + n)
                self.check_binary_expression(node, source, violations);
            }
            "return_statement" => {
                // Check for returning freed memory
                self.check_return_statement(node, source, violations);
            }
            "for_statement" => {
                // Check for dangerous loop free patterns
                self.check_for_loop_pattern(node, source, violations);
            }
            "field_expression" => {
                // Check for field access on freed memory (ptr->field)
                self.check_field_access(node, source, violations);
            }
            _ => {}
        }

        // Recursively process child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.analyze_function_body(&child, source, violations);
            }
        }
    }

    /// Analyze if-statement with branch-sensitive analysis
    fn analyze_if_statement(
        &mut self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // First analyze the condition (it's executed in the current state)
        if let Some(condition) = node.child_by_field_name("condition") {
            self.analyze_function_body(&condition, source, violations);
        }

        // Save state before branches
        let saved_freed = self.freed_vars.clone();
        let saved_nullified = self.nullified_vars.clone();
        let saved_aliases = self.aliases.clone();
        let saved_realloc_updated = self.realloc_updated.clone();
        let saved_realloc_invalidated = self.realloc_invalidated.clone();

        // Analyze the "consequence" (then branch)
        let mut then_returns = false;
        if let Some(consequence) = node.child_by_field_name("consequence") {
            self.analyze_function_body(&consequence, source, violations);
            then_returns = self.unconditionally_returns(&consequence);
        }

        // Save state after then-branch
        let then_freed = self.freed_vars.clone();
        let then_nullified = self.nullified_vars.clone();
        let then_realloc_invalidated = self.realloc_invalidated.clone();
        let then_realloc_updated = self.realloc_updated.clone();

        // Reset state for else branch (starts from saved state)
        self.freed_vars = saved_freed.clone();
        self.nullified_vars = saved_nullified.clone();
        self.aliases = saved_aliases.clone();
        self.realloc_updated = saved_realloc_updated.clone();
        self.realloc_invalidated = saved_realloc_invalidated.clone();

        // Analyze the "alternative" (else branch) if present
        let mut else_returns = false;
        if let Some(alternative) = node.child_by_field_name("alternative") {
            self.analyze_function_body(&alternative, source, violations);
            else_returns = self.unconditionally_returns(&alternative);
        }

        let else_freed = self.freed_vars.clone();
        let else_nullified = self.nullified_vars.clone();
        let else_realloc_invalidated = self.realloc_invalidated.clone();
        let else_realloc_updated = self.realloc_updated.clone();

        // Merge states based on which branches return
        if then_returns && else_returns {
            // Both branches return - code after is unreachable, keep saved state
            self.freed_vars = saved_freed;
            self.nullified_vars = saved_nullified;
            self.realloc_invalidated = saved_realloc_invalidated;
            self.realloc_updated = saved_realloc_updated;
        } else if then_returns {
            // Only then returns - use else branch state
            self.freed_vars = else_freed;
            self.nullified_vars = else_nullified;
            self.realloc_invalidated = else_realloc_invalidated;
            self.realloc_updated = else_realloc_updated;
        } else if else_returns {
            // Only else returns - use then branch state
            self.freed_vars = then_freed;
            self.nullified_vars = then_nullified;
            self.realloc_invalidated = then_realloc_invalidated;
            self.realloc_updated = then_realloc_updated;
        } else {
            // Neither returns - merge states
            // For use-after-free detection: if freed in EITHER branch, it's potentially freed after
            // This ensures we catch use-after-free even on conditional frees
            self.freed_vars = then_freed;
            for var in else_freed {
                self.freed_vars.insert(var);
            }
            // But remove vars that were nullified in both branches
            for var in saved_nullified.iter() {
                if then_nullified.contains(var) && else_nullified.contains(var) {
                    self.freed_vars.remove(var);
                }
            }
            // Union of nullified
            self.nullified_vars = then_nullified;
            for var in else_nullified {
                self.nullified_vars.insert(var);
            }
            // For realloc_invalidated: use union (if invalidated in either branch, could be invalid)
            // This is conservative for detecting use-after-free
            self.realloc_invalidated = then_realloc_invalidated;
            for var in else_realloc_invalidated {
                self.realloc_invalidated.insert(var);
            }
            // Union of realloc_updated
            self.realloc_updated = then_realloc_updated;
            for var in else_realloc_updated {
                self.realloc_updated.insert(var);
            }
        }
    }

    /// Check if a branch unconditionally returns (all paths return)
    fn unconditionally_returns(&self, node: &Node) -> bool {
        match node.kind() {
            "return_statement" => true,
            "compound_statement" => {
                // A compound statement unconditionally returns if its last statement returns
                // or if it contains an unconditional return
                let mut last_child = None;
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        if child.kind() != "{" && child.kind() != "}" {
                            last_child = Some(child);
                        }
                    }
                }
                if let Some(last) = last_child {
                    self.unconditionally_returns(&last)
                } else {
                    false
                }
            }
            "if_statement" => {
                // An if-statement unconditionally returns only if BOTH branches unconditionally return
                let then_returns = node
                    .child_by_field_name("consequence")
                    .map(|c| self.unconditionally_returns(&c))
                    .unwrap_or(false);
                let else_returns = node
                    .child_by_field_name("alternative")
                    .map(|c| self.unconditionally_returns(&c))
                    .unwrap_or(false);
                then_returns && else_returns
            }
            _ => false,
        }
    }

    /// Check if a node contains a return statement
    fn contains_return(&self, node: &Node) -> bool {
        if node.kind() == "return_statement" {
            return true;
        }
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if self.contains_return(&child) {
                    return true;
                }
            }
        }
        false
    }

    /// Process function calls - free(), malloc(), printf(), etc.
    fn process_call_expression(
        &mut self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let Some(function_node) = node.child_by_field_name("function") {
            let function_name = get_node_text(&function_node, source);

            match function_name.as_ref() {
                "free" => {
                    self.process_free_call(node, source, violations);
                }
                "malloc" | "calloc" => {
                    // Allocation will be tracked via assignment
                }
                "realloc" => {
                    // For realloc, the original pointer may become invalid
                    // Track the old pointer as invalidated in case it's used
                    self.track_realloc_old_pointer(node, source);
                }
                _ => {
                    // Check for common free-related macros
                    let upper_name = function_name.to_uppercase();
                    if upper_name.contains("FREE")
                        || upper_name == "XFREE"
                        || upper_name == "G_FREE"
                        || upper_name == "SAFE_DELETE"
                        || upper_name == "DELETE"
                    {
                        // Treat as free() call
                        self.process_free_call(node, source, violations);
                    } else if upper_name.contains("REALLOC") {
                        // Treat as realloc() call - track old pointer as invalidated
                        // Don't check args for freed - realloc expects a possibly-allocated pointer
                        self.track_realloc_old_pointer(node, source);
                    } else {
                        // Check if any argument is a freed pointer
                        self.check_function_args_for_freed(node, source, violations);
                    }
                }
            }
        }
    }

    /// Process free() call - mark variable as freed
    fn process_free_call(
        &mut self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let Some(arguments) = node.child_by_field_name("arguments") {
            for i in 0..arguments.child_count() {
                if let Some(arg) = arguments.child(i) {
                    if arg.kind() == "," || arg.kind() == "(" || arg.kind() == ")" {
                        continue;
                    }

                    // For pointer dereference expressions like free(*ptr),
                    // the memory pointed to by *ptr is freed, not ptr itself.
                    // Skip tracking for these complex patterns to avoid false positives.
                    if arg.kind() == "pointer_expression" {
                        // We're freeing *ptr, not ptr. Skip tracking.
                        continue;
                    }

                    // For subscript expressions like free(arr[i]),
                    // the memory at arr[i] is freed, not arr itself.
                    // Skip tracking to avoid false positives.
                    if arg.kind() == "subscript_expression" {
                        // We're freeing arr[i], not arr. Skip tracking.
                        continue;
                    }

                    // For cast expressions like free((type)ptr), extract the inner value
                    let actual_arg = if arg.kind() == "cast_expression" {
                        if let Some(value) = arg.child_by_field_name("value") {
                            value
                        } else {
                            arg
                        }
                    } else {
                        arg
                    };

                    // For field expressions like free(data->name), track the full path
                    // not just the base variable
                    let (var_name, base_var) = if actual_arg.kind() == "field_expression" {
                        let full_path = get_node_text(&actual_arg, source).to_string();
                        // For union support: also track the base variable
                        // When free(u.member1) is called, u.member2 also becomes invalid
                        let base = self.extract_base_variable(&actual_arg, source);
                        (full_path, Some(base))
                    } else if actual_arg.kind() == "identifier" {
                        (get_node_text(&actual_arg, source).to_string(), None)
                    } else {
                        // For other complex expressions, skip to avoid false positives
                        continue;
                    };

                    if var_name.is_empty() {
                        continue;
                    }

                    // Resolve to canonical name (in case of alias)
                    let canonical = self.resolve_canonical(&var_name);

                    // Check for double-free (only check freed_vars, not realloc_invalidated)
                    // It's OK to free a realloc-invalidated pointer (that's expected when realloc fails)
                    if self.is_actually_freed(&canonical)
                        && !self.nullified_vars.contains(&canonical)
                    {
                        violations.push(RuleViolation {
                            rule_id: "MEM30-C".to_string(),
                            severity: Severity::Critical,
                            message: format!("Double-free: '{}' freed multiple times", var_name),
                            file_path: String::new(),
                            line: node.start_position().row + 1,
                            column: node.start_position().column + 1,
                            suggestion: Some(
                                "Set pointer to NULL after freeing to prevent double-free."
                                    .to_string(),
                            ),
                            ..Default::default()
                        });
                    }

                    // Mark as freed
                    self.freed_vars.insert(canonical.clone());
                    self.freed_vars.insert(var_name.clone());

                    // For union support: track union member relationships
                    // When free(u.member) is called, all u.* accesses become invalid
                    if let Some(base) = base_var {
                        if !base.is_empty() {
                            // Add to union tracking - all field accesses on this base are suspect
                            self.union_members
                                .entry(base.clone())
                                .or_default()
                                .insert(var_name.clone());
                        }
                    }

                    // Also mark any aliases as freed
                    let aliases_to_free: Vec<String> = self
                        .aliases
                        .iter()
                        .filter(|(_, v)| **v == canonical || **v == var_name)
                        .map(|(k, _)| k.clone())
                        .collect();
                    for alias in aliases_to_free {
                        self.freed_vars.insert(alias);
                    }
                }
            }
        }
    }

    /// Process assignment expression - track aliases and NULL assignments
    fn process_assignment(
        &mut self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let (Some(left), Some(right)) = (
            node.child_by_field_name("left"),
            node.child_by_field_name("right"),
        ) {
            // Get full path for field expressions (e.g., im->clip->list)
            let left_full_path = get_node_text(&left, source).to_string();

            // Check if assigning NULL - this clears freed status
            let right_text = get_node_text(&right, source);
            if right_text.trim() == "NULL" || right_text.trim() == "0" {
                // For field expressions like data->name = NULL, track the full path
                self.nullified_vars.insert(left_full_path.clone());
                self.freed_vars.remove(&left_full_path);
                self.realloc_invalidated.remove(&left_full_path);

                // Also track base variable
                let left_var = self.extract_base_variable(&left, source);
                if !left_var.is_empty() {
                    self.nullified_vars.insert(left_var.clone());
                    self.freed_vars.remove(&left_var);
                }
                return;
            }

            let left_var = self.extract_base_variable(&left, source);
            if left_var.is_empty() && left_full_path.is_empty() {
                return;
            }

            // Check if this is a dereference write (*ptr = value)
            if left.kind() == "pointer_expression" {
                // This is writing through a pointer
                if let Some(arg) = left.child_by_field_name("argument") {
                    let ptr_var = self.extract_base_variable(&arg, source);
                    if !ptr_var.is_empty() && self.is_freed(&ptr_var) {
                        violations.push(RuleViolation {
                            rule_id: "MEM30-C".to_string(),
                            severity: Severity::Critical,
                            message: format!(
                                "Use-after-free: writing to freed memory via '{}'",
                                ptr_var
                            ),
                            file_path: String::new(),
                            line: node.start_position().row + 1,
                            column: node.start_position().column + 1,
                            suggestion: Some("Do not access memory after freeing it.".to_string()),
                            ..Default::default()
                        });
                    }
                }
                return;
            }

            // Check if right side is a realloc result variable
            // If we're assigning a realloc result to the original pointer (ptr = new_ptr),
            // clear the freed status since the pointer is now valid again
            let right_var = self.extract_base_variable(&right, source);
            if !right_var.is_empty() {
                // Check if right_var was the result of a realloc on left_var
                // This handles: new_ptr = realloc(ptr, ...); ptr = new_ptr;
                // Also handles: im->clip->list = more; after more = gdRealloc(im->clip->list, ...)
                if self.realloc_updated.contains(&right_var) {
                    // Clear both base variable and full path
                    self.freed_vars.remove(&left_var);
                    self.nullified_vars.remove(&left_var);
                    self.realloc_invalidated.remove(&left_var);
                    // For field expressions, also clear the full path
                    self.freed_vars.remove(&left_full_path);
                    self.nullified_vars.remove(&left_full_path);
                    self.realloc_invalidated.remove(&left_full_path);
                    // Also clear any aliases pointing to the old value
                    self.aliases.remove(&left_var);
                }

                if self.is_freed(&right_var) {
                    // Creating an alias from freed pointer - the new variable is also freed
                    self.freed_vars.insert(left_var.clone());
                    self.aliases.insert(left_var.clone(), right_var.clone());
                } else if right.kind() == "identifier" {
                    // Track this as an alias
                    self.aliases.insert(left_var.clone(), right_var.clone());
                    // If original gets freed, alias should be considered freed too
                    if self.freed_vars.contains(&right_var) {
                        self.freed_vars.insert(left_var.clone());
                    }
                }
            }

            // Check if right side is pointer arithmetic on freed memory
            if right.kind() == "binary_expression" {
                self.check_binary_expression(&right, source, violations);
            }

            // Clear freed status if assigning new allocation
            if right.kind() == "call_expression" {
                if let Some(func) = right.child_by_field_name("function") {
                    let func_name = get_node_text(&func, source);
                    let upper_func_name = func_name.to_uppercase();
                    if func_name == "malloc" || func_name == "calloc" {
                        self.freed_vars.remove(&left_var);
                        self.nullified_vars.remove(&left_var);
                        self.realloc_invalidated.remove(&left_var);
                    } else if func_name == "realloc" || upper_func_name.contains("REALLOC") {
                        // Track the old pointer passed to realloc as invalidated
                        self.track_realloc_old_pointer(&right, source);
                        // For realloc, track that left_var is the result of realloc
                        self.realloc_updated.insert(left_var.clone());
                        self.freed_vars.remove(&left_var);
                        self.nullified_vars.remove(&left_var);
                        self.realloc_invalidated.remove(&left_var);
                    }
                }
            }
        }
    }

    /// Process variable initialization (int *p = ptr)
    fn process_init_declarator(
        &mut self,
        node: &Node,
        source: &str,
        _violations: &mut Vec<RuleViolation>,
    ) {
        if let (Some(declarator), Some(value)) = (
            node.child_by_field_name("declarator"),
            node.child_by_field_name("value"),
        ) {
            let left_var = self.extract_declarator_name(&declarator, source);
            if left_var.is_empty() {
                return;
            }

            // Check if this is a realloc initialization
            if value.kind() == "call_expression" {
                if let Some(func) = value.child_by_field_name("function") {
                    let func_name = get_node_text(&func, source);
                    let upper_func_name = func_name.to_uppercase();
                    if func_name == "realloc" || upper_func_name.contains("REALLOC") {
                        // Track that left_var is the result of realloc
                        self.realloc_updated.insert(left_var.clone());
                        // Also track what pointer was passed to realloc (it's now invalidated)
                        self.track_realloc_old_pointer(&value, source);
                        return;
                    } else if func_name == "malloc" || func_name == "calloc" {
                        // Fresh allocation, nothing special to track
                        return;
                    }
                }
            }

            // Check for cast expression wrapping a call
            if value.kind() == "cast_expression" {
                if let Some(inner_value) = value.child_by_field_name("value") {
                    if inner_value.kind() == "call_expression" {
                        if let Some(func) = inner_value.child_by_field_name("function") {
                            let func_name = get_node_text(&func, source);
                            let upper_func_name = func_name.to_uppercase();
                            if func_name == "realloc" || upper_func_name.contains("REALLOC") {
                                self.realloc_updated.insert(left_var.clone());
                                self.track_realloc_old_pointer(&inner_value, source);
                                return;
                            } else if func_name == "malloc" || func_name == "calloc" {
                                return;
                            }
                        }
                    }
                }
            }

            let right_var = self.extract_base_variable(&value, source);
            if !right_var.is_empty() {
                // Track as alias
                self.aliases.insert(left_var.clone(), right_var.clone());

                // If source is freed, the new variable is also freed
                if self.is_freed(&right_var) {
                    self.freed_vars.insert(left_var);
                }
            }
        }
    }

    /// Check pointer dereference (*ptr) for use-after-free
    fn check_pointer_dereference(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Skip if this is the left side of an assignment (handled separately)
        if let Some(parent) = node.parent() {
            if parent.kind() == "assignment_expression" {
                if let Some(left) = parent.child_by_field_name("left") {
                    if left.start_byte() == node.start_byte() {
                        return; // Handled in process_assignment
                    }
                }
            }
        }

        if let Some(arg) = node.child_by_field_name("argument") {
            let var_name = self.extract_base_variable(&arg, source);
            if !var_name.is_empty() && self.is_freed(&var_name) {
                violations.push(RuleViolation {
                    rule_id: "MEM30-C".to_string(),
                    severity: Severity::Critical,
                    message: format!("Use-after-free: dereferencing freed pointer '{}'", var_name),
                    file_path: String::new(),
                    line: node.start_position().row + 1,
                    column: node.start_position().column + 1,
                    suggestion: Some("Do not access memory after freeing it.".to_string()),
                    ..Default::default()
                });
            }
        }
    }

    /// Check array subscript access (arr[i]) for use-after-free
    fn check_subscript_access(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let Some(arg) = node.child_by_field_name("argument") {
            // First check if the full path is freed (e.g., obj->data.values)
            let full_path = get_node_text(&arg, source);
            if self.is_freed(&full_path) {
                violations.push(RuleViolation {
                    rule_id: "MEM30-C".to_string(),
                    severity: Severity::Critical,
                    message: format!("Use-after-free: accessing freed array '{}'", full_path),
                    file_path: String::new(),
                    line: node.start_position().row + 1,
                    column: node.start_position().column + 1,
                    suggestion: Some("Do not access memory after freeing it.".to_string()),
                    ..Default::default()
                });
                return;
            }

            // Also check base variable
            let var_name = self.extract_base_variable(&arg, source);
            if !var_name.is_empty() && self.is_freed(&var_name) {
                violations.push(RuleViolation {
                    rule_id: "MEM30-C".to_string(),
                    severity: Severity::Critical,
                    message: format!("Use-after-free: accessing freed array '{}'", var_name),
                    file_path: String::new(),
                    line: node.start_position().row + 1,
                    column: node.start_position().column + 1,
                    suggestion: Some("Do not access memory after freeing it.".to_string()),
                    ..Default::default()
                });
            }
        }
    }

    /// Check binary expression for pointer arithmetic on freed memory
    fn check_binary_expression(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Check for ptr + n or ptr - n patterns
        if let (Some(left), Some(operator)) = (
            node.child_by_field_name("left"),
            node.child_by_field_name("operator"),
        ) {
            let op_text = get_node_text(&operator, source);
            if op_text == "+" || op_text == "-" {
                let left_var = self.extract_base_variable(&left, source);
                if !left_var.is_empty() && self.is_freed(&left_var) {
                    violations.push(RuleViolation {
                        rule_id: "MEM30-C".to_string(),
                        severity: Severity::Critical,
                        message: format!(
                            "Use-after-free: pointer arithmetic on freed pointer '{}'",
                            left_var
                        ),
                        file_path: String::new(),
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        suggestion: Some("Do not use freed pointers in arithmetic.".to_string()),
                        ..Default::default()
                    });
                }
            }
        }
    }

    /// Check function arguments for use of freed memory
    fn check_function_args_for_freed(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let Some(arguments) = node.child_by_field_name("arguments") {
            for i in 0..arguments.child_count() {
                if let Some(arg) = arguments.child(i) {
                    if arg.kind() == "," || arg.kind() == "(" || arg.kind() == ")" {
                        continue;
                    }

                    let var_name = self.extract_base_variable(&arg, source);
                    if !var_name.is_empty() && self.is_freed(&var_name) {
                        violations.push(RuleViolation {
                            rule_id: "MEM30-C".to_string(),
                            severity: Severity::Critical,
                            message: format!(
                                "Use-after-free: passing freed pointer '{}' to function",
                                var_name
                            ),
                            file_path: String::new(),
                            line: node.start_position().row + 1,
                            column: node.start_position().column + 1,
                            suggestion: Some("Do not pass freed memory to functions.".to_string()),
                            ..Default::default()
                        });
                    }
                }
            }
        }
    }

    /// Check return statement for returning freed memory
    fn check_return_statement(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Check if the return value is a freed pointer
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "return" {
                    continue;
                }
                let var_name = self.extract_base_variable(&child, source);
                if !var_name.is_empty() && self.is_freed(&var_name) {
                    violations.push(RuleViolation {
                        rule_id: "MEM30-C".to_string(),
                        severity: Severity::Critical,
                        message: format!("Use-after-free: returning freed pointer '{}'", var_name),
                        file_path: String::new(),
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        suggestion: Some("Do not return freed memory from functions.".to_string()),
                        ..Default::default()
                    });
                }
            }
        }
    }

    /// Check for loop pattern for dangerous p = p->next after free(p)
    fn check_for_loop_pattern(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Get the loop text for pattern matching
        let loop_text = get_node_text(node, source);

        // Look for classic linked list free error:
        // for (p = head; p != NULL; p = p->next) { free(p); }
        if loop_text.contains("free(") && loop_text.contains("->") {
            // Check if free happens before the pointer is used in update
            // This is a heuristic check
            if let Some(update) = node.child_by_field_name("update") {
                let update_text = get_node_text(&update, source);
                // Look for patterns like: p = p->next
                if update_text.contains("->") {
                    // Check if there's a free() in the body that frees the same variable
                    if let Some(body) = node.child_by_field_name("body") {
                        let body_text = get_node_text(&body, source);
                        // Extract the variable from update (e.g., "p" from "p = p->next")
                        if let Some(eq_pos) = update_text.find('=') {
                            let var_part = update_text[..eq_pos].trim();
                            // Check if free(var) is in the body
                            let free_pattern = format!("free({})", var_part);
                            if body_text.contains(&free_pattern) {
                                violations.push(RuleViolation {
                                    rule_id: "MEM30-C".to_string(),
                                    severity: Severity::Critical,
                                    message: format!(
                                        "Use-after-free in loop: accessing '{}'->next after free({})",
                                        var_part, var_part
                                    ),
                                    file_path: String::new(),
                                    line: node.start_position().row + 1,
                                    column: node.start_position().column + 1,
                                    suggestion: Some(
                                        "Save pointer->next before freeing pointer.".to_string(),
                                    ),
                                    ..Default::default()
                                });
                            }
                        }
                    }
                }
            }
        }
    }

    /// Check field access for use-after-free (ptr->field)
    fn check_field_access(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Skip if parent is a subscript_expression (checked in check_subscript_access)
        if let Some(parent) = node.parent() {
            if parent.kind() == "subscript_expression" {
                return;
            }
        }

        // Skip if this is inside a free() or realloc() call - handled separately
        if let Some(parent) = node.parent() {
            if parent.kind() == "argument_list" {
                if let Some(grandparent) = parent.parent() {
                    if grandparent.kind() == "call_expression" {
                        if let Some(func) = grandparent.child_by_field_name("function") {
                            let func_name = get_node_text(&func, source);
                            let upper_func_name = func_name.to_uppercase();
                            // Skip for free, realloc, and custom variants
                            if func_name == "free"
                                || func_name == "realloc"
                                || upper_func_name.contains("FREE")
                                || upper_func_name.contains("REALLOC")
                            {
                                return;
                            }
                        }
                    }
                }
            }
            // Skip if this is the left side of an assignment (handled elsewhere)
            if parent.kind() == "assignment_expression" {
                if let Some(left) = parent.child_by_field_name("left") {
                    if left.start_byte() == node.start_byte() {
                        return;
                    }
                }
            }
        }

        // Check if the full field expression is freed (e.g., buf->data)
        let full_path = get_node_text(node, source);
        if self.is_freed(&full_path) {
            violations.push(RuleViolation {
                rule_id: "MEM30-C".to_string(),
                severity: Severity::Critical,
                message: format!("Use-after-free: accessing freed pointer '{}'", full_path),
                file_path: String::new(),
                line: node.start_position().row + 1,
                column: node.start_position().column + 1,
                suggestion: Some("Do not access freed memory.".to_string()),
                ..Default::default()
            });
            return;
        }

        // Check if the base of field expression is freed
        if let Some(arg) = node.child_by_field_name("argument") {
            let var_name = self.extract_base_variable(&arg, source);
            if !var_name.is_empty() && self.is_freed(&var_name) {
                violations.push(RuleViolation {
                    rule_id: "MEM30-C".to_string(),
                    severity: Severity::Critical,
                    message: format!(
                        "Use-after-free: accessing member of freed pointer '{}'",
                        var_name
                    ),
                    file_path: String::new(),
                    line: node.start_position().row + 1,
                    column: node.start_position().column + 1,
                    suggestion: Some("Do not access members of freed memory.".to_string()),
                    ..Default::default()
                });
            }
        }
    }

    /// Check if a variable is in freed state (considering aliases and realloc invalidation)
    /// Used for use-after-free detection
    fn is_freed(&self, var_name: &str) -> bool {
        if self.nullified_vars.contains(var_name) {
            return false;
        }
        if self.freed_vars.contains(var_name) {
            return true;
        }
        // Check if invalidated by realloc (old pointer after realloc)
        if self.realloc_invalidated.contains(var_name) {
            return true;
        }
        // Check if it's an alias of a freed or invalidated variable
        if let Some(canonical) = self.aliases.get(var_name) {
            if self.nullified_vars.contains(canonical) {
                return false;
            }
            if self.freed_vars.contains(canonical) || self.realloc_invalidated.contains(canonical) {
                return true;
            }
        }
        // Check if any union member sharing this base is freed
        for (base, members) in &self.union_members {
            if var_name.starts_with(base) {
                for member in members {
                    if self.freed_vars.contains(member) || self.realloc_invalidated.contains(member)
                    {
                        return true;
                    }
                }
            }
        }
        false
    }

    /// Check if a variable has actually been freed (not just realloc-invalidated)
    /// Used for double-free detection - it's OK to free a realloc-invalidated pointer
    fn is_actually_freed(&self, var_name: &str) -> bool {
        if self.nullified_vars.contains(var_name) {
            return false;
        }
        if self.freed_vars.contains(var_name) {
            return true;
        }
        // Check if it's an alias of a freed variable (not realloc-invalidated)
        if let Some(canonical) = self.aliases.get(var_name) {
            if self.nullified_vars.contains(canonical) {
                return false;
            }
            if self.freed_vars.contains(canonical) {
                return true;
            }
        }
        false
    }

    /// Track the old pointer passed to realloc as invalidated
    fn track_realloc_old_pointer(&mut self, call_node: &Node, source: &str) {
        if let Some(args) = call_node.child_by_field_name("arguments") {
            // First argument to realloc is the old pointer
            for i in 0..args.child_count() {
                if let Some(arg) = args.child(i) {
                    if arg.kind() != "(" && arg.kind() != ")" && arg.kind() != "," {
                        // For field expressions (like im->clip->list), track the full path
                        // since only that specific field becomes invalid
                        let old_ptr = if arg.kind() == "field_expression" {
                            get_node_text(&arg, source).to_string()
                        } else {
                            self.extract_base_variable(&arg, source)
                        };

                        if !old_ptr.is_empty() {
                            // The old pointer is now potentially invalid
                            self.realloc_invalidated.insert(old_ptr.clone());
                            // Also invalidate any aliases pointing to the old pointer
                            let aliases_to_invalidate: Vec<String> = self
                                .aliases
                                .iter()
                                .filter(|(_, v)| **v == old_ptr)
                                .map(|(k, _)| k.clone())
                                .collect();
                            for alias in aliases_to_invalidate {
                                self.realloc_invalidated.insert(alias);
                            }
                        }
                        break; // Only need the first argument
                    }
                }
            }
        }
    }

    /// Resolve a variable to its canonical name (follow alias chain)
    fn resolve_canonical(&self, var_name: &str) -> String {
        let mut current = var_name.to_string();
        let mut visited = HashSet::new();
        while let Some(target) = self.aliases.get(&current) {
            if visited.contains(target) {
                break; // Avoid infinite loop
            }
            visited.insert(current.clone());
            current = target.clone();
        }
        current
    }

    /// Extract the base variable name from various node types
    fn extract_base_variable(&self, node: &Node, source: &str) -> String {
        match node.kind() {
            "identifier" => get_node_text(node, source).to_string(),
            "pointer_expression" => {
                // *ptr - get the base pointer
                if let Some(arg) = node.child_by_field_name("argument") {
                    self.extract_base_variable(&arg, source)
                } else {
                    String::new()
                }
            }
            "field_expression" => {
                // ptr->field - get the base
                if let Some(arg) = node.child_by_field_name("argument") {
                    self.extract_base_variable(&arg, source)
                } else {
                    String::new()
                }
            }
            "subscript_expression" => {
                // arr[i] - get the base array
                if let Some(arg) = node.child_by_field_name("argument") {
                    self.extract_base_variable(&arg, source)
                } else {
                    String::new()
                }
            }
            "parenthesized_expression" => {
                // (ptr) - unwrap
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        if child.kind() != "(" && child.kind() != ")" {
                            return self.extract_base_variable(&child, source);
                        }
                    }
                }
                String::new()
            }
            "cast_expression" => {
                // (type)ptr - get the operand
                if let Some(value) = node.child_by_field_name("value") {
                    self.extract_base_variable(&value, source)
                } else {
                    String::new()
                }
            }
            _ => String::new(),
        }
    }

    /// Extract variable name from a declarator node
    fn extract_declarator_name(&self, node: &Node, source: &str) -> String {
        match node.kind() {
            "identifier" => get_node_text(node, source).to_string(),
            "pointer_declarator" => {
                if let Some(declarator) = node.child_by_field_name("declarator") {
                    self.extract_declarator_name(&declarator, source)
                } else {
                    String::new()
                }
            }
            _ => {
                // Try to find an identifier child
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        if child.kind() == "identifier" {
                            return get_node_text(&child, source).to_string();
                        }
                    }
                }
                String::new()
            }
        }
    }
}

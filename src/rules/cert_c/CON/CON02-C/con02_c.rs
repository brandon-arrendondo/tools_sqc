//! CON02-C: Do not use volatile as a synchronization primitive
//!
//! The `volatile` keyword prevents compiler caching and reordering of accesses to a single
//! variable. However, it provides insufficient guarantees for thread synchronization because
//! the compiler may reorder reads and writes to volatile variables with respect to other
//! memory locations, making volatile unsuitable for synchronization.
//!
//! ## Rationale:
//! Volatile lacks critical synchronization guarantees:
//! - **Atomicity:** Indivisible memory operations
//! - **Visibility:** Effects of writes visible to other threads
//! - **Ordering:** Consistent memory operation sequences across threads
//!
//! ## Examples:
//!
//! **Non-compliant (volatile for synchronization):**
//! ```c
//! volatile bool flag = false;
//!
//! void test() {
//!   while (!flag) {
//!     sleep(1000);
//!   }
//! }
//!
//! void wakeup() {
//!   flag = true;
//! }
//! ```
//!
//! **Compliant (using mutex):**
//! ```c
//! #include <threads.h>
//!
//! mtx_t flag;
//!
//! int debit(unsigned int amount) {
//!   if (mtx_lock(&flag) == thrd_error) {
//!     return -1;
//!   }
//!   account_balance -= amount;
//!   if (mtx_unlock(&flag) == thrd_error) {
//!     return -1;
//!   }
//!   return 0;
//! }
//! ```
//!
//! ## Detection Strategy:
//! - Find volatile variable declarations
//! - Check if volatile variables are used in synchronization contexts:
//!   * In loop conditions (while/for loops)
//!   * In functions that appear to be thread functions
//!   * Without proper mutex protection
//! - Flag volatile usage that suggests synchronization primitive misuse

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use std::collections::{HashMap, HashSet};
use tree_sitter::Node;

pub struct Con02C;

impl CertRule for Con02C {
    fn rule_id(&self) -> &'static str {
        "CON02-C"
    }

    fn description(&self) -> &'static str {
        "Do not use volatile as a synchronization primitive"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "CON02-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // First pass: collect volatile variable declarations
        let volatile_vars = self.collect_volatile_variables(node, source);

        // Second pass: collect global/file-scope variable declarations (potential shared state)
        let global_vars = self.collect_global_variables(node, source);

        // Third pass: find problematic uses (volatile or global vars in sync contexts)
        self.check_node(node, source, &volatile_vars, &global_vars, &mut violations);

        violations
    }
}

impl Con02C {
    /// Collect all volatile variable names and their declaration locations
    fn collect_volatile_variables(&self, node: &Node, source: &str) -> HashMap<String, usize> {
        let mut volatile_vars = HashMap::new();
        self.find_volatile_declarations(node, source, &mut volatile_vars);
        volatile_vars
    }

    /// Collect global/file-scope variables (potential shared state)
    fn collect_global_variables(&self, node: &Node, source: &str) -> HashMap<String, usize> {
        let mut global_vars = HashMap::new();
        self.find_global_declarations(node, source, &mut global_vars, true);
        global_vars
    }

    fn find_global_declarations(
        &self,
        node: &Node,
        source: &str,
        global_vars: &mut HashMap<String, usize>,
        is_top_level: bool,
    ) {
        // At file scope, collect variable declarations
        if is_top_level && node.kind() == "declaration" {
            // Skip if it's volatile (already handled by volatile_vars collection)
            let decl_text = get_node_text(node, source);
            if !decl_text.contains("volatile") {
                if let Some(var_name) = self.extract_variable_name(node, source) {
                    let line = node.start_position().row + 1;
                    global_vars.insert(var_name, line);
                }
            }
        }

        // Stop recursing into function definitions (we only want file-scope globals)
        if node.kind() == "function_definition" {
            return;
        }

        // Recursively check children at top level only
        if is_top_level {
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    self.find_global_declarations(&child, source, global_vars, true);
                }
            }
        }
    }

    fn find_volatile_declarations(
        &self,
        node: &Node,
        source: &str,
        volatile_vars: &mut HashMap<String, usize>,
    ) {
        // Check for volatile in declaration
        if node.kind() == "declaration" {
            let decl_text = get_node_text(node, source);
            if decl_text.contains("volatile") {
                // Extract variable name from declarator
                if let Some(var_name) = self.extract_variable_name(node, source) {
                    let line = node.start_position().row + 1;
                    volatile_vars.insert(var_name, line);
                }
            }
        }

        // Recursively check children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.find_volatile_declarations(&child, source, volatile_vars);
            }
        }
    }

    fn extract_variable_name(&self, declaration_node: &Node, source: &str) -> Option<String> {
        // Look for init_declarator or declarator nodes
        for i in 0..declaration_node.child_count() {
            if let Some(child) = declaration_node.child(i) {
                match child.kind() {
                    "init_declarator" => {
                        if let Some(declarator) = child.child_by_field_name("declarator") {
                            if let Some(name) =
                                self.get_identifier_from_declarator(&declarator, source)
                            {
                                return Some(name);
                            }
                        }
                    }
                    "identifier" => {
                        return Some(get_node_text(&child, source).to_string());
                    }
                    _ => {}
                }
            }
        }
        None
    }

    fn get_identifier_from_declarator(&self, node: &Node, source: &str) -> Option<String> {
        if node.kind() == "identifier" {
            return Some(get_node_text(node, source).to_string());
        }

        // Handle pointer_declarator, array_declarator, etc.
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "identifier" {
                    return Some(get_node_text(&child, source).to_string());
                }
                if let Some(name) = self.get_identifier_from_declarator(&child, source) {
                    return Some(name);
                }
            }
        }

        None
    }

    fn check_node(
        &self,
        node: &Node,
        source: &str,
        volatile_vars: &HashMap<String, usize>,
        global_vars: &HashMap<String, usize>,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Check for variables (volatile or global) used in loop conditions (strong indicator of sync misuse)
        if matches!(
            node.kind(),
            "while_statement" | "for_statement" | "do_statement"
        ) {
            self.check_loop_for_sync_misuse(node, source, volatile_vars, global_vars, violations);
        }

        // Check for variables in potential thread functions without mutex
        if node.kind() == "function_definition" {
            self.check_function_for_sync_misuse(
                node,
                source,
                volatile_vars,
                global_vars,
                violations,
            );
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, volatile_vars, global_vars, violations);
            }
        }
    }

    fn check_loop_for_sync_misuse(
        &self,
        loop_node: &Node,
        source: &str,
        volatile_vars: &HashMap<String, usize>,
        global_vars: &HashMap<String, usize>,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Get loop condition
        let condition = match loop_node.kind() {
            "while_statement" => loop_node.child_by_field_name("condition"),
            "for_statement" => loop_node.child_by_field_name("condition"),
            "do_statement" => loop_node.child_by_field_name("condition"),
            _ => None,
        };

        if let Some(cond) = condition {
            let cond_text = get_node_text(&cond, source);
            let line = loop_node.start_position().row + 1;

            // Check if any volatile variable is referenced in the condition
            for (var_name, _decl_line) in volatile_vars {
                if cond_text.contains(var_name) {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::Medium,
                        message: format!(
                            "Volatile variable '{}' used in loop condition - likely misuse as synchronization primitive",
                            var_name
                        ),
                        file_path: String::new(),
                        line,
                        column: 0,
                        suggestion: Some(
                            "Use proper synchronization primitives (mutexes, condition variables, or atomic operations) instead of volatile".to_string()
                        ),
                        ..Default::default()
                    });
                }
            }

            // Check if any global variable is referenced in the condition (potential sync primitive)
            for (var_name, _decl_line) in global_vars {
                if cond_text.contains(var_name) {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::Medium,
                        message: format!(
                            "Global variable '{}' used in loop condition - potential synchronization primitive without proper protection",
                            var_name
                        ),
                        file_path: String::new(),
                        line,
                        column: 0,
                        suggestion: Some(
                            "Use proper synchronization primitives (mutexes, condition variables, or atomic operations) instead of relying on shared variables".to_string()
                        ),
                        ..Default::default()
                    });
                }
            }
        }
    }

    fn check_function_for_sync_misuse(
        &self,
        function_node: &Node,
        source: &str,
        volatile_vars: &HashMap<String, usize>,
        global_vars: &HashMap<String, usize>,
        violations: &mut Vec<RuleViolation>,
    ) {
        let func_name = self
            .get_function_name(function_node, source)
            .unwrap_or_else(|| "<unknown>".to_string());

        // Check if this looks like a thread function
        if !self.is_potential_thread_function(function_node, source, &func_name) {
            return;
        }

        // Get function body
        let body = match function_node.child_by_field_name("body") {
            Some(b) => b,
            None => return,
        };

        // Check if function uses mutex locks
        let has_mutex = self.uses_mutex_lock(&body, source);

        // Find volatile variable accesses
        let volatile_accesses = self.find_variable_accesses(&body, source, volatile_vars);

        // Find global variable accesses
        let global_accesses = self.find_variable_accesses(&body, source, global_vars);

        // If thread function accesses volatile vars without mutex, flag it
        if !volatile_accesses.is_empty() && !has_mutex {
            for (var_name, line) in volatile_accesses {
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::Medium,
                    message: format!(
                        "Function '{}' accesses volatile variable '{}' without mutex protection in a multi-threaded context",
                        func_name, var_name
                    ),
                    file_path: String::new(),
                    line,
                    column: 0,
                    suggestion: Some(
                        "Use mutex locks (mtx_lock/mtx_unlock) to protect shared variable access instead of relying on volatile".to_string()
                    ),
                    ..Default::default()
                });
            }
        }

        // If thread function accesses global vars without mutex, flag it
        if !global_accesses.is_empty() && !has_mutex {
            for (var_name, line) in global_accesses {
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::Medium,
                    message: format!(
                        "Function '{}' accesses global variable '{}' without mutex protection in a multi-threaded context",
                        func_name, var_name
                    ),
                    file_path: String::new(),
                    line,
                    column: 0,
                    suggestion: Some(
                        "Use mutex locks (mtx_lock/mtx_unlock) to protect shared variable access".to_string()
                    ),
                    ..Default::default()
                });
            }
        }
    }

    fn get_function_name(&self, function_node: &Node, source: &str) -> Option<String> {
        for i in 0..function_node.child_count() {
            if let Some(child) = function_node.child(i) {
                if child.kind() == "function_declarator" {
                    if let Some(name) = self.get_identifier_from_declarator(&child, source) {
                        return Some(name);
                    }
                }
            }
        }
        None
    }

    fn is_potential_thread_function(
        &self,
        function_node: &Node,
        source: &str,
        func_name: &str,
    ) -> bool {
        // Check if function name suggests it's a thread function
        let lower_name = func_name.to_lowercase();
        if lower_name.contains("thread")
            || lower_name.contains("worker")
            || lower_name.contains("task")
        {
            return true;
        }

        // Check if function has void* parameter (common for thread functions)
        for i in 0..function_node.child_count() {
            if let Some(child) = function_node.child(i) {
                if child.kind() == "function_declarator" {
                    if let Some(params) = child.child_by_field_name("parameters") {
                        for j in 0..params.child_count() {
                            if let Some(param) = params.child(j) {
                                if param.kind() == "parameter_declaration" {
                                    let param_text = get_node_text(&param, source);
                                    if param_text.contains("void") && param_text.contains("*") {
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

    fn uses_mutex_lock(&self, node: &Node, source: &str) -> bool {
        if node.kind() == "call_expression" {
            if let Some(func) = node.child_by_field_name("function") {
                let func_name = get_node_text(&func, source);
                if matches!(
                    func_name,
                    "mtx_lock"
                        | "mtx_unlock"
                        | "pthread_mutex_lock"
                        | "pthread_mutex_unlock"
                        | "EnterCriticalSection"
                        | "LeaveCriticalSection"
                ) {
                    return true;
                }
            }
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if self.uses_mutex_lock(&child, source) {
                    return true;
                }
            }
        }

        false
    }

    fn find_variable_accesses(
        &self,
        node: &Node,
        source: &str,
        vars: &HashMap<String, usize>,
    ) -> Vec<(String, usize)> {
        let mut accesses = Vec::new();
        let accessed_vars = HashSet::new();
        self.collect_variable_accesses(node, source, vars, &mut accesses, &accessed_vars);
        accesses
    }

    fn collect_variable_accesses(
        &self,
        node: &Node,
        source: &str,
        vars: &HashMap<String, usize>,
        accesses: &mut Vec<(String, usize)>,
        _accessed_vars: &HashSet<String>,
    ) {
        // Look for identifier nodes that match variable names
        if node.kind() == "identifier" {
            let var_name = get_node_text(node, source).to_string();
            if vars.contains_key(&var_name) {
                let line = node.start_position().row + 1;
                accesses.push((var_name, line));
            }
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.collect_variable_accesses(&child, source, vars, accesses, _accessed_vars);
            }
        }
    }
}

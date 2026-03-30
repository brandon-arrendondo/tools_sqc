//! MSC04-C: Do not use recursive function calls
//!
//! Detects functions that participate in recursion cycles:
//! 1. Direct recursion: function calls itself
//! 2. Indirect recursion: function A calls B, B calls A (or longer cycles)
//!
//! Maps to BRULE-058 (Constrained tier): prohibits recursion.
//! Direct recursion is detected from AST alone; indirect recursion requires
//! prescan data (-d flag) for cross-function call graph analysis.

use super::super::{CertRule, RuleViolation};
use crate::analyze::context::ProjectContext;
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use std::cell::RefCell;
use std::collections::{HashMap, HashSet};
use tree_sitter::Node;

#[derive(Debug)]
pub struct Msc04C {
    call_graph: RefCell<HashMap<String, HashSet<String>>>,
}

impl Msc04C {
    pub fn new() -> Self {
        Msc04C {
            call_graph: RefCell::new(HashMap::new()),
        }
    }

    /// Extract function name from a function_definition node.
    fn extract_func_name<'a>(&self, node: &Node<'a>, source: &'a str) -> Option<String> {
        let declarator = node.child_by_field_name("declarator")?;
        self.find_identifier_in_declarator(&declarator, source)
    }

    fn find_identifier_in_declarator(&self, node: &Node, source: &str) -> Option<String> {
        match node.kind() {
            "identifier" => {
                let name = get_node_text(node, source);
                if name.is_empty() {
                    None
                } else {
                    Some(name.to_string())
                }
            }
            "function_declarator" | "pointer_declarator" | "parenthesized_declarator" => {
                // Recurse into the declarator child
                let inner = node.child_by_field_name("declarator")?;
                self.find_identifier_in_declarator(&inner, source)
            }
            _ => None,
        }
    }

    /// Collect all direct function calls in a subtree (identifiers in call_expression).
    fn collect_callees(&self, node: &Node, source: &str, callees: &mut HashSet<String>) {
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                if function.kind() == "identifier" {
                    let name = get_node_text(&function, source);
                    if !name.is_empty() {
                        callees.insert(name.to_string());
                    }
                }
            }
        }
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.collect_callees(&child, source, callees);
            }
        }
    }

    /// Detect if `start` participates in a recursion cycle via DFS on the call graph.
    /// Returns the cycle path if found (e.g., ["a", "b", "a"] for mutual recursion).
    fn find_cycle(
        &self,
        start: &str,
        graph: &HashMap<String, HashSet<String>>,
    ) -> Option<Vec<String>> {
        let mut visited = HashSet::new();
        let mut path = Vec::new();
        self.dfs_cycle(start, start, graph, &mut visited, &mut path)
    }

    fn dfs_cycle(
        &self,
        current: &str,
        target: &str,
        graph: &HashMap<String, HashSet<String>>,
        visited: &mut HashSet<String>,
        path: &mut Vec<String>,
    ) -> Option<Vec<String>> {
        visited.insert(current.to_string());
        path.push(current.to_string());

        if let Some(callees) = graph.get(current) {
            for callee in callees {
                if callee == target && path.len() > 1 {
                    // Found cycle back to start
                    let mut cycle = path.clone();
                    cycle.push(target.to_string());
                    return Some(cycle);
                }
                if !visited.contains(callee.as_str()) {
                    if let Some(cycle) = self.dfs_cycle(callee, target, graph, visited, path) {
                        return Some(cycle);
                    }
                }
            }
        }

        path.pop();
        None // no cycle through this path
    }

    fn check_function(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        let func_name = match self.extract_func_name(node, source) {
            Some(n) => n,
            None => return,
        };

        // Collect callees within this function body
        let mut callees = HashSet::new();
        if let Some(body) = node.child_by_field_name("body") {
            self.collect_callees(&body, source, &mut callees);
        }

        // 1. Direct recursion: function calls itself
        if callees.contains(&func_name) {
            violations.push(RuleViolation {
                rule_id: self.rule_id().to_string(),
                severity: self.severity(),
                message: format!(
                    "Function '{}' calls itself directly (direct recursion)",
                    func_name
                ),
                file_path: String::new(),
                line: node.start_position().row + 1,
                column: node.start_position().column + 1,
                suggestion: Some("Refactor to use iteration instead of recursion".to_string()),
                requires_manual_review: None,
            });
            return; // Don't also report indirect cycle
        }

        // 2. Indirect recursion: check call graph for cycles through this function
        let graph = self.call_graph.borrow();
        if graph.is_empty() {
            return; // No prescan data — can only detect direct recursion
        }

        // Build a local graph that includes this function's callees
        // (prescan graph may not include the current file if it wasn't prescanned)
        let mut local_graph = graph.clone();
        local_graph.insert(func_name.clone(), callees);

        if let Some(cycle) = self.find_cycle(&func_name, &local_graph) {
            let cycle_str = cycle.join(" -> ");
            violations.push(RuleViolation {
                rule_id: self.rule_id().to_string(),
                severity: self.severity(),
                message: format!(
                    "Function '{}' participates in indirect recursion: {}",
                    func_name, cycle_str
                ),
                file_path: String::new(),
                line: node.start_position().row + 1,
                column: node.start_position().column + 1,
                suggestion: Some("Refactor to eliminate the recursion cycle".to_string()),
                requires_manual_review: None,
            });
        }
    }

    fn walk_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            "function_definition" => {
                self.check_function(node, source, violations);
            }
            _ => {
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        self.walk_node(&child, source, violations);
                    }
                }
            }
        }
    }
}

impl CertRule for Msc04C {
    fn rule_id(&self) -> &'static str {
        "MSC04-C"
    }

    fn description(&self) -> &'static str {
        "Do not use recursive function calls"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "MSC04-C"
    }

    fn set_project_context(&self, context: &ProjectContext) {
        *self.call_graph.borrow_mut() = context.call_graph.clone();
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.walk_node(node, source, &mut violations);
        violations
    }
}

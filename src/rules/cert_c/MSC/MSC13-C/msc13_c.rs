//! MSC13-C: Detect and remove unused values
//!
//! Detects local variables that are initialized or assigned but never
//! subsequently read. Dead stores waste computation and may indicate logic errors.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! void f() {
//!     int x = 42;  // VIOLATION: x is never used
//! }
//! ```
//!
//! **Compliant:**
//! ```c
//! void f() {
//!     int x = 42;
//!     printf("%d", x);  // x is used
//! }
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Msc13C;

impl Msc13C {
    pub fn new() -> Self {
        Self
    }

    /// Collect all local variable declarations in a function body.
    /// Returns (name, declaration_line, has_initializer).
    fn collect_local_vars(&self, body: &Node, source: &str) -> Vec<(String, usize, bool)> {
        let mut vars = Vec::new();
        self.walk_for_declarations(body, source, &mut vars);
        vars
    }

    fn walk_for_declarations(
        &self,
        node: &Node,
        source: &str,
        vars: &mut Vec<(String, usize, bool)>,
    ) {
        if node.kind() == "declaration" {
            // Skip function declarations and extern/typedef
            let decl_text = get_node_text(node, source);
            if decl_text.contains("extern ") || decl_text.contains("typedef ") {
                // Don't flag extern or typedef declarations
            } else {
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        self.extract_declared_names(&child, source, vars);
                    }
                }
            }
        }

        // Recurse into children (but not into nested function definitions)
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() != "function_definition" {
                    self.walk_for_declarations(&child, source, vars);
                }
            }
        }
    }

    fn extract_declared_names(
        &self,
        node: &Node,
        source: &str,
        vars: &mut Vec<(String, usize, bool)>,
    ) {
        match node.kind() {
            "init_declarator" => {
                if let Some(declarator) = node.child_by_field_name("declarator") {
                    if let Some(name) = self.get_identifier_name(&declarator, source) {
                        vars.push((name, node.start_position().row + 1, true));
                    }
                }
            }
            // Plain identifier declaration: `int x;`
            "identifier" => {
                let name = get_node_text(node, source).to_string();
                vars.push((name, node.start_position().row + 1, false));
            }
            // Pointer/array declarator without init: `int *p;`, `int arr[10];`
            "pointer_declarator" | "array_declarator" => {
                if let Some(name) = self.get_identifier_name(node, source) {
                    vars.push((name, node.start_position().row + 1, false));
                }
            }
            // Skip function_declarator (function declarations, not variables)
            "function_declarator" => {}
            _ => {}
        }
    }

    fn get_identifier_name(&self, node: &Node, source: &str) -> Option<String> {
        match node.kind() {
            "identifier" => Some(get_node_text(node, source).to_string()),
            "pointer_declarator" | "array_declarator" => node
                .child_by_field_name("declarator")
                .and_then(|d| self.get_identifier_name(&d, source)),
            _ => {
                // Try to find identifier child
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        if child.kind() == "identifier" {
                            return Some(get_node_text(&child, source).to_string());
                        }
                    }
                }
                None
            }
        }
    }

    /// Count how many times a variable name appears as a "read" reference in
    /// the function body. A "read" is any identifier reference that is NOT:
    /// - The left side of an assignment expression
    /// - The declarator in a declaration
    /// - The operand of address-of (&) used as an out-parameter
    fn count_reads(&self, body: &Node, source: &str, var_name: &str) -> usize {
        let mut count = 0;
        self.walk_for_reads(body, source, var_name, &mut count);
        count
    }

    fn walk_for_reads(&self, node: &Node, source: &str, var_name: &str, count: &mut usize) {
        if node.kind() == "identifier" {
            let text = get_node_text(node, source);
            if text == var_name {
                // Check if this is a read (not a write target or declaration)
                if self.is_read_context(node, source) {
                    *count += 1;
                }
            }
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                // Don't recurse into nested function definitions
                if child.kind() != "function_definition" {
                    self.walk_for_reads(&child, source, var_name, count);
                }
            }
        }
    }

    /// Determines if an identifier is in a "read" context (its value is consumed).
    fn is_read_context(&self, node: &Node, source: &str) -> bool {
        let parent = match node.parent() {
            Some(p) => p,
            None => return true,
        };

        match parent.kind() {
            // Left side of assignment — this is a write, not a read
            "assignment_expression" => {
                if let Some(left) = parent.child_by_field_name("left") {
                    if left.id() == node.id() {
                        return false;
                    }
                }
                true
            }
            // Compound assignment (+=, -=, etc.) — left side is both read and write
            // But if var is only used in +=, the original init is still dead
            // For simplicity, treat compound assignment LHS as a read
            // Init declarator — this is the declaration itself, not a read
            "init_declarator" => false,
            // Declaration — not a read
            "declaration" => false,
            // Pointer declarator, array declarator in a declaration — not a read
            "pointer_declarator" | "array_declarator" => {
                // Check if we're inside a declaration
                let mut p = parent.parent();
                while let Some(pp) = p {
                    if pp.kind() == "declaration" {
                        return false;
                    }
                    if pp.kind() == "function_definition" || pp.kind() == "compound_statement" {
                        break;
                    }
                    p = pp.parent();
                }
                true
            }
            // Field expression: data.field — check if it's on the left of assignment
            "field_expression" => {
                // If this field_expression is on the left of an assignment,
                // the base variable is being written to, not read
                if let Some(grandparent) = parent.parent() {
                    if grandparent.kind() == "assignment_expression" {
                        if let Some(left) = grandparent.child_by_field_name("left") {
                            if left.id() == parent.id() {
                                return false; // data.field = value → write
                            }
                        }
                    }
                }
                true
            }
            // Subscript expression: data[i] — similar to field_expression
            "subscript_expression" => {
                if let Some(grandparent) = parent.parent() {
                    if grandparent.kind() == "assignment_expression" {
                        if let Some(left) = grandparent.child_by_field_name("left") {
                            if left.id() == parent.id() {
                                return false; // data[i] = value → write
                            }
                        }
                    }
                }
                true
            }
            // Update expression (x++, ++x) — this is a read+write
            "update_expression" => true,
            // Address-of in a call argument: func(&x) — treat as read
            // because the function may read through the pointer
            "unary_expression" => {
                if let Some(op) = parent.child_by_field_name("operator") {
                    let op_text = get_node_text(&op, source);
                    if op_text == "&" {
                        return true; // &x — function may read
                    }
                }
                true
            }
            _ => true,
        }
    }
}

impl CertRule for Msc13C {
    fn rule_id(&self) -> &'static str {
        "MSC13-C"
    }

    fn description(&self) -> &'static str {
        "Detect and remove unused values"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "MSC13-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Walk all function definitions
        self.check_functions(node, source, &mut violations);

        violations
    }
}

impl Msc13C {
    fn check_functions(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if node.kind() == "function_definition" {
            if let Some(body) = node.child_by_field_name("body") {
                self.check_function_body(&body, source, violations);
            }
        }

        // Recurse into preproc blocks and other containers
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() != "function_definition"
                    || node.kind() == "translation_unit"
                    || node.kind().starts_with("preproc_")
                {
                    self.check_functions(&child, source, violations);
                }
            }
        }
    }

    fn check_function_body(&self, body: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Collect all local variable declarations
        let local_vars = self.collect_local_vars(body, source);

        // Check each declared variable for reads
        for (name, line, has_init) in &local_vars {
            let reads = self.count_reads(body, source, name);
            if reads == 0 {
                let msg = if *has_init {
                    format!("Variable '{}' is initialized but never read.", name)
                } else {
                    format!("Variable '{}' is declared but never used.", name)
                };
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: self.severity(),
                    message: msg,
                    file_path: String::new(),
                    line: *line,
                    column: 1,
                    suggestion: Some("Use the variable or remove it".to_string()),
                    ..Default::default()
                });
            }
        }

        // Dead store detection: variable assigned, then overwritten before read
        self.check_dead_stores(body, source, &local_vars, violations);
    }

    /// Detect dead stores: an assignment whose value is overwritten before being read.
    /// Pattern: `data = 'C'; data = 'Z'; use(data);` — first assignment is dead.
    fn check_dead_stores(
        &self,
        body: &Node,
        source: &str,
        local_vars: &[(String, usize, bool)],
        violations: &mut Vec<RuleViolation>,
    ) {
        // Collect all assignment sites for each local variable
        let var_names: std::collections::HashSet<_> =
            local_vars.iter().map(|(n, _, _)| n.as_str()).collect();

        let mut assignments: Vec<(String, usize, usize)> = Vec::new(); // (name, line, byte_offset)
        self.collect_all_assignments(body, source, &var_names, &mut assignments);

        // Also add init_declarator assignments
        for (name, line, has_init) in local_vars {
            if *has_init {
                // Find the byte offset for this init_declarator
                // Use line as approximate ordering
                assignments.push((name.clone(), *line, 0));
            }
        }

        // Sort by byte position (line number as proxy)
        assignments.sort_by_key(|a| (a.1, a.2));

        // For each assignment, check if the value is read before the next assignment
        // to the same variable. Simple approach: find next assignment to same var,
        // check if any read exists between this assignment and the next one.
        for i in 0..assignments.len() {
            let (ref name, line, _) = assignments[i];

            // Find next assignment to same variable
            let next_assign_line = assignments[i + 1..]
                .iter()
                .find(|(n, _, _)| n == name)
                .map(|(_, l, _)| *l);

            if let Some(next_line) = next_assign_line {
                // Check if any read of this variable exists between line and next_line
                if !self.has_read_between(body, source, name, line, next_line) {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: self.severity(),
                        message: format!(
                            "Value assigned to '{}' is overwritten before being read.",
                            name
                        ),
                        file_path: String::new(),
                        line,
                        column: 1,
                        suggestion: Some(
                            "Remove the dead assignment or use its value before reassigning"
                                .to_string(),
                        ),
                        ..Default::default()
                    });
                }
            }
        }
    }

    /// Collect all assignment sites for variables in the var_names set
    fn collect_all_assignments(
        &self,
        node: &Node,
        source: &str,
        var_names: &std::collections::HashSet<&str>,
        assignments: &mut Vec<(String, usize, usize)>,
    ) {
        if node.kind() == "assignment_expression" {
            if let Some(left) = node.child_by_field_name("left") {
                if left.kind() == "identifier" {
                    let name = get_node_text(&left, source);
                    if var_names.contains(name) {
                        assignments.push((
                            name.to_string(),
                            node.start_position().row + 1,
                            node.start_byte(),
                        ));
                    }
                }
            }
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() != "function_definition" {
                    self.collect_all_assignments(&child, source, var_names, assignments);
                }
            }
        }
    }

    /// Check if there's a read of var_name between start_line and end_line
    fn has_read_between(
        &self,
        node: &Node,
        source: &str,
        var_name: &str,
        start_line: usize,
        end_line: usize,
    ) -> bool {
        if node.kind() == "identifier" {
            let line = node.start_position().row + 1;
            if line > start_line && line < end_line {
                let text = get_node_text(node, source);
                if text == var_name && self.is_read_context(node, source) {
                    return true;
                }
            }
        }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() != "function_definition"
                    && self.has_read_between(&child, source, var_name, start_line, end_line)
                {
                    return true;
                }
            }
        }

        false
    }
}

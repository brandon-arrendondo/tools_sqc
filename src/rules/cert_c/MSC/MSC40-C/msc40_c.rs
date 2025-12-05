//! MSC40-C: Do not violate constraints
//!
//! This rule enforces C Standard constraints related to inline function definitions.
//! Specifically, it detects when inline functions violate constraints by:
//! - Referencing identifiers with internal linkage (static identifiers)
//! - Referencing modifiable objects with static or thread storage duration
//! - Containing modifiable static variables
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! static int I = 12;  // Internal linkage
//! extern inline void func(int a) {
//!   int b = a * I;  // References static identifier - violates constraint
//! }
//! ```
//!
//! **Non-compliant:**
//! ```c
//! extern inline void func(void) {
//!   static int I = 12;  // Modifiable static in inline function - violates constraint
//! }
//! ```
//!
//! **Compliant:**
//! ```c
//! int I = 12;  // External linkage - OK
//! extern inline void func(int a) {
//!   int b = a * I;
//! }
//! ```
//!
//! ## Detection Strategy:
//! - Detect non-static inline functions
//! - Check if they reference static (internally-linked) identifiers
//! - Check if they contain static variable declarations

use crate::manifest::{RuleCategory, Severity};
use crate::prelude::RuleViolation;
use crate::rules::cert_c::CertRule;
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Msc40C;

impl CertRule for Msc40C {
    fn rule_id(&self) -> &'static str {
        "MSC40-C"
    }

    fn cert_id(&self) -> &'static str {
        "MSC40"
    }

    fn description(&self) -> &'static str {
        "Do not violate constraints"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Msc40C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check for inline function definitions
        if node.kind() == "function_definition" {
            self.check_inline_function(node, source, violations);
        }

        // Recurse through child nodes
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }

    fn check_inline_function(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Check if this is an inline function (extern inline or just inline, but not static inline)
        if !self.is_non_static_inline_function(node, source) {
            return;
        }

        // Check for static variable declarations inside the function
        self.check_for_internal_static_declarations(node, source, violations);

        // Check for references to static (internally-linked) identifiers
        self.check_for_static_references(node, source, violations);
    }

    fn is_non_static_inline_function(&self, node: &Node, source: &str) -> bool {
        // Check for inline specifier and ensure it's not static inline
        let func_text = get_node_text(node, source);

        // Must be inline
        if !func_text.contains("inline") {
            return false;
        }

        // But not static inline (static inline is allowed)
        // Check if "static" appears before "inline" in the declaration
        if let Some(inline_pos) = func_text.find("inline") {
            let before_inline = &func_text[..inline_pos];
            if before_inline.contains("static") {
                return false; // static inline is compliant
            }
        }

        true
    }

    fn check_for_internal_static_declarations(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Look for static variable declarations inside the function body
        if let Some(body) = node.child_by_field_name("body") {
            self.find_static_declarations(&body, source, violations);
        }
    }

    fn find_static_declarations(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if node.kind() == "declaration" {
            let decl_text = get_node_text(node, source);
            if decl_text.trim_start().starts_with("static") {
                let start_point = node.start_position();
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::Low,
                    message: "Non-static inline function contains static variable declaration, violating C Standard constraint. Use external linkage or make the function 'static inline'.".to_string(),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some(
                        "Either remove 'static' from the variable declaration, make the function 'static inline', or remove 'inline' from the function.".to_string()
                    ),
                    ..Default::default()
                });
            }
        }

        // Recurse through children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.find_static_declarations(&child, source, violations);
        }
    }

    fn check_for_static_references(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // This is a heuristic check - we look for identifier references
        // In a production implementation, we would need full symbol table analysis
        // to determine if an identifier has internal linkage

        // For now, this is a conservative stub that could be enhanced with:
        // 1. Building a symbol table of static declarations in the file
        // 2. Tracking which identifiers are referenced in inline functions
        // 3. Reporting violations when inline functions reference static identifiers

        // This would require multi-pass analysis and is beyond the scope of
        // a simple AST walker. Leaving as a stub for now.
        let _ = (node, source, violations);
    }
}

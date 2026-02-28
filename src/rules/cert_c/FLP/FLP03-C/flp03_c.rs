//! FLP03-C: Detect and handle floating-point errors
//!
//! This rule addresses the detection and handling of errors occurring during
//! floating-point operations. Programmers often validate operands before operations
//! but neglect errors that occur during computation itself, which can result in
//! silent failures and unexpected arithmetic results.
//!
//! ## Floating-Point Errors to Detect:
//! - **Divide-by-zero**: Returns infinity rather than aborting
//! - **Inexact operations**: Loss of precision
//! - **Underflow**: Results too small to represent
//! - **Overflow**: Results too large for the data type
//! - **Invalid operations**: Conversions causing undefined values
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! void fpOper_noErrorChecking(void) {
//!     double a = 1e-40, b, c = 0.1;
//!     float x = 0, y;
//!     y = a;           // Inexact and underflows - no error check
//!     b = y / x;       // Divide-by-zero - no error check
//!     c = sin(30) * a; // Inexact - no error check
//! }
//! ```
//!
//! **Compliant (using fenv.h):**
//! ```c
//! #include <fenv.h>
//! #pragma STDC FENV_ACCESS ON
//!
//! void fpOper_fenv(void) {
//!     double a = 1e-40, b, c = 0.1;
//!     float x = 0, y;
//!     int fpeRaised;
//!
//!     feclearexcept(FE_ALL_EXCEPT);
//!     y = a;
//!     fpeRaised = fetestexcept(FE_ALL_EXCEPT);
//!
//!     feclearexcept(FE_ALL_EXCEPT);
//!     b = y / x;
//!     fpeRaised = fetestexcept(FE_ALL_EXCEPT);
//! }
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use std::collections::HashSet;
use tree_sitter::Node;

pub struct Flp03C;

/// Analyzer that tracks floating-point variables
struct FpAnalyzer {
    float_vars: HashSet<String>,
}

impl FpAnalyzer {
    fn new() -> Self {
        Self {
            float_vars: HashSet::new(),
        }
    }

    /// Collect all floating-point variable declarations from the AST
    fn collect_float_vars(&mut self, node: &Node, source: &str) {
        if node.kind() == "declaration" {
            let decl_text = get_node_text(node, source);
            // Check if this is a float/double declaration
            if decl_text.starts_with("float")
                || decl_text.starts_with("double")
                || decl_text.contains(" float ")
                || decl_text.contains(" double ")
            {
                // Extract all identifiers from this declaration
                self.extract_identifiers_from_declaration(node, source);
            }
        }

        // Recursively process children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.collect_float_vars(&child, source);
            }
        }
    }

    fn extract_identifiers_from_declaration(&mut self, node: &Node, source: &str) {
        // Look for init_declarator or declarator nodes containing identifiers
        if node.kind() == "identifier" {
            // Verify parent is a declarator-type node (not type specifier)
            if let Some(parent) = node.parent() {
                let parent_kind = parent.kind();
                if parent_kind == "init_declarator"
                    || parent_kind == "declarator"
                    || parent_kind == "pointer_declarator"
                    || parent_kind == "array_declarator"
                {
                    let var_name = get_node_text(node, source).to_string();
                    self.float_vars.insert(var_name);
                }
            }
        }

        // Recursively check children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.extract_identifiers_from_declaration(&child, source);
            }
        }
    }

    /// Check if an expression involves floating-point values or variables
    fn is_fp_expression(&self, node: &Node, source: &str) -> bool {
        let text = get_node_text(node, source);

        // Check for floating-point literals (contains decimal point or e notation)
        if text.contains('.') && !text.starts_with("/*") {
            return true;
        }

        // Precise scientific notation: digit before e/E, digit or sign after
        // Avoids matching 'e' in identifiers like "sizeof"
        let bytes = text.as_bytes();
        for i in 1..bytes.len().saturating_sub(1) {
            if bytes[i] == b'e' || bytes[i] == b'E' {
                let before = bytes[i - 1];
                let after = bytes[i + 1];
                if before.is_ascii_digit()
                    && (after.is_ascii_digit() || after == b'+' || after == b'-')
                {
                    return true;
                }
            }
        }

        // Check for floating-point type keywords
        if text.contains("float") || text.contains("double") {
            return true;
        }

        // Check if any identifier in this expression is a known float variable
        self.contains_float_identifier(node, source)
    }

    fn contains_float_identifier(&self, node: &Node, source: &str) -> bool {
        if node.kind() == "identifier" {
            let name = get_node_text(node, source);
            if self.float_vars.contains(name) {
                return true;
            }
        }

        // Recursively check children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if self.contains_float_identifier(&child, source) {
                    return true;
                }
            }
        }

        false
    }
}

impl Flp03C {
    #[allow(dead_code)]
    pub fn new() -> Self {
        Self
    }

    /// List of floating-point error checking functions from fenv.h
    const FENV_FUNCTIONS: &'static [&'static str] = &[
        "feclearexcept",
        "fetestexcept",
        "fegetexceptflag",
        "fesetexceptflag",
        "feraiseexcept",
    ];

    /// List of Windows-specific floating-point error checking functions
    const WINDOWS_FP_FUNCTIONS: &'static [&'static str] =
        &["_clearfp", "_statusfp", "_controlfp", "_fpieee_flt"];

    /// Check if a function name is a floating-point error checking function
    fn is_fp_error_check_function(&self, name: &str) -> bool {
        Self::FENV_FUNCTIONS.contains(&name) || Self::WINDOWS_FP_FUNCTIONS.contains(&name)
    }

    /// Check if a node contains floating-point error checking calls
    fn contains_fp_error_checking(&self, node: &Node, source: &str) -> bool {
        // Check this node
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = get_node_text(&function, source);
                if self.is_fp_error_check_function(func_name) {
                    return true;
                }
            }
        }

        // Check for Windows SEH exception handling (_try/_except or __try/__except)
        // These are typically parsed as identifier nodes with the text "_try", "__try", etc.
        let node_text = get_node_text(node, source);
        if node_text.contains("_try")
            || node_text.contains("__try")
            || node_text.contains("_except")
            || node_text.contains("__except")
        {
            return true;
        }

        // Also check for _fpieee_flt which is Windows FP exception handling
        if node_text.contains("_fpieee_flt") || node_text.contains("unmask_fpsr") {
            return true;
        }

        // Recursively check children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if self.contains_fp_error_checking(&child, source) {
                    return true;
                }
            }
        }

        false
    }

    /// Check for floating-point division operations
    fn check_fp_division(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        analyzer: &FpAnalyzer,
    ) {
        if node.kind() == "binary_expression" {
            // Check if this is a division operation
            let mut is_division = false;
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() == "/" {
                        is_division = true;
                        break;
                    }
                }
            }

            // Check each operand individually — at least one must be float
            let left_fp = node
                .child_by_field_name("left")
                .is_some_and(|l| analyzer.is_fp_expression(&l, source));
            let right_fp = node
                .child_by_field_name("right")
                .is_some_and(|r| analyzer.is_fp_expression(&r, source));
            if is_division && (left_fp || right_fp) {
                // Check if there's error checking in the containing function
                if let Some(containing_func) = self.find_containing_function(node) {
                    if !self.contains_fp_error_checking(&containing_func, source) {
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: self.severity(),
                            message: "Floating-point division without error checking (consider using feclearexcept/fetestexcept)".to_string(),
                            file_path: String::new(),
                            line: node.start_position().row + 1,
                            column: node.start_position().column + 1,
                            suggestion: Some(
                                "Use feclearexcept(FE_ALL_EXCEPT) before and fetestexcept(FE_ALL_EXCEPT) after floating-point operations".to_string()
                            ),
                            ..Default::default()
                        });
                    }
                }
            }
        }
    }

    /// Check for floating-point type conversions without error checking
    fn check_fp_conversion(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check for cast expressions involving floating-point types
        if node.kind() == "cast_expression" {
            if let Some(type_node) = node.child_by_field_name("type") {
                let type_text = get_node_text(&type_node, source);
                if type_text.contains("float") || type_text.contains("double") {
                    // Check if there's error checking in the containing function
                    if let Some(containing_func) = self.find_containing_function(node) {
                        if !self.contains_fp_error_checking(&containing_func, source) {
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                severity: self.severity(),
                                message: "Floating-point type conversion without error checking (may cause inexact conversion or overflow)".to_string(),
                                file_path: String::new(),
                                line: node.start_position().row + 1,
                                column: node.start_position().column + 1,
                                suggestion: Some(
                                    "Use feclearexcept/fetestexcept to detect FE_INEXACT, FE_OVERFLOW, or FE_UNDERFLOW".to_string()
                                ),
                                ..Default::default()
                            });
                        }
                    }
                }
            }
        }
    }

    /// Find the containing function definition for a given node
    fn find_containing_function<'a>(&self, node: &Node<'a>) -> Option<Node<'a>> {
        let mut current = Some(*node);
        while let Some(n) = current {
            if n.kind() == "function_definition" {
                return Some(n);
            }
            current = n.parent();
        }
        None
    }
}

impl CertRule for Flp03C {
    fn rule_id(&self) -> &'static str {
        "FLP03-C"
    }

    fn description(&self) -> &'static str {
        "Detect and handle floating-point errors"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "FLP03-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // First pass: collect all floating-point variable declarations
        let mut analyzer = FpAnalyzer::new();
        analyzer.collect_float_vars(node, source);

        // Second pass: check for violations
        self.check_node(node, source, &mut violations, &analyzer);
        violations
    }
}

impl Flp03C {
    fn check_node(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        analyzer: &FpAnalyzer,
    ) {
        // Check for floating-point operations without error checking
        match node.kind() {
            "binary_expression" => {
                self.check_fp_division(node, source, violations, analyzer);
            }
            "cast_expression" => {
                self.check_fp_conversion(node, source, violations);
            }
            _ => {}
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations, analyzer);
            }
        }
    }
}

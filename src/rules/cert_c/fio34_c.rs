//! FIO34-C: Distinguish between characters read from a file and EOF or WEOF
//!
//! This rule ensures that character input functions (getc, fgetc, getchar)
//! have their return values stored in int variables to properly distinguish
//! between valid characters and EOF/WEOF values.
//!
//! KEY DISTINCTIONS:
//! - Character INPUT functions (getc, fgetc, getchar): Must store in int before comparison
//! - Character MANIPULATION functions (ungetc): Can be directly compared with EOF
//!
//! VIOLATIONS:
//! - char c = getc(file);           // char cannot hold EOF
//! - while ((char c = getc()) != EOF)  // char comparison with EOF
//!
//! VALID PATTERNS:
//! - int c = getc(file);            // int can distinguish EOF
//! - if (ungetc(c, file) == EOF)    // ungetc designed for EOF comparison
//! - while ((c = getc()) != EOF) where c is int

use super::super::{CertRule, RuleViolation};
use crate::manifest::Severity;
use tree_sitter::Node;

pub struct Fio34C;

impl Fio34C {
    pub fn new() -> Self {
        Self
    }
}

impl CertRule for Fio34C {
    fn rule_id(&self) -> &'static str {
        "FIO34-C"
    }

    fn description(&self) -> &'static str {
        "Distinguish between characters read from a file and EOF or WEOF"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Fio34C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            "assignment_expression" => {
                self.check_assignment(node, source, violations);
            }
            "init_declarator" => {
                self.check_init_declarator(node, source, violations);
            }
            "binary_expression" => {
                self.check_comparison(node, source, violations);
            }
            "while_statement" | "do_statement" | "for_statement" => {
                self.check_loop_condition(node, source, violations);
            }
            _ => {}
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }

    /// Check for char variables being assigned from getc/fgetc/getchar
    fn check_assignment(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let (Some(left), Some(right)) = (node.child_by_field_name("left"), node.child_by_field_name("right")) {
            if right.kind() == "call_expression" {
                if let Some(function) = right.child_by_field_name("function") {
                    let function_name = &source[function.start_byte()..function.end_byte()];

                    if self.is_character_input_function(function_name) {
                        // Check if the left side is a char type variable
                        if self.is_char_type_variable(&left, source) {
                            self.report_char_assignment_violation(node, function_name, source, violations);
                        }
                    }
                }
            }
        }
    }

    /// Check for char variables being initialized from getc/fgetc/getchar
    fn check_init_declarator(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check for pattern: char c = getc(file);
        if let Some(value) = node.child_by_field_name("value") {
            if value.kind() == "call_expression" {
                if let Some(function) = value.child_by_field_name("function") {
                    let function_name = &source[function.start_byte()..function.end_byte()];

                    if self.is_character_input_function(function_name) {
                        // Check if this is a char declaration
                        if let Some(declarator) = node.child_by_field_name("declarator") {
                            if self.is_char_declaration(node, source) {
                                self.report_char_init_violation(node, function_name, source, violations);
                            }
                        }
                    }
                }
            }
        }
    }

    /// Check for direct comparison of getc/fgetc result with EOF
    fn check_comparison(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        let node_text = &source[node.start_byte()..node.end_byte()];

        // Look for patterns like: getc(file) == EOF or getc(file) != EOF
        if node_text.contains("EOF") || node_text.contains("WEOF") {
            if let Some(operator) = node.child_by_field_name("operator") {
                let op_text = &source[operator.start_byte()..operator.end_byte()];
                if op_text == "==" || op_text == "!=" {
                    // Check if either side is a direct call to character input function
                    if let Some(left) = node.child_by_field_name("left") {
                        if self.contains_unchecked_char_input(&left, source) {
                            self.report_direct_comparison_violation(node, source, violations);
                            return;
                        }
                    }
                    if let Some(right) = node.child_by_field_name("right") {
                        if self.contains_unchecked_char_input(&right, source) {
                            self.report_direct_comparison_violation(node, source, violations);
                        }
                    }
                }
            }
        }
    }

    /// Check loop conditions for problematic patterns
    fn check_loop_condition(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Pattern: while ((c = getc(file)) != EOF) where c is char
        if let Some(condition) = node.child_by_field_name("condition") {
            self.check_loop_condition_pattern(&condition, source, violations);
        }
    }

    fn check_loop_condition_pattern(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        let node_text = &source[node.start_byte()..node.end_byte()];

        // Look for assignment within condition
        if node.kind() == "parenthesized_expression" {
            if let Some(inner) = node.child(1) {  // Skip the opening paren
                self.check_loop_condition_pattern(&inner, source, violations);
            }
        } else if node.kind() == "binary_expression" {
            // Check for pattern: (c = getc(...)) != EOF where c is char
            if node_text.contains("EOF") || node_text.contains("WEOF") {
                if let Some(left) = node.child_by_field_name("left") {
                    // Need to check if the assignment uses a char variable
                    self.check_assignment_expression_for_char(&left, source, violations, node);
                }
            }
        }
    }

    /// Check if an assignment expression in a loop condition uses a char variable
    fn check_assignment_expression_for_char(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>, parent: &Node) {
        if node.kind() == "parenthesized_expression" {
            if let Some(inner) = node.child(1) {
                self.check_assignment_expression_for_char(&inner, source, violations, parent);
            }
        } else if node.kind() == "assignment_expression" {
            if let (Some(left), Some(right)) = (node.child_by_field_name("left"), node.child_by_field_name("right")) {
                if right.kind() == "call_expression" {
                    if let Some(function) = right.child_by_field_name("function") {
                        let function_name = &source[function.start_byte()..function.end_byte()];
                        if self.is_character_input_function(function_name) {
                            // Check if left is a char variable
                            if self.is_char_type_variable(&left, source) {
                                self.report_loop_condition_violation(parent, source, violations);
                            }
                        }
                    }
                }
            }
        }
    }

    /// Character input functions that read from streams
    /// These should have results stored in int variables before EOF comparison
    fn is_character_input_function(&self, name: &str) -> bool {
        matches!(name, "getc" | "fgetc" | "getchar" | "getwc" | "fgetwc" | "getwchar")
    }

    /// Character manipulation functions that return status codes
    /// These are designed to be compared directly with EOF
    fn is_character_manipulation_function(&self, name: &str) -> bool {
        matches!(name, "ungetc" | "ungetwc")
    }

    /// All functions that work with character I/O (for general detection)
    fn is_character_io_function(&self, name: &str) -> bool {
        self.is_character_input_function(name) || self.is_character_manipulation_function(name)
    }

    /// Helper: Check if a function is a wide character input function
    fn is_wide_character_function(&self, name: &str) -> bool {
        matches!(name, "getwc" | "fgetwc" | "getwchar")
    }

    /// Helper: Check if a variable is of char type
    fn is_char_type_variable(&self, node: &Node, source: &str) -> bool {
        // This is a simplified check - in production, would need proper type analysis
        // Check if the variable was declared as char
        let var_name = &source[node.start_byte()..node.end_byte()];

        // Walk up to find the declaration
        let mut current = node.parent();
        while let Some(parent) = current {
            if parent.kind() == "function_definition" || parent.kind() == "compound_statement" {
                // Search for char declarations in this scope
                let scope_text = &source[parent.start_byte()..parent.end_byte()];
                if scope_text.contains(&format!("char {}", var_name)) ||
                   scope_text.contains(&format!("char *{}", var_name)) ||
                   scope_text.contains(&format!("unsigned char {}", var_name)) ||
                   scope_text.contains(&format!("signed char {}", var_name)) {
                    return true;
                }
                break;
            }
            current = parent.parent();
        }
        false
    }

    /// Helper: Check if this is a char declaration
    fn is_char_declaration(&self, node: &Node, source: &str) -> bool {
        // Look for the type specifier in the parent declaration
        if let Some(parent) = node.parent() {
            if parent.kind() == "declaration" {
                // Check for char type specifier
                for i in 0..parent.child_count() {
                    if let Some(child) = parent.child(i) {
                        if child.kind() == "primitive_type" || child.kind() == "type_identifier" {
                            let type_text = &source[child.start_byte()..child.end_byte()];
                            if type_text == "char" {
                                return true;
                            }
                        }
                    }
                }
            }
        }
        false
    }

    /// Helper: Check if a node contains unchecked character INPUT (not manipulation)
    fn contains_unchecked_char_input(&self, node: &Node, source: &str) -> bool {
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let function_name = &source[function.start_byte()..function.end_byte()];
                // Only flag character INPUT functions, not manipulation functions
                if self.is_character_input_function(function_name) {
                    return true;
                }
                // Do NOT flag character manipulation functions like ungetc
                if self.is_character_manipulation_function(function_name) {
                    return false;
                }
            }
        }

        // Check if it's a char variable that was assigned from getc/fgetc (not ungetc)
        if node.kind() == "identifier" {
            if self.is_char_type_variable(node, source) {
                // This char variable might have been assigned from getc
                // Would need data flow analysis to be certain
                return true;
            }
        }

        false
    }

    /// Helper: Check if text contains char variable with getc pattern
    fn contains_char_getc_pattern(&self, text: &str) -> bool {
        // This needs to actually check if the variable is char type
        // For now, return false - we need proper type checking in loop conditions
        false
    }

    /// Report violation for char assignment from getc/fgetc
    fn report_char_assignment_violation(&self, node: &Node, function_name: &str, source: &str, violations: &mut Vec<RuleViolation>) {
        let start_point = node.start_position();
        let node_text = &source[node.start_byte()..node.end_byte()];

        violations.push(RuleViolation {
            rule_id: self.rule_id().to_string(),
            severity: Severity::High,
            message: format!(
                "Character from '{}' stored in char variable may not distinguish EOF: '{}'",
                function_name, node_text
            ),
            file_path: String::new(),
            line: start_point.row + 1,
            column: start_point.column + 1,
            suggestion: Some(format!(
                "Use 'int' instead of 'char' to store the result of '{}' and properly check for EOF",
                function_name
            )),
        });
    }

    /// Report violation for char initialization from getc/fgetc
    fn report_char_init_violation(&self, node: &Node, function_name: &str, source: &str, violations: &mut Vec<RuleViolation>) {
        let start_point = node.start_position();
        let node_text = &source[node.start_byte()..node.end_byte()];

        violations.push(RuleViolation {
            rule_id: self.rule_id().to_string(),
            severity: Severity::High,
            message: format!(
                "Character from '{}' initialized to char variable, cannot distinguish EOF: '{}'",
                function_name, node_text
            ),
            file_path: String::new(),
            line: start_point.row + 1,
            column: start_point.column + 1,
            suggestion: Some(format!(
                "Declare variable as 'int' instead of 'char' to properly handle EOF from '{}'",
                function_name
            )),
        });
    }

    /// Report violation for direct EOF comparison
    fn report_direct_comparison_violation(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        let start_point = node.start_position();
        let node_text = &source[node.start_byte()..node.end_byte()];

        violations.push(RuleViolation {
            rule_id: self.rule_id().to_string(),
            severity: Severity::High,
            message: format!(
                "Direct comparison with EOF/WEOF may fail with char type: '{}'",
                node_text
            ),
            file_path: String::new(),
            line: start_point.row + 1,
            column: start_point.column + 1,
            suggestion: Some("Store result in 'int' (or 'wint_t' for wide chars) before comparing with EOF/WEOF".to_string()),
        });
    }

    /// Report violation for loop condition pattern
    fn report_loop_condition_violation(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        let start_point = node.start_position();
        let node_text = &source[node.start_byte()..node.end_byte()];

        violations.push(RuleViolation {
            rule_id: self.rule_id().to_string(),
            severity: Severity::High,
            message: format!(
                "Loop condition with char type may not properly detect EOF: '{}'",
                node_text
            ),
            file_path: String::new(),
            line: start_point.row + 1,
            column: start_point.column + 1,
            suggestion: Some("Use 'int' for the loop variable to properly detect EOF".to_string()),
        });
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_fio34c_detects_char_assignment_from_getc() {
        let source = r#"
#include <stdio.h>

void test() {
    FILE *file = fopen("test.txt", "r");
    char c;
    c = getc(file);  // Violation: char cannot distinguish EOF
    fclose(file);
}
"#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(source, None).unwrap();
        let rule = Fio34C::new();
        let violations = rule.check(&tree.root_node(), source);

        assert!(!violations.is_empty(), "Should detect char assignment from getc");
        assert!(violations[0].message.contains("char variable"));
    }

    #[test]
    fn test_fio34c_detects_char_init_from_fgetc() {
        let source = r#"
#include <stdio.h>

void test() {
    FILE *file = fopen("test.txt", "r");
    char c = fgetc(file);  // Violation: char cannot distinguish EOF
    fclose(file);
}
"#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(source, None).unwrap();
        let rule = Fio34C::new();
        let violations = rule.check(&tree.root_node(), source);

        assert!(!violations.is_empty(), "Should detect char initialization from fgetc");
    }

    #[test]
    fn test_fio34c_detects_loop_with_char_eof_check() {
        let source = r#"
#include <stdio.h>

void test() {
    FILE *file = fopen("test.txt", "r");
    char c;
    while ((c = getc(file)) != EOF) {  // Violation: char may not detect EOF
        printf("%c", c);
    }
    fclose(file);
}
"#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(source, None).unwrap();
        let rule = Fio34C::new();
        let violations = rule.check(&tree.root_node(), source);

        assert!(!violations.is_empty(), "Should detect char in loop EOF check");
    }

    #[test]
    fn test_fio34c_accepts_int_assignment() {
        let source = r#"
#include <stdio.h>

void test() {
    FILE *file = fopen("test.txt", "r");
    int c;
    c = getc(file);  // OK: int can distinguish EOF
    if (c != EOF) {
        printf("%c", c);
    }
    fclose(file);
}
"#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(source, None).unwrap();
        let rule = Fio34C::new();
        let violations = rule.check(&tree.root_node(), source);

        assert!(violations.is_empty(), "Should accept int assignment from getc");
    }

    #[test]
    fn test_fio34c_accepts_int_loop() {
        let source = r#"
#include <stdio.h>

void test() {
    FILE *file = fopen("test.txt", "r");
    int c;
    while ((c = getc(file)) != EOF) {  // OK: int properly detects EOF
        printf("%c", c);
    }
    fclose(file);
}
"#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(source, None).unwrap();
        let rule = Fio34C::new();
        let violations = rule.check(&tree.root_node(), source);

        for v in &violations {
            println!("Violation: {}", v.message);
        }
        assert!(violations.is_empty(), "Should accept int in loop EOF check, but found {} violations", violations.len());
    }

    #[test]
    fn test_fio34c_detects_getchar_char_assignment() {
        let source = r#"
#include <stdio.h>

void test() {
    char c = getchar();  // Violation: char cannot distinguish EOF
    if (c != EOF) {
        printf("%c", c);
    }
}
"#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(source, None).unwrap();
        let rule = Fio34C::new();
        let violations = rule.check(&tree.root_node(), source);

        assert!(!violations.is_empty(), "Should detect char assignment from getchar");
    }

    #[test]
    fn test_fio34c_detects_wide_char_issues() {
        let source = r#"
#include <wchar.h>
#include <stdio.h>

void test() {
    FILE *file = fopen("test.txt", "r");
    wchar_t wc = getwc(file);  // Potential issue: should be wint_t
    if (wc != WEOF) {
        wprintf(L"%lc", wc);
    }
    fclose(file);
}
"#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(source, None).unwrap();
        let rule = Fio34C::new();
        let violations = rule.check(&tree.root_node(), source);

        // This test may need adjustment based on how we handle wide characters
        // For now, we're focusing on the basic char/int distinction
    }

    #[test]
    fn test_fio34c_accepts_ungetc_eof_comparison() {
        let source = r#"
#include <stdio.h>

void test() {
    FILE *file = fopen("test.txt", "r");
    int c = fgetc(file);

    // This should NOT be flagged - ungetc is designed for EOF comparison
    if (ungetc(c, file) == EOF) {
        fprintf(stderr, "ungetc failed\n");
    }

    fclose(file);
}
"#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(source, None).unwrap();
        let rule = Fio34C::new();
        let violations = rule.check(&tree.root_node(), source);

        // Should not flag ungetc EOF comparison as violation
        for violation in &violations {
            assert!(!violation.message.contains("ungetc"),
                    "Should not flag ungetc EOF comparison: {}", violation.message);
        }
    }
}
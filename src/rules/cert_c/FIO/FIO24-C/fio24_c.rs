//! FIO24-C: Do not open a file that is already open
//!
//! This rule detects when the same file is opened multiple times within a single program.
//! According to C Standard (ISO/IEC 9899:2011, Section 7.21.3), "Whether the same file
//! can be simultaneously open multiple times is implementation-defined."
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! int main(void) {
//!   FILE *logfile = fopen("log", "a");
//!   // ...
//!   do_stuff();  // This function also opens "log"
//!   fclose(logfile);
//! }
//!
//! void do_stuff(void) {
//!   FILE *logfile = fopen("log", "a");  // Same file opened again
//!   fprintf(logfile, "do_stuff\n");
//! }
//! ```
//!
//! **Compliant:**
//! ```c
//! int main(void) {
//!   FILE *logfile = fopen("log", "a");
//!   do_stuff(logfile);  // Pass file pointer instead
//!   fclose(logfile);
//! }
//!
//! void do_stuff(FILE *logfile) {
//!   fprintf(logfile, "do_stuff\n");
//! }
//! ```
//!
//! ## Detection Strategy:
//! - Track fopen() calls and their filename arguments
//! - Track fclose() calls to remove files from open list
//! - Detect when the same filename is opened while still open
//! - Flag violations when duplicate opens are detected

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use std::collections::HashMap;
use tree_sitter::Node;

pub struct Fio24C;

impl CertRule for Fio24C {
    fn rule_id(&self) -> &'static str {
        "FIO24-C"
    }

    fn description(&self) -> &'static str {
        "Do not open a file that is already open"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "FIO24-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Track open files: filename -> (variable_name, opening_location)
        let mut open_files: HashMap<String, Vec<(String, tree_sitter::Point)>> = HashMap::new();
        // Track file pointer variables: variable_name -> filename
        let mut file_pointers: HashMap<String, String> = HashMap::new();

        self.check_node(
            node,
            source,
            &mut violations,
            &mut open_files,
            &mut file_pointers,
        );

        violations
    }
}

impl Fio24C {
    fn check_node(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        open_files: &mut HashMap<String, Vec<(String, tree_sitter::Point)>>,
        file_pointers: &mut HashMap<String, String>,
    ) {
        if node.kind() == "call_expression" {
            self.check_call_expression(node, source, violations, open_files, file_pointers);
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations, open_files, file_pointers);
            }
        }
    }

    fn check_call_expression(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        open_files: &mut HashMap<String, Vec<(String, tree_sitter::Point)>>,
        file_pointers: &mut HashMap<String, String>,
    ) {
        if let Some(func) = node.child_by_field_name("function") {
            let func_name = get_node_text(&func, source).trim();

            if func_name == "fopen" {
                self.check_fopen_call(node, source, violations, open_files, file_pointers);
            } else if func_name == "fclose" {
                self.check_fclose_call(node, source, open_files, file_pointers);
            }
        }
    }

    fn check_fopen_call(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        open_files: &mut HashMap<String, Vec<(String, tree_sitter::Point)>>,
        file_pointers: &mut HashMap<String, String>,
    ) {
        // Get the first argument (filename)
        if let Some(args) = node.child_by_field_name("arguments") {
            if let Some(first_arg) = self.get_first_argument(&args) {
                let filename = get_node_text(&first_arg, source).trim().to_string();

                // Check if this file is already open
                if let Some(existing_opens) = open_files.get(&filename) {
                    if !existing_opens.is_empty() {
                        // File is already open - report violation
                        let start_point = node.start_position();
                        let prev_open = &existing_opens[0];

                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: Severity::Medium,
                            message: format!(
                                "File '{}' is opened again while still open (previously opened at line {}). Opening the same file multiple times is implementation-defined and can cause race conditions.",
                                filename,
                                prev_open.1.row + 1
                            ),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some(
                                "Pass the file pointer as a function argument instead of reopening the file. Alternatively, close the file before opening it again.".to_string()
                            ),
                            ..Default::default()
                        });
                    }
                }

                // Track this open - get the variable name from parent assignment if present
                let var_name = self.get_assigned_variable(node, source);
                let location = node.start_position();

                open_files
                    .entry(filename.clone())
                    .or_default()
                    .push((var_name.clone(), location));

                if !var_name.is_empty() {
                    file_pointers.insert(var_name, filename);
                }
            }
        }
    }

    fn check_fclose_call(
        &self,
        node: &Node,
        source: &str,
        open_files: &mut HashMap<String, Vec<(String, tree_sitter::Point)>>,
        file_pointers: &mut HashMap<String, String>,
    ) {
        // Get the first argument (file pointer)
        if let Some(args) = node.child_by_field_name("arguments") {
            if let Some(first_arg) = self.get_first_argument(&args) {
                let fp_name = get_node_text(&first_arg, source).trim().to_string();

                // Look up which file this pointer refers to
                if let Some(filename) = file_pointers.get(&fp_name) {
                    // Remove this specific open from the tracking
                    if let Some(opens) = open_files.get_mut(filename) {
                        opens.retain(|(var, _)| var != &fp_name);
                        if opens.is_empty() {
                            open_files.remove(filename);
                        }
                    }
                    file_pointers.remove(&fp_name);
                }
            }
        }
    }

    fn get_first_argument<'a>(&self, args_node: &'a Node<'a>) -> Option<Node<'a>> {
        for i in 0..args_node.child_count() {
            if let Some(child) = args_node.child(i) {
                if child.kind() != "(" && child.kind() != ")" && child.kind() != "," {
                    return Some(child);
                }
            }
        }
        None
    }

    fn get_assigned_variable(&self, node: &Node, source: &str) -> String {
        // Look for parent assignment or declaration
        if let Some(parent) = node.parent() {
            match parent.kind() {
                "init_declarator" => {
                    // Variable declaration: FILE *fp = fopen(...)
                    if let Some(declarator) = parent.child_by_field_name("declarator") {
                        return self
                            .extract_identifier(&declarator, source)
                            .unwrap_or_default();
                    }
                }
                "assignment_expression" => {
                    // Variable assignment: fp = fopen(...)
                    if let Some(left) = parent.child_by_field_name("left") {
                        return get_node_text(&left, source).trim().to_string();
                    }
                }
                _ => {}
            }
        }
        String::new()
    }

    fn extract_identifier(&self, node: &Node, source: &str) -> Option<String> {
        if node.kind() == "identifier" {
            return Some(get_node_text(node, source).trim().to_string());
        }

        // Recursively search for identifier in declarator
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if let Some(id) = self.extract_identifier(&child, source) {
                    return Some(id);
                }
            }
        }
        None
    }
}

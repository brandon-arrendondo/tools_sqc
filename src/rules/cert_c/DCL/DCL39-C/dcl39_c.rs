//! DCL39-C: Avoid information leakage when passing a structure across a trust boundary
//!
//! Structures may contain padding bytes with uninitialized data. When passing
//! structures across trust boundaries (kernel/user space), these padding bytes
//! can leak sensitive information.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! struct test { int a; char b; int c; };
//! struct test arg = {.a = 1, .b = 2, .c = 3};
//! copy_to_user(usr_buf, &arg, sizeof(arg));  // Padding leaks data
//! ```
//!
//! **Compliant:**
//! ```c
//! // Use memset to zero all bytes including padding
//! memset(&arg, 0, sizeof(arg));
//! arg.a = 1; arg.b = 2; arg.c = 3;
//! copy_to_user(usr_buf, &arg, sizeof(arg));
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use std::collections::{HashMap, HashSet};
use tree_sitter::Node;

pub struct Dcl39C;

/// Tracks structure variable information
#[derive(Debug, Clone)]
struct StructVarInfo {
    var_name: String,
    #[allow(dead_code)]
    struct_type: String,
    is_zeroed: bool,
}

impl CertRule for Dcl39C {
    fn rule_id(&self) -> &'static str {
        "DCL39-C"
    }

    fn description(&self) -> &'static str {
        "Avoid information leakage when passing a structure across a trust boundary"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "DCL39-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Track structure variables and their initialization status
        let mut struct_vars: HashMap<String, StructVarInfo> = HashMap::new();
        let mut zeroed_vars: HashSet<String> = HashSet::new();

        // First pass: find memset calls that zero structures
        self.find_memset_calls(node, source, &mut zeroed_vars);

        // Second pass: find structure declarations and trust boundary calls
        self.analyze_structures(
            node,
            source,
            &mut struct_vars,
            &zeroed_vars,
            &mut violations,
        );

        violations
    }
}

impl Dcl39C {
    /// Find memset calls that zero variables
    fn find_memset_calls(&self, node: &Node, source: &str, zeroed_vars: &mut HashSet<String>) {
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = get_node_text(&function, source);

                if func_name == "memset" {
                    if let Some(args) = node.child_by_field_name("arguments") {
                        let arg_list = self.get_arguments(&args, source);
                        // memset(ptr, value, size) - if value is 0, it's zeroing
                        if arg_list.len() >= 2 && (arg_list[1] == "0" || arg_list[1] == "'\\0'") {
                            // Extract variable name from first argument
                            let first_arg = &arg_list[0];
                            if first_arg.starts_with('&') {
                                let var_name = first_arg[1..].trim().to_string();
                                zeroed_vars.insert(var_name);
                            }
                        }
                    }
                }
            }
        }

        // Recurse through children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.find_memset_calls(&child, source, zeroed_vars);
            }
        }
    }

    /// Analyze structures for trust boundary violations
    fn analyze_structures(
        &self,
        node: &Node,
        source: &str,
        struct_vars: &mut HashMap<String, StructVarInfo>,
        zeroed_vars: &HashSet<String>,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Check for structure variable declarations
        if node.kind() == "declaration" {
            if let Some((var_name, struct_type)) = self.extract_struct_declaration(node, source) {
                let is_zeroed = zeroed_vars.contains(&var_name);
                struct_vars.insert(
                    var_name.clone(),
                    StructVarInfo {
                        var_name,
                        struct_type,
                        is_zeroed,
                    },
                );
            }
        }

        // Check for trust boundary function calls
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = get_node_text(&function, source);

                if self.is_trust_boundary_function(&func_name) {
                    // Check if a structure is passed directly
                    if let Some(args) = node.child_by_field_name("arguments") {
                        let arg_list = self.get_arguments(&args, source);
                        for arg in &arg_list {
                            // Check for &struct_var pattern
                            if arg.starts_with('&') {
                                let var_name = arg[1..].trim().to_string();
                                // Check if this is a known struct variable that wasn't zeroed
                                if let Some(_info) = struct_vars.get(&var_name) {
                                    // Only check zeroed_vars (memset calls anywhere in function)
                                    // Don't check info.is_zeroed (which was set at declaration time)
                                    if !zeroed_vars.contains(&var_name) {
                                        violations.push(RuleViolation {
                                            rule_id: self.rule_id().to_string(),
                                            message: format!(
                                                "Structure '{}' passed to trust boundary function '{}' \
                                                 without clearing padding bytes. Use memset() to zero \
                                                 the structure or serialize fields individually.",
                                                var_name, func_name
                                            ),
                                            severity: self.severity(),
                                            line: node.start_position().row + 1,
                                            column: node.start_position().column + 1,
                                            file_path: String::new(),
                                            suggestion: Some(format!(
                                                "Add: memset(&{}, 0, sizeof({}));",
                                                var_name, var_name
                                            )),
                                            requires_manual_review: None,
                                        });
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // Recurse through children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.analyze_structures(&child, source, struct_vars, zeroed_vars, violations);
            }
        }
    }

    /// Extract structure variable name and type from declaration
    fn extract_struct_declaration(&self, decl: &Node, source: &str) -> Option<(String, String)> {
        let decl_text = get_node_text(decl, source);

        // Check if this is a struct declaration
        if !decl_text.contains("struct ") {
            return None;
        }

        // Look for type specifier and declarator
        let mut struct_type = String::new();
        let mut var_name = String::new();

        for i in 0..decl.child_count() {
            if let Some(child) = decl.child(i) {
                if child.kind() == "struct_specifier" || child.kind() == "type_identifier" {
                    struct_type = get_node_text(&child, source).to_string();
                }
                if child.kind() == "init_declarator" || child.kind() == "identifier" {
                    var_name = self.extract_var_name(&child, source);
                }
            }
        }

        if !var_name.is_empty() && !struct_type.is_empty() {
            Some((var_name, struct_type))
        } else {
            None
        }
    }

    /// Extract variable name from declarator
    fn extract_var_name(&self, node: &Node, source: &str) -> String {
        if node.kind() == "identifier" {
            return get_node_text(node, source).to_string();
        }

        // Recurse to find identifier
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                let name = self.extract_var_name(&child, source);
                if !name.is_empty() {
                    return name;
                }
            }
        }

        String::new()
    }

    /// Check if function is a trust boundary function
    fn is_trust_boundary_function(&self, name: &str) -> bool {
        matches!(
            name,
            "copy_to_user"
                | "write"
                | "send"
                | "sendto"
                | "sendmsg"
                | "ioctl"
                | "fwrite"
                | "writev"
        )
    }

    /// Get argument strings from argument_list node
    fn get_arguments(&self, args_node: &Node, source: &str) -> Vec<String> {
        let mut arguments = Vec::new();

        for i in 0..args_node.child_count() {
            if let Some(child) = args_node.child(i) {
                let kind = child.kind();
                if kind != "," && kind != "(" && kind != ")" {
                    let arg_text = get_node_text(&child, source).to_string();
                    arguments.push(arg_text);
                }
            }
        }

        arguments
    }
}

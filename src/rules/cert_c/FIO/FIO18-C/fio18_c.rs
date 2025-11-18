//! FIO18-C: Never expect fwrite() to terminate the writing process at a null character
//!
//! fwrite() writes exactly the number of bytes specified, regardless of null characters.
//! Using strlen() for binary data may write fewer bytes than intended, or writing beyond
//! the string length may expose uninitialized memory.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! char buf[100] = "Hello";
//! fwrite(buf, 1, sizeof(buf), fp);  // Writes all 100 bytes, including garbage
//! ```
//!
//! **Compliant:**
//! ```c
//! char buf[] = "Hello";
//! fwrite(buf, 1, strlen(buf) + 1, fp);  // Writes string + null terminator
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Fio18C;

impl CertRule for Fio18C {
    fn rule_id(&self) -> &'static str {
        "FIO18-C"
    }

    fn description(&self) -> &'static str {
        "Never expect fwrite() to terminate the writing process at a null character"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "FIO18-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_fwrite_usage(node, source, &mut violations);
        violations
    }
}

impl Fio18C {
    /// Check for potentially problematic fwrite() usage
    fn check_fwrite_usage(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = get_node_text(&function, source);
                if func_name == "fwrite" {
                    if let Some(args) = node.child_by_field_name("arguments") {
                        self.analyze_fwrite_args(&args, source, node, violations);
                    }
                }
            }
        }

        // Recurse
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_fwrite_usage(&child, source, violations);
            }
        }
    }

    /// Analyze fwrite arguments for potential issues
    /// fwrite(ptr, size, nmemb, stream)
    fn analyze_fwrite_args(
        &self,
        args: &Node,
        source: &str,
        call_node: &Node,
        violations: &mut Vec<RuleViolation>,
    ) {
        let arg_list = self.extract_args(args, source);

        if arg_list.len() >= 3 {
            let nmemb = &arg_list[2];

            // Check for sizeof() on array when writing string data
            // This is suspicious when writing to file - may write uninitialized data
            if nmemb.contains("sizeof(") && !nmemb.contains("strlen") {
                // Check if first argument looks like a char array/string
                let ptr = &arg_list[0];
                if self.looks_like_char_buffer(ptr) {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        message: format!(
                            "fwrite() using sizeof() on char buffer '{}'. \
                             May write uninitialized data beyond null terminator.",
                            ptr
                        ),
                        severity: self.severity(),
                        line: call_node.start_position().row + 1,
                        column: call_node.start_position().column + 1,
                        file_path: String::new(),
                        suggestion: Some(
                            "For strings, use strlen()+1 to include null terminator only. \
                             For binary data, ensure buffer is fully initialized."
                                .to_string(),
                        ),
                        requires_manual_review: Some(true),
                    });
                }
            }

            // Check for hardcoded size that might exceed actual data
            if let Ok(size) = nmemb.parse::<usize>() {
                if size > 1000 {
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        message: format!(
                            "fwrite() with large hardcoded size {}. \
                             Verify this matches actual initialized buffer size.",
                            size
                        ),
                        severity: self.severity(),
                        line: call_node.start_position().row + 1,
                        column: call_node.start_position().column + 1,
                        file_path: String::new(),
                        suggestion: Some(
                            "fwrite() writes exactly the number of bytes specified, \
                             regardless of null characters. Ensure all bytes are initialized."
                                .to_string(),
                        ),
                        requires_manual_review: Some(true),
                    });
                }
            }
        }
    }

    /// Extract arguments from argument list
    fn extract_args(&self, args: &Node, source: &str) -> Vec<String> {
        let mut result = Vec::new();
        for i in 0..args.child_count() {
            if let Some(child) = args.child(i) {
                if child.kind() != "(" && child.kind() != ")" && child.kind() != "," {
                    result.push(get_node_text(&child, source).trim().to_string());
                }
            }
        }
        result
    }

    /// Check if identifier looks like a char buffer
    fn looks_like_char_buffer(&self, name: &str) -> bool {
        let name_lower = name.to_lowercase();
        name_lower.contains("buf")
            || name_lower.contains("str")
            || name_lower.contains("text")
            || name_lower.contains("msg")
            || name_lower.contains("name")
    }
}

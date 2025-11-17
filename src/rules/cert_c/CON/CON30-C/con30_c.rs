//! CON30-C: Clean up thread-specific storage
//!
//! Thread-specific storage (TSS) must be properly freed to avoid memory leaks.
//! When using tss_set() to store allocated memory, ensure either:
//! 1. A destructor is registered via tss_create(&key, destructor), OR
//! 2. Memory is explicitly freed via free(tss_get(key)) before thread exit
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! tss_create(&key, NULL);  // No destructor
//! int *data = malloc(sizeof(int));
//! tss_set(key, data);      // Memory leak - never freed
//! ```
//!
//! **Compliant (Destructor):**
//! ```c
//! tss_create(&key, free);  // Destructor registered
//! ```
//!
//! **Compliant (Explicit):**
//! ```c
//! free(tss_get(key));      // Explicit cleanup before thread exit
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use std::collections::{HashMap, HashSet};
use tree_sitter::Node;

pub struct Con30C;

/// Tracks TSS key information
#[derive(Debug, Clone)]
struct TssKeyInfo {
    key_name: String,
    has_destructor: bool,
    create_line: usize,
    create_column: usize,
}

impl CertRule for Con30C {
    fn rule_id(&self) -> &'static str {
        "CON30-C"
    }

    fn description(&self) -> &'static str {
        "Clean up thread-specific storage"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "CON30-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Track TSS operations
        let mut tss_keys: HashMap<String, TssKeyInfo> = HashMap::new();
        let mut tss_set_calls: HashSet<String> = HashSet::new();
        let mut tss_get_freed: HashSet<String> = HashSet::new();

        self.analyze_tss_operations(
            node,
            source,
            &mut tss_keys,
            &mut tss_set_calls,
            &mut tss_get_freed,
        );

        // Check for violations: keys with tss_set but no destructor and no explicit free
        for (key_name, key_info) in &tss_keys {
            // If tss_set was called for this key
            if tss_set_calls.contains(key_name) {
                // And no destructor was registered
                if !key_info.has_destructor {
                    // And tss_get result wasn't freed
                    if !tss_get_freed.contains(key_name) {
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            message: format!(
                                "Thread-specific storage key '{}' created without destructor and \
                                 memory stored via tss_set() is never freed. Register a destructor \
                                 in tss_create() or explicitly free(tss_get({})).",
                                key_name, key_name
                            ),
                            severity: self.severity(),
                            line: key_info.create_line,
                            column: key_info.create_column,
                            file_path: String::new(),
                            suggestion: Some(format!(
                                "Either register a destructor: tss_create(&{}, free) or \
                                 explicitly cleanup: free(tss_get({}))",
                                key_name, key_name
                            )),
                            requires_manual_review: None,
                        });
                    }
                }
            }
        }

        violations
    }
}

impl Con30C {
    /// Analyze the AST for TSS operations
    fn analyze_tss_operations(
        &self,
        node: &Node,
        source: &str,
        tss_keys: &mut HashMap<String, TssKeyInfo>,
        tss_set_calls: &mut HashSet<String>,
        tss_get_freed: &mut HashSet<String>,
    ) {
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = get_node_text(&function, source);

                // Check for tss_create
                if func_name == "tss_create" {
                    if let Some((key_name, has_destructor)) =
                        self.extract_tss_create_info(node, source)
                    {
                        tss_keys.insert(
                            key_name.clone(),
                            TssKeyInfo {
                                key_name,
                                has_destructor,
                                create_line: node.start_position().row + 1,
                                create_column: node.start_position().column + 1,
                            },
                        );
                    }
                }

                // Check for tss_set
                if func_name == "tss_set" {
                    if let Some(key_name) = self.extract_tss_key_name(node, source) {
                        tss_set_calls.insert(key_name);
                    }
                }

                // Check for free(tss_get(key))
                if func_name == "free" {
                    if let Some(key_name) = self.check_tss_get_in_free(node, source) {
                        tss_get_freed.insert(key_name);
                    }
                }
            }
        }

        // Recurse through children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.analyze_tss_operations(&child, source, tss_keys, tss_set_calls, tss_get_freed);
            }
        }
    }

    /// Extract key name and destructor info from tss_create(&key, destructor)
    fn extract_tss_create_info(&self, call_node: &Node, source: &str) -> Option<(String, bool)> {
        if let Some(args) = call_node.child_by_field_name("arguments") {
            let arg_list = self.get_arguments(args, source);

            if arg_list.len() >= 2 {
                // First arg is &key
                let key_arg = arg_list[0].trim();
                let key_name = if key_arg.starts_with('&') {
                    key_arg[1..].trim().to_string()
                } else {
                    key_arg.to_string()
                };

                // Second arg is destructor (NULL means no destructor)
                let destructor_arg = arg_list[1].trim();
                let has_destructor = destructor_arg != "NULL"
                    && destructor_arg != "0"
                    && destructor_arg != "nullptr"
                    && !destructor_arg.is_empty();

                return Some((key_name, has_destructor));
            }
        }
        None
    }

    /// Extract key name from tss_set(key, value) or tss_get(key)
    fn extract_tss_key_name(&self, call_node: &Node, source: &str) -> Option<String> {
        if let Some(args) = call_node.child_by_field_name("arguments") {
            let arg_list = self.get_arguments(args, source);

            if !arg_list.is_empty() {
                return Some(arg_list[0].trim().to_string());
            }
        }
        None
    }

    /// Check if free() is called on tss_get() result
    fn check_tss_get_in_free(&self, call_node: &Node, source: &str) -> Option<String> {
        if let Some(args) = call_node.child_by_field_name("arguments") {
            // Look for tss_get call inside free's arguments
            for i in 0..args.child_count() {
                if let Some(child) = args.child(i) {
                    if let Some(key) = self.find_tss_get_key(&child, source) {
                        return Some(key);
                    }
                }
            }
        }
        None
    }

    /// Recursively find tss_get call and return the key name
    fn find_tss_get_key(&self, node: &Node, source: &str) -> Option<String> {
        if node.kind() == "call_expression" {
            if let Some(function) = node.child_by_field_name("function") {
                let func_name = get_node_text(&function, source);
                if func_name == "tss_get" {
                    return self.extract_tss_key_name(node, source);
                }
            }
        }

        // Recurse
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if let Some(key) = self.find_tss_get_key(&child, source) {
                    return Some(key);
                }
            }
        }

        None
    }

    /// Get argument strings from an argument_list node
    fn get_arguments(&self, args_node: Node, source: &str) -> Vec<String> {
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

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_tss_without_destructor() {
        let code = r#"
            #include <threads.h>
            tss_t key;
            int main(void) {
                tss_create(&key, NULL);
                int *data = malloc(sizeof(int));
                tss_set(key, data);
                return 0;
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();

        let rule = Con30C;
        let violations = rule.check(&root, code);

        assert!(
            !violations.is_empty(),
            "Should detect TSS without destructor or cleanup"
        );
    }

    #[test]
    fn test_tss_with_destructor() {
        let code = r#"
            #include <threads.h>
            tss_t key;
            void destructor(void *data) { free(data); }
            int main(void) {
                tss_create(&key, destructor);
                int *data = malloc(sizeof(int));
                tss_set(key, data);
                return 0;
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();

        let rule = Con30C;
        let violations = rule.check(&root, code);

        assert!(
            violations.is_empty(),
            "Should not flag TSS with destructor: {:?}",
            violations
        );
    }

    #[test]
    fn test_tss_with_explicit_free() {
        let code = r#"
            #include <threads.h>
            tss_t key;
            int function(void *dummy) {
                tss_create(&key, NULL);
                int *data = malloc(sizeof(int));
                tss_set(key, data);
                free(tss_get(key));
                return 0;
            }
        "#;

        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();

        let rule = Con30C;
        let violations = rule.check(&root, code);

        assert!(
            violations.is_empty(),
            "Should not flag TSS with explicit free: {:?}",
            violations
        );
    }
}

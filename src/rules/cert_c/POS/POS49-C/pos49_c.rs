use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Pos49C;

impl CertRule for Pos49C {
    fn rule_id(&self) -> &'static str {
        "POS49-C"
    }

    fn description(&self) -> &'static str {
        "Do not access shared bit-fields from multiple threads without mutex protection"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        self.rule_id()
    }

    fn check(&self, root: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Only check if source has bit fields and threading patterns
        if !self.has_bit_fields(source) || !self.has_potential_thread_access(source) {
            return violations;
        }

        // Find all bit-field accesses that are NOT protected by mutex
        self.check_node(root, source, &mut violations);

        violations
    }
}

impl Pos49C {
    fn has_bit_fields(&self, source: &str) -> bool {
        // Look for bit field pattern in struct: "unsigned int name : width"
        source.contains("unsigned") && source.contains(':') && source.contains("struct")
    }

    fn check_node<'a>(&self, node: &Node<'a>, source: &str, violations: &mut Vec<RuleViolation>) {
        // Look for field_expression (member access like "flags.flag1 = 1")
        if node.kind() == "field_expression" {
            // Check if this is accessing a bit-field member
            if let Some(field) = node.child_by_field_name("field") {
                let field_name = &source[field.start_byte()..field.end_byte()];

                // Check if this field access is part of an assignment or expression
                if let Some(parent) = node.parent() {
                    if matches!(parent.kind(), "assignment_expression" | "update_expression") {
                        // Skip if the base variable is a local stack variable —
                        // local variables are inherently thread-safe
                        if self.is_local_variable(node, source) {
                            // Local stack variable, not shared across threads
                        } else if !self.is_within_mutex_lock(node, source) {
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                severity: self.severity(),
                                message: format!(
                                    "Bit-field '{}' may be accessed from multiple threads without mutex protection",
                                    field_name
                                ),
                                file_path: String::new(),
                                line: node.start_position().row + 1,
                                column: node.start_position().column + 1,
                                suggestion: Some("Protect bit-field access with pthread_mutex_lock/unlock".to_string()),
                                requires_manual_review: Some(true),
                            });
                        }
                    }
                }
            }
        }

        // Recursively check child nodes
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }

    fn is_local_variable(&self, field_expr: &Node, source: &str) -> bool {
        // Get the base variable name from the field expression (e.g., "servaddr" from "servaddr.sin_port")
        let base_name = if let Some(argument) = field_expr.child_by_field_name("argument") {
            let text = get_node_text(&argument, source);
            let text = text.trim();
            // Handle pointer dereference: (*ptr).field
            if text.starts_with('(') || text.starts_with('*') {
                return false; // Conservative: pointer-based access might be shared
            }
            text.to_string()
        } else {
            return false;
        };

        // Walk up to find the enclosing function_definition
        let mut current = field_expr.parent();
        while let Some(node) = current {
            if node.kind() == "function_definition" {
                // Search the function body for a local declaration of this variable
                if let Some(body) = node.child_by_field_name("body") {
                    return self.has_local_declaration(&body, source, &base_name);
                }
                return false;
            }
            current = node.parent();
        }
        false
    }

    fn has_local_declaration(&self, node: &Node, source: &str, var_name: &str) -> bool {
        if node.kind() == "declaration" {
            let decl_text = get_node_text(node, source);
            // Check if this declaration contains the variable name as a declarator
            let mut cursor = node.walk();
            for child in node.children(&mut cursor) {
                if child.kind() == "init_declarator" || child.kind() == "identifier" {
                    let name = get_node_text(&child, source);
                    // For init_declarator, extract the identifier
                    if child.kind() == "init_declarator" {
                        if let Some(declarator) = child.child_by_field_name("declarator") {
                            let decl_name = get_node_text(&declarator, source).trim().to_string();
                            if decl_name == var_name {
                                // Ensure it's not a pointer parameter or extern/static global
                                if !decl_text.contains("extern") && !decl_text.contains("static") {
                                    return true;
                                }
                            }
                        }
                    } else if name.trim() == var_name {
                        if !decl_text.contains("extern") && !decl_text.contains("static") {
                            return true;
                        }
                    }
                }
            }
        }

        // Recurse into child nodes (but not into nested functions)
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if child.kind() != "function_definition" {
                if self.has_local_declaration(&child, source, var_name) {
                    return true;
                }
            }
        }
        false
    }

    fn is_within_mutex_lock(&self, node: &Node, source: &str) -> bool {
        // Check if this node is between pthread_mutex_lock and pthread_mutex_unlock
        // Get the containing compound_statement or function
        let mut current = node.parent();
        while let Some(parent) = current {
            if parent.kind() == "compound_statement" {
                let node_pos = node.start_byte();
                let _stmt_text = &source[parent.start_byte()..parent.end_byte()];

                // Simple heuristic: check if we're between lock and unlock calls
                // Find text before this node in the statement
                let before_text = &source[parent.start_byte()..node_pos];
                let after_text = &source[node_pos..parent.end_byte()];

                // Check if there's a mutex_lock before and mutex_unlock after
                let has_lock_before = before_text.contains("pthread_mutex_lock")
                    || before_text.contains("mutex_lock");
                let has_unlock_after = after_text.contains("pthread_mutex_unlock")
                    || after_text.contains("mutex_unlock");

                return has_lock_before && has_unlock_after;
            }
            current = parent.parent();
        }
        false
    }

    fn has_potential_thread_access(&self, source: &str) -> bool {
        // Heuristic: look for pthread or multiple functions suggesting threads
        source.contains("pthread_create") ||
        source.contains("pthread_t") ||
        source.contains("thread") ||
        // Check for multiple functions that might be thread targets
        (source.matches("void ").count() >= 2 && source.contains("flags"))
    }
}

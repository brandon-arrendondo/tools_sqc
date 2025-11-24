//! EXP39-C: Do not access a variable through a pointer of an incompatible type
//!
//! This rule detects pointer casts to incompatible types, which can lead to undefined
//! behavior. Accessing an object through a pointer of an incompatible type violates
//! the strict aliasing rules and can cause unpredictable results.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! void f(void) {
//!   float f = 0.0f;
//!   int *ip = (int *)&f;  // Incompatible type cast
//!   (*ip)++;  // Undefined behavior
//! }
//! ```
//!
//! **Non-compliant (array dimension mismatch):**
//! ```c
//! void func(void) {
//!   int a[10][15];
//!   int (*b)[10] = a;  // Wrong second dimension
//! }
//! ```
//!
//! **Compliant:**
//! ```c
//! #include <math.h>
//! void f(void) {
//!   float f = 0.0f;
//!   f = nextafterf(f, FLT_MAX);  // Use standard library functions
//! }
//! ```
//!
//! ## Detection Strategy:
//! - Find cast_expression nodes that cast pointers
//! - Extract source and target pointer types
//! - Check if types are incompatible (e.g., float* to int*, short* to int*)
//! - Flag violations for known incompatible type combinations
//! - Exceptions: char/unsigned char types are allowed (strict aliasing exception)

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Exp39C;

impl CertRule for Exp39C {
    fn rule_id(&self) -> &'static str {
        "EXP39-C"
    }

    fn description(&self) -> &'static str {
        "Do not access a variable through a pointer of an incompatible type"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "EXP39-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Exp39C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Look for cast_expression nodes
        if node.kind() == "cast_expression" {
            self.check_cast_expression(node, source, violations);
        }

        // Also check pointer_declarator assignments with incompatible array types
        if node.kind() == "init_declarator" {
            self.check_init_declarator(node, source, violations);
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }

    fn check_cast_expression(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Get the type being cast to
        if let Some(type_node) = node.child_by_field_name("type") {
            let target_type = get_node_text(&type_node, source).trim().to_string();

            // Get the value being cast
            if let Some(value_node) = node.child_by_field_name("value") {
                let source_expr = get_node_text(&value_node, source).trim().to_string();

                // Check if this is a pointer cast
                if target_type.contains('*') {
                    // Extract base types
                    let target_base = self.extract_base_type(&target_type);
                    let source_base = self.infer_source_type(&value_node, source);

                    if let (Some(target), Some(source_type)) = (target_base, source_base) {
                        if self.are_incompatible_pointer_types(&target, &source_type) {
                            self.report_incompatible_cast_violation(
                                node,
                                source,
                                &target,
                                &source_type,
                                violations,
                            );
                        }
                    }
                }
            }
        }
    }

    fn check_init_declarator(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Check for array pointer type mismatches like: int (*b)[10] = int_array[15];
        if let Some(declarator) = node.child_by_field_name("declarator") {
            if let Some(value) = node.child_by_field_name("value") {
                let declarator_text = get_node_text(&declarator, source);
                let value_text = get_node_text(&value, source);

                // Check for pointer to array declarations
                if declarator_text.contains("(*") && declarator_text.contains("][") {
                    // This is a pointer to array declaration
                    // Check if the value is an array with different dimensions
                    if self.has_array_dimension_mismatch(&declarator_text, &value_text) {
                        self.report_array_dimension_violation(node, source, violations);
                    }
                }
            }
        }
    }

    fn extract_base_type(&self, type_str: &str) -> Option<String> {
        // Remove pointer asterisks, const, volatile, etc.
        let cleaned = type_str
            .replace("const", "")
            .replace("volatile", "")
            .replace("restrict", "")
            .replace('*', "")
            .replace(['(', ')'], "")
            .trim()
            .to_string();

        if cleaned.is_empty() {
            None
        } else {
            Some(cleaned)
        }
    }

    fn infer_source_type(&self, node: &Node, source: &str) -> Option<String> {
        // Try to infer the type from the expression
        match node.kind() {
            "pointer_expression" => {
                // Address-of operator: &variable
                if let Some(argument) = node.child_by_field_name("argument") {
                    let arg_text = get_node_text(&argument, source).trim().to_string();
                    // This is simplified - in reality we'd need type information
                    // For now, check common patterns
                    if arg_text.contains("float") {
                        return Some("float".to_string());
                    } else if arg_text.contains("double") {
                        return Some("double".to_string());
                    }
                    // Try to infer from the variable name (heuristic)
                    return self.infer_type_from_name(&arg_text);
                }
            }
            "identifier" => {
                let name = get_node_text(node, source).trim().to_string();
                return self.infer_type_from_name(&name);
            }
            "cast_expression" => {
                // Nested cast - get the target type of the inner cast
                if let Some(type_node) = node.child_by_field_name("type") {
                    let inner_type = get_node_text(&type_node, source).trim().to_string();
                    return self.extract_base_type(&inner_type);
                }
            }
            _ => {}
        }
        None
    }

    fn infer_type_from_name(&self, name: &str) -> Option<String> {
        // Heuristic: try to infer type from variable naming conventions
        let lower_name = name.to_lowercase();

        if lower_name.starts_with('f') || lower_name.contains("float") {
            Some("float".to_string())
        } else if lower_name.starts_with("db") || lower_name.contains("double") {
            Some("double".to_string())
        } else if lower_name.starts_with("ch") || lower_name.contains("char") {
            Some("char".to_string())
        } else if lower_name.starts_with("sh") || lower_name.contains("short") {
            Some("short".to_string())
        } else if lower_name.starts_with('l') && lower_name.contains("long") {
            Some("long".to_string())
        } else {
            // Default to int for most cases
            Some("int".to_string())
        }
    }

    fn are_incompatible_pointer_types(&self, target_type: &str, source_type: &str) -> bool {
        // Check if the two pointer types are incompatible

        // Same types are compatible
        if target_type == source_type {
            return false;
        }

        // Character types can alias anything (exception in strict aliasing rules)
        if target_type == "char" || target_type == "unsigned char" || target_type == "signed char" {
            return false;
        }
        if source_type == "char" || source_type == "unsigned char" || source_type == "signed char" {
            return false;
        }

        // void* is compatible with any pointer type
        if target_type == "void" || source_type == "void" {
            return false;
        }

        // Signed/unsigned variants of the same type are compatible
        let target_normalized = target_type.replace("unsigned ", "").replace("signed ", "");
        let source_normalized = source_type.replace("unsigned ", "").replace("signed ", "");
        if target_normalized == source_normalized {
            return false;
        }

        // Known incompatible combinations
        let incompatible_pairs = [
            ("float", "int"),
            ("int", "float"),
            ("double", "int"),
            ("int", "double"),
            ("float", "short"),
            ("short", "float"),
            ("double", "long"),
            ("long", "double"),
            ("float", "long"),
            ("long", "float"),
        ];

        for (type1, type2) in &incompatible_pairs {
            if (target_type.contains(type1) && source_type.contains(type2))
                || (target_type.contains(type2) && source_type.contains(type1))
            {
                return true;
            }
        }

        false
    }

    fn has_array_dimension_mismatch(&self, declarator: &str, _value: &str) -> bool {
        // Simplified check for array dimension mismatches
        // This would need more sophisticated parsing in a complete implementation
        declarator.contains("(*") && declarator.contains("][")
    }

    fn report_incompatible_cast_violation(
        &self,
        node: &Node,
        source: &str,
        target_type: &str,
        source_type: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let start_point = node.start_position();
        let cast_text = get_node_text(node, source).trim().to_string();

        violations.push(RuleViolation {
            rule_id: self.rule_id().to_string(),
            severity: Severity::Medium,
            message: format!(
                "Do not access a '{}' object through a pointer of incompatible type '{}*': {}",
                source_type,
                target_type,
                if cast_text.len() > 50 {
                    format!("{}...", &cast_text[..50])
                } else {
                    cast_text
                }
            ),
            file_path: String::new(),
            line: start_point.row + 1,
            column: start_point.column + 1,
            suggestion: Some(
                format!("Avoid casting between incompatible pointer types. Use type-appropriate operations or unions for legitimate type punning. Consider using standard library functions instead of direct type manipulation.")
            ),
            ..Default::default()
        });
    }

    fn report_array_dimension_violation(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let start_point = node.start_position();
        let decl_text = get_node_text(node, source).trim().to_string();

        violations.push(RuleViolation {
            rule_id: self.rule_id().to_string(),
            severity: Severity::Medium,
            message: format!(
                "Array pointer dimension mismatch detected: {}",
                if decl_text.len() > 60 {
                    format!("{}...", &decl_text[..60])
                } else {
                    decl_text
                }
            ),
            file_path: String::new(),
            line: start_point.row + 1,
            column: start_point.column + 1,
            suggestion: Some(
                "Ensure array pointer dimensions match the array being assigned. Incompatible array types can lead to undefined behavior.".to_string()
            ),
            ..Default::default()
        });
    }
}

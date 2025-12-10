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
use std::collections::HashMap;
use tree_sitter::Node;

pub struct Exp39C;

/// Collected variable type information
#[derive(Debug, Clone)]
struct VarTypeInfo {
    base_type: String,
    is_array: bool,
    array_dimensions: Vec<String>, // e.g., ["ROWS", "COLS"] for int a[ROWS][COLS]
}

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

        // First pass: collect variable type information
        let mut var_types = HashMap::new();
        self.collect_var_types(node, source, &mut var_types);

        // Second pass: check for violations
        self.check_node(node, source, &mut violations, &var_types);

        violations
    }
}

impl Exp39C {
    /// Collect variable type information from declarations
    fn collect_var_types(
        &self,
        node: &Node,
        source: &str,
        var_types: &mut HashMap<String, VarTypeInfo>,
    ) {
        if node.kind() == "declaration" {
            self.process_declaration(node, source, var_types);
        }

        // Recurse into children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.collect_var_types(&child, source, var_types);
            }
        }
    }

    /// Process a declaration to extract variable types
    fn process_declaration(
        &self,
        node: &Node,
        source: &str,
        var_types: &mut HashMap<String, VarTypeInfo>,
    ) {
        // Get the type specifier
        let mut base_type = String::new();
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "primitive_type" | "sized_type_specifier" | "type_identifier" => {
                        base_type = get_node_text(&child, source).trim().to_string();
                        break;
                    }
                    "struct_specifier" => {
                        // Handle struct declarations like "struct gadget *gp;"
                        base_type = get_node_text(&child, source).trim().to_string();
                        break;
                    }
                    _ => {}
                }
            }
        }

        if base_type.is_empty() {
            return;
        }

        // Get the declarator(s) - handle pointer_declarator for struct pointers
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "init_declarator" | "array_declarator" => {
                        self.extract_var_from_declarator(&child, source, &base_type, var_types);
                    }
                    "pointer_declarator" => {
                        // Handle "struct gadget *gp;"
                        if let Some(var_name) = self.find_var_name(&child, source) {
                            var_types.insert(
                                var_name,
                                VarTypeInfo {
                                    base_type: base_type.clone(),
                                    is_array: false,
                                    array_dimensions: Vec::new(),
                                },
                            );
                        }
                    }
                    "identifier" => {
                        let var_name = get_node_text(&child, source).trim().to_string();
                        var_types.insert(
                            var_name,
                            VarTypeInfo {
                                base_type: base_type.clone(),
                                is_array: false,
                                array_dimensions: Vec::new(),
                            },
                        );
                    }
                    _ => {}
                }
            }
        }
    }

    /// Extract variable name and type from a declarator
    fn extract_var_from_declarator(
        &self,
        node: &Node,
        source: &str,
        base_type: &str,
        var_types: &mut HashMap<String, VarTypeInfo>,
    ) {
        let node_text = get_node_text(node, source);
        let is_array = node.kind() == "array_declarator" || node_text.contains('[');

        // Extract array dimensions
        let array_dimensions = self.extract_array_dimensions(node, source);

        // Find the identifier
        if let Some(var_name) = self.find_var_name(node, source) {
            var_types.insert(
                var_name,
                VarTypeInfo {
                    base_type: base_type.to_string(),
                    is_array,
                    array_dimensions,
                },
            );
        }
    }

    /// Extract array dimensions from a declarator (e.g., [ROWS][COLS])
    /// Returns dimensions in declaration order: for int a[ROWS][COLS], returns ["ROWS", "COLS"]
    fn extract_array_dimensions(&self, node: &Node, source: &str) -> Vec<String> {
        let mut dimensions = Vec::new();
        self.collect_array_dimensions_recursive(node, source, &mut dimensions);
        // Tree-sitter parses array declarators from outer to inner in the AST,
        // but we want them in source order (first dimension first).
        // Since we push as we recurse, the vector is in reverse source order.
        // Reverse it to get proper source order: [ROWS][COLS] -> ["ROWS", "COLS"]
        dimensions.reverse();
        dimensions
    }

    /// Recursively collect array dimensions
    fn collect_array_dimensions_recursive(
        &self,
        node: &Node,
        source: &str,
        dimensions: &mut Vec<String>,
    ) {
        if node.kind() == "array_declarator" {
            // Get the size expression (last child that's not '[' or ']')
            if let Some(size_node) = node.child_by_field_name("size") {
                let size_text = get_node_text(&size_node, source).trim().to_string();
                dimensions.push(size_text);
            }
            // Recurse into nested array declarators
            if let Some(declarator) = node.child_by_field_name("declarator") {
                self.collect_array_dimensions_recursive(&declarator, source, dimensions);
            }
        }

        // Check children for nested array declarators
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "array_declarator" {
                    self.collect_array_dimensions_recursive(&child, source, dimensions);
                }
            }
        }
    }

    /// Find variable name in a declarator
    fn find_var_name(&self, node: &Node, source: &str) -> Option<String> {
        if node.kind() == "identifier" {
            return Some(get_node_text(node, source).trim().to_string());
        }

        // Check named fields
        if let Some(declarator) = node.child_by_field_name("declarator") {
            return self.find_var_name(&declarator, source);
        }

        // Recurse into children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if let Some(name) = self.find_var_name(&child, source) {
                    return Some(name);
                }
            }
        }

        None
    }

    fn check_node(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        var_types: &HashMap<String, VarTypeInfo>,
    ) {
        // Look for cast_expression nodes
        if node.kind() == "cast_expression" {
            self.check_cast_expression(node, source, violations, var_types);
        }

        // Check for realloc with struct type casting
        if node.kind() == "assignment_expression" {
            self.check_realloc_cast(node, source, violations, var_types);
        }

        // Also check pointer_declarator assignments with incompatible array types
        if node.kind() == "init_declarator" {
            self.check_init_declarator(node, source, violations, var_types);
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations, var_types);
            }
        }
    }

    /// Check for realloc casts from one struct type to another
    fn check_realloc_cast(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        var_types: &HashMap<String, VarTypeInfo>,
    ) {
        if let Some(right) = node.child_by_field_name("right") {
            // Check if right side is a cast expression containing realloc
            if right.kind() == "cast_expression" {
                let right_text = get_node_text(&right, source);
                if right_text.contains("realloc") {
                    // Get the cast target type
                    if let Some(type_node) = right.child_by_field_name("type") {
                        let target_type = get_node_text(&type_node, source).trim().to_string();

                        // Check if casting to a struct pointer
                        if target_type.contains("struct") && target_type.contains('*') {
                            // Get the variable being assigned to
                            if let Some(left) = node.child_by_field_name("left") {
                                let var_name = get_node_text(&left, source).trim().to_string();

                                // Check if there's a memset after this realloc in the same source
                                // If memset is used to reinitialize, it's compliant
                                let node_end = node.end_byte();
                                let source_after = &source[node_end..];
                                let has_memset = source_after
                                    .contains(&format!("memset({}", var_name))
                                    || source_after.contains(&format!("memset( {}", var_name));

                                if !has_memset {
                                    // Also check the realloc argument type
                                    if let Some(value) = right.child_by_field_name("value") {
                                        self.check_realloc_argument_type(
                                            &value,
                                            source,
                                            &target_type,
                                            var_types,
                                            violations,
                                            node,
                                        );
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /// Check if realloc is called with mismatched pointer type
    fn check_realloc_argument_type(
        &self,
        node: &Node,
        source: &str,
        target_type: &str,
        var_types: &HashMap<String, VarTypeInfo>,
        violations: &mut Vec<RuleViolation>,
        violation_node: &Node,
    ) {
        if node.kind() == "call_expression" {
            if let Some(func) = node.child_by_field_name("function") {
                let func_name = get_node_text(&func, source).trim();
                if func_name == "realloc" {
                    // Get first argument (the pointer being reallocated)
                    if let Some(args) = node.child_by_field_name("arguments") {
                        for i in 0..args.child_count() {
                            if let Some(arg) = args.child(i) {
                                if arg.kind() != "(" && arg.kind() != ")" && arg.kind() != "," {
                                    let arg_name = get_node_text(&arg, source).trim().to_string();
                                    if let Some(arg_type) = var_types.get(&arg_name) {
                                        // Check if source and target struct types differ
                                        if arg_type.base_type.contains("struct")
                                            && target_type.contains("struct")
                                        {
                                            let target_struct =
                                                self.extract_struct_name(target_type);
                                            let source_struct =
                                                self.extract_struct_name(&arg_type.base_type);

                                            if target_struct != source_struct {
                                                self.report_struct_cast_violation(
                                                    violation_node,
                                                    source,
                                                    &target_struct,
                                                    &source_struct,
                                                    violations,
                                                );
                                            }
                                        }
                                    }
                                    break; // Only check first argument
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /// Extract struct name from a type string
    fn extract_struct_name(&self, type_str: &str) -> String {
        // Extract "widget" from "struct widget *" or "struct widget"
        let cleaned = type_str
            .replace('*', "")
            .replace("struct", "")
            .trim()
            .to_string();
        cleaned
    }

    /// Report violation for casting between different struct types
    fn report_struct_cast_violation(
        &self,
        node: &Node,
        source: &str,
        target_struct: &str,
        source_struct: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        let start_point = node.start_position();
        let stmt_text = get_node_text(node, source).trim().to_string();

        violations.push(RuleViolation {
            rule_id: self.rule_id().to_string(),
            severity: Severity::Medium,
            message: format!(
                "Accessing memory allocated as 'struct {}' through pointer to incompatible 'struct {}': {}",
                source_struct,
                target_struct,
                if stmt_text.len() > 50 {
                    format!("{}...", &stmt_text[..50])
                } else {
                    stmt_text
                }
            ),
            file_path: String::new(),
            line: start_point.row + 1,
            column: start_point.column + 1,
            suggestion: Some(
                "When using realloc to resize memory, ensure the data is properly reinitialized for the new type. Do not access struct members as if they contain valid data from a different struct type.".to_string()
            ),
            ..Default::default()
        });
    }

    fn check_cast_expression(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        var_types: &HashMap<String, VarTypeInfo>,
    ) {
        // Get the type being cast to
        if let Some(type_node) = node.child_by_field_name("type") {
            let target_type = get_node_text(&type_node, source).trim().to_string();

            // Get the value being cast
            if let Some(value_node) = node.child_by_field_name("value") {
                // Check if this is a pointer cast
                if target_type.contains('*') {
                    // Extract base types
                    let target_base = self.extract_base_type(&target_type);
                    let source_base = self.infer_source_type(&value_node, source, var_types);

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
        var_types: &HashMap<String, VarTypeInfo>,
    ) {
        // Check for array pointer type mismatches like: int (*b)[ROWS] = a;
        if let Some(declarator) = node.child_by_field_name("declarator") {
            if let Some(value) = node.child_by_field_name("value") {
                let declarator_text = get_node_text(&declarator, source);
                let value_text = get_node_text(&value, source).trim().to_string();

                // Check for pointer to array declarations: (*var)[size]
                if declarator_text.contains("(*") && declarator_text.contains('[') {
                    // Extract the dimension from the pointer-to-array declaration
                    // e.g., (*b)[ROWS] should extract "ROWS"
                    if let Some(ptr_dimension) =
                        self.extract_pointer_array_dimension(&declarator_text)
                    {
                        // Check if value is an array variable
                        if let Some(var_info) = var_types.get(&value_text) {
                            if var_info.is_array && !var_info.array_dimensions.is_empty() {
                                // For int a[ROWS][COLS], the second dimension should match
                                // int (*b)[COLS] = a; is valid (pointer to array of COLS ints)
                                // int (*b)[ROWS] = a; is invalid (wrong dimension)

                                // The pointer-to-array dimension should match the LAST dimension
                                // of the source array (the inner-most array dimension)
                                let last_dimension = var_info.array_dimensions.last();
                                if let Some(last_dim) = last_dimension {
                                    if *last_dim != ptr_dimension {
                                        self.report_array_dimension_violation(
                                            node, source, violations,
                                        );
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /// Extract the dimension from a pointer-to-array declaration like "(*b)[ROWS]"
    fn extract_pointer_array_dimension(&self, decl_text: &str) -> Option<String> {
        // Find the last [...] in the declarator
        if let Some(bracket_start) = decl_text.rfind('[') {
            if let Some(bracket_end) = decl_text.rfind(']') {
                if bracket_start < bracket_end {
                    let dimension = decl_text[bracket_start + 1..bracket_end].trim().to_string();
                    if !dimension.is_empty() {
                        return Some(dimension);
                    }
                }
            }
        }
        None
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

    fn infer_source_type(
        &self,
        node: &Node,
        source: &str,
        var_types: &HashMap<String, VarTypeInfo>,
    ) -> Option<String> {
        // Try to infer the type from the expression
        match node.kind() {
            "pointer_expression" => {
                // Address-of operator: &variable
                if let Some(argument) = node.child_by_field_name("argument") {
                    let arg_text = get_node_text(&argument, source).trim().to_string();
                    // Check if we have type info for this variable
                    if let Some(var_info) = var_types.get(&arg_text) {
                        return Some(var_info.base_type.clone());
                    }
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
                // Check if we have type info for this variable
                if let Some(var_info) = var_types.get(&name) {
                    return Some(var_info.base_type.clone());
                }
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

        // Different integer types with different sizes are incompatible
        // This catches short* -> int*, int* -> short*, etc.
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
            ("short", "int"),
            ("int", "short"),
            ("short", "long"),
            ("long", "short"),
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

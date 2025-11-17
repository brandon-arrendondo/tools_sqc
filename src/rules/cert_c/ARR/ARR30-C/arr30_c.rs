//! ARR30-C: Do not form or use out-of-bounds pointers or array subscripts
//!
//! This rule checker detects various patterns of out-of-bounds array access including:
//! - Static array bounds violations
//! - Dynamic allocation bounds violations
//! - Pointer arithmetic beyond buffer bounds
//! - Variable Length Array (VLA) violations
//! - Function parameter array access without bounds checking
//! - Recursive function array access
//! - Dangerous library function usage (strcpy, sprintf, gets, etc.)
//!
//! # Known Limitations
//!
//! ## Macro Expansion
//! This implementation partially supports C preprocessor macros:
//!
//! **Supported:**
//! - Macro constants in array size declarations are resolved:
//!   ```c
//!   #define SIZE 10
//!   int arr[SIZE];  // SIZE is resolved to 10
//!   ```
//!
//! **NOT Supported:**
//! - Function-like macros that generate array accesses are NOT expanded.
//!   These appear as function calls to the parser, not as array subscripts.
//!
//! Example that will NOT be detected:
//! ```c
//! #define UNSAFE_ACCESS(arr, idx) arr[idx + 5]
//! int data[8];
//! UNSAFE_ACCESS(data, 6);  // Parser sees a function call, not data[11]
//! ```
//!
//! Proper detection would require:
//! - Running the C preprocessor (cpp or clang -E) before parsing
//! - Mapping violations back to original source locations via #line directives
//!
//! This is a complex architectural change that may be added in future versions.

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use std::collections::HashMap;
use tree_sitter::Node;

// Import shared utility functions
use crate::utility::cert_c::ast_utils::{
    find_containing_for_loop, find_containing_function, find_containing_if_statement,
    find_identifier_in_declarator, is_function_parameter,
};

pub struct Arr30C;

/// Information about a buffer (array or dynamically allocated memory)
#[derive(Debug, Clone)]
struct BufferInfo {
    name: String,
    size: BufferSize,
    element_type: String,
    allocation_line: usize,
}

/// Represents the size of a buffer
#[derive(Debug, Clone)]
enum BufferSize {
    Static(usize),            // char arr[10]
    DynamicCalculated(usize), // malloc(10 * sizeof(int))
    Dynamic(String),          // malloc(size) - variable expression
    Symbolic(String),         // VLA: int arr[n] - symbolic size
    Unknown,
}

/// Represents an index value that can be constant or variable
#[derive(Debug)]
enum IndexValue {
    Constant(isize),                   // Changed from usize to support negative indices
    Expression(String, Option<isize>), // Expression text and evaluated constant if possible
    Variable(String),
    Unknown,
}

/// Represents a pointer arithmetic offset
#[derive(Debug)]
enum OffsetValue {
    Constant(usize),
    Variable(String),
    Unknown,
}

/// Represents a pointer alias mapping
#[derive(Debug, Clone)]
struct PointerAlias {
    alias_name: String,      // The pointer variable name (e.g., "ptr", "int_array")
    original_buffer: String, // The original buffer name (e.g., "arr", "buffer")
    element_size_bytes: Option<usize>, // Element size for cast pointers (e.g., 4 for int, 1 for char)
}

/// Represents a function-like macro that might involve array access
#[derive(Debug, Clone)]
struct FunctionMacro {
    name: String,
    params: Vec<String>,
    body: String,
    line: usize,
}

impl CertRule for Arr30C {
    fn rule_id(&self) -> &'static str {
        "ARR30-C"
    }

    fn description(&self) -> &'static str {
        "Do not form or use out-of-bounds pointers or array subscripts"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "ARR30-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        // Analyze all buffer allocations once at root level
        if node.parent().is_none() {
            let buffer_info = self.analyze_buffer_allocations(source);
            let pointer_aliases = self.analyze_pointer_aliases(source, &buffer_info);
            let function_macros = self.extract_function_macros(node, source);
            self.check_with_buffer_info(
                node,
                source,
                &buffer_info,
                &pointer_aliases,
                &function_macros,
            )
        } else {
            // This shouldn't happen as we control recursion, but handle gracefully
            Vec::new()
        }
    }
}

impl Arr30C {
    /// Analyze all buffer allocations in the source code using AST traversal
    fn analyze_buffer_allocations(&self, source: &str) -> HashMap<String, BufferInfo> {
        let mut buffers = HashMap::new();

        // Parse the source code into AST
        let mut parser = tree_sitter::Parser::new();
        parser
            .set_language(&tree_sitter_c::language())
            .expect("Error loading C grammar");

        let tree = match parser.parse(source, None) {
            Some(t) => t,
            None => return buffers,
        };

        let root_node = tree.root_node();

        // First pass: collect macro constants (#define NAME VALUE)
        let macros = self.extract_macro_constants(&root_node, source);

        // Second pass: collect typedef information (still needed for typedef arrays)
        let typedefs = self.analyze_typedefs(source);

        // Analyze struct member arrays declared with typedefs
        self.analyze_struct_typedef_members(source, &typedefs, &mut buffers);

        // Traverse AST to find all declarations
        self.extract_buffers_from_ast(&root_node, source, &mut buffers, &typedefs, &macros);

        buffers
    }

    /// Extract macro constants from preprocessor directives
    /// Returns a HashMap of macro name to its integer value
    fn extract_macro_constants(&self, root: &Node, source: &str) -> HashMap<String, i64> {
        let mut macros = HashMap::new();

        let mut cursor = root.walk();

        for child in root.children(&mut cursor) {
            if child.kind() == "preproc_def" {
                // #define NAME VALUE
                if let Some(name_node) = child.child_by_field_name("name") {
                    if let Some(value_node) = child.child_by_field_name("value") {
                        let name = &source[name_node.start_byte()..name_node.end_byte()];
                        let value_str = &source[value_node.start_byte()..value_node.end_byte()];

                        // Try to parse as integer
                        if let Ok(value) = value_str.trim().parse::<i64>() {
                            macros.insert(name.to_string(), value);
                        }
                    }
                }
            }
        }

        macros
    }

    /// Extract function-like macros from preprocessor directives
    /// Returns a HashMap of macro name to FunctionMacro info
    fn extract_function_macros(&self, root: &Node, source: &str) -> HashMap<String, FunctionMacro> {
        let mut macros = HashMap::new();

        let mut cursor = root.walk();

        for child in root.children(&mut cursor) {
            if child.kind() == "preproc_function_def" {
                // #define NAME(params) body
                if let Some(name_node) = child.child_by_field_name("name") {
                    let name = &source[name_node.start_byte()..name_node.end_byte()];

                    // Extract parameters
                    let mut params = Vec::new();
                    if let Some(params_node) = child.child_by_field_name("parameters") {
                        let mut param_cursor = params_node.walk();
                        for param_child in params_node.children(&mut param_cursor) {
                            if param_child.kind() == "identifier" {
                                let param_name =
                                    &source[param_child.start_byte()..param_child.end_byte()];
                                params.push(param_name.to_string());
                            }
                        }
                    }

                    // Extract body
                    let body = if let Some(value_node) = child.child_by_field_name("value") {
                        source[value_node.start_byte()..value_node.end_byte()].to_string()
                    } else {
                        String::new()
                    };

                    let line = child.start_position().row + 1;

                    macros.insert(
                        name.to_string(),
                        FunctionMacro {
                            name: name.to_string(),
                            params,
                            body,
                            line,
                        },
                    );
                }
            }
        }

        macros
    }

    /// Recursively extract buffer allocations from AST
    fn extract_buffers_from_ast(
        &self,
        node: &Node,
        source: &str,
        buffers: &mut HashMap<String, BufferInfo>,
        typedefs: &HashMap<String, usize>,
        macros: &HashMap<String, i64>,
    ) {
        // Check if this node is a declaration
        if node.kind() == "declaration" {
            if let Some(mut buffer) =
                self.extract_buffer_from_declaration_with_typedefs(node, source, typedefs)
            {
                // Try to resolve macro constants in buffer size
                if let BufferSize::Symbolic(ref sym) = buffer.size {
                    if let Some(&value) = macros.get(sym) {
                        buffer.size = BufferSize::Static(value as usize);
                    }
                }

                // Handle realloc: keep existing buffer if it has smaller size
                if let Some(existing) = buffers.get(&buffer.name) {
                    match (&existing.size, &buffer.size) {
                        (
                            BufferSize::DynamicCalculated(old_size),
                            BufferSize::DynamicCalculated(new_size),
                        ) => {
                            if new_size < old_size {
                                buffers.insert(buffer.name.clone(), buffer.clone());
                            }
                        }
                        _ => {
                            buffers.insert(buffer.name.clone(), buffer.clone());
                        }
                    }
                } else {
                    buffers.insert(buffer.name.clone(), buffer.clone());
                }

                // For multidimensional arrays, extract inner dimensions
                self.extract_multidimensional_buffers(node, &buffer.name, source, buffers);
            }

            // Also check for VLA declarations using typedef
            // VLAs need special handling as they may not be caught by AST alone
            if let Some(mut vla_buffer) = self.extract_vla_from_declaration(node, source, typedefs)
            {
                // Try to resolve macro constants in VLA buffer size
                if let BufferSize::Symbolic(ref sym) = vla_buffer.size {
                    if let Some(&value) = macros.get(sym) {
                        vla_buffer.size = BufferSize::Static(value as usize);
                    }
                }

                // Only insert if not already in map (prefer the already-resolved version)
                if !buffers.contains_key(&vla_buffer.name) {
                    buffers.insert(vla_buffer.name.clone(), vla_buffer);
                }
            }
        }

        // Check if this node is a struct_specifier or union_specifier to extract member arrays
        if node.kind() == "struct_specifier" || node.kind() == "union_specifier" {
            self.extract_struct_member_arrays(node, source, buffers);
        }

        // Check for assignment expressions with malloc (e.g., matrix[i] = malloc(...))
        // This handles dynamic allocations inside loops
        if node.kind() == "assignment_expression" || node.kind() == "expression_statement" {
            let assign_node = if node.kind() == "assignment_expression" {
                Some(*node)
            } else if node.kind() == "expression_statement" {
                // Look for assignment_expression child
                node.child(0)
                    .filter(|c| c.kind() == "assignment_expression")
            } else {
                None
            };

            if let Some(assign) = assign_node {
                if let Some((buf_name, buf_info)) =
                    self.extract_buffer_from_assignment(&assign, source)
                {
                    // Insert wildcard buffers from malloc assignments
                    buffers.insert(buf_name, buf_info);
                }
            }
        }

        // Recursively process children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.extract_buffers_from_ast(&child, source, buffers, typedefs, macros);
            }
        }
    }

    /// Extract member arrays from struct_specifier or union_specifier node
    /// Handles patterns like:
    /// typedef struct {
    ///     char name[10];    // Extracts "name" with size 10
    ///     int scores[5];    // Extracts "scores" with size 5
    /// } Student;
    /// typedef union {
    ///     char bytes[4];    // Extracts "bytes" with size 4
    ///     int value;
    /// } Data;
    fn extract_struct_member_arrays(
        &self,
        node: &Node,
        source: &str,
        buffers: &mut HashMap<String, BufferInfo>,
    ) {
        // Find the field_declaration_list child node
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "field_declaration_list" {
                    // Process each field_declaration within the list
                    for j in 0..child.child_count() {
                        if let Some(field) = child.child(j) {
                            if field.kind() == "field_declaration" {
                                // Extract array member from field_declaration
                                if let Some(member_info) =
                                    self.extract_array_from_field_declaration(&field, source)
                                {
                                    buffers.insert(member_info.name.clone(), member_info);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /// Extract array information from a field_declaration node
    /// Handles patterns like: char name[10]; or int scores[5];
    fn extract_array_from_field_declaration(
        &self,
        node: &Node,
        source: &str,
    ) -> Option<BufferInfo> {
        // Look for array_declarator within the field_declaration
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "array_declarator" {
                    // Extract member name and size from array_declarator
                    let mut member_name: Option<String> = None;
                    let mut array_size: Option<usize> = None;

                    for j in 0..child.child_count() {
                        if let Some(declarator_child) = child.child(j) {
                            match declarator_child.kind() {
                                "field_identifier" => {
                                    // Struct member names use field_identifier
                                    member_name = Some(
                                        source[declarator_child.start_byte()
                                            ..declarator_child.end_byte()]
                                            .to_string(),
                                    );
                                }
                                "identifier" if j == 0 => {
                                    // Could also be a regular identifier in some cases
                                    member_name = Some(
                                        source[declarator_child.start_byte()
                                            ..declarator_child.end_byte()]
                                            .to_string(),
                                    );
                                }
                                "number_literal" => {
                                    // Array size
                                    let size_str = &source[declarator_child.start_byte()
                                        ..declarator_child.end_byte()];
                                    array_size = size_str.parse().ok();
                                }
                                _ => {}
                            }
                        }
                    }

                    // If we found both name and size, create BufferInfo
                    if let (Some(name), Some(size)) = (member_name, array_size) {
                        return Some(BufferInfo {
                            name,
                            size: BufferSize::Static(size),
                            element_type: "struct_member".to_string(),
                            allocation_line: node.start_position().row + 1,
                        });
                    }
                }
            }
        }
        None
    }

    /// Extract VLA (Variable Length Array) from declaration node
    fn extract_vla_from_declaration(
        &self,
        node: &Node,
        source: &str,
        _typedefs: &HashMap<String, usize>,
    ) -> Option<BufferInfo> {
        // Look for array_declarator with identifier size (not number_literal)
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "init_declarator" {
                    // Check first child for array_declarator
                    if let Some(declarator) = child.child(0) {
                        if declarator.kind() == "array_declarator" {
                            return self.extract_vla_from_array_declarator(&declarator, source);
                        }
                    }
                } else if child.kind() == "array_declarator" {
                    return self.extract_vla_from_array_declarator(&child, source);
                }
            }
        }
        None
    }

    /// Extract VLA from array_declarator if size is symbolic
    fn extract_vla_from_array_declarator(&self, node: &Node, source: &str) -> Option<BufferInfo> {
        let mut var_name: Option<String> = None;
        let mut size_expr: Option<String> = None;

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "identifier" if i == 0 => {
                        var_name = Some(source[child.start_byte()..child.end_byte()].to_string());
                    }
                    "identifier" if i > 0 => {
                        // This is a symbolic size (VLA)
                        let expr = &source[child.start_byte()..child.end_byte()];
                        // Verify it's not a number
                        if !expr.chars().all(|c| c.is_numeric()) {
                            size_expr = Some(expr.to_string());
                        }
                    }
                    "number_literal" => {
                        // This is a static size, not a VLA
                        return None;
                    }
                    _ => {}
                }
            }
        }

        if let (Some(name), Some(expr)) = (var_name, size_expr) {
            Some(BufferInfo {
                name,
                size: BufferSize::Symbolic(expr),
                element_type: "unknown".to_string(),
                allocation_line: node.start_position().row + 1,
            })
        } else {
            None
        }
    }

    /// Analyze pointer aliases in the source code using AST traversal
    fn analyze_pointer_aliases(
        &self,
        source: &str,
        buffers: &HashMap<String, BufferInfo>,
    ) -> HashMap<String, PointerAlias> {
        let mut aliases = HashMap::new();

        // Parse the source code into AST
        let mut parser = tree_sitter::Parser::new();
        parser
            .set_language(&tree_sitter_c::language())
            .expect("Error loading C grammar");

        let tree = match parser.parse(source, None) {
            Some(t) => t,
            None => return aliases,
        };

        let root_node = tree.root_node();

        // Traverse AST to find all pointer alias declarations
        self.extract_aliases_from_ast(&root_node, source, buffers, &mut aliases);

        aliases
    }

    /// Recursively extract pointer aliases from AST
    fn extract_aliases_from_ast(
        &self,
        node: &Node,
        source: &str,
        buffers: &HashMap<String, BufferInfo>,
        aliases: &mut HashMap<String, PointerAlias>,
    ) {
        // Check if this node is a declaration
        if node.kind() == "declaration" {
            if let Some(alias) = self.extract_alias_from_declaration(node, source, buffers) {
                aliases.insert(alias.alias_name.clone(), alias);
            }
        }

        // Recursively process children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.extract_aliases_from_ast(&child, source, buffers, aliases);
            }
        }
    }

    /// Analyze typedef declarations for array types
    fn analyze_typedefs(&self, source: &str) -> HashMap<String, usize> {
        let mut typedefs = HashMap::new();

        // Pattern: typedef type TypeName[SIZE];
        let typedef_pattern = r"typedef\s+(?:\w+\s+)*\w+\s+(\w+)\s*\[\s*(\d+)\s*\]";

        if let Ok(re) = regex::Regex::new(typedef_pattern) {
            for caps in re.captures_iter(source) {
                if let (Some(typedef_name), Some(size_str)) = (caps.get(1), caps.get(2)) {
                    if let Ok(size) = size_str.as_str().parse::<usize>() {
                        typedefs.insert(typedef_name.as_str().to_string(), size);
                    }
                }
            }
        }

        typedefs
    }

    /// Analyze struct/union members that use typedef array types
    /// This handles cases like: struct { IntArray numbers; }
    fn analyze_struct_typedef_members(
        &self,
        source: &str,
        typedefs: &HashMap<String, usize>,
        buffers: &mut HashMap<String, BufferInfo>,
    ) {
        let lines: Vec<&str> = source.lines().collect();

        for (line_idx, line) in lines.iter().enumerate() {
            let trimmed = line.trim();

            // Pattern: TypedefName member_name;
            // Look for lines that match typedef usage inside structs/unions
            if let Ok(re) = regex::Regex::new(r"^\s*(\w+)\s+(\w+)\s*;") {
                if let Some(caps) = re.captures(trimmed) {
                    if let (Some(type_match), Some(member_match)) = (caps.get(1), caps.get(2)) {
                        let type_name = type_match.as_str();
                        let member_name = member_match.as_str();

                        // Check if this is a known typedef
                        if let Some(&size) = typedefs.get(type_name) {
                            // Add as a tracked buffer using the member name
                            buffers.insert(
                                member_name.to_string(),
                                BufferInfo {
                                    name: member_name.to_string(),
                                    size: BufferSize::Static(size),
                                    element_type: type_name.to_string(),
                                    allocation_line: line_idx + 1,
                                },
                            );
                        }
                    }
                }
            }
        }
    }

    /// Extract numeric value from string
    fn extract_numeric_value(&self, s: &str) -> Option<usize> {
        s.trim().parse().ok()
    }

    /// Calculate size from malloc arguments
    fn calculate_malloc_size(&self, malloc_args: &str) -> Option<BufferSize> {
        let trimmed = malloc_args.trim();

        // Pattern 1: Simple number - malloc(100)
        if let Some(size) = self.extract_numeric_value(trimmed) {
            return Some(BufferSize::DynamicCalculated(size));
        }

        // Pattern 2: COUNT * sizeof(TYPE) - malloc(5 * sizeof(int))
        // Store the COUNT (number of elements), not the byte size
        if trimmed.contains('*') && trimmed.contains("sizeof") {
            // Split only on the first '*' to handle cases like "3 * sizeof(int*)"
            if let Some(mult_pos) = trimmed.find('*') {
                let count_str = &trimmed[..mult_pos].trim();
                let sizeof_str = &trimmed[mult_pos + 1..].trim();

                let count = self.extract_numeric_value(count_str);
                let _sizeof_val = self.extract_sizeof_value(sizeof_str);

                if let Some(c) = count {
                    // Store element count, not byte count
                    return Some(BufferSize::DynamicCalculated(c));
                }
            }
        }

        // Pattern 3: Just sizeof(TYPE) - malloc(sizeof(struct foo))
        if let Some(sizeof_val) = self.extract_sizeof_value(trimmed) {
            return Some(BufferSize::DynamicCalculated(sizeof_val));
        }

        // Pattern 4: Variable expression
        Some(BufferSize::Dynamic(trimmed.to_string()))
    }

    /// Extract size from sizeof expression - using common type sizes
    fn extract_sizeof_value(&self, s: &str) -> Option<usize> {
        if !s.contains("sizeof") {
            return None;
        }

        // Common type sizes (assuming typical 64-bit system)
        let type_sizes = [
            ("int", 4),
            ("char", 1),
            ("short", 2),
            ("long", 8),
            ("float", 4),
            ("double", 8),
            ("void*", 8),
            ("int*", 8),
            ("char*", 8),
        ];

        for (type_name, size) in &type_sizes {
            if s.contains(type_name) {
                return Some(*size);
            }
        }

        // Default to pointer size if we can't determine
        Some(8)
    }

    /// Get array name from subscript expression node
    fn get_array_name_from_subscript(&self, node: &Node, source: &str) -> Option<String> {
        let array_node = node.child(0)?;

        // If the child is itself a subscript_expression, we need the full text
        // For nested subscripts like matrix[0][5], this will return "matrix[0]"
        if array_node.kind() == "subscript_expression" {
            let text = &source[array_node.start_byte()..array_node.end_byte()];
            return Some(text.to_string());
        }

        let text = &source[array_node.start_byte()..array_node.end_byte()];

        // Check if this is member access (contains '.' or '->')
        if text.contains('.') {
            // Extract the member name after the last '.'
            if let Some(member) = text.split('.').last() {
                return Some(member.to_string());
            }
        }

        if text.contains("->") {
            // Extract the member name after the last '->'
            if let Some(member) = text.split("->").last() {
                return Some(member.to_string());
            }
        }

        // Handle regular cases like arr[i], ptr[j]
        // Extract the base identifier
        let identifier = text
            .split(|c: char| !c.is_alphanumeric() && c != '_')
            .find(|s| !s.is_empty())?;

        Some(identifier.to_string())
    }

    /// Get the subscript index value (constant or variable)
    fn get_subscript_index_value(&self, node: &Node, source: &str) -> Option<IndexValue> {
        let index_node = self.get_subscript_index(node)?;
        let index_text = &source[index_node.start_byte()..index_node.end_byte()];

        // Try to parse as simple constant (now supports negative indices)
        if let Ok(const_val) = index_text.trim().parse::<isize>() {
            return Some(IndexValue::Constant(const_val));
        }

        // Try to evaluate as expression
        if let Some(eval_val) = self.evaluate_index_expression(index_text, source) {
            return Some(IndexValue::Expression(
                index_text.to_string(),
                Some(eval_val),
            ));
        }

        // Check if it's an arithmetic expression with variable
        if self.is_arithmetic_expression(index_text) {
            return Some(IndexValue::Expression(index_text.to_string(), None));
        }

        // Try to resolve variable to a constant value via simple constant propagation
        if let Some(const_val) = self.try_resolve_variable_to_constant(index_text, node, source) {
            return Some(IndexValue::Constant(const_val));
        }

        // It's a simple variable
        Some(IndexValue::Variable(index_text.to_string()))
    }

    /// Evaluate compile-time constant index expressions
    fn evaluate_index_expression(&self, expr: &str, source: &str) -> Option<isize> {
        let expr = expr.trim();

        // Pattern 1: sizeof(var) - N
        if expr.contains("sizeof") && expr.contains('-') {
            return self.evaluate_sizeof_expression(expr, source);
        }

        // Pattern 2: Simple arithmetic with constants (e.g., "10 - 1")
        if let Some(result) = self.evaluate_simple_arithmetic(expr) {
            return Some(result);
        }

        None
    }

    /// Evaluate sizeof expressions like "sizeof(buffer) - 1"
    fn evaluate_sizeof_expression(&self, expr: &str, source: &str) -> Option<isize> {
        // Extract sizeof target and arithmetic operation
        let re = regex::Regex::new(r"sizeof\s*\(\s*(\w+)\s*\)\s*(-|\+)\s*(\d+)").ok()?;
        let caps = re.captures(expr)?;

        let var_name = caps.get(1)?.as_str();
        let op = caps.get(2)?.as_str();
        let operand: usize = caps.get(3)?.as_str().parse().ok()?;

        // Find the buffer size by searching for declaration in source
        if let Some(size) = self.find_array_size_in_source(var_name, source) {
            match op {
                "-" => {
                    if operand <= size {
                        return Some((size - operand) as isize);
                    }
                }
                "+" => return Some((size + operand) as isize),
                _ => {}
            }
        }

        None
    }

    /// Find array size from source code for a given variable name
    fn find_array_size_in_source(&self, var_name: &str, source: &str) -> Option<usize> {
        // Look for declarations like: type var_name[SIZE];
        let pattern = format!(r"\b{}\s*\[\s*(\d+)\s*\]", regex::escape(var_name));
        let re = regex::Regex::new(&pattern).ok()?;

        if let Some(caps) = re.captures(source) {
            return caps.get(1)?.as_str().parse().ok();
        }

        None
    }

    /// Evaluate simple arithmetic expressions with only constants
    fn evaluate_simple_arithmetic(&self, expr: &str) -> Option<isize> {
        let expr = expr.trim();

        // Handle "A - B"
        if expr.contains('-') && !expr.starts_with('-') {
            let parts: Vec<&str> = expr.split('-').collect();
            if parts.len() == 2 {
                let a: isize = parts[0].trim().parse().ok()?;
                let b: isize = parts[1].trim().parse().ok()?;
                return Some(a - b);
            }
        }

        // Handle "A + B"
        if expr.contains('+') {
            let parts: Vec<&str> = expr.split('+').collect();
            if parts.len() == 2 {
                let a: isize = parts[0].trim().parse().ok()?;
                let b: isize = parts[1].trim().parse().ok()?;
                return Some(a + b);
            }
        }

        None
    }

    /// Check if expression contains arithmetic operators
    fn is_arithmetic_expression(&self, expr: &str) -> bool {
        expr.contains('+') || expr.contains('-') || expr.contains('*') || expr.contains('/')
    }

    /// Attempt to resolve a variable to a constant through simple intraprocedural constant propagation
    fn try_resolve_variable_to_constant(
        &self,
        var_name: &str,
        current_node: &Node,
        source: &str,
    ) -> Option<isize> {
        // Check if this variable is a loop counter - if so, don't resolve to constant
        // Loop counters change value during execution
        if let Some(for_node) = find_containing_for_loop(current_node) {
            if let Some(loop_var) = self.extract_loop_index_variable(&for_node, source) {
                if loop_var == var_name {
                    // This is a loop counter - don't resolve to its initial value
                    return None;
                }
            }
        }

        // Find enclosing function
        let func_node = find_containing_function(current_node)?;

        // Search for assignments to var_name within this function
        // Look for pattern: var_name = constant_literal
        let func_text = &source[func_node.start_byte()..func_node.end_byte()];

        // Regex pattern: var_name = digit+ OR var_name = -digit+
        let pattern = format!(r"\b{}\s*=\s*(-?\d+)", regex::escape(var_name));
        let re = regex::Regex::new(&pattern).ok()?;

        if let Some(caps) = re.captures(func_text) {
            if let Some(value_str) = caps.get(1) {
                return value_str.as_str().parse::<isize>().ok();
            }
        }

        None
    }

    // Removed: find_enclosing_function - now using ast_utils::find_containing_function

    // Removed: is_function_parameter - now using ast_utils::is_function_parameter with find_containing_function

    /// Check if function has ANY bounds validation for a parameter
    fn has_function_parameter_bounds_check(
        &self,
        func_node: &Node,
        param_name: &str,
        source: &str,
    ) -> bool {
        let func_text = &source[func_node.start_byte()..func_node.end_byte()];

        // Check for various bounds checking patterns:
        // 1. if (param < size) or if (param >= size) return
        // 2. Loop with param in condition: for (i = 0; i < size; i++)
        // 3. Presence of size/length/count parameter

        let bounds_patterns = [
            format!(r"{}\s*<\s*\w+", regex::escape(param_name)), // param < size
            format!(r"\w+\s*>\s*{}", regex::escape(param_name)), // size > param
            format!(r"{}\s*>=\s*\w+", regex::escape(param_name)), // param >= size (with return/check)
            format!(r"if\s*\([^)]*{}", regex::escape(param_name)), // if statement with param
        ];

        for pattern in &bounds_patterns {
            if let Ok(re) = regex::Regex::new(pattern) {
                if re.is_match(func_text) {
                    return true;
                }
            }
        }

        false
    }

    /// Check if array access is within a recursive function
    fn is_recursive_array_access(&self, subscript_node: &Node, source: &str) -> bool {
        // Find enclosing function
        if let Some(func_node) = find_containing_function(subscript_node) {
            // Get function name
            for i in 0..func_node.child_count() {
                if let Some(child) = func_node.child(i) {
                    if child.kind() == "function_declarator" {
                        // Get function name (first child of function_declarator)
                        if let Some(name_node) = child.child(0) {
                            let func_name = &source[name_node.start_byte()..name_node.end_byte()];

                            // Search function body for calls to itself
                            let func_text = &source[func_node.start_byte()..func_node.end_byte()];

                            // Look for function calls in the body (skip the declaration part)
                            // Pattern: function_name(
                            let call_pattern = format!(r"{}\s*\(", regex::escape(func_name));
                            if let Ok(re) = regex::Regex::new(&call_pattern) {
                                // Count matches - if more than 1, it's recursive (declaration + call)
                                let matches: Vec<_> = re.find_iter(func_text).collect();
                                return matches.len() > 1;
                            }
                        }
                    }
                }
            }
        }
        false
    }

    /// Check if a recursive function has dangerous index modification patterns
    /// Returns true if recursion modifies indices in a way that will exceed bounds
    fn has_recursive_index_modification(
        &self,
        subscript_node: &Node,
        index_text: &str,
        source: &str,
        array_size: usize,
    ) -> bool {
        if !self.is_recursive_array_access(subscript_node, source) {
            return false;
        }

        if let Some(func_node) = find_containing_function(subscript_node) {
            let func_text = &source[func_node.start_byte()..func_node.end_byte()];

            // Get function name for recursive call pattern
            let func_name = match self.get_function_name(&func_node, source) {
                Some(name) => name,
                None => return false,
            };

            // Look for recursive calls with index modifications like: func(arr, index + 2, ...)
            // Pattern: function_name(.*index \+ \d+
            let modification_pattern = format!(
                r"{}\s*\([^)]*{}\s*\+\s*(\d+)",
                regex::escape(&func_name),
                regex::escape(index_text)
            );
            if let Ok(re) = regex::Regex::new(&modification_pattern) {
                if let Some(caps) = re.captures(func_text) {
                    if let Some(increment) = caps.get(1) {
                        if let Ok(inc_val) = increment.as_str().parse::<usize>() {
                            // Check if there's a depth limit
                            // Look for patterns like: if (depth > N) return
                            let depth_pattern = r"if\s*\(\s*\w+\s*>\s*(\d+)\s*\)";
                            if let Ok(depth_re) = regex::Regex::new(depth_pattern) {
                                if let Some(depth_caps) = depth_re.captures(func_text) {
                                    if let Some(max_depth) = depth_caps.get(1) {
                                        if let Ok(max_d) = max_depth.as_str().parse::<usize>() {
                                            // Calculate maximum index: inc_val * max_d
                                            // If this exceeds array_size, it's a violation
                                            return inc_val * max_d >= array_size;
                                        }
                                    }
                                }
                            }
                            // No depth limit found, or couldn't parse - flag as dangerous
                            return true;
                        }
                    }
                }
            }
        }

        false
    }

    /// Get function name from function_definition node
    fn get_function_name(&self, func_node: &Node, source: &str) -> Option<String> {
        for i in 0..func_node.child_count() {
            if let Some(child) = func_node.child(i) {
                if child.kind() == "function_declarator" {
                    if let Some(name_node) = child.child(0) {
                        return Some(
                            source[name_node.start_byte()..name_node.end_byte()].to_string(),
                        );
                    }
                }
            }
        }
        None
    }

    /// Enhanced bounds check that considers actual buffer size
    fn has_proper_bounds_check(&self, node: &Node, source: &str, buffer_size: usize) -> bool {
        // Check loop-based bounds checking
        if let Some(for_node) = find_containing_for_loop(node) {
            if self.check_for_loop_bounds_against_size(&for_node, source, buffer_size) {
                return true;
            }
        }

        // Check conditional bounds checking
        if let Some(if_node) = find_containing_if_statement(node) {
            if self.check_if_bounds_against_size(&if_node, source, buffer_size) {
                return true;
            }
        }

        false
    }

    /// Check if there's any form of dynamic bounds checking
    fn has_dynamic_bounds_check(&self, node: &Node, source: &str) -> bool {
        // Check for loop-based bounds checking
        if let Some(for_node) = find_containing_for_loop(node) {
            // Use empty string for index to do generic check
            if self.check_for_loop_bounds_generic(&for_node, source) {
                return true;
            }
        }

        // Check for conditional bounds checking
        if let Some(if_node) = find_containing_if_statement(node) {
            if self.check_if_bounds_generic(&if_node, source) {
                return true;
            }
        }

        // Check for function-level bounds checking (parameter validation)
        if let Some(func_node) = find_containing_function(node) {
            let function_text = &source[func_node.start_byte()..func_node.end_byte()];
            if function_text.contains("size")
                || function_text.contains("length")
                || function_text.contains("count")
            {
                return true;
            }
        }

        false
    }

    /// Find containing for loop
    // Removed: find_containing_for_loop - now using ast_utils::find_containing_for_loop
    // Removed: find_containing_if_statement - now using ast_utils::find_containing_if_statement

    /// Check for loop bounds against specific buffer size
    fn check_for_loop_bounds_against_size(
        &self,
        for_node: &Node,
        source: &str,
        size: usize,
    ) -> bool {
        let loop_text = &source[for_node.start_byte()..for_node.end_byte()];

        // Look for patterns like: i < SIZE or i < 10
        if loop_text.contains(&format!("< {}", size)) {
            return true;
        }

        // Extract the loop index variable name
        let index_var = self.extract_loop_index_variable(for_node, source);
        let index_text = index_var.as_deref().unwrap_or("");

        // Check loop condition for safe bounds
        for i in 0..for_node.child_count() {
            if let Some(child) = for_node.child(i) {
                if child.kind() == "binary_expression" || child.kind() == "comparison_expression" {
                    let condition_text = &source[child.start_byte()..child.end_byte()];
                    if self.condition_contains_safe_bounds(condition_text, index_text) {
                        return true;
                    }
                }
            }
        }

        // Also check inside parenthesized expressions
        for i in 0..for_node.child_count() {
            if let Some(child) = for_node.child(i) {
                if child.kind() == "parenthesized_expression" {
                    for j in 0..child.child_count() {
                        if let Some(grandchild) = child.child(j) {
                            if grandchild.kind() == "binary_expression"
                                || grandchild.kind() == "comparison_expression"
                            {
                                let condition_text =
                                    &source[grandchild.start_byte()..grandchild.end_byte()];
                                if self.condition_contains_safe_bounds(condition_text, index_text) {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }

        false
    }

    /// Check if statement bounds against specific buffer size
    fn check_if_bounds_against_size(&self, if_node: &Node, source: &str, size: usize) -> bool {
        let if_text = &source[if_node.start_byte()..if_node.end_byte()];

        // Look for patterns like: if (idx < SIZE) or if (idx < 3)
        if if_text.contains(&format!("< {}", size)) {
            return true;
        }

        // Also check for macro-based bounds (e.g., "< ROWS", "< COLS")
        // Look for common comparison patterns that indicate bounds checking
        if if_text.contains("< ") && (if_text.contains(">=") || if_text.contains("&&")) {
            // Pattern like: if (idx >= 0 && idx < SOMETHING)
            // This is a proper bounds check even if we don't know the exact value of SOMETHING
            return true;
        }

        false
    }

    /// Extract the index variable name from a for loop
    /// For loops like `for (int i = 0; i < 10; i++)`, extracts "i"
    fn extract_loop_index_variable(&self, for_node: &Node, source: &str) -> Option<String> {
        // Look for the loop initialization to find the index variable
        for i in 0..for_node.child_count() {
            if let Some(child) = for_node.child(i) {
                // Look for declaration or assignment in loop init
                if child.kind() == "declaration" {
                    // Pattern: int i = 0
                    for j in 0..child.child_count() {
                        if let Some(declarator) = child.child(j) {
                            if declarator.kind() == "init_declarator" {
                                if let Some(identifier) = declarator.child(0) {
                                    if identifier.kind() == "identifier" {
                                        return Some(
                                            source[identifier.start_byte()..identifier.end_byte()]
                                                .to_string(),
                                        );
                                    }
                                }
                            }
                        }
                    }
                } else if child.kind() == "assignment_expression" {
                    // Pattern: i = 0
                    if let Some(left) = child.child(0) {
                        if left.kind() == "identifier" {
                            return Some(source[left.start_byte()..left.end_byte()].to_string());
                        }
                    }
                }
            }
        }
        None
    }

    /// Extract buffer allocation from assignment expression
    /// Handles patterns like:
    /// - array[i] = malloc(size * sizeof(type))
    /// - ptr = realloc(ptr, new_size)
    /// - ptr = malloc(size)
    fn extract_buffer_from_assignment(
        &self,
        node: &Node,
        source: &str,
    ) -> Option<(String, BufferInfo)> {
        // Check if this is an assignment with malloc/calloc/realloc on the right side
        let mut left_node: Option<Node> = None;
        let mut right_node: Option<Node> = None;
        let mut found_assign = false;

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "=" {
                    found_assign = true;
                } else if !found_assign {
                    left_node = Some(child);
                } else if child.kind() == "call_expression" {
                    right_node = Some(child);
                    break;
                }
            }
        }

        if !found_assign {
            return None;
        }

        let left = left_node?;
        let right = right_node?;

        // Check if right side is malloc/calloc/realloc
        let func_name_node = right.child(0)?;
        let func_name = &source[func_name_node.start_byte()..func_name_node.end_byte()];

        if func_name != "malloc" && func_name != "calloc" && func_name != "realloc" {
            return None;
        }

        // Handle subscript expressions (e.g., matrix[i])
        if left.kind() == "subscript_expression" {
            // Extract the base array name from subscript
            let base_array = self.get_base_array_from_subscript(&left, source)?;

            // Extract allocation size from malloc/calloc/realloc
            let buffer_size = self.extract_malloc_size_from_call(&right, source)?;

            // Create a wildcard buffer name: base_array[*]
            let buffer_name = format!("{}[*]", base_array);

            let buffer_info = BufferInfo {
                name: buffer_name.clone(),
                size: buffer_size,
                element_type: "unknown".to_string(),
                allocation_line: node.start_position().row + 1,
            };

            return Some((buffer_name, buffer_info));
        }

        // Handle simple identifier assignments (e.g., ptr = realloc(ptr, new_size))
        if left.kind() == "identifier" {
            let var_name = &source[left.start_byte()..left.end_byte()];

            // Extract allocation size from malloc/calloc/realloc
            let buffer_size = self.extract_malloc_size_from_call(&right, source)?;

            let buffer_info = BufferInfo {
                name: var_name.to_string(),
                size: buffer_size,
                element_type: "unknown".to_string(),
                allocation_line: node.start_position().row + 1,
            };

            return Some((var_name.to_string(), buffer_info));
        }

        None
    }

    /// Get base array name from subscript expression (e.g., "matrix" from "matrix[i]")
    fn get_base_array_from_subscript(&self, node: &Node, source: &str) -> Option<String> {
        let array_node = node.child(0)?;
        if array_node.kind() == "identifier" {
            let text = &source[array_node.start_byte()..array_node.end_byte()];
            return Some(text.to_string());
        }
        None
    }

    /// Get index text from subscript expression
    fn get_subscript_index_text(&self, node: &Node, source: &str) -> Option<String> {
        let index_node = self.get_subscript_index(node)?;
        let text = &source[index_node.start_byte()..index_node.end_byte()];
        Some(text.to_string())
    }

    /// Extract malloc/realloc size from call expression
    fn extract_malloc_size_from_call(&self, node: &Node, source: &str) -> Option<BufferSize> {
        // Get function name to determine which argument contains the size
        let func_name_node = node.child(0)?;
        let func_name = &source[func_name_node.start_byte()..func_name_node.end_byte()];

        // Find argument_list
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "argument_list" {
                    // For realloc, the size is the second argument
                    // For malloc/calloc, the size is in the first argument
                    let arg_index = if func_name == "realloc" { 1 } else { 0 };

                    let mut current_arg = 0;
                    for j in 0..child.child_count() {
                        if let Some(arg) = child.child(j) {
                            if arg.kind() != "(" && arg.kind() != ")" && arg.kind() != "," {
                                if current_arg == arg_index {
                                    let arg_text = &source[arg.start_byte()..arg.end_byte()];
                                    return self.calculate_malloc_size(arg_text);
                                }
                                current_arg += 1;
                            }
                        }
                    }
                }
            }
        }
        None
    }

    /// Get buffer name from a subscript expression for lookup
    /// For matrix[0], tries both "matrix[0]" and "matrix[*]"
    /// Returns the wildcard pattern for now
    fn get_subscript_buffer_name(&self, node: &Node, source: &str) -> Option<String> {
        // Extract base array name
        let base_name = self.get_base_array_from_subscript(node, source)?;

        // Return wildcard pattern for lookup
        Some(format!("{}[*]", base_name))
    }

    /// Check nested subscript expressions (multi-dimensional array access)
    /// For matrix[i][j], checks both:
    /// 1. Is i within bounds of matrix?
    /// 2. Is j within bounds of matrix[i]?
    fn check_nested_subscript(
        &self,
        node: &Node,
        source: &str,
        buffers: &HashMap<String, BufferInfo>,
        aliases: &HashMap<String, PointerAlias>,
    ) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Get the inner subscript node (matrix[i])
        if let Some(inner_node) = node.child(0) {
            if inner_node.kind() == "subscript_expression" {
                // Step 1: Check the inner subscript bounds (matrix[i])
                violations.extend(self.check_array_subscript(
                    &inner_node,
                    source,
                    buffers,
                    aliases,
                ));

                // Step 2: Get the buffer name for the inner subscript result
                // For matrix[0], this should look up "matrix[*]" in buffers
                if let Some(inner_buffer_name) = self.get_subscript_buffer_name(&inner_node, source)
                {
                    if let Some(inner_buffer) = buffers.get(&inner_buffer_name) {
                        // Step 3: Check the outer index against the inner buffer's size
                        if let Some(outer_index) = self.get_subscript_index_value(node, source) {
                            let is_violation = match &inner_buffer.size {
                                BufferSize::Static(size) | BufferSize::DynamicCalculated(size) => {
                                    match &outer_index {
                                        IndexValue::Constant(idx) => {
                                            *idx < 0 || (*idx as usize) >= *size
                                        }
                                        IndexValue::Expression(_, Some(eval_idx)) => {
                                            *eval_idx < 0 || (*eval_idx as usize) >= *size
                                        }
                                        IndexValue::Expression(expr, None) => {
                                            self.check_expression_bounds(expr, *size)
                                        }
                                        IndexValue::Variable(_var) => {
                                            // Check for bounds validation
                                            !self.has_proper_bounds_check(node, source, *size)
                                        }
                                        IndexValue::Unknown => false,
                                    }
                                }
                                _ => false,
                            };

                            if is_violation {
                                // Get the full array name for error message
                                let full_array_name =
                                    &source[inner_node.start_byte()..inner_node.end_byte()];

                                let msg = match outer_index {
                                    IndexValue::Constant(idx) => {
                                        format!("Out-of-bounds array access at index {}", idx)
                                    }
                                    IndexValue::Expression(ref expr, Some(eval_idx)) => format!(
                                        "Out-of-bounds array access: '{}' evaluates to {}",
                                        expr, eval_idx
                                    ),
                                    IndexValue::Expression(ref expr, None) => format!(
                                        "Potentially unsafe array access with expression '{}'",
                                        expr
                                    ),
                                    IndexValue::Variable(ref var) => format!(
                                        "Potentially unsafe array access with variable index '{}'",
                                        var
                                    ),
                                    IndexValue::Unknown => {
                                        "Potentially unsafe array access".to_string()
                                    }
                                };

                                violations.push(self.create_violation(
                                    node,
                                    full_array_name,
                                    inner_buffer,
                                    &msg,
                                ));
                            }
                        }
                    }
                }
            }
        }

        violations
    }

    /// Create a violation record
    fn create_violation(
        &self,
        node: &Node,
        array_name: &str,
        buffer_info: &BufferInfo,
        message: &str,
    ) -> RuleViolation {
        let start_point = node.start_position();

        let size_info = match &buffer_info.size {
            BufferSize::Static(s) => format!("size {}", s),
            BufferSize::DynamicCalculated(s) => format!("allocated size {}", s),
            BufferSize::Dynamic(expr) => format!("dynamic size ({})", expr),
            BufferSize::Symbolic(var) => format!("VLA size ({})", var),
            BufferSize::Unknown => "unknown size".to_string(),
        };

        RuleViolation {
            rule_id: self.rule_id().to_string(),
            severity: Severity::High,
            message: format!(
                "{}: Buffer '{}' with {} (allocated at line {})",
                message, array_name, size_info, buffer_info.allocation_line
            ),
            file_path: String::new(),
            line: start_point.row + 1,
            column: start_point.column + 1,
            suggestion: Some(
                "Ensure array access is within allocated bounds. Add explicit bounds checking."
                    .to_string(),
            ),
            ..Default::default()
        }
    }

    /// Check array subscript expressions with buffer size analysis
    fn check_array_subscript(
        &self,
        node: &Node,
        source: &str,
        buffers: &HashMap<String, BufferInfo>,
        aliases: &HashMap<String, PointerAlias>,
    ) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Check if this is a nested subscript expression (e.g., matrix[0][5])
        if let Some(child) = node.child(0) {
            if child.kind() == "subscript_expression" {
                // Delegate to nested subscript handler
                return self.check_nested_subscript(node, source, buffers, aliases);
            }
        }

        if let Some(array_name) = self.get_array_name_from_subscript(node, source) {
            if let Some(index) = self.get_subscript_index_value(node, source) {
                // Check for function parameter violations FIRST, even if buffer not tracked
                // This handles cases like: void func(int arr[], int index) { arr[index]; }
                if let IndexValue::Variable(ref var) = index {
                    if let Some(func_node) = find_containing_function(node) {
                        if is_function_parameter(&func_node, var, source) {
                            if !self.has_function_parameter_bounds_check(&func_node, var, source) {
                                // Create a violation for unvalidated function parameter
                                let start_point = node.start_position();
                                violations.push(RuleViolation {
                                    rule_id: self.rule_id().to_string(),
                                    severity: Severity::High,
                                    message: format!("Potentially unsafe array access with unvalidated function parameter index '{}'", var),
                                    file_path: String::new(),
                                    line: start_point.row + 1,
                                    column: start_point.column + 1,
                                    suggestion: Some("Add bounds checking for function parameter before using as array index.".to_string()),
                                ..Default::default()
                                });
                                return violations;
                            }
                        }
                    }
                }

                // Try to resolve alias first
                let (actual_buffer_name, element_size_bytes) =
                    if let Some(alias) = aliases.get(&array_name) {
                        (alias.original_buffer.as_str(), alias.element_size_bytes)
                    } else {
                        (array_name.as_str(), None)
                    };

                if let Some(buffer_info) = buffers.get(actual_buffer_name) {
                    // Calculate effective buffer size for cast pointers
                    let effective_size = match &buffer_info.size {
                        BufferSize::Static(size) | BufferSize::DynamicCalculated(size) => {
                            if let Some(elem_bytes) = element_size_bytes {
                                // For cast pointers, convert byte size to element count
                                size / elem_bytes
                            } else {
                                *size
                            }
                        }
                        _ => 0, // Will be handled separately below
                    };

                    let is_violation = match &buffer_info.size {
                        BufferSize::Static(_size) | BufferSize::DynamicCalculated(_size) => {
                            match &index {
                                IndexValue::Constant(idx) => {
                                    // Constant index access - check for negative indices OR out of bounds
                                    *idx < 0 || (*idx as usize) >= effective_size
                                }
                                IndexValue::Expression(_, Some(eval_idx)) => {
                                    // Expression evaluated to constant - check bounds
                                    *eval_idx < 0 || (*eval_idx as usize) >= effective_size
                                }
                                IndexValue::Expression(expr, None) => {
                                    // Expression with variable component - analyze it
                                    self.check_expression_bounds(expr, effective_size)
                                }
                                IndexValue::Variable(var) => {
                                    // First, check for recursive function with index modification
                                    if self.has_recursive_index_modification(
                                        node,
                                        var,
                                        source,
                                        effective_size,
                                    ) {
                                        true
                                    } else if let Some(func_node) = find_containing_function(node) {
                                        // Function parameters used as indices without bounds checking are high risk
                                        // Check if the function has ANY bounds validation
                                        if is_function_parameter(&func_node, var, source) {
                                            // Only flag if there's NO bounds checking for this parameter
                                            !self.has_function_parameter_bounds_check(
                                                &func_node, var, source,
                                            )
                                        } else {
                                            false
                                        }
                                    } else {
                                        // Variable index - check for bounds checking
                                        !self.has_proper_bounds_check(node, source, effective_size)
                                    }
                                }
                                IndexValue::Unknown => false,
                            }
                        }
                        BufferSize::Symbolic(size_var) => {
                            // VLA with symbolic size - check for provably out-of-bounds patterns
                            match &index {
                                IndexValue::Variable(var) => {
                                    // Check if var == size_var (e.g., vla[n] when size is n)
                                    // This is always out of bounds (valid range: 0 to n-1)
                                    var == size_var
                                }
                                IndexValue::Expression(expr, _) => {
                                    // Check for symbolic violations like "n + 5" when size is "n"
                                    self.check_symbolic_bounds(expr, size_var)
                                }
                                IndexValue::Constant(idx) => {
                                    // Negative index is always invalid
                                    *idx < 0
                                }
                                IndexValue::Unknown => false,
                            }
                        }
                        BufferSize::Dynamic(_) => {
                            // For dynamic sizes, check if there's any bounds checking
                            match index {
                                IndexValue::Variable(_) | IndexValue::Expression(_, None) => {
                                    !self.has_dynamic_bounds_check(node, source)
                                }
                                _ => false,
                            }
                        }
                        BufferSize::Unknown => false,
                    };

                    if is_violation {
                        let msg = match index {
                            IndexValue::Constant(idx) => {
                                format!("Out-of-bounds array access at index {}", idx)
                            }
                            IndexValue::Expression(ref expr, Some(eval_idx)) => format!(
                                "Out-of-bounds array access: '{}' evaluates to {}",
                                expr, eval_idx
                            ),
                            IndexValue::Expression(ref expr, None) => format!(
                                "Potentially unsafe array access with expression '{}'",
                                expr
                            ),
                            IndexValue::Variable(ref var) => format!(
                                "Potentially unsafe array access with variable index '{}'",
                                var
                            ),
                            IndexValue::Unknown => "Potentially unsafe array access".to_string(),
                        };
                        violations.push(self.create_violation(
                            node,
                            &array_name,
                            buffer_info,
                            &msg,
                        ));
                    }
                }
            }
        }

        violations
    }

    /// Check if an expression with variables could cause out-of-bounds access
    /// For expressions like "var + 5", if constant >= size, it's always unsafe
    fn check_expression_bounds(&self, expr: &str, size: usize) -> bool {
        // Pattern: var + const or const + var
        if expr.contains('+') {
            let parts: Vec<&str> = expr.split('+').collect();
            if parts.len() == 2 {
                // Try to extract the constant part
                for part in parts {
                    if let Ok(const_offset) = part.trim().parse::<usize>() {
                        // If constant offset >= size, ANY value of var causes overflow
                        // (even var = 0 would result in index >= size)
                        if const_offset >= size {
                            return true;
                        }
                    }
                }
            }
        }

        // Pattern: var - const (less common but possible)
        // This is generally safer, so we don't flag without more context

        // For other expressions, require bounds checking
        false
    }

    /// Check symbolic bounds for VLA expressions
    /// Returns true if the expression is provably out of bounds
    fn check_symbolic_bounds(&self, index_expr: &str, size_var: &str) -> bool {
        let expr = index_expr.trim();

        // Pattern 1: size_var + constant (where constant > 0)
        // e.g., "n + 5" when size is "n" - ALWAYS out of bounds
        if expr.contains('+') {
            let parts: Vec<&str> = expr.split('+').collect();
            if parts.len() == 2 {
                let (part1, part2) = (parts[0].trim(), parts[1].trim());

                // Check if one part is size_var and other is positive constant
                if part1 == size_var {
                    if let Ok(offset) = part2.parse::<isize>() {
                        return offset > 0; // ALWAYS out of bounds
                    }
                } else if part2 == size_var {
                    if let Ok(offset) = part1.parse::<isize>() {
                        return offset > 0;
                    }
                }
            }
        }

        // Pattern 2: index == size_var (e.g., vla[n] when size is n)
        // This is out of bounds (valid range: 0 to n-1)
        if expr == size_var {
            return true;
        }

        // Pattern 3: size_var - constant (where constant < 0 would be a problem)
        // e.g., "n - 1" when size is "n" is VALID (last element)
        // We don't flag this as it's generally safe

        false
    }

    /// Check pointer arithmetic for bounds violations
    fn check_pointer_arithmetic(
        &self,
        node: &Node,
        source: &str,
        buffers: &HashMap<String, BufferInfo>,
        aliases: &HashMap<String, PointerAlias>,
    ) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        if let Some((ptr_name, offset)) = self.extract_pointer_arithmetic(node, source) {
            // Try to resolve alias first
            let (actual_buffer_name, element_size_bytes) =
                if let Some(alias) = aliases.get(&ptr_name) {
                    (alias.original_buffer.as_str(), alias.element_size_bytes)
                } else {
                    (ptr_name.as_str(), None)
                };

            if let Some(buffer_info) = buffers.get(actual_buffer_name) {
                match &buffer_info.size {
                    BufferSize::Static(size) | BufferSize::DynamicCalculated(size) => {
                        if let OffsetValue::Constant(off) = offset {
                            // Calculate effective buffer size in elements
                            let effective_size = if let Some(elem_bytes) = element_size_bytes {
                                // For cast pointers, convert byte size to element count
                                // buffer is malloc(16), cast to int* (4 bytes) = 4 ints
                                size / elem_bytes
                            } else {
                                // No cast, use size as-is
                                *size
                            };

                            if off >= effective_size {
                                let msg = format!(
                                    "Pointer arithmetic moves {} elements beyond buffer bounds (effective size: {})",
                                    off, effective_size
                                );
                                violations.push(self.create_violation(
                                    node,
                                    &ptr_name,
                                    buffer_info,
                                    &msg,
                                ));
                            }
                        }
                    }
                    _ => {}
                }
            }
        }

        violations
    }

    /// Extract pointer arithmetic information from assignment
    fn extract_pointer_arithmetic(
        &self,
        node: &Node,
        source: &str,
    ) -> Option<(String, OffsetValue)> {
        let text = &source[node.start_byte()..node.end_byte()];

        // Pattern: ptr += offset
        if text.contains("+=") {
            let parts: Vec<&str> = text.split("+=").collect();
            if parts.len() == 2 {
                let ptr_name = parts[0].trim().to_string();
                let offset_str = parts[1].trim().trim_end_matches(';');

                let offset = if let Ok(const_val) = offset_str.parse::<usize>() {
                    OffsetValue::Constant(const_val)
                } else {
                    OffsetValue::Variable(offset_str.to_string())
                };

                return Some((ptr_name, offset));
            }
        }

        // Pattern: ptr = ptr + offset
        if text.contains('=') && text.contains('+') {
            let parts: Vec<&str> = text.split('=').collect();
            if parts.len() == 2 {
                let ptr_name = parts[0].trim().to_string();
                let rhs = parts[1].trim();

                if rhs.starts_with(&ptr_name) && rhs.contains('+') {
                    let offset_parts: Vec<&str> = rhs.split('+').collect();
                    if offset_parts.len() == 2 {
                        let offset_str = offset_parts[1].trim().trim_end_matches(';');

                        let offset = if let Ok(const_val) = offset_str.parse::<usize>() {
                            OffsetValue::Constant(const_val)
                        } else {
                            OffsetValue::Variable(offset_str.to_string())
                        };

                        return Some((ptr_name, offset));
                    }
                }
            }
        }

        None
    }

    /// Check if assignment is pointer arithmetic
    fn is_pointer_arithmetic_assignment(&self, node: &Node, source: &str) -> bool {
        let text = &source[node.start_byte()..node.end_byte()];
        text.contains("+=") || (text.contains('=') && text.contains('+'))
    }
}

impl Arr30C {
    /// Internal recursive check function that carries buffer_info through the tree
    fn check_with_buffer_info(
        &self,
        node: &Node,
        source: &str,
        buffer_info: &HashMap<String, BufferInfo>,
        aliases: &HashMap<String, PointerAlias>,
        function_macros: &HashMap<String, FunctionMacro>,
    ) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Clone the maps to allow modification during traversal
        let mut local_buffers = buffer_info.clone();
        let mut local_aliases = aliases.clone();

        // Check multiple violation patterns BEFORE extracting declarations
        // This ensures we use the parent's context for checking this node
        match node.kind() {
            "subscript_expression" => {
                violations.extend(self.check_array_subscript(
                    node,
                    source,
                    &local_buffers,
                    &local_aliases,
                ));
            }
            "assignment_expression" => {
                if self.is_pointer_arithmetic_assignment(node, source) {
                    violations.extend(self.check_pointer_arithmetic(
                        node,
                        source,
                        &local_buffers,
                        &local_aliases,
                    ));
                }
            }
            "call_expression" => {
                violations.extend(self.check_dangerous_function_call(node, source, &local_buffers));
                violations.extend(self.check_macro_invocation(
                    node,
                    source,
                    &local_buffers,
                    function_macros,
                ));
            }
            _ => {}
        }

        // Recursively check children, accumulating declarations as we go
        // This ensures declarations are visible to subsequent siblings
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                // Extract declarations from this child if it's a declaration node
                if child.kind() == "declaration" {
                    if let Some(new_buffer) = self.extract_buffer_from_declaration(&child, source) {
                        // Only insert if not already tracked (line-based analysis takes precedence for realloc tracking)
                        if !local_buffers.contains_key(&new_buffer.name) {
                            local_buffers.insert(new_buffer.name.clone(), new_buffer);
                        }
                    }
                    if let Some(new_alias) =
                        self.extract_alias_from_declaration(&child, source, &local_buffers)
                    {
                        local_aliases.insert(new_alias.alias_name.clone(), new_alias);
                    }
                }

                // Track malloc/realloc assignments (e.g., matrix[i] = malloc(...) or ptr = realloc(ptr, size))
                // Check both assignment_expression nodes and their parents (expression_statement)
                let assignment_node = if child.kind() == "assignment_expression" {
                    Some(child)
                } else if child.kind() == "expression_statement" {
                    // Look for assignment_expression child
                    child
                        .child(0)
                        .filter(|c| c.kind() == "assignment_expression")
                } else {
                    None
                };

                if let Some(assign_node) = assignment_node {
                    if let Some((buf_name, buf_info)) =
                        self.extract_buffer_from_assignment(&assign_node, source)
                    {
                        // Insert or update the buffer entry
                        local_buffers.insert(buf_name, buf_info);
                    }
                }

                // Recursively check this child with the accumulated context
                violations.extend(self.check_with_buffer_info(
                    &child,
                    source,
                    &local_buffers,
                    &local_aliases,
                    function_macros,
                ));
            }
        }

        violations
    }

    /// Extract buffer information from a declaration AST node (with typedef support)
    fn extract_buffer_from_declaration_with_typedefs(
        &self,
        node: &Node,
        source: &str,
        typedefs: &HashMap<String, usize>,
    ) -> Option<BufferInfo> {
        // Look for declarator nodes that contain array or pointer declarations
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "init_declarator" => {
                        // Handles: int arr[5] = {...};
                        return self.extract_buffer_from_init_declarator_with_typedefs(
                            &child, source, typedefs,
                        );
                    }
                    "array_declarator" => {
                        // Handles: int arr[5];
                        return self.extract_buffer_from_array_declarator(&child, source);
                    }
                    // For function pointer arrays like void (*functions[3])(void)
                    // the array_declarator is nested inside function_declarator
                    "function_declarator" | "pointer_declarator" => {
                        // Recursively search for array_declarator
                        if let Some(buffer) = self.find_array_declarator_in_node(&child, source) {
                            return Some(buffer);
                        }
                    }
                    _ => {}
                }
            }
        }
        None
    }

    /// Recursively search for array_declarator in a node tree
    fn find_array_declarator_in_node(&self, node: &Node, source: &str) -> Option<BufferInfo> {
        if node.kind() == "array_declarator" {
            return self.extract_buffer_from_array_declarator(node, source);
        }

        // Recursively search children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if let Some(buffer) = self.find_array_declarator_in_node(&child, source) {
                    return Some(buffer);
                }
            }
        }
        None
    }

    /// Extract buffer information from a declaration AST node (without typedefs)
    fn extract_buffer_from_declaration(&self, node: &Node, source: &str) -> Option<BufferInfo> {
        // Look for declarator nodes that contain array or pointer declarations
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "init_declarator" => {
                        // Handles: int arr[5] = {...};
                        return self.extract_buffer_from_init_declarator(&child, source);
                    }
                    "array_declarator" => {
                        // Handles: int arr[5];
                        return self.extract_buffer_from_array_declarator(&child, source);
                    }
                    _ => {}
                }
            }
        }
        None
    }

    /// Extract buffer from init_declarator node (declarations with initializers, with typedef support)
    fn extract_buffer_from_init_declarator_with_typedefs(
        &self,
        node: &Node,
        source: &str,
        typedefs: &HashMap<String, usize>,
    ) -> Option<BufferInfo> {
        // First child is the declarator
        let declarator = node.child(0)?;

        if declarator.kind() == "array_declarator" {
            return self.extract_buffer_from_array_declarator(&declarator, source);
        } else if declarator.kind() == "function_declarator" {
            // For function pointer arrays like: void (*functions[3])(void) = {...}
            // the array_declarator is nested inside function_declarator
            return self.find_array_declarator_in_node(&declarator, source);
        } else if declarator.kind() == "pointer_declarator" {
            // Check if this is a malloc/calloc assignment
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() == "call_expression" {
                        return self.extract_buffer_from_malloc_call(&declarator, &child, source);
                    }
                }
            }
        } else if declarator.kind() == "identifier" {
            // Simple identifier - could be typedef usage
            let var_name = &source[declarator.start_byte()..declarator.end_byte()];

            // Check if this declaration has an initializer that's a call_expression (malloc)
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() == "call_expression" {
                        return self.extract_buffer_from_malloc_call(&declarator, &child, source);
                    }
                }
            }

            // Could be a typedef array - check parent declaration for type
            if let Some(parent) = node.parent() {
                if parent.kind() == "declaration" {
                    return self.check_typedef_declaration(&parent, var_name, source, typedefs);
                }
            }
        }

        None
    }

    /// Extract buffer from init_declarator node (declarations with initializers, without typedefs)
    fn extract_buffer_from_init_declarator(&self, node: &Node, source: &str) -> Option<BufferInfo> {
        // First child is the declarator
        let declarator = node.child(0)?;

        if declarator.kind() == "array_declarator" {
            return self.extract_buffer_from_array_declarator(&declarator, source);
        } else if declarator.kind() == "pointer_declarator" {
            // Check if this is a malloc/calloc assignment
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() == "call_expression" {
                        return self.extract_buffer_from_malloc_call(&declarator, &child, source);
                    }
                }
            }
        } else if declarator.kind() == "identifier" {
            // Simple identifier - could be typedef usage
            let var_name = &source[declarator.start_byte()..declarator.end_byte()];

            // Check if this declaration has an initializer that's a call_expression (malloc)
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() == "call_expression" {
                        return self.extract_buffer_from_malloc_call(&declarator, &child, source);
                    }
                }
            }

            // Could be a typedef array - check parent declaration for type (fallback without typedef cache)
            if let Some(parent) = node.parent() {
                if parent.kind() == "declaration" {
                    // Create empty typedefs map for fallback
                    let empty_typedefs = HashMap::new();
                    return self.check_typedef_declaration(
                        &parent,
                        var_name,
                        source,
                        &empty_typedefs,
                    );
                }
            }
        }

        None
    }

    /// Extract buffer info from array_declarator
    /// For multidimensional arrays, extracts the INNERMOST dimension (the base array)
    /// The caller is responsible for extracting outer dimensions
    fn extract_buffer_from_array_declarator(
        &self,
        node: &Node,
        source: &str,
    ) -> Option<BufferInfo> {
        // Check if first child is a nested array_declarator (multidimensional array)
        if let Some(first_child) = node.child(0) {
            if first_child.kind() == "array_declarator" {
                // This is a multidimensional array like int matrix[3][4]
                // The nested array_declarator contains the outer dimension (3)
                // This node contains the inner dimension (4)
                // We should extract from the nested one to get the base buffer
                return self.extract_buffer_from_array_declarator(&first_child, source);
            }
        }

        // Single-dimensional array or innermost dimension of multidimensional array
        let mut var_name: Option<String> = None;
        let mut size: Option<usize> = None;
        let mut size_expr: Option<String> = None;

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "identifier" => {
                        if var_name.is_none() {
                            var_name =
                                Some(source[child.start_byte()..child.end_byte()].to_string());
                        } else if i > 0 {
                            // This is the size expression (VLA)
                            let expr = &source[child.start_byte()..child.end_byte()];
                            size_expr = Some(expr.to_string());
                        }
                    }
                    "number_literal" => {
                        let size_str = &source[child.start_byte()..child.end_byte()];
                        size = size_str.parse().ok();
                    }
                    // Handle complex declarators (function pointers, nested pointers, etc.)
                    "function_declarator" | "pointer_declarator" | "parenthesized_declarator" => {
                        if var_name.is_none() {
                            var_name = find_identifier_in_declarator(&child, source);
                        }
                    }
                    _ => {}
                }
            }
        }

        let name = var_name?;
        let line = node.start_position().row + 1;

        if let Some(s) = size {
            Some(BufferInfo {
                name,
                size: BufferSize::Static(s),
                element_type: "unknown".to_string(),
                allocation_line: line,
            })
        } else if let Some(expr) = size_expr {
            Some(BufferInfo {
                name,
                size: BufferSize::Symbolic(expr),
                element_type: "unknown".to_string(),
                allocation_line: line,
            })
        } else {
            None
        }
    }

    /// Extract multidimensional array buffers
    /// For int matrix[3][4], creates:
    /// - "matrix" with size 3 (already created by extract_buffer_from_array_declarator)
    /// - "matrix[*]" with size 4 (created here for inner dimension checking)
    fn extract_multidimensional_buffers(
        &self,
        decl_node: &Node,
        base_name: &str,
        source: &str,
        buffers: &mut HashMap<String, BufferInfo>,
    ) {
        // Find the array_declarator in the declaration
        for i in 0..decl_node.child_count() {
            if let Some(child) = decl_node.child(i) {
                if child.kind() == "array_declarator" || child.kind() == "init_declarator" {
                    // Found the declarator - extract inner dimensions
                    self.extract_inner_dimensions(&child, base_name, source, buffers);
                    return;
                }
            }
        }
    }

    /// Recursively extract inner dimensions from array_declarator nodes
    /// For int matrix[3][4], when called on the outer array_declarator:
    /// - Creates "matrix[*]" with size 4
    fn extract_inner_dimensions(
        &self,
        node: &Node,
        base_name: &str,
        source: &str,
        buffers: &mut HashMap<String, BufferInfo>,
    ) {
        if node.kind() == "init_declarator" {
            // Skip to the declarator child
            if let Some(declarator) = node.child(0) {
                self.extract_inner_dimensions(&declarator, base_name, source, buffers);
            }
            return;
        }

        if node.kind() != "array_declarator" {
            return;
        }

        // Check if first child is a nested array_declarator
        if let Some(first_child) = node.child(0) {
            if first_child.kind() == "array_declarator" {
                // This node represents an outer dimension
                // Extract the size from THIS node (the outer dimension in the AST)
                if let Some(size) = self.extract_array_size(node, source) {
                    // Create wildcard entry
                    let wildcard_name = format!("{}[*]", base_name);
                    let line = node.start_position().row + 1;

                    buffers.insert(
                        wildcard_name,
                        BufferInfo {
                            name: base_name.to_string(),
                            size,
                            element_type: "array_element".to_string(),
                            allocation_line: line,
                        },
                    );
                }

                // Continue recursing for deeper dimensions (e.g., int arr[2][3][4])
                self.extract_inner_dimensions(&first_child, base_name, source, buffers);
            }
        }
    }

    /// Extract the size from an array_declarator node
    fn extract_array_size(&self, node: &Node, source: &str) -> Option<BufferSize> {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "number_literal" {
                    let size_str = &source[child.start_byte()..child.end_byte()];
                    if let Ok(size) = size_str.parse::<usize>() {
                        return Some(BufferSize::Static(size));
                    }
                } else if child.kind() == "identifier" && i > 0 {
                    // VLA with symbolic size
                    let expr = &source[child.start_byte()..child.end_byte()];
                    return Some(BufferSize::Symbolic(expr.to_string()));
                }
            }
        }
        None
    }

    /// Extract buffer from malloc/calloc call
    fn extract_buffer_from_malloc_call(
        &self,
        declarator: &Node,
        call_node: &Node,
        source: &str,
    ) -> Option<BufferInfo> {
        let var_name = if declarator.kind() == "pointer_declarator" {
            // Navigate to the identifier within pointer_declarator (may be nested for double pointers)
            self.find_identifier_in_declarator(declarator, source)?
        } else {
            source[declarator.start_byte()..declarator.end_byte()].to_string()
        };

        // Get function name
        let func_name_node = call_node.child(0)?;
        let func_name = &source[func_name_node.start_byte()..func_name_node.end_byte()];

        // Find argument_list
        for i in 0..call_node.child_count() {
            if let Some(child) = call_node.child(i) {
                if child.kind() == "argument_list" {
                    return self.parse_malloc_arguments(
                        func_name,
                        &child,
                        source,
                        &var_name,
                        call_node.start_position().row + 1,
                    );
                }
            }
        }

        None
    }

    /// Recursively find identifier within a declarator (handles nested pointer_declarators)
    fn find_identifier_in_declarator(&self, node: &Node, source: &str) -> Option<String> {
        if node.kind() == "identifier" {
            return Some(source[node.start_byte()..node.end_byte()].to_string());
        }

        // Recursively search children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if let Some(name) = self.find_identifier_in_declarator(&child, source) {
                    return Some(name);
                }
            }
        }

        None
    }

    /// Parse malloc/calloc/realloc arguments from argument_list node
    fn parse_malloc_arguments(
        &self,
        func_name: &str,
        arg_list: &Node,
        source: &str,
        var_name: &str,
        line: usize,
    ) -> Option<BufferInfo> {
        match func_name {
            "malloc" => {
                // Get first argument
                for i in 0..arg_list.child_count() {
                    if let Some(child) = arg_list.child(i) {
                        if child.kind() != "(" && child.kind() != ")" && child.kind() != "," {
                            let arg_text = &source[child.start_byte()..child.end_byte()];
                            let size = self.calculate_malloc_size(arg_text)?;
                            return Some(BufferInfo {
                                name: var_name.to_string(),
                                size,
                                element_type: "unknown".to_string(),
                                allocation_line: line,
                            });
                        }
                    }
                }
            }
            "realloc" => {
                // Get second argument (size) - first arg is the old pointer
                let mut args = Vec::new();
                for i in 0..arg_list.child_count() {
                    if let Some(child) = arg_list.child(i) {
                        if child.kind() != "(" && child.kind() != ")" && child.kind() != "," {
                            args.push(&source[child.start_byte()..child.end_byte()]);
                        }
                    }
                }
                if args.len() >= 2 {
                    let size = self.calculate_malloc_size(args[1])?;
                    return Some(BufferInfo {
                        name: var_name.to_string(),
                        size,
                        element_type: "unknown".to_string(),
                        allocation_line: line,
                    });
                }
            }
            "calloc" => {
                // Get first argument (count)
                let mut args = Vec::new();
                for i in 0..arg_list.child_count() {
                    if let Some(child) = arg_list.child(i) {
                        if child.kind() != "(" && child.kind() != ")" && child.kind() != "," {
                            args.push(&source[child.start_byte()..child.end_byte()]);
                        }
                    }
                }
                if args.len() >= 2 {
                    if let Some(count) = self.extract_numeric_value(args[0]) {
                        if self.extract_sizeof_value(args[1]).is_some() {
                            return Some(BufferInfo {
                                name: var_name.to_string(),
                                size: BufferSize::DynamicCalculated(count),
                                element_type: "unknown".to_string(),
                                allocation_line: line,
                            });
                        }
                    }
                }
            }
            _ => {}
        }
        None
    }

    /// Check if a declaration uses a typedef array type
    fn check_typedef_declaration(
        &self,
        decl_node: &Node,
        var_name: &str,
        source: &str,
        typedefs: &HashMap<String, usize>,
    ) -> Option<BufferInfo> {
        // Get type from declaration
        for i in 0..decl_node.child_count() {
            if let Some(child) = decl_node.child(i) {
                if child.kind() == "type_identifier" {
                    let type_name = &source[child.start_byte()..child.end_byte()];

                    // Check if this type is in our cached typedefs
                    if let Some(&size) = typedefs.get(type_name) {
                        return Some(BufferInfo {
                            name: var_name.to_string(),
                            size: BufferSize::Static(size),
                            element_type: type_name.to_string(),
                            allocation_line: decl_node.start_position().row + 1,
                        });
                    }
                }
            }
        }
        None
    }

    /// Extract pointer alias from declaration AST node
    fn extract_alias_from_declaration(
        &self,
        node: &Node,
        source: &str,
        buffers: &HashMap<String, BufferInfo>,
    ) -> Option<PointerAlias> {
        // Look for init_declarator with pointer assignment
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "init_declarator" {
                    return self.extract_alias_from_init_declarator(&child, source, buffers);
                }
            }
        }
        None
    }

    /// Extract alias from init_declarator
    fn extract_alias_from_init_declarator(
        &self,
        node: &Node,
        source: &str,
        buffers: &HashMap<String, BufferInfo>,
    ) -> Option<PointerAlias> {
        // First, check for cast expression (int *ptr = (int *)buffer)
        let mut declarator_child: Option<Node> = None;

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "pointer_declarator" || child.kind() == "identifier" {
                    declarator_child = Some(child);
                } else if child.kind() == "cast_expression" {
                    if let Some(decl) = declarator_child {
                        return self.extract_alias_from_cast(&decl, &child, source, buffers);
                    }
                }
            }
        }

        // Check for direct assignment (int *ptr = buffer)
        if let Some(declarator) = node.child(0) {
            // Look for assigned value
            for i in 1..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() == "identifier" {
                        let ptr_name = find_identifier_in_declarator(&declarator, source)?;
                        let buf_name = &source[child.start_byte()..child.end_byte()];

                        if buffers.contains_key(buf_name) {
                            return Some(PointerAlias {
                                alias_name: ptr_name,
                                original_buffer: buf_name.to_string(),
                                element_size_bytes: None,
                            });
                        }
                    }
                }
            }
        }

        None
    }

    /// Extract alias from cast expression
    fn extract_alias_from_cast(
        &self,
        declarator: &Node,
        cast_node: &Node,
        source: &str,
        buffers: &HashMap<String, BufferInfo>,
    ) -> Option<PointerAlias> {
        let ptr_name = find_identifier_in_declarator(declarator, source)?;

        // Get cast type
        let mut cast_type: Option<&str> = None;
        let mut target: Option<&str> = None;

        for i in 0..cast_node.child_count() {
            if let Some(child) = cast_node.child(i) {
                match child.kind() {
                    "type_descriptor" => {
                        // Extract type from type_descriptor
                        for j in 0..child.child_count() {
                            if let Some(type_node) = child.child(j) {
                                if type_node.kind() == "primitive_type" {
                                    cast_type =
                                        Some(&source[type_node.start_byte()..type_node.end_byte()]);
                                }
                            }
                        }
                    }
                    "identifier" => {
                        target = Some(&source[child.start_byte()..child.end_byte()]);
                    }
                    _ => {}
                }
            }
        }

        if let (Some(cast_t), Some(buf_name)) = (cast_type, target) {
            if buffers.contains_key(buf_name) {
                let elem_size = match cast_t {
                    "char" => Some(1),
                    "short" => Some(2),
                    "int" => Some(4),
                    "long" => Some(8),
                    "float" => Some(4),
                    "double" => Some(8),
                    _ => None,
                };

                return Some(PointerAlias {
                    alias_name: ptr_name,
                    original_buffer: buf_name.to_string(),
                    element_size_bytes: elem_size,
                });
            }
        }

        None
    }

    // Removed: extract_identifier_from_declarator - now using ast_utils::find_identifier_in_declarator

    /// Check for dangerous library function calls that can cause buffer overflows
    fn check_dangerous_function_call(
        &self,
        node: &Node,
        source: &str,
        buffers: &HashMap<String, BufferInfo>,
    ) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Get function name
        if let Some(func_name_node) = node.child(0) {
            let func_name = &source[func_name_node.start_byte()..func_name_node.end_byte()];

            match func_name {
                "strcpy" => violations.extend(self.check_strcpy(node, source, buffers)),
                "strcat" => violations.extend(self.check_strcat(node, source, buffers)),
                "memcpy" | "memmove" => violations.extend(self.check_memcpy(node, source, buffers)),
                "sprintf" => violations.extend(self.check_sprintf(node, source, buffers)),
                "gets" => violations.extend(self.check_gets(node, source, buffers)),
                _ => {}
            }
        }

        violations
    }

    /// Check for macro invocations that might involve array access
    /// Since macros are not expanded, we flag them for manual review if they:
    /// 1. Match a known function-like macro definition
    /// 2. Take arguments that include tracked buffer names
    fn check_macro_invocation(
        &self,
        node: &Node,
        source: &str,
        buffers: &HashMap<String, BufferInfo>,
        function_macros: &HashMap<String, FunctionMacro>,
    ) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Get the function/macro name
        if let Some(func_name_node) = node.child(0) {
            let func_name = &source[func_name_node.start_byte()..func_name_node.end_byte()];

            // Check if this matches a known function-like macro
            if let Some(macro_info) = function_macros.get(func_name) {
                // Check if the macro body contains array subscript syntax
                if macro_info.body.contains('[') && macro_info.body.contains(']') {
                    // Get the arguments to see if any are tracked buffers
                    if let Some(args) = self.get_function_arguments(node, source) {
                        let mut involves_buffer = false;
                        for arg in &args {
                            let arg_name = arg.trim();
                            if buffers.contains_key(arg_name) {
                                involves_buffer = true;
                                break;
                            }
                        }

                        // If the macro involves array syntax and operates on a tracked buffer,
                        // flag it for manual review
                        if involves_buffer || !args.is_empty() {
                            let start_point = node.start_position();
                            violations.push(RuleViolation {
                                rule_id: "ARR30-C".to_string(),
                                severity: Severity::Medium,
                                message: format!(
                                    "Macro '{}' may generate array access that cannot be statically analyzed. Macro body: '{}'. Manual review required to ensure bounds safety",
                                    func_name,
                                    macro_info.body
                                ),
                                file_path: String::new(),
                                line: start_point.row + 1,
                                column: start_point.column + 1,
                                suggestion: Some(format!(
                                    "Manually verify that macro expansion does not create out-of-bounds access. Macro defined at line {}",
                                    macro_info.line
                                )),
                                requires_manual_review: Some(true),
                                ..Default::default()
                            });
                        }
                    }
                }
            }
        }

        violations
    }

    /// Check strcpy calls for buffer overflow potential
    fn check_strcpy(
        &self,
        node: &Node,
        source: &str,
        buffers: &HashMap<String, BufferInfo>,
    ) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // strcpy(dest, src) - arguments are in argument_list node
        if let Some(args) = self.get_function_arguments(node, source) {
            if args.len() >= 2 {
                let dest_name = args[0].trim();
                let src_text = args[1].trim();

                // Check if destination is a tracked buffer
                if let Some(dest_info) = buffers.get(dest_name) {
                    // Check if source is a string literal or tracked buffer
                    let src_size = if src_text.starts_with('"') {
                        // String literal - count characters (rough estimate)
                        Some(src_text.len() - 2) // Subtract quotes, actual length may vary
                    } else if let Some(src_info) = buffers.get(src_text) {
                        // Source is also a tracked buffer
                        match src_info.size {
                            BufferSize::Static(s) | BufferSize::DynamicCalculated(s) => Some(s),
                            _ => None,
                        }
                    } else {
                        None
                    };

                    // If we know both sizes, check if source is larger
                    if let Some(src_s) = src_size {
                        if let BufferSize::Static(dest_s) | BufferSize::DynamicCalculated(dest_s) =
                            dest_info.size
                        {
                            if src_s > dest_s {
                                violations.push(self.create_library_violation(
                                    node,
                                    dest_name,
                                    dest_info,
                                    &format!(
                                        "strcpy may overflow: source size {} > destination size {}",
                                        src_s, dest_s
                                    ),
                                ));
                                return violations;
                            }
                        }
                    }

                    // Even if we can't determine exact sizes, strcpy is inherently unsafe
                    // Only flag if source is unknown (not a literal)
                    if !src_text.starts_with('"') && src_size.is_none() {
                        violations.push(self.create_library_violation(
                            node,
                            dest_name,
                            dest_info,
                            "strcpy with unknown source size can cause buffer overflow",
                        ));
                    }
                }
            }
        }

        violations
    }

    /// Check strcat calls for buffer overflow potential
    fn check_strcat(
        &self,
        node: &Node,
        source: &str,
        buffers: &HashMap<String, BufferInfo>,
    ) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        if let Some(args) = self.get_function_arguments(node, source) {
            if args.len() >= 2 {
                let dest_name = args[0].trim();

                if let Some(dest_info) = buffers.get(dest_name) {
                    // strcat is dangerous without knowing current string length
                    violations.push(self.create_library_violation(
                        node,
                        dest_name,
                        dest_info,
                        "strcat can cause buffer overflow without length checks",
                    ));
                }
            }
        }

        violations
    }

    /// Check memcpy/memmove calls for buffer overflow potential
    fn check_memcpy(
        &self,
        node: &Node,
        source: &str,
        buffers: &HashMap<String, BufferInfo>,
    ) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // memcpy(dest, src, count)
        if let Some(args) = self.get_function_arguments(node, source) {
            if args.len() >= 3 {
                let dest_name = args[0].trim();
                let src_name = args[1].trim();
                let count_expr = args[2].trim();

                if let Some(dest_info) = buffers.get(dest_name) {
                    // Try to parse count
                    let count = if let Ok(c) = count_expr.parse::<usize>() {
                        Some(c)
                    } else if count_expr.contains("sizeof") {
                        // Check for sizeof(src) pattern
                        if let Some(src_info) = buffers.get(src_name) {
                            match src_info.size {
                                BufferSize::Static(s) | BufferSize::DynamicCalculated(s) => Some(s),
                                _ => None,
                            }
                        } else {
                            None
                        }
                    } else {
                        None
                    };

                    // Check if count exceeds destination size
                    if let Some(c) = count {
                        if let BufferSize::Static(dest_s) | BufferSize::DynamicCalculated(dest_s) =
                            dest_info.size
                        {
                            if c > dest_s {
                                violations.push(self.create_library_violation(
                                    node,
                                    dest_name,
                                    dest_info,
                                    &format!(
                                        "memcpy copies {} bytes into {}-byte buffer",
                                        c, dest_s
                                    ),
                                ));
                            }
                        }
                    }
                }
            }
        }

        violations
    }

    /// Check sprintf calls (always potentially unsafe)
    fn check_sprintf(
        &self,
        node: &Node,
        source: &str,
        buffers: &HashMap<String, BufferInfo>,
    ) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        if let Some(args) = self.get_function_arguments(node, source) {
            if !args.is_empty() {
                let dest_name = args[0].trim();

                if let Some(dest_info) = buffers.get(dest_name) {
                    violations.push(self.create_library_violation(
                        node,
                        dest_name,
                        dest_info,
                        "sprintf can cause buffer overflow; use snprintf instead",
                    ));
                }
            }
        }

        violations
    }

    /// Check gets calls (always unsafe)
    fn check_gets(
        &self,
        node: &Node,
        source: &str,
        buffers: &HashMap<String, BufferInfo>,
    ) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        if let Some(args) = self.get_function_arguments(node, source) {
            if !args.is_empty() {
                let dest_name = args[0].trim();

                if let Some(dest_info) = buffers.get(dest_name) {
                    violations.push(self.create_library_violation(
                        node,
                        dest_name,
                        dest_info,
                        "gets is inherently unsafe and can cause buffer overflow",
                    ));
                }
            }
        }

        violations
    }

    /// Extract function arguments from a call_expression node
    fn get_function_arguments(&self, node: &Node, source: &str) -> Option<Vec<String>> {
        // Find the argument_list node
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "argument_list" {
                    let mut args = Vec::new();
                    for j in 0..child.child_count() {
                        if let Some(arg_node) = child.child(j) {
                            if arg_node.kind() != "("
                                && arg_node.kind() != ")"
                                && arg_node.kind() != ","
                            {
                                let arg_text = &source[arg_node.start_byte()..arg_node.end_byte()];
                                args.push(arg_text.to_string());
                            }
                        }
                    }
                    return Some(args);
                }
            }
        }
        None
    }

    /// Create a violation for dangerous library function
    fn create_library_violation(
        &self,
        node: &Node,
        buffer_name: &str,
        buffer_info: &BufferInfo,
        message: &str,
    ) -> RuleViolation {
        let start_point = node.start_position();

        let size_info = match &buffer_info.size {
            BufferSize::Static(s) => format!("size {}", s),
            BufferSize::DynamicCalculated(s) => format!("allocated size {}", s),
            BufferSize::Dynamic(expr) => format!("dynamic size ({})", expr),
            BufferSize::Symbolic(var) => format!("VLA size ({})", var),
            BufferSize::Unknown => "unknown size".to_string(),
        };

        RuleViolation {
            rule_id: self.rule_id().to_string(),
            severity: Severity::High,
            message: format!("{}: Buffer '{}' with {} (allocated at line {})",
                           message, buffer_name, size_info, buffer_info.allocation_line),
            file_path: String::new(),
            line: start_point.row + 1,
            column: start_point.column + 1,
            suggestion: Some("Use safer alternatives like strncpy, strncat, snprintf, or fgets with proper size limits.".to_string()),
            ..Default::default()
        }
    }

    /// Get array node from subscript expression
    fn get_subscript_array<'a>(&self, node: &'a Node<'a>) -> Option<Node<'a>> {
        node.child(0)
    }

    /// Get index node from subscript expression
    fn get_subscript_index<'a>(&self, node: &'a Node<'a>) -> Option<Node<'a>> {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() != "[" && child.kind() != "]" && i > 0 {
                    return Some(child);
                }
            }
        }
        None
    }

    /// Check if condition contains safe bounds (< operator, not <=)
    fn condition_contains_safe_bounds(&self, condition_text: &str, index_text: &str) -> bool {
        let trimmed_index = index_text.trim();

        // Check for unsafe <= operator first - this is ALWAYS unsafe for array bounds
        // because it allows accessing the element at index == size, which is out of bounds
        if condition_text.contains(&format!("{} <=", trimmed_index)) {
            return false; // <= is ALWAYS unsafe for array bounds
        }

        // Check for safe < operator
        if condition_text.contains(&format!("{} <", trimmed_index)) {
            return true;
        }

        // Check for reverse condition: size > index (safe)
        if condition_text.contains(&format!("> {}", trimmed_index)) {
            // Make sure it's not >= (which would be unsafe)
            return !condition_text.contains(&format!(">= {}", trimmed_index));
        }

        false
    }

    /// Generic loop bounds check (when index variable is unknown)
    fn check_for_loop_bounds_generic(&self, for_node: &Node, source: &str) -> bool {
        for i in 0..for_node.child_count() {
            if let Some(child) = for_node.child(i) {
                if child.kind() == "binary_expression" || child.kind() == "comparison_expression" {
                    let condition_text = &source[child.start_byte()..child.end_byte()];
                    // Look for any < operator (safe bounds check)
                    if condition_text.contains(" < ") && !condition_text.contains(" <= ") {
                        return true;
                    }
                }
            }
        }
        false
    }

    /// Generic if bounds check (when index variable is unknown)
    fn check_if_bounds_generic(&self, if_node: &Node, source: &str) -> bool {
        for i in 0..if_node.child_count() {
            if let Some(child) = if_node.child(i) {
                if child.kind() == "parenthesized_expression" || child.kind() == "binary_expression"
                {
                    let condition_text = &source[child.start_byte()..child.end_byte()];
                    // Look for any < operator (safe bounds check)
                    if condition_text.contains(" < ") && !condition_text.contains(" <= ") {
                        return true;
                    }
                }
            }
        }
        false
    }
}

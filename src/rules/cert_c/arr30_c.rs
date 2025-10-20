use super::super::{CertRule, RuleViolation};
use crate::manifest::Severity;
use tree_sitter::Node;
use std::collections::HashMap;

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
    Static(usize),              // char arr[10]
    DynamicCalculated(usize),   // malloc(10 * sizeof(int))
    Dynamic(String),            // malloc(size) - variable expression
    Symbolic(String),           // VLA: int arr[n] - symbolic size
    Unknown,
}

/// Represents an index value that can be constant or variable
#[derive(Debug)]
enum IndexValue {
    Constant(isize),  // Changed from usize to support negative indices
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
    alias_name: String,          // The pointer variable name (e.g., "ptr", "int_array")
    original_buffer: String,     // The original buffer name (e.g., "arr", "buffer")
    element_size_bytes: Option<usize>, // Element size for cast pointers (e.g., 4 for int, 1 for char)
}

impl Arr30C {
    /// Analyze all buffer allocations in the source code
    fn analyze_buffer_allocations(&self, source: &str) -> HashMap<String, BufferInfo> {
        let mut buffers = HashMap::new();
        let lines: Vec<&str> = source.lines().collect();

        // First pass: collect typedef information
        let typedefs = self.analyze_typedefs(source);

        // Also analyze struct member arrays declared with typedefs
        self.analyze_struct_typedef_members(source, &typedefs, &mut buffers);

        for (line_idx, line) in lines.iter().enumerate() {
            // Pattern 1: Array declarations - type arr[SIZE]
            if let Some(buffer) = self.parse_array_declaration(line, line_idx, &typedefs) {
                buffers.insert(buffer.name.clone(), buffer);
            }

            // Pattern 2: malloc allocations - ptr = malloc(SIZE)
            if let Some(buffer) = self.parse_malloc_allocation(line, line_idx) {
                buffers.insert(buffer.name.clone(), buffer);
            }

            // Pattern 3: calloc allocations - ptr = calloc(COUNT, SIZE)
            if let Some(buffer) = self.parse_calloc_allocation(line, line_idx) {
                buffers.insert(buffer.name.clone(), buffer);
            }

            // Pattern 4: realloc allocations - ptr = realloc(ptr, SIZE)
            if let Some(buffer) = self.parse_realloc_allocation(line, line_idx) {
                buffers.insert(buffer.name.clone(), buffer);
            }
        }

        buffers
    }

    /// Analyze pointer aliases in the source code
    fn analyze_pointer_aliases(&self, source: &str, buffers: &HashMap<String, BufferInfo>) -> HashMap<String, PointerAlias> {
        let mut aliases = HashMap::new();
        let lines: Vec<&str> = source.lines().collect();

        for line in lines.iter() {
            // Pattern 1: Direct pointer assignment: int *ptr = arr;
            if let Some(alias) = self.parse_direct_pointer_assignment(line, buffers) {
                aliases.insert(alias.alias_name.clone(), alias);
            }

            // Pattern 2: Cast assignment: int *ptr = (int *)buffer;
            if let Some(alias) = self.parse_cast_pointer_assignment(line, buffers) {
                aliases.insert(alias.alias_name.clone(), alias);
            }
        }

        aliases
    }

    /// Parse direct pointer assignments like: int *ptr = arr;
    fn parse_direct_pointer_assignment(&self, line: &str, buffers: &HashMap<String, BufferInfo>) -> Option<PointerAlias> {
        // Pattern: type *ptr_name = buffer_name;
        let pattern = r"\w+\s+\*\s*(\w+)\s*=\s*(\w+)\s*;";

        if let Ok(re) = regex::Regex::new(pattern) {
            if let Some(caps) = re.captures(line) {
                if let (Some(ptr_match), Some(buf_match)) = (caps.get(1), caps.get(2)) {
                    let ptr_name = ptr_match.as_str();
                    let buf_name = buf_match.as_str();

                    // Check if buf_name is a tracked buffer
                    if buffers.contains_key(buf_name) {
                        return Some(PointerAlias {
                            alias_name: ptr_name.to_string(),
                            original_buffer: buf_name.to_string(),
                            element_size_bytes: None, // No type conversion
                        });
                    }
                }
            }
        }

        None
    }

    /// Parse cast pointer assignments like: int *int_array = (int *)buffer;
    fn parse_cast_pointer_assignment(&self, line: &str, buffers: &HashMap<String, BufferInfo>) -> Option<PointerAlias> {
        // Pattern: type *ptr_name = (type *)buffer_name;
        let pattern = r"(\w+)\s+\*\s*(\w+)\s*=\s*\(\s*\w+\s*\*\s*\)\s*(\w+)\s*;";

        if let Ok(re) = regex::Regex::new(pattern) {
            if let Some(caps) = re.captures(line) {
                if let (Some(type_match), Some(ptr_match), Some(buf_match)) = (caps.get(1), caps.get(2), caps.get(3)) {
                    let cast_type = type_match.as_str();
                    let ptr_name = ptr_match.as_str();
                    let buf_name = buf_match.as_str();

                    // Check if buf_name is a tracked buffer
                    if buffers.contains_key(buf_name) {
                        // Determine element size based on cast type
                        let elem_size = match cast_type {
                            "char" => Some(1),
                            "short" => Some(2),
                            "int" => Some(4),
                            "long" => Some(8),
                            "float" => Some(4),
                            "double" => Some(8),
                            _ => None, // Unknown type - can't determine element size
                        };

                        return Some(PointerAlias {
                            alias_name: ptr_name.to_string(),
                            original_buffer: buf_name.to_string(),
                            element_size_bytes: elem_size,
                        });
                    }
                }
            }
        }

        None
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
    fn analyze_struct_typedef_members(&self, source: &str, typedefs: &HashMap<String, usize>, buffers: &mut HashMap<String, BufferInfo>) {
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
                                }
                            );
                        }
                    }
                }
            }
        }
    }

    /// Parse VLA (Variable Length Array) declarations: int arr[n];
    fn parse_vla_declaration(&self, line: &str, line_idx: usize) -> Option<BufferInfo> {
        // Pattern: type var[variable_name]
        let vla_pattern = r"(?:const\s+)?(?:volatile\s+)?(?:unsigned\s+)?(?:signed\s+)?(\w+)\s+(\w+)\s*\[\s*([a-zA-Z_]\w*)\s*\]";

        if let Ok(re) = regex::Regex::new(vla_pattern) {
            if let Some(caps) = re.captures(line) {
                if let (Some(var_name), Some(size_var)) = (caps.get(2), caps.get(3)) {
                    // Make sure size_var is not a digit (which would be caught by regular parsing)
                    let size_var_str = size_var.as_str();
                    if !size_var_str.chars().all(|c| c.is_numeric()) {
                        return Some(BufferInfo {
                            name: var_name.as_str().to_string(),
                            size: BufferSize::Symbolic(size_var_str.to_string()),
                            element_type: caps.get(1).map(|m| m.as_str().to_string()).unwrap_or_else(|| "unknown".to_string()),
                            allocation_line: line_idx + 1,
                        });
                    }
                }
            }
        }

        None
    }

    /// Parse typedef array usage: TypedefName var;
    fn parse_typedef_usage(&self, line: &str, line_idx: usize, typedefs: &HashMap<String, usize>) -> Option<BufferInfo> {
        // Pattern 1: Direct typedef usage (e.g., "IntArray local_array = ...")
        // Pattern 2: Struct member typedef usage (e.g., "    IntArray numbers;")

        let patterns = [
            r"^\s*(\w+)\s+(\w+)\s*[=;{]",  // TypedefName varname = ... or TypedefName varname; or = {
        ];

        for pattern in &patterns {
            if let Ok(re) = regex::Regex::new(pattern) {
                if let Some(caps) = re.captures(line) {
                    if let (Some(type_match), Some(var_match)) = (caps.get(1), caps.get(2)) {
                        let type_name = type_match.as_str();
                        let var_name = var_match.as_str();

                        // Filter out C keywords that might match the pattern
                        let c_keywords = ["if", "for", "while", "switch", "return", "int", "char", "void", "float", "double", "struct", "union", "enum", "const", "static", "extern", "volatile", "unsigned", "signed", "long", "short"];
                        if c_keywords.contains(&type_name) {
                            continue;
                        }

                        // Check if type_name is a known typedef
                        if let Some(&size) = typedefs.get(type_name) {
                            return Some(BufferInfo {
                                name: var_name.to_string(),
                                size: BufferSize::Static(size),
                                element_type: type_name.to_string(),
                                allocation_line: line_idx + 1,
                            });
                        }
                    }
                }
            }
        }

        None
    }

    /// Parse array declarations like: int arr[10];
    fn parse_array_declaration(&self, line: &str, line_idx: usize, typedefs: &HashMap<String, usize>) -> Option<BufferInfo> {
        // First, check for VLA declarations: type var[variable]
        if let Some(vla_info) = self.parse_vla_declaration(line, line_idx) {
            return Some(vla_info);
        }

        // Check for typedef-based declarations: TypedefName var; or TypedefName var;
        if let Some(typedef_info) = self.parse_typedef_usage(line, line_idx, typedefs) {
            return Some(typedef_info);
        }

        // Match patterns like: type var_name[SIZE];
        // Handle various types including pointers and function pointer arrays
        let patterns = [
            // Pattern 1: Full qualifier support - handles extern, static, const, volatile, restrict, thread_local, etc.
            r"(?:extern\s+)?(?:static\s+)?(?:_Thread_local\s+)?(?:thread_local\s+)?(?:const\s+)?(?:volatile\s+)?(?:restrict\s+)?(?:unsigned\s+)?(?:signed\s+)?(\w+)\s+(?:restrict\s+)?(\w+)\s*\[\s*(\d+)\s*\]",

            // Pattern 2: Struct/union with qualifiers
            r"(?:extern\s+)?(?:static\s+)?(?:const\s+)?(?:volatile\s+)?(?:struct\s+\w+)\s+(\w+)\s*\[\s*(\d+)\s*\]",

            // Pattern 3: Function pointer arrays with qualifiers: void (*name[SIZE])(...) or int (*name[SIZE])(...)
            r"(?:static\s+)?(?:const\s+)?(\w+)\s+\(\s*\*\s*(\w+)\s*\[\s*(\d+)\s*\]\s*\)",

            // Pattern 4: Member arrays inside struct/union with qualifiers
            r"^\s+(?:const\s+)?(?:volatile\s+)?(?:unsigned\s+)?(\w+)\s+(\w+)\s*\[\s*(\d+)\s*\]\s*;",

            // Pattern 5: Atomic and other C11 qualifiers
            r"(?:_Atomic\s+)?(?:const\s+)?(?:volatile\s+)?(?:unsigned\s+)?(\w+)\s+(\w+)\s*\[\s*(\d+)\s*\]",
        ];

        for (pattern_idx, pattern) in patterns.iter().enumerate() {
            if let Ok(re) = regex::Regex::new(pattern) {
                if let Some(captures) = re.captures(line) {
                    // Determine which pattern matched and extract name and size accordingly
                    let (var_name, size_str) = if pattern_idx == 2 {
                        // Pattern 2 (index 2): Function pointer array pattern
                        (captures.get(1)?.as_str(), captures.get(2)?.as_str())
                    } else if pattern_idx == 1 {
                        // Pattern 1 (index 1): Struct array pattern with 2 capture groups
                        (captures.get(1)?.as_str(), captures.get(2)?.as_str())
                    } else if pattern_idx == 3 || captures.len() == 4 {
                        // Pattern 3 (index 3): Member arrays OR Patterns 0, 4 with 3 capture groups
                        (captures.get(2)?.as_str(), captures.get(3)?.as_str())
                    } else {
                        // Pattern 0 or 4: Regular array patterns with 3 capture groups
                        (captures.get(2)?.as_str(), captures.get(3)?.as_str())
                    };

                    let size = size_str.parse().ok()?;

                    let elem_type = if pattern_idx == 2 {
                        "function_pointer"
                    } else if pattern_idx == 1 {
                        "unknown"
                    } else if captures.len() == 4 {
                        captures.get(1)?.as_str()
                    } else {
                        "unknown"
                    };

                    return Some(BufferInfo {
                        name: var_name.to_string(),
                        size: BufferSize::Static(size),
                        element_type: elem_type.to_string(),
                        allocation_line: line_idx + 1,
                    });
                }
            }
        }
        None
    }

    /// Parse malloc allocations and extract size information
    fn parse_malloc_allocation(&self, line: &str, line_idx: usize) -> Option<BufferInfo> {
        if !line.contains("malloc") || !line.contains("=") {
            return None;
        }

        let var_name = self.extract_variable_name_from_malloc(line)?;

        // Extract malloc argument
        let malloc_start = line.find("malloc(")?;
        let args_start = malloc_start + 7;
        let paren_end = line[args_start..].find(')')?;
        let malloc_args = &line[args_start..args_start + paren_end];

        let size = self.calculate_malloc_size(malloc_args)?;

        Some(BufferInfo {
            name: var_name,
            size,
            element_type: "unknown".to_string(),
            allocation_line: line_idx + 1,
        })
    }

    /// Parse calloc allocations
    fn parse_calloc_allocation(&self, line: &str, line_idx: usize) -> Option<BufferInfo> {
        if !line.contains("calloc") || !line.contains("=") {
            return None;
        }

        let var_name = self.extract_variable_name_from_malloc(line)?;

        // Extract calloc arguments: calloc(count, size)
        let calloc_start = line.find("calloc(")?;
        let args_start = calloc_start + 7;
        let paren_end = line[args_start..].find(')')?;
        let calloc_args = &line[args_start..args_start + paren_end];

        // Parse count and size
        let parts: Vec<&str> = calloc_args.split(',').collect();
        if parts.len() == 2 {
            let count = self.extract_numeric_value(parts[0].trim());
            let size_expr = parts[1].trim();

            if let Some(count_val) = count {
                if self.extract_sizeof_value(size_expr).is_some() {
                    // Store element count, not byte count
                    return Some(BufferInfo {
                        name: var_name,
                        size: BufferSize::DynamicCalculated(count_val),
                        element_type: "unknown".to_string(),
                        allocation_line: line_idx + 1,
                    });
                }
            }
        }

        None
    }

    /// Parse realloc allocations and extract size information
    fn parse_realloc_allocation(&self, line: &str, line_idx: usize) -> Option<BufferInfo> {
        if !line.contains("realloc") || !line.contains("=") {
            return None;
        }

        let var_name = self.extract_variable_name_from_malloc(line)?;

        // Extract realloc arguments: realloc(ptr, size)
        let realloc_start = line.find("realloc(")?;
        let args_start = realloc_start + 8;
        let paren_end = line[args_start..].find(')')?;
        let realloc_args = &line[args_start..args_start + paren_end];

        // Parse the second argument (new size)
        let parts: Vec<&str> = realloc_args.split(',').collect();
        if parts.len() == 2 {
            let new_size_expr = parts[1].trim();
            let size = self.calculate_malloc_size(new_size_expr)?;

            return Some(BufferInfo {
                name: var_name,
                size,
                element_type: "unknown".to_string(),
                allocation_line: line_idx + 1,
            });
        }

        None
    }

    /// Extract variable name from malloc/calloc assignment
    fn extract_variable_name_from_malloc(&self, line: &str) -> Option<String> {
        // Match patterns like: type *var = malloc(...) or var = malloc(...)
        let eq_pos = line.find('=')?;
        let lhs = &line[..eq_pos];

        // Extract the last identifier before =
        let tokens: Vec<&str> = lhs.split_whitespace().collect();
        let var_with_stars = tokens.last()?;

        // Remove leading * characters
        let var_name = var_with_stars.trim_start_matches('*').trim();

        Some(var_name.to_string())
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
            let parts: Vec<&str> = trimmed.split('*').collect();
            if parts.len() == 2 {
                let count = self.extract_numeric_value(parts[0].trim());
                let _sizeof_val = self.extract_sizeof_value(parts[1].trim());

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

    /// Extract numeric value from string
    fn extract_numeric_value(&self, s: &str) -> Option<usize> {
        s.trim().parse().ok()
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
        let identifier = text.split(|c: char| !c.is_alphanumeric() && c != '_')
            .find(|s| !s.is_empty())?;

        Some(identifier.to_string())
    }

    /// Get the subscript index value (constant or variable)
    fn get_subscript_index_value(&self, node: &Node, source: &str) -> Option<IndexValue> {
        let index_node = get_subscript_index(node)?;
        let index_text = &source[index_node.start_byte()..index_node.end_byte()];

        // Try to parse as simple constant (now supports negative indices)
        if let Ok(const_val) = index_text.trim().parse::<isize>() {
            return Some(IndexValue::Constant(const_val));
        }

        // Try to evaluate as expression
        if let Some(eval_val) = self.evaluate_index_expression(index_text, source) {
            return Some(IndexValue::Expression(index_text.to_string(), Some(eval_val)));
        }

        // Check if it's an arithmetic expression with variable
        if self.is_arithmetic_expression(index_text) {
            return Some(IndexValue::Expression(index_text.to_string(), None));
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

    /// Enhanced bounds check that considers actual buffer size
    fn has_proper_bounds_check(&self, node: &Node, source: &str, buffer_size: usize) -> bool {
        // Check loop-based bounds checking
        if let Some(for_node) = self.find_containing_for_loop(node) {
            if self.check_for_loop_bounds_against_size(&for_node, source, buffer_size) {
                return true;
            }
        }

        // Check conditional bounds checking
        if let Some(if_node) = self.find_containing_if_statement(node) {
            if self.check_if_bounds_against_size(&if_node, source, buffer_size) {
                return true;
            }
        }

        false
    }

    /// Check if there's any form of dynamic bounds checking
    fn has_dynamic_bounds_check(&self, node: &Node, source: &str) -> bool {
        has_bounds_check(node, "", source)
    }

    /// Find containing for loop
    fn find_containing_for_loop<'a>(&self, node: &Node<'a>) -> Option<Node<'a>> {
        let mut current = node.parent();
        while let Some(n) = current {
            if n.kind() == "for_statement" {
                return Some(n);
            }
            current = n.parent();
        }
        None
    }

    /// Find containing if statement
    fn find_containing_if_statement<'a>(&self, node: &Node<'a>) -> Option<Node<'a>> {
        let mut current = node.parent();
        while let Some(n) = current {
            if n.kind() == "if_statement" {
                return Some(n);
            }
            current = n.parent();
        }
        None
    }

    /// Check for loop bounds against specific buffer size
    fn check_for_loop_bounds_against_size(&self, for_node: &Node, source: &str, size: usize) -> bool {
        let loop_text = &source[for_node.start_byte()..for_node.end_byte()];

        // Look for patterns like: i < SIZE or i < 10
        if loop_text.contains(&format!("< {}", size)) {
            return true;
        }

        // Generic bounds checking
        check_for_loop_bounds(for_node, "", source)
    }

    /// Check if statement bounds against specific buffer size
    fn check_if_bounds_against_size(&self, if_node: &Node, source: &str, size: usize) -> bool {
        let if_text = &source[if_node.start_byte()..if_node.end_byte()];

        // Look for patterns like: if (idx < SIZE)
        if if_text.contains(&format!("< {}", size)) {
            return true;
        }

        false
    }

    /// Create a violation record
    fn create_violation(&self, node: &Node, array_name: &str, buffer_info: &BufferInfo, message: &str) -> RuleViolation {
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
                           message, array_name, size_info, buffer_info.allocation_line),
            file_path: String::new(),
            line: start_point.row + 1,
            column: start_point.column + 1,
            suggestion: Some("Ensure array access is within allocated bounds. Add explicit bounds checking.".to_string()),
        }
    }

    /// Check array subscript expressions with buffer size analysis
    fn check_array_subscript(&self, node: &Node, source: &str, buffers: &HashMap<String, BufferInfo>, aliases: &HashMap<String, PointerAlias>) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        if let Some(array_name) = self.get_array_name_from_subscript(node, source) {
            if let Some(index) = self.get_subscript_index_value(node, source) {
                // Try to resolve alias first
                let (actual_buffer_name, element_size_bytes) = if let Some(alias) = aliases.get(&array_name) {
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
                                IndexValue::Variable(_) => {
                                    // Variable index - check for bounds checking
                                    !self.has_proper_bounds_check(node, source, effective_size)
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
                            IndexValue::Constant(idx) =>
                                format!("Out-of-bounds array access at index {}", idx),
                            IndexValue::Expression(ref expr, Some(eval_idx)) =>
                                format!("Out-of-bounds array access: '{}' evaluates to {}", expr, eval_idx),
                            IndexValue::Expression(ref expr, None) =>
                                format!("Potentially unsafe array access with expression '{}'", expr),
                            IndexValue::Variable(ref var) =>
                                format!("Potentially unsafe array access with variable index '{}'", var),
                            IndexValue::Unknown =>
                                "Potentially unsafe array access".to_string(),
                        };
                        violations.push(self.create_violation(node, &array_name, buffer_info, &msg));
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
    fn check_pointer_arithmetic(&self, node: &Node, source: &str, buffers: &HashMap<String, BufferInfo>, aliases: &HashMap<String, PointerAlias>) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        if let Some((ptr_name, offset)) = self.extract_pointer_arithmetic(node, source) {
            // Try to resolve alias first
            let (actual_buffer_name, element_size_bytes) = if let Some(alias) = aliases.get(&ptr_name) {
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
                                violations.push(self.create_violation(node, &ptr_name, buffer_info, &msg));
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
    fn extract_pointer_arithmetic(&self, node: &Node, source: &str) -> Option<(String, OffsetValue)> {
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

    /// Check pointer dereference for bounds violations
    fn check_pointer_dereference(&self, node: &Node, source: &str, buffers: &HashMap<String, BufferInfo>, _aliases: &HashMap<String, PointerAlias>) -> Vec<RuleViolation> {
        let violations = Vec::new();

        // Get the pointer being dereferenced
        if let Some(ptr_node) = node.child(1) {
            let ptr_text = &source[ptr_node.start_byte()..ptr_node.end_byte()];
            let ptr_name = ptr_text.trim();

            // Check if this pointer has been moved beyond its bounds via pointer arithmetic
            // This requires tracking pointer modifications, which is complex
            // For now, we check if the pointer exists in our buffer tracking
            if buffers.contains_key(ptr_name) {
                // Check if there's surrounding pointer arithmetic that moved it out of bounds
                // This is a simplified check - full implementation would need data flow analysis
                if let Some(parent) = node.parent() {
                    let parent_text = &source[parent.start_byte()..parent.end_byte()];
                    if parent_text.contains("+=") && parent_text.contains(ptr_name) {
                        // Potential issue - but needs more sophisticated analysis
                        // Skip for now to avoid false positives
                    }
                }
            }
        }

        violations
    }
}

impl CertRule for Arr30C {
    fn rule_id(&self) -> &'static str {
        "ARR30-C"
    }

    fn description(&self) -> &'static str {
        "Do not form or use out-of-bounds pointers or array subscripts"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        // Analyze all buffer allocations once at root level
        if node.parent().is_none() {
            let buffer_info = self.analyze_buffer_allocations(source);
            let pointer_aliases = self.analyze_pointer_aliases(source, &buffer_info);
            self.check_with_buffer_info(node, source, &buffer_info, &pointer_aliases)
        } else {
            // This shouldn't happen as we control recursion, but handle gracefully
            Vec::new()
        }
    }
}

impl Arr30C {
    /// Internal recursive check function that carries buffer_info through the tree
    fn check_with_buffer_info(&self, node: &Node, source: &str, buffer_info: &HashMap<String, BufferInfo>, aliases: &HashMap<String, PointerAlias>) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Clone the maps to allow modification during traversal
        let mut local_buffers = buffer_info.clone();
        let mut local_aliases = aliases.clone();

        // Check multiple violation patterns BEFORE extracting declarations
        // This ensures we use the parent's context for checking this node
        match node.kind() {
            "subscript_expression" => {
                violations.extend(self.check_array_subscript(node, source, &local_buffers, &local_aliases));
            }
            "assignment_expression" => {
                if self.is_pointer_arithmetic_assignment(node, source) {
                    violations.extend(self.check_pointer_arithmetic(node, source, &local_buffers, &local_aliases));
                }
            }
            "pointer_expression" => {
                violations.extend(self.check_pointer_dereference(node, source, &local_buffers, &local_aliases));
            }
            "call_expression" => {
                violations.extend(self.check_dangerous_function_call(node, source, &local_buffers));
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
                        local_buffers.insert(new_buffer.name.clone(), new_buffer);
                    }
                    if let Some(new_alias) = self.extract_alias_from_declaration(&child, source, &local_buffers) {
                        local_aliases.insert(new_alias.alias_name.clone(), new_alias);
                    }
                }

                // Recursively check this child with the accumulated context
                violations.extend(self.check_with_buffer_info(&child, source, &local_buffers, &local_aliases));
            }
        }

        violations
    }

    /// Extract buffer information from a declaration AST node
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

    /// Extract buffer from init_declarator node (declarations with initializers)
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

            // Could be a typedef array - check parent declaration for type
            if let Some(parent) = node.parent() {
                if parent.kind() == "declaration" {
                    return self.check_typedef_declaration(&parent, var_name, source);
                }
            }
        }

        None
    }

    /// Extract buffer info from array_declarator
    fn extract_buffer_from_array_declarator(&self, node: &Node, source: &str) -> Option<BufferInfo> {
        // array_declarator has: declarator, "[", size, "]"
        let mut var_name: Option<String> = None;
        let mut size: Option<usize> = None;
        let mut size_expr: Option<String> = None;

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "identifier" => {
                        var_name = Some(source[child.start_byte()..child.end_byte()].to_string());
                    }
                    "number_literal" => {
                        let size_str = &source[child.start_byte()..child.end_byte()];
                        size = size_str.parse().ok();
                    }
                    "identifier" if i > 0 => {
                        // This is the size expression (VLA)
                        let expr = &source[child.start_byte()..child.end_byte()];
                        size_expr = Some(expr.to_string());
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

    /// Extract buffer from malloc/calloc call
    fn extract_buffer_from_malloc_call(&self, declarator: &Node, call_node: &Node, source: &str) -> Option<BufferInfo> {
        let var_name = if declarator.kind() == "pointer_declarator" {
            // Navigate to the identifier within pointer_declarator
            let mut found_name: Option<String> = None;
            for i in 0..declarator.child_count() {
                if let Some(child) = declarator.child(i) {
                    if child.kind() == "identifier" {
                        found_name = Some(source[child.start_byte()..child.end_byte()].to_string());
                        break;
                    }
                }
            }
            found_name?
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
                    return self.parse_malloc_arguments(func_name, &child, source, &var_name, call_node.start_position().row + 1);
                }
            }
        }

        None
    }

    /// Parse malloc/calloc arguments from argument_list node
    fn parse_malloc_arguments(&self, func_name: &str, arg_list: &Node, source: &str, var_name: &str, line: usize) -> Option<BufferInfo> {
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
    fn check_typedef_declaration(&self, decl_node: &Node, var_name: &str, source: &str) -> Option<BufferInfo> {
        // Get type from declaration
        for i in 0..decl_node.child_count() {
            if let Some(child) = decl_node.child(i) {
                if child.kind() == "type_identifier" {
                    let type_name = &source[child.start_byte()..child.end_byte()];

                    // Try to find typedef definition in source
                    // This is a simplified check - full implementation would cache typedefs
                    let typedef_pattern = format!(r"typedef\s+\w+\s+{}\s*\[\s*(\d+)\s*\]", regex::escape(type_name));
                    if let Ok(re) = regex::Regex::new(&typedef_pattern) {
                        if let Some(caps) = re.captures(source) {
                            if let Some(size_str) = caps.get(1) {
                                if let Ok(size) = size_str.as_str().parse::<usize>() {
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
                }
            }
        }
        None
    }

    /// Extract pointer alias from declaration AST node
    fn extract_alias_from_declaration(&self, node: &Node, source: &str, buffers: &HashMap<String, BufferInfo>) -> Option<PointerAlias> {
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
    fn extract_alias_from_init_declarator(&self, node: &Node, source: &str, buffers: &HashMap<String, BufferInfo>) -> Option<PointerAlias> {
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
                        let ptr_name = self.extract_identifier_from_declarator(&declarator, source)?;
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
    fn extract_alias_from_cast(&self, declarator: &Node, cast_node: &Node, source: &str, buffers: &HashMap<String, BufferInfo>) -> Option<PointerAlias> {
        let ptr_name = self.extract_identifier_from_declarator(declarator, source)?;

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
                                    cast_type = Some(&source[type_node.start_byte()..type_node.end_byte()]);
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

    /// Extract identifier name from declarator
    fn extract_identifier_from_declarator(&self, node: &Node, source: &str) -> Option<String> {
        match node.kind() {
            "identifier" => {
                Some(source[node.start_byte()..node.end_byte()].to_string())
            }
            "pointer_declarator" => {
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        if child.kind() == "identifier" {
                            return Some(source[child.start_byte()..child.end_byte()].to_string());
                        }
                    }
                }
                None
            }
            _ => None,
        }
    }

    /// Check for dangerous library function calls that can cause buffer overflows
    fn check_dangerous_function_call(&self, node: &Node, source: &str, buffers: &HashMap<String, BufferInfo>) -> Vec<RuleViolation> {
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

    /// Check strcpy calls for buffer overflow potential
    fn check_strcpy(&self, node: &Node, source: &str, buffers: &HashMap<String, BufferInfo>) -> Vec<RuleViolation> {
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
                        if let BufferSize::Static(dest_s) | BufferSize::DynamicCalculated(dest_s) = dest_info.size {
                            if src_s > dest_s {
                                violations.push(self.create_library_violation(
                                    node,
                                    dest_name,
                                    dest_info,
                                    &format!("strcpy may overflow: source size {} > destination size {}", src_s, dest_s)
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
                            "strcpy with unknown source size can cause buffer overflow"
                        ));
                    }
                }
            }
        }

        violations
    }

    /// Check strcat calls for buffer overflow potential
    fn check_strcat(&self, node: &Node, source: &str, buffers: &HashMap<String, BufferInfo>) -> Vec<RuleViolation> {
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
                        "strcat can cause buffer overflow without length checks"
                    ));
                }
            }
        }

        violations
    }

    /// Check memcpy/memmove calls for buffer overflow potential
    fn check_memcpy(&self, node: &Node, source: &str, buffers: &HashMap<String, BufferInfo>) -> Vec<RuleViolation> {
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
                        if let BufferSize::Static(dest_s) | BufferSize::DynamicCalculated(dest_s) = dest_info.size {
                            if c > dest_s {
                                violations.push(self.create_library_violation(
                                    node,
                                    dest_name,
                                    dest_info,
                                    &format!("memcpy copies {} bytes into {}-byte buffer", c, dest_s)
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
    fn check_sprintf(&self, node: &Node, source: &str, buffers: &HashMap<String, BufferInfo>) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        if let Some(args) = self.get_function_arguments(node, source) {
            if !args.is_empty() {
                let dest_name = args[0].trim();

                if let Some(dest_info) = buffers.get(dest_name) {
                    violations.push(self.create_library_violation(
                        node,
                        dest_name,
                        dest_info,
                        "sprintf can cause buffer overflow; use snprintf instead"
                    ));
                }
            }
        }

        violations
    }

    /// Check gets calls (always unsafe)
    fn check_gets(&self, node: &Node, source: &str, buffers: &HashMap<String, BufferInfo>) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        if let Some(args) = self.get_function_arguments(node, source) {
            if !args.is_empty() {
                let dest_name = args[0].trim();

                if let Some(dest_info) = buffers.get(dest_name) {
                    violations.push(self.create_library_violation(
                        node,
                        dest_name,
                        dest_info,
                        "gets is inherently unsafe and can cause buffer overflow"
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
                            if arg_node.kind() != "(" && arg_node.kind() != ")" && arg_node.kind() != "," {
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
    fn create_library_violation(&self, node: &Node, buffer_name: &str, buffer_info: &BufferInfo, message: &str) -> RuleViolation {
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
        }
    }
}

// Helper functions from original implementation (kept for compatibility)

fn get_subscript_array<'a>(node: &'a Node<'a>) -> Option<Node<'a>> {
    node.child(0)
}

fn get_subscript_index<'a>(node: &'a Node<'a>) -> Option<Node<'a>> {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if child.kind() != "[" && child.kind() != "]" && i > 0 {
                return Some(child);
            }
        }
    }
    None
}

fn has_bounds_check(subscript_node: &Node, index_text: &str, source: &str) -> bool {
    if has_loop_bounds_check(subscript_node, index_text, source) {
        return true;
    }

    if has_conditional_bounds_check(subscript_node, index_text, source) {
        return true;
    }

    if has_function_bounds_check(subscript_node, index_text, source) {
        return true;
    }

    false
}

fn has_loop_bounds_check(subscript_node: &Node, index_text: &str, source: &str) -> bool {
    let mut current = subscript_node.parent();

    while let Some(node) = current {
        if node.kind() == "for_statement" {
            return check_for_loop_bounds(&node, index_text, source);
        }
        current = node.parent();
    }
    false
}

fn check_for_loop_bounds(for_node: &Node, index_text: &str, source: &str) -> bool {
    for i in 0..for_node.child_count() {
        if let Some(child) = for_node.child(i) {
            if child.kind() == "binary_expression" || child.kind() == "comparison_expression" {
                let condition_text = &source[child.start_byte()..child.end_byte()];
                if condition_contains_safe_bounds(condition_text, index_text) {
                    return true;
                }
            }
        }
    }

    for i in 0..for_node.child_count() {
        if let Some(child) = for_node.child(i) {
            if child.kind() == "parenthesized_expression" {
                for j in 0..child.child_count() {
                    if let Some(grandchild) = child.child(j) {
                        if grandchild.kind() == "binary_expression" || grandchild.kind() == "comparison_expression" {
                            let condition_text = &source[grandchild.start_byte()..grandchild.end_byte()];
                            if condition_contains_safe_bounds(condition_text, index_text) {
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

fn condition_contains_safe_bounds(condition_text: &str, index_text: &str) -> bool {
    let trimmed_index = index_text.trim();

    if condition_text.contains(&format!("{} <", trimmed_index)) {
        return !condition_text.contains(&format!("{} <=", trimmed_index));
    }

    if condition_text.contains(&format!("> {}", trimmed_index)) {
        return !condition_text.contains(&format!(">= {}", trimmed_index));
    }
    false
}

fn has_conditional_bounds_check(subscript_node: &Node, index_text: &str, source: &str) -> bool {
    let mut current = subscript_node.parent();

    while let Some(node) = current {
        if node.kind() == "if_statement" {
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() == "parenthesized_expression" || child.kind() == "binary_expression" {
                        let condition_text = &source[child.start_byte()..child.end_byte()];
                        if condition_contains_safe_bounds(condition_text, index_text) {
                            return true;
                        }
                    }
                }
            }
        }
        current = node.parent();
    }

    false
}

fn has_function_bounds_check(subscript_node: &Node, _index_text: &str, source: &str) -> bool {
    let mut current = subscript_node.parent();

    while let Some(node) = current {
        if node.kind() == "function_definition" {
            let function_text = &source[node.start_byte()..node.end_byte()];
            if function_text.contains("size") || function_text.contains("length") || function_text.contains("count") {
                return true;
            }
        }
        current = node.parent();
    }

    false
}

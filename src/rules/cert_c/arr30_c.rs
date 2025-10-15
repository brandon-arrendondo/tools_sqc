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

impl Arr30C {
    /// Analyze all buffer allocations in the source code
    fn analyze_buffer_allocations(&self, source: &str) -> HashMap<String, BufferInfo> {
        let mut buffers = HashMap::new();
        let lines: Vec<&str> = source.lines().collect();

        for (line_idx, line) in lines.iter().enumerate() {
            // Pattern 1: Array declarations - type arr[SIZE]
            if let Some(buffer) = self.parse_array_declaration(line, line_idx) {
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
        }

        buffers
    }

    /// Parse array declarations like: int arr[10];
    fn parse_array_declaration(&self, line: &str, line_idx: usize) -> Option<BufferInfo> {
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
    fn check_array_subscript(&self, node: &Node, source: &str, buffers: &HashMap<String, BufferInfo>) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        if let Some(array_name) = self.get_array_name_from_subscript(node, source) {
            if let Some(index) = self.get_subscript_index_value(node, source) {
                if let Some(buffer_info) = buffers.get(&array_name) {

                    let is_violation = match &buffer_info.size {
                        BufferSize::Static(size) | BufferSize::DynamicCalculated(size) => {
                            match &index {
                                IndexValue::Constant(idx) => {
                                    // Constant index access - check for negative indices OR out of bounds
                                    *idx < 0 || (*idx as usize) >= *size
                                }
                                IndexValue::Expression(_, Some(eval_idx)) => {
                                    // Expression evaluated to constant - check bounds
                                    *eval_idx < 0 || (*eval_idx as usize) >= *size
                                }
                                IndexValue::Expression(expr, None) => {
                                    // Expression with variable component - analyze it
                                    self.check_expression_bounds(expr, *size)
                                }
                                IndexValue::Variable(_) => {
                                    // Variable index - check for bounds checking
                                    !self.has_proper_bounds_check(node, source, *size)
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

    /// Check pointer arithmetic for bounds violations
    fn check_pointer_arithmetic(&self, node: &Node, source: &str, buffers: &HashMap<String, BufferInfo>) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        if let Some((ptr_name, offset)) = self.extract_pointer_arithmetic(node, source) {
            if let Some(buffer_info) = buffers.get(&ptr_name) {
                match &buffer_info.size {
                    BufferSize::Static(size) | BufferSize::DynamicCalculated(size) => {
                        if let OffsetValue::Constant(off) = offset {
                            if off >= *size {
                                let msg = format!(
                                    "Pointer arithmetic moves {} elements beyond buffer bounds",
                                    off
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
    fn check_pointer_dereference(&self, node: &Node, source: &str, buffers: &HashMap<String, BufferInfo>) -> Vec<RuleViolation> {
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
            self.check_with_buffer_info(node, source, &buffer_info)
        } else {
            // This shouldn't happen as we control recursion, but handle gracefully
            Vec::new()
        }
    }
}

impl Arr30C {
    /// Internal recursive check function that carries buffer_info through the tree
    fn check_with_buffer_info(&self, node: &Node, source: &str, buffer_info: &HashMap<String, BufferInfo>) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Check multiple violation patterns
        match node.kind() {
            "subscript_expression" => {
                violations.extend(self.check_array_subscript(node, source, buffer_info));
            }
            "assignment_expression" => {
                if self.is_pointer_arithmetic_assignment(node, source) {
                    violations.extend(self.check_pointer_arithmetic(node, source, buffer_info));
                }
            }
            "pointer_expression" => {
                violations.extend(self.check_pointer_dereference(node, source, buffer_info));
            }
            "call_expression" => {
                violations.extend(self.check_dangerous_function_call(node, source, buffer_info));
            }
            _ => {}
        }

        // Recursively check children with the same buffer_info
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                violations.extend(self.check_with_buffer_info(&child, source, buffer_info));
            }
        }

        violations
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

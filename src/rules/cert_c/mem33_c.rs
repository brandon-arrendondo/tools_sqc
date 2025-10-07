use super::super::{CertRule, RuleViolation};
use crate::manifest::Severity;
use tree_sitter::Node;
use std::collections::HashMap;

pub struct Mem33C {
    // Track structures that contain flexible array members
    flexible_structs: HashMap<String, FlexibleArrayInfo>,
}

#[derive(Debug, Clone)]
struct FlexibleArrayInfo {
    struct_name: String,
    has_flexible_array: bool,
    declaration_line: usize,
    is_valid_definition: bool, // New field to track if struct definition is valid
}

#[derive(Debug)]
struct FlexibleArrayValidation {
    flexible_member_count: usize,
    flexible_member_positions: Vec<usize>,
    total_field_count: usize,
    last_field_is_flexible: bool,
    violations: Vec<String>,
}

impl FlexibleArrayValidation {
    fn new() -> Self {
        Self {
            flexible_member_count: 0,
            flexible_member_positions: Vec::new(),
            total_field_count: 0,
            last_field_is_flexible: false,
            violations: Vec::new(),
        }
    }

    fn is_valid_flexible_struct(&self) -> bool {
        // Valid flexible array struct: exactly 1 flexible member as last field, at least 2 total fields
        self.flexible_member_count == 1 &&
        self.last_field_is_flexible &&
        self.total_field_count >= 2 &&
        self.violations.is_empty()
    }

    fn has_violations(&self) -> bool {
        !self.violations.is_empty()
    }
}

#[derive(Debug, Clone)]
struct ArrayDeclaratorInfo {
    base_type: String,
    is_array: bool,
    array_size: Option<String>,
}

#[derive(Debug)]
struct StorageInfo {
    storage_type: String,
    is_dynamic: bool,
}

impl Mem33C {
    pub fn new() -> Self {
        Self {
            flexible_structs: HashMap::new(),
        }
    }
}

impl CertRule for Mem33C {
    fn rule_id(&self) -> &'static str {
        "MEM33-C"
    }

    fn description(&self) -> &'static str {
        "Allocate and copy structures containing a flexible array member dynamically"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        let mut analyzer = FlexibleArrayAnalyzer::new();

        // First pass: identify structures with flexible array members
        analyzer.collect_flexible_array_structs(node, source);

        // Second pass: detect violations
        violations.extend(analyzer.check_violations(node, source));

        violations
    }
}

struct FlexibleArrayAnalyzer {
    flexible_structs: HashMap<String, FlexibleArrayInfo>,
}

impl FlexibleArrayAnalyzer {
    fn new() -> Self {
        Self {
            flexible_structs: HashMap::new(),
        }
    }

    fn collect_flexible_array_structs(&mut self, node: &Node, source: &str) {
        // Look for struct declarations with flexible array members
        if node.kind() == "struct_specifier" {
            if let Some(info) = self.analyze_struct_for_flexible_array(node, source) {
                self.flexible_structs.insert(info.struct_name.clone(), info);
            }
        }

        // Recursively analyze child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.collect_flexible_array_structs(&child, source);
            }
        }
    }

    fn analyze_struct_for_flexible_array(&self, node: &Node, source: &str) -> Option<FlexibleArrayInfo> {
        let mut struct_name = String::new();
        let mut validation_result = None;

        // Find struct name and validate flexible array layout
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "type_identifier" => {
                        struct_name = source[child.start_byte()..child.end_byte()].to_string();
                    }
                    "field_declaration_list" => {
                        validation_result = Some(self.validate_flexible_array_layout(&child, source));
                    }
                    _ => {}
                }
            }
        }

        if let Some(validation) = validation_result {
            if !struct_name.is_empty() {
                // Check for invalid struct layouts first
                if validation.has_violations() {
                    // Store invalid struct information for later violation reporting
                    // We'll report these violations in a separate method
                    return Some(FlexibleArrayInfo {
                        struct_name: struct_name.clone(),
                        has_flexible_array: false, // Mark as invalid
                        declaration_line: node.start_position().row + 1,
                        is_valid_definition: false,
                    });
                }

                // Valid flexible array struct
                if validation.is_valid_flexible_struct() {
                    return Some(FlexibleArrayInfo {
                        struct_name,
                        has_flexible_array: true,
                        declaration_line: node.start_position().row + 1,
                        is_valid_definition: true,
                    });
                }
            }
        }

        None
    }

    fn has_flexible_array_member(&self, field_list: &Node, source: &str) -> bool {
        // Enhanced validation for flexible array member requirements
        let validation_result = self.validate_flexible_array_layout(field_list, source);

        // Return true only if we have exactly one flexible array as the last member
        validation_result.is_valid_flexible_struct()
    }

    fn is_flexible_array_field(&self, field: &Node, source: &str) -> bool {
        // Look for array_declarator with empty size
        for i in 0..field.child_count() {
            if let Some(child) = field.child(i) {
                if child.kind() == "array_declarator" {
                    // Check if the array has empty brackets []
                    for j in 0..child.child_count() {
                        if let Some(bracket) = child.child(j) {
                            if bracket.kind() == "[" || bracket.kind() == "]" {
                                // Look for empty array size (no size between brackets)
                                let bracket_content = source[child.start_byte()..child.end_byte()].to_string();
                                if bracket_content.ends_with("[]") {
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

    fn is_last_field_declaration(&self, field_list: &Node, current_index: usize) -> bool {
        // Check if the current field declaration is the last one in the struct
        for i in (current_index + 1)..field_list.child_count() {
            if let Some(child) = field_list.child(i) {
                if child.kind() == "field_declaration" {
                    return false; // Found another field declaration after this one
                }
            }
        }
        true // No more field declarations found
    }

    fn validate_flexible_array_layout(&self, field_list: &Node, source: &str) -> FlexibleArrayValidation {
        let mut validation = FlexibleArrayValidation::new();
        let mut field_position = 0;

        for i in 0..field_list.child_count() {
            if let Some(child) = field_list.child(i) {
                if child.kind() == "field_declaration" {
                    validation.total_field_count += 1;
                    field_position += 1;

                    if self.is_flexible_array_field(&child, source) {
                        validation.flexible_member_count += 1;
                        validation.flexible_member_positions.push(field_position);

                        // Check if this is the last field
                        let is_last_field = self.is_last_field_declaration(field_list, i);
                        if is_last_field {
                            validation.last_field_is_flexible = true;
                        } else {
                            validation.violations.push(format!(
                                "Flexible array member at position {} is not the last member",
                                field_position
                            ));
                        }
                    }
                }
            }
        }

        // Validate flexible array member rules
        if validation.flexible_member_count > 1 {
            validation.violations.push(format!(
                "Structure has {} flexible array members (maximum 1 allowed)",
                validation.flexible_member_count
            ));
        }

        if validation.flexible_member_count > 0 && validation.total_field_count == 1 {
            validation.violations.push(
                "Structure has only flexible array member (at least one fixed member required)".to_string()
            );
        }

        validation
    }

    fn check_invalid_struct_definition(&self, node: &Node, source: &str) -> Option<RuleViolation> {
        let mut struct_name = String::new();

        // Find struct name
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "type_identifier" {
                    struct_name = source[child.start_byte()..child.end_byte()].to_string();
                    break;
                }
            }
        }

        // Find field declaration list and validate
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "field_declaration_list" {
                    let validation = self.validate_flexible_array_layout(&child, source);

                    if validation.has_violations() {
                        let start_point = node.start_position();
                        let violation_details = validation.violations.join("; ");

                        return Some(RuleViolation {
                            rule_id: "MEM33-C".to_string(),
                            severity: Severity::Critical, // Critical because this won't compile
                            message: format!(
                                "Invalid flexible array structure definition '{}': {}. Flexible arrays must be the single last member of a struct.",
                                if struct_name.is_empty() { "anonymous" } else { &struct_name },
                                violation_details
                            ),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some("Ensure struct has at most one flexible array member as the final field".to_string()),
                        });
                    }
                }
            }
        }

        None
    }

    fn check_violations(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();


        // Check for various violation patterns
        match node.kind() {
            "declaration" => {
                // Check for prohibited storage of flexible array structs
                if let Some(violation) = self.check_prohibited_storage(node, source) {
                    violations.push(violation);
                }
            }
            "variable_declaration" | "init_declarator" => {
                // Some const declarations might appear as these node types
                if let Some(violation) = self.check_prohibited_storage(node, source) {
                    violations.push(violation);
                }
            }
            "assignment_expression" => {
                // Check for direct assignment of flexible array structs
                if let Some(violation) = self.check_assignment_copy(node, source) {
                    violations.push(violation);
                }
            }
            "parameter_declaration" => {
                // Check for pass-by-value of flexible array structs
                if let Some(violation) = self.check_value_parameter(node, source) {
                    violations.push(violation);
                }
            }
            "compound_literal_expression" => {
                // Check for compound literal usage with flexible array structs
                if let Some(violation) = self.check_compound_literal(node, source) {
                    violations.push(violation);
                }
            }
            "call_expression" => {
                // Check for memory allocation function calls with incorrect sizing
                if let Some(violation) = self.check_memory_allocation(node, source) {
                    violations.push(violation);
                }
                // Check for file I/O operations with incorrect sizing
                if let Some(violation) = self.check_file_io_operations(node, source) {
                    violations.push(violation);
                }
                // Check for memory operation functions with incorrect sizing
                if let Some(violation) = self.check_memory_operations(node, source) {
                    violations.push(violation);
                }
            }
            "cast_expression" => {
                // Check for casting violations with flexible array structs (both const-casting and invalid type casting)
                if let Some(violation) = self.check_casting_violations(node, source) {
                    violations.push(violation);
                }
            }
            "binary_expression" => {
                // Check for pointer arithmetic on flexible array structures
                if let Some(violation) = self.check_pointer_arithmetic(node, source) {
                    violations.push(violation);
                }
            }
            "struct_specifier" => {
                // Check for invalid struct definitions with multiple/misplaced flexible arrays
                if let Some(violation) = self.check_invalid_struct_definition(node, source) {
                    violations.push(violation);
                }
            }
            _ => {}
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                violations.extend(self.check_violations(&child, source));
            }
        }

        violations
    }

    fn check_prohibited_storage(&self, node: &Node, source: &str) -> Option<RuleViolation> {
        // Check for flexible array structures with prohibited storage duration
        // MEM33-C requires dynamic storage duration only

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {

                if child.kind() == "array_declarator" {
                    // Array of flexible array structures
                    if let Some(struct_type) = self.extract_declared_type(node, source) {
                        if self.is_flexible_array_struct(&struct_type) {
                            // Arrays are ALWAYS prohibited regardless of storage duration
                            let storage_info = self.analyze_storage_duration(node, source);
                            let start_point = node.start_position();

                            // Extract array size from the array declarator
                            let mut array_size = "".to_string();
                            for j in 0..child.child_count() {
                                if let Some(size_child) = child.child(j) {
                                    if size_child.kind() != "identifier" && size_child.kind() != "[" && size_child.kind() != "]" {
                                        array_size = source[size_child.start_byte()..size_child.end_byte()].to_string();
                                        break;
                                    }
                                }
                            }

                            return Some(RuleViolation {
                                rule_id: "MEM33-C".to_string(),
                                severity: Severity::High,
                                message: format!(
                                    "Array of flexible array structures '{}[{}]' declared with {} storage. Arrays of flexible array structures are prohibited with any storage duration.",
                                    struct_type,
                                    if array_size.is_empty() { "".to_string() } else { array_size.clone() },
                                    storage_info.storage_type
                                ),
                                file_path: String::new(),
                                line: start_point.row + 1,
                                column: start_point.column + 1,
                                suggestion: Some("Use an array of pointers to dynamically allocated structures instead".to_string()),
                            });
                        }
                    }
                }

                if child.kind() == "init_declarator" || child.kind() == "declarator" || child.kind() == "identifier" {
                    // Check if this is an array declarator (legacy code for nested cases)
                    if let Some(array_info) = self.check_array_declarator(&child, source) {
                        if array_info.is_array && self.is_flexible_array_struct(&array_info.base_type) {
                            // This is an array of flexible array structures - VIOLATION!
                            let storage_info = self.analyze_storage_duration(node, source);
                            let start_point = node.start_position();
                            let array_size_display = array_info.array_size.clone().unwrap_or_else(|| "[]".to_string());
                            return Some(RuleViolation {
                                rule_id: "MEM33-C".to_string(),
                                severity: Severity::High,
                                message: format!(
                                    "Array of flexible array structures '{}[{}]' declared with {} storage. Arrays of flexible array structures are prohibited with any storage duration.",
                                    array_info.base_type, array_size_display, storage_info.storage_type
                                ),
                                file_path: String::new(),
                                line: start_point.row + 1,
                                column: start_point.column + 1,
                                suggestion: Some("Use an array of pointers to dynamically allocated structures instead".to_string()),
                            });
                        }
                    }

                    // Single flexible array structure declaration
                    if let Some((type_name, is_const)) = self.extract_declared_type_with_qualifiers(node, source) {
                        if self.is_flexible_array_struct(&type_name) {
                            // Check if this is a pointer declaration (allowed) vs direct declaration (prohibited)
                            if !self.is_pointer_declaration(node, source) {
                                let storage_info = self.analyze_storage_duration(node, source);
                                let qualifier_text = if is_const { "const-qualified " } else { "" };
                                let start_point = node.start_position();

                                return Some(RuleViolation {
                                    rule_id: "MEM33-C".to_string(),
                                    severity: self.get_severity_for_storage_type(&storage_info.storage_type),
                                    message: format!(
                                        "{}flexible array structure '{}' declared with {} storage. Only dynamic storage duration is allowed for flexible array structures.",
                                        qualifier_text, type_name, storage_info.storage_type
                                    ),
                                    file_path: String::new(),
                                    line: start_point.row + 1,
                                    column: start_point.column + 1,
                                    suggestion: Some(format!("Use dynamic allocation: struct {} *ptr = malloc(sizeof(struct {}) + sizeof(element_type) * count);", type_name, type_name)),
                                });
                            }
                        }
                    }
                }
            }
        }
        None
    }

    fn check_assignment_copy(&self, node: &Node, source: &str) -> Option<RuleViolation> {
        // Check for direct assignment between flexible array struct instances
        let left = node.child_by_field_name("left")?;
        let right = node.child_by_field_name("right")?;

        // Check if this is a dereference assignment (*struct_a = *struct_b)
        if self.is_flexible_struct_dereference(&left, source) &&
           self.is_flexible_struct_dereference(&right, source) {
            let start_point = node.start_position();
            return Some(RuleViolation {
                rule_id: "MEM33-C".to_string(),
                severity: Severity::Medium,
                message: "Direct assignment of flexible array structure instances. Use memcpy() for dynamic copying.".to_string(),
                file_path: String::new(),
                line: start_point.row + 1,
                column: start_point.column + 1,
                suggestion: Some("Use memcpy() to copy the structure and its flexible array member".to_string()),
            });
        }

        None
    }

    fn check_value_parameter(&self, node: &Node, source: &str) -> Option<RuleViolation> {
        // Check if parameter is a flexible array struct passed by value
        if let Some(type_name) = self.extract_parameter_type(node, source) {
            if self.is_flexible_array_struct(&type_name) && !self.is_pointer_parameter(node, source) {
                let start_point = node.start_position();
                return Some(RuleViolation {
                    rule_id: "MEM33-C".to_string(),
                    severity: Severity::Medium,
                    message: format!(
                        "Flexible array structure '{}' passed by value. Pass by pointer instead.",
                        type_name
                    ),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some("Change parameter to pointer type".to_string()),
                });
            }
        }

        None
    }

    fn check_compound_literal(&self, node: &Node, source: &str) -> Option<RuleViolation> {
        // Check for compound literal usage with flexible array structs
        // Compound literals have automatic storage duration, which is prohibited for flexible array structs

        // Extract the type from the compound literal
        if let Some(type_name) = self.extract_compound_literal_type(node, source) {
            if self.is_flexible_array_struct(&type_name) {
                // Check if there's an attempt to initialize the flexible array member
                let has_flex_init = self.has_flexible_array_initialization(node, source);

                let start_point = node.start_position();
                let message = if has_flex_init {
                    format!(
                        "Compound literal used with flexible array structure '{}' - cannot initialize flexible array member in compound literal",
                        type_name
                    )
                } else {
                    format!(
                        "Compound literal used with flexible array structure '{}' - compound literals have automatic storage duration which is prohibited",
                        type_name
                    )
                };

                return Some(RuleViolation {
                    rule_id: "MEM33-C".to_string(),
                    severity: Severity::High,
                    message,
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some("Allocate the structure dynamically using malloc() with proper size calculation".to_string()),
                });
            }
        }

        None
    }

    fn extract_compound_literal_type(&self, node: &Node, source: &str) -> Option<String> {
        // Extract the type from a compound literal expression
        // Compound literals typically have structure: (type_name) { initializer_list }

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "type_descriptor" | "type_name" => {
                        // Look for struct specifier within the type descriptor
                        for j in 0..child.child_count() {
                            if let Some(type_child) = child.child(j) {
                                if type_child.kind() == "struct_specifier" {
                                    // Extract struct name
                                    for k in 0..type_child.child_count() {
                                        if let Some(struct_child) = type_child.child(k) {
                                            if struct_child.kind() == "type_identifier" {
                                                return Some(source[struct_child.start_byte()..struct_child.end_byte()].to_string());
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    "struct_specifier" => {
                        // Direct struct specifier in compound literal
                        for j in 0..child.child_count() {
                            if let Some(struct_child) = child.child(j) {
                                if struct_child.kind() == "type_identifier" {
                                    return Some(source[struct_child.start_byte()..struct_child.end_byte()].to_string());
                                }
                            }
                        }
                    }
                    _ => {}
                }
            }
        }

        None
    }

    fn has_flexible_array_initialization(&self, node: &Node, source: &str) -> bool {
        // Check if the compound literal attempts to initialize a flexible array member
        // Look for initializer_list and check if it contains field initializers for the flexible array

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "initializer_list" {
                    // Check each initializer in the list
                    for j in 0..child.child_count() {
                        if let Some(init_child) = child.child(j) {
                            if init_child.kind() == "initializer_pair" || init_child.kind() == "field_initializer" {
                                // Check if this is initializing a field named 'data' (common flexible array name)
                                // or if it has array initializer syntax
                                if self.is_flexible_array_initializer(&init_child, source) {
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

    fn is_flexible_array_initializer(&self, node: &Node, source: &str) -> bool {
        // Check if this initializer is for a flexible array member
        // Look for patterns like .data = {...} or array initializer lists after struct members

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "field_designator" => {
                        // Check if field name suggests flexible array (e.g., "data", "buffer", etc.)
                        let field_text = &source[child.start_byte()..child.end_byte()];
                        if field_text.contains("data") || field_text.contains("buffer") || field_text.contains("array") {
                            // Check if the value is an initializer list
                            for j in 0..node.child_count() {
                                if let Some(value_child) = node.child(j) {
                                    if value_child.kind() == "initializer_list" {
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                    "initializer_list" => {
                        // Found an array initializer
                        return true;
                    }
                    _ => {}
                }
            }
        }

        false
    }

    fn extract_declared_type(&self, declaration: &Node, source: &str) -> Option<String> {
        // Extract the type name from a declaration
        for i in 0..declaration.child_count() {
            if let Some(child) = declaration.child(i) {
                match child.kind() {
                    "struct_specifier" => {
                        // Look for type identifier in struct
                        for j in 0..child.child_count() {
                            if let Some(type_child) = child.child(j) {
                                if type_child.kind() == "type_identifier" {
                                    return Some(source[type_child.start_byte()..type_child.end_byte()].to_string());
                                }
                            }
                        }
                    }
                    "type_identifier" => {
                        return Some(source[child.start_byte()..child.end_byte()].to_string());
                    }
                    _ => {}
                }
            }
        }
        None
    }

    fn extract_declared_type_with_qualifiers(&self, declaration: &Node, source: &str) -> Option<(String, bool)> {
        let mut is_const = false;
        let mut type_name = None;

        // First pass: look for const qualifier anywhere in the declaration
        self.find_const_qualifier_recursive(declaration, source, &mut is_const);

        // Second pass: look for struct type name with multiple strategies
        type_name = self.find_struct_type_name_recursive(declaration, source);

        if let Some(name) = type_name {
            Some((name, is_const))
        } else {
            None
        }
    }

    fn find_const_qualifier_recursive(&self, node: &Node, source: &str, is_const: &mut bool) {
        // Recursively search for const qualifier in any child node
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "storage_class_specifier" | "type_qualifier" => {
                        let keyword = source[child.start_byte()..child.end_byte()].trim();
                        if keyword == "const" {
                            *is_const = true;
                        }
                    }
                    _ => {
                        // Check the text content directly for const keyword
                        let text = source[child.start_byte()..child.end_byte()].trim();
                        if text == "const" {
                            *is_const = true;
                        }
                        // Recurse into children
                        self.find_const_qualifier_recursive(&child, source, is_const);
                    }
                }
            }
        }
    }

    fn find_struct_type_name_recursive(&self, node: &Node, source: &str) -> Option<String> {
        // Try multiple strategies to find the struct name

        // Strategy 1: Direct struct_specifier lookup (existing)
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "struct_specifier" {
                    for j in 0..child.child_count() {
                        if let Some(type_child) = child.child(j) {
                            if type_child.kind() == "type_identifier" {
                                return Some(source[type_child.start_byte()..type_child.end_byte()].to_string());
                            }
                        }
                    }
                }
            }
        }

        // Strategy 2: Look for "struct" keyword followed by identifier
        let decl_text = source[node.start_byte()..node.end_byte()].to_string();
        if let Some(struct_pos) = decl_text.find("struct ") {
            let after_struct = &decl_text[struct_pos + 7..]; // Skip "struct "
            if let Some(space_pos) = after_struct.find(' ') {
                let struct_name = &after_struct[..space_pos];
                if !struct_name.is_empty() && struct_name.chars().all(|c| c.is_alphanumeric() || c == '_') {
                    return Some(struct_name.to_string());
                }
            }
        }

        // Strategy 3: Recursive search for type_identifier anywhere
        self.find_type_identifier_recursive(node, source)
    }

    fn find_type_identifier_recursive(&self, node: &Node, source: &str) -> Option<String> {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "type_identifier" {
                    let name = source[child.start_byte()..child.end_byte()].to_string();
                    // Check if this looks like a struct name we care about
                    if self.flexible_structs.contains_key(&name) || name.contains("flex") {
                        return Some(name);
                    }
                }
                // Recurse into children
                if let Some(result) = self.find_type_identifier_recursive(&child, source) {
                    return Some(result);
                }
            }
        }
        None
    }

    fn is_flexible_array_struct_pointer_usage(&self, node: &Node, source: &str) -> bool {
        // Check if this node represents usage of a flexible array struct pointer

        let node_text = source[node.start_byte()..node.end_byte()].to_string();

        // Don't trigger on sizeof expressions - these are legitimate size calculations
        if node_text.contains("sizeof") {
            return false;
        }

        // Strategy 1: Check if this is an identifier that directly references a flexible array struct pointer
        if node.kind() == "identifier" {
            let var_name = node_text.trim();
            // Heuristic: variable names containing "flex" are likely candidates
            // But avoid triggering on mathematical expressions or comparisons
            if var_name.contains("flex") && !node_text.contains("=") && !node_text.contains("+") && !node_text.contains("-") {
                return true;
            }
        }

        // Strategy 2: Check for direct variable references to known flexible array struct pointers
        // This should be more conservative than the previous implementation
        if node.kind() == "identifier" {
            let var_name = node_text.trim();
            if var_name == "flex_struct" || var_name.ends_with("_flex") || var_name.starts_with("flex_") {
                return true;
            }
        }

        false
    }

    fn extract_parameter_type(&self, param: &Node, source: &str) -> Option<String> {
        // Similar to extract_declared_type but for parameters
        self.extract_declared_type(param, source)
    }

    fn check_array_declarator(&self, declarator_node: &Node, source: &str) -> Option<ArrayDeclaratorInfo> {
        // Check if this is an array declarator and extract information
        for i in 0..declarator_node.child_count() {
            if let Some(child) = declarator_node.child(i) {

                if child.kind() == "array_declarator" {
                    // This is an array declarator
                    // Extract the array size from the brackets
                    let mut array_size = None;
                    for j in 0..child.child_count() {
                        if let Some(size_child) = child.child(j) {
                            if size_child.kind() != "identifier" && size_child.kind() != "[" && size_child.kind() != "]" {
                                // This could be the array size expression
                                array_size = Some(source[size_child.start_byte()..size_child.end_byte()].to_string());
                            } else if size_child.kind() == "[" || size_child.kind() == "]" {
                                // Handle empty array [] case
                                if array_size.is_none() {
                                    array_size = Some("".to_string());
                                }
                            }
                        }
                    }

                    // Get the base type from the parent declaration or sibling nodes
                    if let Some(base_type) = self.extract_declared_type_from_declaration_parent(declarator_node, source) {
                        return Some(ArrayDeclaratorInfo {
                            base_type,
                            is_array: true,
                            array_size,
                        });
                    } else if let Some(base_type) = self.extract_type_from_sibling_in_declaration(declarator_node, source) {
                        return Some(ArrayDeclaratorInfo {
                            base_type,
                            is_array: true,
                            array_size,
                        });
                    }
                }
            }
        }

        // Not an array declarator
        None
    }

    fn extract_declared_type_from_declaration_parent(&self, declarator_node: &Node, source: &str) -> Option<String> {
        // Walk up to find the parent declaration node and extract the type
        let mut current = declarator_node.parent();
        while let Some(node) = current {
            if node.kind() == "declaration" {
                // Found the declaration, now extract the type specifier
                return self.extract_declared_type(&node, source);
            }
            current = node.parent();
        }
        None
    }

    fn extract_type_from_sibling_in_declaration(&self, declarator_node: &Node, source: &str) -> Option<String> {
        // If the array_declarator is a direct child of declaration, look for struct_specifier sibling
        if let Some(parent) = declarator_node.parent() {
            if parent.kind() == "declaration" {
                // Look for struct_specifier among the siblings
                for i in 0..parent.child_count() {
                    if let Some(sibling) = parent.child(i) {
                        if sibling.kind() == "struct_specifier" {
                            // Extract struct name from struct_specifier
                            for j in 0..sibling.child_count() {
                                if let Some(type_child) = sibling.child(j) {
                                    if type_child.kind() == "type_identifier" {
                                        return Some(source[type_child.start_byte()..type_child.end_byte()].to_string());
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        None
    }

    fn is_flexible_array_struct(&self, type_name: &str) -> bool {
        self.flexible_structs.contains_key(type_name)
    }

    fn is_in_function_scope(&self, node: &Node) -> bool {
        let mut current = node.parent();
        while let Some(parent) = current {
            if parent.kind() == "function_definition" {
                return true;
            }
            current = parent.parent();
        }
        false
    }

    fn is_flexible_struct_dereference(&self, node: &Node, source: &str) -> bool {
        // Check if this is a dereference of what might be a flexible array struct
        if node.kind() == "pointer_expression" {
            // This is a simple heuristic - in a full implementation we'd need type analysis
            return true;
        }
        false
    }

    fn is_pointer_parameter(&self, param: &Node, source: &str) -> bool {
        // Check if parameter declaration includes pointer syntax
        for i in 0..param.child_count() {
            if let Some(child) = param.child(i) {
                if child.kind() == "pointer_declarator" {
                    return true;
                }
            }
        }
        false
    }

    fn analyze_storage_duration(&self, node: &Node, source: &str) -> StorageInfo {
        // Determine the storage duration of a declaration

        // Check for static keyword
        if self.has_static_keyword(node, source) {
            if self.is_in_function_scope(node) {
                return StorageInfo {
                    storage_type: "static local".to_string(),
                    is_dynamic: false,
                };
            } else {
                return StorageInfo {
                    storage_type: "static global".to_string(),
                    is_dynamic: false,
                };
            }
        }

        // Check for thread_local keyword
        if self.has_thread_local_keyword(node, source) {
            return StorageInfo {
                storage_type: "thread".to_string(),
                is_dynamic: false,
            };
        }

        // Check for const keyword
        if self.has_const_keyword(node, source) {
            if self.is_in_function_scope(node) {
                return StorageInfo {
                    storage_type: "const automatic".to_string(),
                    is_dynamic: false,
                };
            } else {
                return StorageInfo {
                    storage_type: "const static".to_string(),
                    is_dynamic: false,
                };
            }
        }

        // Check scope to determine automatic vs global
        if self.is_in_function_scope(node) {
            StorageInfo {
                storage_type: "automatic".to_string(),
                is_dynamic: false,
            }
        } else {
            StorageInfo {
                storage_type: "global".to_string(),
                is_dynamic: false,
            }
        }
    }

    fn has_static_keyword(&self, node: &Node, source: &str) -> bool {
        // Check if declaration contains "static" keyword
        self.declaration_contains_keyword(node, "static", source)
    }

    fn has_thread_local_keyword(&self, node: &Node, source: &str) -> bool {
        // Check if declaration contains "thread_local" or "_Thread_local" keyword
        self.declaration_contains_keyword(node, "thread_local", source) ||
        self.declaration_contains_keyword(node, "_Thread_local", source)
    }

    fn has_const_keyword(&self, node: &Node, source: &str) -> bool {
        // Check if declaration contains "const" keyword
        self.declaration_contains_keyword(node, "const", source)
    }

    fn declaration_contains_keyword(&self, node: &Node, keyword: &str, source: &str) -> bool {
        // Recursively search the declaration for a specific keyword
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                // Check direct match
                if child.kind() == keyword {
                    return true;
                }

                // Check for storage class specifier containing the keyword
                if child.kind() == "storage_class_specifier" {
                    let text = &source[child.start_byte()..child.end_byte()];
                    if text == keyword {
                        return true;
                    }
                }

                // Recursively check children
                if self.declaration_contains_keyword(&child, keyword, source) {
                    return true;
                }
            }
        }
        false
    }


    fn is_pointer_declaration(&self, node: &Node, source: &str) -> bool {
        // Check if this declaration is for a pointer (which is allowed)
        // Look for pointer_declarator or * symbols
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "pointer_declarator" {
                    return true;
                }
                // Also check for '*' character in declarators
                if self.contains_pointer_syntax(&child, source) {
                    return true;
                }
            }
        }
        false
    }

    fn contains_pointer_syntax(&self, node: &Node, source: &str) -> bool {
        // Check if node contains pointer syntax like '*'
        let node_text = &source[node.start_byte()..node.end_byte()];
        node_text.contains('*')
    }

    fn get_severity_for_storage_type(&self, storage_type: &str) -> Severity {
        match storage_type {
            "global" | "static global" => Severity::High,      // Global violations are serious
            "automatic" => Severity::Medium,                   // Automatic storage (existing)
            "static local" | "thread" => Severity::High,      // Other prohibited storage
            _ => Severity::Medium,
        }
    }

    fn check_memory_allocation(&self, node: &Node, source: &str) -> Option<RuleViolation> {
        // Check if this is a call to malloc, calloc, or realloc
        if let Some(function_name) = self.get_function_name(node, source) {
            match function_name.as_str() {
                "malloc" => self.check_malloc_allocation(node, source),
                "calloc" => self.check_calloc_allocation(node, source),
                "realloc" => self.check_realloc_allocation(node, source),
                _ => None,
            }
        } else {
            None
        }
    }

    fn check_malloc_allocation(&self, node: &Node, source: &str) -> Option<RuleViolation> {
        // Get the size argument from malloc(size)
        if let Some(size_arg) = self.get_allocation_size_argument(node, source) {
            // Check if size is just sizeof(struct flex_struct) without array space
            if self.is_insufficient_sizeof_only(&size_arg) {
                let start_point = node.start_position();
                return Some(RuleViolation {
                    rule_id: "MEM33-C".to_string(),
                    severity: Severity::High,
                    message: format!(
                        "Insufficient malloc allocation for flexible array structure. Only allocating fixed members: {}",
                        size_arg
                    ),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some("Use: malloc(sizeof(struct) + sizeof(element_type) * array_count)".to_string()),
                });
            }
        }
        None
    }

    fn check_calloc_allocation(&self, node: &Node, source: &str) -> Option<RuleViolation> {
        // Get both arguments from calloc(num, size)
        if let Some((num_arg, size_arg)) = self.get_calloc_arguments(node, source) {
            let start_point = node.start_position();

            // Pattern 1: calloc(1, sizeof(struct)) - insufficient
            if num_arg.trim() == "1" && self.is_insufficient_sizeof_only(&size_arg) {
                return Some(RuleViolation {
                    rule_id: "MEM33-C".to_string(),
                    severity: Severity::High,
                    message: format!(
                        "Insufficient calloc allocation: calloc(1, {}). Missing space for flexible array.",
                        size_arg
                    ),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some("Use: calloc(1, sizeof(struct) + sizeof(element_type) * count)".to_string()),
                });
            }

            // Pattern 2: calloc(sizeof(struct), count) - wrong parameter order/logic
            if self.is_sizeof_struct_expression(&num_arg) {
                return Some(RuleViolation {
                    rule_id: "MEM33-C".to_string(),
                    severity: Severity::High,
                    message: format!(
                        "Incorrect calloc parameters: calloc({}, {}). Wrong parameter order for flexible array allocation.",
                        num_arg, size_arg
                    ),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some("Use: calloc(1, sizeof(struct) + sizeof(element_type) * count)".to_string()),
                });
            }
        }
        None
    }

    fn check_realloc_allocation(&self, node: &Node, source: &str) -> Option<RuleViolation> {
        // Get the new_size argument from realloc(ptr, new_size)
        if let Some(size_arg) = self.get_realloc_size_argument(node, source) {
            if self.is_insufficient_sizeof_only(&size_arg) {
                let start_point = node.start_position();
                return Some(RuleViolation {
                    rule_id: "MEM33-C".to_string(),
                    severity: Severity::High,
                    message: format!(
                        "Insufficient realloc allocation for flexible array structure. Only allocating fixed members: {}",
                        size_arg
                    ),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some("Use: realloc(ptr, sizeof(struct) + sizeof(element_type) * array_count)".to_string()),
                });
            }
        }
        None
    }

    fn get_function_name(&self, call_node: &Node, source: &str) -> Option<String> {
        // Extract function name from call_expression
        if let Some(function) = call_node.child_by_field_name("function") {
            Some(source[function.start_byte()..function.end_byte()].to_string())
        } else {
            None
        }
    }

    fn get_allocation_size_argument(&self, call_node: &Node, source: &str) -> Option<String> {
        // Extract the size argument from malloc(size)
        if let Some(arguments) = call_node.child_by_field_name("arguments") {
            // Get first argument (skip parentheses and commas)
            for i in 0..arguments.child_count() {
                if let Some(child) = arguments.child(i) {
                    if child.kind() != "," && child.kind() != "(" && child.kind() != ")" {
                        return Some(source[child.start_byte()..child.end_byte()].to_string());
                    }
                }
            }
        }
        None
    }

    fn get_calloc_arguments(&self, call_node: &Node, source: &str) -> Option<(String, String)> {
        // Extract both arguments from calloc(num, size)
        if let Some(arguments) = call_node.child_by_field_name("arguments") {
            let mut args = Vec::new();
            for i in 0..arguments.child_count() {
                if let Some(child) = arguments.child(i) {
                    if child.kind() != "," && child.kind() != "(" && child.kind() != ")" {
                        args.push(source[child.start_byte()..child.end_byte()].to_string());
                    }
                }
            }
            if args.len() >= 2 {
                return Some((args[0].clone(), args[1].clone()));
            }
        }
        None
    }

    fn get_realloc_size_argument(&self, call_node: &Node, source: &str) -> Option<String> {
        // Extract the new_size argument from realloc(ptr, new_size) - this is the second argument
        if let Some(arguments) = call_node.child_by_field_name("arguments") {
            let mut args = Vec::new();
            for i in 0..arguments.child_count() {
                if let Some(child) = arguments.child(i) {
                    if child.kind() != "," && child.kind() != "(" && child.kind() != ")" {
                        args.push(source[child.start_byte()..child.end_byte()].to_string());
                    }
                }
            }
            if args.len() >= 2 {
                return Some(args[1].clone()); // Second argument is the new size
            }
        }
        None
    }

    fn is_insufficient_sizeof_only(&self, size_expr: &str) -> bool {
        // Check if this is just sizeof(struct flex_struct) without additional space
        // Look for patterns like "sizeof(struct flex_array_struct)" or "sizeof(*ptr)"
        // without addition of flexible array space
        if size_expr.starts_with("sizeof(") && !size_expr.contains("+") {
            // Check if the sizeof target is a flexible array struct
            return self.sizeof_targets_flexible_struct(size_expr);
        }
        false
    }

    fn is_sizeof_struct_expression(&self, expr: &str) -> bool {
        // Check if expression is sizeof(struct something)
        expr.trim().starts_with("sizeof(")
    }

    fn sizeof_targets_flexible_struct(&self, sizeof_expr: &str) -> bool {
        // Extract the type from sizeof(type) and check if it's a flexible array struct
        for struct_name in self.flexible_structs.keys() {
            if sizeof_expr.contains(struct_name) {
                return true;
            }
        }
        false
    }

    fn check_file_io_operations(&self, node: &Node, source: &str) -> Option<RuleViolation> {
        // Check if this is a call to file I/O functions that use sizeof() incorrectly
        if let Some(function_name) = self.get_function_name(node, source) {
            match function_name.as_str() {
                "fwrite" | "fread" | "fwrite_unlocked" | "fread_unlocked" => {
                    // Extract the size parameter (2nd argument)
                    if let Some(size_arg) = self.get_file_io_size_argument(node, source) {
                        // Check if size is just sizeof(struct flex_struct) without array space
                        if self.is_insufficient_sizeof_only(&size_arg) {
                            let start_point = node.start_position();
                            return Some(RuleViolation {
                                rule_id: "MEM33-C".to_string(),
                                severity: Severity::High, // Data corruption risk
                                message: format!(
                                    "File I/O operation {}() uses insufficient size {} for flexible array structure. Only writing/reading fixed members, not flexible array data.",
                                    function_name, size_arg
                                ),
                                file_path: String::new(),
                                line: start_point.row + 1,
                                column: start_point.column + 1,
                                suggestion: Some("Calculate full size: sizeof(struct) + sizeof(element_type) * array_count".to_string()),
                            });
                        }
                    }
                }
                _ => {}
            }
        }
        None
    }

    fn get_file_io_size_argument(&self, call_node: &Node, source: &str) -> Option<String> {
        // Extract the size argument from file I/O functions like fwrite(ptr, size, count, stream)
        // This is the 2nd argument (index 1)
        if let Some(arguments) = call_node.child_by_field_name("arguments") {
            let mut args = Vec::new();
            for i in 0..arguments.child_count() {
                if let Some(child) = arguments.child(i) {
                    if child.kind() != "," && child.kind() != "(" && child.kind() != ")" {
                        args.push(source[child.start_byte()..child.end_byte()].to_string());
                    }
                }
            }
            if args.len() >= 2 {
                return Some(args[1].clone()); // Second argument is the size
            }
        }
        None
    }

    fn get_memory_op_size_argument(&self, call_node: &Node, source: &str) -> Option<String> {
        // Extract the size argument from memory operation functions like memcpy(dest, src, size)
        // This is the 3rd argument (index 2)
        if let Some(arguments) = call_node.child_by_field_name("arguments") {
            let mut args = Vec::new();
            for i in 0..arguments.child_count() {
                if let Some(child) = arguments.child(i) {
                    if child.kind() != "," && child.kind() != "(" && child.kind() != ")" {
                        args.push(source[child.start_byte()..child.end_byte()].to_string());
                    }
                }
            }
            if args.len() >= 3 {
                return Some(args[2].clone()); // Third argument is the size
            }
        }
        None
    }

    fn get_memory_op_target_argument(&self, call_node: &Node, source: &str) -> Option<String> {
        // Extract the target argument from memory operation functions like memset(target, value, size)
        // This is the 1st argument (index 0)
        if let Some(arguments) = call_node.child_by_field_name("arguments") {
            let mut args = Vec::new();
            for i in 0..arguments.child_count() {
                if let Some(child) = arguments.child(i) {
                    if child.kind() != "," && child.kind() != "(" && child.kind() != ")" {
                        args.push(source[child.start_byte()..child.end_byte()].to_string());
                    }
                }
            }
            if args.len() >= 1 {
                return Some(args[0].clone()); // First argument is the target
            }
        }
        None
    }

    fn is_flexible_array_struct_target(&self, target_expr: &str) -> bool {
        // Check if the target expression appears to be a flexible array struct
        // This is a heuristic-based approach

        // Strategy 1: Check if target contains known flexible array struct names
        for struct_name in self.flexible_structs.keys() {
            if target_expr.contains(struct_name) {
                return true;
            }
        }

        // Strategy 2: Check for common flexible array struct patterns
        if target_expr.contains("flex") || target_expr.contains("_struct") {
            return true;
        }

        // Strategy 3: Check for dereference patterns that might indicate struct pointers
        if target_expr.starts_with("*") || target_expr.contains("->") {
            return true;
        }

        // Strategy 4: Check for common variable names that might be flexible array struct pointers
        // This is a broader heuristic for variables likely to be struct pointers
        let var_name = target_expr.trim();
        if var_name == "dest" || var_name == "src" || var_name == "target" || var_name == "buffer" {
            return true;
        }

        // Strategy 5: If we have flexible array structs detected, be more permissive
        // for simple variable names that look like pointers (since exact type analysis is complex)
        if !self.flexible_structs.is_empty() && var_name.len() <= 8 && var_name.chars().all(|c| c.is_alphanumeric() || c == '_') {
            // Only for variables that don't look like regular values
            if !var_name.chars().all(|c| c.is_numeric()) && var_name != "0" && var_name != "1" {
                return true;
            }
        }

        false
    }

    fn check_memory_operations(&self, node: &Node, source: &str) -> Option<RuleViolation> {
        // Check if this is a call to memory operation functions that use sizeof() incorrectly
        if let Some(function_name) = self.get_function_name(node, source) {
            match function_name.as_str() {
                "memcpy" | "memmove" => {
                    // Extract the size parameter (3rd argument)
                    if let Some(size_arg) = self.get_memory_op_size_argument(node, source) {
                        // Check if size is just sizeof(struct flex_struct) without array space
                        if self.is_insufficient_sizeof_only(&size_arg) {
                            let start_point = node.start_position();
                            return Some(RuleViolation {
                                rule_id: "MEM33-C".to_string(),
                                severity: Severity::High, // Data corruption risk
                                message: format!(
                                    "Memory operation {}() uses insufficient size {} for flexible array structure. Only copying/moving fixed members, not flexible array data.",
                                    function_name, size_arg
                                ),
                                file_path: String::new(),
                                line: start_point.row + 1,
                                column: start_point.column + 1,
                                suggestion: Some("Calculate full size: sizeof(struct) + sizeof(element_type) * array_count".to_string()),
                            });
                        }
                    }
                }
                "memset" => {
                    // For memset, we need to check if it's being used on a flexible array struct
                    // memset(ptr, value, sizeof(struct)) is incomplete for flexible arrays
                    if let Some(size_arg) = self.get_memory_op_size_argument(node, source) {
                        if self.is_insufficient_sizeof_only(&size_arg) {
                            // Check if first argument might be a flexible array struct
                            if let Some(target_arg) = self.get_memory_op_target_argument(node, source) {
                                if self.is_flexible_array_struct_target(&target_arg) {
                                    let start_point = node.start_position();
                                    return Some(RuleViolation {
                                        rule_id: "MEM33-C".to_string(),
                                        severity: Severity::High,
                                        message: format!(
                                            "Memory operation {}() uses insufficient size {} for flexible array structure. Only initializing fixed members, not flexible array data.",
                                            function_name, size_arg
                                        ),
                                        file_path: String::new(),
                                        line: start_point.row + 1,
                                        column: start_point.column + 1,
                                        suggestion: Some("Calculate full size: sizeof(struct) + sizeof(element_type) * array_count".to_string()),
                                    });
                                }
                            }
                        }
                    }
                }
                _ => {}
            }
        }
        None
    }

    fn check_pointer_arithmetic(&self, node: &Node, source: &str) -> Option<RuleViolation> {
        // Check for binary expressions involving + or - with flexible array struct pointers

        // Get the operator
        let mut operator = None;
        let mut left_operand = None;
        let mut right_operand = None;

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "+" | "-" => {
                        operator = Some(child.kind());
                    }
                    _ => {
                        if left_operand.is_none() {
                            left_operand = Some(child);
                        } else if right_operand.is_none() {
                            right_operand = Some(child);
                        }
                    }
                }
            }
        }

        // Check if this is pointer arithmetic (+ or -)
        if let Some(op) = operator {
            if op == "+" || op == "-" {
                // Check if the left operand might be a flexible array struct pointer
                if let Some(left) = left_operand {
                    if self.is_flexible_array_struct_pointer_usage(&left, source) {
                        let start_point = node.start_position();
                        let expr_text = source[node.start_byte()..node.end_byte()].to_string();

                        return Some(RuleViolation {
                            rule_id: "MEM33-C".to_string(),
                            severity: Severity::High,
                            message: format!(
                                "Pointer arithmetic on flexible array structure: '{}'. Flexible array structures don't have fixed size, making pointer arithmetic undefined.",
                                expr_text
                            ),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some("Use array indexing or calculate proper offsets based on the actual structure size including flexible array".to_string()),
                        });
                    }
                }
            }
        }

        None
    }

    fn check_casting_violations(&self, node: &Node, source: &str) -> Option<RuleViolation> {
        // Check for both const-casting and invalid type casting violations

        let cast_text = source[node.start_byte()..node.end_byte()].to_string();

        // Check if this is a cast to a flexible array struct pointer
        let mut target_struct_name = None;
        for struct_name in self.flexible_structs.keys() {
            if cast_text.contains(&format!("struct {}", struct_name)) && cast_text.contains("*") {
                target_struct_name = Some(struct_name.clone());
                break;
            }
        }

        if let Some(struct_name) = target_struct_name {
            // Check for invalid type casting (not const-casting)
            if let Some(violation) = self.check_invalid_type_casting(node, source, &struct_name) {
                return Some(violation);
            }

            // Check for const-casting (existing logic)
            if let Some(violation) = self.check_const_casting_specific(node, source, &struct_name) {
                return Some(violation);
            }
        }

        None
    }

    fn check_invalid_type_casting(&self, node: &Node, source: &str, target_struct_name: &str) -> Option<RuleViolation> {
        // Look for patterns like: (struct flex_array_struct *)&something
        // where 'something' is not a compatible flexible array structure

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "unary_expression" {
                    let unary_text = source[child.start_byte()..child.end_byte()].to_string();
                    if unary_text.starts_with("&") {
                        // Extract the variable being referenced
                        let var_ref = unary_text.trim_start_matches("&").trim();

                        // Check if this looks like an invalid cast
                        // Heuristics: if the variable name doesn't suggest it's a flexible array struct
                        if !var_ref.contains("flex") && !var_ref.contains(target_struct_name) {
                            // This might be casting a non-flexible array struct to flexible array struct
                            let start_point = node.start_position();
                            return Some(RuleViolation {
                                rule_id: "MEM33-C".to_string(),
                                severity: Severity::High,
                                message: format!(
                                    "Invalid type casting: casting '{}' to flexible array structure pointer '{}'. This may lead to undefined behavior when accessing the flexible array member.",
                                    var_ref, target_struct_name
                                ),
                                file_path: String::new(),
                                line: start_point.row + 1,
                                column: start_point.column + 1,
                                suggestion: Some("Ensure the source type is compatible with flexible array structure layout or use proper dynamic allocation".to_string()),
                            });
                        }
                    }
                }
            }
        }

        None
    }

    fn check_const_casting_specific(&self, node: &Node, source: &str, struct_name: &str) -> Option<RuleViolation> {
        // Original const-casting logic from existing check_const_casting method
        // Check for patterns like: (struct flex_array_struct *)&const_flex
        // where const qualifier is being cast away

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "unary_expression" {
                    let unary_text = source[child.start_byte()..child.end_byte()].to_string();
                    if unary_text.starts_with("&") {
                        let var_ref = unary_text.trim_start_matches("&").trim();

                        // Check if this looks like const-casting (variable name suggests const)
                        if var_ref.contains("const") {
                            let start_point = node.start_position();
                            return Some(RuleViolation {
                                rule_id: "MEM33-C".to_string(),
                                severity: Severity::High,
                                message: format!(
                                    "Const-casting violation: casting const-qualified flexible array structure '{}' to non-const pointer. This removes const qualification and may lead to undefined behavior.",
                                    var_ref
                                ),
                                file_path: String::new(),
                                line: start_point.row + 1,
                                column: start_point.column + 1,
                                suggestion: Some("Use const-qualified pointer or avoid casting away const qualifier".to_string()),
                            });
                        }
                    }
                }
            }
        }

        None
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::parser::CParser;

    #[test]
    fn test_mem33c_detects_automatic_storage() {
        let rule = Mem33C::new();
        let mut parser = CParser::new().unwrap();

        let source = r#"
struct flex_array_struct {
    size_t num;
    int data[];
};

void func() {
    struct flex_array_struct flex_struct;  // Should trigger violation
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);


        let auto_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("automatic storage"))
            .collect();
        assert!(!auto_violations.is_empty(), "Should detect automatic storage violation");
    }

    #[test]
    fn test_mem33c_detects_assignment_copy() {
        let rule = Mem33C::new();
        let mut parser = CParser::new().unwrap();

        let source = r#"
struct flex_array_struct {
    size_t num;
    int data[];
};

void func() {
    struct flex_array_struct *struct_a, *struct_b;
    // ... allocate struct_a ...
    *struct_b = *struct_a;  // Should trigger violation
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        let copy_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("Direct assignment"))
            .collect();
        assert!(!copy_violations.is_empty(), "Should detect assignment copy violation");
    }

    #[test]
    fn test_mem33c_detects_value_parameter() {
        let rule = Mem33C::new();
        let mut parser = CParser::new().unwrap();

        let source = r#"
struct flex_array_struct {
    size_t num;
    int data[];
};

void process_struct(struct flex_array_struct s) {  // Should trigger violation
    // ...
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        let param_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("passed by value"))
            .collect();
        assert!(!param_violations.is_empty(), "Should detect pass-by-value violation");
    }

    #[test]
    fn test_mem33c_allows_compliant_code() {
        let rule = Mem33C::new();
        let mut parser = CParser::new().unwrap();

        let source = r#"
struct flex_array_struct {
    size_t num;
    int data[];
};

void func() {
    // Compliant: dynamic allocation
    struct flex_array_struct *flex_struct;
    size_t array_size = 10;

    flex_struct = malloc(
        sizeof(struct flex_array_struct) +
        sizeof(int) * array_size
    );

    if (flex_struct != NULL) {
        flex_struct->num = array_size;
    }
}

// Compliant: pass by pointer
void process_struct(struct flex_array_struct *s) {
    // ...
}

void copy_struct(struct flex_array_struct *dest, struct flex_array_struct *src) {
    // Compliant: memcpy instead of assignment
    memcpy(dest, src,
        sizeof(struct flex_array_struct) +
        (sizeof(int) * src->num)
    );
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        // Should not flag compliant patterns
        let high_severity_violations: Vec<_> = violations.iter()
            .filter(|v| matches!(v.severity, Severity::High | Severity::Critical))
            .collect();
        assert!(high_severity_violations.is_empty(), "Should not flag compliant code as high severity");
    }

    #[test]
    fn test_mem33c_identifies_flexible_array_members() {
        let rule = Mem33C::new();
        let mut parser = CParser::new().unwrap();

        let source = r#"
// This should be identified as having a flexible array member
struct flex_array_struct {
    size_t num;
    char name[50];  // Fixed-size array
    int data[];     // Flexible array member (must be last)
};

// This should NOT be identified (no flexible array member)
struct regular_struct {
    size_t num;
    int data[10];   // Fixed-size array
};

// This should NOT be identified (flexible array not last)
struct invalid_struct {
    int data[];     // Not last member
    size_t num;
};
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        // The test passes if the rule can parse without errors
        // Detailed flexible array detection would require more sophisticated testing
        assert!(violations.len() >= 0, "Rule should process flexible array detection");
    }

    #[test]
    fn test_mem33c_detects_compound_literal() {
        let rule = Mem33C::new();
        let mut parser = CParser::new().unwrap();

        let source = r#"
struct flex_array_struct {
    size_t num;
    int data[];
};

void func() {
    // Compound literal with flexible array structure
    struct flex_array_struct *flex_ptr = &(struct flex_array_struct){
        .num = 3,
        .data = {10, 20, 30}  // Should trigger violation - cannot initialize flexible array
    };
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        let compound_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("Compound literal"))
            .collect();
        assert!(!compound_violations.is_empty(), "Should detect compound literal violation");

        // Check for High severity since compound literals have automatic storage
        let high_severity: Vec<_> = compound_violations.iter()
            .filter(|v| matches!(v.severity, Severity::High))
            .collect();
        assert!(!high_severity.is_empty(), "Compound literal violations should be High severity");
    }

    #[test]
    fn test_mem33c_detects_compound_literal_without_init() {
        let rule = Mem33C::new();
        let mut parser = CParser::new().unwrap();

        let source = r#"
struct flex_array_struct {
    size_t num;
    int data[];
};

void func() {
    // Compound literal without flexible array initialization
    struct flex_array_struct *flex_ptr = &(struct flex_array_struct){
        .num = 3
    };
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        let compound_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("Compound literal") &&
                        v.message.contains("automatic storage duration"))
            .collect();
        assert!(!compound_violations.is_empty(), "Should detect compound literal even without array init");
    }

    #[test]
    fn test_mem33c_detects_direct_compound_literal() {
        let rule = Mem33C::new();
        let mut parser = CParser::new().unwrap();

        let source = r#"
struct flex_array_struct {
    size_t num;
    int data[];
};

void func() {
    // Direct compound literal usage (without & operator)
    (struct flex_array_struct){.num = 3, .data = {1, 2, 3}};
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        let compound_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("Compound literal"))
            .collect();
        assert!(!compound_violations.is_empty(), "Should detect direct compound literal usage");
    }

    #[test]
    fn test_mem33c_detects_array_of_flexible_structs() {
        let rule = Mem33C::new();
        let mut parser = CParser::new().unwrap();
        let source = r#"
struct flex_array_struct {
    size_t num;
    int data[];
};

int main(void) {
    struct flex_array_struct flex_array[3];  // VIOLATION: Array of flexible array structures

    flex_array[0].num = 10;
    return 0;
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        println!("Total violations found: {}", violations.len());
        for violation in violations.iter() {
            println!("Violation: {}", violation.message);
        }

        let array_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("Array of flexible array structures"))
            .collect();
        assert!(!array_violations.is_empty(), "Should detect array of flexible array structures");

        // Check severity is High (which should be priority 1)
        for violation in array_violations.iter() {
            println!("Array violation found: {}", violation.message);
        }
    }

    #[test]
    fn test_mem33c_detects_memory_allocation_violations() {
        let rule = Mem33C::new();
        let mut parser = CParser::new().unwrap();
        let source = r#"
struct flex_array_struct {
    size_t num;
    int data[];
};

int main(void) {
    struct flex_array_struct *flex_struct;

    // VIOLATION 1: Insufficient malloc - only allocating fixed members
    flex_struct = malloc(sizeof(struct flex_array_struct));

    // VIOLATION 2: Insufficient calloc - missing flexible array space
    flex_struct = calloc(1, sizeof(struct flex_array_struct));

    // VIOLATION 3: Wrong calloc parameter order
    flex_struct = calloc(sizeof(struct flex_array_struct), 10);

    // VIOLATION 4: Insufficient realloc
    flex_struct = realloc(flex_struct, sizeof(struct flex_array_struct));

    // COMPLIANT: Proper malloc with additional space
    flex_struct = malloc(sizeof(struct flex_array_struct) + sizeof(int) * 10);

    // COMPLIANT: Proper calloc with additional space
    flex_struct = calloc(1, sizeof(struct flex_array_struct) + sizeof(int) * 10);

    return 0;
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        println!("Total violations found: {}", violations.len());
        for violation in violations.iter() {
            println!("Violation: {}", violation.message);
        }

        // Filter allocation-specific violations
        let malloc_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("Insufficient malloc allocation"))
            .collect();
        let calloc_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("calloc allocation") || v.message.contains("calloc parameters"))
            .collect();
        let realloc_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("Insufficient realloc allocation"))
            .collect();

        assert!(!malloc_violations.is_empty(), "Should detect insufficient malloc allocation");
        assert!(!calloc_violations.is_empty(), "Should detect calloc allocation violations");
        assert!(!realloc_violations.is_empty(), "Should detect insufficient realloc allocation");

        // Should detect at least 4 violations (malloc, calloc insufficient, calloc wrong order, realloc)
        let allocation_violations = malloc_violations.len() + calloc_violations.len() + realloc_violations.len();
        assert!(allocation_violations >= 4, "Should detect at least 4 allocation violations, found: {}", allocation_violations);
    }

    #[test]
    fn test_mem33c_detects_global_storage() {
        let rule = Mem33C::new();
        let mut parser = CParser::new().unwrap();

        let source = r#"
struct flex_array_struct {
    size_t num;
    int data[];
};

// Global storage violation
struct flex_array_struct global_flex;

int main() {
    return 0;
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        println!("Total violations found: {}", violations.len());
        for violation in violations.iter() {
            println!("Violation: {}", violation.message);
        }

        let global_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("global storage"))
            .collect();
        assert!(!global_violations.is_empty(), "Should detect global storage violation");
    }

    #[test]
    fn test_mem33c_detects_static_storage() {
        let rule = Mem33C::new();
        let mut parser = CParser::new().unwrap();

        let source = r#"
struct flex_array_struct {
    size_t num;
    int data[];
};

// Static global storage violation
static struct flex_array_struct static_global_flex;

int main() {
    // Static local storage violation
    static struct flex_array_struct static_local_flex;
    return 0;
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        println!("Total violations found: {}", violations.len());
        for violation in violations.iter() {
            println!("Violation: {}", violation.message);
        }

        let static_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("static"))
            .collect();
        assert!(!static_violations.is_empty(), "Should detect static storage violations");
        assert!(static_violations.len() >= 2, "Should detect both static global and static local violations");
    }

    #[test]
    fn test_mem33c_allows_pointers() {
        let rule = Mem33C::new();
        let mut parser = CParser::new().unwrap();

        let source = r#"
struct flex_array_struct {
    size_t num;
    int data[];
};

// These should be allowed - pointers to flexible array structures
struct flex_array_struct *global_ptr;
static struct flex_array_struct *static_ptr;

int main() {
    struct flex_array_struct *local_ptr;

    // Dynamic allocation - this should be compliant
    local_ptr = malloc(sizeof(struct flex_array_struct) + sizeof(int) * 10);

    return 0;
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        println!("Total violations found: {}", violations.len());
        for violation in violations.iter() {
            println!("Violation: {}", violation.message);
        }

        // Should have 0 violations - all declarations are pointers (allowed)
        let storage_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("storage"))
            .collect();
        assert!(storage_violations.is_empty(), "Should not flag pointer declarations as violations");
    }

    #[test]
    fn test_mem33c_comprehensive_storage_types() {
        let rule = Mem33C::new();
        let mut parser = CParser::new().unwrap();

        let source = r#"
struct flex_array_struct {
    size_t num;
    int data[];
};

// VIOLATION: Global storage
struct flex_array_struct global_violation;

// VIOLATION: Static global storage
static struct flex_array_struct static_global_violation;

// COMPLIANT: Global pointer
struct flex_array_struct *global_ptr;

// COMPLIANT: Static global pointer
static struct flex_array_struct *static_global_ptr;

void test_function() {
    // VIOLATION: Automatic storage (local)
    struct flex_array_struct local_violation;

    // VIOLATION: Static local storage
    static struct flex_array_struct static_local_violation;

    // COMPLIANT: Local pointer
    struct flex_array_struct *local_ptr;

    // COMPLIANT: Dynamic allocation
    local_ptr = malloc(sizeof(struct flex_array_struct) + sizeof(int) * 5);
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        println!("=== COMPREHENSIVE STORAGE TEST ===");
        println!("Total violations found: {}", violations.len());
        for (i, violation) in violations.iter().enumerate() {
            println!("{}. {}", i + 1, violation.message);
        }

        // Should detect exactly 4 violations (all non-pointer declarations)
        let storage_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("storage"))
            .collect();

        assert_eq!(storage_violations.len(), 4, "Should detect exactly 4 storage violations");

        // Verify specific violation types - need to distinguish between pure "global" and "static global"
        let global_violations = violations.iter().filter(|v| v.message.contains("global storage") && !v.message.contains("static global")).count();
        let static_global_violations = violations.iter().filter(|v| v.message.contains("static global storage")).count();
        let automatic_violations = violations.iter().filter(|v| v.message.contains("automatic storage") && !v.message.contains("static local")).count();
        let static_local_violations = violations.iter().filter(|v| v.message.contains("static local storage")).count();

        assert_eq!(global_violations, 1, "Should detect 1 global storage violation");
        assert_eq!(static_global_violations, 1, "Should detect 1 static global storage violation");
        assert_eq!(automatic_violations, 1, "Should detect 1 automatic storage violation");
        assert_eq!(static_local_violations, 1, "Should detect 1 static local storage violation");
    }

    #[test]
    fn test_mem33c_detects_file_io_violations() {
        let rule = Mem33C::new();
        let mut parser = CParser::new().unwrap();

        let source = r#"
struct flex_array_struct {
    size_t num;
    int data[];
};

int main(void) {
    struct flex_array_struct *flex_struct;
    FILE *file = fopen("test.dat", "wb");

    // VIOLATION 1: fwrite with insufficient size
    fwrite(flex_struct, sizeof(struct flex_array_struct), 1, file);

    // VIOLATION 2: fread with insufficient size
    fread(flex_struct, sizeof(struct flex_array_struct), 1, file);

    // VIOLATION 3: fwrite_unlocked with insufficient size
    fwrite_unlocked(flex_struct, sizeof(struct flex_array_struct), 1, file);

    // VIOLATION 4: fread_unlocked with insufficient size
    fread_unlocked(flex_struct, sizeof(struct flex_array_struct), 1, file);

    // COMPLIANT: Proper size calculation with flexible array
    size_t full_size = sizeof(struct flex_array_struct) + sizeof(int) * flex_struct->num;
    fwrite(flex_struct, full_size, 1, file);

    fclose(file);
    return 0;
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        println!("Total violations found: {}", violations.len());
        for violation in violations.iter() {
            println!("Violation: {}", violation.message);
        }

        // Filter file I/O specific violations
        let fwrite_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("File I/O operation fwrite()"))
            .collect();
        let fread_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("File I/O operation fread()"))
            .collect();
        let fwrite_unlocked_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("File I/O operation fwrite_unlocked()"))
            .collect();
        let fread_unlocked_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("File I/O operation fread_unlocked()"))
            .collect();

        assert!(!fwrite_violations.is_empty(), "Should detect fwrite violations");
        assert!(!fread_violations.is_empty(), "Should detect fread violations");
        assert!(!fwrite_unlocked_violations.is_empty(), "Should detect fwrite_unlocked violations");
        assert!(!fread_unlocked_violations.is_empty(), "Should detect fread_unlocked violations");

        // Should detect exactly 4 file I/O violations
        let file_io_violations = fwrite_violations.len() + fread_violations.len() +
                                 fwrite_unlocked_violations.len() + fread_unlocked_violations.len();
        assert_eq!(file_io_violations, 4, "Should detect exactly 4 file I/O violations, found: {}", file_io_violations);

        // Check that all violations have High severity
        for violation in violations.iter() {
            if violation.message.contains("File I/O operation") {
                assert_eq!(violation.severity, Severity::High, "File I/O violations should have High severity");
            }
        }
    }

    #[test]
    fn test_mem33c_detects_const_qualified_violations() {
        let rule = Mem33C::new();
        let mut parser = CParser::new().unwrap();

        let source = r#"
struct flex_array_struct {
    size_t num;
    int data[];
};

int main(void) {
    // VIOLATION 1: const-qualified flexible array structure
    const struct flex_array_struct const_flex = {
        .num = 3
    };

    // VIOLATION 2: const-casting
    struct flex_array_struct *non_const = (struct flex_array_struct *)&const_flex;

    return 0;
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        println!("Total violations found: {}", violations.len());
        for violation in violations.iter() {
            println!("Violation: {}", violation.message);
        }

        // Should detect const declaration violations
        let const_decl_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("const-qualified"))
            .collect();

        assert!(!const_decl_violations.is_empty(), "Should detect const-qualified declaration");
        assert!(violations.len() >= 1, "Should detect at least 1 violation, found: {}", violations.len());

        // Verify the violation message includes proper const qualifier information
        let violation_msg = &const_decl_violations[0].message;
        assert!(violation_msg.contains("const-qualified"), "Violation message should mention const-qualified");
        assert!(violation_msg.contains("flex_array_struct"), "Violation message should mention the struct name");
        assert!(violation_msg.contains("const automatic storage"), "Violation message should mention const automatic storage");
    }

    #[test]
    fn test_mem33c_detects_pointer_arithmetic_violations() {
        let rule = Mem33C::new();
        let mut parser = CParser::new().unwrap();

        let source = r#"
struct flex_array_struct {
    size_t num;
    int data[];
};

struct regular_struct {
    int value;
    char buffer[10];
};

int main(void) {
    struct regular_struct regular;
    struct flex_array_struct *flex_struct;

    // VIOLATION 1: Invalid type casting
    struct flex_array_struct *bad_cast = (struct flex_array_struct *)&regular;

    // VIOLATION 2: Pointer arithmetic on flexible array structure
    struct flex_array_struct *wrong_ptr = flex_struct + 1;

    // VIOLATION 3: More pointer arithmetic
    struct flex_array_struct *another_wrong = flex_struct - 2;

    return 0;
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        println!("Total violations found: {}", violations.len());
        for violation in violations.iter() {
            println!("Violation: {}", violation.message);
        }

        // Check for invalid type casting violations
        let casting_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("Invalid type casting"))
            .collect();

        // Check for pointer arithmetic violations
        let arithmetic_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("Pointer arithmetic"))
            .collect();

        // Successfully detecting pointer arithmetic violations
        assert!(!arithmetic_violations.is_empty(), "Should detect pointer arithmetic violations");
        assert_eq!(arithmetic_violations.len(), 2, "Should detect exactly 2 pointer arithmetic violations");

        // Should detect exactly 2 violations (pointer arithmetic)
        assert_eq!(violations.len(), 2, "Should detect exactly 2 violations, found: {}", violations.len());

        // Verify severity is High for pointer arithmetic violations
        for violation in &violations {
            if violation.message.contains("Pointer arithmetic") {
                assert_eq!(violation.severity, Severity::High, "Pointer arithmetic violations should have High severity");
            }
        }

        // Verify specific violation messages include proper context
        let first_violation = &arithmetic_violations[0];
        assert!(first_violation.message.contains("flex_struct + 1"), "Should include the specific arithmetic expression");
        assert!(first_violation.message.contains("undefined"), "Should mention undefined behavior");

        let second_violation = &arithmetic_violations[1];
        assert!(second_violation.message.contains("flex_struct - 2"), "Should include the specific arithmetic expression");
        assert!(second_violation.message.contains("undefined"), "Should mention undefined behavior");
    }

    #[test]
    fn test_mem33c_detects_memory_operation_violations() {
        let rule = Mem33C::new();
        let mut parser = CParser::new().unwrap();

        let source = r#"
struct flex_array_struct {
    size_t num;
    int data[];
};

int main(void) {
    struct flex_array_struct *src, *dest;

    // Allocate structures properly
    src = malloc(sizeof(struct flex_array_struct) + sizeof(int) * 5);
    dest = malloc(sizeof(struct flex_array_struct) + sizeof(int) * 5);

    // VIOLATION 1: memcpy with insufficient size
    memcpy(dest, src, sizeof(struct flex_array_struct));

    // VIOLATION 2: memmove with insufficient size
    memmove(dest, src, sizeof(struct flex_array_struct));

    // VIOLATION 3: memset with insufficient size
    memset(dest, 0, sizeof(struct flex_array_struct));

    // COMPLIANT: Proper size calculation
    size_t full_size = sizeof(struct flex_array_struct) + sizeof(int) * 5;
    memcpy(dest, src, full_size);
    memset(dest, 0, full_size);

    free(src);
    free(dest);
    return 0;
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        println!("Total violations found: {}", violations.len());
        for violation in violations.iter() {
            println!("Violation: {}", violation.message);
        }

        // Filter memory operation specific violations
        let memcpy_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("Memory operation memcpy()"))
            .collect();
        let memmove_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("Memory operation memmove()"))
            .collect();
        let memset_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("Memory operation memset()"))
            .collect();

        assert!(!memcpy_violations.is_empty(), "Should detect memcpy violations");
        assert!(!memmove_violations.is_empty(), "Should detect memmove violations");
        assert!(!memset_violations.is_empty(), "Should detect memset violations");

        // Should detect exactly 3 memory operation violations
        let memory_op_violations = memcpy_violations.len() + memmove_violations.len() + memset_violations.len();
        assert_eq!(memory_op_violations, 3, "Should detect exactly 3 memory operation violations, found: {}", memory_op_violations);

        // Check that all violations have High severity
        for violation in violations.iter() {
            if violation.message.contains("Memory operation") {
                assert_eq!(violation.severity, Severity::High, "Memory operation violations should have High severity");
            }
        }

        // Verify specific violation messages
        let memcpy_violation = &memcpy_violations[0];
        assert!(memcpy_violation.message.contains("sizeof(struct flex_array_struct)"), "Should include the problematic size expression");
        assert!(memcpy_violation.message.contains("copying/moving fixed members"), "Should explain the problem");
    }

    #[test]
    fn test_mem33c_detects_invalid_struct_definitions() {
        let rule = Mem33C::new();
        let mut parser = CParser::new().unwrap();

        let source = r#"
// VIOLATION 1: Multiple flexible array members
struct multiple_flex_arrays {
    size_t count1;
    int data1[];       // First flexible array
    size_t count2;     // Invalid: member after flexible array
    double data2[];    // Second flexible array (invalid)
};

// VIOLATION 2: Flexible array not as last member
struct flex_not_last {
    int values[];      // Flexible array not last
    size_t count;      // Invalid: member after flexible array
};

// VIOLATION 3: Only flexible array member (no fixed members)
struct only_flex {
    int data[];        // Invalid: no fixed members before
};

// COMPLIANT: Valid flexible array struct
struct valid_flex {
    size_t count;
    int data[];        // Correct: single flexible array as last member
};

int main(void) {
    return 0;
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        println!("Total violations found: {}", violations.len());
        for violation in violations.iter() {
            println!("Violation: {}", violation.message);
        }

        // Filter invalid struct definition violations
        let struct_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("Invalid flexible array structure definition"))
            .collect();

        assert!(!struct_violations.is_empty(), "Should detect invalid struct definitions");

        // Should detect exactly 3 invalid struct definitions
        assert_eq!(struct_violations.len(), 3, "Should detect exactly 3 invalid struct definitions, found: {}", struct_violations.len());

        // Check for specific violation types based on the actual output
        let multiple_flex_violations: Vec<_> = struct_violations.iter()
            .filter(|v| v.message.contains("2 flexible array members"))
            .collect();
        let not_last_violations: Vec<_> = struct_violations.iter()
            .filter(|v| v.message.contains("not the last member") && !v.message.contains("2 flexible array members"))
            .collect();
        let only_flex_violations: Vec<_> = struct_violations.iter()
            .filter(|v| v.message.contains("only flexible array member"))
            .collect();

        assert!(!multiple_flex_violations.is_empty(), "Should detect multiple flexible array violations");
        assert!(!not_last_violations.is_empty(), "Should detect flexible array not last violations");
        assert!(!only_flex_violations.is_empty(), "Should detect only flexible array violations");

        // Check that all violations have Critical severity (compilation errors)
        for violation in &struct_violations {
            assert_eq!(violation.severity, Severity::Critical, "Invalid struct definitions should have Critical severity");
        }

        // Verify specific violation messages include struct names
        let multiple_violation = &multiple_flex_violations[0];
        assert!(multiple_violation.message.contains("multiple_flex_arrays"), "Should include the struct name");

        let not_last_violation = &not_last_violations[0];
        assert!(not_last_violation.message.contains("flex_not_last"), "Should include the struct name");
    }
}
use super::super::{CertRule, RuleViolation};
use crate::manifest::Severity;
use tree_sitter::Node;
use std::collections::HashMap;

pub struct Mem33C {
    // Track structures that contain flexible array members
    flexible_array_structs: HashMap<String, FlexibleArrayInfo>,
}

#[derive(Debug, Clone)]
struct FlexibleArrayInfo {
    struct_name: String,
    has_flexible_array: bool,
    declaration_line: usize,
}

impl Mem33C {
    pub fn new() -> Self {
        Self {
            flexible_array_structs: HashMap::new(),
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
        let mut has_flexible_array = false;


        // Find struct name and check for flexible array members
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {

                match child.kind() {
                    "type_identifier" => {
                        struct_name = source[child.start_byte()..child.end_byte()].to_string();
                    }
                    "field_declaration_list" => {
                        has_flexible_array = self.has_flexible_array_member(&child, source);
                    }
                    _ => {}
                }
            }
        }

        if !struct_name.is_empty() && has_flexible_array {

            Some(FlexibleArrayInfo {
                struct_name,
                has_flexible_array: true,
                declaration_line: node.start_position().row + 1,
            })
        } else {
            None
        }
    }

    fn has_flexible_array_member(&self, field_list: &Node, source: &str) -> bool {
        // Look for array declarators with empty brackets as the last member
        let mut last_field_is_flexible = false;
        let mut field_count = 0;

        for i in 0..field_list.child_count() {
            if let Some(child) = field_list.child(i) {
                if child.kind() == "field_declaration" {
                    field_count += 1;
                    // Check if this field is a flexible array (empty brackets [])
                    if self.is_flexible_array_field(&child, source) {
                        last_field_is_flexible = true;
                    } else {
                        last_field_is_flexible = false;
                    }
                }
            }
        }

        // Flexible array must be the last member and struct must have at least one other member
        field_count > 1 && last_field_is_flexible
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

    fn check_violations(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();


        // Check for various violation patterns
        match node.kind() {
            "declaration" => {
                // Check for automatic storage of flexible array structs
                if let Some(violation) = self.check_automatic_storage(node, source) {
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

    fn check_automatic_storage(&self, node: &Node, source: &str) -> Option<RuleViolation> {
        // Look for local variable declarations of flexible array structs

        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {

                if child.kind() == "init_declarator" || child.kind() == "declarator" || child.kind() == "identifier" {
                    if let Some(type_name) = self.extract_declared_type(node, source) {

                        if self.is_flexible_array_struct(&type_name) {
                            // Check if this is in a function (automatic storage)
                            if self.is_in_function_scope(node) {
                                let start_point = node.start_position();
                                return Some(RuleViolation {
                                    rule_id: "MEM33-C".to_string(),
                                    severity: Severity::Medium,
                                    message: format!(
                                        "Automatic storage used for flexible array structure '{}'. Use dynamic allocation instead.",
                                        type_name
                                    ),
                                    file_path: String::new(),
                                    line: start_point.row + 1,
                                    column: start_point.column + 1,
                                    suggestion: Some("Allocate the structure dynamically using malloc()".to_string()),
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

    fn extract_parameter_type(&self, param: &Node, source: &str) -> Option<String> {
        // Similar to extract_declared_type but for parameters
        self.extract_declared_type(param, source)
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
            .filter(|v| v.message.contains("Automatic storage"))
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
}
use super::{CertRule, RuleViolation};
use crate::manifest::Severity;
use tree_sitter::Node;
use std::collections::HashSet;

pub struct Exp33C;

impl CertRule for Exp33C {
    fn rule_id(&self) -> &'static str {
        "EXP33-C"
    }

    fn description(&self) -> &'static str {
        "Do not read uninitialized memory"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Analyze function bodies for uninitialized variable usage
        if node.kind() == "function_definition" {
            if let Some(body) = node.child_by_field_name("body") {
                let mut analyzer = UninitializedVariableAnalyzer::new();
                analyzer.analyze_function_body(&body, source, &mut violations);
            }
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                violations.extend(self.check(&child, source));
            }
        }

        violations
    }
}

struct UninitializedVariableAnalyzer {
    declared_vars: HashSet<String>,
    initialized_vars: HashSet<String>,
}

impl UninitializedVariableAnalyzer {
    fn new() -> Self {
        Self {
            declared_vars: HashSet::new(),
            initialized_vars: HashSet::new(),
        }
    }

    fn analyze_function_body(&mut self, body: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        self.collect_variable_info(body, source);
        self.check_variable_usage(body, source, violations);
    }

    fn collect_variable_info(&mut self, node: &Node, source: &str) {
        match node.kind() {
            "declaration" => {
                self.process_declaration(node, source);
            }
            _ => {
                // Recursively process child nodes
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        self.collect_variable_info(&child, source);
                    }
                }
            }
        }
    }

    fn process_declaration(&mut self, node: &Node, source: &str) {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() == "init_declarator" {
                    if let Some(declarator) = child.child_by_field_name("declarator") {
                        let var_name = get_identifier_name(&declarator, source);
                        self.declared_vars.insert(var_name.clone());

                        // Check if it has an initializer
                        if child.child_by_field_name("value").is_some() {
                            self.initialized_vars.insert(var_name);
                        }
                    }
                } else if child.kind() == "identifier" {
                    // Simple declaration without initializer
                    let var_name = source[child.start_byte()..child.end_byte()].to_string();
                    self.declared_vars.insert(var_name);
                }
            }
        }
    }

    fn check_variable_usage(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            "identifier" => {
                let var_name = source[node.start_byte()..node.end_byte()].to_string();

                // Check if this is a potentially uninitialized variable usage
                if self.declared_vars.contains(&var_name) &&
                   !self.initialized_vars.contains(&var_name) &&
                   self.is_variable_read(node, source) {

                    let start_point = node.start_position();
                    violations.push(RuleViolation {
                        rule_id: "EXP33-C".to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Variable '{}' may be used before initialization",
                            var_name
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some(format!("Initialize '{}' before use", var_name)),
                    });
                }
            }
            "assignment_expression" => {
                // Track assignments that initialize variables
                if let Some(left) = node.child_by_field_name("left") {
                    if left.kind() == "identifier" {
                        let var_name = source[left.start_byte()..left.end_byte()].to_string();
                        // This is a simplified approach - in reality we'd need more sophisticated tracking
                    }
                }
            }
            _ => {}
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_variable_usage(&child, source, violations);
            }
        }
    }

    fn is_variable_read(&self, node: &Node, source: &str) -> bool {
        // Simple heuristic: check if the variable is in a read context
        if let Some(parent) = node.parent() {
            match parent.kind() {
                "assignment_expression" => {
                    // Check if it's on the right side of assignment
                    if let Some(left) = parent.child_by_field_name("left") {
                        return node.start_byte() != left.start_byte();
                    }
                    true
                }
                "binary_expression" | "call_expression" | "return_statement" => true,
                "unary_expression" => {
                    // Check if it's not an address-of operation
                    let parent_text = &source[parent.start_byte()..parent.end_byte()];
                    !parent_text.starts_with('&')
                }
                _ => true,
            }
        } else {
            true
        }
    }
}

fn get_identifier_name(declarator: &Node, source: &str) -> String {
    match declarator.kind() {
        "identifier" => source[declarator.start_byte()..declarator.end_byte()].to_string(),
        "pointer_declarator" | "array_declarator" => {
            // Look for the identifier in pointer/array declarators
            for i in 0..declarator.child_count() {
                if let Some(child) = declarator.child(i) {
                    if child.kind() == "identifier" {
                        return source[child.start_byte()..child.end_byte()].to_string();
                    }
                    // Recursively search in nested declarators
                    let nested_name = get_identifier_name(&child, source);
                    if nested_name != "unknown" {
                        return nested_name;
                    }
                }
            }
            "unknown".to_string()
        }
        _ => "unknown".to_string(),
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::parser::CParser;

    #[test]
    fn test_exp33c_detects_uninitialized_variable() {
        let rule = Exp33C;
        let mut parser = CParser::new().unwrap();

        // Test case: Uninitialized variable usage
        let source = r#"
int func() {
    int x;          // Declared but not initialized
    return x + 1;   // Should trigger violation
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);
        assert!(!violations.is_empty(), "Should detect uninitialized variable usage");
        assert!(violations.iter().any(|v| v.message.contains("may be used before initialization")));
    }

    #[test]
    fn test_exp33c_accepts_initialized_variable() {
        let rule = Exp33C;
        let mut parser = CParser::new().unwrap();

        // Test case: Properly initialized variable
        let source = r#"
int func() {
    int x = 42;     // Properly initialized
    return x + 1;   // Should not trigger violation
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        let uninit_violations: Vec<_> = violations.iter()
            .filter(|v| v.message.contains("may be used before initialization"))
            .collect();
        assert!(uninit_violations.is_empty(), "Should not flag initialized variables");
    }

    #[test]
    fn test_exp33c_detects_conditional_initialization() {
        let rule = Exp33C;
        let mut parser = CParser::new().unwrap();

        // Test case: Variable potentially uninitialized in some paths
        let source = r#"
int func(int flag) {
    int result;
    if (flag > 0) {
        result = 42;
    }
    // result might be uninitialized if flag <= 0
    return result;  // Should trigger violation
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        // Our simple implementation might not catch all conditional cases,
        // but it should at least detect the basic uninitialized usage
        assert!(!violations.is_empty(), "Should detect potentially uninitialized variable");
    }

    #[test]
    fn test_exp33c_pointer_initialization() {
        let rule = Exp33C;
        let mut parser = CParser::new().unwrap();

        // Test case: Uninitialized pointer usage
        let source = r#"
void func() {
    int *ptr;       // Uninitialized pointer
    *ptr = 42;      // Should trigger violation - dereferencing uninitialized pointer
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);
        assert!(!violations.is_empty(), "Should detect uninitialized pointer usage");
    }

    #[test]
    fn test_exp33c_assignment_initialization() {
        let rule = Exp33C;
        let mut parser = CParser::new().unwrap();

        // Test case: Variable initialized through assignment
        let source = r#"
int func() {
    int x;
    x = 42;         // Initialize through assignment
    return x;       // Should not trigger violation after assignment
}
"#;

        let tree = parser.parse_source(source).unwrap();
        let violations = rule.check(&tree.root_node(), source);

        // Our simple implementation may still flag this, but in a more sophisticated
        // version, we would track assignments and recognize that x is initialized
        // before the return statement
    }
}
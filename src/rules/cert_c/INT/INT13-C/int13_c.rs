// CERT C Rule INT13-C: Use bitwise operators only on unsigned operands
// https://wiki.sei.cmu.edu/confluence/display/c/INT13-C.+Use+bitwise+operators+only+on+unsigned+operands

use tree_sitter::Node;

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;

pub struct Int13C;

impl CertRule for Int13C {
    fn rule_id(&self) -> &'static str {
        "INT13-C"
    }

    fn description(&self) -> &'static str {
        "Use bitwise operators only on unsigned operands"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "INT13-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Int13C {
    /// Recursively check nodes for bitwise operations on signed types
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check for binary bitwise operators: &, |, ^, <<, >>
        if node.kind() == "binary_expression" {
            if let Some(operator_node) = node.child_by_field_name("operator") {
                let operator = get_node_text(&operator_node, source);
                
                // Check if this is a bitwise operator
                if self.is_bitwise_operator(operator.trim()) {
                    // Check if operands are signed
                    if let Some(left) = node.child_by_field_name("left") {
                        if self.is_potentially_signed(&left, source) {
                            let position = node.start_position();
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                severity: self.severity(),
                                line: position.row + 1,
                                column: position.column + 1,
                                file_path: String::new(),
                                message: format!(
                                    "Bitwise operator '{}' used on potentially signed operand. Use only unsigned integer types with bitwise operators to avoid implementation-defined behavior.",
                                    operator.trim()
                                ),
                                suggestion: Some(
                                    "Declare the operand as unsigned (e.g., 'unsigned int' instead of 'int')".to_string()
                                ),
                                requires_manual_review: None,
                            });
                        }
                    }
                }
            }
        }
        
        // Check for unary bitwise complement operator: ~
        if node.kind() == "unary_expression" {
            if let Some(operator_node) = node.child_by_field_name("operator") {
                let operator = get_node_text(&operator_node, source);
                
                if operator.trim() == "~" {
                    if let Some(argument) = node.child_by_field_name("argument") {
                        if self.is_potentially_signed(&argument, source) {
                            let position = node.start_position();
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                severity: self.severity(),
                                line: position.row + 1,
                                column: position.column + 1,
                                file_path: String::new(),
                                message: "Bitwise complement operator '~' used on potentially signed operand. Use only unsigned integer types with bitwise operators to avoid implementation-defined behavior.".to_string(),
                                suggestion: Some(
                                    "Declare the operand as unsigned (e.g., 'unsigned int' instead of 'int')".to_string()
                                ),
                                requires_manual_review: None,
                            });
                        }
                    }
                }
            }
        }

        // Recursively check children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }

    /// Check if an operator is a bitwise operator
    fn is_bitwise_operator(&self, op: &str) -> bool {
        matches!(op, "&" | "|" | "^" | "<<" | ">>")
    }

    /// Check if an expression is potentially a signed type
    fn is_potentially_signed(&self, node: &Node, source: &str) -> bool {
        match node.kind() {
            "identifier" => {
                // Check if the identifier looks like a signed declaration
                // For now, we need to track variable declarations
                // This is a simplified check - look for the variable in scope
                self.find_variable_declaration(node, source)
            }
            "number_literal" => {
                // Numeric literals without 'u' suffix are potentially signed
                let text = get_node_text(node, source);
                !text.to_lowercase().contains('u')
            }
            "cast_expression" => {
                // Check the type being cast to
                if let Some(type_node) = node.child_by_field_name("type") {
                    return self.is_signed_type(&type_node, source);
                }
                false
            }
            "binary_expression" | "unary_expression" | "call_expression" => {
                // These could produce signed results - conservatively flag
                true
            }
            _ => false,
        }
    }

    /// Find variable declaration and check if it's signed
    fn find_variable_declaration(&self, identifier: &Node, source: &str) -> bool {
        let var_name = get_node_text(identifier, source);
        
        // Walk up the tree to find declarations
        let mut current = identifier.parent();
        while let Some(parent) = current {
            // Look for declaration in this scope
            if self.is_scope_node(parent.kind()) {
                if let Some(decl) = self.find_declaration_in_scope(&parent, var_name.trim(), source) {
                    return self.is_signed_declaration(&decl, source);
                }
            }
            current = parent.parent();
        }
        
        // If not found, conservatively assume signed
        true
    }

    /// Check if a node represents a scope boundary
    fn is_scope_node(&self, kind: &str) -> bool {
        matches!(
            kind,
            "function_definition" | "compound_statement" | "translation_unit" | "for_statement" | "while_statement" | "if_statement"
        )
    }

    /// Find a declaration in the given scope
    fn find_declaration_in_scope<'a>(
        &self,
        scope: &'a Node<'a>,
        var_name: &str,
        source: &str,
    ) -> Option<Node<'a>> {
        let mut cursor = scope.walk();
        for child in scope.children(&mut cursor) {
            if child.kind() == "declaration" {
                // Check if this declaration contains our variable
                if let Some(declarator) = self.find_declarator_with_name(&child, var_name, source) {
                    return Some(child);
                }
            }
        }
        None
    }

    /// Find a declarator with the given name
    fn find_declarator_with_name<'a>(
        &self,
        decl_node: &'a Node<'a>,
        var_name: &str,
        source: &str,
    ) -> Option<Node<'a>> {
        let mut cursor = decl_node.walk();
        for child in decl_node.children(&mut cursor) {
            if child.kind() == "init_declarator" {
                if let Some(declarator) = child.child_by_field_name("declarator") {
                    let name = self.extract_identifier_from_declarator(&declarator, source);
                    if name == var_name {
                        return Some(declarator);
                    }
                }
            } else if child.kind() == "identifier" {
                if get_node_text(&child, source).trim() == var_name {
                    return Some(child);
                }
            }
        }
        None
    }

    /// Extract identifier name from a declarator
    fn extract_identifier_from_declarator(&self, declarator: &Node, source: &str) -> String {
        if declarator.kind() == "identifier" {
            return get_node_text(declarator, source).to_string();
        }
        
        // Handle pointer declarators, array declarators, etc.
        let mut cursor = declarator.walk();
        for child in declarator.children(&mut cursor) {
            if child.kind() == "identifier" {
                return get_node_text(&child, source).to_string();
            }
        }
        
        String::new()
    }

    /// Check if a declaration is for a signed type
    fn is_signed_declaration(&self, decl_node: &Node, source: &str) -> bool {
        // Look for type specifiers
        let mut cursor = decl_node.walk();
        for child in decl_node.children(&mut cursor) {
            if child.kind() == "type_qualifier" || child.kind() == "storage_class_specifier" {
                continue;
            }
            if child.kind() == "primitive_type" || child.kind() == "sized_type_specifier" {
                return self.is_signed_type(&child, source);
            }
        }
        
        // If no explicit type found, default to signed (int is signed by default)
        true
    }

    /// Check if a type node represents a signed type
    fn is_signed_type(&self, type_node: &Node, source: &str) -> bool {
        let type_text = get_node_text(type_node, source);
        let type_text = type_text.trim();
        
        // Check for explicit unsigned
        if type_text.contains("unsigned") {
            return false;
        }
        
        // Signed types (without unsigned keyword)
        if type_text.contains("int")
            || type_text.contains("char")
            || type_text.contains("short")
            || type_text.contains("long")
        {
            // char, short, int, long without "unsigned" are signed
            // (plain char is implementation-defined, but we'll flag it)
            return true;
        }
        
        // Default: assume signed if uncertain
        true
    }
}

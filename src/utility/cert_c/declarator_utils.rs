// Declarator utilities for CERT C rules
// This module provides reusable functions for analyzing C declarators (arrays, pointers, function pointers)

use tree_sitter::Node;

// ============================================================================
// Declarator Type Checking
// ============================================================================

/// Check if a declarator contains a specific kind of declarator in its tree
/// This is the generic recursive function that powers all specific checks
///
/// # Arguments
/// * `node` - The declarator node to check
/// * `target_kind` - The kind of declarator to search for (e.g., "array_declarator", "pointer_declarator")
///
/// # Returns
/// `true` if the declarator tree contains a node of the target kind
///
/// # Examples
/// ```ignore
/// // Check if field has array declarator
/// if has_declarator_of_kind(&declarator, "array_declarator") {
///     // This is an array field
/// }
/// ```
pub fn has_declarator_of_kind(node: &Node, target_kind: &str) -> bool {
    if node.kind() == target_kind {
        return true;
    }

    // Recursively check children
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if has_declarator_of_kind(&child, target_kind) {
                return true;
            }
        }
    }

    false
}

/// Check if a declarator is an array (has array_declarator)
///
/// # Examples
/// ```ignore
/// int arr[10];      // true
/// int *ptr;         // false
/// int arr[5][10];   // true
/// ```
pub fn is_array_declarator(node: &Node) -> bool {
    has_declarator_of_kind(node, "array_declarator")
}

/// Check if a declarator is a pointer (has pointer_declarator)
///
/// # Examples
/// ```ignore
/// int *ptr;         // true
/// int **ptr;        // true
/// int arr[10];      // false
/// ```
pub fn is_pointer_declarator(node: &Node) -> bool {
    has_declarator_of_kind(node, "pointer_declarator")
}

/// Check if a declarator is a function pointer (has function_declarator)
///
/// # Examples
/// ```ignore
/// int (*fn)(int);   // true
/// int *ptr;         // false
/// ```
pub fn is_function_declarator(node: &Node) -> bool {
    has_declarator_of_kind(node, "function_declarator")
}

#[cfg(test)]
mod tests {
    use super::*;
    use tree_sitter::Parser;

    fn parse_c_code(code: &str) -> tree_sitter::Tree {
        let mut parser = Parser::new();
        let language = tree_sitter_c::language();
        parser.set_language(&language).unwrap();
        parser.parse(code, None).unwrap()
    }

    fn find_declarator(tree: &tree_sitter::Tree) -> Option<Node> {
        let root = tree.root_node();
        // Find first declarator in the tree
        for i in 0..root.child_count() {
            if let Some(child) = root.child(i) {
                if child.kind() == "declaration" {
                    if let Some(declarator) = child.child_by_field_name("declarator") {
                        return Some(declarator);
                    }
                }
            }
        }
        None
    }

    #[test]
    fn test_is_array_declarator() {
        let tree = parse_c_code("int arr[10];");
        let declarator = find_declarator(&tree).unwrap();
        assert!(is_array_declarator(&declarator));
    }

    #[test]
    fn test_is_pointer_declarator() {
        let tree = parse_c_code("int *ptr;");
        let declarator = find_declarator(&tree).unwrap();
        assert!(is_pointer_declarator(&declarator));
    }

    #[test]
    fn test_is_not_array() {
        let tree = parse_c_code("int *ptr;");
        let declarator = find_declarator(&tree).unwrap();
        assert!(!is_array_declarator(&declarator));
    }

    #[test]
    fn test_is_not_pointer() {
        let tree = parse_c_code("int arr[10];");
        let declarator = find_declarator(&tree).unwrap();
        assert!(!is_pointer_declarator(&declarator));
    }

    #[test]
    fn test_multidimensional_array() {
        let tree = parse_c_code("int arr[5][10];");
        let declarator = find_declarator(&tree).unwrap();
        assert!(is_array_declarator(&declarator));
    }

    #[test]
    fn test_double_pointer() {
        let tree = parse_c_code("int **ptr;");
        let declarator = find_declarator(&tree).unwrap();
        assert!(is_pointer_declarator(&declarator));
    }
}

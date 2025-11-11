use super::Str30C;
use crate::parser::CParser;
use crate::rules::CertRule;

#[test]
fn test_str30c_detects_direct_modification() {
    let rule = Str30C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    char *str = "Hello";
    str[0] = 'h';  // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect string literal modification");
    assert!(violations.iter().any(|v| v.message.contains("string literal")));
}

#[test]
fn test_str30c_detects_strcpy_to_literal() {
    let rule = Str30C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    strcpy("fixed", "other");  // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect strcpy to string literal");
    assert!(violations.iter().any(|v| v.message.contains("strcpy")));
}

#[test]
fn test_str30c_detects_pointer_to_literal() {
    let rule = Str30C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    char *ptr = "constant";
    strcpy(ptr, "new");  // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect modification through pointer to literal");
    assert!(violations.iter().any(|v| v.message.contains("pointer to string literal")));
}

#[test]
fn test_str30c_allows_array_modification() {
    let rule = Str30C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    char str[] = "Hello";  // Array, not a pointer to literal
    str[0] = 'h';  // Should NOT trigger violation
    strcpy(str, "new");  // Should NOT trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(violations.is_empty(), "Should not flag modifications to character arrays");
}

#[test]
fn test_str30c_detects_sprintf_to_literal() {
    let rule = Str30C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func(int x) {
    char *str = "fixed";
    sprintf(str, "%d", x);  // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect sprintf to string literal pointer");
    assert!(violations.iter().any(|v| v.message.contains("sprintf")));
}

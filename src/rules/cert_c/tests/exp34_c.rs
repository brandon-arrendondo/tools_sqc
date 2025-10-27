use super::Exp34C;
use crate::parser::CParser;
use crate::rules::CertRule;

#[test]
fn test_exp34c_detects_null_dereference() {
    let rule = Exp34C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    int *ptr = NULL;
    *ptr = 42;          // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect null pointer dereference");
    assert!(violations.iter().any(|v| v.message.contains("null pointer dereference")));
}

#[test]
fn test_exp34c_accepts_null_checked_dereference() {
    let rule = Exp34C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func(int *ptr) {
    if (ptr != NULL) {
        *ptr = 42;      // Should not trigger violation
    }
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    let null_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("null pointer dereference"))
        .collect();
    assert!(null_violations.is_empty(), "Should not flag null-checked dereferences");
}

#[test]
fn test_exp34c_detects_malloc_dereference() {
    let rule = Exp34C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    int *ptr = malloc(sizeof(int));
    *ptr = 42;          // Should trigger violation - malloc can return NULL
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect potential null dereference from malloc");
}

#[test]
fn test_exp34c_detects_array_access() {
    let rule = Exp34C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    int *array = NULL;
    int x = array[0];   // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect null pointer dereference in array access");
}

#[test]
fn test_exp34c_detects_member_access() {
    let rule = Exp34C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
struct Point {
    int x, y;
};

void func() {
    struct Point *p = NULL;
    int x = p->x;       // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect null pointer dereference in member access");
}

#[test]
fn test_exp34c_function_call_with_null() {
    let rule = Exp34C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    char *str = NULL;
    int len = strlen(str);  // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect null pointer passed to function");
}

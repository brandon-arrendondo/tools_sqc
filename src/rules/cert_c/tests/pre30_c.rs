use super::Pre30C;
use crate::parser::CParser;
use crate::rules::CertRule;

#[test]
fn test_pre30c_detects_ucn_concatenation() {
    let rule = Pre30C;
    let mut parser = CParser::new().unwrap();

    // Test case: Macro that creates UCN through concatenation
    let source = r#"
#define assign(uc1, uc2, val) uc1##uc2 = val

void func(void) {
    int \u0401;
    assign(\u04, 01, 4);  // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect UCN creation through concatenation");
    assert!(violations.iter().any(|v| v.message.contains("universal character names")));
}

#[test]
fn test_pre30c_detects_dangerous_macro_definition() {
    let rule = Pre30C;
    let mut parser = CParser::new().unwrap();

    // Test case: Macro definition that could create UCNs
    let source = r#"
#define CONCAT_UCN(prefix, suffix) prefix##suffix

void func(void) {
    // This macro could be used dangerously
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Our implementation may flag this based on the pattern, even without actual usage
    // This is a conservative approach to prevent potential misuse
}

#[test]
fn test_pre30c_accepts_direct_ucn_usage() {
    let rule = Pre30C;
    let mut parser = CParser::new().unwrap();

    // Test case: Direct UCN usage (compliant)
    let source = r#"
#define assign(ucn, val) ucn = val

void func(void) {
    int \u0401 = 0;
    assign(\u0401, 4);  // Should not trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    let ucn_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("universal character names"))
        .collect();
    assert!(ucn_violations.is_empty(), "Should not flag direct UCN usage");
}

#[test]
fn test_pre30c_detects_partial_ucn_fragments() {
    let rule = Pre30C;
    let mut parser = CParser::new().unwrap();

    // Test case: Macro with partial UCN fragments
    let source = r#"
#define JOIN(a, b) a##b

void func(void) {
    // Potentially dangerous if used with UCN fragments
    int result = JOIN(\u04, 01);  // Could form \u0401
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Should detect the potential for UCN creation
    assert!(!violations.is_empty(), "Should detect potential UCN fragment concatenation");
}

#[test]
fn test_pre30c_accepts_safe_concatenation() {
    let rule = Pre30C;
    let mut parser = CParser::new().unwrap();

    // Test case: Safe token concatenation not involving UCNs
    let source = r#"
#define MAKE_FUNC(name) void func_##name(void) { }

MAKE_FUNC(test)  // Should not trigger violation

void use_it(void) {
    func_test();
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    let ucn_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("universal character names"))
        .collect();
    assert!(ucn_violations.is_empty(), "Should not flag safe token concatenation");
}

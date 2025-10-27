use super::Pre32C;
use crate::parser::CParser;
use crate::rules::CertRule;

#[test]
fn test_pre32c_detects_preprocessor_in_memcpy() {
    let rule = Pre32C;
    let mut parser = CParser::new().unwrap();

    // Test case: Preprocessor directive in memcpy arguments (classic example)
    let source = r#"
#include <string.h>

void func(const char *src) {
    char *dest;
    memcpy(dest, src,
        #ifdef PLATFORM1
            12
        #else
            24
        #endif
    );  // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect preprocessor directive in memcpy arguments");
    assert!(violations.iter().any(|v| v.message.contains("preprocessor directives")));
}

#[test]
fn test_pre32c_detects_ifdef_in_printf() {
    let rule = Pre32C;
    let mut parser = CParser::new().unwrap();

    // Test case: Preprocessor directive in printf arguments
    let source = r#"
#include <stdio.h>

void debug_print(int value) {
    printf("Value: %d\n",
        #ifdef DEBUG
            value
        #else
            0
        #endif
    );  // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect preprocessor directive in printf arguments");
}

#[test]
fn test_pre32c_detects_define_in_assert() {
    let rule = Pre32C;
    let mut parser = CParser::new().unwrap();

    // Test case: Preprocessor directive in assert
    let source = r#"
#include <assert.h>

void func(void) {
    assert(
        #define TEMP_VAL 42
        TEMP_VAL > 0
    );  // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect #define in assert arguments");
}

#[test]
fn test_pre32c_accepts_compliant_conditional_calls() {
    let rule = Pre32C;
    let mut parser = CParser::new().unwrap();

    // Test case: Compliant solution - preprocessor outside function call
    let source = r#"
#include <string.h>

void func(const char *src) {
    char *dest;
    #ifdef PLATFORM1
        memcpy(dest, src, 12);
    #else
        memcpy(dest, src, 24);
    #endif
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(violations.is_empty(), "Should not flag compliant preprocessor usage");
}

use super::Fio30C;
use crate::parser::CParser;
use crate::rules::CertRule;

#[test]
fn test_fio30c_detects_direct_argv_usage() {
    let rule = Fio30C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
int main(int argc, char *argv[]) {
    printf(argv[1]);  // Should trigger violation
    return 0;
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect argv used as format string");
    assert!(violations.iter().any(|v| v.message.contains("User input used as format string")));
}

#[test]
fn test_fio30c_accepts_literal_format_string() {
    let rule = Fio30C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
int main(int argc, char *argv[]) {
    printf("Hello, %s!\n", argv[1]);  // Should NOT trigger violation
    return 0;
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(violations.is_empty(), "Should not flag literal format strings with user data as arguments");
}

#[test]
fn test_fio30c_detects_user_input_variable() {
    let rule = Fio30C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    char user_input[256];
    fgets(user_input, sizeof(user_input), stdin);
    printf(user_input);  // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect user input variable used as format string");
    assert!(violations.iter().any(|v| v.message.contains("User input used as format string")));
}

#[test]
fn test_fio30c_detects_fprintf_vulnerability() {
    let rule = Fio30C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func(char *user_data) {
    fprintf(stderr, user_data);  // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect fprintf with user data as format string");
    assert!(violations.iter().any(|v| v.message.contains("fprintf")));
}

#[test]
fn test_fio30c_detects_sprintf_vulnerability() {
    let rule = Fio30C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    char buffer[256];
    char *input = getenv("USER_INPUT");
    sprintf(buffer, input);  // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect sprintf with getenv result as format string");
    assert!(violations.iter().any(|v| v.message.contains("sprintf")));
}

#[test]
fn test_fio30c_accepts_safe_usage() {
    let rule = Fio30C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    char *safe_format = "Value: %d\n";
    int value = 42;
    printf(safe_format, value);  // Should NOT trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(violations.is_empty(), "Should not flag safe format string usage");
}

#[test]
fn test_fio30c_detects_assigned_user_input() {
    let rule = Fio30C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
int main(int argc, char *argv[]) {
    char *format_str = argv[1];
    printf(format_str);  // Should trigger violation
    return 0;
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect assigned user input used as format string");
}

#[test]
fn test_fio30c_accepts_snprintf_with_literal_format() {
    let rule = Fio30C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
#include <stdio.h>

void func() {
    char buffer[100];
    char product[50];
    double price = 19.99;

    // These should NOT trigger violations - literal format strings
    snprintf(buffer, sizeof(buffer), "Product: %s - Price: $%.2f", product, price);
    sprintf(buffer, "Test: %s", product);
    printf("Value: %d\n", 42);
    fprintf(stderr, "Error: %s\n", "message");
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    for v in &violations {
        println!("Unexpected violation: {}", v.message);
    }
    assert!(violations.is_empty(), "Should not flag literal format strings");
}

#[test]
fn test_fio30c_detects_variable_format_strings() {
    let rule = Fio30C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
#include <stdio.h>

void func() {
    char user_format[100];
    char buffer[200];

    fgets(user_format, sizeof(user_format), stdin);

    // This SHOULD trigger a violation - user input as format string
    sprintf(buffer, user_format, "data");
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect user input as format string");
    assert!(violations.iter().any(|v| v.message.contains("User input used as format string")));
}

#[test]
fn test_fio30c_accepts_various_literal_formats() {
    let rule = Fio30C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
#include <stdio.h>

void func() {
    char buffer[200];

    // All of these should be safe - they use literal format strings
    printf("Simple message");
    printf("Message with arg: %s", "literal");
    snprintf(buffer, 200, "Complex format: %d %s %.2f", 42, "text", 3.14);
    fprintf(stdout, "To stdout: %s", "message");
    sprintf(buffer, "Formatted: %x", 255);
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    for v in &violations {
        println!("Unexpected violation: {}", v.message);
    }
    assert!(violations.is_empty(), "Should accept all literal format strings");
}

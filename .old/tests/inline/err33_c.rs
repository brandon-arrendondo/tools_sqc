use super::Err33C;
use crate::parser::CParser;
use crate::rules::CertRule;

#[test]
fn test_err33c_detects_unchecked_malloc() {
    let rule = Err33C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    void *ptr = malloc(100);  // Should trigger violation
    *((int*)ptr) = 42;
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect unchecked malloc");
    assert!(violations.iter().any(|v| v.message.contains("malloc") && v.message.contains("NULL")));
}

#[test]
fn test_err33c_detects_ignored_fopen() {
    let rule = Err33C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    fopen("file.txt", "r");  // Should trigger violation - return value ignored
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect ignored fopen return value");
    assert!(violations.iter().any(|v| v.message.contains("fopen") && v.message.contains("ignored")));
}

#[test]
fn test_err33c_detects_unchecked_fseek() {
    let rule = Err33C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    FILE *file = fopen("test.txt", "r");
    if (file != NULL) {
        int result = fseek(file, 100, SEEK_SET);  // Should trigger violation
        fclose(file);
    }
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect unchecked fseek");
    assert!(violations.iter().any(|v| v.message.contains("fseek")));
}

#[test]
fn test_err33c_accepts_checked_malloc() {
    let rule = Err33C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    void *ptr = malloc(100);
    if (ptr == NULL) {
        return;
    }
    *((int*)ptr) = 42;
    free(ptr);
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Should have fewer or no violations due to proper checking
    let malloc_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("malloc"))
        .collect();
    assert!(malloc_violations.is_empty(), "Should not flag properly checked malloc");
}

#[test]
fn test_err33c_accepts_checked_fopen() {
    let rule = Err33C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    FILE *file = fopen("test.txt", "r");
    if (file == NULL) {
        return;
    }
    fclose(file);
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Should have fewer or no violations due to proper checking
    let fopen_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("fopen"))
        .collect();
    assert!(fopen_violations.is_empty(), "Should not flag properly checked fopen");
}

#[test]
fn test_err33c_accepts_printf_in_condition() {
    let rule = Err33C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    if (printf("Hello, World!") < 0) {
        // Handle error
    }
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Should not flag printf when used in condition
    let printf_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("printf"))
        .collect();
    assert!(printf_violations.is_empty(), "Should not flag printf used in condition");
}

#[test]
fn test_err33c_detects_unchecked_snprintf() {
    let rule = Err33C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    char buffer[10];
    int result = snprintf(buffer, sizeof(buffer), "%s", "long string");  // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect unchecked snprintf");
    assert!(violations.iter().any(|v| v.message.contains("snprintf")));
}

#[test]
fn test_err33c_accepts_checked_snprintf() {
    let rule = Err33C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    char buffer[10];
    int result = snprintf(buffer, sizeof(buffer), "%s", "long string");
    if (result < 0 || result >= sizeof(buffer)) {
        // Handle error or truncation
    }
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Should have fewer violations due to proper checking
    let snprintf_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("snprintf"))
        .collect();
    assert!(snprintf_violations.is_empty(), "Should not flag properly checked snprintf");
}

#[test]
fn test_err33c_detects_unchecked_strtol() {
    let rule = Err33C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    char *str = "123";
    long value = strtol(str, NULL, 10);  // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect unchecked strtol");
    assert!(violations.iter().any(|v| v.message.contains("strtol")));
}

#[test]
fn test_err33c_ignores_safe_functions() {
    let rule = Err33C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    putchar('A');     // Safe to ignore
    puts("Hello");    // Safe to ignore
    memcpy(dest, src, 10);  // Cannot fail
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Should not flag functions that are safe to ignore
    assert!(violations.is_empty(), "Should not flag functions that are safe to ignore");
}

// New comprehensive test cases to validate fixes

#[test]
fn test_err33c_accepts_fopen_with_immediate_check() {
    let rule = Err33C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    FILE *file = fopen("test.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file\n");
        return;
    }
    fclose(file);
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Should not flag properly checked fopen
    let fopen_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("fopen"))
        .collect();
    assert!(fopen_violations.is_empty(), "Should not flag properly checked fopen with immediate check");
}

#[test]
fn test_err33c_accepts_fgets_with_check() {
    let rule = Err33C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    char buffer[100];
    FILE *file = fopen("test.txt", "r");
    if (file != NULL) {
        if (fgets(buffer, sizeof(buffer), file) != NULL) {
            process(buffer);
        }
        fclose(file);
    }
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Should not flag properly checked fgets
    let fgets_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("fgets"))
        .collect();
    assert!(fgets_violations.is_empty(), "Should not flag properly checked fgets");
}

#[test]
fn test_err33c_accepts_fclose_with_check() {
    let rule = Err33C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    FILE *file = fopen("test.txt", "w");
    if (file != NULL) {
        fputs("test", file);
        if (fclose(file) != 0) {
            handle_error();
        }
    }
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Should not flag properly checked fclose
    let fclose_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("fclose"))
        .collect();
    assert!(fclose_violations.is_empty(), "Should not flag properly checked fclose");
}

#[test]
fn test_err33c_detects_unchecked_fopen() {
    let rule = Err33C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    FILE *file = fopen("test.txt", "r");  // Should trigger violation
    fwrite(data, 1, 10, file);            // Use without checking
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect unchecked fopen");
    assert!(violations.iter().any(|v| v.message.contains("fopen")));
}

#[test]
fn test_err33c_detects_ignored_fgets_return() {
    let rule = Err33C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    char buffer[100];
    FILE *file = fopen("test.txt", "r");
    fgets(buffer, sizeof(buffer), file);  // Return value ignored
    process(buffer);
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect ignored fgets return value");
    assert!(violations.iter().any(|v| v.message.contains("fgets")));
}

#[test]
fn test_err33c_accepts_printf_in_error_context() {
    let rule = Err33C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    FILE *file = fopen("test.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file\n");  // Should not flag - error context
        return;
    }
    fclose(file);
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Should not flag fprintf in error handling context
    let fprintf_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("fprintf"))
        .collect();
    assert!(fprintf_violations.is_empty(), "Should not flag fprintf in error handling context");
}

#[test]
fn test_err33c_detects_multiple_unchecked_functions() {
    let rule = Err33C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    void *ptr = malloc(100);      // Should trigger violation
    FILE *file = fopen("test", "r"); // Should trigger violation
    char *str = fgets(buffer, 100, file); // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(violations.len() >= 3, "Should detect multiple unchecked functions");
    assert!(violations.iter().any(|v| v.message.contains("malloc")));
    assert!(violations.iter().any(|v| v.message.contains("fopen")));
    assert!(violations.iter().any(|v| v.message.contains("fgets")));
}

#[test]
fn test_err33c_file_open_check_debug() {
    let rule = Err33C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    FILE *file = fopen("test.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file\n");
        return 1;
    }
    fclose(file);
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Print all violations for debugging
    for violation in &violations {
        println!("VIOLATION: {}", violation.message);
    }

    // Should not flag fopen since it's properly checked
    let fopen_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("fopen"))
        .collect();
    assert!(fopen_violations.is_empty(), "Should not flag properly checked fopen");
}

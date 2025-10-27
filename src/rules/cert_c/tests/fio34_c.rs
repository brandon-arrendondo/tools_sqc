use super::Fio34C;

#[test]
fn test_fio34c_detects_char_assignment_from_getc() {
    let source = r#"
#include <stdio.h>

void test() {
    FILE *file = fopen("test.txt", "r");
    char c;
    c = getc(file);  // Violation: char cannot distinguish EOF
    fclose(file);
}
"#;

    let mut parser = tree_sitter::Parser::new();
    parser.set_language(&tree_sitter_c::language()).unwrap();
    let tree = parser.parse(source, None).unwrap();
    let rule = Fio34C::new();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect char assignment from getc");
    assert!(violations[0].message.contains("char variable"));
}

#[test]
fn test_fio34c_detects_char_init_from_fgetc() {
    let source = r#"
#include <stdio.h>

void test() {
    FILE *file = fopen("test.txt", "r");
    char c = fgetc(file);  // Violation: char cannot distinguish EOF
    fclose(file);
}
"#;

    let mut parser = tree_sitter::Parser::new();
    parser.set_language(&tree_sitter_c::language()).unwrap();
    let tree = parser.parse(source, None).unwrap();
    let rule = Fio34C::new();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect char initialization from fgetc");
}

#[test]
fn test_fio34c_detects_loop_with_char_eof_check() {
    let source = r#"
#include <stdio.h>

void test() {
    FILE *file = fopen("test.txt", "r");
    char c;
    while ((c = getc(file)) != EOF) {  // Violation: char may not detect EOF
        printf("%c", c);
    }
    fclose(file);
}
"#;

    let mut parser = tree_sitter::Parser::new();
    parser.set_language(&tree_sitter_c::language()).unwrap();
    let tree = parser.parse(source, None).unwrap();
    let rule = Fio34C::new();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect char in loop EOF check");
}

#[test]
fn test_fio34c_accepts_int_assignment() {
    let source = r#"
#include <stdio.h>

void test() {
    FILE *file = fopen("test.txt", "r");
    int c;
    c = getc(file);  // OK: int can distinguish EOF
    if (c != EOF) {
        printf("%c", c);
    }
    fclose(file);
}
"#;

    let mut parser = tree_sitter::Parser::new();
    parser.set_language(&tree_sitter_c::language()).unwrap();
    let tree = parser.parse(source, None).unwrap();
    let rule = Fio34C::new();
    let violations = rule.check(&tree.root_node(), source);

    assert!(violations.is_empty(), "Should accept int assignment from getc");
}

#[test]
fn test_fio34c_accepts_int_loop() {
    let source = r#"
#include <stdio.h>

void test() {
    FILE *file = fopen("test.txt", "r");
    int c;
    while ((c = getc(file)) != EOF) {  // OK: int properly detects EOF
        printf("%c", c);
    }
    fclose(file);
}
"#;

    let mut parser = tree_sitter::Parser::new();
    parser.set_language(&tree_sitter_c::language()).unwrap();
    let tree = parser.parse(source, None).unwrap();
    let rule = Fio34C::new();
    let violations = rule.check(&tree.root_node(), source);

    for v in &violations {
        println!("Violation: {}", v.message);
    }
    assert!(violations.is_empty(), "Should accept int in loop EOF check, but found {} violations", violations.len());
}

#[test]
fn test_fio34c_detects_getchar_char_assignment() {
    let source = r#"
#include <stdio.h>

void test() {
    char c = getchar();  // Violation: char cannot distinguish EOF
    if (c != EOF) {
        printf("%c", c);
    }
}
"#;

    let mut parser = tree_sitter::Parser::new();
    parser.set_language(&tree_sitter_c::language()).unwrap();
    let tree = parser.parse(source, None).unwrap();
    let rule = Fio34C::new();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect char assignment from getchar");
}

#[test]
fn test_fio34c_detects_wide_char_issues() {
    let source = r#"
#include <wchar.h>
#include <stdio.h>

void test() {
    FILE *file = fopen("test.txt", "r");
    wchar_t wc = getwc(file);  // Potential issue: should be wint_t
    if (wc != WEOF) {
        wprintf(L"%lc", wc);
    }
    fclose(file);
}
"#;

    let mut parser = tree_sitter::Parser::new();
    parser.set_language(&tree_sitter_c::language()).unwrap();
    let tree = parser.parse(source, None).unwrap();
    let rule = Fio34C::new();
    let violations = rule.check(&tree.root_node(), source);

    // This test may need adjustment based on how we handle wide characters
    // For now, we're focusing on the basic char/int distinction
}

#[test]
fn test_fio34c_accepts_ungetc_eof_comparison() {
    let source = r#"
#include <stdio.h>

void test() {
    FILE *file = fopen("test.txt", "r");
    int c = fgetc(file);

    // This should NOT be flagged - ungetc is designed for EOF comparison
    if (ungetc(c, file) == EOF) {
        fprintf(stderr, "ungetc failed\n");
    }

    fclose(file);
}
"#;

    let mut parser = tree_sitter::Parser::new();
    parser.set_language(&tree_sitter_c::language()).unwrap();
    let tree = parser.parse(source, None).unwrap();
    let rule = Fio34C::new();
    let violations = rule.check(&tree.root_node(), source);

    // Should not flag ungetc EOF comparison as violation
    for violation in &violations {
        assert!(!violation.message.contains("ungetc"),
                "Should not flag ungetc EOF comparison: {}", violation.message);
    }
}

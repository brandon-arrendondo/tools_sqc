use super::Arr39C;
use crate::parser::CParser;
use crate::rules::CertRule;

#[test]
fn test_arr39c_detects_sizeof_scaling() {
    let rule = Arr39C;
    let mut parser = CParser::new().unwrap();

    // Test case: Pointer arithmetic with sizeof scaling
    let source = r#"
void func(void) {
    int buf[10];
    int *buf_ptr = buf;

    while (buf_ptr < (buf + sizeof(buf))) {  // Should trigger violation
        *buf_ptr++ = getdata();
    }
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect sizeof scaling in pointer arithmetic");
    assert!(violations.iter().any(|v| v.message.contains("double scaling")));
}

#[test]
fn test_arr39c_detects_offsetof_scaling() {
    let rule = Arr39C;
    let mut parser = CParser::new().unwrap();

    // Test case: Using offsetof with scaling
    let source = r#"
struct big {
    int a;
    long long ull_b;
};

void func(void) {
    size_t skip = offsetof(struct big, ull_b);
    struct big *s = (struct big *)malloc(sizeof(struct big));
    memset(s + skip, 0, sizeof(struct big) - skip);  // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect offsetof scaling issue");
}

#[test]
fn test_arr39c_detects_wide_char_scaling() {
    let rule = Arr39C;
    let mut parser = CParser::new().unwrap();

    // Test case: Wide character string scaling
    let source = r#"
void func(void) {
    wchar_t error_msg[100];
    size_t prefix_len = 7;

    fgetws(error_msg + wcslen(error_msg) * sizeof(wchar_t),
           100 - 7, stdin);  // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect wide character scaling issue");
    assert!(violations.iter().any(|v| v.message.contains("scaled arithmetic")));
}

#[test]
fn test_arr39c_detects_pointer_assignment_scaling() {
    let rule = Arr39C;
    let mut parser = CParser::new().unwrap();

    // Test case: Pointer assignment with scaling
    let source = r#"
void func(void) {
    int *ptr = buffer;
    ptr += 2 * sizeof(int);  // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect scaled pointer assignment");
}

#[test]
fn test_arr39c_accepts_unscaled_arithmetic() {
    let rule = Arr39C;
    let mut parser = CParser::new().unwrap();

    // Test case: Correct unscaled pointer arithmetic
    let source = r#"
void func(void) {
    int buf[10];
    int *buf_ptr = buf;
    const int BUFSIZE = 10;

    while (buf_ptr < (buf + BUFSIZE)) {  // Should not trigger violation
        *buf_ptr++ = getdata();
    }
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    let scaling_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("double scaling") || v.message.contains("scaled"))
        .collect();
    assert!(scaling_violations.is_empty(), "Should not flag unscaled pointer arithmetic");
}

#[test]
fn test_arr39c_accepts_proper_char_pointer() {
    let rule = Arr39C;
    let mut parser = CParser::new().unwrap();

    // Test case: Using char* to avoid scaling issues
    let source = r#"
void func(void) {
    size_t skip = offsetof(struct big, ull_b);
    unsigned char *ptr = (unsigned char *)malloc(sizeof(struct big));
    memset(ptr + skip, 0, sizeof(struct big) - skip);  // Should not trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Using char* should avoid the scaling issue
    let scaling_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("scaled"))
        .collect();
    assert!(scaling_violations.is_empty(), "Should not flag char* pointer arithmetic");
}

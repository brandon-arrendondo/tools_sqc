use super::Mem30C;
use crate::parser::CParser;
use crate::rules::CertRule;
use crate::manifest::Severity;

#[test]
fn test_mem30c_detects_use_after_free() {
    let rule = Mem30C;
    let mut parser = CParser::new().unwrap();

    // Test case: Basic use-after-free
    let source = r#"
void func(char *buf) {
    free(buf);
    strcpy(buf, "data");  // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect use-after-free");
    assert!(violations.iter().any(|v| v.message.contains("Use-after-free")));
}

#[test]
fn test_mem30c_detects_double_free() {
    let rule = Mem30C;
    let mut parser = CParser::new().unwrap();

    // Test case: Double free
    let source = r#"
void func(char *ptr) {
    free(ptr);
    free(ptr);  // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect double-free");
    assert!(violations.iter().any(|v| v.message.contains("Double-free")));
}

#[test]
fn test_mem30c_detects_dangerous_realloc() {
    let rule = Mem30C;
    let mut parser = CParser::new().unwrap();

    // Test case: Dangerous realloc pattern
    let source = r#"
void func(char *ptr, size_t new_size) {
    ptr = realloc(ptr, new_size);  // Should trigger violation
    if (ptr == NULL) {
        // Memory leak if realloc fails
    }
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect dangerous realloc pattern");
    assert!(violations.iter().any(|v| v.message.contains("realloc")));
}

#[test]
fn test_mem30c_detects_linked_list_free_error() {
    let rule = Mem30C;
    let mut parser = CParser::new().unwrap();

    // Test case: Linked list free error
    let source = r#"
void free_list(struct node *head) {
    for (struct node *p = head; p != NULL; p = p->next) {
        free(p);  // Should trigger violation - accessing p->next after free
    }
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect linked list free error");
}

#[test]
fn test_mem30c_accepts_safe_memory_usage() {
    let rule = Mem30C;
    let mut parser = CParser::new().unwrap();

    // Test case: Safe memory usage
    let source = r#"
void func(char *buf) {
    strcpy(buf, "data");  // Use before free
    free(buf);
    buf = NULL;  // Good practice
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Should have fewer violations for safe usage
    let critical_violations: Vec<_> = violations.iter()
        .filter(|v| matches!(v.severity, Severity::Critical))
        .collect();
    assert!(critical_violations.is_empty(), "Should not flag safe memory usage");
}

#[test]
fn test_mem30c_accepts_safe_linked_list_free() {
    let rule = Mem30C;
    let mut parser = CParser::new().unwrap();

    // Test case: Safe linked list freeing
    let source = r#"
void free_list(struct node *head) {
    struct node *next;
    for (struct node *p = head; p != NULL; p = next) {
        next = p->next;  // Save next before freeing
        free(p);
    }
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Should have fewer violations for safe pattern
    let loop_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("loop"))
        .collect();
    assert!(loop_violations.is_empty(), "Should not flag safe linked list freeing");
}

#[test]
fn test_mem30c_accepts_safe_realloc() {
    let rule = Mem30C;
    let mut parser = CParser::new().unwrap();

    // Test case: Safe realloc usage
    let source = r#"
void func(char *ptr, size_t new_size) {
    char *new_ptr = realloc(ptr, new_size);  // Use temporary variable
    if (new_ptr == NULL) {
        free(ptr);  // Handle failure safely
        return;
    }
    ptr = new_ptr;
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Should have fewer violations for safe realloc pattern
    let realloc_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("realloc"))
        .collect();
    assert!(realloc_violations.is_empty(), "Should not flag safe realloc usage");
}

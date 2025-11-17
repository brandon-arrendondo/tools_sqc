use super::Arr36C;
use crate::parser::CParser;
use crate::rules::CertRule;

#[test]
fn test_arr36c_detects_different_array_subtraction() {
    let rule = Arr36C;
    let mut parser = CParser::new().unwrap();

    // Test case: Pointer subtraction between different arrays
    let source = r#"
void func(void) {
    int nums[10];
    int end;
    int *next_num_ptr = nums;
    size_t free_elements;

    free_elements = &end - next_num_ptr;  // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect pointer subtraction between different arrays");
    assert!(violations.iter().any(|v| v.message.contains("different arrays")));
}

#[test]
fn test_arr36c_accepts_same_array_subtraction() {
    let rule = Arr36C;
    let mut parser = CParser::new().unwrap();

    // Test case: Valid pointer subtraction within same array
    let source = r#"
void func(void) {
    int nums[10];
    int *start_ptr = nums;
    int *end_ptr = &nums[10];
    size_t elements;

    elements = end_ptr - start_ptr;  // Should not trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    let array_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("different arrays"))
        .collect();
    assert!(array_violations.is_empty(), "Should not flag same-array pointer operations");
}

#[test]
fn test_arr36c_detects_different_array_comparison() {
    let rule = Arr36C;
    let mut parser = CParser::new().unwrap();

    // Test case: Pointer comparison between different arrays
    let source = r#"
void func(void) {
    int array1[10];
    int array2[10];
    int *ptr1 = array1;
    int *ptr2 = array2;

    if (ptr1 < ptr2) {  // Should trigger violation
        /* do something */
    }
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);
    assert!(!violations.is_empty(), "Should detect pointer comparison between different arrays");
}

#[test]
fn test_arr36c_struct_member_arithmetic() {
    let rule = Arr36C;
    let mut parser = CParser::new().unwrap();

    // Test case: Pointer arithmetic on structure members (related violation)
    let source = r#"
struct data {
    int a;
    int b;
};

void func(void) {
    struct data d;
    int *ptr1 = &d.a;
    int *ptr2 = &d.b;

    ptrdiff_t diff = ptr2 - ptr1;  // Potentially problematic
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // This should potentially be flagged as structure members are not guaranteed
    // to be in the same "array" for the purposes of this rule
    assert!(!violations.is_empty(), "Should detect struct member pointer arithmetic");
}

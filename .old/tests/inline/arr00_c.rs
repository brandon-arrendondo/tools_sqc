use super::Arr00C;
use crate::manifest::Severity;
use crate::parser::CParser;
use crate::rules::CertRule;

#[test]
fn test_arr00c_detects_direct_array_assignment() {
    let rule = Arr00C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    int arr1[10];
    int arr2[10];
    arr1 = arr2;  // Should trigger violation
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect direct array assignment");
    assert!(violations[0].message.contains("Cannot directly assign arrays"));
}


#[test]
fn test_arr00c_detects_array_comparison() {
    let rule = Arr00C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    int arr1[10];
    int arr2[10];
    if (arr1 == arr2) {  // Should trigger violation
        // This compares addresses, not contents
    }
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect array comparison");
    assert!(violations[0].message.contains("compares addresses, not contents"));
}

#[test]
fn test_arr00c_detects_sizeof_misuse() {
    let rule = Arr00C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func(int arr[]) {
    size_t size = sizeof(arr);  // Should trigger violation - arr is a pointer here
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);


    let sizeof_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("sizeof"))
        .collect();
    assert!(!sizeof_violations.is_empty(), "Should detect sizeof misuse on array parameter");
}

#[test]
fn test_arr00c_detects_sizeof_misuse_with_array_size() {
    let rule = Arr00C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void modify_array(int arr[100]) {
    size_t size = sizeof(arr) / sizeof(arr[0]);  // Wrong! arr is a pointer
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    let sizeof_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("sizeof"))
        .collect();
    assert!(!sizeof_violations.is_empty(), "Should detect sizeof misuse even with explicit array size");
}

#[test]
fn test_arr00c_allows_safe_operations() {
    let rule = Arr00C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void func() {
    int arr1[10];
    int arr2[10];

    // These should be allowed
    arr1[0] = arr2[0];  // Element assignment
    memcpy(arr1, arr2, sizeof(arr1));  // Safe copy

    if (memcmp(arr1, arr2, sizeof(arr1)) == 0) {  // Safe comparison
        // Arrays are equal
    }

    char dest[100];
    char src[50];
    strncpy(dest, src, sizeof(dest) - 1);  // Bounded copy
    dest[sizeof(dest) - 1] = '\0';
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Should not flag safe operations (no High/Critical violations expected)
    let dangerous_violations: Vec<_> = violations.iter()
        .filter(|v| matches!(v.severity, Severity::High | Severity::Critical))
        .collect();
    assert!(dangerous_violations.is_empty(), "Should not flag safe array operations as dangerous");
}

#[test]
fn test_arr00c_checks_nested_contexts() {
    let rule = Arr00C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void outer() {
    int arr1[5], arr2[5];
    if (1) {
        arr1 = arr2;  // Should detect in nested block
    }
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect violations in nested contexts");
}

#[test]
fn test_arr00c_detects_zero_size_vla() {
    let rule = Arr00C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
int main() {
    int size = 0;
    int vla[size];  // Should trigger violation - VLA with size 0

    vla[0] = 100;

    return 0;
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect VLA with zero size");
    let vla_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("size") || v.message.contains("0"))
        .collect();
    assert!(!vla_violations.is_empty(), "Should detect VLA size issue");
}

#[test]
fn test_arr00c_detects_unvalidated_vla() {
    let rule = Arr00C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void create_vla(int size) {
    int vla[size];  // Should trigger violation - unvalidated parameter

    for (int i = 0; i < size; i++) {
        vla[i] = i;
    }
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect unvalidated VLA parameter");
    let vla_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("unvalidated") || v.message.contains("parameter"))
        .collect();
    assert!(!vla_violations.is_empty(), "Should detect unvalidated VLA");
}

#[test]
fn test_arr00c_allows_validated_vla() {
    let rule = Arr00C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void process_vla(int n) {
    if (n <= 0 || n > 1000) {
        return;
    }

    int vla[n];  // Should be OK - size is validated

    for (int i = 0; i < n; i++) {
        vla[i] = i;
    }
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Should not flag validated VLA
    let vla_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("VLA") || v.message.contains("Variable Length"))
        .collect();
    assert!(vla_violations.is_empty(), "Should not flag validated VLA");
}

#[test]
fn test_arr00c_detects_gets_usage() {
    let rule = Arr00C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
#include <stdio.h>

int main() {
    char buffer[50];

    printf("Enter input: ");
    gets(buffer);  // Should trigger critical violation

    printf("You entered: %s\n", buffer);

    return 0;
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect gets() usage");
    let gets_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("gets"))
        .collect();
    assert!(!gets_violations.is_empty(), "Should detect gets() as dangerous");
    assert!(matches!(gets_violations[0].severity, Severity::Critical));
}

#[test]
fn test_arr00c_allows_safe_and_validated_string_operations() {
    let rule = Arr00C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
#include <stdio.h>
#include <string.h>

void func() {
    char dest[100];
    char src[50];

    // Safe bounded operations - these are OK
    strncpy(dest, src, sizeof(dest) - 1);
    dest[sizeof(dest) - 1] = '\0';

    strncat(dest, src, sizeof(dest) - strlen(dest) - 1);

    snprintf(dest, sizeof(dest), "%s", src);

    fgets(dest, sizeof(dest), stdin);

    // Validated strcpy - shows understanding of arrays
    if (strlen(src) < sizeof(dest)) {
        strcpy(dest, src);
    }
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Should not flag safe/validated operations
    // (strcpy/strcat/sprintf with validation shows understanding - covered by ARR38-C)
    let dangerous_violations: Vec<_> = violations.iter()
        .filter(|v| matches!(v.severity, Severity::High | Severity::Critical))
        .collect();
    assert!(dangerous_violations.is_empty(), "Should not flag safe or validated string operations");
}

#[test]
fn test_arr00c_detects_unvalidated_input_loop() {
    let rule = Arr00C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
#include <stdio.h>

int main() {
    int data[100];
    int count;

    printf("How many numbers? ");
    scanf("%d", &count);

    for (int i = 0; i < count; i++) {
        scanf("%d", &data[i]);
    }

    return 0;
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect unvalidated user input in loop");
    let input_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("unvalidated") && v.message.contains("count"))
        .collect();
    assert!(!input_violations.is_empty(), "Should detect 'count' as unvalidated");
}

#[test]
fn test_arr00c_allows_validated_input_loop() {
    let rule = Arr00C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
#include <stdio.h>

#define MAX_SIZE 100

int main() {
    int data[MAX_SIZE];
    int count;

    printf("How many numbers? ");
    scanf("%d", &count);

    if (count < 0 || count > MAX_SIZE) {
        printf("Invalid count\n");
        return 1;
    }

    for (int i = 0; i < count; i++) {
        scanf("%d", &data[i]);
    }

    return 0;
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Should not flag validated input
    let input_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("unvalidated"))
        .collect();
    assert!(input_violations.is_empty(), "Should not flag validated user input");
}

#[test]
fn test_arr00c_detects_uninitialized_loop_bound() {
    let rule = Arr00C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
#include <stdio.h>

int main() {
    int size;
    int arr[10];

    for (int i = 0; i < size; i++) {
        arr[i] = i;
    }

    return 0;
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect uninitialized variable in loop");
    let uninitialized_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("uninitialized") && v.message.contains("size"))
        .collect();
    assert!(!uninitialized_violations.is_empty(), "Should detect 'size' as uninitialized");
}

#[test]
fn test_arr00c_allows_initialized_loop_bound() {
    let rule = Arr00C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
#include <stdio.h>

int main() {
    int size = 10;
    int arr[10];

    for (int i = 0; i < size; i++) {
        arr[i] = i;
    }

    return 0;
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Should not flag initialized variable
    let uninitialized_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("uninitialized"))
        .collect();
    assert!(uninitialized_violations.is_empty(), "Should not flag initialized variable");
}

#[test]
fn test_arr00c_detects_pointer_past_end() {
    let rule = Arr00C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
#include <stdio.h>

int main() {
    int arr[5] = {1, 2, 3, 4, 5};
    int *ptr = arr;

    ptr = arr + 10;  // Should trigger - way past end
    *ptr = 100;

    return 0;
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect pointer past array end");
    let pointer_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("past the end") || v.message.contains("exceeds"))
        .collect();
    assert!(!pointer_violations.is_empty(), "Should detect pointer arithmetic violation");
}

#[test]
fn test_arr00c_allows_valid_pointer_arithmetic() {
    let rule = Arr00C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
#include <stdio.h>

int main() {
    int arr[10] = {0};
    int *ptr;

    // Valid pointer arithmetic within bounds
    ptr = arr + 5;
    *ptr = 42;

    // One past the end is allowed (but shouldn't dereference)
    ptr = arr + 10;

    return 0;
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Should not flag valid pointer arithmetic (arr + 5 for arr[10])
    // Note: arr + 10 is one-past-the-end which is allowed (just can't dereference)
    let pointer_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("past the end") || v.message.contains("exceeds"))
        .collect();
    assert!(pointer_violations.is_empty(), "Should not flag valid pointer arithmetic");
}

#[test]
fn test_arr00c_detects_unvalidated_parameter_subscript() {
    let rule = Arr00C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void update_array(int arr[], int index, int value) {
    arr[index] = value;
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    assert!(!violations.is_empty(), "Should detect unvalidated parameter as array index");
    let subscript_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("parameter") && v.message.contains("index"))
        .collect();
    assert!(!subscript_violations.is_empty(), "Should detect parameter 'index' without bounds checking");
}

#[test]
fn test_arr00c_allows_validated_parameter_subscript() {
    let rule = Arr00C;
    let mut parser = CParser::new().unwrap();

    let source = r#"
void update_array(int arr[], int size, int index, int value) {
    if (index >= 0 && index < size) {
        arr[index] = value;
    }
}
"#;

    let tree = parser.parse_source(source).unwrap();
    let violations = rule.check(&tree.root_node(), source);

    // Should not flag because index is validated
    let subscript_violations: Vec<_> = violations.iter()
        .filter(|v| v.message.contains("parameter") && v.message.contains("index"))
        .collect();
    assert!(subscript_violations.is_empty(), "Should not flag validated parameter");
}

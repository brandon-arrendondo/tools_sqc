//! ERR33-C: Detect and handle standard library errors
//!
//! This rule ensures that return values from standard library functions that can indicate
//! errors are properly checked. The implementation uses AST analysis to detect:
//!
//! 1. Assignment patterns: `ptr = malloc(size)` followed by `if (ptr == NULL)`
//! 2. Direct usage patterns: `if (fopen("file", "r") != NULL)`
//! 3. Ignored return values: `malloc(size);` (standalone call)
//!
//! ## Supported Error Patterns:
//! - NULL pointer returns: malloc, calloc, fopen, fgets, etc.
//! - Non-zero error codes: fseek, fclose, etc.
//! - Negative error indicators: printf, snprintf, etc.
//! - Special cases: strtol (errno checking), etc.

use super::super::{CertRule, RuleViolation};
use crate::manifest::Severity;
use tree_sitter::Node;
use std::collections::HashMap;

pub struct Err33C;

impl CertRule for Err33C {
    fn rule_id(&self) -> &'static str {
        "ERR33-C"
    }

    fn description(&self) -> &'static str {
        "Detect and handle standard library errors"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Err33C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            "call_expression" => {
                self.check_function_call(node, source, violations);
            }
            "expression_statement" => {
                // Check if this is a standalone function call that ignores return value
                if let Some(child) = node.child(0) {
                    if child.kind() == "call_expression" {
                        self.check_ignored_return_value(node, &child, source, violations);
                    }
                }
            }
            "assignment_expression" => {
                self.check_assignment(node, source, violations);
            }
            _ => {}
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }

    fn check_function_call(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(function_node) = node.child_by_field_name("function") {
            let function_name = &source[function_node.start_byte()..function_node.end_byte()];

            if self.is_error_returning_function(function_name) {
                // Check if the return value is properly handled
                if !self.is_return_value_checked(node, source) {
                    let start_point = node.start_position();
                    let call_text = &source[node.start_byte()..node.end_byte()];

                    let error_info = self.get_error_info(function_name);

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Return value of '{}' not checked: '{}' - {}",
                            function_name, call_text, error_info.description
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some(error_info.suggestion),
                    });
                }
            }
        }
    }

    fn check_ignored_return_value(&self, stmt_node: &Node, call_node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let Some(function_node) = call_node.child_by_field_name("function") {
            let function_name = &source[function_node.start_byte()..function_node.end_byte()];

            if self.is_error_returning_function(function_name) {
                let start_point = stmt_node.start_position();
                let call_text = &source[call_node.start_byte()..call_node.end_byte()];

                let error_info = self.get_error_info(function_name);

                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::High,
                    message: format!(
                        "Return value of '{}' ignored: '{}' - {}",
                        function_name, call_text, error_info.description
                    ),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some(error_info.suggestion),
                });
            }
        }
    }

    fn check_assignment(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        if let (Some(left), Some(right)) = (node.child_by_field_name("left"), node.child_by_field_name("right")) {
            if right.kind() == "call_expression" {
                if let Some(function_node) = right.child_by_field_name("function") {
                    let function_name = &source[function_node.start_byte()..function_node.end_byte()];
                    let var_name = &source[left.start_byte()..left.end_byte()];

                    if self.is_error_returning_function(function_name) {
                        // Check if the assigned variable is later checked for errors
                        if !self.is_variable_error_checked(node, var_name, function_name, source) {
                            let start_point = node.start_position();
                            let call_text = &source[right.start_byte()..right.end_byte()];

                            let error_info = self.get_error_info(function_name);

                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                severity: Severity::High,
                                message: format!(
                                    "Return value of '{}' assigned to '{}' but not checked for errors: '{}' - {}",
                                    function_name, var_name, call_text, error_info.description
                                ),
                                file_path: String::new(),
                                line: start_point.row + 1,
                                column: start_point.column + 1,
                                suggestion: Some(error_info.suggestion),
                            });
                        }
                    }
                }
            }
        }
    }

    fn is_error_returning_function(&self, function_name: &str) -> bool {
        matches!(function_name,
            // Memory management
            "malloc" | "calloc" | "realloc" | "aligned_alloc" |

            // File I/O
            "fopen" | "freopen" | "fseek" | "ftell" | "fsetpos" | "fgetpos" |
            "fread" | "fwrite" | "fflush" | "fclose" | "remove" | "rename" |
            "tmpfile" | "tmpnam" | "fgets" | "fputs" | "fgetc" | "fputc" | "ungetc" |

            // String/locale functions
            "setlocale" | "strtol" | "strtoul" | "strtoll" | "strtoull" |
            "strtof" | "strtod" | "strtold" | "strftime" | "mbstowcs" | "wcstombs" |
            "gets" | // deprecated but still needs checking

            // Formatted I/O
            "printf" | "fprintf" | "sprintf" | "snprintf" | "scanf" | "fscanf" | "sscanf" |
            "vprintf" | "vfprintf" | "vsprintf" | "vsnprintf" |

            // Time functions
            "time" | "mktime" | "clock" |

            // System functions
            "system" | "atexit" | "signal" | "raise" |

            // Character classification that can fail
            "mblen" | "mbtowc" | "wctomb" |

            // Math functions that set errno
            "acos" | "asin" | "atan" | "atan2" | "cos" | "sin" | "tan" |
            "acosh" | "asinh" | "atanh" | "cosh" | "sinh" | "tanh" |
            "exp" | "exp2" | "expm1" | "log" | "log10" | "log1p" | "log2" |
            "pow" | "sqrt" | "cbrt" | "hypot" | "fabs" | "fmod" | "remainder" |
            "ceil" | "floor" | "trunc" | "round" | "nearbyint" | "rint" |

            // Environment
            "getenv"
        )
    }

    fn get_error_info(&self, function_name: &str) -> ErrorInfo {
        let functions_info = self.get_function_error_info();
        functions_info.get(function_name).cloned().unwrap_or_else(|| {
            ErrorInfo {
                description: "Can return error indicator".to_string(),
                suggestion: "Check return value for errors".to_string(),
            }
        })
    }

    fn get_function_error_info(&self) -> HashMap<&'static str, ErrorInfo> {
        let mut info = HashMap::new();

        // Memory management
        info.insert("malloc", ErrorInfo {
            description: "Returns NULL on allocation failure".to_string(),
            suggestion: "Check if (ptr == NULL) before using the allocated memory".to_string(),
        });
        info.insert("calloc", ErrorInfo {
            description: "Returns NULL on allocation failure".to_string(),
            suggestion: "Check if (ptr == NULL) before using the allocated memory".to_string(),
        });
        info.insert("realloc", ErrorInfo {
            description: "Returns NULL on reallocation failure".to_string(),
            suggestion: "Use temporary pointer: new_ptr = realloc(ptr, size); if (new_ptr == NULL) handle_error();".to_string(),
        });

        // File I/O
        info.insert("fopen", ErrorInfo {
            description: "Returns NULL if file cannot be opened".to_string(),
            suggestion: "Check if (file == NULL) before using the file pointer".to_string(),
        });
        info.insert("fseek", ErrorInfo {
            description: "Returns non-zero on failure".to_string(),
            suggestion: "Check if (fseek(file, offset, whence) != 0) for seek errors".to_string(),
        });
        info.insert("ftell", ErrorInfo {
            description: "Returns -1L on failure".to_string(),
            suggestion: "Check if (pos == -1L) for position errors".to_string(),
        });
        info.insert("fread", ErrorInfo {
            description: "Returns number of items read, may be less than requested".to_string(),
            suggestion: "Check if (items_read == expected_items) or handle partial reads".to_string(),
        });
        info.insert("fwrite", ErrorInfo {
            description: "Returns number of items written, may be less than requested".to_string(),
            suggestion: "Check if (items_written == expected_items) for write errors".to_string(),
        });
        info.insert("fgets", ErrorInfo {
            description: "Returns NULL on error or EOF".to_string(),
            suggestion: "Check if (fgets(buffer, size, file) != NULL) before using buffer".to_string(),
        });
        info.insert("fclose", ErrorInfo {
            description: "Returns non-zero on error".to_string(),
            suggestion: "Check if (fclose(file) != 0) for close errors".to_string(),
        });
        info.insert("fputs", ErrorInfo {
            description: "Returns EOF on error".to_string(),
            suggestion: "Check if (fputs(str, file) == EOF) for write errors".to_string(),
        });
        info.insert("fgetc", ErrorInfo {
            description: "Returns EOF on error or end of file".to_string(),
            suggestion: "Check if (c = fgetc(file)) != EOF and distinguish from actual EOF".to_string(),
        });
        info.insert("fputc", ErrorInfo {
            description: "Returns EOF on error".to_string(),
            suggestion: "Check if (fputc(c, file) == EOF) for write errors".to_string(),
        });

        // String/locale functions
        info.insert("setlocale", ErrorInfo {
            description: "Returns NULL if locale cannot be set".to_string(),
            suggestion: "Check if (setlocale(category, locale) == NULL) for locale errors".to_string(),
        });
        info.insert("strtol", ErrorInfo {
            description: "Sets errno on overflow/underflow, uses endptr for parsing errors".to_string(),
            suggestion: "Check errno and endptr: errno = 0; val = strtol(str, &endptr, base); if (errno != 0 || endptr == str) handle_error();".to_string(),
        });

        // Formatted I/O
        info.insert("printf", ErrorInfo {
            description: "Returns negative value on output error".to_string(),
            suggestion: "Check if (printf(...) < 0) for output errors".to_string(),
        });
        info.insert("snprintf", ErrorInfo {
            description: "Returns negative on error, or >= buffer size on truncation".to_string(),
            suggestion: "Check result: int ret = snprintf(buf, size, fmt, ...); if (ret < 0 || ret >= size) handle_error();".to_string(),
        });

        // System functions
        info.insert("system", ErrorInfo {
            description: "Returns -1 on failure to execute command".to_string(),
            suggestion: "Check if (system(command) == -1) for execution errors".to_string(),
        });

        info
    }

    fn is_return_value_checked(&self, node: &Node, source: &str) -> bool {
        // Check if this function call is part of a condition or assignment
        if let Some(parent) = node.parent() {
            match parent.kind() {
                // Direct assignment
                "assignment_expression" => {
                    if let Some(left) = parent.child_by_field_name("left") {
                        let var_name = &source[left.start_byte()..left.end_byte()];
                        // Check if the variable is later checked
                        return self.is_variable_checked_in_context(&parent, var_name, source);
                    }
                }
                // Used in a condition
                "if_statement" | "while_statement" | "for_statement" | "conditional_expression" |
                "binary_expression" | "unary_expression" | "parenthesized_expression" => {
                    return true;
                }
                // Used in return statement
                "return_statement" => {
                    return true;
                }
                _ => {}
            }
        }
        false
    }

    /// Checks if a variable assigned from an error-returning function is properly checked for errors.
    ///
    /// This function searches forward in the AST from the assignment point to find error checking
    /// patterns in subsequent statements. It looks for:
    /// - NULL pointer checks for pointer-returning functions (malloc, fopen, fgets, etc.)
    /// - Non-zero return value checks for status-returning functions (fclose, fseek, etc.)
    /// - Negative value checks for size/count-returning functions (printf, snprintf, etc.)
    ///
    /// The search is limited to the immediate scope and next 5 statements to avoid false positives
    /// from distant, unrelated checks.
    fn is_variable_error_checked(&self, assignment_node: &Node, var_name: &str, function_name: &str, source: &str) -> bool {
        println!("DEBUG: is_variable_error_checked called for var '{}' function '{}'", var_name, function_name);
        // Use new forward-looking algorithm
        let result = self.find_error_checks_in_scope(assignment_node, var_name, function_name, source);
        println!("DEBUG: is_variable_error_checked result: {}", result);
        result
    }

    /// Find error checks by looking forward from the assignment statement in the AST
    fn find_error_checks_in_scope(&self, assignment_node: &Node, var_name: &str, function_name: &str, source: &str) -> bool {
        println!("DEBUG: find_error_checks_in_scope called for var {} function {}", var_name, function_name);

        // Walk up the AST to find the function body
        let mut current = assignment_node.parent();
        while let Some(node) = current {
            println!("DEBUG: Looking at parent node kind: {}", node.kind());
            if node.kind() == "compound_statement" {
                println!("DEBUG: Found compound statement, searching forward");
                // Found the function body, now search forward from the assignment position
                return self.search_statements_for_error_checks(&node, assignment_node, var_name, function_name, source);
            }
            current = node.parent();
        }
        println!("DEBUG: No compound statement found");
        false
    }


    /// Search through statements in a compound statement for error checking patterns
    fn search_statements_for_error_checks(&self, compound_stmt: &Node, assignment_node: &Node, var_name: &str, function_name: &str, source: &str) -> bool {
        let assignment_byte_start = assignment_node.start_byte();
        let mut statements_checked = 0;
        const MAX_FORWARD_SEARCH: usize = 5; // Limit search to next 5 statements

        // Walk through all child statements in the compound statement
        for i in 0..compound_stmt.child_count() {
            if let Some(child) = compound_stmt.child(i) {
                // Skip non-statement nodes (like braces)
                if !self.is_statement_node(&child) {
                    continue;
                }

                // Only look at statements that come after the assignment
                if child.start_byte() > assignment_byte_start {
                    let child_text = &source[child.start_byte()..child.end_byte()];
                    println!("DEBUG: Checking statement: {} (kind: {})", child_text.trim(), child.kind());

                    if self.statement_contains_error_check(&child, var_name, function_name, source) {
                        println!("DEBUG: Found error check for variable {}", var_name);
                        return true;
                    }
                    statements_checked += 1;
                    if statements_checked >= MAX_FORWARD_SEARCH {
                        break; // Limit search scope to avoid false positives
                    }
                }
            }
        }
        println!("DEBUG: No error check found for variable {}", var_name);
        false
    }

    /// Check if a node represents a statement
    fn is_statement_node(&self, node: &Node) -> bool {
        matches!(node.kind(),
            "expression_statement" | "if_statement" | "while_statement" |
            "for_statement" | "return_statement" | "break_statement" |
            "continue_statement" | "compound_statement" | "declaration" |
            "init_declarator"
        )
    }


    /// Check if a single statement contains error checking for the variable
    fn statement_contains_error_check(&self, stmt_node: &Node, var_name: &str, function_name: &str, source: &str) -> bool {
        // For if statements, check the condition
        if stmt_node.kind() == "if_statement" {
            if let Some(condition) = stmt_node.child_by_field_name("condition") {
                return self.find_error_check_in_context(&condition, var_name, function_name, source);
            }
        }

        // For other statements, check the entire statement
        self.find_error_check_in_context(stmt_node, var_name, function_name, source)
    }


    fn is_variable_checked_in_context(&self, node: &Node, var_name: &str, source: &str) -> bool {
        // Look in parent scopes for error checking
        let mut current = node.parent();
        for _ in 0..3 { // Check up to 3 levels up
            if let Some(parent) = current {
                if self.contains_error_check(&parent, var_name, source) {
                    return true;
                }
                current = parent.parent();
            } else {
                break;
            }
        }
        false
    }

    fn find_error_check_in_context(&self, node: &Node, var_name: &str, function_name: &str, source: &str) -> bool {
        let text = &source[node.start_byte()..node.end_byte()];

        // Check for NULL pointer checks (more comprehensive patterns)
        if matches!(function_name, "malloc" | "calloc" | "realloc" | "fopen" | "fgets" | "tmpfile") {
            // Direct comparisons
            if text.contains(&format!("{} == NULL", var_name)) ||
               text.contains(&format!("NULL == {}", var_name)) ||
               text.contains(&format!("{} != NULL", var_name)) ||
               text.contains(&format!("NULL != {}", var_name)) {
                return true;
            }

            // Implicit boolean checks
            if text.contains(&format!("if ({})", var_name)) ||
               text.contains(&format!("if ({} )", var_name)) ||
               text.contains(&format!("if({})", var_name)) ||
               text.contains(&format!("!{}", var_name)) ||
               text.contains(&format!("if (!{})", var_name)) {
                return true;
            }

            // Assignment with check in same expression
            if text.contains(&format!("({} = ", var_name)) &&
               (text.contains("!= NULL") || text.contains("== NULL")) {
                return true;
            }
        }

        // For printf/fprintf - skip if in error handling context
        if matches!(function_name, "printf" | "fprintf" | "sprintf" | "snprintf") {
            if self.is_in_error_handling_context(node, source) {
                return true; // Accept printf/fprintf in error contexts
            }

            // Otherwise check for explicit return value checking
            if text.contains(&format!("{} < 0", var_name)) ||
               text.contains(&format!("0 > {}", var_name)) ||
               text.contains(&format!("{} >= sizeof", var_name)) {
                return true;
            }
        }

        // For fclose/fseek - check for non-zero return
        if matches!(function_name, "fclose" | "fseek" | "fflush") {
            if text.contains(&format!("{} != 0", var_name)) ||
               text.contains(&format!("0 != {}", var_name)) ||
               text.contains(&format!("{} == 0", var_name)) ||
               text.contains(&format!("0 == {}", var_name)) {
                return true;
            }
        }

        // For ftell - check for -1L return
        if function_name == "ftell" {
            if text.contains(&format!("{} == -1", var_name)) ||
               text.contains(&format!("-1 == {}", var_name)) ||
               text.contains(&format!("{} == -1L", var_name)) ||
               text.contains(&format!("-1L == {}", var_name)) {
                return true;
            }
        }

        // For fread/fwrite - check if result equals expected
        if matches!(function_name, "fread" | "fwrite") {
            if text.contains(&format!("{} ==", var_name)) ||
               text.contains(&format!("{} !=", var_name)) ||
               text.contains(&format!("{} <", var_name)) ||
               text.contains(&format!("{} >", var_name)) {
                return true;
            }
        }

        // For strtol family - check errno and endptr
        if matches!(function_name, "strtol" | "strtoul" | "strtoll" | "strtoull" | "strtod" | "strtof" | "strtold") {
            if text.contains("errno") || text.contains("endptr") {
                return true;
            }
        }

        // For setlocale - check for NULL return
        if function_name == "setlocale" {
            if text.contains(&format!("{} == NULL", var_name)) ||
               text.contains(&format!("NULL == {}", var_name)) ||
               text.contains(&format!("{} != NULL", var_name)) ||
               text.contains(&format!("NULL != {}", var_name)) {
                return true;
            }
        }

        // For system - check for -1 return
        if function_name == "system" {
            if text.contains(&format!("{} == -1", var_name)) ||
               text.contains(&format!("-1 == {}", var_name)) ||
               text.contains(&format!("{} != -1", var_name)) ||
               text.contains(&format!("-1 != {}", var_name)) {
                return true;
            }
        }

        false
    }

    fn contains_error_check(&self, node: &Node, var_name: &str, source: &str) -> bool {
        let text = &source[node.start_byte()..node.end_byte()];

        // Look for common error checking patterns
        text.contains(&format!("{} == NULL", var_name)) ||
        text.contains(&format!("NULL == {}", var_name)) ||
        text.contains(&format!("{} != NULL", var_name)) ||
        text.contains(&format!("NULL != {}", var_name)) ||
        text.contains(&format!("!{}", var_name)) ||
        text.contains(&format!("if ({})", var_name)) ||
        text.contains(&format!("if({}", var_name)) ||
        text.contains(&format!("{} < 0", var_name)) ||
        text.contains(&format!("0 > {}", var_name)) ||
        text.contains(&format!("{} != 0", var_name)) ||
        text.contains(&format!("0 != {}", var_name)) ||
        text.contains(&format!("{} == -1", var_name)) ||
        text.contains(&format!("-1 == {}", var_name)) ||
        text.contains(&format!("{} >= sizeof", var_name))
    }

    /// Check if a node appears to be in an error handling context
    fn is_in_error_handling_context(&self, node: &Node, source: &str) -> bool {
        let mut current = node.parent();

        // Look up the AST to find error handling indicators
        for level in 0..5 {
            if let Some(parent) = current {
                // Check if we're in an if statement that tests for errors
                if parent.kind() == "if_statement" {
                    if let Some(condition) = parent.child_by_field_name("condition") {
                        let condition_text = &source[condition.start_byte()..condition.end_byte()];
                        // Look for error checking patterns in the condition
                        if condition_text.contains("== NULL") || condition_text.contains("!= NULL") ||
                           condition_text.contains("< 0") || condition_text.contains("!= 0") ||
                           condition_text.contains("== -1") || condition_text.contains("== EOF") {
                            return true;
                        }
                    }
                }

                // Check if we're in the else clause of an error check
                if parent.kind() == "else_clause" {
                    if let Some(if_stmt) = parent.parent() {
                        if if_stmt.kind() == "if_statement" {
                            if let Some(condition) = if_stmt.child_by_field_name("condition") {
                                let condition_text = &source[condition.start_byte()..condition.end_byte()];
                                if condition_text.contains("!= NULL") || condition_text.contains(">= 0") {
                                    return true; // This is likely an error handling else clause
                                }
                            }
                        }
                    }
                }

                // Look for explicit error handling keywords in parent context
                let parent_text = &source[parent.start_byte()..parent.end_byte()];
                if level <= 2 { // Only check close parents for keywords
                    if parent_text.contains("stderr") || parent_text.contains("perror") ||
                       parent_text.contains("return -1") || parent_text.contains("exit(") ||
                       parent_text.contains("goto error") || parent_text.contains("cleanup") {
                        return true;
                    }
                }

                current = parent.parent();
            } else {
                break;
            }
        }
        false
    }
}

#[derive(Debug, Clone)]
struct ErrorInfo {
    description: String,
    suggestion: String,
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::parser::CParser;

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
}
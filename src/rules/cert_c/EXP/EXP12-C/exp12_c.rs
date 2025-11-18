use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Exp12C;

impl CertRule for Exp12C {
    fn rule_id(&self) -> &'static str {
        "EXP12-C"
    }

    fn description(&self) -> &'static str {
        "Do not ignore values returned by functions"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "EXP12-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Check for expression statements that contain function calls
        // Expression statements are where return values might be ignored
        if node.kind() == "expression_statement" {
            // Check if this is an explicit void cast (intentional dismissal)
            if is_explicit_void_cast(node, source) {
                // This is compliant - explicit (void) cast indicates intentional dismissal
            } else {
                // Look for function calls in this expression statement
                check_for_ignored_return_values(node, source, &mut violations);
            }
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                violations.extend(self.check(&child, source));
            }
        }

        violations
    }
}

/// Check if this expression statement contains an explicit (void) cast
fn is_explicit_void_cast(node: &Node, source: &str) -> bool {
    // Look for cast_expression child
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if child.kind() == "cast_expression" {
                // Check if the type is void
                if let Some(type_node) = child.child_by_field_name("type") {
                    let type_text = get_node_text(&type_node, source).trim();
                    if type_text == "void" {
                        return true;
                    }
                }
            }
        }
    }
    false
}

/// Check for function calls whose return values are being ignored
fn check_for_ignored_return_values(node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
    // Look for call_expression nodes
    if node.kind() == "call_expression" {
        if let Some(function_node) = node.child_by_field_name("function") {
            let function_name = get_node_text(&function_node, source);

            // Check if this is a function that returns an important value
            if is_function_with_important_return_value(function_name.trim()) {
                report_violation(node, function_name.trim(), source, violations);
            }
        }
    }

    // Recursively check children
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            check_for_ignored_return_values(&child, source, violations);
        }
    }
}

/// Check if a function is known to return important values that should be checked
fn is_function_with_important_return_value(function_name: &str) -> bool {
    // List of functions from standard library and POSIX that return important values
    // This includes functions that can fail and signal errors via return values
    matches!(
        function_name,
        // Memory allocation functions
        "malloc" | "calloc" | "realloc" | "aligned_alloc" |
        // String functions that can fail
        "asprintf" | "vasprintf" |
        // File I/O functions
        "fopen" | "fclose" | "fread" | "fwrite" | "fseek" | "ftell" |
        "fflush" | "fgetc" | "fgets" | "fputc" | "fputs" | "fprintf" |
        "fscanf" | "feof" | "ferror" | "freopen" | "remove" | "rename" |
        "tmpfile" | "setvbuf" | "ungetc" |
        // Standard I/O
        "scanf" | "sscanf" | "printf" | "sprintf" | "snprintf" |
        "vprintf" | "vsprintf" | "vsnprintf" | "puts" |
        "getc" | "getchar" | "gets" | "putc" | "putchar" |
        // Process/system functions
        "system" | "atexit" | "signal" | "raise" |
        // POSIX functions
        "open" | "close" | "read" | "write" | "lseek" | "access" |
        "chdir" | "chmod" | "chown" | "dup" | "dup2" | "pipe" |
        "fork" | "exec" | "execl" | "execle" | "execlp" | "execv" |
        "execve" | "execvp" | "wait" | "waitpid" |
        "pthread_create" | "pthread_join" | "pthread_mutex_init" |
        "pthread_mutex_lock" | "pthread_mutex_unlock" |
        // String/memory functions with return values
        "strcpy" | "strncpy" | "strcat" | "strncat" | "memcpy" |
        "memmove" | "memset" | "memcmp" | "strcmp" | "strncmp" |
        "strchr" | "strrchr" | "strstr" | "strtok" | "strlen" |
        // Time functions
        "time" | "clock" | "strftime" |
        // Math functions (though these typically can't fail in the same way)
        // Network functions (if applicable)
        "socket" | "bind" | "listen" | "accept" | "connect" |
        "send" | "recv" | "sendto" | "recvfrom" |
        // Other common functions
        "setlocale" | "getenv" | "mktime" | "difftime"
    )
}

/// Report a violation for ignoring a function's return value
fn report_violation(
    node: &Node,
    function_name: &str,
    source: &str,
    violations: &mut Vec<RuleViolation>,
) {
    let start_point = node.start_position();
    let node_text = get_node_text(node, source);

    violations.push(RuleViolation {
        rule_id: "EXP12-C".to_string(),
        severity: Severity::Medium,
        message: format!(
            "Return value of function '{}' should not be ignored: '{}'",
            function_name, node_text
        ),
        file_path: String::new(),
        line: start_point.row + 1,
        column: start_point.column + 1,
        suggestion: Some(format!(
            "Check the return value or explicitly cast to (void) if intentionally ignoring: '(void) {};'",
            node_text
        )),
        ..Default::default()
    });
}

//! DCL09-C: Declare functions that return errno with a return type of errno_t
//!
//! This rule detects functions that return errno values but are declared with type int
//! instead of errno_t. This creates ambiguity about what the function returns.
//!
//! VIOLATIONS:
//! - int func() { return EINVAL; }     // Should be errno_t
//! - int func() { return errno; }      // Should be errno_t
//! - int func() { return EIO; }        // Should be errno_t
//!
//! COMPLIANT:
//! - errno_t func() { return EINVAL; } // Correct type
//! - int func() { return 42; }         // Not returning errno

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils;
use tree_sitter::Node;

pub struct Dcl09C;

impl CertRule for Dcl09C {
    fn rule_id(&self) -> &'static str {
        "DCL09-C"
    }

    fn description(&self) -> &'static str {
        "Declare functions that return errno with a return type of errno_t"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "DCL09-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Check function definitions
        if node.kind() == "function_definition" {
            if let Some(violation) = self.check_function(node, source) {
                violations.push(violation);
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

impl Dcl09C {
    /// Check if a function definition violates DCL09-C
    fn check_function(&self, func_node: &Node, source: &str) -> Option<RuleViolation> {
        // Get function return type
        let return_type = self.get_return_type(func_node, source)?;

        // If already using errno_t, no violation
        if return_type.contains("errno_t") {
            return None;
        }

        // Check if function returns errno values but uses int/other type
        if return_type.contains("int") || return_type == "int" {
            if self.returns_errno_values(func_node, source) {
                let func_name = self.get_function_name(func_node, source)?;
                let start_point = func_node.start_position();

                return Some(RuleViolation {
                    rule_id: "DCL09-C".to_string(),
                    severity: Severity::Low,
                    message: format!(
                        "Function '{}' returns errno values but is declared with return type '{}' instead of 'errno_t'",
                        func_name, return_type
                    ),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some(format!(
                        "Change return type to 'errno_t' to clearly indicate this function returns error codes"
                    )),
                    ..Default::default()
                });
            }
        }

        None
    }

    /// Get the return type of a function
    fn get_return_type(&self, func_node: &Node, source: &str) -> Option<String> {
        // Find the type node
        if let Some(type_node) = func_node.child_by_field_name("type") {
            let type_text = ast_utils::get_node_text(&type_node, source)
                .trim()
                .to_string();
            return Some(type_text);
        }

        None
    }

    /// Get the function name
    fn get_function_name(&self, func_node: &Node, source: &str) -> Option<String> {
        if let Some(declarator) = func_node.child_by_field_name("declarator") {
            return self.extract_function_name(&declarator, source);
        }
        None
    }

    /// Extract function name from declarator
    fn extract_function_name(&self, declarator: &Node, source: &str) -> Option<String> {
        match declarator.kind() {
            "function_declarator" => {
                if let Some(child_declarator) = declarator.child_by_field_name("declarator") {
                    return self.extract_function_name(&child_declarator, source);
                }
            }
            "pointer_declarator" => {
                if let Some(child_declarator) = declarator.child_by_field_name("declarator") {
                    return self.extract_function_name(&child_declarator, source);
                }
            }
            "identifier" => {
                return Some(ast_utils::get_node_text(declarator, source).to_string());
            }
            _ => {}
        }

        None
    }

    /// Check if function body returns errno values
    fn returns_errno_values(&self, func_node: &Node, source: &str) -> bool {
        if let Some(body) = func_node.child_by_field_name("body") {
            return self.check_for_errno_returns(&body, source);
        }
        false
    }

    /// Recursively check for return statements that return errno values
    fn check_for_errno_returns(&self, node: &Node, source: &str) -> bool {
        // Check if this is a return statement
        if node.kind() == "return_statement" {
            // Get the return expression
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() != "return" && child.kind() != ";" {
                        // This is the return expression
                        let return_text = ast_utils::get_node_text(&child, source);
                        if self.is_errno_value(&return_text) {
                            return true;
                        }
                    }
                }
            }
        }

        // Recursively check children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if self.check_for_errno_returns(&child, source) {
                    return true;
                }
            }
        }

        false
    }

    /// Check if a value is an errno constant or errno itself
    fn is_errno_value(&self, text: &str) -> bool {
        let text = text.trim();

        // Check for errno variable
        if text == "errno" {
            return true;
        }

        // Common errno constants (from errno.h)
        let errno_constants = [
            "EPERM",
            "ENOENT",
            "ESRCH",
            "EINTR",
            "EIO",
            "ENXIO",
            "E2BIG",
            "ENOEXEC",
            "EBADF",
            "ECHILD",
            "EAGAIN",
            "ENOMEM",
            "EACCES",
            "EFAULT",
            "ENOTBLK",
            "EBUSY",
            "EEXIST",
            "EXDEV",
            "ENODEV",
            "ENOTDIR",
            "EISDIR",
            "EINVAL",
            "ENFILE",
            "EMFILE",
            "ENOTTY",
            "ETXTBSY",
            "EFBIG",
            "ENOSPC",
            "ESPIPE",
            "EROFS",
            "EMLINK",
            "EPIPE",
            "EDOM",
            "ERANGE",
            "EDEADLK",
            "ENAMETOOLONG",
            "ENOLCK",
            "ENOSYS",
            "ENOTEMPTY",
            "ELOOP",
            "EWOULDBLOCK",
            "ENOMSG",
            "EIDRM",
            "ECHRNG",
            "EL2NSYNC",
            "EL3HLT",
            "EL3RST",
            "ELNRNG",
            "EUNATCH",
            "ENOCSI",
            "EL2HLT",
            "EBADE",
            "EBADR",
            "EXFULL",
            "ENOANO",
            "EBADRQC",
            "EBADSLT",
            "EDEADLOCK",
            "EBFONT",
            "ENOSTR",
            "ENODATA",
            "ETIME",
            "ENOSR",
            "ENONET",
            "ENOPKG",
            "EREMOTE",
            "ENOLINK",
            "EADV",
            "ESRMNT",
            "ECOMM",
            "EPROTO",
            "EMULTIHOP",
            "EDOTDOT",
            "EBADMSG",
            "EOVERFLOW",
            "ENOTUNIQ",
            "EBADFD",
            "EREMCHG",
            "ELIBACC",
            "ELIBBAD",
            "ELIBSCN",
            "ELIBMAX",
            "ELIBEXEC",
            "EILSEQ",
            "ERESTART",
            "ESTRPIPE",
            "EUSERS",
            "ENOTSOCK",
            "EDESTADDRREQ",
            "EMSGSIZE",
            "EPROTOTYPE",
            "ENOPROTOOPT",
            "EPROTONOSUPPORT",
            "ESOCKTNOSUPPORT",
            "EOPNOTSUPP",
            "ENOTSUP",
            "EPFNOSUPPORT",
            "EAFNOSUPPORT",
            "EADDRINUSE",
            "EADDRNOTAVAIL",
            "ENETDOWN",
            "ENETUNREACH",
            "ENETRESET",
            "ECONNABORTED",
            "ECONNRESET",
            "ENOBUFS",
            "EISCONN",
            "ENOTCONN",
            "ESHUTDOWN",
            "ETOOMANYREFS",
            "ETIMEDOUT",
            "ECONNREFUSED",
            "EHOSTDOWN",
            "EHOSTUNREACH",
            "EALREADY",
            "EINPROGRESS",
            "ESTALE",
            "EUCLEAN",
            "ENOTNAM",
            "ENAVAIL",
            "EISNAM",
            "EREMOTEIO",
            "EDQUOT",
            "ENOMEDIUM",
            "EMEDIUMTYPE",
            "ECANCELED",
            "ENOKEY",
            "EKEYEXPIRED",
            "EKEYREVOKED",
            "EKEYREJECTED",
            "EOWNERDEAD",
            "ENOTRECOVERABLE",
            "ERFKILL",
            "EHWPOISON",
        ];

        errno_constants.iter().any(|&constant| text == constant)
    }
}

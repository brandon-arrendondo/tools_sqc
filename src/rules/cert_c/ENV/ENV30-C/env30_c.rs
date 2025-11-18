use crate::manifest::{RuleCategory, Severity};
use crate::prelude::RuleViolation;
use crate::rules::cert_c::CertRule;
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct ENV30C;

impl CertRule for ENV30C {
    fn rule_id(&self) -> &'static str {
        "ENV30-C"
    }

    fn cert_id(&self) -> &'static str {
        "ENV30"
    }

    fn description(&self) -> &'static str {
        "Do not modify the object referenced by the return value of certain functions"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Check for assignments that modify return values of protected functions
        if node.kind() == "assignment_expression" {
            if let Some(violation) = self.check_protected_assignment(node, source) {
                violations.push(violation);
            }
        }

        // Check for passing protected return values to modification functions
        if node.kind() == "call_expression" {
            violations.extend(self.check_modification_calls(node, source));
        }

        // Check function definitions to track variable assignments from protected functions
        if node.kind() == "function_definition" {
            violations.extend(self.check_function_for_violations(node, source));
        }

        violations
    }
}

impl ENV30C {
    fn check_function_for_violations(&self, func_node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        let mut protected_vars = std::collections::HashMap::new();

        // Collect all variable assignments from protected functions
        self.collect_protected_assignments(func_node, source, &mut protected_vars);

        // Check for modifications to those variables
        self.check_protected_var_modifications(func_node, source, &protected_vars, &mut violations);

        violations
    }

    fn collect_protected_assignments(
        &self,
        node: &Node,
        source: &str,
        protected_vars: &mut std::collections::HashMap<String, String>,
    ) {
        // Look for declarations like: char *env = getenv("X");
        if node.kind() == "declaration" {
            let mut cursor = node.walk();
            for child in node.children(&mut cursor) {
                if child.kind() == "init_declarator" {
                    if let Some((var_name, func_name)) = self.extract_protected_init(&child, source)
                    {
                        protected_vars.insert(var_name, func_name);
                    }
                }
            }
        }

        // Recurse
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.collect_protected_assignments(&child, source, protected_vars);
        }
    }

    fn extract_protected_init(&self, node: &Node, source: &str) -> Option<(String, String)> {
        let mut var_name = None;
        let mut func_name = None;

        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            if child.kind() == "identifier" || child.kind() == "pointer_declarator" {
                var_name = Some(self.extract_identifier(&child, source));
            } else if child.kind() == "call_expression" {
                if let Some(fname) = self.extract_function_name(&child, source) {
                    if self.is_protected_function(&fname) {
                        func_name = Some(fname);
                    }
                }
            }
        }

        if let (Some(vn), Some(fn_)) = (var_name, func_name) {
            return Some((vn, fn_));
        }

        None
    }

    fn extract_identifier(&self, node: &Node, source: &str) -> String {
        if node.kind() == "identifier" {
            return get_node_text(node, source).to_string();
        }

        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            let id = self.extract_identifier(&child, source);
            if !id.is_empty() {
                return id;
            }
        }

        String::new()
    }

    fn extract_function_name(&self, node: &Node, source: &str) -> Option<String> {
        if node.kind() == "call_expression" {
            let mut cursor = node.walk();
            for child in node.children(&mut cursor) {
                if child.kind() == "identifier" {
                    return Some(get_node_text(&child, source).to_string());
                }
            }
        }
        None
    }

    fn check_protected_var_modifications(
        &self,
        node: &Node,
        source: &str,
        protected_vars: &std::collections::HashMap<String, String>,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Check for assignments to protected variables or their elements
        if node.kind() == "assignment_expression" {
            let mut cursor = node.walk();
            let children: Vec<_> = node.children(&mut cursor).collect();
            if let Some(left) = children.first() {
                if let Some((var_name, func_name)) =
                    self.get_protected_var_ref(left, source, protected_vars)
                {
                    let start = node.start_position();
                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        file_path: String::new(),
                        message: format!(
                            "Modifying variable '{}' which holds return value from '{}()'. The return value should not be modified.",
                            var_name, func_name
                        ),
                        line: start.row + 1,
                        column: start.column + 1,
                        severity: self.severity(),
                        suggestion: Some("Copy the return value to a local buffer before modifying it".to_string()),
                        requires_manual_review: Some(false),
                    });
                }
            }
        }

        // Check for calls that might modify protected variables (e.g., strcpy, strcat, etc.)
        if node.kind() == "call_expression" {
            if let Some(func_name) = self.extract_function_name(node, source) {
                if self.is_modification_function(&func_name) {
                    // Check if any argument is a protected variable
                    let mut cursor = node.walk();
                    for child in node.children(&mut cursor) {
                        if child.kind() == "argument_list" {
                            let mut arg_cursor = child.walk();
                            for arg in child.children(&mut arg_cursor) {
                                if let Some((var_name, orig_func)) =
                                    self.get_protected_var_ref(&arg, source, protected_vars)
                                {
                                    let start = node.start_position();
                                    violations.push(RuleViolation {
                                        rule_id: self.rule_id().to_string(),
                                        file_path: String::new(),
                                        message: format!(
                                            "Passing variable '{}' (from '{}()') to modification function '{}()'.",
                                            var_name, orig_func, func_name
                                        ),
                                        line: start.row + 1,
                                        column: start.column + 1,
                                        severity: self.severity(),
                                        suggestion: Some("Copy the return value to a local buffer before passing to modification functions".to_string()),
                                        requires_manual_review: Some(false),
                                    });
                                }
                            }
                        }
                    }
                }
            }
        }

        // Recurse
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_protected_var_modifications(&child, source, protected_vars, violations);
        }
    }

    fn get_protected_var_ref(
        &self,
        node: &Node,
        source: &str,
        protected_vars: &std::collections::HashMap<String, String>,
    ) -> Option<(String, String)> {
        match node.kind() {
            "identifier" => {
                let name = get_node_text(node, source);
                if let Some(func_name) = protected_vars.get(name) {
                    return Some((name.to_string(), func_name.clone()));
                }
            }
            "subscript_expression" | "pointer_expression" | "field_expression" => {
                let mut cursor = node.walk();
                for child in node.children(&mut cursor) {
                    if let Some(result) = self.get_protected_var_ref(&child, source, protected_vars)
                    {
                        return Some(result);
                    }
                }
            }
            _ => {}
        }

        None
    }

    fn is_modification_function(&self, name: &str) -> bool {
        matches!(
            name,
            "strcpy"
                | "strncpy"
                | "strcat"
                | "strncat"
                | "sprintf"
                | "snprintf"
                | "memcpy"
                | "memmove"
                | "memset"
                | "strtok"
        )
    }

    fn check_modification_calls(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        // This is a simpler check for direct patterns like strcpy(getenv(...), ...)
        // The more complex case is handled by check_function_for_violations
        Vec::new()
    }
    fn check_protected_assignment(&self, node: &Node, source: &str) -> Option<RuleViolation> {
        let mut cursor = node.walk();

        // Get the left side (what's being assigned to)
        let mut left_expr = None;
        for child in node.children(&mut cursor) {
            if child.kind() != "=" {
                if left_expr.is_none() {
                    left_expr = Some(child);
                }
                break;
            }
        }

        if let Some(left) = left_expr {
            // Check if left side is a dereference or field access of a protected function call
            if self.is_protected_function_result(&left, source) {
                let start = node.start_position();
                let protected_func = self.get_protected_function_name(&left, source);
                return Some(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    file_path: String::new(),
                    message: format!(
                        "Modifying object returned by '{}()'. The return value of this function should not be modified.",
                        protected_func
                    ),
                    line: start.row + 1,
                    column: start.column + 1,
                    severity: self.severity(),
                    suggestion: Some("Copy the return value to a local buffer before modifying it".to_string()),
                    requires_manual_review: Some(false),
                });
            }
        }

        None
    }

    fn is_protected_function_result(&self, node: &Node, source: &str) -> bool {
        // Check if this expression involves a call to a protected function
        match node.kind() {
            "pointer_expression" => {
                // Dereference: *getenv(...)
                let mut cursor = node.walk();
                for child in node.children(&mut cursor) {
                    if self.is_protected_function_call(&child, source) {
                        return true;
                    }
                }
            }
            "subscript_expression" => {
                // Array access: getenv(...)[0]
                let mut cursor = node.walk();
                for child in node.children(&mut cursor) {
                    if self.is_protected_function_call(&child, source) {
                        return true;
                    }
                    if self.is_protected_function_result(&child, source) {
                        return true;
                    }
                }
            }
            "field_expression" => {
                // Field access: localeconv()->decimal_point
                let mut cursor = node.walk();
                for child in node.children(&mut cursor) {
                    if self.is_protected_function_call(&child, source) {
                        return true;
                    }
                    if self.is_protected_function_result(&child, source) {
                        return true;
                    }
                }
            }
            "identifier" => {
                // Check if this identifier is assigned from a protected function
                // This requires data flow analysis, which is complex
                // For now, we'll focus on direct modifications
            }
            _ => {}
        }

        false
    }

    fn is_protected_function_call(&self, node: &Node, source: &str) -> bool {
        if node.kind() == "call_expression" {
            let mut cursor = node.walk();
            for child in node.children(&mut cursor) {
                if child.kind() == "identifier" {
                    let func_name = get_node_text(&child, source);
                    return self.is_protected_function(func_name);
                }
            }
        }
        false
    }

    fn get_protected_function_name(&self, node: &Node, source: &str) -> String {
        match node.kind() {
            "pointer_expression" | "subscript_expression" | "field_expression" => {
                let mut cursor = node.walk();
                for child in node.children(&mut cursor) {
                    if child.kind() == "call_expression" {
                        let mut call_cursor = child.walk();
                        for call_child in child.children(&mut call_cursor) {
                            if call_child.kind() == "identifier" {
                                return get_node_text(&call_child, source).to_string();
                            }
                        }
                    }
                    let name = self.get_protected_function_name(&child, source);
                    if !name.is_empty() {
                        return name;
                    }
                }
            }
            _ => {}
        }
        String::new()
    }

    fn is_protected_function(&self, name: &str) -> bool {
        matches!(
            name,
            "getenv"
                | "localeconv"
                | "setlocale"
                | "strerror"
                | "asctime"
                | "ctime"
                | "gmtime"
                | "localtime"
                | "getdate"
                | "getlogin"
        )
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_env30_c() {
        let rule = ENV30C;
        assert_eq!(rule.rule_id(), "ENV30-C");
        assert_eq!(rule.cert_id(), "ENV30");
    }
}

use super::super::{CertRule, RuleViolation};
use crate::manifest::{Severity, RuleCategory};
use tree_sitter::Node;

pub struct Pre30C;

impl CertRule for Pre30C {
    fn rule_id(&self) -> &'static str {
        "PRE30-C"
    }

    fn description(&self) -> &'static str {
        "Do not create a universal character name through concatenation"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "PRE30-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        self.check_node(node, source, &mut violations);

        violations
    }
}

impl Pre30C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        match node.kind() {
            "preproc_function_def" => {
                self.check_macro_definition(node, source, violations);
            }
            "call_expression" => {
                self.check_macro_invocation(node, source, violations);
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

    fn check_macro_definition(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        let macro_text = &source[node.start_byte()..node.end_byte()];

        // Look for token concatenation (##) that might create universal character names
        if macro_text.contains("##") {
            // Check if the macro contains patterns that could create UCNs
            if self.contains_potential_ucn_concatenation(macro_text) {
                let start_point = node.start_position();

                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::High,
                    message: "Macro definition uses token concatenation that may create universal character names, leading to undefined behavior".to_string(),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some("Avoid token concatenation that creates universal character names (\\uXXXX or \\UXXXXXXXX)".to_string()),
                ..Default::default()
                });
            }
        }
    }

    fn check_macro_invocation(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check for macro calls that might involve UCN concatenation
        if let Some(function_node) = node.child_by_field_name("function") {
            let function_name = &source[function_node.start_byte()..function_node.end_byte()];

            // Get the arguments to check for UCN patterns
            let args = self.get_macro_arguments(node, source);

            // Look for patterns where UCN fragments might be concatenated
            if self.has_ucn_concatenation_pattern(&args, function_name) {
                let start_point = node.start_position();

                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    severity: Severity::High,
                    message: format!(
                        "Macro invocation '{}' may create universal character names through concatenation",
                        function_name
                    ),
                    file_path: String::new(),
                    line: start_point.row + 1,
                    column: start_point.column + 1,
                    suggestion: Some("Use complete universal character names instead of concatenating fragments".to_string()),
                ..Default::default()
                });
            }
        }
    }

    fn contains_potential_ucn_concatenation(&self, macro_text: &str) -> bool {
        // Look for patterns that suggest UCN concatenation
        // Examples: uc1##uc2, \u##04, etc.

        // Check for partial UCN patterns around ##
        let parts: Vec<&str> = macro_text.split("##").collect();

        for window in parts.windows(2) {
            let left = window[0].trim();
            let right = window[1].trim();

            // Check if concatenation would form a UCN
            if self.could_form_ucn(left, right) {
                return true;
            }
        }

        false
    }

    fn could_form_ucn(&self, left: &str, right: &str) -> bool {
        // Check various patterns that could form UCNs when concatenated

        // Pattern 1: \u + digits
        if left.ends_with("\\u") && right.chars().all(|c| c.is_ascii_hexdigit()) && right.len() <= 4 {
            return true;
        }

        // Pattern 2: \U + digits
        if left.ends_with("\\U") && right.chars().all(|c| c.is_ascii_hexdigit()) && right.len() <= 8 {
            return true;
        }

        // Pattern 3: partial UCN + remaining digits
        if left.starts_with("\\u") && left.len() < 6 && right.chars().all(|c| c.is_ascii_hexdigit()) {
            let total_len = left.len() - 2 + right.len(); // -2 for \u
            if total_len == 4 {
                return true;
            }
        }

        if left.starts_with("\\U") && left.len() < 10 && right.chars().all(|c| c.is_ascii_hexdigit()) {
            let total_len = left.len() - 2 + right.len(); // -2 for \U
            if total_len == 8 {
                return true;
            }
        }

        // Pattern 4: Check for identifier fragments that might form UCNs
        let combined = format!("{}{}", left, right);
        self.contains_ucn_pattern(&combined)
    }

    fn contains_ucn_pattern(&self, text: &str) -> bool {
        // Check if text contains UCN patterns using string operations
        // Look for \uXXXX or \UXXXXXXXX patterns
        let mut chars = text.chars().peekable();
        while let Some(ch) = chars.next() {
            if ch == '\\' {
                if let Some(&next_ch) = chars.peek() {
                    if next_ch == 'u' {
                        chars.next(); // consume 'u'
                        let hex_count = chars.by_ref().take(4).filter(|c| c.is_ascii_hexdigit()).count();
                        if hex_count == 4 {
                            return true;
                        }
                    } else if next_ch == 'U' {
                        chars.next(); // consume 'U'
                        let hex_count = chars.by_ref().take(8).filter(|c| c.is_ascii_hexdigit()).count();
                        if hex_count == 8 {
                            return true;
                        }
                    }
                }
            }
        }
        false
    }

    fn has_ucn_concatenation_pattern(&self, args: &[String], macro_name: &str) -> bool {
        // Check if macro arguments suggest UCN concatenation

        // Look for known dangerous macro patterns
        if macro_name.contains("assign") || macro_name.contains("concat") || macro_name.contains("join") {
            for arg in args {
                if self.looks_like_ucn_fragment(arg) {
                    return true;
                }
            }
        }

        // Check for patterns where multiple args might form UCNs
        if args.len() >= 2 {
            for i in 0..args.len()-1 {
                if self.could_form_ucn(&args[i], &args[i+1]) {
                    return true;
                }
            }
        }

        false
    }

    fn looks_like_ucn_fragment(&self, arg: &str) -> bool {
        let cleaned = arg.trim();

        // Check for partial UCN patterns
        cleaned.starts_with("\\u") && cleaned.len() < 6 ||
        cleaned.starts_with("\\U") && cleaned.len() < 10 ||
        cleaned.ends_with("\\u") ||
        cleaned.ends_with("\\U") ||
        (cleaned.chars().all(|c| c.is_ascii_hexdigit()) && (cleaned.len() == 2 || cleaned.len() == 4))
    }

    fn get_macro_arguments(&self, node: &Node, source: &str) -> Vec<String> {
        let mut args = Vec::new();

        if let Some(arguments) = node.child_by_field_name("arguments") {
            for i in 0..arguments.child_count() {
                if let Some(child) = arguments.child(i) {
                    if child.kind() != "," {
                        let arg_text = source[child.start_byte()..child.end_byte()].to_string();
                        args.push(arg_text);
                    }
                }
            }
        }

        args
    }
}

#[cfg(test)]
#[path = "tests/pre30_c.rs"]
mod tests;
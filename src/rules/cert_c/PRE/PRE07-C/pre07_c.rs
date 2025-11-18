use crate::manifest::{RuleCategory, Severity};
use crate::prelude::RuleViolation;
use crate::rules::cert_c::CertRule;
use tree_sitter::Node;

pub struct PRE07C;

impl CertRule for PRE07C {
    fn rule_id(&self) -> &'static str {
        "PRE07-C"
    }

    fn cert_id(&self) -> &'static str {
        "PRE07"
    }

    fn description(&self) -> &'static str {
        "Avoid using repeated question marks in C source code"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn check(&self, _node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Trigraph sequences: ??= ??/ ??' ??( ??) ??! ??< ??> ??-
        // These are replaced by the preprocessor with: # \ ^ [ ] | { } ~
        let trigraph_chars = ['=', '/', '\'', '(', ')', '!', '<', '>', '-'];

        // Search source for trigraph patterns
        let lines: Vec<&str> = source.lines().collect();
        
        for (line_idx, line) in lines.iter().enumerate() {
            // Work with bytes to avoid UTF-8 issues
            let line_bytes = line.as_bytes();
            
            let mut i = 0;
            while i + 2 < line_bytes.len() {
                // Check for ?? followed by trigraph character
                if line_bytes[i] == b'?' && line_bytes[i + 1] == b'?' {
                    let third_char = line_bytes[i + 2] as char;
                    if trigraph_chars.contains(&third_char) {
                        // Check if this is escaped by string splitting (like "?" "?!")
                        // Look backwards for "? and forwards for "
                        let mut is_escaped = false;
                        
                        // Simple check: if we see \" immediately before the second ? and " after third char
                        if i >= 2 && i + 3 < line_bytes.len() {
                            if line_bytes[i - 1] == b'?' && 
                               line_bytes[i - 2] == b'"' &&
                               line_bytes[i + 3] == b'"' {
                                is_escaped = true;
                            }
                        }
                        
                        if !is_escaped {
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                file_path: String::new(),
                                message: format!(
                                    "Trigraph sequence '??{}' detected. Trigraphs can cause unintended behavior and are deprecated.",
                                    third_char
                                ),
                                line: line_idx + 1,
                                column: i + 1,
                                severity: self.severity(),
                                suggestion: Some("Avoid trigraphs by using alternative syntax or escaping the question marks".to_string()),
                                requires_manual_review: Some(false),
                            });
                        }
                    }
                }
                i += 1;
            }
        }

        violations
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_pre07_c() {
        let rule = PRE07C;
        assert_eq!(rule.rule_id(), "PRE07-C");
        assert_eq!(rule.cert_id(), "PRE07");
    }
}

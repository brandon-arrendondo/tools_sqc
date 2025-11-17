use serde::{Deserialize, Serialize};
use sha2::{Digest, Sha256};
use std::collections::HashMap;

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Suppression {
    pub rule_id: String,
    pub justification: String,
    pub fingerprint: SuppressionFingerprint,
    pub review_date: Option<String>,
    pub reviewer: Option<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum SuppressionFingerprint {
    /// Hash of the exact code being suppressed
    CodeHash { hash: String, lines: (usize, usize) },

    /// AST-based semantic fingerprint
    AstSignature {
        signature: String,
        node_type: String,
    },

    /// Next N lines with pattern matching
    NextLines {
        line_count: usize,
        pattern: Option<String>,
        checksum: String,
    },
}

impl Suppression {
    /// Parse suppression comments from source code
    /// Formats supported:
    /// - // SQC-SUPPRESS: ARR30-C HASH:a3f5d2b1 JUSTIFICATION: "Validated by caller"
    /// - /* SQC-SUPPRESS-NEXT: MEM30-C LINES:3 PATTERN:"free(*)" */
    pub fn parse_from_comment(comment: &str, line_number: usize, source: &str) -> Option<Self> {
        if !comment.contains("SQC-SUPPRESS") {
            return None;
        }

        // Parse the suppression directive
        let rule_id = Self::extract_rule_id(comment)?;
        let justification = Self::extract_justification(comment).unwrap_or_default();

        let fingerprint = if comment.contains("HASH:") {
            let hash = Self::extract_hash(comment)?;
            let lines = Self::find_code_range(line_number, source);
            SuppressionFingerprint::CodeHash { hash, lines }
        } else if comment.contains("SQC-SUPPRESS-NEXT") {
            let line_count = Self::extract_line_count(comment).unwrap_or(1);
            let pattern = Self::extract_pattern(comment);
            let checksum = Self::calculate_checksum(source, line_number + 1, line_count);
            SuppressionFingerprint::NextLines {
                line_count,
                pattern,
                checksum,
            }
        } else {
            // Default to code hash for the next line
            let lines = (line_number + 1, line_number + 1);
            let hash = Self::calculate_hash_for_lines(source, lines.0, lines.1);
            SuppressionFingerprint::CodeHash { hash, lines }
        };

        Some(Suppression {
            rule_id,
            justification,
            fingerprint,
            review_date: Self::extract_review_date(comment),
            reviewer: Self::extract_reviewer(comment),
        })
    }

    /// Verify if suppression is still valid for the current code
    pub fn is_valid(&self, source: &str, violation_line: usize) -> bool {
        match &self.fingerprint {
            SuppressionFingerprint::CodeHash { hash, lines } => {
                // Check if violation is within suppressed range
                if violation_line < lines.0 || violation_line > lines.1 {
                    return false;
                }

                // Extract the code and calculate hash including rule ID
                let code = Self::extract_lines(source, lines.0, lines.1 - lines.0 + 1);
                let current_hash =
                    SuppressionManager::calculate_suppression_hash(&self.rule_id, &code);
                current_hash == *hash
            }

            SuppressionFingerprint::NextLines {
                line_count,
                pattern,
                checksum,
            } => {
                // Verify checksum of the suppressed lines
                let current_checksum =
                    Self::calculate_checksum(source, violation_line, *line_count);
                if current_checksum != *checksum {
                    return false;
                }

                // If pattern specified, verify it matches
                if let Some(pat) = pattern {
                    let code = Self::extract_lines(source, violation_line, *line_count);
                    code.contains(pat)
                } else {
                    true
                }
            }

            SuppressionFingerprint::AstSignature {
                signature,
                node_type: _,
            } => {
                // Would need AST comparison here
                // For now, just check if signature appears in nearby code
                let context = Self::extract_lines(source, violation_line.saturating_sub(2), 5);
                context.contains(signature)
            }
        }
    }

    fn calculate_hash_for_lines(source: &str, start_line: usize, end_line: usize) -> String {
        let code = Self::extract_lines(source, start_line, end_line - start_line + 1);
        // Note: This is for internal use, not for suppression hashes
        // For suppression hashes, use calculate_suppression_hash which includes rule ID
        let mut hasher = Sha256::new();
        hasher.update(code.trim().as_bytes());
        format!("{:x}", hasher.finalize())[..16].to_string()
    }

    fn calculate_checksum(source: &str, start_line: usize, line_count: usize) -> String {
        Self::calculate_hash_for_lines(source, start_line, start_line + line_count - 1)
    }

    fn extract_lines(source: &str, start_line: usize, count: usize) -> String {
        source
            .lines()
            .skip(start_line.saturating_sub(1))
            .take(count)
            .collect::<Vec<_>>()
            .join("\n")
    }

    fn find_code_range(comment_line: usize, source: &str) -> (usize, usize) {
        // Find the next non-comment, non-empty line after the suppression
        let lines: Vec<&str> = source.lines().collect();
        let mut start = comment_line;
        let mut end = comment_line;

        for i in comment_line..lines.len() {
            let line = lines[i].trim();
            if !line.is_empty() && !line.starts_with("//") && !line.starts_with("/*") {
                start = i + 1;

                // Find the end of the statement/block
                if line.contains('{') {
                    // Find matching closing brace
                    let mut brace_count = 1;
                    for j in (i + 1)..lines.len() {
                        if lines[j].contains('{') {
                            brace_count += 1;
                        }
                        if lines[j].contains('}') {
                            brace_count -= 1;
                            if brace_count == 0 {
                                end = j + 1;
                                break;
                            }
                        }
                    }
                } else {
                    // Single line statement
                    end = start;
                }
                break;
            }
        }

        (start, end)
    }

    fn extract_rule_id(comment: &str) -> Option<String> {
        let re = regex::Regex::new(r"SQC-SUPPRESS(?:-NEXT)?:\s*([A-Z0-9-]+)").ok()?;
        re.captures(comment)?.get(1).map(|m| m.as_str().to_string())
    }

    fn extract_justification(comment: &str) -> Option<String> {
        let re = regex::Regex::new(
            r#"JUSTIFICATION:\s*"([^"]+)"|JUSTIFICATION:\s*(.+?)(?:\s+[A-Z]+:|$)"#,
        )
        .ok()?;
        re.captures(comment).and_then(|cap| {
            cap.get(1)
                .or(cap.get(2))
                .map(|m| m.as_str().trim().to_string())
        })
    }

    fn extract_hash(comment: &str) -> Option<String> {
        let re = regex::Regex::new(r"HASH:([a-f0-9]+)").ok()?;
        re.captures(comment)?.get(1).map(|m| m.as_str().to_string())
    }

    fn extract_pattern(comment: &str) -> Option<String> {
        let re = regex::Regex::new(r#"PATTERN:"([^"]+)""#).ok()?;
        re.captures(comment)?.get(1).map(|m| m.as_str().to_string())
    }

    fn extract_line_count(comment: &str) -> Option<usize> {
        let re = regex::Regex::new(r"LINES:(\d+)").ok()?;
        re.captures(comment)?
            .get(1)
            .and_then(|m| m.as_str().parse().ok())
    }

    fn extract_review_date(comment: &str) -> Option<String> {
        let re = regex::Regex::new(r"DATE:(\d{4}-\d{2}-\d{2})").ok()?;
        re.captures(comment)?.get(1).map(|m| m.as_str().to_string())
    }

    fn extract_reviewer(comment: &str) -> Option<String> {
        let re = regex::Regex::new(r"REVIEWER:(\w+)").ok()?;
        re.captures(comment)?.get(1).map(|m| m.as_str().to_string())
    }
}

pub struct SuppressionManager {
    suppressions: HashMap<String, Vec<Suppression>>,
}

impl SuppressionManager {
    pub fn new() -> Self {
        Self {
            suppressions: HashMap::new(),
        }
    }

    /// Extract all suppressions from source code
    pub fn extract_from_source(&mut self, file_path: &str, source: &str) {
        let mut file_suppressions = Vec::new();

        for (line_num, line) in source.lines().enumerate() {
            let trimmed = line.trim();
            if trimmed.starts_with("//") || trimmed.starts_with("/*") {
                if let Some(suppression) =
                    Suppression::parse_from_comment(trimmed, line_num, source)
                {
                    file_suppressions.push(suppression);
                }
            }
        }

        if !file_suppressions.is_empty() {
            self.suppressions
                .insert(file_path.to_string(), file_suppressions);
        }
    }

    /// Check if a violation should be suppressed
    pub fn should_suppress(
        &self,
        file_path: &str,
        rule_id: &str,
        line: usize,
        source: &str,
    ) -> Option<&Suppression> {
        self.suppressions
            .get(file_path)?
            .iter()
            .find(|s| s.rule_id == rule_id && s.is_valid(source, line))
    }

    /// Generate a suppression comment for a violation
    pub fn generate_suppression_comment(
        rule_id: &str,
        source: &str,
        line: usize,
        justification: &str,
    ) -> String {
        let code_line = source.lines().nth(line.saturating_sub(1)).unwrap_or("");
        let hash = Self::calculate_suppression_hash(rule_id, code_line);

        format!(
            "// SQC-SUPPRESS: {} HASH:{} JUSTIFICATION: \"{}\"",
            rule_id, hash, justification
        )
    }

    /// Calculate hash that includes both the rule ID(s) and the code
    pub fn calculate_suppression_hash(rule_ids: &str, code: &str) -> String {
        let mut hasher = Sha256::new();
        // Include rule IDs in the hash to make it specific to the rules being suppressed
        hasher.update(rule_ids.as_bytes());
        hasher.update(b":");
        // Include the normalized code (trimmed and whitespace-normalized)
        let normalized_code = code.trim().split_whitespace().collect::<Vec<_>>().join(" ");
        hasher.update(normalized_code.as_bytes());
        format!("{:x}", hasher.finalize())[..16].to_string()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_parse_suppression() {
        let comment =
            "// SQC-SUPPRESS: ARR30-C HASH:a3f5d2b1 JUSTIFICATION: \"Bounds checked by caller\"";
        let source = "int arr[5];\narr[10] = 0;";

        let suppression = Suppression::parse_from_comment(comment, 0, source).unwrap();
        assert_eq!(suppression.rule_id, "ARR30-C");
        assert_eq!(suppression.justification, "Bounds checked by caller");
    }

    #[test]
    fn test_hash_validation() {
        let source = "int x = 5;\nint y = 10;\nint z = x + y;";
        let suppression = Suppression {
            rule_id: "TEST-1".to_string(),
            justification: "Test".to_string(),
            fingerprint: SuppressionFingerprint::CodeHash {
                hash: Suppression::calculate_hash_for_lines(source, 2, 2),
                lines: (2, 2),
            },
            review_date: None,
            reviewer: None,
        };

        // Should be valid for unchanged code
        assert!(suppression.is_valid(source, 2));

        // Should be invalid for changed code
        let modified = "int x = 5;\nint y = 20;\nint z = x + y;";
        assert!(!suppression.is_valid(modified, 2));
    }
}

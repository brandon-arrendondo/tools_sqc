use serde::{Deserialize, Serialize};
use sha2::{Digest, Sha256};
use std::collections::HashMap;
use std::path::Path;

/// Maximum number of lines a suppress comment can appear before the violation it covers.
const MAX_PROXIMITY: usize = 5;

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Suppression {
    pub rule_id: String,
    pub hash: String,
    pub justification: String,
    /// 1-based line number of the suppress comment in the source file.
    /// 0 means this came from a TOML file (no proximity check).
    pub comment_line: usize,
}

/// A single entry in `.sqc-suppress.toml`.
#[derive(Debug, Clone, Deserialize)]
pub struct TomlSuppressionEntry {
    pub file: String,
    pub rule: String,
    pub hash: String,
    #[serde(default)]
    pub justification: String,
}

/// Top-level structure of `.sqc-suppress.toml`.
#[derive(Debug, Clone, Deserialize)]
pub struct TomlSuppressionFile {
    #[serde(default)]
    pub suppression: Vec<TomlSuppressionEntry>,
}

impl Suppression {
    /// Parse a SQC-SUPPRESS directive from a comment.
    ///
    /// `line_number` is 0-based (from enumerate). The comment may be a standalone
    /// line (`// SQC-SUPPRESS: ...`) or an inline suffix on a code line.
    pub fn parse(comment: &str, line_number: usize) -> Option<Self> {
        if !comment.contains("SQC-SUPPRESS") {
            return None;
        }

        let rule_id = Self::extract_rule_id(comment)?;
        let hash = Self::extract_hash(comment)?;
        let justification = Self::extract_justification(comment).unwrap_or_default();

        Some(Suppression {
            rule_id,
            hash,
            justification,
            comment_line: line_number + 1, // convert to 1-based
        })
    }

    /// Check whether this suppression matches a violation on `violation_line` (1-based).
    ///
    /// Two checks:
    /// 1. Proximity — comment is on the same line or up to MAX_PROXIMITY lines before
    ///    (skipped for TOML entries where comment_line == 0)
    /// 2. Hash — SHA-256 of (rule_id + normalized violation line) matches stored hash
    pub fn matches(&self, source: &str, violation_line: usize) -> bool {
        // Skip proximity check for TOML-sourced suppressions (comment_line == 0)
        if self.comment_line > 0 {
            if self.comment_line > violation_line
                || violation_line - self.comment_line > MAX_PROXIMITY
            {
                return false;
            }
        }

        // Hash the violation line (stripping any inline SQC-SUPPRESS comment)
        let code = Self::get_line(source, violation_line);
        let code = Self::strip_suppress_comment(&code);
        let current_hash = SuppressionManager::calculate_suppression_hash(&self.rule_id, &code);
        current_hash == self.hash
    }

    /// Get a single 1-based line from source.
    fn get_line(source: &str, line: usize) -> String {
        source
            .lines()
            .nth(line.saturating_sub(1))
            .unwrap_or("")
            .to_string()
    }

    /// Strip an inline `// SQC-SUPPRESS...` suffix from a code line so the hash
    /// covers only the code portion.
    fn strip_suppress_comment(line: &str) -> String {
        if let Some(pos) = line.find("// SQC-SUPPRESS") {
            line[..pos].to_string()
        } else if let Some(pos) = line.find("/* SQC-SUPPRESS") {
            line[..pos].to_string()
        } else {
            line.to_string()
        }
    }

    fn extract_rule_id(comment: &str) -> Option<String> {
        let re = regex::Regex::new(r"SQC-SUPPRESS:\s*([A-Z0-9]+-[A-Z0-9]+)").ok()?;
        re.captures(comment)?.get(1).map(|m| m.as_str().to_string())
    }

    fn extract_hash(comment: &str) -> Option<String> {
        let re = regex::Regex::new(r"HASH:([a-f0-9]+)").ok()?;
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
}

#[derive(Default)]
pub struct SuppressionManager {
    /// Inline comment suppressions, keyed by full file path.
    suppressions: HashMap<String, Vec<Suppression>>,
    /// TOML file suppressions (from `.sqc-suppress.toml`), keyed by the `file` field from TOML.
    toml_suppressions: Vec<(String, Suppression)>,
}

impl SuppressionManager {
    pub fn new() -> Self {
        Self::default()
    }

    /// Load suppressions from a `.sqc-suppress.toml` file.
    pub fn load_from_toml(&mut self, toml_path: &str) -> Result<usize, String> {
        let content = std::fs::read_to_string(toml_path)
            .map_err(|e| format!("Cannot read {}: {}", toml_path, e))?;
        let parsed: TomlSuppressionFile = toml::from_str(&content)
            .map_err(|e| format!("Invalid TOML in {}: {}", toml_path, e))?;

        let count = parsed.suppression.len();
        for entry in parsed.suppression {
            let suppression = Suppression {
                rule_id: entry.rule,
                hash: entry.hash,
                justification: entry.justification,
                comment_line: 0, // sentinel: skip proximity check
            };
            self.toml_suppressions.push((entry.file, suppression));
        }
        Ok(count)
    }

    /// Extract all suppressions from source code.
    ///
    /// Scans every line for SQC-SUPPRESS directives. Supports both standalone
    /// comment lines and inline comments on code lines.
    pub fn extract_from_source(&mut self, file_path: &str, source: &str) {
        let mut file_suppressions = Vec::new();

        for (line_num, line) in source.lines().enumerate() {
            if line.contains("SQC-SUPPRESS") {
                if let Some(suppression) = Suppression::parse(line, line_num) {
                    file_suppressions.push(suppression);
                }
            }
        }

        if !file_suppressions.is_empty() {
            self.suppressions
                .insert(file_path.to_string(), file_suppressions);
        }
    }

    /// Check if a violation should be suppressed.
    ///
    /// Checks inline comment suppressions first, then TOML file suppressions.
    /// Returns the matching suppression if found.
    pub fn should_suppress(
        &self,
        file_path: &str,
        rule_id: &str,
        line: usize,
        source: &str,
    ) -> Option<&Suppression> {
        // Check inline comment suppressions
        if let Some(file_supps) = self.suppressions.get(file_path) {
            if let Some(s) = file_supps
                .iter()
                .find(|s| s.rule_id == rule_id && s.matches(source, line))
            {
                return Some(s);
            }
        }

        // Check TOML suppressions (suffix match on file path)
        self.toml_suppressions.iter().find_map(|(pattern, s)| {
            if s.rule_id == rule_id
                && file_path_matches(file_path, pattern)
                && s.matches(source, line)
            {
                Some(s)
            } else {
                None
            }
        })
    }

    /// Calculate hash: SHA-256(rule_id + ":" + whitespace-normalized code), truncated to 16 hex.
    pub fn calculate_suppression_hash(rule_id: &str, code: &str) -> String {
        let mut hasher = Sha256::new();
        hasher.update(rule_id.as_bytes());
        hasher.update(b":");
        let normalized = code.split_whitespace().collect::<Vec<_>>().join(" ");
        hasher.update(normalized.as_bytes());
        format!("{:x}", hasher.finalize())[..16].to_string()
    }
}

/// Match a full file path against a TOML suppression file pattern.
///
/// The pattern can be a bare filename (`ringbuffer.c`), a relative path
/// (`src/ringbuffer.c`), or a full path. Matching is by suffix: the full
/// path must end with the pattern, preceded by a path separator or start of string.
fn file_path_matches(full_path: &str, pattern: &str) -> bool {
    if full_path == pattern {
        return true;
    }
    let normalized = full_path.replace('\\', "/");
    let norm_pattern = pattern.replace('\\', "/");
    if normalized.ends_with(&norm_pattern) {
        let prefix_len = normalized.len() - norm_pattern.len();
        prefix_len == 0
            || normalized.as_bytes().get(prefix_len - 1) == Some(&b'/')
            || normalized.as_bytes().get(prefix_len - 1) == Some(&b'\\')
    } else {
        // Also try matching just the filename component
        let file_name = Path::new(full_path)
            .file_name()
            .and_then(|f| f.to_str())
            .unwrap_or("");
        file_name == pattern
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_parse_basic() {
        let comment =
            "// SQC-SUPPRESS: ARR30-C HASH:a3f5d2b1 JUSTIFICATION: \"Bounds checked by caller\"";
        let s = Suppression::parse(comment, 0).unwrap();
        assert_eq!(s.rule_id, "ARR30-C");
        assert_eq!(s.hash, "a3f5d2b1");
        assert_eq!(s.justification, "Bounds checked by caller");
        assert_eq!(s.comment_line, 1);
    }

    #[test]
    fn test_parse_no_hash_returns_none() {
        let comment = "// SQC-SUPPRESS: ARR30-C JUSTIFICATION: \"no hash\"";
        assert!(Suppression::parse(comment, 0).is_none());
    }

    #[test]
    fn test_match_line_before() {
        // Suppress comment on line 1, violation on line 2
        let rule_id = "EXP34-C";
        let code_line = "    *ptr = value;";
        let hash = SuppressionManager::calculate_suppression_hash(rule_id, code_line);

        let source = format!(
            "// SQC-SUPPRESS: EXP34-C HASH:{} JUSTIFICATION: \"test\"\n{}",
            hash, code_line
        );

        let s = Suppression::parse(source.lines().next().unwrap(), 0).unwrap();
        assert!(s.matches(&source, 2));
    }

    #[test]
    fn test_match_inline() {
        // Suppress comment inline on the same line as the violation
        let rule_id = "EXP34-C";
        let code_line = "    *ptr = value;";
        let hash = SuppressionManager::calculate_suppression_hash(rule_id, code_line);

        let line_with_suppress = format!(
            "{} // SQC-SUPPRESS: EXP34-C HASH:{} JUSTIFICATION: \"test\"",
            code_line, hash
        );
        let source = format!("int x;\n{}\nint y;", line_with_suppress);

        let s = Suppression::parse(&line_with_suppress, 1).unwrap();
        // Violation on line 2 (same line as the inline suppress comment)
        assert!(s.matches(&source, 2));
    }

    #[test]
    fn test_match_stacked() {
        // Two suppress comments before one code line
        let code_line = "    bytes_read = fread(buf, 1, file_size, fp);";
        let hash_err = SuppressionManager::calculate_suppression_hash("ERR00-C", code_line);
        let hash_exp = SuppressionManager::calculate_suppression_hash("EXP34-C", code_line);

        let source = format!(
            "// SQC-SUPPRESS: ERR00-C HASH:{} JUSTIFICATION: \"r1\"\n\
             // SQC-SUPPRESS: EXP34-C HASH:{} JUSTIFICATION: \"r2\"\n\
             {}",
            hash_err, hash_exp, code_line
        );

        let s1 = Suppression::parse(source.lines().nth(0).unwrap(), 0).unwrap();
        let s2 = Suppression::parse(source.lines().nth(1).unwrap(), 1).unwrap();

        // Both should match violations on line 3
        assert!(s1.matches(&source, 3));
        assert!(s2.matches(&source, 3));
    }

    #[test]
    fn test_no_match_wrong_rule() {
        let code_line = "    *ptr = value;";
        let hash = SuppressionManager::calculate_suppression_hash("INT30-C", code_line);

        let source = format!(
            "// SQC-SUPPRESS: INT30-C HASH:{} JUSTIFICATION: \"test\"\n{}",
            hash, code_line
        );

        let s = Suppression::parse(source.lines().next().unwrap(), 0).unwrap();
        // Rule ID won't match at the should_suppress level, but matches() only checks hash
        assert!(s.matches(&source, 2));
    }

    #[test]
    fn test_no_match_too_far() {
        let code_line = "    *ptr = value;";
        let hash = SuppressionManager::calculate_suppression_hash("EXP34-C", code_line);

        // Comment on line 1, violation on line 20 — too far
        let mut lines = vec![format!(
            "// SQC-SUPPRESS: EXP34-C HASH:{} JUSTIFICATION: \"test\"",
            hash
        )];
        for _ in 0..18 {
            lines.push("// filler".to_string());
        }
        lines.push(code_line.to_string());
        let source = lines.join("\n");

        let s = Suppression::parse(source.lines().next().unwrap(), 0).unwrap();
        assert!(!s.matches(&source, 20));
    }

    #[test]
    fn test_no_match_code_changed() {
        let original_code = "    *ptr = value;";
        let hash = SuppressionManager::calculate_suppression_hash("EXP34-C", original_code);

        // Code has been modified since hash was generated
        let modified_source = format!(
            "// SQC-SUPPRESS: EXP34-C HASH:{} JUSTIFICATION: \"test\"\n    *ptr = new_value;",
            hash
        );

        let s = Suppression::parse(modified_source.lines().next().unwrap(), 0).unwrap();
        assert!(!s.matches(&modified_source, 2));
    }

    #[test]
    fn test_strip_suppress_comment() {
        let line = "    *ptr = val; // SQC-SUPPRESS: EXP34-C HASH:abc JUSTIFICATION: \"test\"";
        let stripped = Suppression::strip_suppress_comment(line);
        assert_eq!(stripped, "    *ptr = val; ");
    }

    #[test]
    fn test_manager_roundtrip() {
        let rule_id = "EXP34-C";
        let code_line = "    *ptr = value;";
        let hash = SuppressionManager::calculate_suppression_hash(rule_id, code_line);

        let source = format!(
            "void f(int *ptr) {{\n\
             // SQC-SUPPRESS: EXP34-C HASH:{} JUSTIFICATION: \"validated by caller\"\n\
             {}\n\
             }}",
            hash, code_line
        );

        let mut mgr = SuppressionManager::new();
        mgr.extract_from_source("test.c", &source);

        // Should suppress EXP34-C on line 3
        assert!(mgr
            .should_suppress("test.c", "EXP34-C", 3, &source)
            .is_some());

        // Should NOT suppress different rule
        assert!(mgr
            .should_suppress("test.c", "INT30-C", 3, &source)
            .is_none());

        // Should NOT suppress different file
        assert!(mgr
            .should_suppress("other.c", "EXP34-C", 3, &source)
            .is_none());
    }

    #[test]
    fn test_file_path_matches_bare_filename() {
        assert!(file_path_matches(
            "/home/user/project/src/ringbuffer.c",
            "ringbuffer.c"
        ));
        assert!(file_path_matches("ringbuffer.c", "ringbuffer.c"));
        assert!(!file_path_matches(
            "/home/user/project/src/other.c",
            "ringbuffer.c"
        ));
    }

    #[test]
    fn test_file_path_matches_relative_path() {
        assert!(file_path_matches(
            "/home/user/project/src/ringbuffer.c",
            "src/ringbuffer.c"
        ));
        assert!(!file_path_matches(
            "/home/user/project/lib/ringbuffer.c",
            "src/ringbuffer.c"
        ));
    }

    #[test]
    fn test_toml_suppression() {
        let rule_id = "INT30-C";
        let code_line = "    result = a + b;";
        let hash = SuppressionManager::calculate_suppression_hash(rule_id, code_line);

        // Build source: violation on line 5
        let source = format!(
            "void f() {{\n  int a = 1;\n  int b = 2;\n  unsigned result;\n{}\n}}",
            code_line
        );

        let mut mgr = SuppressionManager::new();
        // Add a TOML-sourced suppression (comment_line = 0)
        mgr.toml_suppressions.push((
            "test.c".to_string(),
            Suppression {
                rule_id: rule_id.to_string(),
                hash,
                justification: "overflow checked".to_string(),
                comment_line: 0,
            },
        ));

        // Should match via TOML (no proximity check, suffix match on filename)
        assert!(mgr
            .should_suppress("/some/path/test.c", rule_id, 5, &source)
            .is_some());

        // Should NOT match different file
        assert!(mgr
            .should_suppress("/some/path/other.c", rule_id, 5, &source)
            .is_none());
    }

    #[test]
    fn test_inline_and_toml_coexist() {
        // Inline suppresses one rule, TOML suppresses a different rule on the same file
        let code_line = "    result = *ptr + offset;";
        let hash_exp = SuppressionManager::calculate_suppression_hash("EXP34-C", code_line);
        let hash_int = SuppressionManager::calculate_suppression_hash("INT30-C", code_line);

        // Source has an inline suppression for EXP34-C on line 2
        let source = format!(
            "// SQC-SUPPRESS: EXP34-C HASH:{} JUSTIFICATION: \"ptr checked\"\n{}",
            hash_exp, code_line
        );

        let mut mgr = SuppressionManager::new();
        mgr.extract_from_source("/project/test.c", &source);

        // Add TOML suppression for INT30-C on the same file
        mgr.toml_suppressions.push((
            "test.c".to_string(),
            Suppression {
                rule_id: "INT30-C".to_string(),
                hash: hash_int,
                justification: "overflow checked".to_string(),
                comment_line: 0,
            },
        ));

        // EXP34-C suppressed by inline comment
        assert!(mgr
            .should_suppress("/project/test.c", "EXP34-C", 2, &source)
            .is_some());

        // INT30-C suppressed by TOML entry
        assert!(mgr
            .should_suppress("/project/test.c", "INT30-C", 2, &source)
            .is_some());

        // ARR30-C not suppressed by either
        assert!(mgr
            .should_suppress("/project/test.c", "ARR30-C", 2, &source)
            .is_none());
    }

    #[test]
    fn test_inline_detection() {
        // Ensure extract_from_source finds inline suppress comments (not just standalone)
        let code_line = "    free(ptr);";
        let hash = SuppressionManager::calculate_suppression_hash("EXP34-C", code_line);

        let source = format!(
            "void f(void *ptr) {{\n\
             {} // SQC-SUPPRESS: EXP34-C HASH:{} JUSTIFICATION: \"NULL is valid\"\n\
             }}",
            code_line, hash
        );

        let mut mgr = SuppressionManager::new();
        mgr.extract_from_source("test.c", &source);

        assert!(mgr
            .should_suppress("test.c", "EXP34-C", 2, &source)
            .is_some());
    }
}

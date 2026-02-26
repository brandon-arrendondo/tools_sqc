//! PRE08-C: Guarantee that header file names are unique
//!
//! This rule ensures that included header file names are unambiguous across different
//! C implementations. The C Standard only guarantees significance of the first 8
//! characters of a filename (case-insensitive), so all included headers must differ
//! in their first 8 characters.
//!
//! ## Non-compliant example:
//!
//! ```c
//! #include "Library.h"
//! #include "library.h"        // First 8 chars identical (case-insensitive)
//!
//! #include "utilities_math.h"
//! #include "utilities_physics.h"  // First 8 chars identical: "utilities"
//! ```
//!
//! ## Compliant solution:
//!
//! ```c
//! #include "Lib_main.h"
//! #include "lib_2.h"          // First 8 chars differ
//!
//! #include "util_math.h"
//! #include "util_phys.h"      // First 8 chars differ
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use std::collections::HashMap;
use tree_sitter::Node;

pub struct Pre08C;

impl Pre08C {
    pub fn new() -> Self {
        Self
    }

    /// Extract the header filename from an include directive
    fn extract_header_name(&self, node: &Node, source: &str) -> Option<String> {
        // Look for the path node which contains the filename
        let path_node = node.child_by_field_name("path")?;
        let path_text = get_node_text(&path_node, source);

        // Remove surrounding quotes or angle brackets
        let trimmed = path_text.trim();
        let filename = if trimmed.starts_with('"') && trimmed.ends_with('"') {
            trimmed.trim_matches('"')
        } else if trimmed.starts_with('<') && trimmed.ends_with('>') {
            trimmed.trim_matches('<').trim_matches('>')
        } else {
            trimmed
        };

        // Extract just the filename (without directory path)
        let basename = filename.rsplit('/').next().unwrap_or(filename);

        Some(basename.to_string())
    }

    /// Get the significant portion of a filename (first 8 chars, case-insensitive)
    fn get_significant_name(&self, filename: &str) -> String {
        // Remove extension for comparison
        let without_ext = filename.split('.').next().unwrap_or(filename);

        // Take first 8 characters and convert to lowercase for case-insensitive comparison
        let significant = if without_ext.len() > 8 {
            &without_ext[..8]
        } else {
            without_ext
        };

        significant.to_lowercase()
    }

    /// Check all include directives in the translation unit for uniqueness
    fn check_include_uniqueness(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Only check at the translation_unit level to avoid duplicate checks
        if node.kind() != "translation_unit" {
            return;
        }

        // Collect all include directives
        let mut includes: HashMap<String, Vec<(String, usize)>> = HashMap::new();
        self.collect_includes(node, source, &mut includes);

        // Check for conflicts (headers with same first 8 chars)
        for (_significant_name, headers) in includes.iter() {
            if headers.len() > 1 {
                // Multiple headers map to the same significant name - report violations
                for (i, (header1, line1)) in headers.iter().enumerate() {
                    for (header2, _line2) in headers.iter().skip(i + 1) {
                        // Report violation for the first occurrence
                        violations.push(RuleViolation {
                            rule_id: self.rule_id().to_string(),
                            severity: self.severity(),
                            message: format!(
                                "Header file '{}' has the same first 8 characters (case-insensitive) as '{}'. This may cause ambiguity on systems with limited filename support.",
                                header1, header2
                            ),
                            file_path: String::new(),
                            line: *line1,
                            column: 1,
                            suggestion: Some(format!(
                                "Rename one of the headers to ensure the first 8 characters differ (e.g., '{}' -> unique name)",
                                header1
                            )),
                            ..Default::default()
                        });
                    }
                }
            }
        }
    }

    /// Recursively collect all include directives
    fn collect_includes(
        &self,
        node: &Node,
        source: &str,
        includes: &mut HashMap<String, Vec<(String, usize)>>,
    ) {
        if node.kind() == "preproc_include" {
            if let Some(header) = self.extract_header_name(node, source) {
                let significant = self.get_significant_name(&header);
                let line = node.start_position().row + 1;
                includes
                    .entry(significant)
                    .or_default()
                    .push((header, line));
            }
        }

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.collect_includes(&child, source, includes);
            }
        }
    }
}

impl CertRule for Pre08C {
    fn rule_id(&self) -> &'static str {
        "PRE08-C"
    }

    fn description(&self) -> &'static str {
        "Guarantee that header file names are unique"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "PRE08-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_include_uniqueness(node, source, &mut violations);
        violations
    }
}

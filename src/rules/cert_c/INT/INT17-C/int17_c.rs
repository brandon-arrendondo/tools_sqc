//! INT17-C: Define integer constants in an implementation-independent manner
//!
//! Integer constants, especially hexadecimal ones, should not assume specific bit-widths
//! as this creates implementation-dependent code that may fail on platforms with different
//! integer sizes.
//!
//! ## Examples:
//!
//! **Non-compliant:**
//! ```c
//! const unsigned long mask = 0xFFFFFFFF;  // Assumes 32-bit unsigned long
//! unsigned long flipbits(unsigned long x) {
//!     return x ^ mask;  // Won't work correctly on 64-bit systems
//! }
//! ```
//!
//! **Compliant:**
//! ```c
//! const unsigned long mask = -1;  // All bits set regardless of size
//! unsigned long flipbits(unsigned long x) {
//!     return x ^ mask;
//! }
//!
//! // Or for MSB:
//! const unsigned long msb = ~(ULONG_MAX >> 1);
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Int17C;

impl CertRule for Int17C {
    fn rule_id(&self) -> &'static str {
        "INT17-C"
    }

    fn description(&self) -> &'static str {
        "Define integer constants in an implementation-independent manner"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "INT17-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.traverse(node, source, &mut violations);
        violations
    }
}

impl Int17C {
    /// Recursively traverse the AST looking for implementation-dependent hex constants
    fn traverse(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check if this is a number literal
        if node.kind() == "number_literal" {
            let literal_text = get_node_text(node, source);

            // Check if it's a problematic hex constant
            if self.is_implementation_dependent_constant(literal_text) {
                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    message: format!(
                        "Hex constant '{}' assumes specific bit-width and is implementation-dependent. \
                         Use -1 for all bits set, or shift expressions like ~(ULONG_MAX >> 1) for MSB.",
                        literal_text
                    ),
                    severity: self.severity(),
                    line: node.start_position().row + 1,
                    column: node.start_position().column + 1,
                    file_path: String::new(),
                    suggestion: Some(
                        "Use -1 for all bits set (unsigned), or macros from <limits.h> for portable values"
                            .to_string(),
                    ),
                    requires_manual_review: None,
                });
            }
        }

        // Recurse through all children
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.traverse(&child, source, violations);
            }
        }
    }

    /// Check if a hex constant is implementation-dependent
    fn is_implementation_dependent_constant(&self, literal: &str) -> bool {
        // Normalize to lowercase and remove common suffixes
        let normalized = literal
            .to_lowercase()
            .trim_end_matches('u')
            .trim_end_matches('l')
            .trim_end_matches("ul")
            .trim_end_matches("lu")
            .trim_end_matches("ll")
            .trim_end_matches("ull")
            .trim_end_matches("llu")
            .to_string();

        // Check for hexadecimal constants that assume specific bit-widths
        if !normalized.starts_with("0x") && !normalized.starts_with("0X") {
            return false;
        }

        // Common implementation-dependent patterns:

        // 32-bit patterns
        if normalized == "0xffffffff" {
            return true; // All bits set for 32-bit
        }
        if normalized == "0x80000000" {
            return true; // MSB for 32-bit
        }
        if normalized == "0x7fffffff" {
            return true; // Max positive for signed 32-bit
        }

        // 16-bit patterns
        if normalized == "0xffff" {
            return true; // All bits set for 16-bit
        }
        if normalized == "0x8000" {
            return true; // MSB for 16-bit
        }
        if normalized == "0x7fff" {
            return true; // Max positive for signed 16-bit
        }

        // 8-bit patterns (less common but still problematic in some contexts)
        if normalized == "0xff" {
            return true; // All bits set for 8-bit
        }
        if normalized == "0x80" {
            return true; // MSB for 8-bit
        }
        if normalized == "0x7f" {
            return true; // Max positive for signed 8-bit
        }

        // 64-bit patterns
        if normalized == "0xffffffffffffffff" {
            return true; // All bits set for 64-bit
        }
        if normalized == "0x8000000000000000" {
            return true; // MSB for 64-bit
        }

        // Additional patterns: consecutive F's or patterns that suggest bit-width assumptions
        // Check for patterns like 0xFFFF0000, 0xFF00, etc. that combine masks
        if normalized.len() >= 4 {
            // At least "0x" + 2 hex digits
            let hex_part = &normalized[2..];

            // Check if it's all F's (any length) - suggests all-bits-set assumption
            if hex_part.chars().all(|c| c == 'f') && hex_part.len().is_multiple_of(2) {
                // Even number of F's suggests byte-aligned mask
                return true;
            }

            // Check for MSB patterns: 8 followed by zeros (like 0x800000, 0x8000000000)
            if hex_part.starts_with('8') && hex_part.chars().skip(1).all(|c| c == '0') {
                return true;
            }

            // Check for max positive patterns: 7 followed by F's
            if hex_part.starts_with('7') && hex_part.chars().skip(1).all(|c| c == 'f') {
                return true;
            }
        }

        false
    }
}

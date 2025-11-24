//! PRE13-C: Use the Standard predefined macros to test for versions and features
//!
//! This rule ensures that standard predefined macros are tested with `defined()` before
//! their values are used in preprocessor conditionals. Testing macro values without first
//! confirming they are defined can cause undefined behavior on non-conforming implementations.
//!
//! ## Non-compliant example:
//!
//! ```c
//! #if (__STDC__ == 1)  // Wrong: tests value without checking if defined
//!     // ...
//! #endif
//!
//! #if __STDC_VERSION__ >= 201112L  // Wrong: tests value without defined() check
//!     // ...
//! #endif
//! ```
//!
//! ## Compliant solution:
//!
//! ```c
//! #if defined(__STDC__)
//!     #if (__STDC__ == 1)  // OK: first checked if defined
//!         // ...
//!     #endif
//! #endif
//!
//! #if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)  // OK: uses defined()
//!     // ...
//! #endif
//! ```

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Pre13C;

impl Pre13C {
    pub fn new() -> Self {
        Self
    }

    /// Standard predefined macros that should be tested with defined()
    fn is_standard_macro(&self, name: &str) -> bool {
        matches!(
            name,
            "__STDC__"
                | "__STDC_VERSION__"
                | "__STDC_HOSTED__"
                | "__STDC_MB_MIGHT_NEQ_WC__"
                | "__STDC_UTF_16__"
                | "__STDC_UTF_32__"
                | "__STDC_ANALYZABLE__"
                | "__STDC_IEC_559__"
                | "__STDC_IEC_559_COMPLEX__"
                | "__STDC_ISO_10646__"
                | "__STDC_LIB_EXT1__"
                | "__STDC_NO_ATOMICS__"
                | "__STDC_NO_COMPLEX__"
                | "__STDC_NO_THREADS__"
                | "__STDC_NO_VLA__"
                | "__DATE__"
                | "__FILE__"
                | "__LINE__"
                | "__TIME__"
        )
    }

    /// Check if a preprocessor conditional uses standard macros without defined()
    fn check_preprocessor_conditional(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Check both #if and #elif directives
        let is_conditional = node.kind() == "preproc_if" || node.kind() == "preproc_elif";
        if !is_conditional {
            return;
        }

        // Get the condition node
        let condition = match node.child_by_field_name("condition") {
            Some(c) => c,
            None => return,
        };

        // Get the condition text
        let condition_text = get_node_text(&condition, source);

        // Check if condition uses standard macros directly without defined()
        for macro_name in [
            "__STDC__",
            "__STDC_VERSION__",
            "__STDC_HOSTED__",
            "__STDC_MB_MIGHT_NEQ_WC__",
            "__STDC_UTF_16__",
            "__STDC_UTF_32__",
            "__STDC_ANALYZABLE__",
            "__STDC_IEC_559__",
            "__STDC_IEC_559_COMPLEX__",
            "__STDC_ISO_10646__",
            "__STDC_LIB_EXT1__",
            "__STDC_NO_ATOMICS__",
            "__STDC_NO_COMPLEX__",
            "__STDC_NO_THREADS__",
            "__STDC_NO_VLA__",
        ] {
            // Check if the macro is used in the condition
            if condition_text.contains(macro_name) {
                // Check if it's used with defined()
                let defined_pattern = format!("defined({})", macro_name);
                let defined_pattern_spaces = format!("defined ({})", macro_name);

                if !condition_text.contains(&defined_pattern)
                    && !condition_text.contains(&defined_pattern_spaces)
                {
                    // Check if the macro appears directly (not in defined())
                    // This is a violation: using macro value without checking if it's defined

                    violations.push(RuleViolation {
                        rule_id: self.rule_id().to_string(),
                        severity: self.severity(),
                        message: format!(
                            "Standard macro '{}' is used in preprocessor conditional without first checking if it is defined. Use 'defined({})' before testing its value.",
                            macro_name, macro_name
                        ),
                        file_path: String::new(),
                        line: node.start_position().row + 1,
                        column: node.start_position().column + 1,
                        suggestion: Some(format!(
                            "Wrap the condition with 'defined({})' check: '#if defined({}) && ({})'",
                            macro_name, macro_name, condition_text
                        )),
                        ..Default::default()
                    });

                    // Only report once per condition, even if multiple macros are used incorrectly
                    return;
                }
            }
        }
    }
}

impl CertRule for Pre13C {
    fn rule_id(&self) -> &'static str {
        "PRE13-C"
    }

    fn description(&self) -> &'static str {
        "Use the Standard predefined macros to test for versions and features"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "PRE13-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Pre13C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check this node
        self.check_preprocessor_conditional(node, source, violations);

        // Recursively check child nodes
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                self.check_node(&child, source, violations);
            }
        }
    }
}

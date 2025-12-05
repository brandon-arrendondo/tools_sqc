// CERT C Rule: DCL08-C
// Properly encode relationships in constant definitions
//
// This rule checks that:
// 1. When constants have mathematical relationships, they should be encoded
//   (e.g., OUT_STR_LEN = IN_STR_LEN + 2 instead of separate literals)
// 2. Constants should not encode false or impermanent relationships

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Dcl08C;

impl CertRule for Dcl08C {
    fn rule_id(&self) -> &'static str {
        "DCL08-C"
    }

    fn description(&self) -> &'static str {
        "Properly encode relationships in constant definitions"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "DCL08-C"
    }

    fn check(&self, root_node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // First pass: collect all enum constants across the entire file
        let mut all_enum_constants = std::collections::HashMap::new();
        self.collect_all_enum_constants(root_node, source, &mut all_enum_constants);

        // Second pass: check each enum for violations
        let mut cursor = root_node.walk();
        self.check_node(
            root_node,
            source,
            &mut violations,
            &mut cursor,
            &all_enum_constants,
        );

        violations
    }
}

impl Dcl08C {
    /// Collect all enum constant names across the entire file
    fn collect_all_enum_constants(
        &self,
        node: &Node,
        source: &str,
        constants: &mut std::collections::HashMap<String, usize>,
    ) {
        if node.kind() == "enum_specifier" {
            if let Some(body) = node.child_by_field_name("body") {
                let mut cursor = body.walk();
                if cursor.goto_first_child() {
                    loop {
                        let child = cursor.node();
                        if child.kind() == "enumerator" {
                            if let Some(name_node) = child.child_by_field_name("name") {
                                let name = get_node_text(&name_node, source);
                                let line = name_node.start_position().row + 1;
                                constants.insert(name.to_string(), line);
                            }
                        }
                        if !cursor.goto_next_sibling() {
                            break;
                        }
                    }
                }
            }
        }

        // Recurse into children
        let mut cursor = node.walk();
        if cursor.goto_first_child() {
            loop {
                let child = cursor.node();
                self.collect_all_enum_constants(&child, source, constants);
                if !cursor.goto_next_sibling() {
                    break;
                }
            }
        }
    }

    fn check_node(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        cursor: &mut tree_sitter::TreeCursor,
        all_constants: &std::collections::HashMap<String, usize>,
    ) {
        // Check if this is an enum declaration
        if node.kind() == "enum_specifier" {
            self.check_enum_specifier(node, source, violations, all_constants);
        }

        // Recursively check child nodes
        if cursor.goto_first_child() {
            loop {
                let child = cursor.node();
                self.check_node(&child, source, violations, cursor, all_constants);

                if !cursor.goto_next_sibling() {
                    break;
                }
            }
            cursor.goto_parent();
        }
    }

    fn check_enum_specifier(
        &self,
        node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
        all_constants: &std::collections::HashMap<String, usize>,
    ) {
        // Get the enum body
        let body = match node.child_by_field_name("body") {
            Some(b) => b,
            None => return,
        };

        // Collect all enumerator definitions
        let mut enumerators = Vec::new();
        let mut cursor = body.walk();

        if cursor.goto_first_child() {
            loop {
                let child = cursor.node();
                if child.kind() == "enumerator" {
                    enumerators.push(child);
                }

                if !cursor.goto_next_sibling() {
                    break;
                }
            }
        }

        // Need at least 1 enumerator to check
        if enumerators.is_empty() {
            return;
        }

        // Check for potential missing relationships (same enum, literal values only)
        self.check_missing_relationships(&enumerators, source, violations, all_constants);
    }

    fn check_missing_relationships(
        &self,
        enumerators: &[Node],
        source: &str,
        violations: &mut Vec<RuleViolation>,
        all_constants: &std::collections::HashMap<String, usize>,
    ) {
        // Collect enumerator info for THIS enum: name and value (if literal)
        let mut enum_values = Vec::new();
        let mut this_enum_names = std::collections::HashSet::new();

        for enumerator in enumerators {
            if let Some(name_node) = enumerator.child_by_field_name("name") {
                let name = get_node_text(&name_node, source);
                this_enum_names.insert(name.to_string());

                // Check if there's a value
                if let Some(value_node) = enumerator.child_by_field_name("value") {
                    let value_text = get_node_text(&value_node, source);

                    // Check if it's a simple numeric literal
                    if let Ok(value) = value_text.parse::<i64>() {
                        enum_values.push((name, value, name_node.start_position()));
                    } else if value_text.contains('+') || value_text.contains('-') {
                        // This is a reference with arithmetic
                        // Check if it references a constant from THIS enum (that's OK)
                        // or a constant from a DIFFERENT scope (that's potentially misleading)
                        let references_other_enum = all_constants.iter().any(|(enum_name, _)| {
                            value_text.contains(enum_name.as_str())
                                && !this_enum_names.contains(enum_name)
                        });

                        if references_other_enum {
                            // References a constant from a different enum - potentially misleading
                            let position = name_node.start_position();
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                message: format!(
                                    "Constant '{}' encodes a potentially misleading relationship: {}. \
                                    If no permanent relationship exists, define constants independently.",
                                    name, value_text
                                ),
                                severity: self.severity(),
                                line: position.row + 1,
                                column: position.column + 1,
                                file_path: String::new(),
                                suggestion: Some(format!(
                                    "Either define '{}' with its literal value or ensure the relationship with {} is permanent and accurate",
                                    name, value_text
                                )),
                                requires_manual_review: Some(true),
                            });
                        }
                    }
                }
            }
        }

        // Check if any pair of values might have a relationship
        // For noncompliant_1: IN_STR_LEN=18, OUT_STR_LEN=20 (20 = 18+2)
        if enum_values.len() >= 2 {
            // Check all pairs for simple arithmetic relationships
            for i in 0..enum_values.len() {
                for j in (i + 1)..enum_values.len() {
                    let (name1, val1, _) = &enum_values[i];
                    let (name2, val2, pos2) = &enum_values[j];

                    // Check if val2 could be expressed as val1 + offset
                    if val2 > val1 {
                        let offset = val2 - val1;
                        // Flag small offsets (like +2, +3) as potential relationships
                        if offset > 0 && offset <= 10 {
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                message: format!(
                                    "Constants '{}' and '{}' may have a relationship ({} = {} + {}). \
                                    Consider encoding: {} = {} + {}",
                                    name1, name2, val2, val1, offset, name2, name1, offset
                                ),
                                severity: self.severity(),
                                line: pos2.row + 1,
                                column: pos2.column + 1,
                                file_path: String::new(),
                                suggestion: Some(format!(
                                    "enum {{ {} = {}, {} = {} + {} }};",
                                    name1, val1, name2, name1, offset
                                )),
                                requires_manual_review: Some(true),
                            });
                        }
                    }
                }
            }
        }
    }

    fn contains_reference_with_arithmetic(
        &self,
        value_text: &str,
        known_values: &[(&str, i64, tree_sitter::Point)],
    ) -> bool {
        // Check if the value contains a reference to another enum constant plus arithmetic
        for (name, _, _) in known_values {
            if value_text.contains(name) && (value_text.contains('+') || value_text.contains('-')) {
                return true;
            }
        }
        false
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_rule_id() {
        let rule = Dcl08C;
        assert_eq!(rule.rule_id(), "DCL08-C");
    }

    #[test]
    fn test_severity() {
        let rule = Dcl08C;
        assert_eq!(rule.severity(), Severity::Low);
    }
}

use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use tree_sitter::Node;

pub struct Pos49C;

impl CertRule for Pos49C {
    fn rule_id(&self) -> &'static str {
        "POS49-C"
    }

    fn description(&self) -> &'static str {
        "TODO: Implement POS49-C"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        self.rule_id()
    }

    fn check(&self, _root: &Node, _source: &str) -> Vec<RuleViolation> {
        Vec::new()
    }
}

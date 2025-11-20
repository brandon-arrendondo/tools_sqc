use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use tree_sitter::Node;

pub struct Mem36C;

impl CertRule for Mem36C {
    fn rule_id(&self) -> &'static str {
        "MEM36-C"
    }

    fn description(&self) -> &'static str {
        "TODO: Implement MEM36-C"
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

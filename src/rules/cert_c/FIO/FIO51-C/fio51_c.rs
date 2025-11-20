use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use tree_sitter::Node;

pub struct Fio51C;

impl CertRule for Fio51C {
    fn rule_id(&self) -> &'static str {
        "FIO51-C"
    }

    fn description(&self) -> &'static str {
        "TODO: Implement FIO51-C"
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

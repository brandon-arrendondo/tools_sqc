// SPDX-License-Identifier: MIT
// Copyright (c) 2024 Ryan Urchick

use tree_sitter::Node;
use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};

pub struct Fio19C;

impl CertRule for Fio19C {
    fn rule_id(&self) -> &'static str { "FIO19-C" }
    fn description(&self) -> &'static str { "TODO" }
    fn severity(&self) -> Severity { Severity::Medium }
    fn category(&self) -> RuleCategory { RuleCategory::Rule }
    fn cert_id(&self) -> &'static str { "FIO19-C" }
    fn check(&self, _node: &Node, _source: &str) -> Vec<RuleViolation> {
        // TODO: Implement
        Vec::new()
    }
}

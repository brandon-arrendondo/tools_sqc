// SPDX-License-Identifier: MIT
// Copyright (c) 2024 Ryan Urchick

use tree_sitter::Node;
use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};

pub struct Pre06C;

impl CertRule for Pre06C {
    fn rule_id(&self) -> &'static str { "PRE06-C" }
    fn description(&self) -> &'static str { "TODO" }
    fn severity(&self) -> Severity { Severity::Medium }
    fn category(&self) -> RuleCategory { RuleCategory::Rule }
    fn cert_id(&self) -> &'static str { "PRE06-C" }
    fn check(&self, _node: &Node, _source: &str) -> Vec<RuleViolation> {
        // TODO: Implement
        Vec::new()
    }
}

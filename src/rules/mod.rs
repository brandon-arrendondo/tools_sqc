use anyhow::Result;
use tree_sitter::Node;

pub mod arr30_c;
pub mod arr32_c;
pub mod str31_c;

pub trait CertRule {
    fn rule_id(&self) -> &'static str;
    fn description(&self) -> &'static str;
    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation>;
}

#[derive(Debug, Clone)]
pub struct RuleViolation {
    pub rule_id: String,
    pub severity: crate::manifest::Severity,
    pub message: String,
    pub file_path: String,
    pub line: usize,
    pub column: usize,
    pub suggestion: Option<String>,
}

pub struct RuleRegistry {
    rules: Vec<Box<dyn CertRule>>,
}

impl RuleRegistry {
    pub fn new() -> Self {
        let mut registry = Self {
            rules: Vec::new(),
        };

        registry.register(Box::new(arr30_c::Arr30C));
        registry.register(Box::new(arr32_c::Arr32C));
        registry.register(Box::new(str31_c::Str31C));

        registry
    }

    pub fn register(&mut self, rule: Box<dyn CertRule>) {
        self.rules.push(rule);
    }

    pub fn get_rule(&self, rule_id: &str) -> Option<&dyn CertRule> {
        self.rules.iter()
            .find(|rule| rule.rule_id() == rule_id)
            .map(|rule| rule.as_ref())
    }

    pub fn all_rules(&self) -> &[Box<dyn CertRule>] {
        &self.rules
    }
}

impl Default for RuleRegistry {
    fn default() -> Self {
        Self::new()
    }
}
mod cert_c;

use tree_sitter::Node;

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

impl Default for RuleRegistry {
    fn default() -> Self {
        Self::new()
    }
}

pub fn get_rule_description(registry: &RuleRegistry, rule_id: &str) -> String {
    if let Some(rule) = registry.get_rule(rule_id) {
        rule.description().to_string()
    } else {
        "Unknown rule".to_string()
    }
}
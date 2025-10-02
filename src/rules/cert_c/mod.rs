pub mod arr30_c;
pub mod arr32_c;
pub mod arr36_c;
pub mod arr37_c;
pub mod arr38_c;
pub mod arr39_c;
pub mod dcl00_c;
pub mod exp33_c;
pub mod int30_c;
pub mod mem30_c;
pub mod pre30_c;
pub mod pre31_c;
pub mod pre32_c;
pub mod str31_c;

use super::{CertRule, RuleRegistry};

impl RuleRegistry {
    pub fn new() -> Self {
        let mut registry = Self {
            rules: Vec::new(),
        };

        registry.register(Box::new(arr30_c::Arr30C));
        registry.register(Box::new(arr32_c::Arr32C));
        registry.register(Box::new(arr36_c::Arr36C));
        registry.register(Box::new(arr37_c::Arr37C));
        registry.register(Box::new(arr38_c::Arr38C));
        registry.register(Box::new(arr39_c::Arr39C));
        registry.register(Box::new(dcl00_c::Dcl00C));
        registry.register(Box::new(exp33_c::Exp33C));
        registry.register(Box::new(int30_c::Int30C));
        registry.register(Box::new(mem30_c::Mem30C));
        registry.register(Box::new(pre30_c::Pre30C));
        registry.register(Box::new(pre31_c::Pre31C));
        registry.register(Box::new(pre32_c::Pre32C));
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

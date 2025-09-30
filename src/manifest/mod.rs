use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::fs;
use anyhow::{Context, Result};

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct RuleManifest {
    pub metadata: ManifestMetadata,
    pub rules: HashMap<String, RuleConfig>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ManifestMetadata {
    pub name: String,
    pub version: String,
    pub description: Option<String>,
    pub cert_version: String,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct RuleConfig {
    pub enabled: bool,
    pub severity: Severity,
    pub description: String,
    pub category: RuleCategory,
    pub cert_id: String,
    pub parameters: Option<HashMap<String, String>>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum Severity {
    Low,
    Medium,
    High,
    Critical,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum RuleCategory {
    Rule,
    Recommendation,
}

impl RuleManifest {
    pub fn load(path: &str) -> Result<Self> {
        let content = fs::read_to_string(path)
            .with_context(|| format!("Failed to read manifest file: {}", path))?;

        let manifest: RuleManifest = toml::from_str(&content)
            .with_context(|| format!("Failed to parse manifest file: {}", path))?;

        Ok(manifest)
    }

    pub fn enabled_rules(&self) -> impl Iterator<Item = (&String, &RuleConfig)> {
        self.rules.iter().filter(|(_, config)| config.enabled)
    }

    pub fn get_rule(&self, rule_id: &str) -> Option<&RuleConfig> {
        self.rules.get(rule_id)
    }
}

impl Default for RuleManifest {
    fn default() -> Self {
        let mut rules = HashMap::new();

        // Example CERT C rules
        rules.insert("ARR30-C".to_string(), RuleConfig {
            enabled: true,
            severity: Severity::High,
            description: "Do not form or use out-of-bounds pointers or array subscripts".to_string(),
            category: RuleCategory::Rule,
            cert_id: "ARR30-C".to_string(),
            parameters: None,
        });

        rules.insert("STR31-C".to_string(), RuleConfig {
            enabled: true,
            severity: Severity::Medium,
            description: "Guarantee that storage for strings has sufficient space for character data and the null terminator".to_string(),
            category: RuleCategory::Rule,
            cert_id: "STR31-C".to_string(),
            parameters: None,
        });

        Self {
            metadata: ManifestMetadata {
                name: "Default CERT C Rules".to_string(),
                version: "1.0.0".to_string(),
                description: Some("Default set of CERT C rules and recommendations".to_string()),
                cert_version: "2016".to_string(),
            },
            rules,
        }
    }
}
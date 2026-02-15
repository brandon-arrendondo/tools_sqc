mod cert_c;

use crate::analyze::context::ProjectContext;
use tree_sitter::Node;

pub trait CertRule {
    fn rule_id(&self) -> &'static str;
    fn description(&self) -> &'static str;
    fn severity(&self) -> crate::manifest::Severity;
    fn category(&self) -> crate::manifest::RuleCategory;
    fn cert_id(&self) -> &'static str;
    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation>;

    /// Inject cross-file context gathered by the pre-scan phase.
    /// Default is a no-op; only rules that need cross-file data override this.
    fn set_project_context(&self, _context: &ProjectContext) {}
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
    /// Indicates if this violation requires manual investigation by the user.
    /// Used for ambiguous cases where the tool cannot definitively determine if it's a violation.
    /// `None` or `Some(false)` means it's a definite violation.
    /// `Some(true)` means it requires manual review.
    #[doc(hidden)]
    pub requires_manual_review: Option<bool>,
}

impl Default for RuleViolation {
    fn default() -> Self {
        Self {
            rule_id: String::new(),
            severity: crate::manifest::Severity::Low,
            message: String::new(),
            file_path: String::new(),
            line: 0,
            column: 0,
            suggestion: None,
            requires_manual_review: None,
        }
    }
}

impl RuleViolation {
    /// Returns true if this violation requires manual review
    pub fn needs_manual_review(&self) -> bool {
        self.requires_manual_review.unwrap_or(false)
    }
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

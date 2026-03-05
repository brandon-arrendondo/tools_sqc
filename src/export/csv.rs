pub use crate::analyze::get_code_snippet;
use crate::prelude::*;
use crate::rules::{get_rule_description, RuleRegistry, RuleViolation};

pub fn export_all_violations_to_csv(
    violations: &[RuleViolation],
    csv_path: &str,
    _manifest: &RuleManifest,
) -> Result<()> {
    use csv::Writer;

    let registry = RuleRegistry::new();
    let mut writer = Writer::from_path(csv_path)?;

    // Write CSV headers
    writer.write_record([
        "Title",
        "Description",
        "Work Item Type",
        "State",
        "Severity",
        "Priority",
    ])?;

    // Write all violations
    for violation in violations {
        let file_hash = calculate_file_hash(&violation.file_path)?;

        let title = format!(
            "{}:{}:{} version:{}",
            violation.rule_id, violation.file_path, violation.line, file_hash
        );

        let code_snippet = get_code_snippet(&violation.file_path, violation.line)?;
        let rule_description = get_rule_description(&registry, &violation.rule_id);
        let description = format!(
            "{} - {}: {}",
            violation.rule_id, rule_description, code_snippet
        );

        writer.write_record([&title, &description, "Bug", "Proposed", "1 - Critical", "1"])?;
    }

    writer.flush()?;
    Ok(())
}

use crate::rules::RuleViolation;
use crate::utility::files::get_relative_path;

use anyhow::Result;
use serde_json;
use std::fs::File;
use std::io::BufWriter;

pub fn export_all_violations_to_json(
    violations: &[RuleViolation],
    json_path: &str,
    base_path: &str,
) -> Result<()> {
    let output: Vec<serde_json::Value> = violations
        .iter()
        .map(|v| {
            let relative_path = get_relative_path(&v.file_path, base_path);
            serde_json::json!({
                "rule_id": v.rule_id,
                "severity": v.severity,
                "message": v.message,
                "file": relative_path,
                "line": v.line,
                "column": v.column,
                "suggestion": v.suggestion,
            })
        })
        .collect();

    let file = File::create(json_path)?;
    let writer = BufWriter::new(file);
    serde_json::to_writer_pretty(writer, &output)?;

    Ok(())
}

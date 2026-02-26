use crate::manifest::Severity;
use crate::rules::RuleViolation;
use crate::utility::files::get_relative_path;

use anyhow::Result;
use std::collections::BTreeMap;
use std::fs::File;
use std::io::BufWriter;

fn severity_to_sarif_level(severity: &Severity) -> &'static str {
    match severity {
        Severity::Critical | Severity::High => "error",
        Severity::Medium => "warning",
        Severity::Low => "note",
    }
}

pub fn export_all_violations_to_sarif(
    violations: &[RuleViolation],
    sarif_path: &str,
    base_path: &str,
) -> Result<()> {
    // Collect unique rules for the rules array
    let mut rules_map: BTreeMap<String, &RuleViolation> = BTreeMap::new();
    for v in violations {
        rules_map.entry(v.rule_id.clone()).or_insert(v);
    }

    // Build rule index for result references
    let rule_index: BTreeMap<&str, usize> = rules_map
        .keys()
        .enumerate()
        .map(|(i, k)| (k.as_str(), i))
        .collect();

    let rules_array: Vec<serde_json::Value> = rules_map
        .iter()
        .map(|(rule_id, v)| {
            serde_json::json!({
                "id": rule_id,
                "shortDescription": {
                    "text": v.message
                },
                "defaultConfiguration": {
                    "level": severity_to_sarif_level(&v.severity)
                },
                "helpUri": format!("https://wiki.sei.cmu.edu/confluence/display/c/{}", rule_id)
            })
        })
        .collect();

    let results_array: Vec<serde_json::Value> = violations
        .iter()
        .map(|v| {
            let relative_path = get_relative_path(&v.file_path, base_path);
            let idx = rule_index.get(v.rule_id.as_str()).copied().unwrap_or(0);
            let mut result = serde_json::json!({
                "ruleId": v.rule_id,
                "ruleIndex": idx,
                "level": severity_to_sarif_level(&v.severity),
                "message": {
                    "text": v.message
                },
                "locations": [{
                    "physicalLocation": {
                        "artifactLocation": {
                            "uri": relative_path,
                            "uriBaseId": "%SRCROOT%"
                        },
                        "region": {
                            "startLine": v.line,
                            "startColumn": v.column
                        }
                    }
                }]
            });
            if let Some(suggestion) = &v.suggestion {
                result["fixes"] = serde_json::json!([{
                    "description": {
                        "text": suggestion
                    }
                }]);
            }
            result
        })
        .collect();

    let sarif = serde_json::json!({
        "$schema": "https://raw.githubusercontent.com/oasis-tcs/sarif-spec/main/sarif-2.1/schema/sarif-schema-2.1.0.json",
        "version": "2.1.0",
        "runs": [{
            "tool": {
                "driver": {
                    "name": "SqC",
                    "version": env!("CARGO_PKG_VERSION"),
                    "informationUri": "https://github.com/your-org/sqc",
                    "rules": rules_array
                }
            },
            "results": results_array
        }]
    });

    let file = File::create(sarif_path)?;
    let writer = BufWriter::new(file);
    serde_json::to_writer_pretty(writer, &sarif)?;

    Ok(())
}

use clap::{Arg, Command};
use anyhow::Result;
use std::fs;
use sha2::{Sha256, Digest};

mod rules;
mod manifest;
mod ui;
mod git;
mod parser;

use manifest::RuleManifest;
use ui::TerminalUI;
use git::ProjectSource;
use rules::{RuleRegistry, RuleViolation};
use parser::CParser;

fn main() -> Result<()> {
    let matches = Command::new("sqc")
        .about("Software Code Quality - CERT C compliance checker")
        .version("0.1.0")
        .arg(
            Arg::new("path")
                .help("Path to the directory or git repository to analyze")
                .value_name("PATH")
                .default_value(".")
                .index(1),
        )
        .arg(
            Arg::new("manifest")
                .long("manifest")
                .short('m')
                .help("Path to the rules manifest file")
                .value_name("FILE")
                .default_value("sqc-rules.toml"),
        )
        .arg(
            Arg::new("interactive")
                .long("interactive")
                .short('i')
                .help("Run in interactive terminal UI mode")
                .action(clap::ArgAction::SetTrue),
        )
        .arg(
            Arg::new("export")
                .long("export")
                .short('e')
                .help("Export all violations to file (CSV or Excel based on extension, non-interactive mode)")
                .value_name("FILE"),
        )
        .get_matches();

    let path = matches.get_one::<String>("path").unwrap();
    let manifest_path = matches.get_one::<String>("manifest").unwrap();
    let interactive = matches.get_flag("interactive");
    let export_file = matches.get_one::<String>("export");

    // Verify the path and determine source type
    let project_source = ProjectSource::open(path)?;
    println!("Detected {} at: {}", project_source.source_type(), path);

    let manifest = RuleManifest::load(manifest_path)?;

    if interactive {
        let mut ui = TerminalUI::new(path, manifest)?;
        ui.run()?;
    } else {
        println!("Analyzing {} at: {}", project_source.source_type(), path);
        println!("Using manifest: {}", manifest_path);

        // Perform analysis
        let violations = analyze_project(&project_source, &manifest)?;
        println!("Found {} violations", violations.len());

        // Export to file if requested
        if let Some(export_path) = export_file {
            export_all_violations(&violations, export_path, path, &manifest)?;
            println!("Exported violations to: {}", export_path);
        }
    }

    Ok(())
}

fn analyze_project(project_source: &ProjectSource, manifest: &RuleManifest) -> Result<Vec<RuleViolation>> {
    let mut violations = Vec::new();
    let registry = RuleRegistry::new();
    let c_files = project_source.get_c_files()?;
    let mut parser = CParser::new()?;

    for file_path in c_files {
        if let Ok((tree, source)) = parser.parse_file(&file_path) {
            let root_node = tree.root_node();

            for (rule_id, rule_config) in manifest.enabled_rules() {
                if let Some(rule) = registry.get_rule(rule_id) {
                    let mut file_violations = rule.check(&root_node, &source);
                    for violation in &mut file_violations {
                        violation.file_path = file_path.clone();
                        violation.severity = rule_config.severity.clone();
                    }
                    violations.extend(file_violations);
                }
            }
        }
    }

    Ok(violations)
}

fn export_all_violations(violations: &[RuleViolation], export_path: &str, base_path: &str, _manifest: &RuleManifest) -> Result<()> {
    use std::path::Path;

    let path = Path::new(export_path);
    if let Some(extension) = path.extension() {
        match extension.to_str() {
            Some("xlsx") => export_all_violations_to_excel(violations, export_path, base_path, _manifest),
            Some("csv") => export_all_violations_to_csv(violations, export_path, base_path, _manifest),
            _ => {
                // Default to Excel for unknown extensions
                export_all_violations_to_excel(violations, export_path, base_path, _manifest)
            }
        }
    } else {
        // No extension, default to Excel
        export_all_violations_to_excel(violations, export_path, base_path, _manifest)
    }
}

fn export_all_violations_to_csv(violations: &[RuleViolation], csv_path: &str, base_path: &str, _manifest: &RuleManifest) -> Result<()> {
    use csv::Writer;

    let registry = RuleRegistry::new();
    let mut writer = Writer::from_path(csv_path)?;

    // Write CSV headers
    writer.write_record(&[
        "Title",
        "Description",
        "Work Item Type",
        "State",
        "Severity",
        "Priority"
    ])?;

    // Write all violations
    for violation in violations {
        let file_hash = calculate_file_hash(&violation.file_path)?;
        let relative_path = get_relative_path(&violation.file_path, base_path);

        let title = format!("{}:{}:{} version:{}",
            violation.rule_id, relative_path, violation.line, file_hash);

        let code_snippet = get_code_snippet(&violation.file_path, violation.line)?;
        let rule_description = get_rule_description(&registry, &violation.rule_id);
        let description = format!("{} - {}: {}",
            violation.rule_id, rule_description, code_snippet);

        writer.write_record(&[
            &title,
            &description,
            "Bug",
            "Proposed",
            "1 - Critical",
            "1"
        ])?;
    }

    writer.flush()?;
    Ok(())
}

fn export_all_violations_to_excel(violations: &[RuleViolation], excel_path: &str, base_path: &str, _manifest: &RuleManifest) -> Result<()> {
    use rust_xlsxwriter::{Workbook, Format, Color as XlsxColor};

    let registry = RuleRegistry::new();
    let mut workbook = Workbook::new();
    let worksheet = workbook.add_worksheet();

    // Create header format
    let header_format = Format::new()
        .set_bold()
        .set_background_color(XlsxColor::RGB(0xD9D9D9));

    // Write headers
    let headers = [
        "Title",
        "Description",
        "Work Item Type",
        "State",
        "Severity",
        "Priority"
    ];

    for (col, header) in headers.iter().enumerate() {
        worksheet.write_string_with_format(0, col as u16, *header, &header_format)?;
    }

    // Write all violations
    let mut row = 1;
    for violation in violations {
        let file_hash = calculate_file_hash(&violation.file_path)?;
        let relative_path = get_relative_path(&violation.file_path, base_path);

        let title = format!("{}:{}:{} version:{}",
            violation.rule_id, relative_path, violation.line, file_hash);

        let code_snippet = get_code_snippet(&violation.file_path, violation.line)?;
        let rule_description = get_rule_description(&registry, &violation.rule_id);
        let description = format!("{} - {}: {}",
            violation.rule_id, rule_description, code_snippet);

        worksheet.write_string(row, 0, &title)?;
        worksheet.write_string(row, 1, &description)?;
        worksheet.write_string(row, 2, "Bug")?;
        worksheet.write_string(row, 3, "Proposed")?;
        worksheet.write_string(row, 4, "1 - Critical")?;
        worksheet.write_string(row, 5, "1")?;

        row += 1;
    }

    // Auto-fit columns
    worksheet.autofit();

    workbook.save(excel_path)?;
    Ok(())
}

fn calculate_file_hash(file_path: &str) -> Result<String> {
    let content = fs::read(file_path)?;
    let mut hasher = Sha256::new();
    hasher.update(&content);
    let result = hasher.finalize();
    Ok(format!("{:x}", result)[..8].to_string()) // First 8 chars of hash
}

fn get_code_snippet(file_path: &str, line_number: usize) -> Result<String> {
    let content = fs::read_to_string(file_path)?;
    let lines: Vec<&str> = content.lines().collect();

    if line_number > 0 && line_number <= lines.len() {
        let line = lines[line_number - 1].trim();
        Ok(line.to_string())
    } else {
        Ok("(line not found)".to_string())
    }
}

fn get_rule_description(registry: &RuleRegistry, rule_id: &str) -> String {
    if let Some(rule) = registry.get_rule(rule_id) {
        rule.description().to_string()
    } else {
        "Unknown rule".to_string()
    }
}

fn get_relative_path(file_path: &str, base_path: &str) -> String {
    use std::path::Path;

    // Get the canonical paths to handle . and .. properly
    let base_path_obj = Path::new(base_path);
    let file_path_obj = Path::new(file_path);

    // Try to get the relative path from base to file
    if let Ok(relative) = file_path_obj.strip_prefix(base_path_obj) {
        relative.to_string_lossy().to_string()
    } else {
        // Fall back to just the filename if we can't get relative path
        file_path.split('/').last().unwrap_or(file_path).to_string()
    }
}

use clap::{Arg, Command};
use anyhow::Result;
use std::fs;
use sha2::{Sha256, Digest};

mod rules;
mod manifest;
mod ui;
mod git;
mod parser;
mod suppression;

use manifest::RuleManifest;
use ui::TerminalUI;
use git::ProjectSource;
use rules::{RuleRegistry, RuleViolation};
use parser::CParser;
use suppression::SuppressionManager;

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
        .arg(
            Arg::new("generate_suppression")
                .long("generate-suppression")
                .help("Generate suppression comment for a specific file:line:rule")
                .value_name("FILE:LINE:RULE")
                .conflicts_with("interactive")
                .conflicts_with("export"),
        )
        .get_matches();

    let path = matches.get_one::<String>("path").unwrap();
    let manifest_path = matches.get_one::<String>("manifest").unwrap();
    let interactive = matches.get_flag("interactive");
    let export_file = matches.get_one::<String>("export");
    let generate_suppression = matches.get_one::<String>("generate_suppression");

    // Verify the path and determine source type
    let project_source = ProjectSource::open(path)?;
    println!("Detected {} at: {}", project_source.source_type(), path);

    let manifest = RuleManifest::load(manifest_path)?;

    // Handle suppression generation
    if let Some(gen_spec) = generate_suppression {
        return handle_generate_suppression(gen_spec);
    }

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
    let mut suppression_manager = SuppressionManager::new();

    for file_path in c_files {
        if let Ok((tree, source)) = parser.parse_file(&file_path) {
            let root_node = tree.root_node();

            // Extract suppressions from the current file
            suppression_manager.extract_from_source(&file_path, &source);

            for (rule_id, rule_config) in manifest.enabled_rules() {
                if let Some(rule) = registry.get_rule(rule_id) {
                    let mut file_violations = rule.check(&root_node, &source);

                    // Filter out suppressed violations
                    file_violations.retain(|violation| {
                        if let Some(suppression) = suppression_manager.should_suppress(
                            &file_path,
                            rule_id,
                            violation.line,
                            &source
                        ) {
                            // Log that violation was suppressed (optional)
                            eprintln!("Suppressed {} at {}:{} - Justification: {}",
                                     rule_id, file_path, violation.line, suppression.justification);
                            false
                        } else {
                            true
                        }
                    });

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

fn handle_generate_suppression(spec: &str) -> Result<()> {
    // Parse the specification: FILE:LINE:RULE or FILE:LINE-LINE:RULE
    let parts: Vec<&str> = spec.splitn(3, ':').collect();
    if parts.len() != 3 {
        eprintln!("Error: Invalid format. Use FILE:LINE:RULE or FILE:LINE-LINE:RULE");
        eprintln!("Example: src/main.c:42:ARR30-C");
        eprintln!("Example: src/utils.c:15-18:MEM30-C");
        return Ok(());
    }

    let file_path = parts[0];
    let line_spec = parts[1];
    let rule_id = parts[2];

    // Read the source file
    let source = match fs::read_to_string(file_path) {
        Ok(content) => content,
        Err(e) => {
            eprintln!("Error: Cannot read file '{}': {}", file_path, e);
            return Ok(());
        }
    };

    // Parse line specification
    let (start_line, end_line) = if line_spec.contains('-') {
        let range_parts: Vec<&str> = line_spec.split('-').collect();
        if range_parts.len() != 2 {
            eprintln!("Error: Invalid line range format. Use LINE-LINE");
            return Ok(());
        }
        let start: usize = range_parts[0].parse().map_err(|_| {
            eprintln!("Error: Invalid start line number");
        }).unwrap_or(0);
        let end: usize = range_parts[1].parse().map_err(|_| {
            eprintln!("Error: Invalid end line number");
        }).unwrap_or(0);
        (start, end)
    } else {
        let line: usize = line_spec.parse().map_err(|_| {
            eprintln!("Error: Invalid line number");
        }).unwrap_or(0);
        (line, line)
    };

    if start_line == 0 || end_line == 0 || start_line > end_line {
        eprintln!("Error: Invalid line numbers");
        return Ok(());
    }

    // Extract the code lines
    let lines: Vec<&str> = source.lines().collect();
    if start_line > lines.len() || end_line > lines.len() {
        eprintln!("Error: Line numbers exceed file length (file has {} lines)", lines.len());
        return Ok(());
    }

    let code_lines = &lines[(start_line - 1)..end_line];
    let code = code_lines.join("\n");

    // Calculate the hash
    let hash = SuppressionManager::calculate_suppression_hash(rule_id, &code);

    // Generate the suppression comment
    println!("Generated suppression comment for {}:{}:{}", file_path, line_spec, rule_id);
    println!();
    println!("Code being suppressed:");
    for (i, line) in code_lines.iter().enumerate() {
        println!("{:4}: {}", start_line + i, line);
    }
    println!();

    if start_line == end_line {
        println!("Add this comment BEFORE line {}:", start_line);
        println!("// SQC-SUPPRESS: {} HASH:{} JUSTIFICATION: \"TODO: Add justification\"", rule_id, hash);
    } else {
        println!("Add this comment BEFORE line {}:", start_line);
        println!("// SQC-SUPPRESS: {} LINES:{} HASH:{} JUSTIFICATION: \"TODO: Add justification\"",
                rule_id, end_line - start_line + 1, hash);
    }
    println!();
    println!("Note: Replace 'TODO: Add justification' with an actual explanation of why this violation is acceptable.");

    Ok(())
}

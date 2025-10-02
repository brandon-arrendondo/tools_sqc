
pub mod suppression;

use super::manifest::RuleManifest;
use super::parser::CParser;
use super::rules::{RuleRegistry, RuleViolation};
use super::files::ProjectSource;
use suppression::SuppressionManager;

use anyhow::Result;
use std::fs;

pub fn analyze_project(project_source: &ProjectSource, manifest: &RuleManifest) -> Result<Vec<RuleViolation>> {
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


pub fn handle_generate_suppression(spec: &str) -> Result<()> {
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

pub fn get_code_snippet(file_path: &str, line_number: usize) -> Result<String> {
    let content = fs::read_to_string(file_path)?;
    let lines: Vec<&str> = content.lines().collect();

    if line_number > 0 && line_number <= lines.len() {
        let line = lines[line_number - 1].trim();
        Ok(line.to_string())
    } else {
        Ok("(line not found)".to_string())
    }
}



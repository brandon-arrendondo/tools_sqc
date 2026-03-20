pub mod cfg;
pub mod const_eval;
pub mod context;
pub mod dataflow;
pub mod function_summary;
pub mod null_state;
pub mod prescan;
pub mod suppression;
pub mod value_range;

use super::files::ProjectSource;
use super::manifest::RuleManifest;
use super::parser::CParser;
use super::progress::ProgressReporter;
use super::rules::{RuleRegistry, RuleViolation};
use suppression::SuppressionManager;

use anyhow::Result;
use std::collections::HashMap;
use std::fs;

/// A violation that was suppressed by an inline SQC-SUPPRESS comment.
pub struct SuppressedViolation {
    pub violation: RuleViolation,
    pub justification: String,
}

/// Results from project analysis, containing both active and suppressed violations.
pub struct AnalysisResults {
    pub violations: Vec<RuleViolation>,
    pub suppressed: Vec<SuppressedViolation>,
}

pub fn analyze_project(
    project_source: &ProjectSource,
    manifest: &RuleManifest,
    progress: Option<&dyn ProgressReporter>,
    directories: &[String],
    include_paths: &[String],
    diff_only: bool,
    suppress_file: Option<&str>,
) -> Result<AnalysisResults> {
    let mut violations = Vec::new();
    let mut suppressed = Vec::new();
    let registry = RuleRegistry::new();

    // Pre-compute whether any enabled rule needs VRA (used by prescan + per-file analysis)
    let needs_vra = manifest
        .enabled_rules()
        .any(|(rule_id, _)| registry.get_rule(rule_id).is_some_and(|r| r.needs_vra()));

    // Pre-scan additional directories for cross-file context
    let mut context = if directories.is_empty() {
        context::ProjectContext::new()
    } else {
        prescan::prescan_directories(directories, progress, needs_vra)?
    };

    // Resolve #include directives against include search paths
    if !include_paths.is_empty() {
        let c_files = if diff_only {
            project_source.get_modified_c_files()?
        } else {
            project_source.get_c_files()?
        };
        prescan::resolve_includes(&c_files, include_paths, &mut context, progress, needs_vra)?;
    }

    if context.has_cross_file_data() {
        for rule in registry.all_rules() {
            rule.set_project_context(&context);
        }
    }

    // Validate that all enabled rules are implemented
    let mut unimplemented_rules = Vec::new();
    for (rule_id, _) in manifest.enabled_rules() {
        if registry.get_rule(rule_id).is_none() {
            unimplemented_rules.push(rule_id.clone());
        }
    }

    if !unimplemented_rules.is_empty() {
        eprintln!("Warning: The following rules are enabled in manifest but not implemented:");
        for rule_id in &unimplemented_rules {
            eprintln!("  - {}", rule_id);
        }
        eprintln!("These rules will be skipped during analysis.\n");
    }

    let c_files = if diff_only {
        project_source.get_modified_c_files()?
    } else {
        project_source.get_c_files()?
    };
    let total_files = c_files.len();
    let mut parser = CParser::new()?;
    let mut suppression_manager = SuppressionManager::new();

    // Load TOML suppression file if provided or auto-detected
    let toml_path = suppress_file.map(String::from).or_else(|| {
        let auto_path =
            std::path::Path::new(project_source.get_root_path()).join(".sqc-suppress.toml");
        if auto_path.exists() {
            auto_path.to_str().map(String::from)
        } else {
            None
        }
    });
    if let Some(ref path) = toml_path {
        match suppression_manager.load_from_toml(path) {
            Ok(count) => {
                eprintln!("Loaded {} suppressions from {}", count, path);
            }
            Err(e) => {
                eprintln!("Warning: {}", e);
            }
        }
    }

    for (file_idx, file_path) in c_files.iter().enumerate() {
        // Check for cancellation before processing each file
        if let Some(reporter) = progress {
            if reporter.is_cancelled() {
                // Return partial results collected so far
                break;
            }
        }

        if let Ok((tree, source)) = parser.parse_file(file_path) {
            let root_node = tree.root_node();

            // Build CFGs for all function definitions in this file
            let mut function_cfgs: HashMap<usize, cfg::FunctionCfg> = HashMap::new();
            collect_function_cfgs(&root_node, &source, &mut function_cfgs);

            // Compute VRA if any enabled rule needs it
            let vra_results = compute_vra_if_needed(
                needs_vra,
                &function_cfgs,
                &root_node,
                &source,
                &context.function_summaries,
            );

            // Extract suppressions from the current file
            suppression_manager.extract_from_source(file_path, &source);

            for (rule_id, rule_config) in manifest.enabled_rules() {
                // Check cancellation between rules
                if let Some(reporter) = progress {
                    if reporter.is_cancelled() {
                        break;
                    }
                    // Report progress with current file and rule (full relative path)
                    reporter.report_file(file_idx + 1, total_files, file_path, rule_id);
                }

                // Check if rule is implemented
                if let Some(rule) = registry.get_rule(rule_id) {
                    // Skip rules that don't apply to this file type (e.g. header-only rules)
                    if !rule.applies_to_file(file_path) {
                        continue;
                    }
                    // Provide CFGs for flow-sensitive rules (e.g. EXP34-C)
                    rule.set_function_cfgs(&function_cfgs);
                    // Provide VRA results for integer-range-sensitive rules
                    if !vra_results.is_empty() {
                        rule.set_vra_results(&vra_results);
                    }
                    let mut file_violations = rule.check(&root_node, &source);

                    // Set file path and severity on all violations
                    for violation in &mut file_violations {
                        violation.file_path = file_path.clone();
                        violation.severity = rule_config
                            .severity
                            .clone()
                            .unwrap_or_else(|| rule.severity());
                    }

                    // Partition into active and suppressed violations
                    for violation in file_violations {
                        if let Some(suppression) = suppression_manager.should_suppress(
                            file_path,
                            rule_id,
                            violation.line,
                            &source,
                        ) {
                            suppressed.push(SuppressedViolation {
                                violation,
                                justification: suppression.justification.clone(),
                            });
                        } else {
                            violations.push(violation);
                        }
                    }
                }
                // Note: Unimplemented rules are already warned about at the start of analysis
            }
        }
    }

    // Report completion
    if let Some(reporter) = progress {
        reporter.report_complete(violations.len());
    }

    Ok(AnalysisResults {
        violations,
        suppressed,
    })
}

pub fn handle_generate_suppression(spec: &str) -> Result<()> {
    // Parse the specification: FILE:LINE:RULE
    let parts: Vec<&str> = spec.splitn(3, ':').collect();
    if parts.len() != 3 {
        eprintln!("Error: Invalid format. Use FILE:LINE:RULE");
        eprintln!("Example: src/main.c:42:ARR30-C");
        return Ok(());
    }

    let file_path = parts[0];
    let rule_id = parts[2];

    let line: usize = match parts[1].parse() {
        Ok(n) if n > 0 => n,
        _ => {
            eprintln!("Error: Invalid line number");
            return Ok(());
        }
    };

    // Read the source file
    let source = match fs::read_to_string(file_path) {
        Ok(content) => content,
        Err(e) => {
            eprintln!("Error: Cannot read file '{}': {}", file_path, e);
            return Ok(());
        }
    };

    let lines: Vec<&str> = source.lines().collect();
    if line > lines.len() {
        eprintln!(
            "Error: Line {} exceeds file length ({} lines)",
            line,
            lines.len()
        );
        return Ok(());
    }

    // Get the code line, stripping any existing SQC-SUPPRESS comment
    let raw_line = lines[line - 1];
    let code = if let Some(pos) = raw_line.find("// SQC-SUPPRESS") {
        &raw_line[..pos]
    } else if let Some(pos) = raw_line.find("/* SQC-SUPPRESS") {
        &raw_line[..pos]
    } else {
        raw_line
    };

    let hash = SuppressionManager::calculate_suppression_hash(rule_id, code);

    println!(
        "Generated suppression for {}:{}:{}",
        file_path, line, rule_id
    );
    println!();
    println!("Code:");
    println!("{:4}: {}", line, raw_line);
    println!();
    let filename = std::path::Path::new(file_path)
        .file_name()
        .and_then(|f| f.to_str())
        .unwrap_or(file_path);

    println!("Add on the line before, or inline:");
    println!(
        "// SQC-SUPPRESS: {} HASH:{} JUSTIFICATION: \"TODO: Add justification\"",
        rule_id, hash
    );
    println!();
    println!("Or add to .sqc-suppress.toml (for read-only codebases):");
    println!("[[suppression]]");
    println!("file = \"{}\"", filename);
    println!("rule = \"{}\"", rule_id);
    println!("hash = \"{}\"", hash);
    println!("justification = \"TODO: Add justification\"");

    Ok(())
}

/// Compute VRA for all functions if any enabled rule needs it.
fn compute_vra_if_needed(
    needs_vra: bool,
    function_cfgs: &HashMap<usize, cfg::FunctionCfg>,
    root_node: &tree_sitter::Node,
    source: &str,
    prescan_summaries: &HashMap<String, function_summary::FunctionSummary>,
) -> HashMap<usize, value_range::RangeAnalysisResult> {
    if !needs_vra || function_cfgs.is_empty() {
        return HashMap::new();
    }

    // Only compute macros and same-file summaries when VRA is actually needed
    let macros = const_eval::collect_macro_constants(root_node, source);
    let file_summaries = function_summary::compute_summaries(root_node, source, &macros, true);

    // Merge prescan (cross-file) summaries with same-file summaries by reference.
    // Only clone+extend if both sides are non-empty; otherwise use whichever is available.
    let merged;
    let summaries: &HashMap<String, function_summary::FunctionSummary> =
        if prescan_summaries.is_empty() {
            &file_summaries
        } else if file_summaries.is_empty() {
            prescan_summaries
        } else {
            merged = {
                let mut m = prescan_summaries.clone();
                m.extend(file_summaries);
                m
            };
            &merged
        };

    let mut results = HashMap::new();
    for (&start_byte, func_cfg) in function_cfgs {
        if let Some(func_node) = find_function_at_byte(root_node, start_byte) {
            results.insert(
                start_byte,
                value_range::analyze_value_ranges(func_cfg, &func_node, source, &macros, summaries),
            );
        }
    }
    results
}

/// Find the function_definition node at a given start byte.
fn find_function_at_byte<'a>(
    node: &tree_sitter::Node<'a>,
    start_byte: usize,
) -> Option<tree_sitter::Node<'a>> {
    if node.kind() == "function_definition" && node.start_byte() == start_byte {
        return Some(*node);
    }
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if let Some(found) = find_function_at_byte(&child, start_byte) {
                return Some(found);
            }
        }
    }
    None
}

/// Collect CFGs for all function_definition nodes in the AST.
/// Keyed by the function's start byte offset.
fn collect_function_cfgs(
    node: &tree_sitter::Node,
    source: &str,
    cfgs: &mut HashMap<usize, cfg::FunctionCfg>,
) {
    if node.kind() == "function_definition" {
        if let Some(function_cfg) = cfg::build_function_cfg(node, source) {
            cfgs.insert(node.start_byte(), function_cfg);
        }
    }
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            collect_function_cfgs(&child, source, cfgs);
        }
    }
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

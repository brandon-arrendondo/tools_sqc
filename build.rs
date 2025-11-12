use std::fs::{self, File};
use std::io::Write;
use std::path::PathBuf;
use walkdir::WalkDir;
use anyhow::{Context, Result};
use serde::Deserialize;
use std::collections::HashMap;

#[derive(Deserialize)]
struct RuleConfig {
    rules: Option<HashMap<String, HashMap<String, RuleSettings>>>,
    rule: Option<RuleSingleSettings>,
}

#[derive(Deserialize)]
struct RuleSettings {
    enabled: bool,
}

#[derive(Deserialize)]
struct RuleSingleSettings {
    enabled: bool,
}

fn main() {
    // Only compile resources on Windows
    #[cfg(target_os = "windows")]
    {
        let mut res = winres::WindowsResource::new();
        res.set_icon("icon.ico");
        if let Err(e) = res.compile() {
            eprintln!("Warning: Failed to compile Windows resources: {}", e);
        }
    }

    // Generate rules-all.toml from individual RULE-ID.toml files
    if let Err(e) = generate_rules_all_toml() {
        eprintln!("Error generating rules-all.toml: {}", e);
        std::process::exit(1);
    }

    // Generate integration tests from C test files
    if let Err(e) = generate_integration_tests() {
        eprintln!("Error generating integration tests: {}", e);
        std::process::exit(1);
    }
}

fn generate_rules_all_toml() -> Result<()> {
    let rules_dir = PathBuf::from("src/rules");

    // Find all ruleset directories (direct children of src/rules/)
    let rulesets: Vec<PathBuf> = match fs::read_dir(&rules_dir) {
        Ok(entries) => entries
            .filter_map(|e| match e {
                Ok(entry) => Some(entry.path()),
                Err(err) => {
                    eprintln!("Warning: Skipping directory entry in src/rules: {}", err);
                    None
                }
            })
            .filter(|p| p.is_dir())
            .collect(),
        Err(e) => {
            eprintln!("Warning: Could not read src/rules directory: {}", e);
            return Ok(()); // Not fatal - continue build
        }
    };

    // Process each ruleset
    for ruleset_dir in rulesets {
        let ruleset_name = match ruleset_dir.file_name() {
            Some(name) => name.to_string_lossy().to_string(),
            None => {
                eprintln!("Warning: Invalid ruleset directory path: {}", ruleset_dir.display());
                continue;
            }
        };

        let output_path = ruleset_dir.join("rules-all.toml");

        // Collect all RULE-ID.toml files in this ruleset
        let mut toml_files: Vec<PathBuf> = WalkDir::new(&ruleset_dir)
            .into_iter()
            .filter_map(|e| match e {
                Ok(entry) => Some(entry),
                Err(err) => {
                    eprintln!("Warning: Skipping entry in {}: {}", ruleset_dir.display(), err);
                    None
                }
            })
            .filter(|e| {
                e.path().is_file()
                    && e.path().extension().map_or(false, |ext| ext == "toml")
                    && e.path()
                        .file_name()
                        .and_then(|name| name.to_str())
                        .map_or(false, |name_str| {
                            // Match pattern like ARR30-C.toml, but exclude rules-all.toml
                            name_str != "rules-all.toml"
                                && name_str.contains('-')
                                && !name_str.starts_with('.')
                        })
            })
            .map(|e| e.path().to_path_buf())
            .collect();

        if toml_files.is_empty() {
            println!("No rule manifests found in ruleset: {}", ruleset_name);
            continue;
        }

        // Sort by rule ID for consistent output
        toml_files.sort();

        // Combine all TOML files - extract only the rules.cert_c section
        let mut combined_content = String::new();
        combined_content.push_str("# Auto-generated file - do not edit directly\n");
        combined_content.push_str(&format!("# Generated from individual rule manifests in {}\n", ruleset_name));
        combined_content.push_str("# To modify, edit the individual TOML files and rebuild\n");
        combined_content.push_str("# Full metadata is in individual rule TOML files\n\n");

        for toml_path in &toml_files {
            // Read full content
            match fs::read_to_string(toml_path) {
                Ok(content) => {
                    // Extract only the [rules.cert_c.RULE-ID] section
                    if let Some(start_idx) = content.find("[rules.cert_c.") {
                        let section_content = &content[start_idx..];

                        // Find the end of this section (next section or end of file)
                        let end_idx = section_content.find("\n[")
                            .map(|i| i + 1)
                            .unwrap_or(section_content.len());

                        let rule_section = &section_content[..end_idx];
                        combined_content.push_str(rule_section);
                        if !rule_section.ends_with('\n') {
                            combined_content.push('\n');
                        }
                        combined_content.push('\n');
                    }
                }
                Err(e) => {
                    eprintln!("Warning: Failed to read {}: {}", toml_path.display(), e);
                }
            }
        }

        // Write combined file
        let mut file = File::create(&output_path)
            .context(format!("Failed to create rules-all.toml at {}", output_path.display()))?;

        file.write_all(combined_content.as_bytes())
            .context(format!("Failed to write rules-all.toml to {}", output_path.display()))?;

        println!("cargo:rerun-if-changed={}", ruleset_dir.display());
        println!("Generated {} rules-all.toml with {} rule manifests",
                 ruleset_name, toml_files.len());
    }

    Ok(())
}

fn generate_integration_tests() -> Result<()> {
    let out_dir = std::env::var("OUT_DIR")
        .context("OUT_DIR environment variable not set")?;
    let out_dir_path = PathBuf::from(&out_dir);

    // Create tests subdirectory
    let tests_dir = out_dir_path.join("tests");
    fs::create_dir_all(&tests_dir)
        .context("Failed to create tests directory")?;

    // Track all rule modules for the main file
    let mut rule_modules = Vec::new();

    let cert_c_dir = PathBuf::from("src/rules/cert_c");

    // Walk through CATEGORY directories
    let category_entries = fs::read_dir(&cert_c_dir)
        .context("Failed to read src/rules/cert_c directory - does it exist?")?;

    for category_entry in category_entries {
        let category_entry = match category_entry {
            Ok(entry) => entry,
            Err(e) => {
                eprintln!("Warning: Skipping category entry: {}", e);
                continue;
            }
        };

        let category_path = category_entry.path();

        if !category_path.is_dir() {
            continue;
        }

        let category_name = match category_path.file_name().and_then(|n| n.to_str()) {
            Some(name) => name,
            None => {
                eprintln!("Warning: Skipping category with invalid path: {}", category_path.display());
                continue;
            }
        };

        // Skip special directories
        if category_name == "tests" || category_name == "utils" || category_name.starts_with('.') {
            continue;
        }

        // Walk through RULE-ID directories
        let rule_entries = match fs::read_dir(&category_path) {
            Ok(entries) => entries,
            Err(e) => {
                eprintln!("Warning: Skipping category {}: {}", category_name, e);
                continue;
            }
        };

        for rule_entry in rule_entries {
            let rule_entry = match rule_entry {
                Ok(entry) => entry,
                Err(e) => {
                    eprintln!("Warning: Skipping rule entry in {}: {}", category_name, e);
                    continue;
                }
            };

            let rule_path = rule_entry.path();

            if !rule_path.is_dir() {
                continue;
            }

            let rule_id = match rule_path.file_name().and_then(|n| n.to_str()) {
                Some(id) => id,
                None => {
                    eprintln!("Warning: Skipping rule with invalid path: {}", rule_path.display());
                    continue;
                }
            };

            let rule_tests_dir = rule_path.join("tests");

            if !rule_tests_dir.exists() {
                continue;
            }

            // Create per-rule test file
            let rule_snake = rule_id.to_lowercase().replace('-', "_");
            let rule_test_file = tests_dir.join(format!("{}_tests.rs", rule_snake));

            let mut rule_file = File::create(&rule_test_file)
                .context(format!("Failed to create test file for {}: {}", rule_id, rule_test_file.display()))?;

            // Write per-rule file header (no use statements - they're in the main file)
            writeln!(rule_file, "// Auto-generated tests for {}", rule_id)?;
            writeln!(rule_file, "// DO NOT EDIT - Generated by build.rs\n")?;

            // Track this module for the main file
            rule_modules.push(rule_snake.clone());

            // Generate tests for fail/ directory
            let fail_dir = rule_tests_dir.join("fail");
            if fail_dir.exists() {
                let fail_entries = match fs::read_dir(&fail_dir) {
                    Ok(entries) => entries,
                    Err(e) => {
                        eprintln!("Warning: Could not read fail directory for {}: {}", rule_id, e);
                        continue;
                    }
                };

                for test_file in fail_entries {
                    let test_file = match test_file {
                        Ok(file) => file,
                        Err(e) => {
                            eprintln!("Warning: Skipping test file in {}/fail: {}", rule_id, e);
                            continue;
                        }
                    };

                    let test_path = test_file.path();

                    if test_path.extension().map_or(false, |e| e == "c") {
                        generate_test_function(
                            &mut rule_file,
                            rule_id,
                            category_name,
                            &test_path,
                            "fail"
                        ).context(format!("Failed to generate test for {:?}", test_path))?;
                    }
                }
            }

            // Generate tests for pass/ directory
            let pass_dir = rule_tests_dir.join("pass");
            if pass_dir.exists() {
                let pass_entries = match fs::read_dir(&pass_dir) {
                    Ok(entries) => entries,
                    Err(e) => {
                        eprintln!("Warning: Could not read pass directory for {}: {}", rule_id, e);
                        continue;
                    }
                };

                for test_file in pass_entries {
                    let test_file = match test_file {
                        Ok(file) => file,
                        Err(e) => {
                            eprintln!("Warning: Skipping test file in {}/pass: {}", rule_id, e);
                            continue;
                        }
                    };

                    let test_path = test_file.path();

                    if test_path.extension().map_or(false, |e| e == "c") {
                        generate_test_function(
                            &mut rule_file,
                            rule_id,
                            category_name,
                            &test_path,
                            "pass"
                        ).context(format!("Failed to generate test for {:?}", test_path))?;
                    }
                }
            }
        }
    }

    // Generate main integration_tests.rs with module includes using include!()
    let dest_path = out_dir_path.join("integration_tests.rs");
    let mut main_file = File::create(&dest_path)
        .context("Failed to create integration_tests.rs")?;

    writeln!(main_file, "// Auto-generated test includes")?;
    writeln!(main_file, "// DO NOT EDIT - Generated by build.rs\n")?;
    writeln!(main_file, "#[cfg(test)]")?;
    writeln!(main_file, "mod generated_tests {{")?;
    writeln!(main_file, "    use crate::parser::CParser;")?;
    writeln!(main_file, "    use crate::rules::{{CertRule, RuleRegistry}};")?;
    writeln!(main_file, "    use std::path::Path;\n")?;

    // Sort for consistent output
    rule_modules.sort();

    for rule_module in &rule_modules {
        // Use include!() to inline the test file contents
        writeln!(main_file, "    include!(concat!(env!(\"OUT_DIR\"), \"/tests/{}_tests.rs\"));", rule_module)?;
    }

    writeln!(main_file, "}}")?;

    println!("cargo:rerun-if-changed=src/rules/cert_c");
    println!("Generated {} per-rule test files", rule_modules.len());
    Ok(())
}

fn generate_test_function(
    f: &mut File,
    rule_id: &str,
    category: &str,
    test_path: &std::path::Path,
    test_type: &str  // "fail" or "pass"
) -> Result<()> {
    // Convert rule_id to snake_case for function name
    let rule_snake = rule_id.to_lowercase().replace('-', "_");

    // Get test file name without extension
    let test_file_stem = test_path.file_stem()
        .and_then(|s| s.to_str())
        .context("Invalid test file name")?;
    let test_name_safe = test_file_stem.replace('-', "_").replace('.', "_");

    // Generate test function name: test_arr00_c_fail_wiki_noncompliant_1
    let test_fn_name = format!("test_{}_{}_{}",  rule_snake, test_type, test_name_safe);

    // Get relative path from project root
    let test_filename = test_path.file_name()
        .and_then(|n| n.to_str())
        .context("Invalid test filename")?;
    let relative_path = format!("src/rules/cert_c/{}/{}/tests/{}/{}",
        category, rule_id, test_type, test_filename);

    // Check if rule is implemented by reading the TOML file
    let toml_path = format!("src/rules/cert_c/{}/{}/{}.toml", category, rule_id, rule_id);
    let is_enabled = check_if_rule_enabled(&toml_path)?;

    writeln!(f, "#[test]")?;

    // If rule is not enabled/implemented, mark test as ignored
    if !is_enabled {
        writeln!(f, "#[ignore = \"Rule {} not yet implemented\"]", rule_id)?;
    }
    writeln!(f, "fn {}() {{", test_fn_name)?;
    writeln!(f, "    let registry = RuleRegistry::new();")?;
    writeln!(f, "    let rule = registry.get_rule(\"{}\").expect(\"Rule {} not found in registry\");",
        rule_id, rule_id)?;
    writeln!(f, "    ")?;
    writeln!(f, "    let test_path = Path::new(env!(\"CARGO_MANIFEST_DIR\")).join(\"{}\");",
        relative_path)?;
    writeln!(f, "    let source = std::fs::read_to_string(&test_path)")?;
    writeln!(f, "        .unwrap_or_else(|e| panic!(\"Failed to read {{:?}}: {{}}\", test_path, e));")?;
    writeln!(f, "    ")?;
    writeln!(f, "    let mut parser = CParser::new().expect(\"Failed to create parser\");")?;
    writeln!(f, "    let tree = parser.parse_source(&source)")?;
    writeln!(f, "        .unwrap_or_else(|e| panic!(\"Failed to parse {{:?}}: {{}}\", test_path, e));")?;
    writeln!(f, "    ")?;
    writeln!(f, "    let violations = rule.check(&tree.root_node(), &source);")?;
    writeln!(f, "    ")?;

    if test_type == "fail" {
        writeln!(f, "    let detected_violation = !violations.is_empty();")?;
        writeln!(f, "    ")?;
        writeln!(f, "    // Record result for report generation")?;
        writeln!(f, "    // For fail tests: we expect violations to be detected (detected_violation should be true)")?;
        writeln!(f, "    super::record_test_result(\"{}\", detected_violation, true);", test_fn_name)?;
        writeln!(f, "    ")?;
        writeln!(f, "    assert!(")?;
        writeln!(f, "        detected_violation,")?;
        writeln!(f, "        \"[{}] Expected violation in {{:?}} but found none\",", rule_id)?;
        writeln!(f, "        test_path.file_name().unwrap()")?;
        writeln!(f, "    );")?;
    } else {
        writeln!(f, "    let no_violation = violations.is_empty();")?;
        writeln!(f, "    ")?;
        writeln!(f, "    // Record result for report generation")?;
        writeln!(f, "    // For pass tests: we expect NO violations to be detected (no_violation should be true)")?;
        writeln!(f, "    super::record_test_result(\"{}\", no_violation, false);", test_fn_name)?;
        writeln!(f, "    ")?;
        writeln!(f, "    assert!(")?;
        writeln!(f, "        no_violation,")?;
        writeln!(f, "        \"[{}] Unexpected violation in {{:?}}: {{}}\",", rule_id)?;
        writeln!(f, "        test_path.file_name().unwrap(),")?;
        writeln!(f, "        violations.first().map(|v| &v.message).unwrap_or(&String::from(\"unknown\"))")?;
        writeln!(f, "    );")?;
    }

    writeln!(f, "}}")?;
    writeln!(f)?;

    Ok(())
}

fn check_if_rule_enabled(toml_path: &str) -> Result<bool> {
    // Read the TOML file and check if rule is enabled
    let content = match fs::read_to_string(toml_path) {
        Ok(content) => content,
        Err(_) => return Ok(false), // If TOML file doesn't exist, assume not implemented
    };

    // Parse TOML properly using serde
    let config: RuleConfig = match toml::from_str(&content) {
        Ok(config) => config,
        Err(e) => {
            eprintln!("Warning: Failed to parse TOML {}: {}", toml_path, e);
            eprintln!("         Falling back to string matching");
            // Fallback to string matching if TOML parse fails
            return Ok(content.contains("[rules.cert_c.") && content.contains("enabled = true"));
        }
    };

    // Check for implemented rule format: [rules.cert_c.RULE-ID] enabled = true
    if let Some(rules) = config.rules {
        if let Some(cert_c_rules) = rules.get("cert_c") {
            for (_rule_id, settings) in cert_c_rules {
                if settings.enabled {
                    return Ok(true);
                }
            }
        }
    }

    // Check for unimplemented rule format: [rule] enabled = false
    if let Some(rule_settings) = config.rule {
        return Ok(rule_settings.enabled);
    }

    // Default to false if format is unclear
    Ok(false)
}

pub mod prelude;

mod analyze;
mod export;
mod files;
mod manifest;
mod parser;
mod progress;
mod rules;
mod ui;
mod utility;

use crate::prelude::*;
use clap::{Arg, Command};

use analyze::analyze_project;
use analyze::handle_generate_suppression;
use export::export_all_violations;
use files::ProjectSource;
use progress::CLIProgressReporter;
use ui::TerminalUI;

fn main() -> Result<()> {
    let matches = Command::new("sqc")
        .about("Software Code Quality - CERT C compliance checker")
        .version("0.1.0")
        .arg(
            Arg::new("path")
                .help("Path to the file, directory, or git repository to analyze")
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
                .default_value("rules_templates/rules-all.toml"),
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

        // Create progress reporter for CLI
        let progress_reporter = CLIProgressReporter::new();

        // Perform analysis with progress reporting
        let violations = analyze_project(&project_source, &manifest, Some(&progress_reporter))?;

        // Export to file if requested
        if let Some(export_path) = export_file {
            export_all_violations(&violations, export_path, path, &manifest)?;
            println!("Exported violations to: {}", export_path);
        }
    }

    Ok(())
}

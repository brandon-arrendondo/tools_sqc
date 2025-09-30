use clap::{Arg, Command};
use anyhow::Result;

mod rules;
mod manifest;
mod ui;
mod git;
mod parser;

use manifest::RuleManifest;
use ui::TerminalUI;
use git::ProjectSource;

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
        .get_matches();

    let path = matches.get_one::<String>("path").unwrap();
    let manifest_path = matches.get_one::<String>("manifest").unwrap();
    let interactive = matches.get_flag("interactive");

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

        // Get C files to analyze
        let c_files = project_source.get_c_files()?;
        println!("Found {} C files to analyze", c_files.len());

        // TODO: Implement non-interactive analysis
    }

    Ok(())
}

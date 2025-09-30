use clap::{Arg, App};
use anyhow::Result;

mod rules;
mod manifest;
mod ui;
mod git;
mod parser;

use manifest::RuleManifest;
use ui::TerminalUI;

fn main() -> Result<()> {
    let matches = App::new("sqc")
        .about("Software Code Quality - CERT C compliance checker")
        .version("0.1.0")
        .arg(
            Arg::new("path")
                .help("Path to the git repository to analyze")
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
                .help("Run in interactive terminal UI mode"),
        )
        .get_matches();

    let repo_path = matches.value_of("path").unwrap();
    let manifest_path = matches.value_of("manifest").unwrap();
    let interactive = matches.is_present("interactive");

    let manifest = RuleManifest::load(manifest_path)?;

    if interactive {
        let mut ui = TerminalUI::new(repo_path, manifest)?;
        ui.run()?;
    } else {
        println!("Analyzing repository at: {}", repo_path);
        println!("Using manifest: {}", manifest_path);
        // TODO: Implement non-interactive analysis
    }

    Ok(())
}

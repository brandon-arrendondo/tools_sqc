// Library interface for tools_scq
// This allows running tests via `cargo test`

pub mod analyze;
pub mod export;
pub mod files;
pub mod manifest;
pub mod parser;
pub mod progress;
pub mod rules;
pub mod ui;
pub mod utility;

// Prelude for common types and functions
pub mod prelude {
    pub use anyhow::{Context, Result};
    pub use crate::files::ProjectSource;
    pub use crate::manifest::{RuleManifest, Severity};
    pub use crate::rules::{CertRule, RuleRegistry, RuleViolation};
    pub use crate::utility::files::get_relative_path;
    pub use crate::utility::hash::calculate_file_hash;
}

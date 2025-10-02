//! Common imports used throughout the SQC project

// Re-export commonly used types and traits
pub use crate::rules::{RuleRegistry, RuleViolation};
pub use crate::manifest::{RuleManifest};
pub use crate::utility::hash::calculate_file_hash;
pub use crate::utility::files::get_relative_path;

// Re-export common error handling
pub use anyhow::{Result, Error};

// Re-export common standard library items
pub use std::collections::HashMap;
pub use std::fs;
pub use std::path::{Path, PathBuf};

// Re-export commonly used external crates
pub use serde::{Deserialize, Serialize};

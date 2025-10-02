use sha2::{Sha256, Digest};
use std::fs;
use anyhow::Result;

pub fn calculate_file_hash(file_path: &str) -> Result<String> {
    let content = fs::read(file_path)?;
    let mut hasher = Sha256::new();
    hasher.update(&content);
    let result = hasher.finalize();
    Ok(format!("{:x}", result)[..8].to_string()) // First 8 chars of hash
}

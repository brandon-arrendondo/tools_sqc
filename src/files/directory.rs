use walkdir::WalkDir;
use std::path::Path;
use anyhow::{Context, Result};

pub struct DirectorySource {
    path: String,
    is_file: bool,
}

impl DirectorySource {
    pub fn open(path: &str) -> Result<Self> {
        let path_obj = Path::new(path);
        if !path_obj.exists() {
            return Err(anyhow::anyhow!("Path does not exist: {}", path));
        }

        let is_file = path_obj.is_file();
        let is_dir = path_obj.is_dir();

        if !is_file && !is_dir {
            return Err(anyhow::anyhow!("Path is neither a file nor a directory: {}", path));
        }

        // If it's a file, verify it's a C file
        if is_file {
            if let Some(extension) = path_obj.extension() {
                if extension != "c" && extension != "h" {
                    return Err(anyhow::anyhow!("File must have .c or .h extension: {}", path));
                }
            } else {
                return Err(anyhow::anyhow!("File must have .c or .h extension: {}", path));
            }
        }

        Ok(Self {
            path: path.to_string(),
            is_file,
        })
    }

    pub fn get_c_files(&self) -> Result<Vec<String>> {
        let mut c_files = Vec::new();

        // If it's a single file, just return that file
        if self.is_file {
            // Convert to absolute path
            let path_obj = Path::new(&self.path);
            if let Ok(abs_path) = path_obj.canonicalize() {
                if let Some(path_str) = abs_path.to_str() {
                    c_files.push(path_str.to_string());
                }
            } else {
                // Fall back to the original path if canonicalize fails
                c_files.push(self.path.clone());
            }
            return Ok(c_files);
        }

        // Otherwise, walk the directory
        for entry in WalkDir::new(&self.path)
            .into_iter()
            .filter_map(|e| e.ok())
        {
            let path = entry.path();
            if let Some(extension) = path.extension() {
                if extension == "c" || extension == "h" {
                    if let Some(path_str) = path.to_str() {
                        // Skip files in .git directory (in case there's a .git folder but not a valid repo)
                        if !path_str.contains("/.git/") {
                            c_files.push(path_str.to_string());
                        }
                    }
                }
            }
        }

        Ok(c_files)
    }

    pub fn get_modified_c_files(&self) -> Result<Vec<String>> {
        // For non-git directories, return all C files as "modified"
        // since we don't have version control information
        self.get_c_files()
    }

    pub fn get_root_path(&self) -> &str {
        &self.path
    }

    pub fn is_file(&self) -> bool {
        self.is_file
    }
}
use walkdir::WalkDir;
use std::path::Path;
use anyhow::{Context, Result};

pub struct DirectorySource {
    directory_path: String,
}

impl DirectorySource {
    pub fn open(path: &str) -> Result<Self> {
        let path_obj = Path::new(path);
        if !path_obj.exists() {
            return Err(anyhow::anyhow!("Path does not exist: {}", path));
        }
        if !path_obj.is_dir() {
            return Err(anyhow::anyhow!("Path is not a directory: {}", path));
        }

        Ok(Self {
            directory_path: path.to_string(),
        })
    }

    pub fn get_c_files(&self) -> Result<Vec<String>> {
        let mut c_files = Vec::new();

        for entry in WalkDir::new(&self.directory_path)
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
        &self.directory_path
    }
}
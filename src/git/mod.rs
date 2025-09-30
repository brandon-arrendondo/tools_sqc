use anyhow::{Context, Result};
use git2::{Repository, Status, StatusOptions};
use walkdir::WalkDir;
use std::path::Path;

pub enum ProjectSource {
    Git(GitRepo),
    Directory(DirectorySource),
}

impl ProjectSource {
    pub fn open(path: &str) -> Result<Self> {
        // First try to open as a git repository
        if let Ok(git_repo) = GitRepo::open(path) {
            Ok(ProjectSource::Git(git_repo))
        } else {
            // Fall back to directory source
            let dir_source = DirectorySource::open(path)?;
            Ok(ProjectSource::Directory(dir_source))
        }
    }

    pub fn get_c_files(&self) -> Result<Vec<String>> {
        match self {
            ProjectSource::Git(git_repo) => git_repo.get_c_files(),
            ProjectSource::Directory(dir_source) => dir_source.get_c_files(),
        }
    }

    pub fn get_modified_c_files(&self) -> Result<Vec<String>> {
        match self {
            ProjectSource::Git(git_repo) => git_repo.get_modified_c_files(),
            ProjectSource::Directory(dir_source) => dir_source.get_modified_c_files(),
        }
    }

    pub fn get_root_path(&self) -> &str {
        match self {
            ProjectSource::Git(git_repo) => git_repo.get_repo_root(),
            ProjectSource::Directory(dir_source) => dir_source.get_root_path(),
        }
    }

    pub fn source_type(&self) -> &str {
        match self {
            ProjectSource::Git(_) => "git repository",
            ProjectSource::Directory(_) => "directory",
        }
    }
}

pub struct GitRepo {
    repo: Repository,
    repo_path: String,
}

impl GitRepo {
    pub fn open(path: &str) -> Result<Self> {
        let repo = Repository::open(path)
            .with_context(|| format!("Failed to open git repository at: {}", path))?;

        Ok(Self {
            repo,
            repo_path: path.to_string(),
        })
    }

    pub fn get_c_files(&self) -> Result<Vec<String>> {
        let mut c_files = Vec::new();

        for entry in WalkDir::new(&self.repo_path)
            .into_iter()
            .filter_map(|e| e.ok())
        {
            let path = entry.path();
            if let Some(extension) = path.extension() {
                if extension == "c" || extension == "h" {
                    if let Some(path_str) = path.to_str() {
                        // Skip files in .git directory
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
        let mut modified_files = Vec::new();
        let mut status_options = StatusOptions::new();
        status_options.include_untracked(true);

        let statuses = self.repo.statuses(Some(&mut status_options))
            .context("Failed to get repository status")?;

        for entry in statuses.iter() {
            let flags = entry.status();
            if flags.intersects(
                Status::WT_MODIFIED
                | Status::WT_NEW
                | Status::INDEX_MODIFIED
                | Status::INDEX_NEW
            ) {
                if let Some(path) = entry.path() {
                    if path.ends_with(".c") || path.ends_with(".h") {
                        modified_files.push(path.to_string());
                    }
                }
            }
        }

        Ok(modified_files)
    }

    pub fn get_repo_root(&self) -> &str {
        &self.repo_path
    }
}

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
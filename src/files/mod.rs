mod directory;
mod git;

use directory::DirectorySource;
use git::GitRepo;

use anyhow::Result;

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

    #[allow(dead_code)]
    pub fn get_root_path(&self) -> &str {
        match self {
            ProjectSource::Git(git_repo) => git_repo.get_repo_root(),
            ProjectSource::Directory(dir_source) => dir_source.get_root_path(),
        }
    }

    pub fn source_type(&self) -> &str {
        match self {
            ProjectSource::Git(_) => "git repository",
            ProjectSource::Directory(dir_source) => {
                if dir_source.is_file() {
                    "file"
                } else {
                    "directory"
                }
            }
        }
    }
}

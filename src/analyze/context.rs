use std::collections::HashSet;

/// Cross-file context gathered by pre-scanning additional directories.
///
/// Holds function names found in `.c`/`.h` files so that rules like DCL31-C
/// and DCL07-C can suppress false positives for project-internal functions
/// defined in other translation units.
#[derive(Debug, Default, Clone)]
pub struct ProjectContext {
    pub known_functions: HashSet<String>,
}

impl ProjectContext {
    pub fn new() -> Self {
        Self::default()
    }

    /// Returns `true` if the given name was found during the pre-scan.
    pub fn is_known_function(&self, name: &str) -> bool {
        self.known_functions.contains(name)
    }

    /// Returns `true` if any cross-file data was collected.
    pub fn has_cross_file_data(&self) -> bool {
        !self.known_functions.is_empty()
    }
}

use super::function_summary::FunctionSummary;
use std::collections::{HashMap, HashSet};

/// Cross-file context gathered by pre-scanning additional directories.
///
/// Holds function names found in `.c`/`.h` files so that rules like DCL31-C
/// and DCL07-C can suppress false positives for project-internal functions
/// defined in other translation units.
#[derive(Debug, Default, Clone)]
pub struct ProjectContext {
    pub known_functions: HashSet<String>,
    /// Functions declared (prototyped) in `.h` header files.
    /// A function with a header prototype is public API and should not be
    /// flagged by DCL15-C/DCL19-C as needing `static`.
    pub header_declared_functions: HashSet<String>,
    /// Function summaries computed during prescan for inter-procedural analysis.
    pub function_summaries: HashMap<String, FunctionSummary>,
    /// Call graph: maps function name to the set of functions it calls.
    pub call_graph: HashMap<String, HashSet<String>>,
    /// Macro constants collected from `#define` directives across all scanned files.
    pub macro_constants: HashMap<String, i64>,
}

impl ProjectContext {
    pub fn new() -> Self {
        Self::default()
    }

    /// Returns `true` if the given name was found during the pre-scan.
    pub fn is_known_function(&self, name: &str) -> bool {
        self.known_functions.contains(name)
    }

    /// Returns the summary for a function, if available.
    pub fn get_function_summary(&self, name: &str) -> Option<&FunctionSummary> {
        self.function_summaries.get(name)
    }

    /// Returns `true` if the function has a prototype in a `.h` header file,
    /// indicating it is public API with intentional external linkage.
    pub fn is_header_declared(&self, name: &str) -> bool {
        self.header_declared_functions.contains(name)
    }

    /// Returns `true` if any cross-file data was collected.
    pub fn has_cross_file_data(&self) -> bool {
        !self.known_functions.is_empty()
            || !self.function_summaries.is_empty()
            || !self.macro_constants.is_empty()
    }
}

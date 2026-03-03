use std::io::{self, Write};
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::Arc;

/// Trait for reporting progress during analysis
pub trait ProgressReporter: Send + Sync {
    /// Report progress for a file being analyzed
    ///
    /// # Arguments
    /// * `current` - Current file number (1-indexed)
    /// * `total` - Total number of files
    /// * `file_path` - Relative path to the file being analyzed
    /// * `rule_id` - Current rule being checked
    fn report_file(&self, current: usize, total: usize, file_path: &str, rule_id: &str);

    /// Report completion of analysis
    ///
    /// # Arguments
    /// * `total_violations` - Total number of violations found
    fn report_complete(&self, total_violations: usize);

    /// Check if the analysis should be cancelled
    ///
    /// # Returns
    /// `true` if cancellation has been requested, `false` otherwise
    fn is_cancelled(&self) -> bool;

    /// Report the start of a directory pre-scan phase
    fn report_prescan_start(&self, _num_directories: usize) {}

    /// Report the completion of directory pre-scanning
    fn report_prescan_complete(&self, _num_functions: usize) {}

    /// Report the start of include path resolution
    fn report_include_resolve_start(&self, _num_include_paths: usize) {}

    /// Report the completion of include path resolution
    fn report_include_resolve_complete(&self, _num_headers: usize) {}
}

/// CLI progress reporter that displays progress on a single terminal line
pub struct CLIProgressReporter {
    last_line_length: Arc<std::sync::Mutex<usize>>,
}

impl Default for CLIProgressReporter {
    fn default() -> Self {
        Self {
            last_line_length: Arc::new(std::sync::Mutex::new(0)),
        }
    }
}

impl CLIProgressReporter {
    pub fn new() -> Self {
        Self::default()
    }

    /// Clear the current line
    fn clear_line(&self) {
        let last_len = *self.last_line_length.lock().unwrap();
        if last_len > 0 {
            print!("\r{}\r", " ".repeat(last_len));
            io::stdout().flush().unwrap_or(());
        }
    }
}

impl ProgressReporter for CLIProgressReporter {
    fn report_prescan_start(&self, num_directories: usize) {
        println!(
            "Pre-scanning {} {} for function definitions...",
            num_directories,
            if num_directories == 1 {
                "directory"
            } else {
                "directories"
            }
        );
    }

    fn report_prescan_complete(&self, num_functions: usize) {
        println!(
            "Pre-scan complete: found {} function definitions",
            num_functions
        );
    }

    fn report_include_resolve_start(&self, num_include_paths: usize) {
        println!(
            "Resolving #include directives against {} search {}...",
            num_include_paths,
            if num_include_paths == 1 {
                "path"
            } else {
                "paths"
            }
        );
    }

    fn report_include_resolve_complete(&self, num_headers: usize) {
        println!(
            "Include resolution complete: parsed {} header {}",
            num_headers,
            if num_headers == 1 { "file" } else { "files" }
        );
    }

    fn report_file(&self, current: usize, total: usize, file_path: &str, rule_id: &str) {
        // Format: "Scanning: [file 15/42] src/foo/bar.c - Checking: ARR30-C"
        let message = format!(
            "Scanning: [file {}/{}] {} - Checking: {}",
            current, total, file_path, rule_id
        );

        // Store length for clearing
        *self.last_line_length.lock().unwrap() = message.len();

        // Print with carriage return to overwrite line
        print!("\r{}", message);
        io::stdout().flush().unwrap_or(());
    }

    fn report_complete(&self, total_violations: usize) {
        // Clear the progress line
        self.clear_line();

        // Print completion message
        println!("Scan complete. Found {} violations", total_violations);
    }

    fn is_cancelled(&self) -> bool {
        // CLI mode doesn't support cancellation
        false
    }
}

/// GUI progress reporter that stores state for display in the terminal UI
#[allow(dead_code)]
pub struct GUIProgressReporter {
    pub current_file: Arc<std::sync::Mutex<usize>>,
    pub total_files: Arc<std::sync::Mutex<usize>>,
    pub file_path: Arc<std::sync::Mutex<String>>,
    pub rule_id: Arc<std::sync::Mutex<String>>,
    pub cancellation: Arc<AtomicBool>,
}

impl GUIProgressReporter {
    #[allow(dead_code)]
    pub fn new(cancellation: Arc<AtomicBool>) -> Self {
        Self {
            current_file: Arc::new(std::sync::Mutex::new(0)),
            total_files: Arc::new(std::sync::Mutex::new(0)),
            file_path: Arc::new(std::sync::Mutex::new(String::new())),
            rule_id: Arc::new(std::sync::Mutex::new(String::new())),
            cancellation,
        }
    }

    /// Get current progress state for display
    #[allow(dead_code)]
    pub fn get_state(&self) -> (usize, usize, String, String) {
        let current = *self.current_file.lock().unwrap();
        let total = *self.total_files.lock().unwrap();
        let file = self.file_path.lock().unwrap().clone();
        let rule = self.rule_id.lock().unwrap().clone();
        (current, total, file, rule)
    }
}

impl ProgressReporter for GUIProgressReporter {
    fn report_file(&self, current: usize, total: usize, file_path: &str, rule_id: &str) {
        *self.current_file.lock().unwrap() = current;
        *self.total_files.lock().unwrap() = total;
        *self.file_path.lock().unwrap() = file_path.to_string();
        *self.rule_id.lock().unwrap() = rule_id.to_string();
    }

    fn report_complete(&self, _total_violations: usize) {
        // GUI handles completion display separately
    }

    fn is_cancelled(&self) -> bool {
        self.cancellation.load(Ordering::Relaxed)
    }
}

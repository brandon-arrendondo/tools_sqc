use super::context::ProjectContext;
use super::function_summary;
use crate::parser::CParser;
use crate::progress::ProgressReporter;

use anyhow::Result;
use std::collections::{HashMap, HashSet};
use tree_sitter::Node;
use walkdir::WalkDir;

/// Pre-scan the given directories to collect function names and summaries from `.c`/`.h` files.
///
/// This provides cross-file context so that rules like DCL31-C and DCL07-C
/// can suppress false positives for functions defined in other translation units.
/// Function summaries enable inter-procedural analysis (e.g., knowing if a callee
/// can return NULL, frees a parameter, or never returns).
pub fn prescan_directories(
    dirs: &[String],
    progress: Option<&dyn ProgressReporter>,
) -> Result<ProjectContext> {
    let mut known_functions = HashSet::new();
    let mut function_summaries = HashMap::new();
    let mut call_graph = HashMap::new();
    let mut parser = CParser::new()?;

    if let Some(reporter) = progress {
        reporter.report_prescan_start(dirs.len());
    }

    for dir in dirs {
        for entry in WalkDir::new(dir)
            .into_iter()
            .filter_map(|e| e.ok())
            .filter(|e| {
                let path = e.path();
                matches!(
                    path.extension().and_then(|ext| ext.to_str()),
                    Some("c") | Some("h")
                )
            })
        {
            let file_path = entry.path().to_string_lossy().to_string();
            if let Ok((tree, source)) = parser.parse_file(&file_path) {
                let root = tree.root_node();
                collect_function_names(&root, &source, &mut known_functions);

                // Compute function summaries for this file
                let file_summaries = function_summary::compute_summaries(&root, &source);
                for (name, summary) in file_summaries {
                    function_summaries.insert(name, summary);
                }

                // Build call graph for this file
                collect_call_graph(&root, &source, &mut call_graph);
            }
        }
    }

    if let Some(reporter) = progress {
        reporter.report_prescan_complete(known_functions.len());
    }

    Ok(ProjectContext {
        known_functions,
        function_summaries,
        call_graph,
    })
}

/// Extract function names from top-level `function_definition` and `declaration`
/// nodes, recursing into `preproc_*` blocks (same pattern as EXP33-C/SIG31-C).
fn collect_function_names(node: &Node, source: &str, names: &mut HashSet<String>) {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            match child.kind() {
                "function_definition" => {
                    if let Some(name) = extract_function_name_from_declarator(&child, source) {
                        names.insert(name);
                    }
                }
                "declaration" => {
                    // Only collect if it contains a function_declarator (i.e. a prototype)
                    if let Some(name) = extract_function_name_from_declaration(&child, source) {
                        names.insert(name);
                    }
                }
                kind if kind.starts_with("preproc_") => {
                    collect_function_names(&child, source, names);
                }
                _ => {}
            }
        }
    }
}

/// Extract function name from a `function_definition` node's declarator.
fn extract_function_name_from_declarator(node: &Node, source: &str) -> Option<String> {
    let declarator = node.child_by_field_name("declarator")?;
    extract_identifier_from_declarator(&declarator, source)
}

/// Extract function name from a `declaration` node if it's a function prototype.
fn extract_function_name_from_declaration(node: &Node, source: &str) -> Option<String> {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            match child.kind() {
                "function_declarator" => {
                    return extract_identifier_from_declarator(&child, source);
                }
                "init_declarator" => {
                    for j in 0..child.child_count() {
                        if let Some(grandchild) = child.child(j) {
                            if grandchild.kind() == "function_declarator" {
                                return extract_identifier_from_declarator(&grandchild, source);
                            }
                        }
                    }
                }
                _ => {}
            }
        }
    }
    None
}

/// Drill into a declarator tree to find the leaf `identifier`.
fn extract_identifier_from_declarator(node: &Node, source: &str) -> Option<String> {
    match node.kind() {
        "identifier" => {
            let name = node.utf8_text(source.as_bytes()).unwrap_or("").to_string();
            if name.is_empty() {
                None
            } else {
                Some(name)
            }
        }
        "function_declarator" | "pointer_declarator" => {
            let inner = node.child_by_field_name("declarator")?;
            extract_identifier_from_declarator(&inner, source)
        }
        _ => {
            // Fallback: search children for an identifier
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() == "identifier" {
                        let name = child.utf8_text(source.as_bytes()).unwrap_or("").to_string();
                        if !name.is_empty() {
                            return Some(name);
                        }
                    }
                }
            }
            None
        }
    }
}

/// Build a call graph by walking function definitions and recording call expressions.
fn collect_call_graph(
    node: &Node,
    source: &str,
    call_graph: &mut HashMap<String, HashSet<String>>,
) {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            match child.kind() {
                "function_definition" => {
                    if let Some(func_name) = extract_function_name_from_declarator(&child, source) {
                        let mut callees = HashSet::new();
                        collect_callees(&child, source, &mut callees);
                        call_graph.insert(func_name, callees);
                    }
                }
                kind if kind.starts_with("preproc_") => {
                    collect_call_graph(&child, source, call_graph);
                }
                _ => {}
            }
        }
    }
}

/// Collect all function names called within a node (recursive walk).
fn collect_callees(node: &Node, source: &str, callees: &mut HashSet<String>) {
    if node.kind() == "call_expression" {
        if let Some(function) = node.child_by_field_name("function") {
            if function.kind() == "identifier" {
                if let Ok(name) = function.utf8_text(source.as_bytes()) {
                    if !name.is_empty() {
                        callees.insert(name.to_string());
                    }
                }
            }
        }
    }
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            collect_callees(&child, source, callees);
        }
    }
}

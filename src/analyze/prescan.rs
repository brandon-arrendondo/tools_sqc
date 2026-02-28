use super::context::ProjectContext;
use super::function_summary::{self, FunctionSummary};
use crate::analyze::null_state::NullState;
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
    let mut header_declared_functions = HashSet::new();
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
            let is_header = entry.path().extension().and_then(|ext| ext.to_str()) == Some("h");

            if let Ok((tree, source)) = parser.parse_file(&file_path) {
                let root = tree.root_node();
                collect_function_names(&root, &source, &mut known_functions);

                // Track function declarations from header files separately —
                // these are public API with intentional external linkage.
                if is_header {
                    collect_header_declarations(&root, &source, &mut header_declared_functions);
                }

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

    // Second pass: collect argument null states at call sites and aggregate
    // per-callee per-param. This seeds callee parameter states during analysis.
    collect_callsite_null_states(dirs, &mut function_summaries, &header_declared_functions);

    if let Some(reporter) = progress {
        reporter.report_prescan_complete(known_functions.len());
    }

    Ok(ProjectContext {
        known_functions,
        header_declared_functions,
        function_summaries,
        call_graph,
    })
}

/// Collect function declarations (prototypes) from a header file.
/// These represent public API functions with intentional external linkage.
fn collect_header_declarations(node: &Node, source: &str, names: &mut HashSet<String>) {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            match child.kind() {
                "declaration" => {
                    // Only collect non-static function prototypes
                    if !has_static_specifier(&child, source) {
                        if let Some(name) = extract_function_name_from_declaration(&child, source) {
                            names.insert(name);
                        }
                    }
                }
                kind if kind.starts_with("preproc_")
                    || kind == "linkage_specification"
                    || kind == "declaration_list" =>
                {
                    collect_header_declarations(&child, source, names);
                }
                _ => {}
            }
        }
    }
}

/// Check if a declaration node has a `static` storage class specifier.
fn has_static_specifier(node: &Node, source: &str) -> bool {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if child.kind() == "storage_class_specifier" {
                if let Ok(text) = child.utf8_text(source.as_bytes()) {
                    if text == "static" {
                        return true;
                    }
                }
            }
        }
    }
    false
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
                kind if kind.starts_with("preproc_")
                    || kind == "linkage_specification"
                    || kind == "declaration_list" =>
                {
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
///
/// Handles both direct function declarators (`void foo(...)`) and
/// pointer-returning declarators (`int *foo(...)`) where tree-sitter
/// wraps the function_declarator inside a pointer_declarator.
fn extract_function_name_from_declaration(node: &Node, source: &str) -> Option<String> {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            match child.kind() {
                "function_declarator" => {
                    return extract_identifier_from_declarator(&child, source);
                }
                "pointer_declarator" => {
                    // e.g. `ArrayList *ArrayList_New(int a, int b);`
                    // pointer_declarator wraps the function_declarator
                    return extract_func_name_from_nested_declarator(&child, source);
                }
                "init_declarator" => {
                    for j in 0..child.child_count() {
                        if let Some(grandchild) = child.child(j) {
                            if grandchild.kind() == "function_declarator" {
                                return extract_identifier_from_declarator(&grandchild, source);
                            }
                            if grandchild.kind() == "pointer_declarator" {
                                return extract_func_name_from_nested_declarator(
                                    &grandchild,
                                    source,
                                );
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

/// Recursively search a declarator subtree for a function_declarator
/// and extract its identifier.  Handles chains like
/// `pointer_declarator -> function_declarator -> identifier`.
fn extract_func_name_from_nested_declarator(node: &Node, source: &str) -> Option<String> {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            match child.kind() {
                "function_declarator" => {
                    return extract_identifier_from_declarator(&child, source);
                }
                "pointer_declarator" => {
                    return extract_func_name_from_nested_declarator(&child, source);
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
                kind if kind.starts_with("preproc_")
                    || kind == "linkage_specification"
                    || kind == "declaration_list" =>
                {
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

// ---------------------------------------------------------------------------
// Second pass: call-site argument null state collection
// ---------------------------------------------------------------------------

/// Collect argument null states at all call sites and aggregate into callee summaries.
///
/// For each callee, joins per-param states across all call sites:
/// - all NotNull → NotNull (safe to skip null check)
/// - any DefinitelyNull → PossiblyNull (mixed callers)
/// - any Unknown → leaves as Unknown (will default to PossiblyNull in analysis)
///
/// Functions declared in headers get an implicit Unknown caller to prevent
/// false NotNull seeding for externally-visible functions.
fn collect_callsite_null_states(
    dirs: &[String],
    summaries: &mut HashMap<String, FunctionSummary>,
    header_declared: &HashSet<String>,
) {
    // Accumulate: callee_name → Vec<Vec<NullState>> (one inner vec per call site)
    let mut callsite_args: HashMap<String, Vec<Vec<NullState>>> = HashMap::new();
    let mut parser = match CParser::new() {
        Ok(p) => p,
        Err(_) => return,
    };

    for dir in dirs {
        for entry in WalkDir::new(dir)
            .into_iter()
            .filter_map(|e| e.ok())
            .filter(|e| matches!(e.path().extension().and_then(|ext| ext.to_str()), Some("c")))
        {
            let file_path = entry.path().to_string_lossy().to_string();
            if let Ok((tree, source)) = parser.parse_file(&file_path) {
                collect_callsite_args_from_tree(&tree.root_node(), &source, &mut callsite_args);
            }
        }
    }

    // For header-declared functions, add an implicit Unknown arg vector.
    // This prevents false NotNull seeding for externally-visible functions
    // that may be called from code we don't scan.
    for func_name in header_declared {
        if let Some(summary) = summaries.get(func_name) {
            // Count how many params this function has from its summary
            let max_param = summary
                .dereferences_params
                .iter()
                .chain(summary.checks_null_params.iter())
                .chain(summary.frees_params.iter())
                .chain(summary.modifies_params.iter())
                .max()
                .copied()
                .unwrap_or(0);
            if max_param > 0 || !summary.dereferences_params.is_empty() {
                let unknown_args = vec![NullState::Unknown; max_param + 1];
                callsite_args
                    .entry(func_name.clone())
                    .or_default()
                    .push(unknown_args);
            }
        }
    }

    // Join per-callee per-param
    for (callee_name, arg_vectors) in &callsite_args {
        if let Some(summary) = summaries.get_mut(callee_name) {
            let max_params = arg_vectors.iter().map(|v| v.len()).max().unwrap_or(0);
            for param_idx in 0..max_params {
                let mut joined = NullState::Unknown;
                let mut any_known = false;
                for args in arg_vectors {
                    if let Some(&state) = args.get(param_idx) {
                        if state != NullState::Unknown {
                            if !any_known {
                                joined = state;
                                any_known = true;
                            } else {
                                joined = joined.join(state);
                            }
                        }
                    }
                    // Missing arg → treat as Unknown (no contribution to join)
                }
                // Only store if we have concrete info
                if any_known {
                    summary.callsite_param_null_states.insert(param_idx, joined);
                }
            }
        }
    }
}

/// Walk an AST tree collecting call-site argument null states.
fn collect_callsite_args_from_tree(
    node: &Node,
    source: &str,
    callsite_args: &mut HashMap<String, Vec<Vec<NullState>>>,
) {
    if node.kind() == "call_expression" {
        if let Some(function) = node.child_by_field_name("function") {
            if function.kind() == "identifier" {
                let callee_name = function.utf8_text(source.as_bytes()).unwrap_or("");
                if !callee_name.is_empty() {
                    if let Some(args_node) = node.child_by_field_name("arguments") {
                        let mut arg_states = Vec::new();
                        for i in 0..args_node.child_count() {
                            if let Some(arg) = args_node.child(i) {
                                if arg.kind() == "," || arg.kind() == "(" || arg.kind() == ")" {
                                    continue;
                                }
                                arg_states
                                    .push(function_summary::infer_arg_null_state(&arg, source));
                            }
                        }
                        if !arg_states.is_empty() {
                            callsite_args
                                .entry(callee_name.to_string())
                                .or_default()
                                .push(arg_states);
                        }
                    }
                }
            }
        }
    }

    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            collect_callsite_args_from_tree(&child, source, callsite_args);
        }
    }
}

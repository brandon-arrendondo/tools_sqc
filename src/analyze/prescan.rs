use super::const_eval;
use super::context::ProjectContext;
use super::function_summary::{self, FunctionSummary};
use crate::analyze::null_state::NullState;
use crate::parser::CParser;
use crate::progress::ProgressReporter;

use anyhow::Result;
use std::collections::{HashMap, HashSet};
use std::path::{Path, PathBuf};
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
    let mut macro_constants = HashMap::new();
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

                // Collect macro constants from #define directives
                let file_macros = const_eval::collect_macro_constants(&root, &source);
                macro_constants.extend(file_macros);
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
        macro_constants,
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
/// For each function definition, first collects local variable assignments
/// to resolve identifier arguments (e.g., `data = NULL; sink(data)` → DefinitelyNull).
fn collect_callsite_args_from_tree(
    node: &Node,
    source: &str,
    callsite_args: &mut HashMap<String, Vec<Vec<NullState>>>,
) {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            match child.kind() {
                "function_definition" => {
                    // Collect local variable states within this function
                    if let Some(body) = child.child_by_field_name("body") {
                        let mut local_states = collect_local_var_states(&body, source);
                        // Detect early-return null guards: `if (p == NULL) return;`
                        // After the guard, p is guaranteed NotNull.
                        collect_early_return_null_guards(&body, source, &mut local_states);
                        collect_calls_with_locals(&body, source, &local_states, callsite_args);
                    }
                }
                kind if kind.starts_with("preproc_") => {
                    collect_callsite_args_from_tree(&child, source, callsite_args);
                }
                _ => {}
            }
        }
    }
}

/// Collect simple local variable assignments within a function body.
/// Tracks the *last* assignment to each variable (flow-insensitive, conservative).
/// Only tracks simple patterns: `var = NULL`, `var = "string"`, `var = &x`, `var = func()`.
fn collect_local_var_states(body: &Node, source: &str) -> HashMap<String, NullState> {
    let mut states = HashMap::new();
    collect_assignments_recursive(body, source, &mut states);
    states
}

/// Detect early-return null guard patterns in the function body.
/// Pattern: `if (var == NULL) return;` or `if (!var) return;` — after the guard,
/// var is guaranteed NotNull for the rest of the function.
/// Inserts guarded variables as NotNull into the states map.
fn collect_early_return_null_guards(
    body: &Node,
    source: &str,
    states: &mut HashMap<String, NullState>,
) {
    for i in 0..body.child_count() {
        if let Some(child) = body.child(i) {
            if child.kind() == "if_statement" {
                if let Some(condition) = child.child_by_field_name("condition") {
                    // Check if consequence contains a return statement (early exit)
                    if has_early_return_consequence(&child) {
                        // Extract variable names from null-check condition
                        for var_name in extract_null_checked_vars(&condition, source) {
                            states.insert(var_name, NullState::NotNull);
                        }
                    }
                }
            }
        }
    }
}

/// Check if an if_statement's consequence contains a return/goto/exit.
fn has_early_return_consequence(if_node: &Node) -> bool {
    if let Some(consequence) = if_node.child_by_field_name("consequence") {
        return node_contains_return(&consequence);
    }
    false
}

fn node_contains_return(node: &Node) -> bool {
    if matches!(node.kind(), "return_statement" | "goto_statement") {
        return true;
    }
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if node_contains_return(&child) {
                return true;
            }
        }
    }
    false
}

/// Extract variable names from a null-check condition.
/// Recognizes: `var == NULL`, `NULL == var`, `!var`, `var == 0`, `0 == var`.
fn extract_null_checked_vars(condition: &Node, source: &str) -> Vec<String> {
    let mut vars = Vec::new();
    let cond_text = condition
        .utf8_text(source.as_bytes())
        .unwrap_or("")
        .trim()
        .to_string();
    // Strip outer parens from parenthesized_expression
    let cond_text = if cond_text.starts_with('(') && cond_text.ends_with(')') {
        &cond_text[1..cond_text.len() - 1]
    } else {
        &cond_text
    };

    // Split on || for compound conditions: `if (!a || !b) return;`
    for part in cond_text.split("||") {
        let part = part.trim();
        // Pattern: !var
        if let Some(var) = part.strip_prefix('!') {
            let var = var.trim();
            if is_simple_identifier(var) {
                vars.push(var.to_string());
            }
        }
        // Pattern: var == NULL or var == 0
        else if let Some(pos) = part.find("==") {
            let left = part[..pos].trim();
            let right = part[pos + 2..].trim();
            if (right == "NULL" || right == "0") && is_simple_identifier(left) {
                vars.push(left.to_string());
            } else if (left == "NULL" || left == "0") && is_simple_identifier(right) {
                vars.push(right.to_string());
            }
        }
    }
    vars
}

/// Check if a string is a simple C identifier (no operators, no spaces).
fn is_simple_identifier(s: &str) -> bool {
    !s.is_empty()
        && s.chars().all(|c| c.is_alphanumeric() || c == '_')
        && s.chars()
            .next()
            .is_some_and(|c| c.is_alphabetic() || c == '_')
}

fn collect_assignments_recursive(
    node: &Node,
    source: &str,
    states: &mut HashMap<String, NullState>,
) {
    match node.kind() {
        "expression_statement" => {
            // Look for assignment expressions: var = expr
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() == "assignment_expression" {
                        if let (Some(left), Some(right)) = (
                            child.child_by_field_name("left"),
                            child.child_by_field_name("right"),
                        ) {
                            if left.kind() == "identifier" {
                                let var_name =
                                    left.utf8_text(source.as_bytes()).unwrap_or("").to_string();
                                if !var_name.is_empty() {
                                    let state = infer_rhs_null_state(&right, source);
                                    if state != NullState::Unknown {
                                        states.insert(var_name, state);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        "declaration" => {
            // Handle `type *var = expr;` init declarations
            if let Some(decl) = node.child_by_field_name("declarator") {
                extract_init_state(&decl, source, states);
            }
            // Also check for multiple declarators and array declarations
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() == "init_declarator" {
                        extract_init_state(&child, source, states);
                    }
                    // Stack arrays can never be null — mark as NotNull
                    if child.kind() == "array_declarator" {
                        let var_name = extract_leaf_id(&child, source);
                        if !var_name.is_empty() {
                            states.insert(var_name, NullState::NotNull);
                        }
                    }
                }
            }
        }
        _ => {}
    }

    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            collect_assignments_recursive(&child, source, states);
        }
    }
}

/// Extract null state from an init_declarator: `*var = expr` or `var = expr`.
fn extract_init_state(decl: &Node, source: &str, states: &mut HashMap<String, NullState>) {
    if let Some(value) = decl.child_by_field_name("value") {
        // Find the variable name in the declarator
        let name_node = decl.child_by_field_name("declarator").unwrap_or(*decl);
        let var_name = extract_leaf_id(&name_node, source);
        if !var_name.is_empty() {
            let state = infer_rhs_null_state(&value, source);
            if state != NullState::Unknown {
                states.insert(var_name, state);
            }
        }
    }
}

/// Extract the leaf identifier from a declarator chain.
fn extract_leaf_id(node: &Node, source: &str) -> String {
    match node.kind() {
        "identifier" => node.utf8_text(source.as_bytes()).unwrap_or("").to_string(),
        "pointer_declarator" | "array_declarator" => {
            if let Some(inner) = node.child_by_field_name("declarator") {
                extract_leaf_id(&inner, source)
            } else {
                String::new()
            }
        }
        _ => {
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() == "identifier" {
                        return child.utf8_text(source.as_bytes()).unwrap_or("").to_string();
                    }
                }
            }
            String::new()
        }
    }
}

/// Infer null state of a right-hand-side expression in an assignment.
fn infer_rhs_null_state(node: &Node, source: &str) -> NullState {
    // Delegate to the existing literal-level inference first
    let literal_state = function_summary::infer_arg_null_state(node, source);
    if literal_state != NullState::Unknown {
        return literal_state;
    }

    // Additional patterns for RHS:
    match node.kind() {
        "call_expression" => {
            // malloc/calloc/realloc can return NULL → PossiblyNull
            if let Some(func) = node.child_by_field_name("function") {
                let func_name = func.utf8_text(source.as_bytes()).unwrap_or("");
                if matches!(
                    func_name,
                    "malloc" | "calloc" | "realloc" | "aligned_alloc" | "strdup" | "strndup"
                ) {
                    return NullState::PossiblyNull;
                }
            }
            NullState::Unknown
        }
        _ => NullState::Unknown,
    }
}

/// Walk call expressions within a function body, using local variable states
/// to resolve identifier arguments.
fn collect_calls_with_locals(
    node: &Node,
    source: &str,
    local_states: &HashMap<String, NullState>,
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
                                // First try literal-level inference
                                let state = function_summary::infer_arg_null_state(&arg, source);
                                if state != NullState::Unknown {
                                    arg_states.push(state);
                                } else if arg.kind() == "identifier" {
                                    // Look up in local variable states
                                    let name = arg.utf8_text(source.as_bytes()).unwrap_or("");
                                    if let Some(&local_state) = local_states.get(name) {
                                        arg_states.push(local_state);
                                    } else {
                                        arg_states.push(NullState::Unknown);
                                    }
                                } else {
                                    arg_states.push(NullState::Unknown);
                                }
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
            collect_calls_with_locals(&child, source, local_states, callsite_args);
        }
    }
}

// ---------------------------------------------------------------------------
// Include path resolution (-I flag)
// ---------------------------------------------------------------------------

/// Resolve `#include` directives from source files against the given include
/// search paths, parse found headers, and merge declarations into `context`.
///
/// Each resolved header is parsed only once (deduped by canonical path).
/// Both `"quoted.h"` and `<angle.h>` forms are resolved; unfound system
/// headers are silently skipped.
pub fn resolve_includes(
    source_files: &[String],
    include_paths: &[String],
    context: &mut super::context::ProjectContext,
    progress: Option<&dyn ProgressReporter>,
) -> Result<()> {
    if let Some(reporter) = progress {
        reporter.report_include_resolve_start(include_paths.len());
    }

    let mut parser = CParser::new()?;
    let mut resolved_set: HashSet<PathBuf> = HashSet::new();

    // Queue of (include_path, source_dir) pairs to resolve — supports transitive includes
    let mut queue: Vec<(String, Option<PathBuf>)> = Vec::new();

    // Seed the queue with #include directives from source files
    for file_path in source_files {
        if let Ok((tree, source)) = parser.parse_file(file_path) {
            let directives = extract_include_directives(&tree.root_node(), &source);
            let source_dir = Path::new(file_path).parent().map(|p| p.to_path_buf());
            for inc in directives {
                queue.push((inc, source_dir.clone()));
            }
        }
    }

    // Process queue: resolve each header, parse it, and enqueue its transitive includes
    while let Some((include_path, source_dir)) = queue.pop() {
        if let Some(resolved) = resolve_header(&include_path, source_dir.as_deref(), include_paths)
        {
            let canonical = match resolved.canonicalize() {
                Ok(c) => c,
                Err(_) => resolved.clone(),
            };
            if resolved_set.contains(&canonical) {
                continue;
            }
            resolved_set.insert(canonical);

            let header_path = resolved.to_string_lossy().to_string();
            if let Ok((htree, hsource)) = parser.parse_file(&header_path) {
                let root = htree.root_node();
                collect_function_names(&root, &hsource, &mut context.known_functions);
                collect_header_declarations(
                    &root,
                    &hsource,
                    &mut context.header_declared_functions,
                );
                let file_summaries = function_summary::compute_summaries(&root, &hsource);
                for (name, summary) in file_summaries {
                    context.function_summaries.insert(name, summary);
                }

                // Collect macro constants from resolved headers
                let header_macros = const_eval::collect_macro_constants(&root, &hsource);
                context.macro_constants.extend(header_macros);

                // Enqueue transitive includes from this header
                let header_dir = resolved.parent().map(|p| p.to_path_buf());
                for inc in extract_include_directives(&root, &hsource) {
                    queue.push((inc, header_dir.clone()));
                }
            }
        }
    }

    if let Some(reporter) = progress {
        reporter.report_include_resolve_complete(resolved_set.len());
    }

    Ok(())
}

/// Extract `#include` directive paths from an AST.
///
/// Walks `preproc_include` nodes and extracts the path string, stripping
/// both `"..."` and `<...>` delimiters. Recurses into `preproc_*` nodes
/// to handle conditional includes.
fn extract_include_directives(node: &Node, source: &str) -> Vec<String> {
    let mut directives = Vec::new();
    extract_includes_recursive(node, source, &mut directives);
    directives
}

fn extract_includes_recursive(node: &Node, source: &str, directives: &mut Vec<String>) {
    match node.kind() {
        "preproc_include" => {
            // The path child contains the include path (e.g. "foo.h" or <foo.h>)
            if let Some(path_node) = node.child_by_field_name("path") {
                if let Ok(text) = path_node.utf8_text(source.as_bytes()) {
                    let text = text.trim();
                    // Strip delimiters: "foo.h" -> foo.h, <foo.h> -> foo.h
                    let path = if (text.starts_with('"') && text.ends_with('"'))
                        || (text.starts_with('<') && text.ends_with('>'))
                    {
                        &text[1..text.len() - 1]
                    } else {
                        text
                    };
                    if !path.is_empty() {
                        directives.push(path.to_string());
                    }
                }
            }
        }
        kind if kind.starts_with("preproc_") => {
            // Recurse into conditional compilation blocks
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    extract_includes_recursive(&child, source, directives);
                }
            }
        }
        _ => {}
    }

    // For non-preproc nodes, walk children (translation_unit, etc.)
    if !node.kind().starts_with("preproc_") {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                extract_includes_recursive(&child, source, directives);
            }
        }
    }
}

/// Resolve an include path against search directories.
///
/// Search order: (1) source file's directory (if available), (2) each `-I`
/// path in order. Returns the first match where the candidate is a file.
fn resolve_header(
    include_path: &str,
    source_dir: Option<&Path>,
    include_search_paths: &[String],
) -> Option<PathBuf> {
    // First: try relative to the source file's directory
    if let Some(dir) = source_dir {
        let candidate = dir.join(include_path);
        if candidate.is_file() {
            return Some(candidate);
        }
    }

    // Then: try each -I path in order
    for search_dir in include_search_paths {
        let candidate = Path::new(search_dir).join(include_path);
        if candidate.is_file() {
            return Some(candidate);
        }
    }

    None
}

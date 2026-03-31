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
    needs_vra: bool,
) -> Result<ProjectContext> {
    let mut known_functions = HashSet::new();
    let mut header_declared_functions = HashSet::new();
    let mut function_summaries = HashMap::new();
    let mut call_graph = HashMap::new();
    let mut macro_constants = HashMap::new();
    let mut macro_aliases = HashMap::new();
    let mut struct_field_types = HashMap::new();
    let mut global_constants: HashMap<String, i64> = HashMap::new();
    let mut global_var_null_states: HashMap<String, NullState> = HashMap::new();
    let mut callsite_args: HashMap<String, Vec<Vec<NullState>>> = HashMap::new();
    let mut source_files: Vec<PathBuf> = Vec::new();
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

                // Collect macro constants from #define directives (before summaries —
                // return-range computation needs macro values when VRA is active)
                let file_macros = const_eval::collect_macro_constants(&root, &source);
                macro_constants.extend(file_macros.clone());

                // Compute function summaries for this file
                let file_summaries =
                    function_summary::compute_summaries(&root, &source, &file_macros, needs_vra);
                for (name, summary) in file_summaries {
                    function_summaries.insert(name, summary);
                }

                // Build call graph for this file
                collect_call_graph(&root, &source, &mut call_graph);

                // Collect macro aliases (#define ALIAS identifier)
                let file_aliases = const_eval::collect_macro_aliases(&root, &source);
                macro_aliases.extend(file_aliases);

                // Collect struct field types from struct definitions
                collect_struct_definitions(&root, &source, &mut struct_field_types);

                // Collect global constants for dead-branch elimination
                collect_global_constants(&root, &source, &mut global_constants);

                // Collect global pointer variable null states (cross-file)
                if !is_header {
                    collect_global_var_null_states(&root, &source, &mut global_var_null_states);
                }

                // Collect call-site argument null states in the same pass
                // (avoids re-parsing all files in a second directory walk)
                if !is_header {
                    collect_callsite_args_from_tree(&root, &source, &mut callsite_args);
                    source_files.push(entry.path().to_path_buf());
                }
            }
        }
    }

    // Aggregate callsite null states into function summaries
    aggregate_callsite_null_states(
        &callsite_args,
        &mut function_summaries,
        &header_declared_functions,
    );

    // Iterative propagation: resolve parameter null states through relay chains.
    // Each pass resolves one additional hop. Runs up to MAX_PROPAGATION_PASSES
    // times or until convergence (no state changes between passes).
    //   void mid(int *p) { low(p); }  — p now resolved via mid's param state
    propagate_param_null_states(
        &source_files,
        &mut parser,
        &mut function_summaries,
        &mut callsite_args,
        &header_declared_functions,
    );

    if let Some(reporter) = progress {
        reporter.report_prescan_complete(known_functions.len());
    }

    Ok(ProjectContext {
        known_functions,
        header_declared_functions,
        function_summaries,
        call_graph,
        macro_constants,
        macro_aliases,
        struct_field_types,
        global_constants,
        global_var_null_states,
    })
}

/// Build a [`ProjectContext`] from a single already-parsed tree.
///
/// This is the intra-file equivalent of [`prescan_directories`]: it computes
/// function summaries and call-site null states so that inter-procedural rules
/// (e.g. EXP34-C) can resolve parameter state within one translation unit.
///
/// Used by the generated integration tests when a `.c` file carries the
/// `// sqc-test: prescan` marker.
#[cfg(test)]
pub fn prescan_single_tree(root: &Node, source: &str) -> ProjectContext {
    let macros = const_eval::collect_macro_constants(root, source);
    let mut function_summaries = function_summary::compute_summaries(root, source, &macros, false);

    let mut callsite_args: HashMap<String, Vec<Vec<NullState>>> = HashMap::new();
    collect_callsite_args_from_tree(root, source, &mut callsite_args);

    let empty_headers = HashSet::new();
    aggregate_callsite_null_states(&callsite_args, &mut function_summaries, &empty_headers);

    let known_functions: HashSet<String> = function_summaries.keys().cloned().collect();

    ProjectContext {
        known_functions,
        function_summaries,
        macro_constants: macros,
        ..ProjectContext::default()
    }
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

/// Extract function names from `function_definition` and `declaration` nodes.
///
/// Recurses into all child nodes — not just `preproc_*` blocks — because
/// tree-sitter may misparse files with complex macros (e.g., sqlite's
/// `sqliteInt.h`), burying function definitions inside `ERROR` or
/// `compound_statement` nodes at the top level. Even inside error recovery,
/// tree-sitter correctly identifies `function_definition` nodes.
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
                "preproc_function_def" => {
                    // Collect function-like macro names so DCL07-C/DCL31-C
                    // don't flag macro invocations as undeclared functions.
                    if let Some(name_node) = child.child_by_field_name("name") {
                        let name = name_node
                            .utf8_text(source.as_bytes())
                            .unwrap_or("")
                            .to_string();
                        if !name.is_empty() {
                            names.insert(name);
                        }
                    }
                    collect_function_names(&child, source, names);
                }
                _ => {
                    // Recurse into all other nodes (preproc_*, linkage_specification,
                    // ERROR, compound_statement, etc.) to find buried definitions.
                    collect_function_names(&child, source, names);
                }
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

/// Aggregate pre-collected callsite argument null states into callee summaries.
///
/// For each callee, joins per-param states across all call sites:
/// - all NotNull → NotNull (safe to skip null check)
/// - any DefinitelyNull → PossiblyNull (mixed callers)
/// - any Unknown → leaves as Unknown (will default to PossiblyNull in analysis)
///
/// Functions declared in headers get an implicit Unknown caller to prevent
/// false NotNull seeding for externally-visible functions.
fn aggregate_callsite_null_states(
    callsite_args: &HashMap<String, Vec<Vec<NullState>>>,
    summaries: &mut HashMap<String, FunctionSummary>,
    header_declared: &HashSet<String>,
) {
    // For header-declared functions, we need a mutable copy to add implicit Unknown entries
    let mut callsite_args = callsite_args.clone();

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

    // Count-based voting per-callee per-param.
    //
    // The prescan's local state tracking can't model null checks between
    // assignment and use, so PossiblyNull from prescan has a high false
    // positive rate.  Instead of a lattice join (where one PossiblyNull
    // callsite poisons a parameter used by 50 NotNull callers), we count
    // callsite states and apply:
    //   a) Any DefinitelyNull → PossiblyNull (confirmed NULL flow)
    //   b) Majority PossiblyNull (> NotNull count) → PossiblyNull
    //   c) Otherwise → NotNull (PossiblyNull is noise)
    for (callee_name, arg_vectors) in &callsite_args {
        if let Some(summary) = summaries.get_mut(callee_name) {
            let max_params = arg_vectors.iter().map(|v| v.len()).max().unwrap_or(0);
            for param_idx in 0..max_params {
                let mut null_count: usize = 0;
                let mut possibly_count: usize = 0;
                let mut not_null_count: usize = 0;
                for args in arg_vectors {
                    if let Some(&state) = args.get(param_idx) {
                        match state {
                            NullState::DefinitelyNull => null_count += 1,
                            NullState::PossiblyNull => possibly_count += 1,
                            NullState::NotNull => not_null_count += 1,
                            NullState::Unknown => {} // no contribution
                        }
                    }
                }
                let total_known = null_count + possibly_count + not_null_count;
                if total_known > 0 {
                    let aggregated = if null_count > 0 || possibly_count > not_null_count {
                        NullState::PossiblyNull
                    } else {
                        NullState::NotNull
                    };
                    summary
                        .callsite_param_null_states
                        .insert(param_idx, aggregated);
                }
            }
        }
    }
}

/// Second pass: propagate parameter null states through relay functions.
///
/// After the first aggregation, each function has `callsite_param_null_states` derived
/// from its direct callers. However, relay functions (which forward parameters to callees
/// without modification) produce Unknown at those call sites because function parameters
/// aren't in `local_states`.
///
/// This pass re-parses source files, seeds each function's parameter names with their
/// aggregated null states, and re-collects callsite args. Then re-aggregates to propagate
/// the states one level deeper through the call chain.
/// Maximum number of propagation passes for relay chain resolution.
/// Each pass resolves one additional hop in the call chain:
///   Pass 1: caller → relay (single hop)
///   Pass 2: caller → relay1 → relay2 (two hops)
///   Pass 3: caller → relay1 → relay2 → relay3 (three hops)
/// Most real-world code has at most 2-3 relay hops.
const MAX_PROPAGATION_PASSES: usize = 3;

fn propagate_param_null_states(
    source_files: &[PathBuf],
    parser: &mut CParser,
    summaries: &mut HashMap<String, FunctionSummary>,
    callsite_args: &mut HashMap<String, Vec<Vec<NullState>>>,
    header_declared: &HashSet<String>,
) {
    for _pass in 0..MAX_PROPAGATION_PASSES {
        // Snapshot the current param null states before re-collection
        let param_states_snapshot: HashMap<String, HashMap<usize, NullState>> = summaries
            .iter()
            .filter(|(_, s)| !s.callsite_param_null_states.is_empty())
            .map(|(name, s)| (name.clone(), s.callsite_param_null_states.clone()))
            .collect();

        // If no functions have param states, nothing to propagate
        if param_states_snapshot.is_empty() {
            return;
        }

        // Re-collect callsite args with parameter state awareness
        let mut new_callsite_args: HashMap<String, Vec<Vec<NullState>>> = HashMap::new();

        for file_path in source_files {
            if let Ok((tree, source)) = parser.parse_file(&file_path.to_string_lossy()) {
                let root = tree.root_node();
                collect_callsite_args_with_param_states(
                    &root,
                    &source,
                    &param_states_snapshot,
                    &mut new_callsite_args,
                );
            }
        }

        // Only proceed if this pass found any new information
        if new_callsite_args.is_empty() {
            return;
        }

        // Merge new callsite args into the existing ones
        for (callee, arg_vecs) in new_callsite_args {
            callsite_args.entry(callee).or_default().extend(arg_vecs);
        }

        // Clear old aggregated states and re-aggregate with the merged data
        let prev_states: HashMap<String, HashMap<usize, NullState>> = summaries
            .iter()
            .filter(|(_, s)| !s.callsite_param_null_states.is_empty())
            .map(|(name, s)| (name.clone(), s.callsite_param_null_states.clone()))
            .collect();

        for summary in summaries.values_mut() {
            summary.callsite_param_null_states.clear();
        }
        aggregate_callsite_null_states(callsite_args, summaries, header_declared);

        // Check for convergence: if no param states changed, stop early
        let converged = summaries.iter().all(|(name, s)| {
            let prev = prev_states.get(name);
            match prev {
                Some(prev_map) => *prev_map == s.callsite_param_null_states,
                None => s.callsite_param_null_states.is_empty(),
            }
        });
        if converged {
            return;
        }
    }
}

/// Like `collect_callsite_args_from_tree`, but also seeds function parameters
/// with their aggregated null states from the first pass. This resolves
/// parameter forwarding patterns like `void mid(int *p) { low(p); }`.
fn collect_callsite_args_with_param_states(
    node: &Node,
    source: &str,
    param_states: &HashMap<String, HashMap<usize, NullState>>,
    callsite_args: &mut HashMap<String, Vec<Vec<NullState>>>,
) {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            match child.kind() {
                "function_definition" => {
                    if let Some(body) = child.child_by_field_name("body") {
                        let mut local_states = collect_local_var_states(&body, source);
                        collect_early_return_null_guards(&body, source, &mut local_states);

                        // Extract function name and seed parameter states
                        let func_name = extract_function_name(&child, source);
                        if let Some(func_name) = func_name {
                            if let Some(func_param_states) = param_states.get(&func_name) {
                                // Get parameter names for this function
                                let param_names =
                                    function_summary::collect_param_names(&child, source);
                                for (idx, name) in param_names.iter().enumerate() {
                                    if !name.is_empty() && !local_states.contains_key(name.as_str())
                                    {
                                        // Only seed if the param isn't already in local_states
                                        // (guards and assignments take priority)
                                        if let Some(&state) = func_param_states.get(&idx) {
                                            local_states.insert(name.clone(), state);
                                        }
                                    }
                                }
                            }
                        }

                        collect_calls_with_locals(&body, source, &local_states, callsite_args);
                    }
                }
                kind if kind.starts_with("preproc_") => {
                    collect_callsite_args_with_param_states(
                        &child,
                        source,
                        param_states,
                        callsite_args,
                    );
                }
                "linkage_specification" => {
                    // Handle extern "C" { ... } blocks
                    collect_callsite_args_with_param_states(
                        &child,
                        source,
                        param_states,
                        callsite_args,
                    );
                }
                "declaration_list" => {
                    collect_callsite_args_with_param_states(
                        &child,
                        source,
                        param_states,
                        callsite_args,
                    );
                }
                _ => {}
            }
        }
    }
}

/// Extract the function name from a function_definition node.
fn extract_function_name(func_node: &Node, source: &str) -> Option<String> {
    let declarator = func_node.child_by_field_name("declarator")?;
    extract_func_name_recursive(&declarator, source)
}

fn extract_func_name_recursive(node: &Node, source: &str) -> Option<String> {
    match node.kind() {
        "function_declarator" => {
            if let Some(declarator) = node.child_by_field_name("declarator") {
                let name = declarator.utf8_text(source.as_bytes()).ok()?;
                let name = name.trim();
                // Handle pointer declarators: strip leading *
                let name = name.trim_start_matches('*');
                if !name.is_empty() {
                    return Some(name.to_string());
                }
            }
            None
        }
        "pointer_declarator" => {
            if let Some(inner) = node.child_by_field_name("declarator") {
                extract_func_name_recursive(&inner, source)
            } else {
                None
            }
        }
        "identifier" => {
            let name = node.utf8_text(source.as_bytes()).ok()?;
            Some(name.trim().to_string())
        }
        _ => {
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if let Some(name) = extract_func_name_recursive(&child, source) {
                        return Some(name);
                    }
                }
            }
            None
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
// Struct field type collection
// ---------------------------------------------------------------------------

/// Collect null states for file-scope non-static, non-extern pointer globals.
///
/// Finds declarations like `char *globalData;` or `char *globalData = NULL;`
/// at file scope, then walks all function bodies for assignments to those
/// globals. Tracks the common Juliet pattern: `data = NULL; globalVar = data;`.
///
/// Results are joined into the provided `states` map across all files.
/// Used by EXP34-C for cross-file variant 68 detection.
fn collect_global_var_null_states(
    root: &Node,
    source: &str,
    states: &mut HashMap<String, NullState>,
) {
    use crate::analyze::null_state;

    // Step 1: Find file-scope pointer globals (non-static, non-extern).
    let mut global_vars: HashSet<String> = HashSet::new();
    collect_prescan_pointer_globals(root, source, &mut global_vars, states);

    if global_vars.is_empty() {
        return;
    }

    // Step 2: Walk function bodies for assignments to these globals.
    for i in 0..root.child_count() {
        let child = match root.child(i) {
            Some(c) => c,
            None => continue,
        };
        match child.kind() {
            "function_definition" => {
                if let Some(body) = child.child_by_field_name("body") {
                    scan_global_var_assignments(&body, source, &global_vars, states);
                }
            }
            k if k.starts_with("preproc_") => {
                // Recurse into preprocessor blocks for nested function definitions
                scan_preproc_for_functions(&child, source, &global_vars, states);
            }
            _ => {}
        }
    }

    fn scan_preproc_for_functions(
        node: &Node,
        source: &str,
        global_vars: &HashSet<String>,
        states: &mut HashMap<String, NullState>,
    ) {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "function_definition" => {
                        if let Some(body) = child.child_by_field_name("body") {
                            scan_global_var_assignments(&body, source, global_vars, states);
                        }
                    }
                    k if k.starts_with("preproc_") => {
                        scan_preproc_for_functions(&child, source, global_vars, states);
                    }
                    _ => {}
                }
            }
        }
    }

    /// Classify an RHS expression as a NullState for prescan purposes.
    fn classify_rhs(node: &Node, source: &str) -> NullState {
        let text = node
            .utf8_text(source.as_bytes())
            .unwrap_or("")
            .trim()
            .to_string();
        if null_state::is_null_value(&text) {
            return NullState::DefinitelyNull;
        }
        // Cast to NULL: (type*)NULL or (type*)0
        if node.kind() == "cast_expression" {
            if let Some(value) = node.child_by_field_name("value") {
                let vt = value
                    .utf8_text(source.as_bytes())
                    .unwrap_or("")
                    .trim()
                    .to_string();
                if null_state::is_null_value(&vt) {
                    return NullState::DefinitelyNull;
                }
            }
        }
        // String literal → NotNull
        if node.kind() == "string_literal" {
            return NullState::NotNull;
        }
        // Address-of → NotNull
        if node.kind() == "pointer_expression" {
            if let Some(op) = node.child_by_field_name("operator") {
                if op.utf8_text(source.as_bytes()).unwrap_or("") == "&" {
                    return NullState::NotNull;
                }
            }
        }
        NullState::NotNull
    }

    /// Recursively scan a function body for assignments to global pointer vars.
    /// Handles `data = NULL; globalVar = data;` relay pattern.
    fn scan_global_var_assignments(
        node: &Node,
        source: &str,
        global_vars: &HashSet<String>,
        states: &mut HashMap<String, NullState>,
    ) {
        if node.kind() == "assignment_expression" {
            if let Some(left) = node.child_by_field_name("left") {
                let var_name = left.utf8_text(source.as_bytes()).unwrap_or("").to_string();
                if global_vars.contains(&var_name) {
                    if let Some(right) = node.child_by_field_name("right") {
                        let new_state = if right.kind() == "identifier" {
                            let rhs_text =
                                right.utf8_text(source.as_bytes()).unwrap_or("").to_string();
                            if null_state::is_null_value(&rhs_text) {
                                NullState::DefinitelyNull
                            } else {
                                // Check preceding statement for relay pattern:
                                //   data = NULL; globalVar = data;
                                check_preceding_assign_state(node, &rhs_text, source)
                            }
                        } else {
                            classify_rhs(&right, source)
                        };
                        let existing = states.get(&var_name).copied().unwrap_or(NullState::Unknown);
                        states.insert(var_name, existing.join(new_state));
                    }
                }
            }
        }
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                scan_global_var_assignments(&child, source, global_vars, states);
            }
        }
    }

    /// Check the preceding statement for a null/non-null assignment to the
    /// relay variable. Handles: `data = NULL; globalVar = data;`
    fn check_preceding_assign_state(
        assignment_node: &Node,
        var_name: &str,
        source: &str,
    ) -> NullState {
        let expr_stmt = match assignment_node.parent() {
            Some(p) if p.kind() == "expression_statement" => p,
            _ => return NullState::Unknown,
        };
        if let Some(prev) = expr_stmt.prev_sibling() {
            // Check expression_statement: `var = EXPR;`
            if prev.kind() == "expression_statement" {
                if let Some(expr) = prev.child(0) {
                    if expr.kind() == "assignment_expression" {
                        if let Some(left) = expr.child_by_field_name("left") {
                            if left.utf8_text(source.as_bytes()).unwrap_or("") == var_name {
                                if let Some(right) = expr.child_by_field_name("right") {
                                    return classify_rhs(&right, source);
                                }
                            }
                        }
                    }
                }
            }
            // Check declaration: `TYPE *var = EXPR;`
            if prev.kind() == "declaration" {
                for i in 0..prev.child_count() {
                    if let Some(child) = prev.child(i) {
                        if child.kind() == "init_declarator" {
                            let name = extract_declarator_name(&child, source);
                            if name == var_name {
                                if let Some(value) = child.child_by_field_name("value") {
                                    return classify_rhs(&value, source);
                                }
                            }
                        }
                    }
                }
            }
        }
        NullState::Unknown
    }
}

/// Find file-scope non-static, non-extern pointer declarations.
fn collect_prescan_pointer_globals(
    node: &Node,
    source: &str,
    global_vars: &mut HashSet<String>,
    states: &mut HashMap<String, NullState>,
) {
    use crate::analyze::null_state;

    for i in 0..node.child_count() {
        let child = match node.child(i) {
            Some(c) => c,
            None => continue,
        };
        match child.kind() {
            "declaration" => {
                // Collect type qualifiers and storage class
                let mut has_extern = false;
                let mut has_static = false;
                let mut has_pointer = false;
                for j in 0..child.child_count() {
                    if let Some(tc) = child.child(j) {
                        if tc.kind() == "storage_class_specifier" {
                            let text = tc.utf8_text(source.as_bytes()).unwrap_or("");
                            if text == "extern" {
                                has_extern = true;
                            }
                            if text == "static" {
                                has_static = true;
                            }
                        }
                        if tc.kind() == "pointer_declarator" || tc.kind() == "init_declarator" {
                            has_pointer = true;
                        }
                    }
                }
                // Skip extern (defined elsewhere) and static (file-local)
                if has_extern || has_static || !has_pointer {
                    continue;
                }
                // Extract pointer declarators with optional initializer
                for j in 0..child.child_count() {
                    if let Some(decl) = child.child(j) {
                        if decl.kind() == "init_declarator" {
                            // Check that it's a pointer type
                            if !has_pointer_in_declarator(&decl) {
                                continue;
                            }
                            let name = extract_declarator_name(&decl, source);
                            if name.is_empty() {
                                continue;
                            }
                            global_vars.insert(name.clone());
                            if let Some(value) = decl.child_by_field_name("value") {
                                let vtext = value
                                    .utf8_text(source.as_bytes())
                                    .unwrap_or("")
                                    .trim()
                                    .to_string();
                                let state = if null_state::is_null_value(&vtext) {
                                    NullState::DefinitelyNull
                                } else {
                                    NullState::NotNull
                                };
                                let existing =
                                    states.get(&name).copied().unwrap_or(NullState::Unknown);
                                states.insert(name, existing.join(state));
                            }
                        } else if decl.kind() == "pointer_declarator" {
                            // Bare pointer declarator without init
                            let name = extract_declarator_name(&decl, source);
                            if !name.is_empty() {
                                global_vars.insert(name);
                                // No initializer — leave as Unknown
                            }
                        }
                    }
                }
            }
            k if k.starts_with("preproc_") => {
                collect_prescan_pointer_globals(&child, source, global_vars, states);
            }
            _ => {}
        }
    }
}

/// Check if a declarator node contains a pointer indicator (`*`).
fn has_pointer_in_declarator(node: &Node) -> bool {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if child.kind() == "pointer_declarator" {
                return true;
            }
        }
    }
    false
}

/// Collect global constants (`[const] TYPE NAME = VALUE;`) from file-scope declarations.
/// Only collects non-static constants (static ones are file-local and handled by
/// `init_state::collect_file_scope_constants` within each file).
fn collect_global_constants(root: &Node, source: &str, constants: &mut HashMap<String, i64>) {
    for i in 0..root.child_count() {
        if let Some(child) = root.child(i) {
            match child.kind() {
                "declaration" => {
                    let type_text = {
                        let mut text = String::new();
                        for j in 0..child.child_count() {
                            if let Some(tc) = child.child(j) {
                                if tc.kind() == "storage_class_specifier"
                                    || tc.kind() == "type_qualifier"
                                    || tc.kind() == "primitive_type"
                                    || tc.kind() == "sized_type_specifier"
                                {
                                    if let Ok(t) = tc.utf8_text(source.as_bytes()) {
                                        text.push_str(t);
                                        text.push(' ');
                                    }
                                }
                            }
                        }
                        text
                    };
                    // Skip static declarations (handled per-file in init_state)
                    if type_text.contains("static") {
                        continue;
                    }
                    // Accept: const TYPE NAME = VALUE; or TYPE NAME = VALUE;
                    // (non-const globals are included for benchmark support)
                    for j in 0..child.child_count() {
                        if let Some(decl) = child.child(j) {
                            if decl.kind() == "init_declarator" {
                                let name = extract_declarator_name(&decl, source);
                                if name.is_empty() {
                                    continue;
                                }
                                if let Some(value) = decl.child_by_field_name("value") {
                                    let empty_macros: HashMap<String, i64> = HashMap::new();
                                    if let Some(val) =
                                        const_eval::try_evaluate_expr(&value, source, &empty_macros)
                                    {
                                        constants.insert(name, val);
                                    }
                                }
                            }
                        }
                    }
                }
                "preproc_ifdef" | "preproc_if" | "preproc_else" | "preproc_elif" => {
                    collect_global_constants(&child, source, constants);
                }
                _ => {}
            }
        }
    }
}

/// Extract declarator name from an init_declarator node.
fn extract_declarator_name(decl: &Node, source: &str) -> String {
    for i in 0..decl.child_count() {
        if let Some(child) = decl.child(i) {
            match child.kind() {
                "identifier" => {
                    return child.utf8_text(source.as_bytes()).unwrap_or("").to_string();
                }
                "pointer_declarator" => {
                    return extract_declarator_name(&child, source);
                }
                _ => {}
            }
        }
    }
    String::new()
}

/// Collect struct field types from struct definitions and typedefs.
///
/// Handles three patterns:
/// 1. `struct Name { type field; ... };`
/// 2. `typedef struct { type field; ... } Name;`
/// 3. `typedef struct Name { type field; ... } Name;` (or alias)
fn collect_struct_definitions(
    node: &Node,
    source: &str,
    struct_field_types: &mut HashMap<String, HashMap<String, String>>,
) {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            match child.kind() {
                "struct_specifier" => {
                    // Pattern 1: `struct Name { ... };` (top-level or inside declaration)
                    collect_from_struct_specifier(&child, source, struct_field_types);
                }
                "type_definition" => {
                    // Pattern 2/3: `typedef struct { ... } Name;`
                    collect_from_typedef(&child, source, struct_field_types);
                }
                "declaration" => {
                    // Struct definitions can appear inside declarations:
                    // `struct Name { ... } var;`
                    for j in 0..child.child_count() {
                        if let Some(gc) = child.child(j) {
                            if gc.kind() == "struct_specifier" {
                                collect_from_struct_specifier(&gc, source, struct_field_types);
                            }
                        }
                    }
                }
                kind if kind.starts_with("preproc_")
                    || kind == "linkage_specification"
                    || kind == "declaration_list" =>
                {
                    collect_struct_definitions(&child, source, struct_field_types);
                }
                _ => {}
            }
        }
    }
}

/// Extract fields from a `struct_specifier` node with a name and body.
fn collect_from_struct_specifier(
    node: &Node,
    source: &str,
    struct_field_types: &mut HashMap<String, HashMap<String, String>>,
) {
    let name = match node.child_by_field_name("name") {
        Some(n) => n.utf8_text(source.as_bytes()).unwrap_or("").to_string(),
        None => return, // Anonymous struct without typedef — can't reference by name
    };
    if name.is_empty() {
        return;
    }
    if let Some(body) = node.child_by_field_name("body") {
        let fields = extract_struct_fields(&body, source);
        if !fields.is_empty() {
            struct_field_types.insert(name, fields);
        }
    }
}

/// Extract fields from a `type_definition` containing a struct specifier.
/// Handles: `typedef struct [Name] { ... } Alias;`
fn collect_from_typedef(
    node: &Node,
    source: &str,
    struct_field_types: &mut HashMap<String, HashMap<String, String>>,
) {
    let mut struct_spec = None;
    let mut typedef_name = None;

    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if child.kind() == "struct_specifier" {
                struct_spec = Some(child);
            }
            // The typedef alias is a type_identifier at the end
            if child.kind() == "type_identifier" {
                typedef_name = Some(child.utf8_text(source.as_bytes()).unwrap_or("").to_string());
            }
            // Handle pointer typedefs: `typedef struct Foo *FooPtr;`
            if child.kind() == "pointer_declarator" {
                if let Some(inner) = child.child_by_field_name("declarator") {
                    if inner.kind() == "type_identifier" {
                        // Skip pointer typedefs — we want value types only
                    }
                }
            }
        }
    }

    if let Some(spec) = struct_spec {
        // First, collect under the struct's own name (if it has one)
        collect_from_struct_specifier(&spec, source, struct_field_types);

        // Then, also register under the typedef alias
        if let Some(alias) = typedef_name {
            if !alias.is_empty() {
                if let Some(body) = spec.child_by_field_name("body") {
                    let fields = extract_struct_fields(&body, source);
                    if !fields.is_empty() {
                        struct_field_types.insert(alias, fields);
                    }
                }
            }
        }
    }
}

/// Extract field name → type text from a `field_declaration_list` node.
fn extract_struct_fields(body: &Node, source: &str) -> HashMap<String, String> {
    let mut fields = HashMap::new();
    for i in 0..body.child_count() {
        if let Some(child) = body.child(i) {
            if child.kind() == "field_declaration" {
                if let Some((field_name, type_text)) = extract_field_decl(&child, source) {
                    fields.insert(field_name, type_text);
                }
            }
        }
    }
    fields
}

/// Extract (field_name, type_text) from a single `field_declaration` node.
///
/// A field_declaration looks like: `type_specifiers declarator ;`
/// e.g., `unsigned int flags;` or `char *name;` or `struct Inner *inner;`
fn extract_field_decl(node: &Node, source: &str) -> Option<(String, String)> {
    // Collect type specifier text (everything before the declarator)
    let mut type_parts = Vec::new();
    let mut field_name = None;
    let mut has_pointer = false;

    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            match child.kind() {
                "type_qualifier"
                | "primitive_type"
                | "sized_type_specifier"
                | "struct_specifier"
                | "enum_specifier"
                | "union_specifier"
                | "type_identifier" => {
                    type_parts.push(child.utf8_text(source.as_bytes()).unwrap_or("").to_string());
                }
                "field_identifier" => {
                    field_name = Some(child.utf8_text(source.as_bytes()).unwrap_or("").to_string());
                }
                "pointer_declarator" => {
                    has_pointer = true;
                    // Extract field_identifier from inside pointer_declarator
                    field_name = extract_field_id_from_declarator(&child, source);
                }
                "array_declarator" => {
                    // e.g., `char name[64];` — extract field_identifier
                    field_name = extract_field_id_from_declarator(&child, source);
                }
                "function_declarator" => {
                    // Function pointer fields — skip for type resolution purposes
                    return None;
                }
                _ => {}
            }
        }
    }

    let name = field_name?;
    if name.is_empty() || type_parts.is_empty() {
        return None;
    }

    let mut type_text = type_parts.join(" ");
    if has_pointer {
        type_text.push_str(" *");
    }

    Some((name, type_text))
}

/// Extract field_identifier from a declarator chain (pointer_declarator, array_declarator).
fn extract_field_id_from_declarator(node: &Node, source: &str) -> Option<String> {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            match child.kind() {
                "field_identifier" => {
                    return Some(child.utf8_text(source.as_bytes()).unwrap_or("").to_string());
                }
                "pointer_declarator" | "array_declarator" => {
                    return extract_field_id_from_declarator(&child, source);
                }
                _ => {}
            }
        }
    }
    None
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
    needs_vra: bool,
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
                // Collect macro constants and aliases from resolved headers
                let header_macros = const_eval::collect_macro_constants(&root, &hsource);
                context.macro_constants.extend(header_macros.clone());

                let file_summaries =
                    function_summary::compute_summaries(&root, &hsource, &header_macros, needs_vra);
                for (name, summary) in file_summaries {
                    context.function_summaries.insert(name, summary);
                }
                let header_aliases = const_eval::collect_macro_aliases(&root, &hsource);
                context.macro_aliases.extend(header_aliases);

                // Collect struct field types from resolved headers
                collect_struct_definitions(&root, &hsource, &mut context.struct_field_types);

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

#[cfg(test)]
mod tests {
    use super::*;

    fn parse_c(code: &str) -> (tree_sitter::Tree, String) {
        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        (tree, code.to_string())
    }

    // -- collect_function_names --

    #[test]
    fn test_collect_function_names_definitions() {
        let code = "void foo(void) {} int bar(int x) { return x; }";
        let (tree, source) = parse_c(code);
        let mut names = HashSet::new();
        collect_function_names(&tree.root_node(), &source, &mut names);
        assert!(names.contains("foo"));
        assert!(names.contains("bar"));
    }

    #[test]
    fn test_collect_function_names_declarations() {
        let code = "void foo(void); int bar(int x);";
        let (tree, source) = parse_c(code);
        let mut names = HashSet::new();
        collect_function_names(&tree.root_node(), &source, &mut names);
        assert!(names.contains("foo"));
        assert!(names.contains("bar"));
    }

    #[test]
    fn test_collect_function_names_pointer_return() {
        let code = "char *strdup(const char *s); int *get_ptr(void) { return 0; }";
        let (tree, source) = parse_c(code);
        let mut names = HashSet::new();
        collect_function_names(&tree.root_node(), &source, &mut names);
        assert!(names.contains("strdup"));
        assert!(names.contains("get_ptr"));
    }

    #[test]
    fn test_collect_function_names_inside_ifdef() {
        let code = "#ifdef FEATURE\nvoid guarded(void) {}\n#endif\n";
        let (tree, source) = parse_c(code);
        let mut names = HashSet::new();
        collect_function_names(&tree.root_node(), &source, &mut names);
        assert!(names.contains("guarded"));
    }

    #[test]
    fn test_collect_function_names_macro() {
        let code = "#define MY_FUNC(x) ((x) + 1)\n";
        let (tree, source) = parse_c(code);
        let mut names = HashSet::new();
        collect_function_names(&tree.root_node(), &source, &mut names);
        assert!(names.contains("MY_FUNC"));
    }

    // -- collect_header_declarations --

    #[test]
    fn test_collect_header_declarations_skips_static() {
        let code = "void public_func(int x);\nstatic void internal(void);";
        let (tree, source) = parse_c(code);
        let mut names = HashSet::new();
        collect_header_declarations(&tree.root_node(), &source, &mut names);
        assert!(names.contains("public_func"));
        assert!(!names.contains("internal"));
    }

    // -- collect_call_graph --

    #[test]
    fn test_collect_call_graph() {
        let code = "void a(void) { b(); c(); } void b(void) { c(); }";
        let (tree, source) = parse_c(code);
        let mut graph = HashMap::new();
        collect_call_graph(&tree.root_node(), &source, &mut graph);
        assert!(graph.get("a").unwrap().contains("b"));
        assert!(graph.get("a").unwrap().contains("c"));
        assert!(graph.get("b").unwrap().contains("c"));
    }

    // -- collect_local_var_states --

    #[test]
    fn test_local_var_null() {
        let code = "void f(void) { int *p = NULL; }";
        let (tree, source) = parse_c(code);
        let func = tree.root_node().child(0).unwrap();
        let body = func.child_by_field_name("body").unwrap();
        let states = collect_local_var_states(&body, &source);
        assert_eq!(states.get("p"), Some(&NullState::DefinitelyNull));
    }

    #[test]
    fn test_local_var_string_literal() {
        let code = r#"void f(void) { char *s = "hello"; }"#;
        let (tree, source) = parse_c(code);
        let func = tree.root_node().child(0).unwrap();
        let body = func.child_by_field_name("body").unwrap();
        let states = collect_local_var_states(&body, &source);
        assert_eq!(states.get("s"), Some(&NullState::NotNull));
    }

    #[test]
    fn test_local_var_malloc() {
        let code = "void f(void) { void *p = malloc(10); }";
        let (tree, source) = parse_c(code);
        let func = tree.root_node().child(0).unwrap();
        let body = func.child_by_field_name("body").unwrap();
        let states = collect_local_var_states(&body, &source);
        assert_eq!(states.get("p"), Some(&NullState::PossiblyNull));
    }

    #[test]
    fn test_local_var_stack_array_not_null() {
        // Stack arrays are always non-null
        let code = "void f(void) { char buf[64]; }";
        let (tree, source) = parse_c(code);
        let func = tree.root_node().child(0).unwrap();
        let body = func.child_by_field_name("body").unwrap();
        let states = collect_local_var_states(&body, &source);
        assert_eq!(states.get("buf"), Some(&NullState::NotNull));
    }

    // -- collect_early_return_null_guards --

    #[test]
    fn test_early_return_null_guard() {
        let code = "void f(int *p) { if (p == NULL) return; *p = 1; }";
        let (tree, source) = parse_c(code);
        let func = tree.root_node().child(0).unwrap();
        let body = func.child_by_field_name("body").unwrap();
        let mut states = HashMap::new();
        collect_early_return_null_guards(&body, &source, &mut states);
        assert_eq!(states.get("p"), Some(&NullState::NotNull));
    }

    #[test]
    fn test_early_return_bang_guard() {
        let code = "void f(int *p) { if (!p) return; *p = 1; }";
        let (tree, source) = parse_c(code);
        let func = tree.root_node().child(0).unwrap();
        let body = func.child_by_field_name("body").unwrap();
        let mut states = HashMap::new();
        collect_early_return_null_guards(&body, &source, &mut states);
        assert_eq!(states.get("p"), Some(&NullState::NotNull));
    }

    // -- is_simple_identifier --

    #[test]
    fn test_is_simple_identifier() {
        assert!(is_simple_identifier("foo"));
        assert!(is_simple_identifier("_bar"));
        assert!(is_simple_identifier("baz123"));
        assert!(!is_simple_identifier(""));
        assert!(!is_simple_identifier("123abc"));
        assert!(!is_simple_identifier("a b"));
        assert!(!is_simple_identifier("a+b"));
    }

    // -- collect_struct_definitions --

    #[test]
    fn test_struct_named() {
        let code = "struct Point { int x; int y; };";
        let (tree, source) = parse_c(code);
        let mut fields = HashMap::new();
        collect_struct_definitions(&tree.root_node(), &source, &mut fields);
        let point = fields.get("Point").expect("Point not found");
        assert_eq!(point.get("x"), Some(&"int".to_string()));
        assert_eq!(point.get("y"), Some(&"int".to_string()));
    }

    #[test]
    fn test_struct_typedef() {
        let code = "typedef struct { char *name; int age; } Person;";
        let (tree, source) = parse_c(code);
        let mut fields = HashMap::new();
        collect_struct_definitions(&tree.root_node(), &source, &mut fields);
        let person = fields.get("Person").expect("Person not found");
        assert!(person.contains_key("name"));
        assert!(person.contains_key("age"));
    }

    // -- extract_include_directives --

    #[test]
    fn test_extract_includes() {
        let code = "#include <stdio.h>\n#include \"myheader.h\"\n";
        let (tree, source) = parse_c(code);
        let dirs = extract_include_directives(&tree.root_node(), &source);
        assert!(dirs.contains(&"stdio.h".to_string()));
        assert!(dirs.contains(&"myheader.h".to_string()));
    }

    #[test]
    fn test_extract_includes_conditional() {
        let code = "#ifdef HAVE_FOO\n#include \"foo.h\"\n#endif\n";
        let (tree, source) = parse_c(code);
        let dirs = extract_include_directives(&tree.root_node(), &source);
        assert!(dirs.contains(&"foo.h".to_string()));
    }

    // -- resolve_header --

    #[test]
    fn test_resolve_header_source_dir() {
        let dir = tempfile::TempDir::new().unwrap();
        let header = dir.path().join("test.h");
        std::fs::File::create(&header).unwrap();
        let result = resolve_header("test.h", Some(dir.path()), &[]);
        assert!(result.is_some());
    }

    #[test]
    fn test_resolve_header_include_path() {
        let dir = tempfile::TempDir::new().unwrap();
        let header = dir.path().join("sys.h");
        std::fs::File::create(&header).unwrap();
        let search = vec![dir.path().to_string_lossy().to_string()];
        let result = resolve_header("sys.h", None, &search);
        assert!(result.is_some());
    }

    #[test]
    fn test_resolve_header_not_found() {
        assert!(resolve_header("nonexistent.h", None, &[]).is_none());
    }

    // -- aggregate_callsite_null_states --

    #[test]
    fn test_aggregate_all_not_null() {
        let mut summaries = HashMap::new();
        summaries.insert(
            "sink".to_string(),
            FunctionSummary {
                dereferences_params: vec![0].into_iter().collect(),
                ..Default::default()
            },
        );
        let callsite_args = HashMap::from([(
            "sink".to_string(),
            vec![vec![NullState::NotNull], vec![NullState::NotNull]],
        )]);
        aggregate_callsite_null_states(&callsite_args, &mut summaries, &HashSet::new());
        assert_eq!(
            summaries
                .get("sink")
                .unwrap()
                .callsite_param_null_states
                .get(&0),
            Some(&NullState::NotNull)
        );
    }

    #[test]
    fn test_aggregate_any_null_produces_possibly_null() {
        let mut summaries = HashMap::new();
        summaries.insert(
            "sink".to_string(),
            FunctionSummary {
                dereferences_params: vec![0].into_iter().collect(),
                ..Default::default()
            },
        );
        let callsite_args = HashMap::from([(
            "sink".to_string(),
            vec![vec![NullState::NotNull], vec![NullState::DefinitelyNull]],
        )]);
        aggregate_callsite_null_states(&callsite_args, &mut summaries, &HashSet::new());
        assert_eq!(
            summaries
                .get("sink")
                .unwrap()
                .callsite_param_null_states
                .get(&0),
            Some(&NullState::PossiblyNull)
        );
    }

    // -- collect_callsite_args_from_tree --

    #[test]
    fn test_callsite_args_null() {
        let code = "void caller(void) { sink(NULL); }";
        let (tree, source) = parse_c(code);
        let mut args = HashMap::new();
        collect_callsite_args_from_tree(&tree.root_node(), &source, &mut args);
        assert_eq!(args.get("sink").unwrap()[0][0], NullState::DefinitelyNull);
    }

    #[test]
    fn test_callsite_args_local_var() {
        let code = r#"void caller(void) { char *s = "hi"; sink(s); }"#;
        let (tree, source) = parse_c(code);
        let mut args = HashMap::new();
        collect_callsite_args_from_tree(&tree.root_node(), &source, &mut args);
        assert_eq!(args.get("sink").unwrap()[0][0], NullState::NotNull);
    }

    // -- prescan_directories integration --

    #[test]
    fn test_prescan_directories_basic() {
        let dir = tempfile::TempDir::new().unwrap();
        std::fs::write(dir.path().join("a.c"), "void func_a(void) { func_b(); }").unwrap();
        std::fs::write(dir.path().join("b.c"), "void func_b(void) {}").unwrap();
        let dirs = vec![dir.path().to_string_lossy().to_string()];
        let ctx = prescan_directories(&dirs, None, false).unwrap();
        assert!(ctx.known_functions.contains("func_a"));
        assert!(ctx.known_functions.contains("func_b"));
        assert!(ctx.call_graph.get("func_a").unwrap().contains("func_b"));
    }

    #[test]
    fn test_prescan_directories_with_header() {
        let dir = tempfile::TempDir::new().unwrap();
        std::fs::write(dir.path().join("api.h"), "int public_api(int x);").unwrap();
        std::fs::write(
            dir.path().join("impl.c"),
            "int public_api(int x) { return x + 1; }",
        )
        .unwrap();
        let dirs = vec![dir.path().to_string_lossy().to_string()];
        let ctx = prescan_directories(&dirs, None, false).unwrap();
        assert!(ctx.known_functions.contains("public_api"));
        assert!(ctx.header_declared_functions.contains("public_api"));
    }

    #[test]
    fn test_prescan_directories_struct() {
        let dir = tempfile::TempDir::new().unwrap();
        std::fs::write(
            dir.path().join("types.c"),
            "struct Config { int timeout; char *host; };",
        )
        .unwrap();
        let dirs = vec![dir.path().to_string_lossy().to_string()];
        let ctx = prescan_directories(&dirs, None, false).unwrap();
        assert!(ctx
            .struct_field_types
            .get("Config")
            .unwrap()
            .contains_key("timeout"));
    }
}

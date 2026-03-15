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
    let mut macro_aliases = HashMap::new();
    let mut struct_field_types = HashMap::new();
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

                // Collect macro aliases (#define ALIAS identifier)
                let file_aliases = const_eval::collect_macro_aliases(&root, &source);
                macro_aliases.extend(file_aliases);

                // Collect struct field types from struct definitions
                collect_struct_definitions(&root, &source, &mut struct_field_types);

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

    // Second pass: propagate parameter null states through relay functions.
    // After the first aggregation, functions have callsite_param_null_states.
    // Re-collect callsite args using these param states to resolve parameter forwarding:
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
                    // Also recurse into body for nested function names
                    collect_function_names(&child, source, names);
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
fn propagate_param_null_states(
    source_files: &[PathBuf],
    parser: &mut CParser,
    summaries: &mut HashMap<String, FunctionSummary>,
    callsite_args: &mut HashMap<String, Vec<Vec<NullState>>>,
    header_declared: &HashSet<String>,
) {
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

    // Only proceed if the second pass found any new information
    if new_callsite_args.is_empty() {
        return;
    }

    // Merge new callsite args into the existing ones
    for (callee, arg_vecs) in new_callsite_args {
        callsite_args.entry(callee).or_default().extend(arg_vecs);
    }

    // Clear old aggregated states and re-aggregate with the merged data
    for summary in summaries.values_mut() {
        summary.callsite_param_null_states.clear();
    }
    aggregate_callsite_null_states(callsite_args, summaries, header_declared);
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

                // Collect macro constants and aliases from resolved headers
                let header_macros = const_eval::collect_macro_constants(&root, &hsource);
                context.macro_constants.extend(header_macros);
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

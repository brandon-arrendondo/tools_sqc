//! Null-state forward dataflow analysis using the CFG.
//!
//! Computes the null/non-null state of pointer variables at every point in a
//! function. Used by EXP34-C to detect null pointer dereferences with proper
//! flow sensitivity through branches, loops, and early returns.

use super::cfg::{BasicBlock, BlockId, CfgEdge, FunctionCfg};
use super::dataflow::find_node_at_range;
use crate::analyze::function_summary::FunctionSummary;
use std::collections::{HashMap, HashSet, VecDeque};
use tree_sitter::Node;

// ---------------------------------------------------------------------------
// Null lattice
// ---------------------------------------------------------------------------

/// Null state for a single pointer variable.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum NullState {
    /// No information (bottom of lattice).
    Unknown,
    /// Definitely assigned NULL / 0 / nullptr.
    DefinitelyNull,
    /// May or may not be null (merged paths, malloc return, etc.).
    PossiblyNull,
    /// Known non-null (checked, assigned &var, assigned non-null literal, etc.).
    NotNull,
}

impl NullState {
    /// Lattice join: merge two states from converging paths.
    fn join(self, other: NullState) -> NullState {
        use NullState::*;
        if self == other {
            return self;
        }
        match (self, other) {
            (Unknown, x) | (x, Unknown) => x,
            _ => PossiblyNull,
        }
    }

    /// Returns true if dereference at this state is potentially unsafe.
    pub fn is_unsafe(self) -> bool {
        matches!(self, NullState::DefinitelyNull | NullState::PossiblyNull)
    }
}

/// State map: variable name -> NullState.
type StateMap = HashMap<String, NullState>;

/// Join two state maps (union of keys, lattice join per key).
fn join_states(a: &StateMap, b: &StateMap) -> StateMap {
    let mut result = a.clone();
    for (var, &state_b) in b {
        let entry = result.entry(var.clone()).or_insert(NullState::Unknown);
        *entry = entry.join(state_b);
    }
    result
}

// ---------------------------------------------------------------------------
// Analysis result
// ---------------------------------------------------------------------------

/// Result of null-state analysis for one function.
pub struct NullAnalysisResult {
    /// Entry state for each block (after joining predecessors + edge refinement).
    pub block_entry_states: HashMap<BlockId, StateMap>,
    /// Exit state for each block (after simulating block statements).
    pub block_exit_states: HashMap<BlockId, StateMap>,
    /// Set of variables declared as pointer types.
    pub declared_pointers: HashSet<String>,
}

// ---------------------------------------------------------------------------
// Edge refinement (condition parsing)
// ---------------------------------------------------------------------------

/// Information extracted from a condition for edge refinement.
struct ConditionInfo {
    /// Variable being checked.
    var_name: String,
    /// State on the true-branch edge.
    true_state: NullState,
    /// State on the false-branch edge.
    false_state: NullState,
}

/// Parse a condition AST node to extract null-check info.
/// Returns None if the condition is not a recognizable null check.
fn parse_null_condition(node: &Node, source: &str) -> Option<ConditionInfo> {
    match node.kind() {
        "parenthesized_expression" => {
            // Unwrap parens: child(0)='(', child(1)=expr, child(2)=')'
            node.child(1)
                .and_then(|inner| parse_null_condition(&inner, source))
        }
        "binary_expression" => {
            let left = node.child_by_field_name("left")?;
            let operator = node.child_by_field_name("operator")?;
            let right = node.child_by_field_name("right")?;
            let op = get_text(&operator, source);

            match op.as_str() {
                "==" => {
                    // ptr == NULL  => true: DefinitelyNull, false: NotNull
                    // NULL == ptr  => same
                    if let Some(var) = extract_null_check_var(&left, &right, source) {
                        return Some(ConditionInfo {
                            var_name: var,
                            true_state: NullState::DefinitelyNull,
                            false_state: NullState::NotNull,
                        });
                    }
                }
                "!=" => {
                    // ptr != NULL  => true: NotNull, false: DefinitelyNull
                    if let Some(var) = extract_null_check_var(&left, &right, source) {
                        return Some(ConditionInfo {
                            var_name: var,
                            true_state: NullState::NotNull,
                            false_state: NullState::DefinitelyNull,
                        });
                    }
                }
                "&&" | "||" => {
                    // Try to find a null check in either operand
                    if let Some(info) = parse_null_condition(&left, source) {
                        return Some(info);
                    }
                    return parse_null_condition(&right, source);
                }
                _ => {}
            }
            None
        }
        "unary_expression" => {
            // !ptr => true: DefinitelyNull, false: NotNull
            let operator = node.child(0)?;
            if get_text(&operator, source) == "!" {
                let arg = node.child_by_field_name("argument")?;
                if arg.kind() == "identifier" {
                    return Some(ConditionInfo {
                        var_name: get_text(&arg, source),
                        true_state: NullState::DefinitelyNull,
                        false_state: NullState::NotNull,
                    });
                }
            }
            None
        }
        "identifier" => {
            // if (ptr) => true: NotNull, false: DefinitelyNull
            Some(ConditionInfo {
                var_name: get_text(node, source),
                true_state: NullState::NotNull,
                false_state: NullState::DefinitelyNull,
            })
        }
        _ => None,
    }
}

/// Given left and right operands of == or !=, extract the variable name
/// if one side is NULL and the other is an identifier.
fn extract_null_check_var(left: &Node, right: &Node, source: &str) -> Option<String> {
    let lt = get_text(left, source);
    let rt = get_text(right, source);
    if is_null_value(&rt) && left.kind() == "identifier" {
        Some(lt)
    } else if is_null_value(&lt) && right.kind() == "identifier" {
        Some(rt)
    } else {
        None
    }
}

// ---------------------------------------------------------------------------
// Transfer function (per-block simulation)
// ---------------------------------------------------------------------------

/// Simulate a single block's statements on the given entry state,
/// returning the exit state.
fn apply_transfer(
    block: &BasicBlock,
    entry: &StateMap,
    body_node: &Node,
    source: &str,
    declared_pointers: &mut HashSet<String>,
    summaries: &HashMap<String, FunctionSummary>,
) -> StateMap {
    let mut state = entry.clone();
    for &(start, end) in &block.statements {
        if let Some(stmt_node) = find_node_at_range(body_node, start, end) {
            process_statement_for_null_state(
                &stmt_node,
                source,
                &mut state,
                declared_pointers,
                summaries,
            );
        }
    }
    state
}

/// Process a single statement/expression, updating null state.
fn process_statement_for_null_state(
    node: &Node,
    source: &str,
    state: &mut StateMap,
    declared_pointers: &mut HashSet<String>,
    summaries: &HashMap<String, FunctionSummary>,
) {
    match node.kind() {
        "declaration" => {
            process_declaration_null(node, source, state, declared_pointers, summaries);
        }
        "expression_statement" => {
            // Handle assert(var) before other expression processing
            process_assert_for_null_state(node, source, state);
            if let Some(expr) = node.child(0) {
                process_expression_null(&expr, source, state, declared_pointers, summaries);
            }
        }
        "assignment_expression" => {
            process_expression_null(node, source, state, declared_pointers, summaries);
        }
        // Condition expressions (parenthesized_expression at top level of if/while)
        // are added as statements in the condition block. We don't mutate null state
        // from conditions — that's handled by edge refinement.
        _ => {
            // Recognize assert(var) / assert(var != NULL) as making var NotNull
            process_assert_for_null_state(node, source, state);
            // Recurse into compound expressions to find nested assignments
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() == "assignment_expression" {
                        process_expression_null(
                            &child,
                            source,
                            state,
                            declared_pointers,
                            summaries,
                        );
                    }
                }
            }
        }
    }
}

fn process_declaration_null(
    node: &Node,
    source: &str,
    state: &mut StateMap,
    declared_pointers: &mut HashSet<String>,
    summaries: &HashMap<String, FunctionSummary>,
) {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if child.kind() == "init_declarator" {
                if let Some(declarator) = child.child_by_field_name("declarator") {
                    let var_name = get_identifier_from_declarator(&declarator, source);
                    if var_name.is_empty() {
                        continue;
                    }
                    let is_ptr = is_pointer_declarator(&declarator) && !contains_array(&declarator);

                    if is_ptr {
                        declared_pointers.insert(var_name.clone());
                    }

                    if let Some(value) = child.child_by_field_name("value") {
                        if is_ptr {
                            let rval = classify_rvalue_null(&value, source, summaries);
                            // Propagate from another variable: Node *current = head;
                            if rval == NullState::NotNull && value.kind() == "identifier" {
                                let src_name = get_text(&value, source);
                                if let Some(&src_state) = state.get(&src_name) {
                                    state.insert(var_name, src_state);
                                    continue;
                                }
                            }
                            // Propagate from field access: ptr = other->next
                            if rval == NullState::NotNull && value.kind() == "field_expression" {
                                if let Some(arg) = value.child_by_field_name("argument") {
                                    let base = get_text(&arg, source);
                                    if let Some(&base_state) = state.get(&base) {
                                        if base_state.is_unsafe() {
                                            state.insert(var_name, NullState::PossiblyNull);
                                            continue;
                                        }
                                    }
                                }
                            }
                            state.insert(var_name, rval);
                        }
                    } else if is_ptr {
                        // Uninitialized pointer
                        state.insert(var_name, NullState::PossiblyNull);
                    }
                }
            } else if child.kind() == "pointer_declarator" || child.kind() == "identifier" {
                // Bare uninitialized pointer: "int *ptr;"
                let var_name = get_identifier_from_declarator(&child, source);
                if !var_name.is_empty() && is_pointer_declarator(&child) && !contains_array(&child)
                {
                    declared_pointers.insert(var_name.clone());
                    state.insert(var_name, NullState::PossiblyNull);
                }
            }
        }
    }
}

fn process_expression_null(
    node: &Node,
    source: &str,
    state: &mut StateMap,
    declared_pointers: &HashSet<String>,
    summaries: &HashMap<String, FunctionSummary>,
) {
    if node.kind() == "assignment_expression" {
        if let (Some(left), Some(right)) = (
            node.child_by_field_name("left"),
            node.child_by_field_name("right"),
        ) {
            let left_name = get_text(&left, source);
            let left_is_ptr = left.kind() != "identifier" || declared_pointers.contains(&left_name);

            if !left_is_ptr {
                return;
            }

            let new_state = classify_rvalue_null(&right, source, summaries);
            // Propagation from another variable
            if new_state == NullState::NotNull && right.kind() == "identifier" {
                let right_name = get_text(&right, source);
                if let Some(&rhs_state) = state.get(&right_name) {
                    state.insert(left_name, rhs_state);
                    return;
                }
            }
            if new_state == NullState::NotNull && right.kind() == "field_expression" {
                // ptr = other->next: propagate other's state
                if let Some(arg) = right.child_by_field_name("argument") {
                    let base = get_text(&arg, source);
                    if let Some(&base_state) = state.get(&base) {
                        if base_state.is_unsafe() {
                            state.insert(left_name, NullState::PossiblyNull);
                            return;
                        }
                    }
                }
            }
            // Non-nullable function call clears null taint
            if right.kind() == "call_expression" && new_state == NullState::NotNull {
                state.insert(left_name, NullState::NotNull);
                return;
            }
            state.insert(left_name, new_state);
        }
    }
}

/// Recognize assert(var) or assert(var != NULL) and set var to NotNull.
fn process_assert_for_null_state(node: &Node, source: &str, state: &mut StateMap) {
    // Look for expression_statement -> call_expression -> assert
    let call_node = if node.kind() == "expression_statement" {
        node.child(0)
    } else if node.kind() == "call_expression" {
        Some(*node)
    } else {
        None
    };

    if let Some(call) = call_node {
        if call.kind() != "call_expression" {
            return;
        }
        if let Some(function) = call.child_by_field_name("function") {
            let func_name = get_text(&function, source);
            if func_name != "assert" {
                return;
            }
            if let Some(args) = call.child_by_field_name("arguments") {
                for i in 0..args.child_count() {
                    if let Some(arg) = args.child(i) {
                        if arg.kind() == "(" || arg.kind() == ")" || arg.kind() == "," {
                            continue;
                        }
                        // assert(var) — var is non-null after this
                        if arg.kind() == "identifier" {
                            let name = get_text(&arg, source);
                            state.insert(name, NullState::NotNull);
                            return;
                        }
                        // assert(var != NULL) or assert(NULL != var)
                        if arg.kind() == "binary_expression" {
                            if let (Some(left), Some(right)) = (
                                arg.child_by_field_name("left"),
                                arg.child_by_field_name("right"),
                            ) {
                                let lt = get_text(&left, source);
                                let rt = get_text(&right, source);
                                if is_null_value(rt.trim()) && left.kind() == "identifier" {
                                    state.insert(lt, NullState::NotNull);
                                } else if is_null_value(lt.trim()) && right.kind() == "identifier" {
                                    state.insert(rt, NullState::NotNull);
                                }
                            }
                            return;
                        }
                    }
                }
            }
        }
    }
}

/// Classify the null state resulting from an rvalue expression.
fn classify_rvalue_null(
    node: &Node,
    source: &str,
    summaries: &HashMap<String, FunctionSummary>,
) -> NullState {
    let text = get_text(node, source);
    let trimmed = text.trim();

    // NULL/0/nullptr
    if is_null_value(trimmed) {
        return NullState::DefinitelyNull;
    }

    // Cast to NULL: (type*)NULL
    if node.kind() == "cast_expression" {
        if let Some(value) = node.child_by_field_name("value") {
            let vt = get_text(&value, source);
            if is_null_value(vt.trim()) {
                return NullState::DefinitelyNull;
            }
        }
    }

    // Nullable function call
    if node.kind() == "call_expression" {
        if let Some(function) = node.child_by_field_name("function") {
            let func_name = get_text(&function, source);
            if is_nullable_function(&func_name, summaries) {
                return NullState::PossiblyNull;
            }
        }
    }

    // Cast wrapping a nullable call
    if node.kind() == "cast_expression" {
        if let Some(value) = node.child_by_field_name("value") {
            if value.kind() == "call_expression" {
                if let Some(function) = value.child_by_field_name("function") {
                    let func_name = get_text(&function, source);
                    if is_nullable_function(&func_name, summaries) {
                        return NullState::PossiblyNull;
                    }
                }
            }
        }
    }

    // Address-of is always non-null
    if node.kind() == "pointer_expression" {
        if let Some(op) = node.child_by_field_name("operator") {
            if get_text(&op, source) == "&" {
                return NullState::NotNull;
            }
        }
    }

    // String literal is always non-null
    if node.kind() == "string_literal" {
        return NullState::NotNull;
    }

    NullState::NotNull
}

// ---------------------------------------------------------------------------
// Forward dataflow (worklist algorithm)
// ---------------------------------------------------------------------------

/// Run null-state forward dataflow on a function CFG.
///
/// `func_node` is the `function_definition` AST node (for param extraction).
/// `source` is the full source text.
/// `summaries` are inter-procedural function summaries.
pub fn analyze_null_states(
    cfg: &FunctionCfg,
    func_node: &Node,
    source: &str,
    summaries: &HashMap<String, FunctionSummary>,
) -> NullAnalysisResult {
    let body = match func_node.child_by_field_name("body") {
        Some(b) => b,
        None => {
            return NullAnalysisResult {
                block_entry_states: HashMap::new(),
                block_exit_states: HashMap::new(),
                declared_pointers: HashSet::new(),
            }
        }
    };

    let mut declared_pointers = HashSet::new();

    // Initialize entry state: pointer params -> PossiblyNull
    let mut initial_state = StateMap::new();
    if let Some(declarator) = func_node.child_by_field_name("declarator") {
        collect_param_pointer_state(
            &declarator,
            source,
            &mut initial_state,
            &mut declared_pointers,
        );
    }

    let mut entry_states: HashMap<BlockId, StateMap> = HashMap::new();
    let mut exit_states: HashMap<BlockId, StateMap> = HashMap::new();

    // Initialize all blocks
    for block in &cfg.blocks {
        entry_states.insert(block.id, StateMap::new());
        exit_states.insert(block.id, StateMap::new());
    }

    // Entry block gets initial state
    entry_states.insert(cfg.entry, initial_state.clone());
    let entry_exit = apply_transfer(
        &cfg.blocks[cfg.entry],
        &initial_state,
        &body,
        source,
        &mut declared_pointers,
        summaries,
    );
    exit_states.insert(cfg.entry, entry_exit);

    // Worklist
    let mut worklist: VecDeque<BlockId> = VecDeque::new();
    for (succ, _) in cfg.successors(cfg.entry) {
        worklist.push_back(succ);
    }

    let mut iterations = 0;
    const MAX_ITERATIONS: usize = 500;

    while let Some(block_id) = worklist.pop_front() {
        iterations += 1;
        if iterations > MAX_ITERATIONS * cfg.blocks.len() {
            break;
        }

        // Join predecessor exit states with edge refinement
        let preds = cfg.predecessors(block_id);
        let mut new_entry = StateMap::new();
        let mut first = true;

        for (pred_id, edge_kind) in &preds {
            let pred_exit = exit_states.get(pred_id).cloned().unwrap_or_default();

            // Apply edge refinement from condition
            let refined =
                apply_edge_refinement(&pred_exit, *pred_id, edge_kind, cfg, &body, source);

            if first {
                new_entry = refined;
                first = false;
            } else {
                new_entry = join_states(&new_entry, &refined);
            }
        }

        if first {
            // No predecessors (unreachable block)
            continue;
        }

        // Compute exit state
        let block = &cfg.blocks[block_id];
        let new_exit = apply_transfer(
            block,
            &new_entry,
            &body,
            source,
            &mut declared_pointers,
            summaries,
        );

        // Check convergence
        let old_exit = exit_states.get(&block_id);
        if old_exit.map_or(true, |old| *old != new_exit) {
            entry_states.insert(block_id, new_entry);
            exit_states.insert(block_id, new_exit);

            // Add successors to worklist
            for (succ, _) in cfg.successors(block_id) {
                if !worklist.contains(&succ) {
                    worklist.push_back(succ);
                }
            }
        } else {
            entry_states.insert(block_id, new_entry);
        }
    }

    NullAnalysisResult {
        block_entry_states: entry_states,
        block_exit_states: exit_states,
        declared_pointers,
    }
}

/// Apply edge refinement: given a predecessor's exit state and the edge type,
/// refine the state based on the predecessor's condition.
fn apply_edge_refinement(
    pred_exit: &StateMap,
    pred_id: BlockId,
    edge_kind: &CfgEdge,
    cfg: &FunctionCfg,
    body: &Node,
    source: &str,
) -> StateMap {
    let mut state = pred_exit.clone();

    // Only refine on TrueBranch/FalseBranch edges
    let is_true = matches!(edge_kind, CfgEdge::TrueBranch);
    let is_false = matches!(edge_kind, CfgEdge::FalseBranch);
    if !is_true && !is_false {
        return state;
    }

    // Get the condition range from the predecessor block
    let pred_block = match cfg.get_block(pred_id) {
        Some(b) => b,
        None => return state,
    };
    let (cond_start, cond_end) = match pred_block.condition_range {
        Some(r) => r,
        None => return state,
    };

    // Find the condition AST node
    let cond_node = match find_node_at_range(body, cond_start, cond_end) {
        Some(n) => n,
        None => return state,
    };

    // Parse condition for null-check info
    if let Some(info) = parse_null_condition(&cond_node, source) {
        let refined_state = if is_true {
            info.true_state
        } else {
            info.false_state
        };
        // Only refine if the variable is tracked
        if state.contains_key(&info.var_name) {
            state.insert(info.var_name, refined_state);
        }
    }

    state
}

// ---------------------------------------------------------------------------
// Dereference query
// ---------------------------------------------------------------------------

/// Check if dereferencing `var_name` at byte offset `deref_byte` is potentially unsafe.
///
/// Finds the block containing `deref_byte`, simulates from block entry up to
/// that point, and returns true if the variable is in an unsafe null state.
pub fn is_null_deref_at(
    result: &NullAnalysisResult,
    cfg: &FunctionCfg,
    body: &Node,
    source: &str,
    var_name: &str,
    deref_byte: usize,
    summaries: &HashMap<String, FunctionSummary>,
) -> bool {
    // Find which block contains this dereference
    let block = match find_block_containing(cfg, deref_byte) {
        Some(b) => b,
        None => return false, // Can't determine — be conservative
    };

    // Get entry state for this block
    let entry = match result.block_entry_states.get(&block.id) {
        Some(s) => s,
        None => return false,
    };

    // Simulate forward from entry through statements up to the dereference
    let mut state = entry.clone();
    let mut declared_pointers = result.declared_pointers.clone();

    for &(start, end) in &block.statements {
        // Stop before processing statements that come after the dereference
        if start >= deref_byte {
            break;
        }
        if let Some(stmt_node) = find_node_at_range(body, start, end) {
            process_statement_for_null_state(
                &stmt_node,
                source,
                &mut state,
                &mut declared_pointers,
                summaries,
            );
        }
    }

    // Check variable's state at dereference point
    match state.get(var_name) {
        Some(ns) => ns.is_unsafe(),
        None => false, // Unknown variable — not tracked as pointer
    }
}

/// Find the basic block whose byte range contains the given offset.
fn find_block_containing(cfg: &FunctionCfg, byte_offset: usize) -> Option<&BasicBlock> {
    // First try statement-level containment (more precise)
    for block in &cfg.blocks {
        for &(start, end) in &block.statements {
            if byte_offset >= start && byte_offset < end {
                return Some(block);
            }
        }
    }
    // Fallback to block byte range
    for block in &cfg.blocks {
        if block.byte_range.0 > 0
            && byte_offset >= block.byte_range.0
            && byte_offset < block.byte_range.1
        {
            return Some(block);
        }
    }
    None
}

// ---------------------------------------------------------------------------
// Parameter collection
// ---------------------------------------------------------------------------

fn collect_param_pointer_state(
    declarator: &Node,
    source: &str,
    state: &mut StateMap,
    declared_pointers: &mut HashSet<String>,
) {
    if declarator.kind() == "function_declarator" {
        if let Some(params) = declarator.child_by_field_name("parameters") {
            for i in 0..params.child_count() {
                if let Some(param) = params.child(i) {
                    if param.kind() == "parameter_declaration" {
                        let param_text = get_text(&param, source);
                        if let Some(param_decl) = param.child_by_field_name("declarator") {
                            let name = get_identifier_from_declarator(&param_decl, source);
                            if !name.is_empty()
                                && (is_pointer_declarator(&param_decl)
                                    || param_text.contains('*')
                                    || param_text.starts_with("FILE")
                                    || name.contains("callback"))
                            {
                                declared_pointers.insert(name.clone());
                                state.insert(name, NullState::PossiblyNull);
                            }
                        }
                    }
                }
            }
        }
    } else {
        for i in 0..declarator.child_count() {
            if let Some(child) = declarator.child(i) {
                collect_param_pointer_state(&child, source, state, declared_pointers);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Helper functions (shared with EXP34-C)
// ---------------------------------------------------------------------------

fn get_text(node: &Node, source: &str) -> String {
    source[node.start_byte()..node.end_byte()].to_string()
}

pub fn is_null_value(text: &str) -> bool {
    let t = text.trim();
    t == "NULL" || t == "0" || t == "nullptr"
}

pub fn is_nullable_function(func_name: &str, summaries: &HashMap<String, FunctionSummary>) -> bool {
    if let Some(summary) = summaries.get(func_name) {
        if summary.can_return_null {
            return true;
        }
    }
    matches!(
        func_name,
        "malloc"
            | "calloc"
            | "realloc"
            | "strstr"
            | "strchr"
            | "strrchr"
            | "fopen"
            | "fdopen"
            | "freopen"
            | "tmpfile"
            | "popen"
            | "getenv"
            | "setlocale"
            | "strtok"
            | "bsearch"
            | "fgets"
            | "gets"
            | "strdup"
            | "strndup"
            | "strpbrk"
            | "memchr"
            | "localtime"
            | "gmtime"
            | "asctime"
            | "ctime"
            | "create_int"
    )
}

pub fn is_cast_to_null(node: &Node, source: &str) -> bool {
    if node.kind() == "cast_expression" {
        if let Some(value) = node.child_by_field_name("value") {
            let vt = get_text(&value, source);
            return is_null_value(vt.trim());
        }
    }
    false
}

pub fn is_pointer_declarator(declarator: &Node) -> bool {
    match declarator.kind() {
        "pointer_declarator" => true,
        "array_declarator" => true,
        _ => {
            for i in 0..declarator.child_count() {
                if let Some(child) = declarator.child(i) {
                    if is_pointer_declarator(&child) {
                        return true;
                    }
                }
            }
            false
        }
    }
}

fn contains_array(node: &Node) -> bool {
    if node.kind() == "array_declarator" {
        return true;
    }
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if contains_array(&child) {
                return true;
            }
        }
    }
    false
}

fn get_identifier_from_declarator(declarator: &Node, source: &str) -> String {
    match declarator.kind() {
        "identifier" => get_text(declarator, source),
        "pointer_declarator" | "array_declarator" => {
            if let Some(inner) = declarator.child_by_field_name("declarator") {
                get_identifier_from_declarator(&inner, source)
            } else {
                String::new()
            }
        }
        _ => {
            for i in 0..declarator.child_count() {
                if let Some(child) = declarator.child(i) {
                    if child.kind() == "identifier" {
                        return get_text(&child, source);
                    }
                }
            }
            String::new()
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::analyze::cfg::build_function_cfg;

    fn analyze(code: &str) -> (FunctionCfg, NullAnalysisResult, tree_sitter::Tree, String) {
        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();
        let func = (0..root.child_count())
            .filter_map(|i| root.child(i))
            .find(|c| c.kind() == "function_definition")
            .unwrap();
        let cfg = build_function_cfg(&func, code).unwrap();
        let summaries = HashMap::new();
        let result = analyze_null_states(&cfg, &func, code, &summaries);
        (cfg, result, tree, code.to_string())
    }

    #[test]
    fn test_null_assigned_then_deref() {
        let code = r#"
void foo() {
    int *p = NULL;
    *p = 42;
}
"#;
        let (cfg, result, tree, source) = analyze(code);
        let root = tree.root_node();
        let func = root.child(0).unwrap();
        let body = func.child_by_field_name("body").unwrap();
        // Find dereference byte — "*p = 42"
        let deref_pos = source.find("*p = 42").unwrap();
        let summaries = HashMap::new();
        assert!(is_null_deref_at(
            &result, &cfg, &body, &source, "p", deref_pos, &summaries
        ));
    }

    #[test]
    fn test_null_check_before_deref() {
        let code = r#"
void foo() {
    int *p = NULL;
    if (p != NULL) {
        *p = 42;
    }
}
"#;
        let (cfg, result, tree, source) = analyze(code);
        let root = tree.root_node();
        let func = root.child(0).unwrap();
        let body = func.child_by_field_name("body").unwrap();
        let deref_pos = source.find("*p = 42").unwrap();
        let summaries = HashMap::new();
        assert!(!is_null_deref_at(
            &result, &cfg, &body, &source, "p", deref_pos, &summaries
        ));
    }

    #[test]
    fn test_early_return_after_null_check() {
        let code = r#"
int foo(int *p) {
    if (p == NULL) {
        return -1;
    }
    *p = 42;
    return 0;
}
"#;
        let (cfg, result, tree, source) = analyze(code);
        let root = tree.root_node();
        let func = root.child(0).unwrap();
        let body = func.child_by_field_name("body").unwrap();
        let deref_pos = source.find("*p = 42").unwrap();
        let summaries = HashMap::new();
        assert!(!is_null_deref_at(
            &result, &cfg, &body, &source, "p", deref_pos, &summaries
        ));
    }

    #[test]
    fn test_deref_inside_null_branch() {
        let code = r#"
void foo(int *p) {
    if (p == NULL) {
        *p = 42;
    }
}
"#;
        let (cfg, result, tree, source) = analyze(code);
        let root = tree.root_node();
        let func = root.child(0).unwrap();
        let body = func.child_by_field_name("body").unwrap();
        let deref_pos = source.find("*p = 42").unwrap();
        let summaries = HashMap::new();
        assert!(is_null_deref_at(
            &result, &cfg, &body, &source, "p", deref_pos, &summaries
        ));
    }

    #[test]
    fn test_malloc_with_check() {
        let code = r#"
void foo() {
    int *p = malloc(sizeof(int));
    if (p == NULL) {
        return;
    }
    *p = 42;
}
"#;
        let (cfg, result, tree, source) = analyze(code);
        let root = tree.root_node();
        let func = root.child(0).unwrap();
        let body = func.child_by_field_name("body").unwrap();
        let deref_pos = source.find("*p = 42").unwrap();
        let summaries = HashMap::new();
        assert!(!is_null_deref_at(
            &result, &cfg, &body, &source, "p", deref_pos, &summaries
        ));
    }

    #[test]
    fn test_malloc_without_check() {
        let code = r#"
void foo() {
    int *p = malloc(sizeof(int));
    *p = 42;
}
"#;
        let (cfg, result, tree, source) = analyze(code);
        let root = tree.root_node();
        let func = root.child(0).unwrap();
        let body = func.child_by_field_name("body").unwrap();
        let deref_pos = source.find("*p = 42").unwrap();
        let summaries = HashMap::new();
        assert!(is_null_deref_at(
            &result, &cfg, &body, &source, "p", deref_pos, &summaries
        ));
    }

    #[test]
    fn test_while_loop_guard() {
        let code = r#"
void foo(int *p) {
    while (p != NULL) {
        *p = 42;
        p = NULL;
    }
}
"#;
        let (cfg, result, tree, source) = analyze(code);
        let root = tree.root_node();
        let func = root.child(0).unwrap();
        let body = func.child_by_field_name("body").unwrap();
        let deref_pos = source.find("*p = 42").unwrap();
        let summaries = HashMap::new();
        assert!(!is_null_deref_at(
            &result, &cfg, &body, &source, "p", deref_pos, &summaries
        ));
    }
}

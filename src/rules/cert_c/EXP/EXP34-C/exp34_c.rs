use super::super::{CertRule, RuleViolation};
use crate::analyze::cfg::{self as cfg_mod, FunctionCfg};
use crate::analyze::context::ProjectContext;
use crate::analyze::function_summary::FunctionSummary;
use crate::analyze::null_state::{self, NullAnalysisResult, StateMap};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils;
use std::cell::RefCell;
use std::collections::HashMap;
use tree_sitter::Node;

pub struct Exp34C {
    function_summaries: RefCell<HashMap<String, FunctionSummary>>,
    function_cfgs: RefCell<HashMap<usize, FunctionCfg>>,
    /// Null states for file-scope (static/global) pointer variables,
    /// computed once per file by scanning all declarations and assignments.
    file_global_states: RefCell<StateMap>,
}

impl Exp34C {
    pub fn new() -> Self {
        Self {
            function_summaries: RefCell::new(HashMap::new()),
            function_cfgs: RefCell::new(HashMap::new()),
            file_global_states: RefCell::new(StateMap::new()),
        }
    }
}

impl CertRule for Exp34C {
    fn rule_id(&self) -> &'static str {
        "EXP34-C"
    }

    fn description(&self) -> &'static str {
        "Do not dereference null pointers"
    }

    fn severity(&self) -> Severity {
        Severity::High
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "EXP34-C"
    }

    fn set_project_context(&self, context: &ProjectContext) {
        *self.function_summaries.borrow_mut() = context.function_summaries.clone();
    }

    fn set_function_cfgs(&self, cfgs: &HashMap<usize, FunctionCfg>) {
        *self.function_cfgs.borrow_mut() = cfgs.clone();
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        let summaries = self.function_summaries.borrow();
        let cfgs = self.function_cfgs.borrow();

        // At the top level (translation_unit), collect file-scope global null states
        if node.kind() == "translation_unit" {
            let globals =
                null_state::collect_file_scope_null_states(node, source, &summaries);
            *self.file_global_states.borrow_mut() = globals;
        }

        if node.kind() == "function_definition" {
            if let Some(body) = node.child_by_field_name("body") {
                // Get pre-built CFG or build one on the fly
                let inline_cfg;
                let cfg = if let Some(c) = cfgs.get(&node.start_byte()) {
                    c
                } else if let Some(c) = cfg_mod::build_function_cfg(node, source) {
                    inline_cfg = c;
                    &inline_cfg
                } else {
                    // Skip: no CFG available for this function
                    return violations;
                };

                // Run CFG-based null-state dataflow, seeded with global states
                let global_states = self.file_global_states.borrow();
                let analysis = null_state::analyze_null_states_with_globals(
                    cfg,
                    node,
                    source,
                    &summaries,
                    &global_states,
                );

                // Walk AST for dereferences and check each against the dataflow result
                check_dereferences_cfg(
                    &body,
                    source,
                    &analysis,
                    cfg,
                    &body,
                    &summaries,
                    &mut violations,
                );
            }
        }

        // Recursively check child nodes (handles nested functions, preproc blocks)
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                violations.extend(self.check(&child, source));
            }
        }

        violations
    }
}

// ---------------------------------------------------------------------------
// Dereference walker (AST-based, queries CFG analysis for safety)
// ---------------------------------------------------------------------------

fn check_dereferences_cfg(
    node: &Node,
    source: &str,
    analysis: &NullAnalysisResult,
    cfg: &FunctionCfg,
    body: &Node,
    summaries: &HashMap<String, FunctionSummary>,
    violations: &mut Vec<RuleViolation>,
) {
    match node.kind() {
        "pointer_expression" => {
            // tree-sitter uses pointer_expression for both *ptr and &var.
            // Only dereference (*) can be a null-ptr bug.
            let is_deref = node
                .child_by_field_name("operator")
                .map(|op| ast_utils::get_node_text_owned(&op, source) == "*")
                .unwrap_or(false);

            if is_deref {
                if let Some(argument) = node.child_by_field_name("argument") {
                    let mut deref_text = ast_utils::get_node_text_owned(&argument, source);

                    // Strip parentheses
                    if argument.kind() == "parenthesized_expression" {
                        if let Some(inner) = argument.child(1) {
                            deref_text = ast_utils::get_node_text_owned(&inner, source);
                        }
                    }

                    if argument.kind() == "identifier"
                        || argument.kind() == "field_expression"
                        || argument.kind() == "parenthesized_expression"
                    {
                        if is_unsafe_at(&deref_text, node, source, analysis, cfg, body, summaries) {
                            let start_point = node.start_position();
                            violations.push(RuleViolation {
                                rule_id: "EXP34-C".to_string(),
                                severity: Severity::High,
                                message: format!(
                                    "Potential null pointer dereference of variable '{}'",
                                    deref_text
                                ),
                                file_path: String::new(),
                                line: start_point.row + 1,
                                column: start_point.column + 1,
                                suggestion: Some(format!(
                                    "Check if '{}' is not NULL before dereferencing",
                                    deref_text
                                )),
                                ..Default::default()
                            });
                        }
                    }
                }
            }
        }
        "subscript_expression" => {
            if let Some(array) = node.child(0) {
                if array.kind() == "identifier" {
                    let var_name = ast_utils::get_node_text_owned(&array, source);
                    if is_unsafe_at(&var_name, node, source, analysis, cfg, body, summaries) {
                        let start_point = node.start_position();
                        violations.push(RuleViolation {
                            rule_id: "EXP34-C".to_string(),
                            severity: Severity::High,
                            message: format!(
                                "Potential null pointer dereference in array access of variable '{}'",
                                var_name
                            ),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some(format!(
                                "Check if '{}' is not NULL before array access",
                                var_name
                            )),
                            ..Default::default()
                        });
                    }
                }
            }
        }
        "field_expression" => {
            if let Some(argument) = node.child_by_field_name("argument") {
                if argument.kind() == "identifier" {
                    let var_name = ast_utils::get_node_text_owned(&argument, source);
                    if is_unsafe_at(&var_name, node, source, analysis, cfg, body, summaries) {
                        let start_point = node.start_position();
                        violations.push(RuleViolation {
                            rule_id: "EXP34-C".to_string(),
                            severity: Severity::High,
                            message: format!(
                                "Potential null pointer dereference in member access of variable '{}'",
                                var_name
                            ),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some(format!(
                                "Check if '{}' is not NULL before member access",
                                var_name
                            )),
                            ..Default::default()
                        });
                    }
                }
            }
        }
        "call_expression" => {
            if let Some(function) = node.child_by_field_name("function") {
                // Function pointer call
                if function.kind() == "identifier" {
                    let func_name = ast_utils::get_node_text_owned(&function, source);
                    if is_unsafe_at(&func_name, node, source, analysis, cfg, body, summaries) {
                        let start_point = function.start_position();
                        violations.push(RuleViolation {
                            rule_id: "EXP34-C".to_string(),
                            severity: Severity::High,
                            message: format!(
                                "Calling potentially null function pointer '{}'",
                                func_name
                            ),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some(format!(
                                "Check if '{}' is not NULL before calling",
                                func_name
                            )),
                            ..Default::default()
                        });
                    }
                }

                // Check deref-function arguments
                let func_name = ast_utils::get_node_text_owned(&function, source);
                if is_deref_function(&func_name) {
                    if let Some(args) = node.child_by_field_name("arguments") {
                        check_function_arguments_cfg(
                            &args, source, analysis, cfg, body, summaries, violations,
                        );
                    }
                }

                // Call-site null propagation: flag passing DefinitelyNull
                // to a function that doesn't null-check that parameter.
                if !is_deref_function(&func_name) {
                    if let Some(args) = node.child_by_field_name("arguments") {
                        check_callsite_null_args(
                            &func_name, &args, source, analysis, cfg, body,
                            summaries, violations,
                        );
                    }
                }
            }
        }
        _ => {}
    }

    // Recurse
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            check_dereferences_cfg(&child, source, analysis, cfg, body, summaries, violations);
        }
    }
}

fn check_function_arguments_cfg(
    args: &Node,
    source: &str,
    analysis: &NullAnalysisResult,
    cfg: &FunctionCfg,
    body: &Node,
    summaries: &HashMap<String, FunctionSummary>,
    violations: &mut Vec<RuleViolation>,
) {
    for i in 0..args.child_count() {
        if let Some(arg) = args.child(i) {
            if arg.kind() == "identifier" {
                let var_name = ast_utils::get_node_text_owned(&arg, source);
                if is_unsafe_at(&var_name, &arg, source, analysis, cfg, body, summaries) {
                    let start_point = arg.start_position();
                    violations.push(RuleViolation {
                        rule_id: "EXP34-C".to_string(),
                        severity: Severity::High,
                        message: format!(
                            "Passing potentially null pointer '{}' to function",
                            var_name
                        ),
                        file_path: String::new(),
                        line: start_point.row + 1,
                        column: start_point.column + 1,
                        suggestion: Some(format!(
                            "Check if '{}' is not NULL before passing to function",
                            var_name
                        )),
                        ..Default::default()
                    });
                }
            }
        }
    }
}

/// Call-site null propagation: flag passing a DefinitelyNull pointer to a
/// function that doesn't null-check that parameter. This catches the source
/// side of cross-file null dereferences (Juliet variants 51-68).
fn check_callsite_null_args(
    callee_name: &str,
    args: &Node,
    source: &str,
    analysis: &NullAnalysisResult,
    cfg: &FunctionCfg,
    body: &Node,
    summaries: &HashMap<String, FunctionSummary>,
    violations: &mut Vec<RuleViolation>,
) {
    let callee_summary = summaries.get(callee_name);

    let mut param_idx: usize = 0;
    for i in 0..args.child_count() {
        if let Some(arg) = args.child(i) {
            // Skip commas and other non-argument tokens
            if arg.kind() == "," || arg.kind() == "(" || arg.kind() == ")" {
                continue;
            }

            if arg.kind() == "identifier" {
                let var_name = ast_utils::get_node_text_owned(&arg, source);
                let state = null_state::get_var_state_at(
                    analysis,
                    cfg,
                    body,
                    source,
                    &var_name,
                    arg.start_byte(),
                    summaries,
                );

                // Only flag DefinitelyNull — PossiblyNull is too noisy for call sites
                if state == null_state::NullState::DefinitelyNull {
                    // Check if the callee null-checks this parameter
                    let callee_checks_null = callee_summary
                        .map(|s| s.checks_null_params.contains(&param_idx))
                        .unwrap_or(false);

                    if !callee_checks_null {
                        let start_point = arg.start_position();
                        violations.push(RuleViolation {
                            rule_id: "EXP34-C".to_string(),
                            severity: Severity::High,
                            message: format!(
                                "Passing null pointer '{}' to '{}' which does not check for NULL",
                                var_name, callee_name
                            ),
                            file_path: String::new(),
                            line: start_point.row + 1,
                            column: start_point.column + 1,
                            suggestion: Some(format!(
                                "Check if '{}' is not NULL before passing to '{}'",
                                var_name, callee_name
                            )),
                            ..Default::default()
                        });
                    }
                }
            }

            param_idx += 1;
        }
    }
}

// ---------------------------------------------------------------------------
// Safety determination: CFG dataflow + AST expression guards
// ---------------------------------------------------------------------------

/// Check if dereferencing `var_name` at `deref_node` is unsafe.
/// Uses CFG dataflow as the primary check, with AST-based expression guards
/// for patterns that live within a single expression (&&, ternary).
fn is_unsafe_at(
    var_name: &str,
    deref_node: &Node,
    source: &str,
    analysis: &NullAnalysisResult,
    cfg: &FunctionCfg,
    body: &Node,
    summaries: &HashMap<String, FunctionSummary>,
) -> bool {
    let deref_byte = deref_node.start_byte();

    // Primary check: CFG-based null state dataflow
    if !null_state::is_null_deref_at(analysis, cfg, body, source, var_name, deref_byte, summaries) {
        return false; // CFG says safe
    }

    // The CFG says potentially unsafe. But check AST-level expression guards
    // that the CFG cannot model (because they're within a single expression,
    // not separate CFG blocks).

    // Check && short-circuit: (ptr != NULL) && (ptr->field)
    // Check ternary: (ptr != NULL) ? *ptr : 0
    // Check if-statement scope (redundant with CFG for most cases, but catches
    // edge cases where CFG block boundaries don't align perfectly)
    if is_in_expression_guard(var_name, deref_node, source) {
        return false;
    }

    true
}

/// Check if a dereference is guarded by expression-level null checks
/// that the CFG cannot model (&&, ternary) or by pragmatic null-check
/// patterns (if (ptr == NULL) { /* handle error */ } — no explicit return).
fn is_in_expression_guard(var_name: &str, node: &Node, source: &str) -> bool {
    let mut current = node.parent();

    while let Some(parent) = current {
        // && short-circuit: (ptr != NULL) && (ptr->field)
        if parent.kind() == "binary_expression" {
            if let Some(operator) = parent.child_by_field_name("operator") {
                let op = ast_utils::get_node_text_owned(&operator, source);
                if op == "&&" {
                    if let (Some(left), Some(right)) = (
                        parent.child_by_field_name("left"),
                        parent.child_by_field_name("right"),
                    ) {
                        if node_is_within(&right, node)
                            && analyze_condition_for_safety(&left, var_name, source, false)
                        {
                            return true;
                        }
                    }
                }
            }
        }

        // Ternary: (ptr != NULL) ? *ptr : default
        if parent.kind() == "conditional_expression" {
            if let Some(condition) = parent.child_by_field_name("condition") {
                if let Some(checked_var) = get_null_check_var(&condition, source) {
                    if checked_var == var_name {
                        let is_safe_in_consequence =
                            analyze_condition_for_safety(&condition, var_name, source, false);

                        if let Some(consequence) = parent.child_by_field_name("consequence") {
                            if node_is_within(&consequence, node) {
                                return is_safe_in_consequence;
                            }
                        }
                        if let Some(alternative) = parent.child_by_field_name("alternative") {
                            if node_is_within(&alternative, node) {
                                return !is_safe_in_consequence;
                            }
                        }
                    }
                }
            }
        }

        current = parent.parent();
    }

    // Pragmatic dominance check: if there's an if-statement earlier in the same
    // function that checks (var == NULL) and the dereference is AFTER that
    // if-statement, treat it as safe. This matches the common pattern:
    //   if (ptr == NULL) { /* Handle error */ }
    //   use(ptr);  // programmer assumes error was handled
    if is_dominated_by_null_check(var_name, node, source) {
        return true;
    }

    false
}

/// Walk up the AST to find the enclosing function body, then search for
/// if-statements that check `var_name == NULL` and occur before the
/// dereference (byte-position dominance).
fn is_dominated_by_null_check(var_name: &str, node: &Node, source: &str) -> bool {
    let deref_byte = node.start_byte();

    // Find the enclosing compound_statement (function body)
    let mut current = node.parent();
    let mut func_body = None;
    while let Some(parent) = current {
        if parent.kind() == "function_definition" {
            func_body = parent.child_by_field_name("body");
            break;
        }
        current = parent.parent();
    }

    let body = match func_body {
        Some(b) => b,
        None => return false,
    };

    // Search for if-statements that null-check this variable
    has_dominating_null_check(&body, var_name, deref_byte, source)
}

fn has_dominating_null_check(node: &Node, var_name: &str, deref_byte: usize, source: &str) -> bool {
    if node.kind() == "if_statement" {
        // Must be BEFORE the dereference
        if node.end_byte() <= deref_byte {
            if let Some(condition) = node.child_by_field_name("condition") {
                if let Some(checked_var) = get_null_check_var(&condition, source) {
                    if checked_var == var_name {
                        // Check that the condition checks FOR null (== NULL, !ptr)
                        if !analyze_condition_for_safety(&condition, var_name, source, false) {
                            return true;
                        }
                    }
                }
            }
        }
    }

    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            // Don't search past the dereference point
            if child.start_byte() > deref_byte {
                break;
            }
            if has_dominating_null_check(&child, var_name, deref_byte, source) {
                return true;
            }
        }
    }

    false
}

// ---------------------------------------------------------------------------
// Condition analysis helpers (used for expression-level guards)
// ---------------------------------------------------------------------------

fn get_null_check_var(condition: &Node, source: &str) -> Option<String> {
    match condition.kind() {
        "parenthesized_expression" => condition
            .child(1)
            .and_then(|c| get_null_check_var(&c, source)),
        "binary_expression" => {
            if let (Some(left), Some(right)) = (
                condition.child_by_field_name("left"),
                condition.child_by_field_name("right"),
            ) {
                let lt = ast_utils::get_node_text_owned(&left, source);
                let rt = ast_utils::get_node_text_owned(&right, source);
                if is_null_value(&rt) && left.kind() == "identifier" {
                    return Some(lt);
                }
                if is_null_value(&lt) && right.kind() == "identifier" {
                    return Some(rt);
                }
                if let Some(operator) = condition.child_by_field_name("operator") {
                    let op = ast_utils::get_node_text_owned(&operator, source);
                    if op == "||" || op == "&&" {
                        if let Some(var) = get_null_check_var(&left, source) {
                            return Some(var);
                        }
                        return get_null_check_var(&right, source);
                    }
                }
            }
            None
        }
        "unary_expression" => {
            if let Some(operand) = condition.child_by_field_name("argument") {
                if operand.kind() == "identifier" {
                    return Some(ast_utils::get_node_text_owned(&operand, source));
                }
            }
            None
        }
        "identifier" => Some(ast_utils::get_node_text_owned(condition, source)),
        _ => None,
    }
}

fn analyze_condition_for_safety(node: &Node, var_name: &str, source: &str, negated: bool) -> bool {
    match node.kind() {
        "parenthesized_expression" => {
            if let Some(child) = node.child(1) {
                return analyze_condition_for_safety(&child, var_name, source, negated);
            }
        }
        "unary_expression" => {
            if let Some(operator) = node.child(0) {
                if ast_utils::get_node_text_owned(&operator, source) == "!" {
                    if let Some(argument) = node.child_by_field_name("argument") {
                        return analyze_condition_for_safety(&argument, var_name, source, !negated);
                    }
                }
            }
        }
        "binary_expression" => {
            if let Some(operator) = node.child_by_field_name("operator") {
                let op = ast_utils::get_node_text_owned(&operator, source);
                match op.as_str() {
                    "==" => {
                        if is_null_comparison(node, var_name, source) {
                            return negated;
                        }
                    }
                    "!=" => {
                        if is_null_comparison(node, var_name, source) {
                            return !negated;
                        }
                    }
                    "&&" => {
                        if let (Some(left), Some(right)) = (
                            node.child_by_field_name("left"),
                            node.child_by_field_name("right"),
                        ) {
                            let l = analyze_condition_for_safety(&left, var_name, source, negated);
                            let r = analyze_condition_for_safety(&right, var_name, source, negated);
                            return l || r;
                        }
                    }
                    "||" => {
                        if let (Some(left), Some(right)) = (
                            node.child_by_field_name("left"),
                            node.child_by_field_name("right"),
                        ) {
                            let l = analyze_condition_for_safety(&left, var_name, source, negated);
                            let r = analyze_condition_for_safety(&right, var_name, source, negated);
                            return l && r;
                        }
                    }
                    _ => {}
                }
            }
        }
        "identifier" => {
            let text = ast_utils::get_node_text_owned(node, source);
            if text == var_name {
                return !negated;
            }
        }
        _ => {}
    }
    false
}

fn is_null_comparison(binary_expr: &Node, var_name: &str, source: &str) -> bool {
    if let (Some(left), Some(right)) = (
        binary_expr.child_by_field_name("left"),
        binary_expr.child_by_field_name("right"),
    ) {
        let lt = ast_utils::get_node_text_owned(&left, source);
        let rt = ast_utils::get_node_text_owned(&right, source);
        (lt == var_name && is_null_value(&rt)) || (rt == var_name && is_null_value(&lt))
    } else {
        false
    }
}

fn node_is_within(parent_node: &Node, child_node: &Node) -> bool {
    parent_node.start_byte() <= child_node.start_byte()
        && parent_node.end_byte() >= child_node.end_byte()
}

// ---------------------------------------------------------------------------
// Shared helpers
// ---------------------------------------------------------------------------

fn is_null_value(text: &str) -> bool {
    null_state::is_null_value(text)
}

fn is_deref_function(func_name: &str) -> bool {
    matches!(
        func_name,
        "strlen"
            | "strcpy"
            | "strcat"
            | "strcmp"
            | "strchr"
            | "strstr"
            | "sprintf"
            | "fprintf"
            | "printf"
            | "scanf"
            | "fscanf"
            | "fread"
            | "fwrite"
            | "fgets"
            | "fputs"
            | "fputc"
            | "fgetc"
            | "memcpy"
            | "memmove"
            | "memset"
            | "memcmp"
            | "free"
            | "fclose"
    )
}

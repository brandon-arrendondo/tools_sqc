//! Function summary computation for inter-procedural analysis.
//!
//! Computes lightweight summaries of each function's behavior during the prescan
//! phase. These summaries are used by rules to reason about callee behavior
//! without re-analyzing the callee's body.

use crate::analyze::const_eval::{self, MacroConstantMap, ValueRange, VarRangeMap};
use crate::analyze::null_state::NullState;
use std::collections::{HashMap, HashSet};
use tree_sitter::Node;

/// Summary of a function's behavior relevant to CERT C rules.
#[derive(Debug, Clone, Default)]
pub struct FunctionSummary {
    /// Parameter indices that this function frees (e.g., free(param[0])).
    pub frees_params: HashSet<usize>,
    /// Whether this function can return NULL.
    pub can_return_null: bool,
    /// Whether this function returns dynamically allocated memory.
    pub returns_allocation: bool,
    /// Parameter indices that this function checks for NULL.
    pub checks_null_params: HashSet<usize>,
    /// Parameter indices that this function writes through (modifies via pointer).
    pub modifies_params: HashSet<usize>,
    /// Parameter indices that this function dereferences in any way (read or write).
    /// Superset of modifies_params — includes `*param`, `param[i]`, `param->field`.
    pub dereferences_params: HashSet<usize>,
    /// Whether this function never returns (calls abort/exit/longjmp).
    pub never_returns: bool,
    /// Aggregated null states of arguments at all call sites (populated by prescan second pass).
    /// Maps parameter index → joined NullState from all callers.
    pub callsite_param_null_states: HashMap<usize, NullState>,
    /// Computed return value range for integer-returning functions.
    /// `Some(range)` when all return paths provably return values in [min, max].
    /// `None` for void, pointer-returning, or unevaluable return expressions.
    pub return_range: Option<ValueRange>,
}

/// Compute function summaries for all function definitions in the AST.
///
/// When `compute_return_ranges` is true, also computes return value ranges
/// for integer-returning functions (needed for VRA inter-procedural analysis).
/// Pass false during prescan when no VRA-consuming rules are enabled.
pub fn compute_summaries(
    root: &Node,
    source: &str,
    macros: &MacroConstantMap,
    compute_return_ranges: bool,
) -> HashMap<String, FunctionSummary> {
    let mut summaries = HashMap::new();

    collect_function_summaries(root, source, macros, compute_return_ranges, &mut summaries);

    summaries
}

fn collect_function_summaries(
    node: &Node,
    source: &str,
    macros: &MacroConstantMap,
    compute_return_ranges: bool,
    summaries: &mut HashMap<String, FunctionSummary>,
) {
    if node.kind() == "function_definition" {
        if let Some(name) = extract_function_name(node, source) {
            let summary = analyze_function(node, source, macros, compute_return_ranges);
            summaries.insert(name, summary);
        }
    }

    // Recurse into preproc blocks
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            match child.kind() {
                "function_definition" => {
                    if let Some(name) = extract_function_name(&child, source) {
                        let summary =
                            analyze_function(&child, source, macros, compute_return_ranges);
                        summaries.insert(name, summary);
                    }
                }
                kind if kind.starts_with("preproc_") => {
                    collect_function_summaries(
                        &child,
                        source,
                        macros,
                        compute_return_ranges,
                        summaries,
                    );
                }
                _ => {}
            }
        }
    }
}

/// Analyze a single function definition to produce its summary.
fn analyze_function(
    func_node: &Node,
    source: &str,
    macros: &MacroConstantMap,
    compute_return_ranges: bool,
) -> FunctionSummary {
    let mut summary = FunctionSummary::default();

    // Collect parameter names
    let params = collect_param_names(func_node, source);

    // Check the return type
    let is_pointer_return;
    let is_void_return;
    if let Some(return_type) = func_node.child_by_field_name("type") {
        let type_text = return_type.utf8_text(source.as_bytes()).unwrap_or("");
        // Functions returning pointer types might return NULL
        let decl_text = func_node
            .child_by_field_name("declarator")
            .map(|d| d.utf8_text(source.as_bytes()).unwrap_or(""))
            .unwrap_or("");
        is_pointer_return = decl_text.contains('*');
        is_void_return = type_text == "void";
        if is_pointer_return {
            // Could return NULL unless proven otherwise
            summary.can_return_null = true;
        }
        // void functions can't return NULL
        if is_void_return {
            summary.can_return_null = false;
        }
    } else {
        is_pointer_return = false;
        is_void_return = false;
    }

    // Analyze function body
    if let Some(body) = func_node.child_by_field_name("body") {
        let body_text = body.utf8_text(source.as_bytes()).unwrap_or("");

        // Check for never-returns patterns
        summary.never_returns = check_never_returns(&body, source);

        // Check for returns-allocation pattern
        summary.returns_allocation = body_text.contains("malloc(")
            || body_text.contains("calloc(")
            || body_text.contains("realloc(")
            || body_text.contains("aligned_alloc(");

        // Check for NULL return
        if !summary.can_return_null {
            // Even non-pointer return types: check if the function returns NULL
            summary.can_return_null = check_returns_null(&body, source);
        }

        // Analyze parameter usage
        analyze_param_usage(&body, source, &params, &mut summary);

        // Compute return value range for integer-returning functions (only when VRA is needed)
        if compute_return_ranges && !is_void_return && !is_pointer_return {
            summary.return_range = compute_return_range(&body, source, macros);
        }
    }

    summary
}

/// Collect parameter names from a function declaration.
pub fn collect_param_names(func_node: &Node, source: &str) -> Vec<String> {
    let mut params = Vec::new();

    if let Some(declarator) = func_node.child_by_field_name("declarator") {
        collect_params_recursive(&declarator, source, &mut params);
    }

    params
}

fn collect_params_recursive(node: &Node, source: &str, params: &mut Vec<String>) {
    if node.kind() == "function_declarator" {
        if let Some(param_list) = node.child_by_field_name("parameters") {
            for i in 0..param_list.child_count() {
                if let Some(param) = param_list.child(i) {
                    if param.kind() == "parameter_declaration" {
                        if let Some(decl) = param.child_by_field_name("declarator") {
                            let name = extract_leaf_identifier(&decl, source);
                            params.push(name);
                        } else {
                            params.push(String::new()); // Unnamed parameter
                        }
                    }
                }
            }
        }
    } else {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                collect_params_recursive(&child, source, params);
            }
        }
    }
}

/// Check if a function body always calls abort/exit/longjmp (never returns normally).
fn check_never_returns(body: &Node, source: &str) -> bool {
    let body_text = body.utf8_text(source.as_bytes()).unwrap_or("");

    // Quick text check — if none of these are present, the function can return
    if !body_text.contains("abort(")
        && !body_text.contains("exit(")
        && !body_text.contains("_Exit(")
        && !body_text.contains("longjmp(")
        && !body_text.contains("quick_exit(")
    {
        return false;
    }

    // More precise: check if every code path ends with a no-return call.
    // For simplicity, check if the function's body ends with a no-return call
    // (last statement is abort/exit/etc. — no return statement after it).
    let has_return = body_text.contains("return ");
    let ends_with_noreturn = body_text.contains("abort()")
        || body_text.contains("exit(EXIT_FAILURE)")
        || body_text.contains("exit(1)")
        || body_text.contains("exit(EXIT_SUCCESS)")
        || body_text.contains("exit(0)");

    // If the function has no return statements and ends with a no-return call
    if !has_return && ends_with_noreturn {
        return true;
    }

    // Simple heuristic: if every path through the function ends with
    // abort/exit, it never returns. This is too expensive to check fully
    // without a CFG, so we use a conservative approach.
    false
}

/// Check if a function body contains any `return NULL` / `return 0` statements.
fn check_returns_null(body: &Node, source: &str) -> bool {
    if body.kind() == "return_statement" {
        for i in 0..body.child_count() {
            if let Some(child) = body.child(i) {
                if child.kind() != "return" {
                    let text = child.utf8_text(source.as_bytes()).unwrap_or("").trim();
                    if text == "NULL" || text == "0" || text == "nullptr" {
                        return true;
                    }
                }
            }
        }
    }

    for i in 0..body.child_count() {
        if let Some(child) = body.child(i) {
            if check_returns_null(&child, source) {
                return true;
            }
        }
    }

    false
}

/// Analyze how parameters are used in the function body.
fn analyze_param_usage(
    body: &Node,
    source: &str,
    params: &[String],
    summary: &mut FunctionSummary,
) {
    let body_text = body.utf8_text(source.as_bytes()).unwrap_or("");

    for (idx, param_name) in params.iter().enumerate() {
        if param_name.is_empty() {
            continue;
        }

        // Check if parameter is freed
        if body_text.contains(&format!("free({})", param_name))
            || body_text.contains(&format!("free( {} )", param_name))
        {
            summary.frees_params.insert(idx);
        }

        // Check if parameter is null-checked
        if body_text.contains(&format!("{} == NULL", param_name))
            || body_text.contains(&format!("NULL == {}", param_name))
            || body_text.contains(&format!("{} != NULL", param_name))
            || body_text.contains(&format!("NULL != {}", param_name))
            || body_text.contains(&format!("!{}", param_name))
        {
            summary.checks_null_params.insert(idx);
        }

        // Check if parameter is written through (dereferenced on left side of assignment)
        if body_text.contains(&format!("*{} =", param_name))
            || body_text.contains(&format!("{}->", param_name))
            || body_text.contains(&format!("{}[", param_name))
        {
            summary.modifies_params.insert(idx);
        }

        // Check if parameter is dereferenced in any way (read or write)
        if body_text.contains(&format!("*{}", param_name))
            || body_text.contains(&format!("{}->", param_name))
            || body_text.contains(&format!("{}[", param_name))
        {
            summary.dereferences_params.insert(idx);
        }
    }
}

/// Compute the return value range for an integer-returning function.
///
/// Collects all `return expr;` statements in the body, evaluates each
/// expression as a constant range, and joins them. Returns `None` if any
/// return expression cannot be evaluated (conservative).
fn compute_return_range(
    body: &Node,
    source: &str,
    macros: &MacroConstantMap,
) -> Option<ValueRange> {
    let mut return_exprs = Vec::new();
    collect_return_expressions(body, &mut return_exprs);

    if return_exprs.is_empty() {
        return None;
    }

    let empty_vars = VarRangeMap::new();
    let mut combined: Option<ValueRange> = None;

    for expr_node in &return_exprs {
        // Try to evaluate the return expression as a constant range.
        // Uses empty var_ranges — only resolves literals, macros, sizeof, and
        // arithmetic on those. Parameter-dependent returns yield None.
        let range = const_eval::try_evaluate_range(expr_node, source, macros, &empty_vars)?;
        combined = Some(match combined {
            Some(existing) => {
                ValueRange::new(existing.min.min(range.min), existing.max.max(range.max))
            }
            None => range,
        });
    }

    combined
}

/// Recursively collect the expression child of every `return_statement` in `node`.
fn collect_return_expressions<'a>(node: &Node<'a>, out: &mut Vec<Node<'a>>) {
    if node.kind() == "return_statement" {
        // The return expression is the first non-keyword child
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                if child.kind() != "return" && child.kind() != ";" {
                    out.push(child);
                    return;
                }
            }
        }
        // Bare `return;` — no expression (void-style)
        return;
    }

    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            collect_return_expressions(&child, out);
        }
    }
}

fn extract_function_name(func_node: &Node, source: &str) -> Option<String> {
    let declarator = func_node.child_by_field_name("declarator")?;
    let name = extract_leaf_identifier(&declarator, source);
    if name.is_empty() {
        None
    } else {
        Some(name)
    }
}

fn extract_leaf_identifier(node: &Node, source: &str) -> String {
    match node.kind() {
        "identifier" => node.utf8_text(source.as_bytes()).unwrap_or("").to_string(),
        "function_declarator" | "pointer_declarator" | "array_declarator" => {
            if let Some(inner) = node.child_by_field_name("declarator") {
                extract_leaf_identifier(&inner, source)
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

/// Infer the null state of a call argument from AST structure alone.
///
/// Used during prescan to collect argument states at each call site without
/// running full dataflow. Returns:
/// - DefinitelyNull for NULL/0/nullptr literals or casts wrapping them
/// - NotNull for string literals, &var, non-zero numeric literals
/// - Unknown for identifiers and complex expressions (conservative)
pub fn infer_arg_null_state(arg: &Node, source: &str) -> NullState {
    match arg.kind() {
        "null" | "nullptr" => NullState::DefinitelyNull,
        "number_literal" => {
            let text = arg.utf8_text(source.as_bytes()).unwrap_or("").trim();
            if text == "0" {
                NullState::DefinitelyNull
            } else {
                NullState::NotNull
            }
        }
        "string_literal" | "concatenated_string" | "char_literal" => NullState::NotNull,
        "unary_expression" => {
            // &var is always non-null
            if let Some(op) = arg.child_by_field_name("operator") {
                if op.utf8_text(source.as_bytes()).unwrap_or("") == "&" {
                    return NullState::NotNull;
                }
            }
            NullState::Unknown
        }
        "cast_expression" => {
            // (type*)NULL or (type*)0
            if let Some(value) = arg.child_by_field_name("value") {
                let inner = infer_arg_null_state(&value, source);
                if inner == NullState::DefinitelyNull {
                    return NullState::DefinitelyNull;
                }
            }
            NullState::Unknown
        }
        "parenthesized_expression" => {
            // Unwrap (expr)
            if let Some(inner) = arg.child(1) {
                return infer_arg_null_state(&inner, source);
            }
            NullState::Unknown
        }
        "identifier" => {
            let text = arg.utf8_text(source.as_bytes()).unwrap_or("");
            if text == "NULL" {
                NullState::DefinitelyNull
            } else if matches!(text, "stdout" | "stderr" | "stdin") {
                // Standard C streams are guaranteed non-null
                NullState::NotNull
            } else {
                NullState::Unknown
            }
        }
        _ => NullState::Unknown,
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn parse_and_summarize(code: &str) -> HashMap<String, FunctionSummary> {
        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let macros = const_eval::collect_macro_constants(&tree.root_node(), code);
        compute_summaries(&tree.root_node(), code, &macros, true)
    }

    #[test]
    fn test_never_returns() {
        let code = r#"
        void die(const char *msg) {
            fprintf(stderr, "%s\n", msg);
            abort();
        }
        "#;
        let summaries = parse_and_summarize(code);
        let summary = summaries.get("die").unwrap();
        assert!(summary.never_returns);
    }

    #[test]
    fn test_frees_params() {
        let code = r#"
        void cleanup(void *ptr) {
            free(ptr);
        }
        "#;
        let summaries = parse_and_summarize(code);
        let summary = summaries.get("cleanup").unwrap();
        assert!(summary.frees_params.contains(&0));
    }

    #[test]
    fn test_can_return_null() {
        let code = r#"
        char *find_match(const char *haystack, const char *needle) {
            char *result = strstr(haystack, needle);
            if (!result) {
                return NULL;
            }
            return result;
        }
        "#;
        let summaries = parse_and_summarize(code);
        let summary = summaries.get("find_match").unwrap();
        assert!(summary.can_return_null);
    }

    #[test]
    fn test_checks_null_params() {
        let code = r#"
        int safe_strlen(const char *s) {
            if (s == NULL) {
                return 0;
            }
            return strlen(s);
        }
        "#;
        let summaries = parse_and_summarize(code);
        let summary = summaries.get("safe_strlen").unwrap();
        assert!(summary.checks_null_params.contains(&0));
    }

    #[test]
    fn test_modifies_params() {
        let code = r#"
        void init_struct(struct config *cfg) {
            cfg->value = 0;
            cfg->name = "default";
        }
        "#;
        let summaries = parse_and_summarize(code);
        let summary = summaries.get("init_struct").unwrap();
        assert!(summary.modifies_params.contains(&0));
    }

    #[test]
    fn test_returns_allocation() {
        let code = r#"
        char *create_buffer(size_t size) {
            char *buf = malloc(size);
            if (!buf) return NULL;
            memset(buf, 0, size);
            return buf;
        }
        "#;
        let summaries = parse_and_summarize(code);
        let summary = summaries.get("create_buffer").unwrap();
        assert!(summary.returns_allocation);
        assert!(summary.can_return_null);
    }

    #[test]
    fn test_return_range_constant() {
        let code = r#"
        int get_five(void) { return 5; }
        "#;
        let summaries = parse_and_summarize(code);
        let summary = summaries.get("get_five").unwrap();
        assert_eq!(summary.return_range, Some(ValueRange::exact(5)));
    }

    #[test]
    fn test_return_range_multiple_paths() {
        let code = r#"
        int get_bounded(int flag) {
            if (flag) return 1;
            return 10;
        }
        "#;
        let summaries = parse_and_summarize(code);
        let summary = summaries.get("get_bounded").unwrap();
        assert_eq!(summary.return_range, Some(ValueRange::new(1, 10)));
    }

    #[test]
    fn test_return_range_void() {
        let code = r#"
        void do_nothing(void) { return; }
        "#;
        let summaries = parse_and_summarize(code);
        let summary = summaries.get("do_nothing").unwrap();
        assert_eq!(summary.return_range, None);
    }

    #[test]
    fn test_return_range_pointer() {
        let code = r#"
        int *get_ptr(void) { return 0; }
        "#;
        let summaries = parse_and_summarize(code);
        let summary = summaries.get("get_ptr").unwrap();
        assert_eq!(summary.return_range, None);
    }

    #[test]
    fn test_return_range_param_dependent() {
        let code = r#"
        int identity(int x) { return x; }
        "#;
        let summaries = parse_and_summarize(code);
        let summary = summaries.get("identity").unwrap();
        // Parameter-dependent return — not evaluable
        assert_eq!(summary.return_range, None);
    }

    #[test]
    fn test_return_range_macro() {
        let code = r#"
        #define MAX_COUNT 100
        int get_max(void) { return MAX_COUNT; }
        "#;
        let summaries = parse_and_summarize(code);
        let summary = summaries.get("get_max").unwrap();
        assert_eq!(summary.return_range, Some(ValueRange::exact(100)));
    }

    #[test]
    fn test_return_range_zero() {
        let code = r#"
        int get_zero(void) { return 0; }
        "#;
        let summaries = parse_and_summarize(code);
        let summary = summaries.get("get_zero").unwrap();
        assert_eq!(summary.return_range, Some(ValueRange::exact(0)));
    }

    #[test]
    fn test_return_range_negative() {
        let code = r#"
        int get_error(void) { return -1; }
        "#;
        let summaries = parse_and_summarize(code);
        let summary = summaries.get("get_error").unwrap();
        assert_eq!(summary.return_range, Some(ValueRange::exact(-1)));
    }
}

//! Lightweight constant evaluation and value-range analysis for macro constants.
//!
//! Resolves `#define` macro constants and propagates value ranges through
//! arithmetic expressions. Used by INT32-C and INT30-C to suppress false
//! positives when expressions provably fit within type limits.
//!
//! This is NOT a full CFG-based dataflow — it's syntactic constant folding
//! plus loop-bound ancestor walks.

use std::collections::HashMap;
use std::sync::LazyLock;
use tree_sitter::Node;

/// Map of macro name → constant integer value.
pub type MacroConstantMap = HashMap<String, i64>;

/// Map of variable name → value range.
pub type VarRangeMap = HashMap<String, ValueRange>;

/// An integer value range [min, max].
#[derive(Debug, Clone, Copy, PartialEq)]
pub struct ValueRange {
    pub min: i64,
    pub max: i64,
}

impl ValueRange {
    pub fn exact(val: i64) -> Self {
        Self { min: val, max: val }
    }

    pub fn new(min: i64, max: i64) -> Self {
        Self { min, max }
    }

    pub fn add(&self, other: &ValueRange) -> Option<Self> {
        let min = self.min.checked_add(other.min)?;
        let max = self.max.checked_add(other.max)?;
        Some(Self { min, max })
    }

    pub fn sub(&self, other: &ValueRange) -> Option<Self> {
        let min = self.min.checked_sub(other.max)?;
        let max = self.max.checked_sub(other.min)?;
        Some(Self { min, max })
    }

    pub fn mul(&self, other: &ValueRange) -> Option<Self> {
        // For multiplication, all four corners must be checked
        let corners = [
            self.min.checked_mul(other.min)?,
            self.min.checked_mul(other.max)?,
            self.max.checked_mul(other.min)?,
            self.max.checked_mul(other.max)?,
        ];
        Some(Self {
            min: *corners.iter().min().unwrap(),
            max: *corners.iter().max().unwrap(),
        })
    }

    pub fn shl(&self, other: &ValueRange) -> Option<Self> {
        if other.max > 63 || other.max < 0 {
            return None;
        }
        // Clamp negative lower bound to 0: negative shifts are UB in C,
        // so in correct code only the non-negative range is reachable.
        let shift_min = other.min.max(0);
        let other = ValueRange::new(shift_min, other.max);
        let corners = [
            self.min.checked_shl(other.min as u32)?,
            self.min.checked_shl(other.max as u32)?,
            self.max.checked_shl(other.min as u32)?,
            self.max.checked_shl(other.max as u32)?,
        ];
        Some(Self {
            min: *corners.iter().min().unwrap(),
            max: *corners.iter().max().unwrap(),
        })
    }

    /// Returns true if every value in this range fits in a signed integer of the given bit width.
    pub fn fits_in_signed(&self, bits: u32) -> bool {
        if bits == 0 || bits > 64 {
            return false;
        }
        if bits == 64 {
            return true; // i64 always fits in 64-bit signed
        }
        let type_min = -(1i64 << (bits - 1));
        let type_max = (1i64 << (bits - 1)) - 1;
        self.min >= type_min && self.max <= type_max
    }

    /// Returns true if every value in this range fits in an unsigned integer of the given bit width.
    pub fn fits_in_unsigned(&self, bits: u32) -> bool {
        if bits == 0 || bits > 64 {
            return false;
        }
        if self.min < 0 {
            return false;
        }
        if bits >= 64 {
            return true;
        }
        let type_max = (1i64 << bits) - 1;
        self.max <= type_max
    }
}

// ---------------------------------------------------------------------------
// Built-in C standard limit macros (<limits.h>, <stdint.h>)
// ---------------------------------------------------------------------------

/// Returns a map of C standard limit macros to their platform values.
/// Uses LP64 data model (64-bit long) which is standard on modern Linux/macOS.
/// Lazily-initialized built-in C limit macros — allocated once, reused across all files.
static BUILTIN_LIMIT_MACROS: LazyLock<MacroConstantMap> = LazyLock::new(|| {
    let mut m = MacroConstantMap::new();
    // <limits.h> — char
    m.insert("CHAR_BIT".into(), 8);
    m.insert("CHAR_MAX".into(), 127);
    m.insert("CHAR_MIN".into(), -128);
    m.insert("SCHAR_MAX".into(), 127);
    m.insert("SCHAR_MIN".into(), -128);
    m.insert("UCHAR_MAX".into(), 255);
    // <limits.h> — short (16-bit)
    m.insert("SHRT_MAX".into(), 32767);
    m.insert("SHRT_MIN".into(), -32768);
    m.insert("USHRT_MAX".into(), 65535);
    // <limits.h> — int (32-bit)
    m.insert("INT_MAX".into(), 2147483647);
    m.insert("INT_MIN".into(), -2147483648);
    m.insert("UINT_MAX".into(), 4294967295);
    // <limits.h> — long (64-bit on LP64)
    m.insert("LONG_MAX".into(), i64::MAX);
    m.insert("LONG_MIN".into(), i64::MIN);
    // <limits.h> — long long (64-bit)
    m.insert("LLONG_MAX".into(), i64::MAX);
    m.insert("LLONG_MIN".into(), i64::MIN);
    // <stdint.h> — fixed-width
    m.insert("INT8_MAX".into(), 127);
    m.insert("INT8_MIN".into(), -128);
    m.insert("INT16_MAX".into(), 32767);
    m.insert("INT16_MIN".into(), -32768);
    m.insert("INT32_MAX".into(), 2147483647);
    m.insert("INT32_MIN".into(), -2147483648);
    m.insert("INT64_MAX".into(), i64::MAX);
    m.insert("INT64_MIN".into(), i64::MIN);
    m.insert("UINT8_MAX".into(), 255);
    m.insert("UINT16_MAX".into(), 65535);
    m.insert("UINT32_MAX".into(), 4294967295);
    m
});

// ---------------------------------------------------------------------------
// sizeof resolution
// ---------------------------------------------------------------------------

/// Resolve sizeof(type) to a constant value.
/// Uses conservative sizes (LP64 model). Returns None for unknown types.
fn resolve_sizeof_type(type_text: &str) -> Option<i64> {
    let t = type_text.trim();
    match t {
        "char" | "signed char" | "unsigned char" | "int8_t" | "uint8_t" | "bool" | "_Bool" => {
            Some(1)
        }
        "short" | "short int" | "signed short" | "unsigned short" | "int16_t" | "uint16_t" => {
            Some(2)
        }
        "int" | "signed int" | "unsigned int" | "signed" | "unsigned" | "int32_t" | "uint32_t"
        | "wchar_t" | "float" => Some(4),
        "long"
        | "signed long"
        | "unsigned long"
        | "long int"
        | "signed long int"
        | "unsigned long int"
        | "long long"
        | "signed long long"
        | "unsigned long long"
        | "long long int"
        | "signed long long int"
        | "unsigned long long int"
        | "int64_t"
        | "uint64_t"
        | "size_t"
        | "ssize_t"
        | "ptrdiff_t"
        | "double"
        | "time_t"
        | "off_t" => Some(8),
        "long double" => Some(16),
        _ => {
            // Pointer types: any type ending with '*' is pointer-sized (8 on LP64)
            if t.ends_with('*') {
                Some(8)
            } else {
                None
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Macro constant collection
// ---------------------------------------------------------------------------

/// Collect `#define ALIAS func_name` patterns where the value is a single C identifier.
/// These represent macro aliases for function names (e.g., `#define SYSTEM system`).
/// Returns a map from alias → target identifier.
pub fn collect_macro_aliases(root: &Node, source: &str) -> HashMap<String, String> {
    let mut raw_defs: Vec<(String, String)> = Vec::new();
    collect_preproc_defs(root, source, &mut raw_defs);

    let mut aliases = HashMap::new();
    for (name, value) in &raw_defs {
        let v = value.trim();
        // A function alias is a single C identifier (no operators, parens, digits-only, etc.)
        if !v.is_empty()
            && v.chars()
                .all(|c| c.is_ascii_alphanumeric() || c == '_')
            && !v.chars().next().unwrap_or('0').is_ascii_digit()
            // Skip pure integer strings (they're constants, not function aliases)
            && v.parse::<i64>().is_err()
        {
            aliases.insert(name.clone(), v.to_string());
        }
    }
    aliases
}

/// Walk `preproc_def` nodes in the AST to collect `#define NAME value` constants.
/// Handles decimal, hex, octal literals, expressions, and references to other macros.
/// Recurses into `preproc_ifdef/if/ifndef` blocks.
/// Includes built-in C standard limit macros (CHAR_MAX, INT_MAX, etc.).
pub fn collect_macro_constants(root: &Node, source: &str) -> MacroConstantMap {
    let mut macros = BUILTIN_LIMIT_MACROS.clone();
    // Two-pass: first collect all raw definitions, then resolve references
    let mut raw_defs: Vec<(String, String)> = Vec::new();
    collect_preproc_defs(root, source, &mut raw_defs);

    // Iteratively resolve — handles forward references and chains
    let mut changed = true;
    let mut iterations = 0;
    while changed && iterations < 5 {
        changed = false;
        iterations += 1;
        for (name, value_text) in &raw_defs {
            if macros.contains_key(name) {
                continue;
            }
            if let Some(val) = try_evaluate_text(value_text.trim(), &macros) {
                macros.insert(name.clone(), val);
                changed = true;
            }
        }
    }
    macros
}

/// Collect raw `#define NAME value` pairs from the AST.
fn collect_preproc_defs(node: &Node, source: &str, defs: &mut Vec<(String, String)>) {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            match child.kind() {
                "preproc_def" => {
                    // preproc_def has children: name (identifier), value (preproc_arg)
                    let name = child
                        .child_by_field_name("name")
                        .and_then(|n| n.utf8_text(source.as_bytes()).ok())
                        .unwrap_or("")
                        .to_string();
                    let value = child
                        .child_by_field_name("value")
                        .and_then(|n| n.utf8_text(source.as_bytes()).ok())
                        .unwrap_or("")
                        .trim()
                        .to_string();
                    if !name.is_empty() && !value.is_empty() {
                        // Skip function-like macros (have parenthesized params)
                        if !value.starts_with('(')
                            || value.chars().filter(|&c| c == '(').count()
                                == value.chars().filter(|&c| c == ')').count()
                        {
                            defs.push((name, value));
                        }
                    }
                }
                kind if kind.starts_with("preproc_") => {
                    collect_preproc_defs(&child, source, defs);
                }
                _ => {}
            }
        }
    }
}

/// Try to evaluate a text string as an integer constant expression.
/// Handles: decimal, hex, octal literals, macro references, simple arithmetic.
fn try_evaluate_text(text: &str, macros: &MacroConstantMap) -> Option<i64> {
    let text = text.trim();
    // Strip trailing C++ line comments (tree-sitter includes them in preproc_arg)
    let text = if let Some(pos) = text.find("//") {
        text[..pos].trim()
    } else {
        text
    };
    if text.is_empty() {
        return None;
    }
    // Strip trailing type suffixes: U, L, UL, LL, ULL (case-insensitive)
    let text = strip_integer_suffix(text);

    // Strip outer parentheses
    let text = if text.starts_with('(') && text.ends_with(')') {
        let inner = &text[1..text.len() - 1];
        // Verify balanced parens
        if parens_balanced(inner) {
            inner.trim()
        } else {
            text
        }
    } else {
        text
    };

    // Try as a literal
    if let Some(val) = parse_integer_literal(text) {
        return Some(val);
    }

    // Try as a macro reference
    if is_c_identifier(text) {
        return macros.get(text).copied();
    }

    // Try simple binary expressions: A op B
    // Search for operator from right to left (respecting precedence: +/- before */<<)
    if let Some(val) = try_evaluate_binary_text(text, macros) {
        return Some(val);
    }

    // Try unary negation
    if let Some(rest) = text.strip_prefix('-') {
        let rest = rest.trim();
        if let Some(val) = try_evaluate_text(rest, macros) {
            return val.checked_neg();
        }
    }

    None
}

/// Try to evaluate a binary expression in text form.
fn try_evaluate_binary_text(text: &str, macros: &MacroConstantMap) -> Option<i64> {
    // Scan for lowest-precedence operators first (+, -), then (*, /), then (<<, >>)
    // Scan right-to-left for left-associativity
    let bytes = text.as_bytes();
    let mut paren_depth = 0i32;

    // Pass 1: + and - (lowest precedence)
    let mut i = bytes.len();
    while i > 0 {
        i -= 1;
        match bytes[i] {
            b')' => paren_depth += 1,
            b'(' => paren_depth -= 1,
            b'+' | b'-' if paren_depth == 0 && i > 0 => {
                // Make sure it's not part of << or >>
                if bytes[i] == b'-' && i > 0 && bytes[i - 1] == b'>' {
                    continue; // -> operator
                }
                let left = text[..i].trim();
                let right = text[i + 1..].trim();
                if !left.is_empty() && !right.is_empty() {
                    let lv = try_evaluate_text(left, macros)?;
                    let rv = try_evaluate_text(right, macros)?;
                    return if bytes[i] == b'+' {
                        lv.checked_add(rv)
                    } else {
                        lv.checked_sub(rv)
                    };
                }
            }
            _ => {}
        }
    }

    // Pass 2: * and /
    paren_depth = 0;
    i = bytes.len();
    while i > 0 {
        i -= 1;
        match bytes[i] {
            b')' => paren_depth += 1,
            b'(' => paren_depth -= 1,
            b'*' | b'/' if paren_depth == 0 && i > 0 => {
                let left = text[..i].trim();
                let right = text[i + 1..].trim();
                if !left.is_empty() && !right.is_empty() {
                    let lv = try_evaluate_text(left, macros)?;
                    let rv = try_evaluate_text(right, macros)?;
                    return if bytes[i] == b'*' {
                        lv.checked_mul(rv)
                    } else if rv == 0 {
                        None
                    } else {
                        Some(lv / rv)
                    };
                }
            }
            _ => {}
        }
    }

    // Pass 3: << and >>
    paren_depth = 0;
    i = bytes.len();
    while i > 1 {
        i -= 1;
        match bytes[i] {
            b')' => paren_depth += 1,
            b'(' => paren_depth -= 1,
            b'<' if paren_depth == 0 && i > 0 && bytes[i - 1] == b'<' => {
                let left = text[..i - 1].trim();
                let right = text[i + 1..].trim();
                if !left.is_empty() && !right.is_empty() {
                    let lv = try_evaluate_text(left, macros)?;
                    let rv = try_evaluate_text(right, macros)?;
                    if !(0..=63).contains(&rv) {
                        return None;
                    }
                    return lv.checked_shl(rv as u32);
                }
                i -= 1; // skip the first <
            }
            b'>' if paren_depth == 0 && i > 0 && bytes[i - 1] == b'>' => {
                let left = text[..i - 1].trim();
                let right = text[i + 1..].trim();
                if !left.is_empty() && !right.is_empty() {
                    let lv = try_evaluate_text(left, macros)?;
                    let rv = try_evaluate_text(right, macros)?;
                    if !(0..=63).contains(&rv) {
                        return None;
                    }
                    return Some(lv >> rv);
                }
                i -= 1;
            }
            _ => {}
        }
    }

    None
}

// ---------------------------------------------------------------------------
// AST-based constant folding
// ---------------------------------------------------------------------------

/// Try to evaluate an AST expression node to an exact integer value.
pub fn try_evaluate_expr(node: &Node, source: &str, macros: &MacroConstantMap) -> Option<i64> {
    match node.kind() {
        "number_literal" => {
            let text = node.utf8_text(source.as_bytes()).ok()?;
            parse_integer_literal(strip_integer_suffix(text.trim()))
        }
        "identifier" => {
            let name = node.utf8_text(source.as_bytes()).ok()?;
            macros.get(name).copied()
        }
        "parenthesized_expression" => {
            let inner = node.child(1)?; // skip '('
            try_evaluate_expr(&inner, source, macros)
        }
        "binary_expression" => {
            let left = node.child_by_field_name("left")?;
            let right = node.child_by_field_name("right")?;
            let op = node.child_by_field_name("operator").or_else(|| {
                // tree-sitter C grammar: operator is sometimes an unnamed child
                for i in 0..node.child_count() {
                    if let Some(c) = node.child(i) {
                        let k = c.kind();
                        if matches!(k, "+" | "-" | "*" | "/" | "%" | "<<" | ">>") {
                            return Some(c);
                        }
                    }
                }
                None
            })?;
            let op_text = op.utf8_text(source.as_bytes()).ok()?;
            let lv = try_evaluate_expr(&left, source, macros)?;
            let rv = try_evaluate_expr(&right, source, macros)?;
            match op_text {
                "+" => lv.checked_add(rv),
                "-" => lv.checked_sub(rv),
                "*" => lv.checked_mul(rv),
                "/" => {
                    if rv == 0 {
                        None
                    } else {
                        Some(lv / rv)
                    }
                }
                "%" => {
                    if rv == 0 {
                        None
                    } else {
                        Some(lv % rv)
                    }
                }
                "<<" => {
                    if !(0..=63).contains(&rv) {
                        None
                    } else {
                        lv.checked_shl(rv as u32)
                    }
                }
                ">>" => {
                    if !(0..=63).contains(&rv) {
                        None
                    } else {
                        Some(lv >> rv)
                    }
                }
                _ => None,
            }
        }
        "unary_expression" => {
            let arg = node.child_by_field_name("argument")?;
            let op = node
                .child_by_field_name("operator")
                .or_else(|| node.child(0))?;
            let op_text = op.utf8_text(source.as_bytes()).ok()?;
            let val = try_evaluate_expr(&arg, source, macros)?;
            match op_text {
                "-" => val.checked_neg(),
                "+" => Some(val),
                "~" => Some(!val),
                _ => None,
            }
        }
        "cast_expression" => {
            // (type)expr — evaluate the inner expression
            let value = node.child_by_field_name("value")?;
            try_evaluate_expr(&value, source, macros)
        }
        "sizeof_expression" => {
            // sizeof(type) or sizeof(expr)
            resolve_sizeof_node(node, source)
        }
        _ => None,
    }
}

/// Resolve a sizeof_expression AST node to a constant value.
fn resolve_sizeof_node(node: &Node, source: &str) -> Option<i64> {
    // sizeof_expression children: "sizeof" "(" type_descriptor ")" or "sizeof" "(" expression ")"
    // The type is in a parenthesized_expression or type_descriptor child.
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            match child.kind() {
                "type_descriptor" | "primitive_type" | "sized_type_specifier" => {
                    let type_text = child.utf8_text(source.as_bytes()).ok()?;
                    return resolve_sizeof_type(type_text);
                }
                "parenthesized_expression" => {
                    // sizeof(expr) — check if inner is a type-like identifier
                    if let Some(inner) = child.child(1) {
                        if inner.kind() == "identifier" {
                            let text = inner.utf8_text(source.as_bytes()).ok()?;
                            // Could be a typedef name like wchar_t, int64_t, etc.
                            return resolve_sizeof_type(text);
                        }
                    }
                }
                _ => {}
            }
        }
    }
    None
}

// ---------------------------------------------------------------------------
// Range evaluation
// ---------------------------------------------------------------------------

/// Evaluate an AST expression node to a value range.
/// Falls back to `var_ranges` for identifiers not in macros.
pub fn try_evaluate_range(
    node: &Node,
    source: &str,
    macros: &MacroConstantMap,
    var_ranges: &VarRangeMap,
) -> Option<ValueRange> {
    // First try exact evaluation
    if let Some(val) = try_evaluate_expr(node, source, macros) {
        return Some(ValueRange::exact(val));
    }

    match node.kind() {
        "identifier" => {
            let name = node.utf8_text(source.as_bytes()).ok()?;
            if let Some(&val) = macros.get(name) {
                return Some(ValueRange::exact(val));
            }
            var_ranges.get(name).copied()
        }
        "parenthesized_expression" => {
            let inner = node.child(1)?;
            try_evaluate_range(&inner, source, macros, var_ranges)
        }
        "binary_expression" => {
            let left = node.child_by_field_name("left")?;
            let right = node.child_by_field_name("right")?;
            let op = node.child_by_field_name("operator").or_else(|| {
                for i in 0..node.child_count() {
                    if let Some(c) = node.child(i) {
                        let k = c.kind();
                        if matches!(k, "+" | "-" | "*" | "/" | "%" | "<<" | ">>") {
                            return Some(c);
                        }
                    }
                }
                None
            })?;
            let op_text = op.utf8_text(source.as_bytes()).ok()?;
            let lr = try_evaluate_range(&left, source, macros, var_ranges)?;
            let rr = try_evaluate_range(&right, source, macros, var_ranges)?;
            match op_text {
                "+" => lr.add(&rr),
                "-" => lr.sub(&rr),
                "*" => lr.mul(&rr),
                "<<" => lr.shl(&rr),
                _ => None,
            }
        }
        "unary_expression" => {
            let arg = node.child_by_field_name("argument")?;
            let op = node
                .child_by_field_name("operator")
                .or_else(|| node.child(0))?;
            let op_text = op.utf8_text(source.as_bytes()).ok()?;
            let r = try_evaluate_range(&arg, source, macros, var_ranges)?;
            match op_text {
                "-" => Some(ValueRange::new(r.max.checked_neg()?, r.min.checked_neg()?)),
                "+" => Some(r),
                _ => None,
            }
        }
        "cast_expression" => {
            let value = node.child_by_field_name("value")?;
            try_evaluate_range(&value, source, macros, var_ranges)
        }
        "sizeof_expression" => resolve_sizeof_node(node, source).map(ValueRange::exact),
        _ => None,
    }
}

// ---------------------------------------------------------------------------
// Loop-bound extraction
// ---------------------------------------------------------------------------

/// Extract value ranges for variables bounded by enclosing loop conditions.
/// Walks AST ancestors looking for `for`/`while` statements and extracts
/// `var < BOUND` or `var <= BOUND` patterns.
pub fn extract_loop_var_ranges(
    node: &Node,
    source: &str,
    macros: &MacroConstantMap,
) -> VarRangeMap {
    let mut ranges = VarRangeMap::new();
    let mut current = node.parent();
    while let Some(parent) = current {
        match parent.kind() {
            "while_statement" | "do_statement" => {
                if let Some(condition) = parent.child_by_field_name("condition") {
                    extract_bound_from_condition(&condition, source, macros, &mut ranges);
                }
            }
            "for_statement" => {
                // Extract upper bound from condition
                if let Some(condition) = parent.child_by_field_name("condition") {
                    extract_bound_from_condition(&condition, source, macros, &mut ranges);
                }
                // Extract lower bound from initializer
                if let Some(initializer) = parent.child_by_field_name("initializer") {
                    extract_init_from_for(&initializer, source, macros, &mut ranges);
                }
            }
            "function_definition" | "translation_unit" => break,
            _ => {}
        }
        current = parent.parent();
    }
    ranges
}

/// Extract variable bounds from a loop condition expression.
/// Handles: `var < expr`, `var <= expr`, `expr > var`, `expr >= var`.
/// Also handles compound `&&` conditions by extracting bounds from each sub-expression.
fn extract_bound_from_condition(
    condition: &Node,
    source: &str,
    macros: &MacroConstantMap,
    ranges: &mut VarRangeMap,
) {
    // Unwrap parenthesized_expression
    let cond = if condition.kind() == "parenthesized_expression" {
        condition.child(1).unwrap_or(*condition)
    } else {
        *condition
    };

    if cond.kind() != "binary_expression" {
        return;
    }

    // Handle compound && conditions: extract bounds from each side
    let op = get_operator_text(&cond, source);
    if op == "&&" {
        if let Some(left) = cond.child_by_field_name("left") {
            extract_bound_from_condition(&left, source, macros, ranges);
        }
        if let Some(right) = cond.child_by_field_name("right") {
            extract_bound_from_condition(&right, source, macros, ranges);
        }
        return;
    }
    let left = match cond.child_by_field_name("left") {
        Some(n) => n,
        None => return,
    };
    let right = match cond.child_by_field_name("right") {
        Some(n) => n,
        None => return,
    };

    let op = get_operator_text(&cond, source);

    match op.as_str() {
        "<" => {
            // var < BOUND → var in [0, BOUND-1] (assuming non-negative loop counter)
            if left.kind() == "identifier" {
                if let Some(bound) = try_evaluate_expr(&right, source, macros) {
                    let var_name = left.utf8_text(source.as_bytes()).unwrap_or("");
                    if !var_name.is_empty() {
                        let entry = ranges
                            .entry(var_name.to_string())
                            .or_insert(ValueRange::new(0, bound - 1));
                        // Tighten upper bound if this condition is more restrictive
                        if bound - 1 < entry.max {
                            entry.max = bound - 1;
                        }
                    }
                }
            }
        }
        "<=" => {
            if left.kind() == "identifier" {
                if let Some(bound) = try_evaluate_expr(&right, source, macros) {
                    let var_name = left.utf8_text(source.as_bytes()).unwrap_or("");
                    if !var_name.is_empty() {
                        let entry = ranges
                            .entry(var_name.to_string())
                            .or_insert(ValueRange::new(0, bound));
                        if bound < entry.max {
                            entry.max = bound;
                        }
                    }
                }
            }
        }
        ">" => {
            // BOUND > var → same as var < BOUND
            if right.kind() == "identifier" {
                if let Some(bound) = try_evaluate_expr(&left, source, macros) {
                    let var_name = right.utf8_text(source.as_bytes()).unwrap_or("");
                    if !var_name.is_empty() {
                        let entry = ranges
                            .entry(var_name.to_string())
                            .or_insert(ValueRange::new(0, bound - 1));
                        if bound - 1 < entry.max {
                            entry.max = bound - 1;
                        }
                    }
                }
            }
        }
        ">=" => {
            if right.kind() == "identifier" {
                if let Some(bound) = try_evaluate_expr(&left, source, macros) {
                    let var_name = right.utf8_text(source.as_bytes()).unwrap_or("");
                    if !var_name.is_empty() {
                        let entry = ranges
                            .entry(var_name.to_string())
                            .or_insert(ValueRange::new(0, bound));
                        if bound < entry.max {
                            entry.max = bound;
                        }
                    }
                }
            }
        }
        _ => {}
    }
}

/// Extract initializer value from a for-loop init clause.
fn extract_init_from_for(
    init: &Node,
    source: &str,
    macros: &MacroConstantMap,
    ranges: &mut VarRangeMap,
) {
    // Handle `int var = expr` (declaration) or `var = expr` (assignment_expression)
    match init.kind() {
        "declaration" => {
            for i in 0..init.child_count() {
                if let Some(child) = init.child(i) {
                    if child.kind() == "init_declarator" {
                        if let (Some(declarator), Some(value)) = (
                            child.child_by_field_name("declarator"),
                            child.child_by_field_name("value"),
                        ) {
                            let var_name = declarator
                                .utf8_text(source.as_bytes())
                                .unwrap_or("")
                                .to_string();
                            if !var_name.is_empty() {
                                if let Some(val) = try_evaluate_expr(&value, source, macros) {
                                    if let Some(range) = ranges.get_mut(&var_name) {
                                        range.min = val;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        "assignment_expression" => {
            if let (Some(left), Some(right)) = (
                init.child_by_field_name("left"),
                init.child_by_field_name("right"),
            ) {
                if left.kind() == "identifier" {
                    let var_name = left.utf8_text(source.as_bytes()).unwrap_or("").to_string();
                    if !var_name.is_empty() {
                        if let Some(val) = try_evaluate_expr(&right, source, macros) {
                            if let Some(range) = ranges.get_mut(&var_name) {
                                range.min = val;
                            }
                        }
                    }
                }
            }
        }
        _ => {}
    }
}

// ---------------------------------------------------------------------------
// Local variable resolution
// ---------------------------------------------------------------------------

/// Scan backward in the enclosing compound_statement for assignments to `var_name`
/// and try to evaluate the RHS as a range.
pub fn resolve_local_var_range(
    var_name: &str,
    node: &Node,
    source: &str,
    macros: &MacroConstantMap,
    loop_ranges: &VarRangeMap,
) -> Option<ValueRange> {
    // Find the enclosing compound_statement (function body or block)
    let mut current = node.parent();
    while let Some(parent) = current {
        if parent.kind() == "compound_statement" {
            // Scan statements before our node
            let node_start = node.start_byte();
            for i in 0..parent.child_count() {
                if let Some(stmt) = parent.child(i) {
                    if stmt.start_byte() >= node_start {
                        break;
                    }
                    // Look for `var_name = expr` or `type var_name = expr`
                    if let Some(range) =
                        check_stmt_for_var_assignment(&stmt, var_name, source, macros, loop_ranges)
                    {
                        return Some(range);
                    }
                }
            }
        }
        if parent.kind() == "function_definition" {
            break;
        }
        current = parent.parent();
    }
    None
}

/// Check a single statement for an assignment to `var_name` and return its range.
fn check_stmt_for_var_assignment(
    stmt: &Node,
    var_name: &str,
    source: &str,
    macros: &MacroConstantMap,
    loop_ranges: &VarRangeMap,
) -> Option<ValueRange> {
    match stmt.kind() {
        "expression_statement" => {
            for i in 0..stmt.child_count() {
                if let Some(child) = stmt.child(i) {
                    if child.kind() == "assignment_expression" {
                        if let (Some(left), Some(right)) = (
                            child.child_by_field_name("left"),
                            child.child_by_field_name("right"),
                        ) {
                            if left.kind() == "identifier" {
                                let name = left.utf8_text(source.as_bytes()).unwrap_or("");
                                if name == var_name {
                                    return try_evaluate_range(&right, source, macros, loop_ranges);
                                }
                            }
                        }
                    }
                }
            }
        }
        "declaration" => {
            for i in 0..stmt.child_count() {
                if let Some(child) = stmt.child(i) {
                    if child.kind() == "init_declarator" {
                        if let (Some(declarator), Some(value)) = (
                            child.child_by_field_name("declarator"),
                            child.child_by_field_name("value"),
                        ) {
                            let name = extract_leaf_identifier(&declarator, source);
                            if name == var_name {
                                return try_evaluate_range(&value, source, macros, loop_ranges);
                            }
                        }
                    }
                }
            }
        }
        _ => {}
    }
    None
}

/// Extract leaf identifier from a declarator chain (pointer_declarator → identifier).
fn extract_leaf_identifier(node: &Node, source: &str) -> String {
    match node.kind() {
        "identifier" => node.utf8_text(source.as_bytes()).unwrap_or("").to_string(),
        "pointer_declarator" | "array_declarator" => {
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

// ---------------------------------------------------------------------------
// Convenience functions for rule integration
// ---------------------------------------------------------------------------

/// Returns true if the expression provably fits in a signed integer of the given bit width.
/// Combines macro constants, loop-bound extraction, and local variable resolution.
///
/// For left shift operations, also verifies the left operand is non-negative
/// (shifting negative values is UB in C regardless of result).
pub fn expression_fits_in_signed(
    node: &Node,
    source: &str,
    macros: &MacroConstantMap,
    bits: u32,
) -> bool {
    let loop_ranges = extract_loop_var_ranges(node, source, macros);
    let mut var_ranges = loop_ranges.clone();
    resolve_identifiers_in_expr(node, source, macros, &loop_ranges, &mut var_ranges);

    if let Some(range) = try_evaluate_range(node, source, macros, &var_ranges) {
        // For left shift: shifting negative values is UB even if result fits
        if node.kind() == "binary_expression" {
            if is_shift_operator(node, source) && range.min < 0 {
                return false;
            }
            // Also check if left operand of shift is negative
            if is_shift_operator(node, source) {
                if let Some(left) = node.child_by_field_name("left") {
                    if let Some(lr) = try_evaluate_range(&left, source, macros, &var_ranges) {
                        if lr.min < 0 {
                            return false;
                        }
                    }
                }
            }
        }
        return range.fits_in_signed(bits);
    }
    false
}

/// Returns true if the expression provably fits in an unsigned integer of the given bit width.
pub fn expression_fits_in_unsigned(
    node: &Node,
    source: &str,
    macros: &MacroConstantMap,
    bits: u32,
) -> bool {
    let loop_ranges = extract_loop_var_ranges(node, source, macros);
    let mut var_ranges = loop_ranges.clone();
    resolve_identifiers_in_expr(node, source, macros, &loop_ranges, &mut var_ranges);

    if let Some(range) = try_evaluate_range(node, source, macros, &var_ranges) {
        return range.fits_in_unsigned(bits);
    }
    false
}

/// VRA-backed version of `expression_fits_in_signed`.
/// Tries CFG-based value-range analysis first, falls back to syntactic analysis.
pub fn expression_fits_in_signed_vra(
    node: &Node,
    source: &str,
    macros: &MacroConstantMap,
    bits: u32,
    vra_var_ranges: Option<&VarRangeMap>,
) -> bool {
    // Try VRA-provided ranges first
    if let Some(var_ranges) = vra_var_ranges {
        if let Some(range) = try_evaluate_range(node, source, macros, var_ranges) {
            // For left shift: shifting negative values is UB even if result fits
            if node.kind() == "binary_expression" && is_shift_operator(node, source) {
                if range.min < 0 {
                    return false;
                }
                if let Some(left) = node.child_by_field_name("left") {
                    if let Some(lr) = try_evaluate_range(&left, source, macros, var_ranges) {
                        if lr.min < 0 {
                            return false;
                        }
                    }
                }
            }
            return range.fits_in_signed(bits);
        }
    }
    // Fallback to syntactic analysis
    expression_fits_in_signed(node, source, macros, bits)
}

/// VRA-backed version of `expression_fits_in_unsigned`.
/// Tries CFG-based value-range analysis first, falls back to syntactic analysis.
pub fn expression_fits_in_unsigned_vra(
    node: &Node,
    source: &str,
    macros: &MacroConstantMap,
    bits: u32,
    vra_var_ranges: Option<&VarRangeMap>,
) -> bool {
    // Try VRA-provided ranges first
    if let Some(var_ranges) = vra_var_ranges {
        if let Some(range) = try_evaluate_range(node, source, macros, var_ranges) {
            return range.fits_in_unsigned(bits);
        }
    }
    // Fallback to syntactic analysis
    expression_fits_in_unsigned(node, source, macros, bits)
}

/// Resolve identifiers in an expression by scanning local assignments.
pub fn resolve_identifiers_in_expr(
    node: &Node,
    source: &str,
    macros: &MacroConstantMap,
    loop_ranges: &VarRangeMap,
    var_ranges: &mut VarRangeMap,
) {
    if node.kind() == "identifier" {
        let name = node.utf8_text(source.as_bytes()).unwrap_or("");
        if !name.is_empty() && !macros.contains_key(name) && !var_ranges.contains_key(name) {
            if let Some(range) = resolve_local_var_range(name, node, source, macros, loop_ranges) {
                var_ranges.insert(name.to_string(), range);
            }
        }
    }
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            resolve_identifiers_in_expr(&child, source, macros, loop_ranges, var_ranges);
        }
    }
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

fn is_shift_operator(node: &Node, source: &str) -> bool {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if child.kind() == "<<" || child.kind() == ">>" {
                return true;
            }
            // Also check text content for operator nodes
            if let Ok(text) = child.utf8_text(source.as_bytes()) {
                if text == "<<" || text == ">>" {
                    return true;
                }
            }
        }
    }
    false
}

fn get_operator_text(node: &Node, source: &str) -> String {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            let kind = child.kind();
            if matches!(
                kind,
                "<" | "<="
                    | ">"
                    | ">="
                    | "=="
                    | "!="
                    | "+"
                    | "-"
                    | "*"
                    | "/"
                    | "<<"
                    | ">>"
                    | "&&"
                    | "||"
            ) {
                return child.utf8_text(source.as_bytes()).unwrap_or("").to_string();
            }
        }
    }
    String::new()
}

fn parse_integer_literal(text: &str) -> Option<i64> {
    let text = text.trim();
    if text.is_empty() {
        return None;
    }
    // Handle negative literals
    if let Some(rest) = text.strip_prefix('-') {
        let val = parse_unsigned_literal(rest.trim())?;
        return val.checked_neg();
    }
    parse_unsigned_literal(text)
}

fn parse_unsigned_literal(text: &str) -> Option<i64> {
    let text = strip_integer_suffix(text);
    if text.starts_with("0x") || text.starts_with("0X") {
        i64::from_str_radix(&text[2..], 16).ok()
    } else if text.starts_with("0b") || text.starts_with("0B") {
        i64::from_str_radix(&text[2..], 2).ok()
    } else if text.starts_with('0') && text.len() > 1 && text.chars().all(|c| c.is_ascii_digit()) {
        i64::from_str_radix(&text[1..], 8).ok()
    } else {
        text.parse::<i64>().ok()
    }
}

fn strip_integer_suffix(text: &str) -> &str {
    // Strip trailing: ULL, ull, UL, ul, LL, ll, U, u, L, l
    let suffixes = [
        "ULL", "ull", "Ull", "uLL", "UL", "ul", "Ul", "uL", "LL", "ll", "U", "u", "L", "l",
    ];
    for suffix in &suffixes {
        if let Some(stripped) = text.strip_suffix(suffix) {
            return stripped;
        }
    }
    text
}

fn is_c_identifier(text: &str) -> bool {
    !text.is_empty()
        && text
            .chars()
            .next()
            .is_some_and(|c| c.is_alphabetic() || c == '_')
        && text.chars().all(|c| c.is_alphanumeric() || c == '_')
}

fn parens_balanced(text: &str) -> bool {
    let mut depth = 0i32;
    for ch in text.chars() {
        match ch {
            '(' => depth += 1,
            ')' => {
                depth -= 1;
                if depth < 0 {
                    return false;
                }
            }
            _ => {}
        }
    }
    depth == 0
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_parse_integer_literal() {
        assert_eq!(parse_integer_literal("42"), Some(42));
        assert_eq!(parse_integer_literal("0xFF"), Some(255));
        assert_eq!(parse_integer_literal("0x1F"), Some(31));
        assert_eq!(parse_integer_literal("010"), Some(8));
        assert_eq!(parse_integer_literal("0"), Some(0));
        assert_eq!(parse_integer_literal("-1"), Some(-1));
        assert_eq!(parse_integer_literal("50UL"), Some(50));
        assert_eq!(parse_integer_literal("1000LL"), Some(1000));
    }

    #[test]
    fn test_try_evaluate_text_simple() {
        let macros = MacroConstantMap::new();
        assert_eq!(try_evaluate_text("42", &macros), Some(42));
        assert_eq!(try_evaluate_text("(42)", &macros), Some(42));
        assert_eq!(try_evaluate_text("50 * 1000", &macros), Some(50000));
        assert_eq!(try_evaluate_text("250 * 1000", &macros), Some(250000));
    }

    #[test]
    fn test_try_evaluate_text_with_macros() {
        let mut macros = MacroConstantMap::new();
        macros.insert("DELAY_MS".to_string(), 50);
        assert_eq!(try_evaluate_text("DELAY_MS", &macros), Some(50));
        assert_eq!(try_evaluate_text("DELAY_MS * 1000", &macros), Some(50000));
        assert_eq!(try_evaluate_text("(DELAY_MS * 1000)", &macros), Some(50000));
    }

    #[test]
    fn test_try_evaluate_text_shift() {
        let macros = MacroConstantMap::new();
        assert_eq!(try_evaluate_text("1 << 4", &macros), Some(16));
        assert_eq!(try_evaluate_text("500 * (1 << 1)", &macros), Some(1000));
    }

    #[test]
    fn test_collect_macro_constants_chained() {
        let mut macros = MacroConstantMap::new();
        macros.insert("A".to_string(), 10);
        // Simulates A * 5 where A=10
        assert_eq!(try_evaluate_text("A * 5", &macros), Some(50));
    }

    #[test]
    fn test_value_range_fits() {
        let r = ValueRange::new(0, 50000);
        assert!(r.fits_in_signed(32)); // [-2^31, 2^31-1] easily fits 50000
        assert!(r.fits_in_unsigned(16)); // [0, 65535] fits 50000
        assert!(!r.fits_in_unsigned(15)); // [0, 32767] doesn't fit 50000

        let r2 = ValueRange::new(-100, 100);
        assert!(r2.fits_in_signed(8)); // [-128, 127]
        assert!(!r2.fits_in_unsigned(8)); // negative min
    }

    #[test]
    fn test_value_range_mul() {
        let a = ValueRange::new(0, 50);
        let b = ValueRange::exact(1000);
        let result = a.mul(&b).unwrap();
        assert_eq!(result.min, 0);
        assert_eq!(result.max, 50000);
        assert!(result.fits_in_signed(32));
    }

    #[test]
    fn test_value_range_shl() {
        let a = ValueRange::exact(500);
        let b = ValueRange::new(0, 1);
        let result = a.shl(&b).unwrap();
        assert_eq!(result.min, 500);
        assert_eq!(result.max, 1000);
        assert!(result.fits_in_signed(32));
    }
}

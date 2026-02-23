# Juliet Benchmark - FP Reduction Plan

**Date**: 2026-02-13
**Baseline**: Round 2 results (43.0% TP rate, 736,563 FPs)
**Goal**: Identify and prioritize rule fixes for Round 3+

---

## Analysis Method

Aggregated per-rule TP/FP counts from Juliet ground truth analysis files (118 CWE categories, 54,484 files). Top-10 rules per category captured ~586K of 736K total FPs. Total violation counts from raw CSV parsing (4.0M violations).

---

## Tier A: Massive Volume (44% of all violations)

### DCL31-C — "Declare identifiers before using"
- **FP**: 162,720 | **TP**: 119,071 | **TP Rate**: 42.3% | **Total**: 847K (21% of all violations)
- **Net FP**: +43,649
- **Root cause**: Incomplete standard library function whitelist. Flags every function call not found in `declared_functions` unless it matches a small hardcoded list. Missing entire categories of stdlib/POSIX functions (`memcmp`, `localtime`, `div`, `abs`, etc.).
- **Fix**: Expand the hardcoded function whitelist significantly, or make header detection more robust (track which headers are included and what they declare).
- **Estimated impact**: 25-50K FP reduction

### DCL07-C — "Include type information in function declarators"
- **FP**: 162,606 | **TP**: 118,784 | **TP Rate**: 42.2% | **Total**: 840K (21% of all violations)
- **Net FP**: +43,822
- **Root cause**: `is_standard_function()` method deliberately returns `false` for ALL functions with comment "Don't skip standard functions - they should still be declared properly!" This causes every undeclared function call to be flagged.
- **Fix**: Implement standard function recognition with header awareness. At minimum, skip common functions (`printf`, `malloc`, `strlen`, etc.) when the corresponding header is included.
- **Estimated impact**: 25-50K FP reduction

### Combined DCL31-C + DCL07-C
These two rules generate **325K FPs** (44% of all false positives). Both share the same fundamental problem: over-flagging undeclared function calls. A shared standard function database would benefit both.

---

## Tier B: High Volume

### FLP34-C — "Ensure floating-point conversions are within range"
- **FP**: 65,386 | **TP**: 42,040 | **TP Rate**: 39.1% | **Total**: 292K (7.3% of all violations)
- **Net FP**: +23,346
- **Root cause**: Overly broad heuristic detection. Flags assignments based on target type containing "float" without verifying the source expression is actually floating-point. Range-check detection too lenient (any `isnan` call anywhere in function counts).
- **Fix**: Require source-side float type verification. Make range-check detection local to the conversion being checked.
- **Estimated impact**: ~20K FP reduction

### EXP34-C — "Do not dereference null pointers"
- **FP**: 14,643 | **TP**: 10,921 | **TP Rate**: 42.7% | **Total**: 85K
- **Net FP**: +3,722
- **Root cause**: Parameter classification marks any type with `_t` suffix (except `size_t`) as "potentially null pointer" — catches `uint_t`, `time_t`, `pid_t`, etc. which aren't pointers. Global null-state tracking doesn't handle reassignment properly.
- **Fix**: Only mark actual pointer types as potentially null. Track null-safety per-scope, not globally. Reduce the nullable function return list.
- **Estimated impact**: ~3-5K FP reduction

### ARR37-C — "Do not add/subtract integer to pointer to non-array object"
- **FP**: 12,300 | **TP**: 5,309 | **TP Rate**: 30.1% | **Total**: 68K
- **Net FP**: +6,991
- **Root cause**: Treats `Unknown` pointer type as unsafe (flags them). Many pointers can't be classified due to limited type inference but are actually safe array pointers.
- **Fix**: Flip Unknown pointer default: only flag if proven to be non-array, not if unproven to be array.
- **Estimated impact**: ~5-7K FP reduction

---

## Tier C: Low TP Rate (likely buggy/over-broad)

### INT07-C — "Use only explicitly signed or unsigned char"
- **FP**: 2,657 | **TP**: 63 | **TP Rate**: 2.3%
- **Root cause**: Flags ALL arithmetic/comparison operations involving plain `char`. Almost pure noise — normal C idioms like `char c; c + 1` trigger it.
- **Fix**: Only flag char operations where the result is used in a sign-dependent way (shifts, cross-type signed comparisons). Ignore simple loop counters and benign arithmetic.
- **Estimated impact**: ~2.5K FP reduction (small absolute, but fixes a nearly useless rule)

### ARR02-C — "Define explicit bounds for array objects"
- **FP**: 923 | **TP**: 36 | **TP Rate**: 3.8%
- **Root cause**: Style/maintainability rule. Flags `int arr[] = {1, 2, 3}` which is perfectly safe — compiler determines size from initializer.
- **Fix**: Only flag when initializer suggests size was misunderstood (designated initializers with gaps). Or disable by default since it's a recommendation, not a security rule.
- **Estimated impact**: ~900 FP reduction

### STR31-C — "Guarantee sufficient storage for strings"
- **FP**: 2,657 | **TP**: 221 | **TP Rate**: 7.7%
- **Root cause**: Over-ambitious static analysis with arbitrary thresholds. Buffers >= 256 bytes always considered safe. Names containing "hello"/"world" assumed safe. Buffers >= 50 bytes for strcat always safe. These heuristics create both FPs and FNs.
- **Fix**: Remove magic-number thresholds. Focus only on definitely-unsafe patterns (strcpy with unvalidated input, gets(), unbounded scanf %s).
- **Estimated impact**: ~2.4K FP reduction

### API02-C — "Functions that read/write arrays should take size argument"
- **FP**: 713 | **TP**: 85 | **TP Rate**: 10.7%
- **Root cause**: Requires `size_t` parameter immediately after pointer parameter. Too rigid — misses common patterns where size is at end of parameter list or shared across multiple pointers.
- **Fix**: Allow size_t anywhere in parameter list. Exclude standard library function prototypes.
- **Estimated impact**: ~600 FP reduction

### DCL13-C — "Declare function parameters const when not modified"
- **FP**: 1,035 | **TP**: 133 | **TP Rate**: 11.4%
- **Root cause**: Flags any non-const pointer parameter without full usage analysis. Only skips parameters named "src", "source", "input".
- **Fix**: Require more sophisticated usage analysis before flagging. Exclude library function signatures. Consider API compatibility reasons for non-const.
- **Estimated impact**: ~900 FP reduction

### EXP15-C — "Do not place a semicolon on the same line as an if/for/while"
- **FP**: 1,664 | **TP**: 268 | **TP Rate**: 13.9%
- **Root cause**: Text-based pattern matching instead of AST-level empty statement detection. Doesn't verify the semicolon creates an actual empty statement.
- **Fix**: Check for actual empty statement in AST rather than text matching. Verify following statement is uncontrolled.
- **Estimated impact**: ~1.4K FP reduction

---

## Recommended Rounds

### Round 3 (Highest Impact — Target: ~50-100K FP reduction)

| Priority | Rule | Estimated FP Reduction | Difficulty |
|----------|------|----------------------:|------------|
| 1 | DCL31-C | 25-50K | Medium (expand function whitelist) |
| 2 | DCL07-C | 25-50K | Medium (add standard function recognition) |
| 3 | FLP34-C | ~20K | Medium (add source-type verification) |

### Round 4 (Medium Impact — Target: ~15-20K FP reduction)

| Priority | Rule | Estimated FP Reduction | Difficulty |
|----------|------|----------------------:|------------|
| 4 | ARR37-C | 5-7K | Low (flip Unknown default) |
| 5 | EXP34-C | 3-5K | Medium (fix _t classification, scope null tracking) |
| 6 | INT07-C | ~2.5K | Low (tighten char arithmetic checks) |
| 7 | STR31-C | ~2.4K | Medium (remove magic thresholds) |

### Round 5 (Low Impact — Target: ~4K FP reduction)

| Priority | Rule | Estimated FP Reduction | Difficulty |
|----------|------|----------------------:|------------|
| 8 | EXP15-C | ~1.4K | Low (use AST for empty statement) |
| 9 | DCL13-C | ~900 | Low (better usage analysis) |
| 10 | ARR02-C | ~900 | Low (restrict to ambiguous cases) |
| 11 | API02-C | ~600 | Low (relax parameter ordering) |

---

## Projected Cumulative Impact

| After Round | Estimated TP Rate | Estimated FP Count | FP Reduction from Baseline |
|-------------|------------------:|-------------------:|---------------------------:|
| Round 2 (current) | 43.0% | 736,563 | -102,778 (-12.2%) |
| Round 3 (projected) | ~47-50% | ~640-690K | ~150-200K total |
| Round 4 (projected) | ~49-52% | ~620-670K | ~170-220K total |
| Round 5 (projected) | ~50-53% | ~615-665K | ~175-225K total |

---

## Data Sources

- Per-rule FP/TP: Aggregated from `/tmp/juliet_results/*_analysis.txt` (top-10 rules per CWE category)
- Total violations: Parsed from `/tmp/juliet_results/*.csv` (all rules, all categories)
- Rule implementations reviewed: `src/rules/cert_c/{category}/{rule}/`
- Note: FP/TP counts are from top-10 per category only (captures ~80% of classified violations)

---

## Pending Improvements (post-Round 13)

### STR31-C: `check_strcpy_safety` — Add `is_function_parameter` Guard

**Status**: Identified but not yet implemented (2026-02-22)

**Context**: Round 13 added a suppression to `check_strcpy_safety`: when the source is a string literal and the destination buffer size is unknown, assume safe (return `true`). This eliminated ~1,277 FPs in CWE134.

**Problem**: This suppression also fires on TPs in cross-function tests (CWE124, CWE127). In those tests, a small fixed-size stack buffer is passed to a helper function that calls `strcpy(data, "fixedstring")`. Inside the helper, `data` is a parameter — `find_buffer_size()` returns `None`, triggering the suppression and hiding the TP.

**Fix**: Gate the suppression on `!self.is_function_parameter(dest, source)`:

```rust
// In check_strcpy_safety, after the buffer_size block:
if source_length.is_some() && !self.is_function_parameter(dest, source) {
    return true;  // literal source + non-param dest of unknown size → assume safe
}
```

**Expected impact**: Recover ~300–400 TPs (CWE124/127 cross-function cases) with minimal FP regression, since the CWE134 FPs come from functions where the destination IS a parameter (good functions that accept a buffer and copy a fixed string into it — which is actually safe but unverifiable at the call site).

**Note**: `is_function_parameter()` searches the source file for function signatures containing the variable name — it's a file-level heuristic, not function-scoped. This is acceptable for this use case since the goal is to detect "is `dest` a formal parameter of any function?" rather than proving exact scope.

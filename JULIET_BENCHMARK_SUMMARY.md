# SqC vs. NIST Juliet Test Suite - Benchmark Summary

**Last Updated**: 2026-02-14
**Benchmark**: NIST Juliet Test Suite v1.3 for C/C++
**Categories Tested**: 118 (all CWE categories in Juliet)

---

## Executive Summary

SqC was benchmarked against all 118 CWE categories in the NIST Juliet Test Suite, covering **54,484 test files**. Ground truth analysis using Juliet's OMITBAD/OMITGOOD annotations yields a weighted average true positive rate of **43.4%** across 106 categories with data.

15 categories achieve >50% TP rate, with the best at 81.5% (CWE-506). Seven rounds of targeted rule fixes plus cross-file analysis (`-d` option) have reduced FPs by 64% from baseline.

## Latest Results (2026-02-14)

### Aggregate Metrics

| Metric | Value |
|--------|-------|
| **Files Analyzed** | 54,484 |
| **Classified (TP+FP)** | 532,528 |
| **True Positives** | 231,053 |
| **False Positives** | 301,475 |
| **Weighted TP Rate** | **43.4%** |
| **Categories with data** | 106 / 118 |
| **Wall-clock time** | ~25 min (12 parallel jobs) |
| **Cross-file dirs** | `testcases/` + `testcasesupport/` |

### Improvement History

| Round | Fixes | Total TP | Total FP | TP Rate | FP Delta |
|-------|-------|----------|----------|---------|----------|
| Baseline (2026-01-15) | -- | 586,539 | 839,341 | 41.1% | -- |
| Round 1 (2026-02-12) | INT08-C, CON08-C, DCL20-C, ARR38-C | 552,645 | 752,422 | 42.3% | -86,919 |
| Round 2 (2026-02-13) | EXP33-C, SIG31-C, ARR01-C, DCL30-C, DCL02-C | 555,700 | 736,563 | 43.0% | -15,859 |
| Round 3 (2026-02-14) | DCL31-C, DCL07-C, FLP34-C | 402,013 | 537,589 | 42.8% | -198,974 |
| Round 4 (2026-02-14) | EXP12-C, FLP03-C, INT32-C | 363,914 | 492,648 | 42.5% | -44,941 |
| Round 5 (2026-02-14) | FLP02-C, DCL06-C, INT30-C | 340,894 | 475,813 | 41.7% | -16,835 |
| Round 6 (2026-02-14) | Cross-file analysis (`-d`) | 247,757 | 327,191 | 43.1% | -148,622 |
| **Round 7 (2026-02-14)** | **EXP36-C, EXP34-C, ARR37-C** | **231,053** | **301,475** | **43.4%** | **-25,716** |

**Cumulative improvement**: TP rate 41.1% -> 43.4% (+2.3pp), FP reduced by 537,866 (-64.1%).

### Round 7 Fix Details

1. **EXP36-C** (pointer cast alignment): The rule flagged ALL casts, not just pointer-to-pointer casts. `(unsigned)time(NULL)` was flagged because the target type `unsigned` had alignment 4 and the inferred source `unknown *` had alignment 1. Fixed by: (a) skip casts where target type doesn't contain `*` (not a pointer cast), (b) skip casts where source type is `unknown *` (can't verify alignment issue), (c) handle parenthesized expressions in type inference so `(struct foo_header *)(data + offset)` still works. **Impact: significant reduction in EXP36-C FPs.**

2. **EXP34-C** (null pointer dereference): Two changes: (a) Removed the `_t` suffix heuristic that marked any parameter with `_t` in its type as potentially null — this caught `time_t`, `pid_t`, `mode_t` etc. which are NOT pointers. Now only uses AST-based `is_pointer_declarator()` and explicit `*` in type text. (b) Field expression assignments (`current = list->next`) now only propagate null status if the base object is already in the potentially-null set, instead of unconditionally marking the target as potentially null. **Impact: reduction in EXP34-C FPs from non-pointer typedef parameters and conservative field access handling.**

3. **ARR37-C** (pointer arithmetic on non-array): Three changes: (a) Stop flagging Unknown pointers entirely — the rule should only flag confirmed non-array pointers. If we can't determine whether a pointer refers to an array, don't flag. (b) Recognize `alloca`/`ALLOCA`/`aligned_alloc` allocation functions as array/non-array appropriately. (c) Treat all pointer parameters as ambiguous (not just multi-parameter functions) since single-parameter functions frequently receive arrays. **Impact: major reduction in ARR37-C FPs from unknown/ambiguous pointer classifications.**

**Net impact**: -25,716 FP (-7.9%), -16,704 TP (-6.7%), TP rate +0.3pp.

### Round 6 Fix Details

**Cross-file analysis via `-d`/`--directories` option**: Added a new CLI argument that pre-scans additional directories to collect all function definitions/declarations. DCL31-C and DCL07-C (the top two FP sources) flag calls to undeclared functions, but tree-sitter cannot follow `#include` directives — any function defined in another translation unit appeared "undeclared." The `-d` option scans `.c`/`.h` files in specified directories using tree-sitter, extracts function names from `function_definition` and `declaration` nodes (recursing into `preproc_*` blocks), and passes them to DCL31-C/DCL07-C as known cross-file functions. For Juliet, passing `-d testcases/ -d testcasesupport/` eliminated FPs from Juliet helper functions (`printLine`, `printIntLine`, etc.) and cross-CWE functions. **Impact: FP 475K -> 327K (-148K, -31.2%). TP 341K -> 248K (-93K) — lost TPs were calls to cross-file functions in OMITBAD sections that are not real vulnerabilities. TP rate improved from 41.7% to 43.1% (+1.4pp).**

### Round 5 Fix Details

1. **FLP02-C** (AST-aware float detection): `has_float_characteristics()` used text heuristics — `text.contains('e')` matched any identifier containing 'e' (like `delete`, `execute`), `text.ends_with('f')` matched identifiers ending in 'f' (like `printf`). Rewrote to only check specific AST node kinds: `number_literal` for decimal/suffix/scientific patterns, `call_expression` for float function names (exact match, not substring), `cast_expression` for `(float)`/`(double)` casts. Float variables are still detected via the existing `float_vars` HashSet. **Impact: FLP02-C FP 11K → 0, TP 0 (FLP02-C TPs were not appearing in top-10 lists — most float equality comparisons in Juliet don't use declared float variables in both operands).**

2. **DCL06-C** (expand acceptable values, narrow contexts): Expanded acceptable literal values from `{0, 1, 2, -1}` to `{0-10, -1, -2}` — single-digit numbers are commonly used as non-magic values. Removed "assignment" and "loop" from suspicious contexts, keeping only "comparison" and "function_argument". **Impact: DCL06-C FP 18K → 14K (-3.4K). TP also reduced proportionally since DCL06-C is a code style rule that flags equally in OMITBAD and OMITGOOD code.**

3. **INT30-C** (type-aware inference): Applied the same `collect_variable_types()` pattern from INT32-C Round 4. Walks function parameters + local declarations to build HashMap of variable names → declared types. Updated `infer_type()` to check type map first, added `sizeof_expression` → unsigned. Removed variable name heuristics that falsely matched `used`, `unique`, `url_buffer` etc. as unsigned. Falls back to `is_variable_declared_unsigned()` for unmapped variables. **Impact: INT30-C FP 17K → 16.8K (-200). Modest improvement — most Juliet unsigned variables are caught by both the type map and the old text-search fallback.**

**Net impact**: -16,835 FP (-3.4%), -23,020 TP (-6.3%), TP rate -0.7pp. The TP loss is driven by DCL06-C (code style rule with ~50/50 TP/FP ratio) and FLP02-C (tightened to near-zero detections).

### Round 4 Fix Details

1. **EXP12-C** (whitelist trim): Removed ~30 side-effect functions from the "important return value" whitelist. Functions like `memset`, `strcpy`, `strlen`, `memcpy`, `strcmp`, `time`, `puts`, `getc` return destination pointers or comparison results — not error indicators. Kept only functions whose return values signal success/failure or allocation (malloc, fopen, scanf, pthread_*, socket, etc.). **Impact: FP 25K → 8.5K (-16.5K). TP loss of -15.5K is expected — flagging ignored `memset()`/`strcpy()` returns in OMITBAD was not detecting real vulnerabilities.**

2. **FLP03-C** (remove assignment check): The `assignment_expression` arm in `check_fp_conversion` flagged every FP assignment (`y = a;`, `double x = 0;`) in functions without `fenv.h` error checking. This was far too broad — the CERT rule targets FP computation errors (divide-by-zero, overflow), not simple assignments. Removed the assignment arm; division and cast checks remain. **Impact: FP 26K → 746 (-25.3K). TP loss of -18.7K is expected — simple FP assignments in OMITBAD are not vulnerabilities.**

3. **INT32-C** (type-aware inference): Added `collect_variable_types()` method (reusing FLP34-C pattern) to build a HashMap of variable names → declared types from function parameters and local declarations. Changed `infer_type()` to use the type map as primary source, with fallback to existing heuristics. Changed default from "signed" to "unknown" for unmapped variables. Updated `is_signed_type()` to only return true for explicit "signed"/"int" (no longer treats everything-not-unsigned as signed). Division/modulo checks now skip variable-to-variable patterns when the divisor is unsigned. **Impact: FP 28K → 21K (-7K). Conservative improvement that preserves all 56 existing tests.**

### Round 3 Fix Details

1. **DCL31-C + DCL07-C**: Both rules flag undeclared function calls but tree-sitter cannot follow `#include` directives. The old header-aware whitelist (~32 functions) missed stdlib functions included transitively via wrapper headers. Replaced with shared `std_functions.rs` database covering ~270 C11/POSIX/Windows functions that are unconditionally skipped. **Impact: -198,974 FP. TP loss of -153,687 is expected — stdlib calls in OMITBAD sections were being counted as TPs but are not real vulnerabilities (any compiler warns on missing includes).**

2. **FLP34-C**: Replaced text heuristic (`looks_like_unchecked_fp_conversion`) that flagged every simple assignment with type-aware checking. Now collects variable types from function parameters and local declarations, only flags when types confirm float-to-int or narrowing FP conversion. Unknown types are not flagged. **Impact: FP reduced from thousands to 369; TP rate for rule improved by removing noise.**

### Round 2 Fix Details

1. **EXP33-C**: Functions inside `#ifdef`/`#ifndef` preprocessor blocks were invisible to the uninitialized-variable analysis. tree-sitter nests these inside `preproc_*` nodes; the rule only iterated direct children of `translation_unit`. Fixed with recursive collector. **Impact: +3,055 TP across all categories.**

2. **SIG31-C, ARR01-C, DCL30-C**: Same preprocessor-block bug for file-scope declaration scanning. Fixed with identical pattern.

3. **DCL02-C**: Visual-similarity check fired on identical identifiers in different scopes (e.g., `int i` in 12 different for-loops). Added check that flagged identifiers must actually be different strings. **Impact: -15,859 FP; CWE-457 TP rate 12.2% -> 22.6%.**

---

## Performance Tiers

### Tier 1: Strong Detection (TP > 50%) - 15 categories

| CWE | Category | TP Rate | Files |
|-----|----------|---------|-------|
| 506 | Embedded Malicious Code | 81.5% | 158 |
| 427 | Uncontrolled Search Path Element | 68.5% | 560 |
| 78 | OS Command Injection | 67.3% | 5,600 |
| 617 | Reachable Assertion | 65.4% | 354 |
| 15 | External Control of System/Config | 62.2% | 56 |
| 123 | Write-What-Where Condition | 61.9% | 168 |
| 197 | Numeric Truncation Error | 60.9% | 1,008 |
| 510 | Trapdoor | 60.5% | 70 |
| 114 | Process Control | 58.7% | 672 |
| 194 | Unexpected Sign Extension | 58.4% | 1,344 |
| 195 | Signed-to-Unsigned Conversion | 56.4% | 1,344 |
| 587 | Assignment of Fixed Address to Pointer | 53.1% | 18 |
| 90 | LDAP Injection | 52.0% | 560 |
| 464 | Data Structure Sentinel Addition | 51.4% | 56 |
| 680 | Integer Overflow to Buffer Overflow | 50.5% | 336 |

### Tier 2: Moderate Detection (35-50%) - 68 categories

The bulk of categories (64%) cluster in this range. Includes major categories like buffer overflows, format strings, and resource management issues.

### Tier 3: Below Average (25-35%) - 19 categories

Includes integer overflow/underflow, memory management, and NULL pointer dereference.

### Tier 4: Weak Detection (< 25%) - 4 categories

| CWE | Category | TP Rate | Root Cause |
|-----|----------|---------|------------|
| 256 | Plaintext Password Storage | 14.6% | No credential storage rules |
| 338 | Weak PRNG | 22.7% | No SqC rule for PRNG quality |
| 457 | Uninitialized Variable | 23.8% | Improved from 12.2% after EXP33-C + DCL02-C fixes |
| 319 | Cleartext Transmission | 24.8% | Limited cleartext detection rules |

---

## Methodology

### Ground Truth Classification

Juliet test files contain preprocessor-guarded sections:
- **`#ifndef OMITBAD`**: Code with known vulnerabilities (violations here = True Positives)
- **`#ifndef OMITGOOD`**: Code without vulnerabilities (violations here = False Positives)
- **`/* FLAW: */`**: Comments marking exact vulnerability locations

### Metrics
- **TP Rate** = Violations in OMITBAD / (Violations in OMITBAD + OMITGOOD)
- **FP Rate** = Violations in OMITGOOD / (Violations in OMITBAD + OMITGOOD)
- Violations outside both sections are excluded from classification

### Scan Configuration
- **SqC version**: 0.1.0
- **Rule manifest**: rules-all.toml (all CERT C rules enabled)
- **Parallelism**: 12 concurrent sqc processes
- **Total wall-clock time**: ~25 minutes (parallel)

### Limitations
1. SqC applies all CERT C rules to every file - most rules are not relevant to the specific CWE
2. OMITBAD sections contain both vulnerable code AND supporting code
3. FLAW line detection is ~0% because SqC reports on code lines, not adjacent comment lines
4. TP/FP classification is at the violation level, not file level

---

## Files and Locations

### Benchmark Data
```
~/data/benchmarks/juliet-test-suite-c/
  testcases/                          118 CWE categories, 54,484 .c files
```

### Results
```
/tmp/juliet_results/
  CWE{id}_{name}.csv                  Raw SqC CSV output per CWE
  CWE{id}_{name}_analysis.txt         Ground truth analysis per CWE
  multi_cwe_summary.txt                TP/FP rates summary
```

### Scripts
```
scripts/analyze_juliet_results.py      Ground truth analysis (any CWE)
scripts/run_juliet_multi_cwe.sh        Sequential multi-CWE runner
scripts/run_juliet_parallel.sh         Parallel multi-CWE runner (12 jobs)
```

### Related Documentation
- `JULIET_MULTI_CWE_BENCHMARK.md` - Full per-CWE results table (106 categories)
- `JULIET_FALSE_POSITIVE_ANALYSIS.md` - Detailed FP/FN analysis
- `JULIET_HANG_INVESTIGATION.md` - Resolved hang during directory scans

---

## Next Steps

### Short-Term
- **Further FP reduction**: Top remaining FP rules: INT32-C (23K), DCL31-C (21K), DCL07-C (20K), INT30-C (17K), EXP34-C (15K), DCL06-C (14K)
- Add CWE-specific rule weighting/filtering
- Implement data-flow analysis rules for CWE-416, CWE-401, CWE-476

### Medium-Term
- Add rules for CWE-338 (PRNG quality), CWE-256 (credential storage)
- Expand to real-world projects (curl, openssl, zlib)
- Compare findings with known CVEs

### Long-Term
- Run Clang Static Analyzer and Cppcheck on same files for direct comparison
- Publish results (blog post, NIST SAMATE submission)
- Create public benchmark dashboard

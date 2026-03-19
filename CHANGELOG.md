# SqC — Changelog

## v0.3.24 (2026-03-19)

### VRA Phase 5: Inter-Procedural Return Ranges

- `FunctionSummary` gains `return_range: Option<ValueRange>` — computed during prescan by evaluating all `return expr;` statements as constant ranges (literals, macros, sizeof, arithmetic). Conservative: `None` if any return is parameter-dependent or unevaluable.
- VRA transfer function resolves `call_expression` RHS in assignments and declarations: `int x = get_count();` uses callee's return range instead of full type range.
- Return ranges stored in `RangeAnalysisResult` for intra-block replay consistency in `eval_expr_range_at()` / `get_var_range_at()`.
- Prescan reordered: macro constants collected before function summaries so `#define`-based return values resolve correctly.
- Benefits all 4 VRA-consuming rules (INT30-C, INT32-C, INT33-C, INT34-C) — e.g., `x = get_nonzero(); y / x;` no longer flagged by INT33-C when `get_nonzero` provably returns `[1, N]`.

## v0.3.23 (2026-03-19)

### CFG-Based Forward Value-Range Analysis

New `value_range.rs` module implements proper forward dataflow on the CFG, replacing syntactic ancestor walks for integer range reasoning. Follows the same worklist pattern as `null_state.rs`.

**Phases 1–4: Core engine + rule migration**

- **Core VRA engine**: worklist algorithm with interval lattice, edge refinement for all comparison operators (`<`, `<=`, `>`, `>=`, `==`, `!=`), compound conditions (`&&`, `||`), negation, and bare identifier conditions
- **Type-aware initial ranges**: `unsigned int` → `[0, UINT_MAX]`, `int` → `[INT_MIN, INT_MAX]`, etc. Extracts signedness and bit width from declaration AST
- **Widening**: after 3 iterations of back-edge targets, growing dimensions widen to type bounds — guarantees termination for loops
- **Caching**: VRA computed once per file per function, shared across all rules via `set_vra_results()` trait method; only computed when at least one enabled rule requests it via `needs_vra()`
- **INT33-C**: `divisor_provably_nonzero()` tries VRA first via `eval_expr_range_at()`, falls back to syntactic analysis. Handles early-return guard patterns (`if (b == 0) return;`) and sequential assignments across blocks
- **INT34-C**: `check_shift_operation()` tries VRA first for shift amount range, falls back to syntactic analysis
- **INT32-C**: all 6 `expression_fits_in_signed` call sites replaced with VRA-backed `expression_fits_in_signed_vra()`
- **INT30-C**: all 4 `expression_fits_in_unsigned` call sites replaced with VRA-backed `expression_fits_in_unsigned_vra()`
- Added `PartialEq` derive to `ValueRange` in `const_eval.rs`

## v0.3.22 (2026-03-19)

### ARR38-C/ARR30-C False Positive Reduction

- ARR38-C: function-scoped alias resolution — `collect_pointer_aliases` now runs per-function instead of file-wide, preventing cross-function contamination (e.g., `data = dataBadBuffer` vs `data = dataGoodBuffer`). Eliminates ~69 CWE805 FPs.
- ARR38-C: skip heuristic checks when buffer size is verified — `is_hardcoded_large_size` no longer fires when `check_size_exceeds_buffer` already confirmed the copy fits the known buffer.
- ARR30-C: multi-assignment constant resolution — `try_resolve_variable_to_constant` now resolves to the last value when ALL assignments are constants (handles `data = -1; data = 7;` goodG2B patterns). Eliminates ~67 CWE129 FPs.
- ARR38-C CWE806 (−183 FP): `strncat(dest, data, strlen(data))` compared buffer allocation size instead of actual content. Fix: function-scoped `find_content_size_in_function()` tracks `memset(var, char, N)` and uses N as effective strlen bound.
- ARR30-C CWE129 (−67 FP): `check_if_bounds_against_size` searched full if-body text (matched for-loops). Fix: extract only `parenthesized_expression` condition from AST.

## v0.3.21 (2026-03-19)

### CWE-121/122: Buffer Overflow Detection

- ARR30-C: literal loop bounds, ALLOCA tracking, pointer alias tracking
- ARR38-C: ALLOCA detection, strlen/wcslen overflow, snprintf variants, pointer alias resolution, N*sizeof(type) parsing
- Benchmark: CWE-121 39.3%→39.9% TP rate (+205 TP, +281 FP), CWE-122 41.7%→36.6% (−5.1pp, +43 TP, +134 FP)

## v0.3.20 (2026-03-18)

### Benchmark Infrastructure Overhaul

- New `bench/` package replaces shell scripts with Python runner + SQLite
- `bench/runner.py`: `ProcessPoolExecutor`-based parallel CWE runner, writes directly to `data/benchmarks.db`
- `bench/analyzer.py`: TP/FP classifier extracted from `analyze_juliet_results.py`, returns structured data
- `bench/db.py`: SQLite schema (7 tables), WAL mode, full CRUD + query API
- `mcp_servers/server.py`: Updated to launch `python -m bench juliet`, queries SQLite first with legacy fallback
- `scripts/backfill_juliet_results.py`: Imported 21 Juliet runs + 7 real-world runs from markdown docs
- Fast mode default, resume support, machine metadata collection

### First 68-CWE Fast Benchmark

- Overall: 8,413 TP / 10,484 FP, 44.5% TP rate, 14.0% per-file
- 10 CWEs at 100% precision, 24 at zero detection
- 48 min on 4-core i5-6200U

## v0.3.19 (2026-03-15)

### CWE-78: ENV03-C + STR02-C Improvements

- ENV03-C: function-scoped clearenv() — checks sanitization per-function instead of file-level
- STR02-C: intra-function taint tracking (recv, fgets, fgetws, scanf, getenv, etc.) with cast handling and propagation
- Precision 42.0% → 45.5%, FP −330, TP −78 (cross-function patterns remain undetected)

## v0.3.18 (2026-03-14)

### Fast Benchmark Mode (CWE-Focused Manifests)

- `generate_rule_cwe_map.py` generates 147 per-CWE manifest TOMLs in `rules_templates/cwe/`
- `run_juliet_parallel.sh --fast` uses per-CWE manifests for targeted scanning
- Validated on CWE-476: noise drops from 61.8% → 0%, TP rate 39.5% → 46.5%, per-file detection unchanged (29.0%)

### CWE-194/195: Signed-to-size_t Implicit Conversion Detection (Priority 8)

- **INT31-C**: Added `check_call_argument_conversion()` — detects signed integer variables (`short`, `int`, `int32_t`, etc.) passed to functions expecting `size_t`. Covers 20 standard library functions including `malloc`, `calloc`, `realloc`, `memcpy`, `memmove`, `memset`, `strncpy`, `strncat`, `snprintf`, `fread`, `fwrite`, etc.
- Suppressions: explicit cast `(size_t)data`, `sizeof` expressions, numeric literals, limit-macro bounds check, non-negative guard
- Previously 0% CWE-matched detection on 2,688 Juliet files

### Continued FP Fixes

- Various false positive reduction improvements

## v0.3.17 (2026-03-12)

### CWE-78: Macro Alias + Windows API Coverage (Priority 6)

- Added `collect_macro_aliases()` to const_eval.rs — collects `#define ALIAS identifier` patterns
- ENV33-C, ENV03-C, STR02-C now resolve macro aliases before matching dangerous function lists
- Added Windows exec/spawn variants to ENV33-C: `_execl`, `_execv`, `_execlp`, `_execvp`, `_execle`, `_execve`, `_spawnl`, `_spawnle`, `_spawnlp`, `_spawnv`, `_spawnve`, `_spawnvp`
- **Benchmark**: CWE-78 CWE-matched TP 1,282, FP 1,773, precision 42.0%, per-file 13.0%

### CWE-253: Incorrect Return Value Check (Priority 7)

- ERR33-C now validates comparison correctness when a function call is directly embedded in a `binary_expression`
- Functions classified by `ErrorReturnKind`: NullPointer, NegativeInt, Eof, NonZero, Count
- Detects: pointer functions with ordered comparison, negative-on-error compared `== 0`, EOF-returning compared `== 0`, non-zero-on-error compared `== 0`, count-returning compared `< 0`
- Macro alias resolution added to ERR33-C; extended function coverage to wchar_t variants
- **Benchmark**: CWE-253 CWE-matched TP 178, FP 0, **100% precision**, 26.0% per-file detection

## v0.3.15 (2026-03-12)

### CWE-Aware Scoring System

- Implemented 5 new benchmark metrics: FLAW-line hit rate, CWE-matched TP rate, per-file detection rate, noise ratio, incidental TP/FP
- `scripts/generate_rule_cwe_map.py` produces `data/rule_cwe_map.json` (117 rules to 144 CWEs)
- Analysis pipeline fully integrated: `analyze_juliet_results.py`, `mcp_servers/server.py`, `run_juliet_parallel.sh`
- **Key finding**: 95% of Juliet findings are noise from unrelated rules; CWE-matched TP rate is 45.6% vs 44.4% incidental

### CWE Mapping Fixes

- Added CWE-124, CWE-126, CWE-127 mappings to ARR30-C, ARR38-C, STR31-C (buffer underwrite/overread/underread)

## v0.3.14 (2026-03-11)

### Juliet Regression Investigation

- Full-suite benchmark: 126,106 TP, 158,036 FP, 44.4% TP rate (-0.2pp from v0.3.5)
- Investigated 5 suspected root causes — all ruled out as dominant
- Confirmed regression is cumulative effect of many individually correct suppressions
- Discovered two scoring methodology issues: off-by-one in FLAW-line matching, incidental noise scored as TP

## v0.3.13

### EXP34-C Multi-Pass Prescan Propagation

- Added `propagate_param_null_states()` — multi-pass prescan resolves relay chains: `high(p) { if(!p) return; mid(p); }` → `mid(p) { low(p); }` → `low(p) { *p = 42; }` — `p` now NotNull at `low`

### EXP33-C For-Loop Init Recognition

- `has_preceding_assignment_in_block()` walks ancestor scopes
- `for_init_assigns_var()` recognizes for-statement init clauses as dominating assignments
- Handles `for (i = 0; ...)`, `for (int i = 0; ...)`, and comma expressions

### INT30-C Subtraction Guard

- `is_subtraction_guarded_by_comparison()` detects `if (a >= b) { a - b }` patterns
- Supports `>=`, `>`, `<=`, `<` and compound `&&` conditions
- Generalized `1U`/`1u` suffix handling in loop-bound and compound addition checks

## v0.3.8

### STR31-C Function Parameter Guard

- Gated string-literal suppression on `!is_function_parameter()` in `check_strcpy_safety` and `check_strcat_safety`
- Fixed `check_sequential_strcat_overflow` to scan only current function's line range
- Expected recovery: ~300-400 TPs on CWE-124/127

### EXP33-C Field/Subscript Write Fix

- Field/subscript write no longer treated as read (-576 FP, -299 TP on 12-CWE Juliet)

### API00-C Validation Pattern Expansion

- 4 new validation patterns recognized for parameter checking

## v0.3.5

### Struct Field Type Resolution

- Prescan collects struct definitions into `struct_field_types` in `ProjectContext`
- `infer_type()` resolves `field_expression` types (e.g., `s->count` → `unsigned int`)
- Integrated with INT32-C and INT30-C

## v0.3.3

### Suppression Elimination (d_lib_networking)

- **POS49-C**: Added `is_local_variable()` — skip stack-local struct member assignments
- **EXP12-C**: Already fixed — `connect() != 0` in binary_expression not flagged
- **INT30-C**: `is_literal_one()` strips unsigned/long suffixes; `is_preceded_by_increment()` checks for `var++`/`++var`/`var += 1` before subtraction

## v0.2.25

### STR04-C, INT18-C, EXP05-C Type/Const Fixes

- STR04-C: binary buffer skip — only flag `unsigned char` arrays with string literal evidence
- INT18-C: uint64_t recognition via `type_identifier` nodes
- EXP05-C: AST-based const detection replacing text-based check
- d_lib_networking: 51 → 47 violations (-4 FP)

## v0.2.22

### ARR02-C, POS02-C, PRE31-C, MEM05-C Fixes

- ARR02-C: skip implicit bounds check for string-literal-initialized arrays
- POS02-C: removed `socket`/`setsockopt` from privileged operations
- PRE31-C: strip string literal content before function-call pattern checks
- MEM05-C: ALL_CAPS macro constant VLA detection + word-boundary recursion matching

### INT32-C While/For Loop-Bound Detection

- Extended `is_inside_bounds_checked_block()` to while/for statements
- `extract_mutation_target()` ensures loop-bounded variable matches operation target

### INT30-C uint64_t Subtraction Skip

- Skip subtraction when either operand has declared type `uint64_t`

### Const-Eval Negative-Shift Clamp

- `ValueRange::shl()` clamps negative shift-amount lower bounds to 0

## v0.2.21

### Const-Eval / Value-Range Analysis

- New `src/analyze/const_eval.rs` module (~550 lines)
- `MacroConstantMap` for `#define` constant collection
- `ValueRange { min, max }` interval arithmetic
- `try_evaluate_expr()` / `try_evaluate_range()` for recursive AST constant folding
- `extract_loop_var_ranges()` for for/while/do loop bounds
- Integration with INT32-C (`expression_fits_in_signed`) and INT30-C (`expression_fits_in_unsigned`)
- d_lib_networking INT32-C: 10 → 8 (-2 FP via constant folding)

### Benchmark Measurement Fix

- Analysis script now outputs all rules (previously top 10 only)
- All 16 existing benchmark runs reanalyzed with full per-rule data
- Eliminated phantom regressions from top-10 truncation (POS02-C, ERR05-C, MEM06-C were artifacts)

## v0.2.20

### Real-World FP Fixes (d_lib_networking, Rounds 1-4)

- MSC37-C: `STATIC void` macro prefix — `has_void_specifier()` scans all children for `void`
- INT36-C: `(void)` discard cast — bare `void` no longer matched as pointer type
- PRE02-C: trailing comment stripping in macro values
- ERR33-C: `(void)` cast recognized as intentional discard
- CON03-C: skip `const`-qualified variables and synchronization primitive types
- DCL30-C: scalar value copy through pointer no longer flagged as address escape
- FIO47-C: snprintf argument count corrected (subtract 3, not 1)
- EXP37-C: init_declarator skip for K&R-style declarations
- API00-C: skip static functions (-12 FP) + caller-aware suppression via NotNull

### `-I`/`--include-path` Flag

- Pre-pass extracts `#include` directives, resolves against `-I` search paths
- Transitive include resolution with cycle prevention
- d_lib_networking: 223 → 205 violations (-18) with 3 include dirs

### INT01-C Dedup Fix

- Eliminated double-visit of `function_declarator`/`parameter_list` nodes (-3 duplicate violations)

### EXP34-C Stack Array NotNull

- Array declarations tracked as NotNull in prescan

### Juliet Benchmark

- v0.2.19 → v0.2.20: -2,720 FP, +0.1pp TP rate (44.1% → 44.2%)

## v0.2.17

### EXP34-C Phase 3: API Rule Narrowing + Prescan Enhancement

- MEM10-C positive guard suppression (-38 FP)
- API02-C `const wchar_t *` exclusion
- API00-C caller-aware suppression via function summaries
- Prescan local variable tracking for callsite null state resolution
- CWE-476: TP 313→320 (+7), FP 542→512 (-30), rate 36.6%→38.5%
- CWE-690 bonus: +36 TP, -63 FP

## v0.2.16

### EXP34-C Phase 2: Call-Site Null Propagation

- Call-site flagging for DefinitelyNull args
- Callee param seeding via `infer_arg_null_state()` in function_summary.rs
- Multi-pass aggregation with lattice join
- CWE-476: +19 TP, +17 FP (rate 35.9% → 36.6%)

## v0.2.15

### EXP34-C Phase 1: CFG-Based Null State Dataflow

- `src/analyze/null_state.rs` — forward dataflow with NullState lattice
- EXP34-C rewritten from ~1200-line linear walk to CFG-based analysis
- MEM10-C parameter-only null check fix (-106 FP on CWE-476)

### d_lib_common FP.md Round 2 (v0.2.14-0.2.15)

- Resolved all 17 FP patterns (~51 violations) from FP.md
- Key fixes: FIO46-C source-order stream tracking, INT32-C field_expression skip, FLP03-C scientific notation, EXP12-C parent-check, INT01-C sizeof skip
- Juliet: -10,678 FP (-5.4%), TP rate 44.7% → 44.2%

## v0.2.13

### INT31-C Implicit Narrowing Assignment Detection

- `check_assignment_conversion()` for `init_declarator` and `assignment_expression`
- Type width comparison with FP suppressions (double-flag, validated vars, bounds-check, literal-fits, bitmask)
- Real-world: curl +24, hostap +156, sqlite +49 new findings
- Juliet: 44.6% → 44.7% TP rate, -13,961 FP (-6.6%)

### d_lib_common REFACTOR.md Round 1

- DCL19-C: `STATIC` macro recognition
- INT32-C: skip unsigned operands in binary overflow checks
- DCL15-C: skip functions with prototypes in `.h` headers
- INT36-C: exclude struct field access and array subscript
- PRE31-C: skip string literal arguments from side-effect analysis
- EXP30-C: recognize `x = f(x)` as safe
- INT30-C: detect `if (var > 0)` guard before unsigned decrement
- DCL07-C/31-C: skip indirect calls and preprocessor-guarded blocks

### Prescan Infrastructure Improvements

- `linkage_specification` (`extern "C" {}`) traversal in all prescan walkers
- `pointer_declarator` handling for pointer-returning prototypes
- `header_declared_functions` field in `ProjectContext`

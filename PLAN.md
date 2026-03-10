# SqC — Plans & Action Items

**Last Updated**: 2026-03-09 (v0.3.8)

---

## Real-World FP Fixes — d_lib_networking (v0.2.20, in progress)

### MSC37-C: `STATIC void` macro prefix false positive (FIXED)

**Problem**: `STATIC void func()` where `STATIC` is a macro expanding to `static` was flagged as "non-void function has no return statement". Tree-sitter treats `STATIC` (unknown identifier) as the type field, putting `void` in an ERROR node. `is_void_type()` only checked the type field, so it saw `STATIC` instead of `void`.

**Fix**: Added `has_void_specifier()` to scan all direct children of the function_definition node for `void`, catching cases where a macro precedes the return type. Test case added.

### INT36-C: `(void)` discard cast false positive (FIXED)

**Problem**: `(void)fprintf(...)` was flagged as "unsafe integer-to-pointer conversion". Two bugs: (1) `is_pointer_type()` matched bare `void` (discard cast) as a pointer type. (2) `is_integer_type()` matched function names containing "int" as a substring (`fprintf` → "int"). Bug #1 was the gate that let #2 fire.

**Fix**: Removed `type_text.contains("void")` from `is_pointer_type()` — `void *` already matches via `*`. Bare `void` (discard cast) is not a pointer type. Eliminates 7 FPs on d_lib_networking.

### PRE02-C: Trailing comment in macro value (FIXED)

**Problem**: `#define FOO 50 // delay - reduced from 100` was flagged because tree-sitter includes the trailing `// ...` comment in the `preproc_def` value node. The ` - ` in the comment text matched the operator detection.

**Fix**: Strip trailing `//` comments from the value text before checking for operators. Eliminates 3 FPs on d_lib_networking.

### ERR33-C: `(void)` cast not recognized as intentional discard (FIXED)

**Problem**: `(void)fprintf(...)` and `(void)fflush(...)` were flagged as unchecked return values. `is_call_in_assignment_or_declaration()` walked through `cast_expression` parents but didn't recognize `(void)` as intentionally consuming the return value — it kept walking and hit `expression_statement`, returning false.

**Fix**: In the `cast_expression` arm, check if the cast target type is bare `void`. If so, return true (value is intentionally discarded). This is the standard CERT-C compliant pattern for acknowledging a discarded return value. Eliminates 2 FPs on d_lib_networking.

### CON03-C: const variables and synchronization primitives (FIXED)

**Problem**: `const char[]` arrays (read-only certificate data) and `pthread_mutex_t` were flagged as lacking thread synchronization. Read-only data cannot have data races. Mutexes ARE the synchronization mechanism.

**Fix**: Skip `const`-qualified variables (no data races on read-only data). Skip known synchronization primitive types (`pthread_mutex_t`, `pthread_rwlock_t`, `mtx_t`, `sem_t`, etc.). Eliminates 4 FPs on d_lib_networking.

### DCL30-C: scalar value copy through pointer (FIXED)

**Problem**: `*sock = sockfd` (where `sockfd` is `int`) was flagged as "local variable address escapes". But this copies the integer VALUE, not the ADDRESS of `sockfd`. The pointer `sock` was passed by the caller.

**Fix**: In the `pointer_expression` LHS case, only flag when the RHS local variable is a pointer/array type (actual address escape). Scalar value copies are safe. Eliminates 1 FP on d_lib_networking.

### DCL07-C/DCL31-C: `-I`/`--include-path` flag for header resolution (FIXED)

**Problem**: 75 of 223 violations were DCL07-C/DCL31-C false positives for functions declared in external headers (mbedtls, d_lib_common). The `-d` flag existed but required users to manually identify directories containing all source files.

**Fix**: Added `-I`/`--include-path` CLI flag (mirrors compiler convention). Pre-pass extracts `#include` directives from source files, resolves them against `-I` search paths (both `"quoted"` and `<angle>` forms), parses found headers, and merges function declarations into `ProjectContext`. Composes naturally with `-d` — both populate the same context. Headers parsed only once (deduped by canonical path).

**Result**: With `-I` pointing at 3 include dirs: 223→205 total violations (−18). DCL07-C/DCL31-C specifically: 75→66 (−9). Remaining 66 are functions from system headers or generated headers not available on disk.

### Round 2: Transitive includes, FIO47-C, EXP37-C, API00-C (v0.2.20)

**Transitive include resolution** (`prescan.rs`): Converted single-pass `#include` iteration to queue-based processing. After parsing a resolved header, its `#include` directives are extracted and enqueued for resolution (with the header's directory as the new source_dir). `resolved_set` prevents cycles. Result: 71 headers parsed (up from ~20 in single-pass). Remaining DCL07/31-C violations are for project-local functions (`Memory_Malloc`, `FileUtil_ReadFileIntoBuf`, `BasicTCPConnection_setBlockingState`) that aren't in any included header — best fixed by d_lib_networking adding explicit `#include` directives for headers it actually uses.

**FIO47-C snprintf arg count**: `count_arguments()` now subtracts 3 for `snprintf`/`vsnprintf` (buffer + size + format) instead of 1. −1 FP.

**EXP37-C init_declarator skip**: K&R-style declaration check now skips `declaration` nodes with an `init_declarator` child (e.g., `uint32_t count = MACRO_CALL();`). −2 FPs.

**API00-C static function skip**: `check_function_parameter_validation()` returns early for `static` functions (via `storage_class_specifier`) and `STATIC`/`STATIC_FUNC`/`STATIC_INLINE`/`STATIC_NOINLINE` macro prefixes. API00-C is about public API contracts — static functions are internal. −12 FPs.

**Result**: 155 → **137** total violations (−18). 12/15 FP patterns now resolved.

### Round 3: DCL13-C address-of-member + local alias (v0.2.20)

**DCL13-C address-of-member detection** (`dcl13_c.rs`): `arg_addresses_param_member()` detects `&(param->field)` passed as function argument. tree-sitter parses `&expr` as `pointer_expression` (not `unary_expression`). `expr_derives_from_param()` recursively walks `field_expression`, `subscript_expression`, `pointer_expression`, and `parenthesized_expression` to find the parameter at the root. −5 FPs (all `sock` params with `&(sock->ssl)`, `&(sock->server_fd)`, etc. passed to mbedtls functions).

**DCL13-C local pointer alias tracking**: `collect_pointer_aliases()` finds `T *local = param;` patterns in function body. If any alias is modified (or passed to a modifying call), the original parameter is considered modified. −1 FP (`buf` aliased as `cur = buf; recv(sock, cur, ...)`).

**Result**: 137 → **131** total violations (−6). 13/17 FP patterns resolved.

### Progress Summary

| Pattern | Rule(s) | FPs | Status |
|---------|---------|-----|--------|
| 2 | MSC37-C | 3 | FIXED |
| 3 | INT36-C | 7 | FIXED |
| 4 | POS49-C | 4 | Already resolved |
| 5 | CON03-C | 4 | FIXED (1 remains — legit finding) |
| 7 | DCL30-C | 1 | FIXED |
| 10 | PRE02-C | 3 | FIXED |
| 12 | ERR33-C | 2 | FIXED |
| 13 | EXP12-C | 2 | Already resolved |
| 1 | DCL07-C/DCL31-C | 75→15 | FIXED (`-I` flag + transitive resolution) |
| 8 | FIO47-C | 1 | FIXED (snprintf arg count) |
| 15 | EXP37-C | 2 | FIXED (init_declarator skip) |
| 16 | API00-C | 20→8 | FIXED (static function skip) |
| 17 | DCL13-C | 7→1 | FIXED (addr-of-member + alias) |
| 6 | EXP33-C | 1 | OPEN |
| 9 | MEM05-C | 2 | FIXED (macro constant VLA + recursion word boundary) |
| 11 | ARR32-C | 1 | FIXED (macro constant accepted) |
| 14 | PRE08-C | 3 | OPEN |
| 22 | ARR02-C | 3 | FIXED (string-literal-initialized arrays) |
| 23 | POS02-C | 3 | FIXED (socket/setsockopt not privileged) |
| 24 | PRE31-C (logging) | 2 | FIXED (string literal stripping) |
| 25 | INT32-C | 8→4 | FIXED (while/for bounds, const_eval shl clamp, local var chain) |
| 20 | EXP34-C | 4 | OPEN (callback params, boolean-guarded null, output param) |
| 21 | INT30-C | 6→5 | FIXED (uint64_t subtraction skip) |
| 36 | POS49-C | 1 | FIXED (local stack variable skip — v0.3.3) |
| 37 | EXP12-C | 1 | Already resolved (no code change needed) |
| 39 | INT30-C | 1 | FIXED (increment-then-subtract + 1U literal — v0.3.3) |

Total violations: 233 → 223 → 205 → 155 (with `-I`) → 137 → 131 (Round 3) → 123 (Round 4) → 86 (Round 5) → 71 (Round 6) → 68 (Round 7, `-I d_lib_common`) → 51 (Round 8, v0.2.23) → 51 (Round 9, v0.2.24) → 47 (Round 10, v0.2.25) → 36 (Round 11, codebase + config fixes) → **0** (Round 15, all resolved — 18 inline suppressions, 11 sqc FP). 3 more suppressions removable after v0.3.2 (POS49-C, EXP12-C, INT30-C).

### Round 4: INT01-C dedup + EXP34-C stack array (v0.2.20)

**INT01-C duplicate firing** (`int01_c.rs`): `check_size_params()` double-visited `function_declarator` and `parameter_list` nodes — once via explicit child iteration (lines 258–264) and again via general recursion (lines 270–274). Fix: skip already-handled node kinds in the general recursion when inside a `function_definition` or `function_declarator`. −3 duplicate violations (6→3).

**EXP34-C stack array NotNull** (`prescan.rs`): `collect_assignments_recursive()` didn't track array declarations, so local arrays like `unsigned char cert_buf[N]` weren't recognized as NotNull at call sites. Fix: detect `array_declarator` children in declaration nodes and mark them NotNull (stack arrays can never be null). −1 FP.

**Result**: 131 → **123** total violations (−8). 15/17 FP patterns resolved.

### Juliet Benchmark: v0.2.19 → v0.2.20

| Metric | v0.2.19 | v0.2.20 | Delta |
|--------|---------|---------|-------|
| TP | 145,639 | 144,278 | -1,361 |
| FP | 184,644 | 181,924 | **-2,720** |
| TP Rate | 44.1% | **44.2%** | **+0.1pp** |

No CWE regressions (all deltas are improvements or neutral).

**Top rule changes** (full per-rule data): API00-C −1,917 FP (static skip), INT01-C −231 FP (dedup), EXP34-C −221 FP (array NotNull), DCL30-C −201 FP, FIO47-C −88 FP. **No rule regressions** (previously reported POS02-C/ERR05-C/MEM06-C regressions were a measurement artifact from the top-10-only aggregation — see "Benchmark Measurement Fix" section below).

Net: strongly positive (−2,720 FP, +0.1pp TP rate).

### Round 6: ARR02-C, POS02-C, PRE31-C, MEM05-C (v0.2.22)

**ARR02-C string-literal-initialized arrays** (`arr02_c.rs`): Skip implicit bounds check when `init_declarator` value is a `string_literal` or `concatenated_string`. `const char name[] = "..."` is standard C — the compiler determines the size from the initializer. −3 FPs (AWS cert blobs in ca.c).

**POS02-C unprivileged socket operations** (`pos02_c.rs`): Removed `socket` and `setsockopt` from `is_privileged_operation()`. These are standard unprivileged networking calls — only `SOCK_RAW` requires `CAP_NET_RAW`, and `setsockopt` for `SO_RCVTIMEO`/`SO_SNDTIMEO` is unprivileged. Kept `bind`/`listen` (server-side privileged port pattern). −3 FPs.

**PRE31-C string literal stripping** (`pre31_c.rs`): Added `strip_string_literals()` to remove content inside quotes before checking for function-call patterns. `NW_LOGE(PR "...mbedtls_ssl_write()..." PW)` was flagged because `contains_any_function_call` found `mbedtls_ssl_write()` inside the quoted portion — the arg starts with `PR` not `"`, so the existing string-literal skip at the top didn't trigger. All side-effect checks now operate on the stripped text. −2 FPs.

**MEM05-C macro constant VLA + recursion word boundary** (`mem05_c.rs`): (a) Added `is_likely_macro_constant()` — ALL_CAPS identifiers are conventionally preprocessor constants, not runtime values. `unsigned char cert_buf[SSL_CERT_BUFFER_SIZE]` no longer flagged as VLA. (b) `count_word_matches()` uses word-boundary matching for recursion detection — `pthread_mutex_init` no longer matches as a recursive call to `mutex_init`. −2 FPs.

**Result**: 86 → **71** total violations (−15 total since Round 5, −10 from these fixes). 19/23 FP patterns resolved.

### Round 7: INT32-C while/for bounds, INT30-C uint64_t, const_eval shl (v0.2.22)

**INT32-C while/for loop-bound detection** (`int32_c.rs`): Extended `is_inside_bounds_checked_block()` to check `while_statement` and `for_statement` ancestors (was if_statement only). Added `extract_mutation_target()` to only suppress when the loop-bounded variable matches the operation's TARGET — prevents false negatives like `sum += array[i]` inside `for (i < N)` where `i` is bounded but `sum` is not. Added `extract_loop_bounded_vars()` to parse comparison operators from loop conditions. −3 violations (`attempts++`, `SSL_CONNECTION_BASE_DELAY_MS * (1 << ...)`, `1 << (attempts - 1)` — all inside `while (attempts < SSL_MAX_CONNECTION_ATTEMPTS)`).

**const_eval shl negative-clamp** (`const_eval.rs`): `ValueRange::shl()` now clamps negative shift-amount lower bounds to 0 instead of returning None. Negative shifts are UB in C, so in correct programs only non-negative values are reachable. This unblocked the `1 << (attempts - 1)` evaluation chain: with attempts ∈ [0,1] from loop range, `attempts - 1` ∈ [-1, 0], clamped to [0, 0], `1 << 0 = 1`. Then `500 * 1 = 500`, `delay_ms = 500`, `500 * 1000 = 500000` fits in int32. −1 violation (`delay_ms * 1000`).

**INT30-C uint64_t subtraction skip** (`int30_c.rs`): Added `any_operand_64bit_unsigned()` check in `check_subtraction()`. When either operand has declared type `uint64_t` (or equivalent 64-bit unsigned type), subtraction is skipped — 2^64 wraparound is practically impossible for real-world values. Uses `get_declared_type()` to look up the actual declared type from `type_map` (not the lossy "unsigned" from `infer_type()`). −1 violation (`BISSELL_TIMER_MS_SINCE_BOOT() - start_time_ms`).

**stdout/stderr/stdin NotNull** (`function_summary.rs`): Added standard C stream identifiers as known NotNull in `infer_arg_null_state()`. General improvement for prescan null-state inference (no d_lib_networking impact — the EXP34-C violations at line 60 are for callback parameters, not stream identifiers).

**Result**: 100 → **95** total violations (−5). INT32-C 8→4, INT30-C 6→5, EXP34-C 4→4.

**Remaining INT32-C** (4): `-err` negation (INT_MIN UB), `DATAMODEL_GET_*() + 1` ×2 (opaque function return), `memcpy` size cast (AST parsing issue).

**Remaining INT30-C** (5): All `+1` patterns on function return values or loop counters without provable upper bounds.

**Remaining EXP34-C** (4): Callback params from mbedtls (2), output parameter deref (1), boolean-guarded null check (1). All require deeper interprocedural analysis.

### Round 10: STR04-C, INT18-C, EXP05-C type/const fixes (v0.2.25)

**STR04-C binary buffer skip** (`str04_c.rs`): `check_string_declaration()` now only flags `unsigned char` arrays with string literal evidence (string_literal or concatenated_string in initializer). Bare `unsigned char buf[N]` without string init is a binary buffer (cert data, key data), not a text string. −2 FPs.

**INT18-C uint64_t recognition** (`int18_c.rs`): `has_larger_type_specifier()` now checks `type_identifier` nodes (tree-sitter parses `uint64_t` as type_identifier, not primitive_type). Added `operand_has_larger_declared_type()` to check if RHS operands are already declared as the larger type — if `start_time_ms` is `uint64_t`, subtraction already happens in 64-bit. −1 FP.

**EXP05-C const detection** (`exp05_c.rs`): Replaced text-based `check_body_for_const()` with AST-based `declaration_declares_const_var()`. Old code did `decl_text.contains("const") && decl_text.contains(var_name)` which matched `const` from cast expressions in initializers (e.g., `bool x = f((const T*)&servaddr)` triggered because "const" and "servaddr" both appeared in the declaration text). New code checks only `type_qualifier` children for `const`. −1 FP.

**Result**: 51 → **47** total violations (−4 FP). FP rate 57% → 45%.

### Round 12: POS49-C, EXP12-C, INT30-C suppression elimination (v0.3.3)

Targeted removal of 3 inline FP suppressions in d_lib_networking by fixing the underlying sqc rules.

**POS49-C local stack variable skip** (`pos49_c.rs`): Added `is_local_variable()` — walks up to enclosing `function_definition`, searches body for a local `declaration` of the base variable (excluding `extern`/`static`). `has_local_declaration()` recurses through declaration nodes matching variable name. Conservative: pointer dereferences (`(*ptr).field`) still flagged since pointed-to memory could be shared. −1 FP (`servaddr.sin_port = htons(port)` where `servaddr` is a stack-local `struct sockaddr_in`).

**EXP12-C already fixed**: `if (connect(...) != 0)` no longer triggers EXP12-C — the call_expression inside a binary_expression (comparison) is not within an expression_statement, so `check_for_ignored_return_values` is never invoked. Suppression can be removed with no code changes.

**INT30-C increment-then-subtract** (`int30_c.rs`): Two fixes to `is_subtract_one_guarded()`:
1. `is_literal_one()` — accepts `1U`, `1u`, `1UL`, etc. (strips unsigned/long suffixes before comparing to "1"). Previously only matched bare `1`.
2. `is_preceded_by_increment()` — checks if `var++`, `++var`, or `var += 1` appears before the subtraction in the same `compound_statement`. Proves `var >= 1` at the subtraction point. −1 FP (`attempts - 1U` where `attempts++` runs at the top of the while loop body).

**Result**: 3 suppressions can be removed from d_lib_networking (POS49-C, EXP12-C, INT30-C). All 2814 tests pass, no regressions.

### Const-Eval / Value-Range Analysis (v0.2.21)

**Problem**: sqc treats `#define` macro constants as unknown variables, so `SSL_CERT_RETRY_DELAY_MS * 1000` (where the macro is 50) gets flagged as potential overflow even though `50 * 1000 = 50000` trivially fits in `int`.

**Implementation**: New `src/analyze/const_eval.rs` module (~550 lines) with:
- `MacroConstantMap` — collects `#define NAME value` constants from tree-sitter `preproc_def` nodes
- `ValueRange { min, max }` — interval arithmetic with checked `add`, `sub`, `mul`, `shl` operations
- `try_evaluate_expr()` — recursive AST constant folder (handles literals, macro refs, binary/unary ops)
- `try_evaluate_range()` — range-based variant returning `ValueRange` instead of exact values
- `extract_loop_var_ranges()` — walks AST ancestors for `for`/`while`/`do` loop bounds, supports compound `&&` conditions
- `resolve_local_var_range()` — scans backward in enclosing compound_statement for `type var = expr` assignments
- `expression_fits_in_signed()`/`expression_fits_in_unsigned()` — convenience wrappers combining all of the above

**Integration**:
- `ProjectContext.macro_constants` — cross-file macros collected during prescan and include resolution
- INT32-C: `RefCell<MacroConstantMap>` fields, `set_project_context()`, early return in all 8 arithmetic check functions when `expression_fits_in_signed()` proves safety
- INT30-C: same pattern with `expression_fits_in_unsigned()`
- Comment stripping for trailing `//` in `#define` values (tree-sitter includes comment text in `preproc_arg`)
- Negative-shift UB guard: refuses to suppress left-shift when left operand range includes negatives
- Compound assignment guard: only resolves RHS identifiers, not LHS (mutation target in loops)

**d_lib_networking results**: INT32-C 10→8 (−2), INT30-C 6→6 (unchanged). The 2 suppressed: `SSL_CERT_RETRY_DELAY_MS * 1000` (50×1000=50000) and `SSL_HANDSHAKE_RETRY_DELAY_MS * 1000` (250×1000=250000).

**Remaining INT32-C** (6): require flow-sensitive analysis — post-increment state (`attempts++`), cross-statement propagation, parameter bounds. Beyond syntactic const_eval scope.

**Remaining INT30-C** (6): `cert_buflen + 1` from function returns, loop-bounded `cur += 1` (needs flow-sensitive knowledge that loop variable ≥ initial value at usage point).

**Test impact**: 1 test moved from fail/ to pass/ (`testcases_shift_over.c` — 1000000 << 10 = 1,024,000,000 fits in INT_MAX). 8 new const_eval unit tests. All 2801 tests pass.

---

## Benchmark Measurement Fix (v0.2.21)

**Discovery**: The MCP benchmark server's `compare_runs()` aggregated per-rule TP/FP from only the **top 10 rules** per CWE analysis file. This produced lossy data that created phantom regressions and inaccurate per-rule deltas.

**Impact on v0.2.20**: The reported "regressions" — POS02-C +840 FP, ERR05-C +395 FP, MEM06-C +191 FP — were entirely phantom. Verified by comparing raw CSV violations: all three rules had **zero actual change** between v0.2.19 and v0.2.20. The apparent increase was caused by API00-C dropping out of the top 10 FP list in several CWEs (due to the static function skip), which promoted these other rules into the visible window.

**Historical impact**: Per-rule deltas documented in JULIET_RESULTS.md have varying accuracy. Total TP/FP counts and TP rates were always correct (computed from full data). Per-rule numbers were approximately correct for dominant rules but could be significantly off for rules near the top-10 boundary. The direction of changes (improvement vs regression) was generally correct for major rules.

**Fix**: Analysis script now outputs all rules (not top 10). All 16 existing benchmark runs reanalyzed with full per-rule data. MCP server parser updated to handle both old and new format.

---

## EXP34-C / CWE-476 Improvement Roadmap

Goal: Raise CWE-476 (NULL Pointer Dereference) TP rate from 37.6% toward 80%+.

### Phase 1 — CFG-based Null State Dataflow (COMPLETE)

- [x] `src/analyze/null_state.rs` — forward dataflow with NullState lattice, edge refinement, assert recognition
- [x] EXP34-C rewritten to use CFG-based analysis (replaced ~1200-line linear walk)
- [x] MEM10-C parameter-only null check fix (−106 FP on CWE-476)
- [x] DCL13-C main() exemption (no benchmark impact)

### Phase 2 — Call-Site Null Propagation (COMPLETE)

Cross-file variants (51–68) have null assignment in file A and dereference in file B.

- [x] **Call-site flagging (Approach A)**: Re-enabled `check_callsite_null_args` — flags DefinitelyNull args passed to callees that don't null-check. Removed `dereferences_params` gate, added `is_null_safe_function()` whitelist.
- [x] **Callee param seeding (Approach B)**: `infer_arg_null_state()` in `function_summary.rs` classifies call-site args via AST. Prescan second pass (`collect_callsite_null_states` in `prescan.rs`) aggregates per-callee per-param with lattice join. Header-declared functions get implicit Unknown caller to prevent false NotNull seeding.
- [x] **Param state wiring**: `analyze_null_states_with_globals()` accepts `func_name` param, seeds pointer params from `callsite_param_null_states` instead of blanket PossiblyNull.

**Juliet results** (0.2.16 vs 0.2.15): CWE-476 +19 TP, +17 FP (TP rate 35.9% → 36.6%). Overall +19 TP, +11 FP (44.8% unchanged).

**Real-world results** (0.2.16 vs 0.2.13): -5,892 violations (-1.3%) across 5 codebases. Modest impact — most real-world functions receive mixed null/non-null args, limiting all-NotNull param seeding benefit.

**Remaining gaps**: Relay chains (variants 52–54) produce Unknown from AST inference (param forwarding). Indirect data flow (63–67) and cross-file globals (68) not addressed. EXP33-C CFG integration deferred.

### Phase 3 — API Rule Narrowing + Prescan Enhancement (COMPLETE)

Targeted CWE-476 FP reduction via rule narrowing and enhanced inter-procedural analysis.

- [x] **MEM10-C positive guard suppression**: Suppress when condition is `!= NULL` / bare truthiness and param is only used inside guarded block. −38 FP, 0 TP loss (clean elimination).
- [x] **API02-C `const wchar_t *` exclusion**: Extended existing `const char *` skip to wide strings. Original plan to skip all `char *` was too aggressive — `char *` as destination buffer correctly requires size param.
- [x] **API00-C caller-aware suppression**: Converted to stateful struct with `function_summaries`. Added `set_project_context()` to receive summaries. If callsite param state is NotNull → suppress violation.
- [x] **Prescan local variable tracking**: `collect_local_var_states()` scans function bodies for assignments (`var = NULL` → DefinitelyNull, `var = "str"` → NotNull, etc.). `collect_calls_with_locals()` resolves identifier args via local state instead of returning Unknown.
- [x] **Variant 45 global tracking verified**: `badSink()` correctly flagged, `goodG2BSink()` and `goodB2GSink()` correctly suppressed. No code changes needed.

**Juliet results** (0.2.17 vs 0.2.16): CWE-476 TP 313→320 (+7), FP 542→512 (−30), TP rate 36.6%→38.5% (+1.9pp). CWE-690 bonus: +36 TP, −63 FP. Overall TP rate 44.2% (unchanged).

**Side benefits**: Prescan enhancement improved FIO03-C (−169 FP), ERR05-C (−105 FP), FIO20-C (−102 FP).

**Known regressions**: EXP34-C +76 FP, FIO06-C +169 FP (side effects of enhanced prescan providing more inter-procedural data). Candidates for future investigation.

### Phase 4 — Remaining Edge Cases

- Relay chains (variants 52–54): prescan returns Unknown for param forwarding
- Indirect data flow (variants 63–67): not addressed
- Cross-file globals (variant 68): not addressed
- EXP33-C CFG integration (deferred — needs full CFG rewrite like EXP34-C)
- EXP34-C/FIO06-C regression investigation from Phase 3
- Target: 80%+ CWE-476 TP rate

---

## Juliet FP Reduction — Pending Improvements

### STR31-C: `check_strcpy_safety` — Add `is_function_parameter` Guard (COMPLETE v0.3.8)

**Status**: Implemented on `str31_fp_fix` branch (v0.3.8).

**Problem**: Round 13 added a suppression: when source is a string literal and dest buffer size is unknown, assume safe. This suppression also fires on TPs in cross-function tests (CWE124, CWE127) where a small stack buffer is passed to a helper that calls `strcpy(data, "fixedstring")`.

**Fix**: Gated the suppression on `!self.is_function_parameter(dest, source)` in both `check_strcpy_safety` and `check_strcat_safety`. Also fixed `check_sequential_strcat_overflow` to scan only the current function's line range (not the whole file) and eliminated re-parsing in `analyze_cumulative_strcat` by reusing the root node from `check()`.

**Expected impact**: Recover ~300–400 TPs (CWE124/127) with minimal FP regression.

### INT31-C: Implicit Narrowing Assignment Detection (COMPLETE)

- [x] Enable `check_assignment_conversion()` in INT31-C

**Implemented** (0.2.13): `check_assignment_conversion()` detects implicit narrowing in
`init_declarator` and `assignment_expression` nodes. Uses `get_type_width()` (8/16/32/64)
and `infer_rhs_width()` (dispatches on cast_expression, identifier, parenthesized_expression).
FP suppressions: double-flag prevention (RHS cast already caught), validated vars, bounds-checked
blocks, literal-fits-in-width, safe bitmask (`& 0xFF`). Conservative: returns None for unknown
types — only flags when both sides have known widths.

**Origin**: d_lib_common FN-001 — real data corruption bug caught manually, not by sqc.

**Benchmark results** (0.2.13 vs 0.2.12):
- Juliet CWE197: -9 TP / -9 FP (unchanged — Juliet uses explicit casts)
- Juliet overall: 44.6% → 44.7% TP rate (+0.1pp), -13,961 FP (-6.6%)
- Real-world INT31-C new findings: curl +24, hostap +156, sqlite +49, mosquitto +0 (229 total)
- Real-world total: 545K → 461K violations (-15.4% across 4 codebases)

### INT34-C: Literal Shift Amount >= Type Width

Current fix skips all non-negative integer literals to eliminate FPs from `x >> 8` etc. This means we miss the case where the literal is >= the promoted type width (e.g. `uint8_t x; x << 32;`). Compilers warn with `-Wshift-count-overflow`. Low priority — requires knowing promoted operand type.

### Top Remaining FP Rules (v0.2.20, full per-rule data)

| Rule | FP | TP | FP% | Notes |
|------|---:|---:|----:|-------|
| DCL06-C | 15.9K | 19.1K | 46% | Code style — reductions lose TPs proportionally |
| INT30-C | 13.6K | 12.3K | 52% | Guard expansion in v0.2.18 |
| INT32-C | 11.1K | 6.7K | 62% | field_expression → not_applicable reduced both |
| EXP33-C | 8.5K | 6.7K | 56% | |
| ERR05-C | 7.3K | 4.1K | 64% | |
| ERR33-C | 7.1K | 5.2K | 58% | Nested calls + math overlap fixed |
| INT36-C | 6.8K | 4.1K | 62% | |
| EXP12-C | 6.4K | 4.4K | 59% | Parent-check added |
| DCL00-C | 5.1K | 3.0K | 63% | |
| MEM04-C | 5.1K | 2.8K | 64% | |
| MEM06-C | 5.0K | 2.9K | 64% | |
| API00-C | 4.5K | 3.5K | 56% | Static function skip in v0.2.20 |

**Key insight**: Most remaining top FP rules have ~50–65% FP ratios. Further rule tuning will proportionally lose TPs. The ~44–45% Juliet ceiling is likely an architectural constraint for single-TU analysis. Higher-value gains will come from structural improvements (cross-function analysis, value-range analysis) rather than per-rule tuning.

---

## Real-World FP — Remaining Issues (after v0.2.18)

Verified against d_lib_common (7,694 violations) and d_hal_linux_random (45 violations).
Of the original 17 d_lib_common patterns + 6 d_hal_linux_random patterns, 12 are fixed. 8 remain.

| # | Rule | Violations | Difficulty | Description |
|---|------|--------:|------------|-------------|
| 1 | **INT30-C** | ~12 | Medium | Loop-bounded increments (`index += 1` where `index < bufferSize`), addition guarded by branch. v0.3.2: increment-then-subtract pattern + `1U` literal suffix now handled |
| 2 | **EXP33-C** | ~36 | Medium | For-loop init not recognized; array declarations without initializers. v0.3.8: field/subscript write no longer treated as read (−576 FP, −299 TP on 12-CWE Juliet) |
| 3 | **INT33-C** | ~7 | Hard | Division guarded by earlier comparison (`lower < upper` → divisor ≥ 2). Needs value-range |
| 4 | **INT34-C** | ~1 | Hard | Shift bounded by loop iteration count. Needs value-range |
| 5 | **EXP34-C** | ~28 | Medium | Helper functions called only after caller validates params with early-return null guard. v0.3.8: compound `\|\|` null guard now collects all vars |
| 6 | **MEM30-C** | ~1 | Hard | Sequential struct/member frees (`free(s->items); free(s);`). Needs field-level tracking |
| 7 | **MEM31-C** | ~9 | Hard | Cross-function ownership (`strdup` into struct field, freed via custom `_Delete`). Needs ownership model |
| 8 | **API00-C** | ~18 | Easy | Validation present but after variable declarations; static helper functions called from validated callers. v0.3.8: 4 new validation patterns recognized (DONE) |

### Actionable Now (v0.2.19 targets)

**Issue 1 — INT30-C loop-bounded increment**: Inside `while (index < bufferSize)` or `for (...; index < limit; ...)`, `index + 1` / `index++` / `index += 1` cannot wrap (counterpart of `is_guarded_by_gt_zero` for decrements). Detect enclosing loop condition with `var < expr` or `var <= expr` pattern.

**Issue 5 — EXP34-C prescan null guard**: Enhance prescan `collect_local_var_states()` to detect early-return null guard patterns on function parameters (`if (p == NULL) return;`). After such a guard, the param is NotNull at subsequent call sites. This feeds into Phase 2 callee param seeding.

**Issue 8 — API00-C validation past declarations**: `check_validation_patterns()` needs to scan past `declaration` nodes in the function body to find `if_statement` validation. Also: static helper functions called only from validated contexts should be suppressed (caller-aware already partially works via Phase 3).

### Deferred (require new analysis capabilities)

- **Issue 3 (INT33-C)** / **Issue 4 (INT34-C)**: Need value-range analysis to prove divisor ≠ 0 or shift < width from enclosing conditions
- **Issue 6 (MEM30-C)**: Needs field-level alias tracking to know `self` is still valid after `free(self->items)`
- **Issue 7 (MEM31-C)**: Needs cross-function ownership model (struct field allocated in constructor, freed in destructor)

---

## Real-World FP Fixes (d_lib_common)

Targeted FP reduction driven by findings from `~/data/d_lib_common/REFACTOR.md` and `~/data/d_lib_common/FP.md`.

### Round 2: FP.md Cleanup (0.2.14)

Addressed 17 FP patterns (~51 violations) documented in `FP.md`. All 17 patterns resolved (14 fixed, 3 already resolved by Batch 1 cascading effects).

| Pattern | Rule | FPs | Fix |
|---------|------|----:|-----|
| 1 | FIO46-C | 7 | Source-order stream tracking: store `start_byte()` in `closed_streams`, only flag when call occurs AFTER fclose |
| 3 | INT32-C | 5 | Return `not_applicable` for `field_expression` nodes without type evidence |
| 5 | FLP03-C | 2 | Precise scientific notation regex (digit before/after e/E), check operands individually in division |
| 7 | EXP40-C | 2 | Check parent `declaration` node for `const` type qualifier before flagging |
| 8 | EXP12-C | 2 | Check `node.parent()` kind — skip when parent is `assignment_expression`, `init_declarator`, etc. |
| 11 | INT01-C | 2 | Skip `sizeof(...) * N` binary expressions in allocation args |
| 12 | INT10-C | 1 | Add `collect_variable_types()` and `operand_has_unsigned_type()` for struct field type resolution |
| 14 | ARR39-C | 1 | Skip ALL_CAPS-only arithmetic (macro/enum constants are integers, not pointers) |
| 15 | EXP05-C | 1 | Don't recurse into `function_definition` nodes when scanning global scope for const declarations |
| 16 | EXP02-C | 1 | Exempt `NULL_CHECK && FUNCTION_CALL` guard pattern from side-effect warning |
| 2,6,9,10,13,17 | Various | ~30 | Resolved by cascading effects of Batch 1 fixes (no longer triggered) |

**Result (commit 1 — `d31a6c3`)**: 0/17 FP patterns remain. d_lib_common FP-report violations dropped from ~51 to ~6.

**Follow-up (commit 2 — `0a45c9f`)**: 6 remaining violations fixed:
- INT32-C (×3): propagate unsigned type through binary_expression chains (`unsigned_char - 'a' + 10`)
- INT10-C (×1): skip `field_expression` operands in modulo sign check
- EXP05-C (×1): skip `field_expression` in const-qualification check (base pointer const ≠ member const)
- ARR39-C (×1): recursive `is_all_caps_arithmetic()` for nested binary_expressions (`A + B + C`)

All original FP.md violations now resolved (0 remaining).

**Juliet benchmark** (0.2.15 vs 0.2.13): -10,678 FP (-5.4%), -11,689 TP, TP rate 44.7% → 44.2% (-0.5pp).
Top deltas: EXP12-C -5,477 FP/-7,530 TP (parent-check semantically correct but Juliet bad files have multiple call sites), INT01-C -2,714 FP/-740 TP, INT32-C -181 FP/-1,122 TP. Accepted: real-world precision prioritized over Juliet score.

**Deferred (require cross-function analysis)**:
- Pattern 4 — INT30-C guards (~8 FPs): Requires value-range analysis
- Pattern 9 — EXP34-C null guards (~5 FPs): Cross-function null invariants
- Pattern 17 — MEM31-C ownership (2 FPs): Cross-function ownership tracking

### Round 1: REFACTOR.md Cleanup (Completed)

| FP | Rule | Fix | Commit |
|----|------|-----|--------|
| FP-001 | DCL19-C | Recognize `STATIC` macro as static-equivalent (ported `has_static_macro_in_prefix()` from DCL15-C) | `fc862520` |
| FP-004 | INT32-C | Skip unsigned operands in all 5 binary overflow checks — unsigned wrap is INT30-C, not INT32-C | `0d545f83` |
| FP-002 | DCL15-C | Skip functions with prototypes in `.h` headers (public API). Also fixed prescan to traverse `linkage_specification`/`declaration_list` nodes (`extern "C" {}` blocks) and handle `pointer_declarator`-wrapped prototypes | `ff5508c0` |
| FP-005 | INT36-C | Exclude struct field access (`->`) and array subscript (`[`) from pointer-to-integer heuristic — these dereference and yield member/element types, not pointer types | `51f4b229` |
| FP-007 | PRE31-C | Skip string literal arguments from side-effect analysis — `=` inside format strings is not an assignment | `51f4b229` |
| FP-008 | EXP30-C | Recognize `x = f(x)` as safe — RHS fully evaluated before LHS write per C11 6.5.16 | `51f4b229` |
| FP-011 | INT30-C | Detect `if (var > 0)` / `if (0 < var)` guard before unsigned decrement — wrap provably impossible | `51f4b229` |
| FP-006 | EXP07-C | No longer fires on d_lib_common (resolved by prior EXP07-C improvements) | — |
| FP-010 | INT31-C | No longer fires on d_lib_common (resolved by prior INT31-C byte-extraction improvements) | — |
| FP-009 | DCL07-C/31-C | Skip indirect calls (function pointers via `->` / `.`) + skip calls inside `preproc_ifdef`/`preproc_if`/`preproc_elif` blocks | 0.2.12 |

### Prescan Infrastructure Improvements (from FP-002)

The FP-002 fix improved the prescan infrastructure beyond just DCL15-C:

- **`linkage_specification` traversal**: All three prescan walkers (`collect_function_names`, `collect_header_declarations`, `collect_call_graph`) now recurse into `extern "C" {}` blocks. This means functions defined/declared inside these blocks are properly discovered — improves DCL31-C/DCL07-C cross-file suppression for any project using C++ compatibility headers.
- **`pointer_declarator` handling**: `extract_function_name_from_declaration` now handles pointer-returning prototypes like `char *strdup(const char *)`. Previously these were silently missed by the prescan.
- **`header_declared_functions`**: New field in `ProjectContext` — available to any rule via `set_project_context()`. Currently used by DCL15-C; could be used by DCL19-C for the same purpose.

---

## Architecture Evolution

### Near-term

- [ ] Internal parallelization (rayon for file-level parallelism)
- [ ] Incremental parsing (only re-parse changed files)
- [ ] Baseline-aware suppression ("only new violations")
- [ ] Docker image for containerized CI/CD

### DCL13-C: Alias Tracking for Remaining FP

- [ ] Fix last DCL13-C FP: `ringbuffer.c:275 ptrBuffer` (case 17 from Round 12)

**Problem**: `ptrBuffer` is stored into `ptrRingBufferInfo->buffer` and then `memset` writes through the struct member. sqc doesn't track that `ptrBuffer` and `ptrRingBufferInfo->buffer` are aliased, so it reports `ptrBuffer` as unmodified. This is fundamentally beyond AST-level analysis — requires alias/points-to tracking.

**Possible shortcut**: If a pointer parameter is stored into a struct field (assignment `struct->field = param`), treat it as potentially modified — the struct may be written through later. This avoids full alias analysis while covering the common "store-then-write-through-alias" pattern.

### Struct Field Type Resolution (DONE — v0.3.5)

Implemented struct field type resolution for INT32-C and INT30-C. The prescan phase now collects struct definitions (named structs, typedef'd structs) into a `struct_field_types: HashMap<String, HashMap<String, String>>` in `ProjectContext`. When `infer_type()` encounters a `field_expression` (e.g., `s->count`), it resolves the field's type via:
1. Look up base variable type from `collect_variable_types()` (e.g., `s → "struct Data *"`)
2. Extract struct name (e.g., "Data")
3. Look up field type in struct database (e.g., "count" → "unsigned int")

**Files changed**: `prescan.rs` (collection), `context.rs` (storage), `ast_utils.rs` (shared helpers), `int32_c.rs` (integration), `int30_c.rs` (integration). Also added `struct_specifier` to `extract_type_and_name()` in both INT rules so struct pointer params appear in the type_map.

**Limitation**: Still can't follow `typedef struct Foo Bar` (only handles `typedef struct { ... } Name` and `struct Name { ... }`). INT10-C not yet integrated (unit struct, minimal FP count).

### Analysis Capabilities Lacking

- No preprocessor expansion (macros appear as function calls)
- No alias analysis (pointer aliasing not resolved — see DCL13-C remaining FP above)
- No symbolic execution
- No SSA form (beyond reaching definitions)
- No value range analysis (beyond const_eval macro folding + loop-bound extraction + shl negative-clamp + increment-before-subtract detection — see v0.2.21/v0.2.22/v0.3.3)
- No whole-program analysis (inter-procedural limited to function summaries + call-site null state propagation + local variable tracking + `-I` header resolution)
- Struct field type resolution available for INT32-C/INT30-C (v0.3.5) — limited to structs visible during prescan

---

## Real-World Validation

- [x] libcrc, sqlite, mosquitto, curl, hostap — three-way comparison complete
- [x] d_lib_common FP triage — 12 FPs documented in REFACTOR.md, all resolved (10 fixed in sqc, 2 resolved by prior improvements)
- [ ] Review remaining high-severity findings on d_lib_common
- [ ] Run same process on next module (d_lib_wifi, d_lib_ble)
- [ ] Generate per-module BRULE coverage cards for development workbook
- [ ] Compile presentation slides: critical issues found/fixed per module

---

## Competitor Research (TODO)

Research agenda for benchmarking against other tools:

### Tools to Evaluate

| Tool | Priority | Notes |
|------|----------|-------|
| Infer (Meta) | High | Flow-sensitive, separation logic |
| Frama-C (CEA) | High | Formal methods, Eva + WP plugins |
| Semgrep CE | Medium | Pattern-based baseline comparison |
| Flawfinder | Low | Lightweight CWE patterns |
| PVS-Studio | Low | Commercial, free for OSS |

### Key Metrics to Extract Per Tool

- TP Rate / FP Rate on Juliet or equivalent
- CWE coverage / CERT C rule coverage
- Analysis depth (AST / data-flow / inter-procedural / whole-program)
- Runtime performance
- Price / availability
- CI/CD integration (SARIF, GitHub, etc.)

### Academic Papers to Find

- ISSTA 2022 (TUM) — C analyzer comparison
- Goseva-Popstojanova & Perhinschi 2015 — Juliet evaluation
- JKU 2014 — Juliet scanner comparison
- NIST SATE IV/V/VI results

See `research/` directory for fetched content from prior research sessions.

---

## Definition of Done

**Tier 1 — Minimum Viable for CI/CD** (COMPLETE)
- [x] `--fail-on-violation` and `--fail-on-severity` flags
- [x] JSON, CSV, SARIF output
- [x] Incremental analysis (`--diff`)
- [x] Severity threshold filtering
- [x] GitHub Actions + Azure DevOps example workflows

**Tier 2 — Production Quality**
- [x] Real-world validation on 5+ open-source projects
- [ ] Baseline-aware suppression
- [ ] Docker image
- [ ] TP rate >= 45% on Juliet

**Tier 3 — Competitive**
- [ ] TP rate >= 50% on Juliet
- [ ] Direct benchmarked comparison with Infer, Frama-C
- [ ] Published comparison results

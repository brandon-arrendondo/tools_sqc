# SqC — Plans & Action Items

**Last Updated**: 2026-03-12 (v0.3.16)

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

- Relay chains (variants 52–54): **partially addressed** (v0.3.13) — multi-pass prescan propagates param states through single-hop relay. Deep chains (3+ hops) still produce Unknown.
- Indirect data flow (variants 63–67): not addressed
- Cross-file globals (variant 68): not addressed
- EXP33-C CFG integration (deferred — needs full CFG rewrite like EXP34-C)
- EXP34-C/FIO06-C regression investigation from Phase 3
- Target: 80%+ CWE-476 TP rate

---

## v0.3.14 Juliet Regression — Investigation & Next Steps

**Benchmark date**: 2026-03-11. Run: `sqc-0.3.14-fb7ef13c`, 118 CWEs, 54,484 files, 1h 14m.

### Regression Summary

| Metric | v0.3.5 (last full suite) | v0.3.14 | Delta |
|--------|--------------------------|---------|-------|
| TP | 130,004 | 126,106 | **−3,898** |
| FP | 161,510 | 158,036 | −3,474 |
| TP Rate | 44.6% | **44.4%** | **−0.2pp** |

TP loss (3,898) slightly exceeds FP reduction (3,474), so the rate went down. The FP reductions are
legitimate — the regression is the cost of continuing to tune rules in the 44–45% ceiling range where
each suppression removes both FPs and TPs proportionally.

### Root Cause: Distributed, No Single Bug

All suspected changes were investigated and ruled out as dominant causes:

| Change | Commit | Risk | Finding |
|--------|--------|------|---------|
| EXP33-C multi-branch (≥2 inits → ConditionallyInitialized) | `894d3875` | Medium | Juliet CWE457 bad() functions have 0 inits, not 2+. Not the cause. |
| INT30-C for-loop update skip | `59a3f9ea` | Medium | CWE190/191 bad() use `data+1`/`data-1`, not `data++`/`data--`. Not the cause. |
| INT30-C subtraction guard (`a>=b` → skip `a-b`) | `612e1ea2` | Medium | CWE191 bad() has no guard. Not the cause. |
| EXP33-C ancestor scope walk (removed innermost-only break) | `437a8282` | Low | Correctly suppresses FPs in nested-block patterns. Not over-broad. |
| EXP33-C preceding-assignment-in-block | `7a8ef10e` | Low | Only suppresses when direct assignment precedes read. Not over-broad. |

**Conclusion**: Regression is the cumulative effect of many individually correct suppressions, each
removing slightly more TPs than FPs when applied across all 118 CWEs. This is expected at the 44–45%
architectural ceiling (see "Key Insight" in "Top Remaining FP Rules" section).

### Key Diagnostic: flaw_lines_detected = 0 Everywhere

All major CWEs show `flaw_lines_detected: 0` despite having TPs. This means violations are found
in `_bad` functions (counted as TPs) but **not at the specific lines Juliet marks as flaws**. This
is a pre-existing condition, not a v0.3.14 regression.

| CWE | Files | TP | FP | TP% | flaw_lines_detected | flaw_lines_total |
|-----|------:|---:|---:|----:|--------------------:|-----------------:|
| CWE457 | 616 | 704 | 2,549 | 21.6% | 0 | 3,024 |
| CWE190 | 5,040 | 5,001 | 9,393 | 34.7% | 0 | 16,380 |
| CWE476 | 372 | 308 | 475 | 39.3% | 0 | 1,098 |
| CWE690 | 1,120 | 3,358 | 3,906 | 46.2% | 0 | 2,580 |

### Investigation Results (2026-03-11): Both (a) AND (b) Are True

Manual investigation on 3 CWE categories confirmed two root causes:

**Root Cause 1: Off-by-one (measurement bug)**

Juliet `/* POTENTIAL FLAW: ... */` comments are always on the line BEFORE the vulnerable code.
The analysis script (`analyze_juliet_results.py`) records the comment line number, but sqc reports
the code line. Result: exact matches are systematically missed.

Evidence:
- CWE476 `struct_01.c`: FLAW comment line 29, sqc EXP34-C reports line 30 (the dereference)
- CWE190 `short_max_multiply_01.c`: FLAW comment line 30, sqc INT08-C reports line 31
- Fix: check `line_num` AND `line_num - 1` against `flaw_lines` set

**Root Cause 2: Incidental noise scored as TP (fundamental methodology flaw)**

The section-based scoring (OMITBAD = TP, OMITGOOD = FP) counts ANY finding in the bad section as
a true positive, regardless of whether it's related to the CWE being tested. This inflates both
TP and FP counts with noise from unrelated rules.

Evidence — CWE121 `char_type_overrun_memcpy_05.c`:
- Actual vulnerability (line 50): `memcpy(charFirst, SRC_STR, sizeof(structCharVoid))` — wrong size
- sqc found: FLP03-C (line 51), INT36-C (line 52) in OMITBAD → scored as 2 "TP"
- sqc found: FLP03-C (lines 79, 98), INT36-C (lines 80, 99) in OMITGOOD → scored as 4 "FP"
- **None of these relate to buffer overflow.** The actual flaw was undetected.

Evidence — CWE476 `struct_01.c` (positive case):
- sqc found EXP34-C on line 30 (the actual null dereference) → genuine TP, correctly in OMITBAD
- This IS a CWE-relevant detection — EXP34-C maps directly to CWE-476

Evidence — CWE190 `short_max_multiply_01.c` (mixed case):
- sqc found INT08-C on line 31 (the actual overflow) → genuine TP, related to CWE-190
- sqc found INT08-C on lines 50, 67 (good variants with guards) → scored as FP
- FPs are from same rule, unable to prove the guard makes the code safe

**Impact on the 44.4% TP Rate**

The current metric measures: "of all findings sqc generates, what fraction lands in OMITBAD sections?"
Since OMITGOOD sections typically have ~2x the lines (2 good variants per bad), pure random noise
would produce ~33% "TP rate." The measured 44.4% is above the noise floor (indicating some real
detection), but the signal is diluted by incidental findings from unrelated rules.

**Conclusion**: The 44-45% ceiling is NOT an architectural limit on sqc's detection capability.
It's a measurement artifact from scoring methodology. **CWE-aware scoring is now implemented**
(see "CWE-Aware Scoring" section below) — validated on CWE-476 where CWE-matched TP rate is
46.1% vs 39.3% incidental, with 62% noise ratio confirming the dilution effect.

### CWE-Aware Scoring (IMPLEMENTED — v0.3.15)

All 5 proposed metrics are now computed automatically by the benchmark infrastructure.

**Implementation**: `scripts/generate_rule_cwe_map.py` produces `data/rule_cwe_map.json` (117 rules → 144 CWEs). `analyze_juliet_results.py` accepts `--cwe` and `--rule-cwe-map` to append CWE-aware metrics after existing output. `mcp/server.py` passes these args in `reanalyze_run()` and parses the new fields in `_parse_analysis()`. `run_juliet_parallel.sh` auto-regenerates the map at startup. All backward-compatible — old runs parse identically.

| Metric | What It Measures | How to Compute |
|--------|-----------------|----------------|
| **FLAW-line hit rate** | Did sqc find the vulnerability? | % of FLAW-comment lines with a CWE-matched finding within ±1 line |
| **CWE-matched TP rate** | Relevant findings in bad vs good sections | TP/(TP+FP) counting only CWE-mapped rules |
| **Per-file detection rate** | Binary: did we catch this test case? | % of OMITBAD files with ≥1 CWE-relevant finding |
| **Noise ratio** | How much output is unrelated to the CWE? | % of findings from non-CWE-mapped rules |
| **Incidental TP/FP** | Section-split noise (current metric) | Retained for backward compatibility, de-emphasized |

### Full-Suite CWE-Aware Results (v0.3.15, 2026-03-12)

**Run**: `sqc-0.3.15-9e241f4b`, 118 CWEs, 54,484 files, 1h 53m.

**Coverage**: 65 of 118 CWEs have rule-to-CWE mappings. 34 CWEs have at least one detection. 31 CWEs have mappings but zero CWE-relevant detections. 53 CWEs have no CERT-C rule mapped at all.

| Metric | Incidental (old) | CWE-Aware (new) |
|--------|-----------------|-----------------|
| TP Rate | 44.4% (126K/284K) | **45.6%** (5,195/11,404 CWE-matched) |
| Noise ratio | unmeasured | **95.0%** (217K/229K findings are unrelated rules) |
| Per-file detection | unmeasured | **10.0%** (4,426/44,243 files caught by relevant rule) |
| FLAW-line hit rate | 0% (exact match) | **3.6%** (4,258/117,447 with ±1 tolerance) |

**The 95% noise ratio is the headline number.** Only 5% of all Juliet findings come from rules
that actually map to the CWE being tested. The other 95% are incidental — DCL06-C, ERR05-C,
INT36-C, etc. firing in both OMITBAD and OMITGOOD sections. This explains why the incidental TP
rate was stuck at 44%: it was measuring noise distribution, not detection capability.

#### CWEs with Strong Detection (per-file ≥20%)

| CWE | Files | Per-File | CM TP Rate | Flaw-Hit | Key Rules |
|-----|------:|---------:|-----------:|---------:|-----------|
| CWE-481 (assign vs compare) | 18 | **66.7%** | 100% | 66.7% | EXP45-C |
| CWE-391 (unchecked error) | 54 | **37.0%** | 62.1% | 64.8% | ERR00-C, ERR33-C, ERR34-C |
| CWE-467 (sizeof pointer) | 54 | **37.0%** | 100% | 37.0% | ARR01-C, MEM35-C |
| CWE-758 (undefined behavior) | 365 | **32.6%** | 51.9% | 13.2% | EXP30-C, EXP33-C, INT34-C, MEM30-C |
| CWE-416 (use after free) | 150 | **30.7%** | 37.1% | 6.9% | MEM00-C, MEM01-C, MEM30-C |
| CWE-244 (heap inspection) | 72 | **30.6%** | 100% | 16.7% | MEM03-C |
| CWE-369 (divide by zero) | 1,008 | **30.1%** | 33.7% | 12.0% | FLP03-C, INT33-C |
| CWE-415 (double free) | 336 | **30.1%** | 44.1% | 15.7% | MEM00-C, MEM01-C, MEM30-C |
| CWE-476 (null deref) | 372 | **29.0%** | 46.5% | 3.7% | EXP34-C, API00-C |
| CWE-665 (improper init) | 224 | **29.0%** | 42.1% | 5.8% | ARR02-C, EXP33-C |
| CWE-338 (weak PRNG) | 18 | **27.8%** | 100% | 27.8% | MSC30-C |
| CWE-680 (int overflow → BOF) | 336 | **27.1%** | 43.7% | 0.0% | INT30-C, INT32-C |
| CWE-690 (null from return) | 1,120 | **25.9%** | 82.4% | 8.1% | EXP34-C |
| CWE-457 (uninitialized var) | 616 | **23.4%** | 34.7% | 2.5% | EXP33-C |
| CWE-401 (memory leak) | 1,228 | **21.7%** | 49.7% | 6.4% | MEM31-C |

#### High-Volume CWEs with Low Detection (biggest improvement opportunities)

| CWE | Files | Per-File | CM TP Rate | Flaw-Hit | Key Rules |
|-----|------:|---------:|-----------:|---------:|-----------|
| CWE-122 (heap BOF) | 3,656 | **4.4%** | 41.7% | 1.9% | ARR30-C, STR31-C |
| CWE-134 (format string) | 3,360 | **3.7%** | 33.4% | 1.1% | FIO30-C, FIO47-C |
| CWE-121 (stack BOF) | 5,906 | **12.8%** | 39.3% | 5.6% | ARR30-C, ARR38-C, STR31-C |
| CWE-190 (integer overflow) | 5,040 | **12.9%** | 44.2% | 4.0% | INT30-C, INT32-C |
| CWE-191 (integer underflow) | 3,864 | **14.6%** | 43.5% | 4.5% | INT30-C, INT32-C |
| CWE-590 (free not on heap) | 900 | **10.4%** | 100% | 4.5% | API07-C, MEM34-C |
| CWE-252 (unchecked return) | 630 | **11.9%** | 100% | 23.8% | ERR33-C, EXP34-C |
| CWE-197 (numeric truncation) | 1,008 | **18.0%** | 67.5% | 8.6% | INT31-C, FLP34-C |
| CWE-404 (resource shutdown) | 448 | **17.4%** | 65.3% | 11.9% | FIO42-C, MEM31-C |

#### CWEs with Mapping but Zero Detection (rules exist, never fire on Juliet patterns)

| CWE | Files | Incidental TP | Mapped Rules |
|-----|------:|--------------:|--------------|
| CWE-78 (OS cmd injection) | 5,600 | 17,350 | ENV03-C, ENV33-C, STR02-C |
| CWE-194 (sign extension) | 1,344 | 3,637 | INT31-C |
| CWE-195 (signed→unsigned) | 1,344 | 3,298 | INT31-C, FLP34-C |
| CWE-761 (free not at start) | 672 | 2,830 | API07-C |
| CWE-253 (incorrect check ret) | 684 | 409 | ERR33-C, POS34-C |
| CWE-114 (process control) | 672 | 1,751 | ERR07-C, MEM10-C |
| CWE-789 (uncontrolled alloc) | 560 | 2,163 | ARR30-C, MEM35-C |
| CWE-327 (broken crypto) | 54 | 376 | MSC30-C, MSC32-C |
| CWE-367 (TOCTOU) | 36 | 188 | FIO01-C, POS01-C |

CWE-78 is the most striking: 5,600 files with 17,350 incidental TPs but 0 CWE-matched detections.
The mapped rules (ENV03-C, ENV33-C, STR02-C) don't fire on Juliet's command injection patterns.
CWE-194/195 have INT31-C mapped but it doesn't trigger on sign extension test cases.

### Data-Driven Priorities (based on CWE-aware metrics)

The old priority framework optimized incidental TP rate (stuck at 44%). With CWE-aware scoring,
we can now prioritize by **per-file detection rate** (did we catch the bug?) on high-volume CWEs.

#### Priority 1 — CWE-122/121: Buffer Overflow Detection (9,562 files, 4.4%/12.8% per-file)

The two largest CWE categories by file count. STR31-C and ARR30-C are mapped but only catch
4.4% (heap) / 12.8% (stack) of test files. These are `memcpy`, `strcpy`, `memmove` with
undersized destination buffers. The rules fire on some patterns but miss most variants —
likely the cross-function and complex-flow variants (51–68) that require interprocedural analysis.

**Action**: Investigate which Juliet variants are detected vs missed by STR31-C/ARR30-C.
Focus on single-file variants first (01–18), then cross-function. Even getting stack BOF
from 12.8% → 30% would be a major improvement on the highest-volume CWE.

#### Priority 2 — CWE-457: Uninitialized Variable (616 files, 23.4% per-file)

EXP33-C detects 144 of 616 files. The 34.7% CWE-matched TP rate means it also has FPs in
OMITGOOD sections. Gap is from cross-function variants (51–68) and control flow patterns
(switch, goto) that EXP33-C's single-function analyzer doesn't model. Single-file variants
(01–18) should all be detectable — investigate why some are missed.

#### Priority 3 — CWE-190/191: Integer Overflow/Underflow (8,904 files, 12.9%/14.6% per-file)

INT30-C and INT32-C are the matched rules. These have high noise (the rules fire heavily
everywhere), but only 12.9%/14.6% of integer overflow test files get a CWE-relevant finding.
The matched TP rate (~44%) is reasonable — the gap is detection coverage, not precision.

#### Priority 4 — CWE-690: Null Deref from Return (1,120 files, 25.9% per-file, 82.4% CM TP rate)

Already the best-performing high-volume CWE by CWE-matched TP rate. EXP34-C is the sole
matched rule and achieves 82.4% precision when it fires. Getting per-file detection from
25.9% → 50%+ would make this a showcase CWE. The 74% of undetected files are likely
cross-function patterns where the null-returning call is in a different function.

#### Priority 5 — Zero-Detection CWEs (rules exist but never fire)

31 CWEs have CERT-C rules mapped but produce zero CWE-relevant detections. These represent
rules that exist in sqc but whose patterns don't match what Juliet tests for.

**Investigation Results (2026-03-12)**:

**CWE-124/126/127 (buffer underwrite/overread/underread)** — FIXED (quick win). These CWEs
had no rule mappings despite being children of CWE-125/CWE-119 which ARE mapped. Added
CWE-124, CWE-126, CWE-127 to ARR30-C, ARR38-C, STR31-C TOMLs. These rules already fire
on the Juliet test files — they were just classified as noise instead of CWE-matched.
Regenerated `rule_cwe_map.json` (144 → 147 unique CWEs).

**CWE-194/195 (sign extension, signed→unsigned)** — INT31-C mapped but only checks explicit
casts and direct assignments. Juliet uses implicit conversions in function arguments
(e.g., `strncpy(dest, src, signed_short)` where `short` is implicitly widened). Fix requires
adding function-argument type checking to INT31-C — medium difficulty.

**CWE-253 (incorrect check of function return value)** — **FIXED in P7.** ERR33-C now
validates comparison correctness when a function call is directly embedded in a
binary_expression. Functions classified by `ErrorReturnKind` enum (NullPointer, NegativeInt,
Eof, NonZero, Count). `check_incorrect_comparison()` walks up from call to binary_expression,
extracts operator/value, and validates against the function's error semantics.

**CWE-78 (OS command injection)** — ENV03-C/ENV33-C/STR02-C mapped. 5,600 files, 17,350
incidental TPs but 0 CWE-matched. **Primary root cause: macro indirection.** Juliet uses
`#define SYSTEM system` and calls `SYSTEM(data)`. Tree-sitter sees `SYSTEM` as the function
name, not `system`. All three rules check for literal function names (`"system" | "popen"`),
so they never match. Secondary gaps:
1. **Macro alias resolution**: Rules need to recognize `SYSTEM` → `system` via prescan
   macro constant collection. Easy-medium fix, would unlock all 5,600 Juliet files.
2. **Windows API coverage**: `_execl()`, `_execv()`, `_spawnl()` etc. not detected by
   ENV33-C or STR02-C. ~400-700 additional TPs after macro fix. Easy fix.
3. **ENV03-C file-level scope**: `clearenv()` anywhere in file suppresses all `system()`
   calls including vulnerable ones. Should be function-scoped. ~200-400 missed TPs.
4. **STR02-C limited taint tracking**: only checks string literal vs non-literal for
   `system()` args; exec family only checks `getenv()`. No tracking of `recv()`, `scanf()`,
   etc. as taint sources. ~1,500-2,000 FP reduction if improved.

#### Priority 6 — CWE-78 Macro Alias + Windows API Coverage (DONE — v0.3.16)

**Primary fix (DONE)**: Added `collect_macro_aliases()` to const_eval.rs — collects
`#define ALIAS identifier` patterns. Added `macro_aliases: HashMap<String, String>` to
ProjectContext, collected during prescan and header resolution. All three rules (ENV33-C,
ENV03-C, STR02-C) now implement `set_project_context()` and merge project-level + per-file
aliases. Macro names are resolved before matching against dangerous function lists.
Verified: `SYSTEM(data)` now triggers ENV33-C, ENV03-C, STR02-C.

**Secondary fix (DONE)**: Added Windows exec/spawn variants to ENV33-C: `_execl()`,
`_execv()`, `_execlp()`, `_execvp()`, `_execle()`, `_execve()`, `_spawnl()`, `_spawnle()`,
`_spawnlp()`, `_spawnv()`, `_spawnve()`, `_spawnvp()`. STR02-C also checks Windows
`_exec*()` variants for argument validation. Verified: `EXECVP(...)` → `_execvp`
correctly triggers ENV33-C through macro alias resolution.

#### Priority 7 — CWE-253 ERR33-C Comparison Validation (DONE — v0.3.16)

Added CWE-253 (incorrect check of function return value) detection to ERR33-C. Functions
are classified by error return kind: NullPointer (fgets, fopen, malloc — return NULL),
NegativeInt (fprintf, printf, snprintf — return < 0), Eof (putc, fputs, scanf — return EOF),
NonZero (remove, rename, fclose — return non-zero), Count (fread, fwrite — return count).
When a direct call appears in a binary_expression, the comparison operator/value is validated
against the function's error semantics. Incorrect patterns detected:
- Pointer functions with ordered comparison: `fgets() < 0` (pointer, not int)
- Negative-on-error compared `== 0`: `fprintf() == 0` (error is < 0)
- EOF-returning compared `== 0`: `putc() == 0` (error is EOF/-1)
- Non-zero-on-error compared `== 0`: `remove() == 0` (0 = success)
- Count-returning compared `< 0` or `== 0`: `fwrite() < 0` (size_t unsigned)

Also added macro alias resolution to ERR33-C (same pattern as ENV33-C) and extended
function coverage to include wchar_t variants (fgetws, fwprintf, putwc, etc.) for CWE-253
detection without adding them to the unchecked-return-value list (avoids ERR33-C noise).
Verified: all Juliet CWE-253 BAD patterns detected, zero false positives on GOOD patterns.

#### Priority 8 — CWE-194/195 INT31-C Implicit Conversion in Arguments (medium)

INT31-C only checks explicit casts and direct assignments. Juliet CWE-194/195 tests use
implicit conversions in function arguments (`strncpy(dest, src, signed_short)`). Extend
`check_assignment_conversion()` to inspect call_expression arguments against known function
parameter types. 2,688 files (1,344 each), currently 0% detection.

#### Future — Fast Benchmark Mode (CWE-focused manifests)

Currently the benchmark runs all 283 rules against every CWE directory, producing the 95%
noise. A "fast mode" would generate per-CWE manifest TOMLs from `rule_cwe_map.json`
(e.g. CWE-476.toml with only EXP34-C + API00-C enabled), eliminating noise at the source.
Expected speedup: significant — fewer rules per file means less AST traversal. Two modes:
- **Fast**: CWE-matched rules only. Primary metrics (per-file detection, flaw-hit, CWE-matched TP rate). CWEs without mappings are skipped.
- **Full**: All rules (current behavior). Retains incidental TP/FP for backward compat.

Implementation: `generate_rule_cwe_map.py` already has the `cwe_to_rules` mapping. Add a
`--fast` flag to `run_juliet_parallel.sh` that generates per-CWE manifests in a temp dir
and passes them instead of `rules-all.toml`.

#### De-prioritized: Incidental TP Rate

The old 44.4% incidental TP rate is no longer a target metric. Rule tuning that removes
equal TPs and FPs is neutral on CWE-matched metrics and just reduces noise. The 95% noise
ratio means most "improvements" to incidental rate were noise reshuffling.

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

**Key insight**: Most remaining top FP rules have ~50–65% FP ratios. Further rule tuning will proportionally lose TPs. The ~44–45% incidental TP rate ceiling is a measurement artifact from scoring all rules against all CWEs (see "CWE-Aware Scoring" section). CWE-matched TP rates are higher per-CWE. Per-file detection rate (29% on CWE-476) is the more actionable metric for prioritizing improvements. Higher-value gains will come from structural improvements (cross-function analysis, value-range analysis) rather than per-rule tuning.

---

## Real-World FP — Remaining Issues (after v0.2.18)

Verified against d_lib_common (7,694 violations) and d_hal_linux_random (45 violations).
Of the original 17 d_lib_common patterns + 6 d_hal_linux_random patterns, 12 are fixed. 8 remain.

| # | Rule | Violations | Difficulty | Description |
|---|------|--------:|------------|-------------|
| 1 | **INT30-C** | ~12 | Medium | Loop-bounded increments (`index += 1` where `index < bufferSize`), addition guarded by branch. v0.3.2: increment-then-subtract pattern + `1U` literal suffix now handled. v0.3.13: subtraction guarded by comparison (`if (a >= b) { a - b }`) + `1U` suffix in loop-bound and compound addition |
| 2 | **EXP33-C** | ~36 | Medium | For-loop init not recognized; array declarations without initializers. v0.3.8: field/subscript write no longer treated as read (−576 FP, −299 TP on 12-CWE Juliet). v0.3.13: for-loop init clause recognized as dominating assignment; ancestor scope walk for preceding assignments |
| 3 | **INT33-C** | ~7 | Hard | Division guarded by earlier comparison (`lower < upper` → divisor ≥ 2). Needs value-range |
| 4 | **INT34-C** | ~1 | Hard | Shift bounded by loop iteration count. Needs value-range |
| 5 | **EXP34-C** | ~28 | Medium | Helper functions called only after caller validates params with early-return null guard. v0.3.8: compound `\|\|` null guard now collects all vars. v0.3.13: multi-pass prescan propagates param null states through relay chains (`high→mid→low`) |
| 6 | **MEM30-C** | ~1 | Hard | Sequential struct/member frees (`free(s->items); free(s);`). Needs field-level tracking |
| 7 | **MEM31-C** | ~9 | Hard | Cross-function ownership (`strdup` into struct field, freed via custom `_Delete`). Needs ownership model |
| 8 | **API00-C** | ~18 | Easy | Validation present but after variable declarations; static helper functions called from validated callers. v0.3.8: 4 new validation patterns recognized (DONE) |

### Actionable Now

No medium-difficulty FP issues remain. Next targets are architecture improvements (baseline suppression, parallelization) or hard deferred issues.

### Recently Completed

**Issue 1 — INT30-C subtraction guard** (v0.3.13): Added `is_subtraction_guarded_by_comparison()` — detects `if (a >= b) { a - b }` patterns. Walks ancestors for if/while/for conditions comparing both operands. Supports `a >= b`, `a > b`, `b <= a`, `b < a`, and compound `&&` conditions. Also generalized `1U`/`1u` suffix handling in loop-bound and compound addition checks.

**Issue 5 — EXP34-C parameter null propagation** (v0.3.13): Added multi-pass prescan with `propagate_param_null_states()`. After initial aggregation, re-collects callsite args with function parameters seeded from aggregated states. Resolves relay chains: `high(p) { if(!p) return; mid(p); }` → `mid(p) { low(p); }` → `low(p) { *p = 42; }` — p now NotNull at low_level.

**Issue 2 — EXP33-C for-loop init** (v0.3.13): Fixed `has_preceding_assignment_in_block()` to walk ancestor scopes (not just innermost compound_statement). Added `for_init_assigns_var()` to recognize for-statement init clauses as dominating assignments for reads in the condition, update, and body. Handles `for (i = 0; ...)`, `for (int i = 0; ...)`, and comma expressions `for (i = 0, j = 0; ...)`.

**Issue 8 — API00-C validation past declarations** (v0.3.8): 4 new validation patterns recognized (DONE).

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
- No whole-program analysis (inter-procedural limited to function summaries + call-site null state propagation + multi-pass param relay propagation + local variable tracking + `-I` header resolution)
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

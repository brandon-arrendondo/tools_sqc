# SqC — Plans & Action Items

**Last Updated**: 2026-03-01 (v0.2.18)

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

### STR31-C: `check_strcpy_safety` — Add `is_function_parameter` Guard

**Status**: Identified but not yet implemented.

**Problem**: Round 13 added a suppression: when source is a string literal and dest buffer size is unknown, assume safe. This suppression also fires on TPs in cross-function tests (CWE124, CWE127) where a small stack buffer is passed to a helper that calls `strcpy(data, "fixedstring")`.

**Fix**: Gate the suppression on `!self.is_function_parameter(dest, source)`.

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

### Top Remaining FP Rules (after 0.2.18)

| Rule | FP | TP | FP% | Notes |
|------|---:|---:|----:|-------|
| DCL06-C | 15.6K | 18.9K | 45% | Code style — reductions lose TPs proportionally |
| INT30-C | 13.1K | 11.9K | 52% | Guard expansion in v0.2.18 |
| INT32-C | 10.8K | 6.6K | 62% | field_expression → not_applicable reduced both |
| EXP33-C | 6.8K | 5.4K | 56% | |
| INT36-C | 6.7K | 4.1K | 62% | |
| ERR33-C | 6.4K | 3.8K | 63% | Nested calls + math overlap fixed |
| ERR05-C | 6.3K | 3.3K | 65% | |
| EXP12-C | 5.4K | 3.4K | 62% | Parent-check added (was 11K/10.9K) |

**Key insight**: Most remaining top FP rules have ~50–65% FP ratios. Further rule tuning will proportionally lose TPs. The ~44–45% Juliet ceiling is likely an architectural constraint for single-TU analysis. Higher-value gains will come from structural improvements (cross-function analysis, value-range analysis) rather than per-rule tuning.

---

## Real-World FP — Remaining Issues (after v0.2.18)

Verified against d_lib_common (7,694 violations) and d_hal_linux_random (45 violations).
Of the original 17 d_lib_common patterns + 6 d_hal_linux_random patterns, 12 are fixed. 8 remain.

| # | Rule | Violations | Difficulty | Description |
|---|------|--------:|------------|-------------|
| 1 | **INT30-C** | ~12 | Medium | Loop-bounded increments (`index += 1` where `index < bufferSize`), addition guarded by branch |
| 2 | **EXP33-C** | ~36 | Medium | For-loop init not recognized; array declarations without initializers |
| 3 | **INT33-C** | ~7 | Hard | Division guarded by earlier comparison (`lower < upper` → divisor ≥ 2). Needs value-range |
| 4 | **INT34-C** | ~1 | Hard | Shift bounded by loop iteration count. Needs value-range |
| 5 | **EXP34-C** | ~28 | Medium | Helper functions called only after caller validates params with early-return null guard |
| 6 | **MEM30-C** | ~1 | Hard | Sequential struct/member frees (`free(s->items); free(s);`). Needs field-level tracking |
| 7 | **MEM31-C** | ~9 | Hard | Cross-function ownership (`strdup` into struct field, freed via custom `_Delete`). Needs ownership model |
| 8 | **API00-C** | ~18 | Easy | Validation present but after variable declarations; static helper functions called from validated callers |

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

### Struct Field Type Resolution (TODO)

Several rules (INT32-C, INT10-C, INT30-C) need to know whether `self->field` is signed or unsigned. Currently, `field_expression` nodes return `"not_applicable"` or `"unknown"` because tree-sitter doesn't resolve struct member types.

**Approach**: Build a struct-field-type database during prescan by parsing `struct` definitions. Map `struct_name.field_name → type_text`. When encountering `self->field`, look up the variable's struct type from `collect_variable_types()`, then resolve the field's type from the database.

**Impact**: Would recover TPs lost by the INT32-C `field_expression → not_applicable` change (e.g., signed int fields in struct arithmetic). Would also improve INT10-C and INT30-C precision for struct-heavy code.

**Complexity**: Medium — requires struct definition parsing + two-level lookup (variable → struct type → field type). Limited by typedef resolution (can't follow `typedef struct Foo Bar`).

### Analysis Capabilities Lacking

- No preprocessor expansion (macros appear as function calls)
- No alias analysis (pointer aliasing not resolved — see DCL13-C remaining FP above)
- No symbolic execution
- No SSA form (beyond reaching definitions)
- No value range analysis (beyond literal constants)
- No whole-program analysis (inter-procedural limited to function summaries + call-site null state propagation + local variable tracking)
- No struct field type resolution (field_expression types unknown — see TODO above)

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

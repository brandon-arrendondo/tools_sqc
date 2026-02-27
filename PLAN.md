# SqC — Plans & Action Items

**Last Updated**: 2026-02-27

---

## EXP34-C / CWE-476 Improvement Roadmap

Goal: Raise CWE-476 (NULL Pointer Dereference) TP rate from 37.6% toward 80%+.

### Phase 1 — CFG-based Null State Dataflow (COMPLETE)

- [x] `src/analyze/null_state.rs` — forward dataflow with NullState lattice, edge refinement, assert recognition
- [x] EXP34-C rewritten to use CFG-based analysis (replaced ~1200-line linear walk)
- [x] MEM10-C parameter-only null check fix (−106 FP on CWE-476)
- [x] DCL13-C main() exemption (no benchmark impact)

### Phase 2 — Call-Site Null Propagation (NEXT)

Cross-file variants (51–68) have null assignment in file A and dereference in file B.

1. **Extend `FunctionSummary`**: Add `possibly_null_params: Vec<usize>` field
2. **Two-pass analysis**: First pass collects call-site null info; second pass uses it in callee analysis
3. **Call-site flagging**: When a call passes a definitely/possibly-null value as a pointer argument, flag EXP34-C at the call site
4. **EXP33-C CFG integration**: Apply branch-merge fix to EXP33-C initialization tracking

**Estimated gain**: Tier D = ~120 files at 70% detection → ~252 additional TPs.

### Phase 3 — Tier E Coverage + API Rule Narrowing

- Whole-program global null tracking (multi-TU coordination)
- API00-C / API02-C scope narrowing

### Phase 4 — Remaining Edge Cases

- Global pre-pass for static globals (Variant 45)
- Remaining FP reduction + edge cases
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

### INT34-C: Literal Shift Amount >= Type Width

Current fix skips all non-negative integer literals to eliminate FPs from `x >> 8` etc. This means we miss the case where the literal is >= the promoted type width (e.g. `uint8_t x; x << 32;`). Compilers warn with `-Wshift-count-overflow`. Low priority — requires knowing promoted operand type.

### Top Remaining FP Rules (candidates for next round)

| Rule | FP | TP | FP% | Notes |
|------|---:|---:|----:|-------|
| INT32-C | ~19K | ~15K | 57% | Type-aware inference already applied |
| DCL06-C | ~15K | ~19K | 44% | Code style — reductions lose TPs proportionally |
| INT30-C | ~14K | ~13K | 52% | Pointer arithmetic guards applied |
| EXP12-C | ~11K | ~11K | 50% | Whitelist already trimmed |
| INT36-C | ~7K | ~4K | 63% | |
| ERR33-C | ~6K | ~4K | 63% | Nested calls + math overlap fixed |
| ERR05-C | ~6K | ~3K | 65% | |
| EXP33-C | ~6K | ~5K | 54% | |

**Key insight**: Most remaining top FP rules have ~50–65% FP ratios. Further rule tuning will proportionally lose TPs. The ~44% Juliet ceiling is likely an architectural constraint for single-TU analysis.

---

## Real-World FP Fixes (d_lib_common)

Targeted FP reduction driven by findings from `~/data/d_lib_common/REFACTOR.md`.

### Completed

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

### Analysis Capabilities Lacking

- No preprocessor expansion (macros appear as function calls)
- No alias analysis (pointer aliasing not resolved — see DCL13-C remaining FP above)
- No symbolic execution
- No SSA form (beyond reaching definitions)
- No value range analysis (beyond literal constants)
- No whole-program analysis (limited to function summary pre-scanning)

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

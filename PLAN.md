# SqC — Plans & Roadmap

**Last Updated**: 2026-03-19 (v0.3.24)

For completed work, see [CHANGELOG.md](CHANGELOG.md).
For benchmark data, see [JULIET_RESULTS.md](JULIET_RESULTS.md) and [REALWORLD_RESULTS.md](REALWORLD_RESULTS.md).
For competitor research, see [RESEARCH.md](RESEARCH.md).

---

## Immediate Next Steps

### CWE-121/122 Remaining FP Reduction

~29 FPs remain from v0.3.21 buffer overflow detection (mostly addressed in v0.3.22):
- ~~ARR38-C CWE805 (69 FPs)~~: FIXED — function-scoped alias resolution prevents cross-function contamination.
- ~~ARR30-C CWE129 goodG2B (~67 FPs)~~: FIXED — multi-assignment constant resolution handles `data = -1; data = 7;` patterns.
- ARR30-C CWE135 (29 FPs): ALLOCA tracking enables `strcpy` flagging on correctly-sized buffers. Lower priority.

### VRA Phase 5: Inter-Procedural Value Ranges — COMPLETE (v0.3.24)

Phases 1–4 delivered in v0.3.23 (core module, caching, INT33/34-C, INT32/30-C migration). Phase 5 adds return-range summaries for inter-procedural precision.

- `FunctionSummary.return_range: Option<ValueRange>` computed during prescan from constant return expressions
- VRA transfer function resolves `call_expression` RHS using callee return ranges
- Benefits all 4 VRA-consuming rules (INT30/32/33/34-C)

### v0.3.24 Performance Regression — IMMEDIATE

~2x benchmark slowdown (88m vs 45m). Root cause: per-file `collect_macro_constants` + `compute_summaries` in `mod.rs` analysis loop. Heavy CWEs (5040+ files) see 4–6x slowdown. CWE-121 (5906 files) timed out.

**Fix:** Move same-file summary computation behind the `needs_vra` check, and cache `file_macros` so they're not recomputed (already computed for VRA in `compute_vra_if_needed`). Avoid cloning `context.function_summaries` per file — pass a reference or compute merged summaries once.

**Files to modify:**
- `src/analyze/mod.rs` — restructure per-file analysis loop to avoid redundant macro/summary computation

### VRA Phase 6: INT31-C Migration

INT31-C (integer conversion/truncation) uses syntactic `is_inside_bounds_checked_block()` — walks parent if-statements looking for type-limit macros. VRA would replace this with proper range narrowing: if VRA proves the value is within the target type's range at the cast site, suppress the violation regardless of how the constraint was established.

**Files to modify:**
- `src/rules/cert_c/INT/INT31-C/int31_c.rs` — add VRA integration (same pattern as INT30/32-C: `vra_results` field, `set_vra_results`, `vra_var_ranges_at` helper); replace `is_inside_bounds_checked_block` calls with VRA range checks

### Benchmark Infrastructure: Remaining Phases

- Phase 2: `query_violations()` flexible drill-down, `get_performance_trend()`, `estimate_eta()`
- Phase 3: Real-world runner integration (`bench/realworld_runner.py`)
- Phase 4: Remove legacy shell scripts after full migration validation

### Real-World Validation: Next Modules

- [ ] Run sqc on d_lib_wifi, d_lib_ble
- [ ] Review remaining high-severity findings on d_lib_common
- [ ] Generate per-module BRULE coverage cards

---

## Medium Term

### CWE-457: Uninitialized Variable (Priority 1)

616 files, 23.4% per-file. EXP33-C detects 144/616 files. Gap: cross-function variants (51–68) and control flow (switch, goto). Single-file variants (01–18) should all be detectable.

### CWE-190/191: Integer Overflow/Underflow (Priority 3)

8,904 files, 12.9%/14.6% per-file. INT30-C/INT32-C matched. Reasonable ~44% CWE-matched TP rate — gap is detection coverage, not precision.

### CWE-690: Null Deref from Return (Priority 4)

1,120 files, 25.9% per-file, 82.4% CWE-matched TP rate. Best-performing high-volume CWE. Getting to 50%+ per-file would make this a showcase. 74% undetected are likely cross-function patterns.

### EXP34-C Phase 4 — Remaining Edge Cases

- Relay chains (3+ hops): multi-pass handles single-hop, deep chains still Unknown
- Indirect data flow (variants 63–67): not addressed
- Cross-file globals (variant 68): not addressed
- EXP33-C CFG integration (needs full rewrite like EXP34-C)
- EXP34-C/FIO06-C regression investigation from Phase 3

### Real-World FP — Deferred Hard Issues

Remaining from d_lib_common/d_hal_linux_random triage (require new analysis capabilities):

| Rule | Violations | Issue |
|------|--------:|-------|
| ~~INT33-C~~ | ~~7~~ | ~~Division guarded by earlier comparison.~~ **ADDRESSED in v0.3.23** — CFG-based VRA handles guard patterns. Needs benchmark verification. |
| ~~INT34-C~~ | ~~1~~ | ~~Shift bounded by loop iteration count.~~ **ADDRESSED in v0.3.23** — CFG-based VRA handles loop bounds. Needs benchmark verification. |
| MEM30-C | ~1 | Sequential struct/member frees. Needs field-level tracking. |
| MEM31-C | ~9 | Cross-function ownership (strdup → struct field → custom \_Delete). Needs ownership model. |

### Zero-Detection CWEs (rules exist but never fire)

~25 CWEs still have CERT-C rules mapped but produce zero CWE-relevant detections. Remaining high-value targets after P6/P7/P8 fixes:

| CWE | Files | Mapped Rules | Issue |
|-----|------:|--------------|-------|
| CWE-761 (free not at start) | 672 | API07-C | Pattern mismatch |
| CWE-114 (process control) | 672 | ERR07-C, MEM10-C | Pattern mismatch |
| CWE-789 (uncontrolled alloc) | 560 | ARR30-C, MEM35-C | Pattern mismatch |
| CWE-327 (broken crypto) | 54 | MSC30-C, MSC32-C | Pattern mismatch |
| CWE-367 (TOCTOU) | 36 | FIO01-C, POS01-C | Pattern mismatch |

---

## Long Term

### Architecture Evolution

- [ ] **Internal parallelization** — rayon for file-level parallelism
- [ ] **Incremental parsing** — only re-parse changed files
- [ ] **Baseline-aware suppression** — "only new violations" mode
- [ ] **Docker image** — containerized CI/CD distribution

### Analysis Capabilities Lacking

- No preprocessor expansion (macros appear as function calls; macro aliases partially addressed via `collect_macro_aliases`)
- No alias analysis (pointer aliasing not resolved; file-scoped alias collection causes cross-function issues)
- No symbolic execution
- No SSA form (beyond reaching definitions)
- ~~No full value-range analysis~~ **v0.3.23**: CFG-based forward VRA with interval lattice, edge refinement, widening. Intra-procedural only; inter-procedural return ranges planned (Phase 5).
- Limited whole-program analysis (function summaries + call-site null state + multi-pass relay propagation + local variable tracking + `-I` header resolution)
- Struct field type resolution limited to structs visible during prescan (INT32-C/INT30-C only)

### DCL13-C: Alias Tracking for Last FP

`ringbuffer.c:275 ptrBuffer` — pointer stored into struct field, then `memset` writes through the struct member. Requires alias/points-to tracking. Possible shortcut: if a pointer param is stored into a struct field, treat it as potentially modified.

### Definition of Done

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
- [ ] CWE-matched TP rate >= 50% on key CWEs

**Tier 3 — Competitive**
- [ ] Direct benchmarked comparison with Infer, Frama-C (see [RESEARCH.md](RESEARCH.md))
- [ ] Published comparison results
- [ ] Per-file detection >= 30% on top 10 CWEs by volume

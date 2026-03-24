# SqC — Plans & Roadmap

**Last Updated**: 2026-03-23 (v0.3.37 benchmarked)

For completed work, see [CHANGELOG.md](CHANGELOG.md).
For benchmark data, see [JULIET_RESULTS.md](JULIET_RESULTS.md) and [REALWORLD_RESULTS.md](REALWORLD_RESULTS.md).
For competitor research, see [RESEARCH.md](RESEARCH.md).

---

## Immediate Next Steps

### ARR36-C Real-World FP Reduction (Priority 1)

v0.3.36 added strchr-based pointer subtraction detection (CWE-469, 100% TP on Juliet). On real-world code it fires +2,727 times (hostap +3,147, sqlite +1,211). Likely FPs from legitimate pointer arithmetic patterns. Investigate and tighten.

### Real-World FP Reduction — Top Rules (Priority 1)

v0.3.37 per-rule data (all 5 codebases, 153.6K total violations, rules-benchmark.toml).

| Rule | Count | Issue | Status |
|------|------:|-------|--------|
| MEM30-C | 15,330 | Use-after-free | Needs field-level free tracking (deferred) |
| DCL13-C | 12,138 | Const correctness | Needs alias tracking (deferred) |
| INT32-C | 12,018 | Signed overflow | Stable after VRA |
| API00-C | 9,227 | Missing size parameter | Stable |
| INT30-C | 8,488 | Unsigned overflow | Stable after VRA |
| EXP33-C | 6,611 | Uninitialized | v0.3.37: -477 (-6.7%) |
| MEM31-C | 5,440 | Memory leak | Needs ownership model (deferred) |
| EXP34-C | 5,290 | Null deref | Stable |
| DCL31-C | 4,842 | Undeclared function | Stable (prescan) |
| DCL07-C | 4,765 | Implicit int declaration | Stable (prescan) |
| **ARR36-C** | **4,685** | **Pointer subtraction** | **+2,727 regression — PRIORITY** |
| ARR00-C | 2,157 | Array bounds | v0.3.37: -480 (-18.2%) |
| ERR33-C | 989 | Unchecked return | v0.3.37: -818 (-45.3%) |

### Juliet TP Rate — Path to 50%

v0.3.37: **48.4% TP rate**. Remaining gap dominated by high-FP rules where Juliet good/bad patterns are structurally identical to our analysis: INT32-C (55% FP), ENV33-C (58% FP), STR31-C (59% FP), INT33-C (65% FP), FLP03-C (69% FP).

### EXP33-C — Remaining

- **Cross-function variants 63/64**: pointer passed between source files, needs inter-procedural analysis

---

## Medium Term

### CWE-457: Uninitialized Variable — Remaining Gaps (Priority 1)

v0.3.37: 165 TP, 302 FP, **35.3% TP rate** (up from 32.2% in v0.3.34). The arr[0].field fix and initializer suffix matching removed 72 FP while losing only 13 TP — clean improvement.

Remaining gaps:
- Cross-function variants 63/64 (~70 files): pointer passed between source files, needs inter-procedural analysis
- Per-element tracking for stack arrays: `team[0].x = 1; use(team[3].x)` correctly flags, but no way to track that ALL elements are initialized
- 302 FP still high — likely dominated by cross-function initialization patterns Juliet's "good" functions use

### CWE-190/191: Integer Overflow/Underflow (Priority 3)

v0.3.37: CWE-190 655 TP/790 FP (45.3%), CWE-191 560 TP/716 FP (43.9%). Unchanged since v0.3.28. INT30-C/INT32-C matched. Stable after VRA — gap is coverage not precision.

### CWE-690: Null Deref from Return (Priority 4)

v0.3.37: 203 TP, 12 FP, **94.4% TP rate**, 18.1% per-file. Best precision of any high-volume CWE. Per-file rate (18.1%) still below 30% target — 74% undetected are likely cross-function patterns.

### EXP34-C Phase 4 — Remaining Edge Cases

- Relay chains (3+ hops): multi-pass handles single-hop, deep chains still Unknown
- Indirect data flow (variants 63–67): not addressed
- Cross-file globals (variant 68): not addressed
- EXP34-C/FIO06-C regression investigation from Phase 3

### Real-World FP — Deferred Hard Issues

Require new analysis capabilities beyond current architecture:

| Rule | v0.3.37 Count | Issue | Required Capability |
|------|--------:|-------|---------------------|
| MEM30-C | 15,330 | Sequential struct/member frees, cross-function free propagation | Field-level free tracking |
| MEM31-C | 5,440 | Cross-function ownership (strdup → struct field → custom_Delete) | Ownership model |
| DCL13-C | 12,138 | Const correctness — pointer params through struct fields | Alias/points-to tracking |

### Zero-Detection CWEs — Remaining

| CWE | Portable Files | Mapped Rules | Issue | Effort |
|-----|------:|--------------|-------|--------|
| CWE-468 (incorrect pointer scaling) | 36 | ARR39-C, EXP08-C | Implicit void* casts losing type info | High |

4 formerly zero-detection CWEs resolved in v0.3.35–v0.3.36 (see CHANGELOG). 13 are Windows-only (not actionable).

---

## Long Term

### Test Coverage (Priority 2)

Current test infrastructure auto-generates integration tests from `.c` files in `tests/` directories. Coverage gaps:
- Many rules have only wiki-sourced tests (1-3 cases) — need broader pattern coverage
- No tests for inter-procedural analysis paths (prescan, call-site propagation, `-d` directories)
- No regression tests for specific FP patterns fixed in each round (real-world patterns from arraylist.c, intset.c, file_util.c etc.)
- EXP34-C param-null tests had to be restructured when default changed — need tests that exercise with/without project context
- No negative tests for false-positive patterns (verify FP stays suppressed)

### Architecture Evolution

- [x] **Prescan cache** (v0.3.28) — `--save-prescan`/`--load-prescan`, persistent in `data/prescan_cache/`, `--rebuild-prescan` for stale cache
- [x] **External parallelization** (v0.3.27–v0.3.28) — `scripts/sqc_parallel_scan.py` with subdirectory splitting + prescan cache
- [ ] **Internal parallelization** — rayon for file-level parallelism within a single sqc invocation
- [ ] **File-size-aware batching** — current subdir splitting can leave one large unit dominating wall time (e.g., wpa_supplicant/ 69 files = 1061s). Batch by file size rather than directory to balance work across workers.
- [ ] **Incremental parsing** — only re-parse changed files
- [ ] **Baseline-aware suppression** — "only new violations" mode
- [ ] **Docker image** — containerized CI/CD distribution

### Analysis Capabilities Lacking

- No preprocessor expansion (macros appear as function calls; macro aliases partially addressed via `collect_macro_aliases`)
- No alias analysis (pointer aliasing not resolved; file-scoped alias collection causes cross-function issues)
- No symbolic execution
- No SSA form (beyond reaching definitions)
- Value-range analysis is intra-procedural with inter-procedural return ranges (v0.3.23–v0.3.24). No inter-procedural argument ranges or field-sensitive VRA.
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
- [ ] CWE-matched TP rate >= 50% on key CWEs (currently 48.4% overall; 16 CWEs already at 100%, 6 above 50%)

**Tier 3 — Competitive**
- [ ] Direct benchmarked comparison with Infer, Frama-C (see [RESEARCH.md](RESEARCH.md))
- [ ] Published comparison results
- [ ] Per-file detection >= 30% on top 10 CWEs by volume

# SqC — Plans & Roadmap

**Last Updated**: 2026-03-20 (post-v0.3.25)

For completed work, see [CHANGELOG.md](CHANGELOG.md).
For benchmark data, see [JULIET_RESULTS.md](JULIET_RESULTS.md) and [REALWORLD_RESULTS.md](REALWORLD_RESULTS.md).
For competitor research, see [RESEARCH.md](RESEARCH.md).

---

## Immediate Next Steps

### CWE-121/122 Remaining FP Reduction

- ARR30-C CWE135 (29 FPs): ALLOCA tracking enables `strcpy` flagging on correctly-sized buffers. Lower priority.

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

### CWE-457: Uninitialized Variable — Remaining Gaps (Priority 1)

616 files, ~35% per-file (estimated after v0.3.25 fixes). Remaining gaps:
- Cross-function variants 63/64 (~70 files): pointer passed between source files, needs inter-procedural analysis
- Array partial_init through alloca/malloc (~66 files): partial subscript init upgrades to MallocInitialized, losing content-level tracking
- struct variant 12: struct field access pattern edge case

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
- [ ] CWE-matched TP rate >= 50% on key CWEs

**Tier 3 — Competitive**
- [ ] Direct benchmarked comparison with Infer, Frama-C (see [RESEARCH.md](RESEARCH.md))
- [ ] Published comparison results
- [ ] Per-file detection >= 30% on top 10 CWEs by volume

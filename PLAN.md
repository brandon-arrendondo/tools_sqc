# SqC — Plans & Roadmap

**Last Updated**: 2026-03-16 (v0.3.19)

For completed work, see [CHANGELOG.md](CHANGELOG.md).
For benchmark data, see [JULIET_RESULTS.md](JULIET_RESULTS.md) and [REALWORLD_RESULTS.md](REALWORLD_RESULTS.md).
For competitor research, see [RESEARCH.md](RESEARCH.md).

---

## Immediate Next Steps

### Benchmark v0.3.18

Run full Juliet suite to measure CWE-194/195 improvement from INT31-C `check_call_argument_conversion()`. Previously 0% CWE-matched detection on 2,688 files.

### ~~CWE-78: ENV03-C + STR02-C~~ ✓ DONE (v0.3.19)

1. **ENV03-C function-scoped clearenv()**: Now checks sanitization per-function instead of file-level.
2. **STR02-C taint tracking**: Intra-function taint analysis (recv, fgets, fgetws, scanf, getenv, etc.) with cast handling and propagation. Precision 42.0% → 45.5%, FP -330, TP -78 (cross-function patterns remain undetected).

### ~~Fast Benchmark Mode (CWE-focused manifests)~~ ✓ DONE (v0.3.18)

`generate_rule_cwe_map.py` now generates 147 per-CWE manifest TOMLs in `rules_templates/cwe/`. `run_juliet_parallel.sh --fast` uses them. Validated on CWE-476: noise drops from 61.8% → 0%, TP rate 39.5% → 46.5%, per-file detection unchanged (29.0%).

### Benchmark Infrastructure Overhaul

Refactor Juliet and real-world benchmark tooling into a unified Python runner with SQLite output, optimized for Claude-driven analysis.

1. **Juliet MCP server**: Update to use `--fast` mode (per-CWE manifests) by default
2. **Unified Python runner**: Replace shell scripts (`run_juliet_parallel.sh`, `run_juliet_multi_cwe.sh`) with a single Python entry point that handles both Juliet and real-world benchmarks
3. **SQLite output**: Store all benchmark results (violations, TP/FP classification, CWE-aware metrics, per-CWE timing, total run duration) in a SQLite database instead of flat CSV + text files. Include machine metadata (CPU model, core count, RAM) per run so the MCP server can estimate time remaining from historical runs on the same hardware and detect performance regressions across versions.
4. **Claude-optimized analysis**: Design schema and query interface so Claude can efficiently drill into results (per-CWE, per-rule, per-variant, cross-run diffs, performance trends) without parsing large text files
5. **Run orchestration**: Parallel CWE scanning, progress tracking, resume-on-failure, automatic comparison with prior runs

### Real-World Validation: Next Modules

- [ ] Run sqc on d_lib_wifi, d_lib_ble
- [ ] Review remaining high-severity findings on d_lib_common
- [ ] Generate per-module BRULE coverage cards

---

## Medium Term

### CWE-122/121: Buffer Overflow Detection (Priority 1)

Largest CWE categories: 9,562 files, 4.4%/12.8% per-file detection. STR31-C and ARR30-C are mapped but miss most variants — likely cross-function and complex-flow (51–68).

**Action**: Investigate which Juliet variants are detected vs missed. Focus single-file variants (01–18) first. Stack BOF 12.8% → 30% would be a major win.

### CWE-457: Uninitialized Variable (Priority 2)

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
| INT33-C | ~7 | Division guarded by earlier comparison. Needs value-range. |
| INT34-C | ~1 | Shift bounded by loop iteration count. Needs value-range. |
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
- No alias analysis (pointer aliasing not resolved)
- No symbolic execution
- No SSA form (beyond reaching definitions)
- No full value-range analysis (beyond const_eval macro folding + loop-bound extraction)
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

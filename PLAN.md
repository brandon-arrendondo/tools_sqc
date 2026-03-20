# SqC — Plans & Roadmap

**Last Updated**: 2026-03-20 (v0.3.26 Juliet complete, realworld pending)

For completed work, see [CHANGELOG.md](CHANGELOG.md).
For benchmark data, see [JULIET_RESULTS.md](JULIET_RESULTS.md) and [REALWORLD_RESULTS.md](REALWORLD_RESULTS.md).
For competitor research, see [RESEARCH.md](RESEARCH.md).

---

## Immediate Next Steps

### CWE-121/122 Remaining FP Reduction

- ARR30-C CWE135 (29 FPs): ALLOCA tracking enables `strcpy` flagging on correctly-sized buffers. Lower priority.

### Benchmark Infrastructure: Remaining Phases

- Phase 2: `query_violations()` flexible drill-down, `get_performance_trend()`, `estimate_eta()`
- Phase 3: Real-world SQLite integration (see details below)
- Phase 4: Remove legacy shell scripts after full migration validation

### Real-World Benchmark → SQLite Migration

Currently realworld results live only as JSON files in `/tmp/realworld_results/` and historical summary counts in `REALWORLD_RESULTS.md`. The `realworld_runs` and `realworld_results` tables in `data/benchmarks.db` were backfilled from markdown but have no auto-ingestion. Plan:

**Step 1: Ingest v0.3.25 results** (immediate)
- Script to parse completed JSON files from `.63:/tmp/realworld_results/sqc-0.3.25-85555478/`
- Insert into `realworld_runs` (version, commit, hostname, cpu) + `realworld_results` (project, tool, violation_count)
- Add new `realworld_violations` table for per-violation detail (rule_id, file, line, message, severity) — enables per-rule trending

**Step 2: Auto-ingestion in MCP server** (next)
- `realworld_server.py` writes to SQLite on run completion (when `.done` file appears)
- Parse JSON result file → insert run + results + violations into DB
- Remote runs: fetch JSON via SCP on completion, then ingest locally

**Step 3: Query tools**
- `compare_realworld_runs(base, target)` — per-rule delta across codebases
- `get_realworld_rule_trend(rule_id)` — violation count over versions
- Deprecate REALWORLD_RESULTS.md as source of truth (generate from DB if needed)

### Real-World Validation

v0.3.25 results (all 5 codebases complete, run on brandon-ThinkCentre-M715q 8-core Ryzen):

| Project | v0.3.5 | v0.3.25 | Delta | Top Reducers |
|---------|-------:|--------:|------:|--------------|
| hostap | 179,833 | 160,121 | −19,712 (−11.0%) | DCL08-C −11,718, DCL31/07-C −3,586, EXP33-C −2,643 |
| sqlite | 129,035 | 116,642 | −12,393 (−9.6%) | DCL31/07-C −9,260, EXP33-C −2,376, INT36-C −733 |
| curl | 63,207 | 55,975 | −7,232 (−11.4%) | DCL31/07-C −5,640, EXP33-C −940 |
| mosquitto | 29,824 | 26,470 | −3,354 (−11.2%) | DCL08-C −1,647, DCL31/07-C −926, EXP33-C −291 |
| libcrc | 734 | 705 | −29 (−3.9%) | |
| **Total** | **402,633** | **359,913** | **−42,720 (−10.6%)** | |

Regressions to investigate:
- INT31-C +501 on sqlite (VRA Phase 6 broadened detection — likely new TPs but verify)
- ~~ERR33-C +149/+234 on hostap/sqlite~~ — partially addressed in v0.3.26 (CWE-253 `== 0` fix)

Next:
- [ ] Compare v0.3.26 realworld results against v0.3.25 (benchmark running)
- [ ] Ingest all results into SQLite (see migration plan above)
- [ ] Run sqc on d_lib_wifi, d_lib_ble
- [ ] Review remaining high-severity findings on d_lib_common

---

## Medium Term

### CWE-457: Uninitialized Variable — Remaining Gaps (Priority 1)

616 files, 28.7% per-file, 31.8% TP rate (v0.3.25 benchmark). v0.3.26 addresses the +109 FP regression via early-return branch detection. Remaining gaps:
- Cross-function variants 63/64 (~70 files): pointer passed between source files, needs inter-procedural analysis
- Array partial_init through alloca/malloc (~66 files): partial subscript init upgrades to MallocInitialized, losing content-level tracking
- struct variant 12: struct field access pattern edge case

### CWE-190/191: Integer Overflow/Underflow (Priority 3)

8,904 files, 13.0%/14.5% per-file, 45.3%/43.8% TP rate (v0.3.25). INT30-C/INT32-C matched. Stable after VRA integration — gap is detection coverage, not precision.

### CWE-690: Null Deref from Return (Priority 4)

1,120 files, 25.2% per-file, 82.0% TP rate (v0.3.25). Best-performing high-volume CWE. Getting to 50%+ per-file would make this a showcase. 74% undetected are likely cross-function patterns.

### EXP34-C Phase 4 — Remaining Edge Cases

- Relay chains (3+ hops): multi-pass handles single-hop, deep chains still Unknown
- Indirect data flow (variants 63–67): not addressed
- Cross-file globals (variant 68): not addressed
- EXP33-C CFG integration (needs full rewrite like EXP34-C)
- EXP34-C/FIO06-C regression investigation from Phase 3

### Real-World FP Reduction — v0.3.25 Findings (Priority 2)

v0.3.25 realworld results (curl + mosquitto + libcrc, 83K violations). Overall −11% vs v0.3.5.

**High-impact FP targets (per-rule across 3 codebases):**

| Rule | Count | Issue | Fix Approach |
|------|------:|-------|--------------|
| ~~EXP34-C~~ | ~~6,679~~ | ~~Struct pointer params flagged as null deref~~ | ✅ Done in v0.3.26 — params default to NotNull without call-site data |
| POS49-C | 4,644 | Every shared field flagged without lock analysis | Without ownership model, too noisy on threaded code. Consider restricting to known-unsafe patterns only. |
| MEM30-C | 3,452 | Use-after-free FPs from sequential struct/member frees | Needs field-level free tracking. Cross-function free propagation. |
| MEM31-C | 3,354 | Leak FPs from cross-function ownership | Needs ownership model (strdup→field→custom_Delete). |
| DCL13-C | 2,373 | Const correctness — pointer params through struct fields | Known alias tracking limitation (ringbuffer.c pattern). |
| ~~PRE00-C~~ | ~~1,709~~ | ~~Every function-like macro flagged~~ | ✅ Done in v0.3.26 — restricted to multi-eval + side effects |
| EXP33-C | 1,394 | Uninitialized variable FPs | CFG integration needed (same as Juliet EXP33-C rewrite). |

**Quick wins (low effort, high impact):**
- ~~PRE00-C: restrict to macros with side effects or multiple-evaluation args~~ ✅ Done in v0.3.26
- ~~EXP34-C param non-null: assume non-null for direct function params~~ ✅ Done in v0.3.26
- POS49-C: suppress unless field is accessed within a known critical section pattern

### Real-World FP — Deferred Hard Issues

Remaining from d_lib_common/d_hal_linux_random triage (require new analysis capabilities):

| Rule | Violations | Issue |
|------|--------:|-------|
| MEM30-C | ~3,452 | Sequential struct/member frees, cross-function free propagation. Needs field-level tracking. |
| MEM31-C | ~3,354 | Cross-function ownership (strdup → struct field → custom \_Delete). Needs ownership model. |

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

### Test Coverage (Priority 2)

Current test infrastructure auto-generates integration tests from `.c` files in `tests/` directories. Coverage gaps:
- Many rules have only wiki-sourced tests (1-3 cases) — need broader pattern coverage
- No tests for inter-procedural analysis paths (prescan, call-site propagation, `-d` directories)
- No regression tests for specific FP patterns fixed in each round (real-world patterns from arraylist.c, intset.c, file_util.c etc.)
- EXP34-C param-null tests had to be restructured when default changed — need tests that exercise with/without project context
- No negative tests for false-positive patterns (verify FP stays suppressed)

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

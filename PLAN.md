# SqC — Plans & Roadmap

**Last Updated**: 2026-03-23 (v0.3.37 changes committed, benchmark pending)

For completed work, see [CHANGELOG.md](CHANGELOG.md).
For benchmark data, see [JULIET_RESULTS.md](JULIET_RESULTS.md) and [REALWORLD_RESULTS.md](REALWORLD_RESULTS.md).
For competitor research, see [RESEARCH.md](RESEARCH.md).

---

## Immediate Next Steps

### TODO: Pin real-world benchmark source commits

Confirm and record exact commit SHAs for all 5 real-world benchmark codebases
(libcrc, sqlite, mosquitto, curl, hostap) in BENCHMARK_INSTALL.md. Currently
no commits are pinned — results may drift as upstream repos change. Verify
tonight on home setup by checking `git rev-parse HEAD` in each clone.

### TODO: Tonight — full 5-codebase benchmark on home setup

1. Rebuild v0.3.36, commit
2. Run full `run_all` benchmark (all 5 codebases) on 24-core home machine
3. Compare against v0.3.34 baseline (same source commits) for clean delta
4. Run Juliet benchmark — expect new TPs from CWE-761/469/464/843, verify no regressions
5. Record results in CHANGELOG.md

**Changes in this branch** (`fix/exp33c-realworld-fp-reduction`):

v0.3.35:
- **EXP33-C**: arr[0].field tracking + initializer suffix matching + zalloc recognition
- **ARR00-C**: Pointer subtraction chain resolution (end = pos + N → same base as pos)
- **ERR33-C**: Suppress all printf-family (fprintf, sprintf, snprintf, vsnprintf, etc.)
- **API07-C**: CWE-761 detection — free(ptr) after pointer arithmetic (984 Juliet files)
- **BENCHMARK_INSTALL.md**: rules-all.toml → rules-benchmark.toml (matches MCP server)

v0.3.36:
- **ARR36-C**: CWE-469 — strchr/wcschr return tracking for cross-array subtraction (36/36 TP, 0 FP)
- **STR03-C**: CWE-464 — (char)atoi() sentinel detection (38/38 TP, 0 FP)
- **API07-C**: CWE-843 — void* type confusion detection (40/40 TP, 0 FP)

v0.3.37:
- **FIO30-C**: CWE-134 format string — recv/recvfrom/recvmsg taint tracking + macro alias resolution + cast/offset expression handling (3,360 Juliet files, previously 0 FIO30-C detections)

Preliminary hostap-only result (work machine, fresh clone at `2a98e6b98`):
- EXP33-C: 3,611 (down from ~5,201 baseline, **-30.6%**)
- Total hostap: 62,393 (vs ~78K baseline, but source version may differ)

### Real-World FP Reduction — Next Targets (Priority 1)

v0.3.34 per-rule data (all 5 codebases, 152.6K total violations, rules-benchmark.toml).
Update with v0.3.35 full benchmark results when available.

| Rule | Count | Issue | Status |
|------|------:|-------|--------|
| MEM30-C | 15,330 | Use-after-free | Needs field-level free tracking (deferred) |
| DCL13-C | 12,138 | Const correctness | Needs alias tracking (deferred) |
| INT32-C | 12,037 | Signed overflow | Stable after VRA — gap is coverage not precision |
| API00-C | 9,227 | Missing size parameter | v0.3.33: -624 from API00-C refinements |
| INT30-C | 8,474 | Unsigned overflow | Stable after VRA |
| EXP33-C | 7,088 | Uninitialized | v0.3.35: arr[0].field fix + suffix matching (hostap -30.6% preliminary) |
| MEM31-C | 5,440 | Memory leak | Needs ownership model (deferred) |
| EXP34-C | 5,267 | Null deref | v0.3.30: -80% via count-based aggregation |
| DCL31-C | 4,840 | Undeclared function | v0.3.32: -54% via prescan |
| DCL07-C | 4,765 | Implicit int declaration | v0.3.32: -55% via prescan |
| ARR00-C | 2,637 | Array bounds | v0.3.35: pointer subtraction chain fix (hostap -34.7% preliminary) |
| ERR33-C | 1,807 | Unchecked return | v0.3.35: printf-family suppressed |

See [CHANGELOG.md](CHANGELOG.md) for completed items.

### EXP33-C — Remaining

- **Cross-function variants 63/64**: pointer passed between source files, needs inter-procedural analysis

---

## Medium Term

### CWE-457: Uninitialized Variable — Remaining Gaps (Priority 1)

v0.3.35: CFG rewrite + arr[0].field fix complete. Remaining gaps:
- Cross-function variants 63/64 (~70 files): pointer passed between source files, needs inter-procedural analysis
- Per-element tracking for stack arrays: `team[0].x = 1; use(team[3].x)` correctly flags, but no way to track that ALL elements are initialized

### CWE-190/191: Integer Overflow/Underflow (Priority 3)

8,904 files, 13.0%/14.5% per-file, 45.3%/43.8% TP rate (v0.3.28). INT30-C/INT32-C matched. Stable after VRA integration — gap is detection coverage, not precision.

### CWE-690: Null Deref from Return (Priority 4)

1,120 files, 25.2% per-file, 82.0% TP rate (v0.3.28). Best-performing high-volume CWE. Getting to 50%+ per-file would make this a showcase. 74% undetected are likely cross-function patterns.

### EXP34-C Phase 4 — Remaining Edge Cases

- Relay chains (3+ hops): multi-pass handles single-hop, deep chains still Unknown
- Indirect data flow (variants 63–67): not addressed
- Cross-file globals (variant 68): not addressed
- EXP34-C/FIO06-C regression investigation from Phase 3

### Real-World FP Reduction — Remaining Targets (Priority 2)

v0.3.28 realworld results (5 codebases, rules-benchmark.toml): curl 31.7K, hostap 78.5K, mosquitto 19.2K, libcrc 419, sqlite pending. Total ~130K (down from ~322K in v0.3.27 with noise rules).

**High-impact FP targets (per-rule across 3 codebases):**

| Rule | Count | Issue | Fix Approach |
|------|------:|-------|--------------|
| MEM30-C | 3,452 | Use-after-free FPs from sequential struct/member frees | Needs field-level free tracking. Cross-function free propagation. |
| MEM31-C | 3,354 | Leak FPs from cross-function ownership | Needs ownership model (strdup→field→custom_Delete). |
| DCL13-C | 2,373 | Const correctness — pointer params through struct fields | Known alias tracking limitation (ringbuffer.c pattern). |
| EXP33-C | 7,088 | Uninitialized variable FPs | CFG rewrite regressed real-world (+2,899). See Immediate Next Steps. |

### Real-World FP — Deferred Hard Issues

Remaining from d_lib_common/d_hal_linux_random triage (require new analysis capabilities):

| Rule | Violations | Issue |
|------|--------:|-------|
| MEM30-C | ~3,452 | Sequential struct/member frees, cross-function free propagation. Needs field-level tracking. |
| MEM31-C | ~3,354 | Cross-function ownership (strdup → struct field → custom \_Delete). Needs ownership model. |

### Zero-Detection CWEs (rules exist but never fire)

19 CWEs have CERT-C rules mapped but produce zero CWE-relevant detections. Most are Windows-only in Juliet. Remaining portable targets:

| CWE | Portable Files | Mapped Rules | Issue | Effort |
|-----|------:|--------------|-------|--------|
| CWE-468 (incorrect pointer scaling) | 36 | ARR39-C, EXP08-C | Implicit void* casts losing type info | High |

**Completed**:
- CWE-761 (free not at start, 984 files) — v0.3.35 API07-C detection added
- CWE-843 (type confusion, 100 C files) — v0.3.36 API07-C void* tracking (40 TP, 0 FP; 60 cross-function)
- CWE-469 (pointer subtraction, 36 files) — v0.3.36 ARR36-C strchr tracking (36 TP, 0 FP)
- CWE-464 (sentinel, 56 files) — v0.3.36 STR03-C atoi detection (38 TP, 0 FP)

**Windows-only** (13 CWEs, not actionable): CWE-114, CWE-226, CWE-259, CWE-272, CWE-273, CWE-327, CWE-367, CWE-459, CWE-666, CWE-789, etc.

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
- [ ] CWE-matched TP rate >= 50% on key CWEs

**Tier 3 — Competitive**
- [ ] Direct benchmarked comparison with Infer, Frama-C (see [RESEARCH.md](RESEARCH.md))
- [ ] Published comparison results
- [ ] Per-file detection >= 30% on top 10 CWEs by volume

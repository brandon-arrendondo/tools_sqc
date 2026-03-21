# SqC — Plans & Roadmap

**Last Updated**: 2026-03-21 (v0.3.27 benchmark noise reduction + parallel scanner)

For completed work, see [CHANGELOG.md](CHANGELOG.md).
For benchmark data, see [JULIET_RESULTS.md](JULIET_RESULTS.md) and [REALWORLD_RESULTS.md](REALWORLD_RESULTS.md).
For competitor research, see [RESEARCH.md](RESEARCH.md).

---

## Immediate Next Steps

### Real-World FP Reduction — Next Targets (Priority 1)

Top remaining FP sources from v0.3.26 per-rule SQLite data (all 5 codebases):

| Rule | Count | Issue | Approach |
|------|------:|-------|----------|
| ~~EXP19-C~~ | ~~42,140~~ | ~~Braceless control flow~~ | ~~Disabled in benchmarks — style rule, all TPs but pure noise~~ |
| EXP34-C | 26,457 | Null deref (post param-fix) | Remaining: struct field chains, cross-function patterns |
| ~~POS49-C~~ | ~~15,693~~ → 107 | ~~Shared field without lock~~ | ~~Restricted to bit-field writes only (commit 8a1778a1). 99.3% reduction.~~ |
| ~~DCL08-C~~ | ~~14,354~~ | ~~Constant variable~~ | ~~Disabled in benchmarks — recommendation rule, zero Juliet impact~~ |
| INT32-C | 12,077 | Signed overflow | Stable after VRA — gap is coverage not precision |
| DCL07-C | 11,237 | Implicit int declaration | Cross-file prescan limitation |
| DCL31-C | 10,620 | Undeclared function | Cross-file prescan limitation |
| API00-C | 10,072 | Missing size parameter | Post caller-aware suppression |
| ~~EXP14-C~~ | ~~8,028~~ | ~~Cast loses qualifiers~~ | ~~Disabled in benchmarks — recommendation rule, zero Juliet impact~~ |
| INT30-C | 8,508 | Unsigned overflow | Stable after VRA |

**Quick wins:**
- ~~POS49-C: bit-field-only restriction reduced 15,693 → 107 violations (99.3%)~~
- ~~EXP19-C: disabled in rules-benchmark.toml — style-only rule, 42K TPs but pure noise~~
- ~~Benchmark noise audit: 12 recommendation rules disabled in rules-benchmark.toml (~72K violations removed)~~

**Completed — Benchmark noise audit (v0.3.27):**
13 style/recommendation rules now disabled in `rules-benchmark.toml` (zero Juliet CWE contribution, ~114K realworld violations of pure noise removed): EXP19-C, DCL08-C, DCL06-C, EXP02-C, EXP14-C, EXP12-C, EXP10-C, DCL04-C, INT02-C, INT01-C, INT17-C, INT16-C, PRE31-C.

### Benchmark Infrastructure: Remaining

- ~~Phase 3: Real-world SQLite integration~~ ✅ Done in v0.3.26 — `realworld_violations` table, auto-ingest in MCP, `compare_realworld_runs()`, v0.3.5/v0.3.25/v0.3.26 ingested
- ~~Phase 5: Parallel realworld scanner~~ ✅ Done in v0.3.27 — `scripts/sqc_parallel_scan.py` splits codebases by subdirectory, runs via ProcessPoolExecutor. MCP server uses it for all sqc scans.
- ~~Phase 6: Benchmark noise reduction~~ ✅ Done in v0.3.27 — `rules-benchmark.toml` with 13 noisy rules disabled (~114K violations removed)
- Phase 7: **Prescan cache** (`--save-prescan` / `--load-prescan`) — serialize `ProjectContext` to file so parallel workers skip repeated prescan. Currently each worker re-prescans the full codebase (~10-20s × N workers). See Architecture Evolution below.
- Phase 2: `get_performance_trend()`, `get_realworld_rule_trend()` query tools
- Phase 4: Remove legacy shell scripts after full migration validation

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
| ~~POS49-C~~ | ~~15,693~~ → 107 | ~~Every shared field flagged~~ | ~~Restricted to bit-field writes only (v0.3.27). 99.3% reduction.~~ |
| MEM30-C | 3,452 | Use-after-free FPs from sequential struct/member frees | Needs field-level free tracking. Cross-function free propagation. |
| MEM31-C | 3,354 | Leak FPs from cross-function ownership | Needs ownership model (strdup→field→custom_Delete). |
| DCL13-C | 2,373 | Const correctness — pointer params through struct fields | Known alias tracking limitation (ringbuffer.c pattern). |
| EXP33-C | 4,700 | Uninitialized variable FPs | CFG integration needed (same as Juliet EXP33-C rewrite). |

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

- [ ] **Prescan cache** (Priority 1) — `sqc prescan /path -o prescan.db` + `sqc /path --load-prescan prescan.db`. Serialize `ProjectContext` (known_functions, function_summaries, call_graph, macro_constants, macro_aliases, struct_field_types) to a binary file. Eliminates repeated prescan in parallel scanning (28 workers × ~15s prescan = ~7 min wasted CPU on hostap). All fields are `HashMap`/`HashSet` of `String`/`i64`/enums — straightforward serde. Could use bincode or MessagePack for speed. The parallel scanner would generate cache once, then each worker loads it instead of `-d`. Also useful for CI incremental: prescan once, analyze changed files only.
- [ ] **Internal parallelization** — rayon for file-level parallelism within a single sqc invocation
- [x] **External parallelization** — `scripts/sqc_parallel_scan.py` splits by subdirectory, runs N sqc processes (v0.3.27). Uses prescan cache to avoid repeated prescan per worker (v0.3.28).
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

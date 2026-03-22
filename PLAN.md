# SqC — Plans & Roadmap

**Last Updated**: 2026-03-22 (v0.3.32 prescan improvements)

For completed work, see [CHANGELOG.md](CHANGELOG.md).
For benchmark data, see [JULIET_RESULTS.md](JULIET_RESULTS.md) and [REALWORLD_RESULTS.md](REALWORLD_RESULTS.md).
For competitor research, see [RESEARCH.md](RESEARCH.md).

---

## Immediate Next Steps

### Real-World FP Reduction — Next Targets (Priority 1)

v0.3.31 per-rule data (all 5 codebases, 161.9K total violations, rules-benchmark.toml):

| Rule | Count | Issue | Approach |
|------|------:|-------|----------|
| MEM30-C | 15,330 | Use-after-free | Needs field-level free tracking (deferred) |
| DCL13-C | 12,138 | Const correctness | Needs alias tracking (deferred) |
| INT32-C | 12,034 | Signed overflow | Stable after VRA — gap is coverage not precision |
| DCL07-C | 10,617 | Implicit int declaration | v0.3.32: prescan deep recursion + macro alias + library whitelist |
| DCL31-C | 10,543 | Undeclared function | v0.3.32: prescan deep recursion + macro alias + library whitelist |
| API00-C | 9,851 | Missing size parameter | Post caller-aware suppression |
| INT30-C | 8,474 | Unsigned overflow | Stable after VRA |
| EXP34-C | 5,290 | Null deref | v0.3.30: -80% via count-based aggregation |
| MEM31-C | 5,440 | Memory leak | Needs ownership model (deferred) |
| EXP33-C | 4,189 | Uninitialized | v0.3.31: -8%, remaining FPs need CFG rewrite |
| ARR00-C | 2,637 | Array bounds | v0.3.31: -64%, remaining are pointer subtraction/negative index |
| ERR33-C | 1,807 | Unchecked return | v0.3.31: -75%, remaining are fclose/snprintf/getenv |

See [CHANGELOG.md](CHANGELOG.md) for completed items.

### Benchmark Infrastructure: Remaining

- [x] Per-rule violation counts in realworld benchmark DB (1.6M individual violations stored, per-rule queries exposed via MCP tools)
- [x] `get_rule_trend()` and `get_project_history()` MCP query tools
- [x] SQLite-first pattern for realworld `get_results()`, `compare_runs()`, `list_runs()` (matches Juliet pattern)
- Remove legacy shell scripts after full migration validation

### Prescan Quality Audit (Priority 2)

**v0.3.32 audit complete.** Found three root causes for DCL07-C/DCL31-C FPs:

1. **`collect_function_names` missed ERROR nodes** — tree-sitter misparses files with complex macros (e.g., sqlite's `sqliteInt.h`), burying `function_definition` nodes inside `ERROR` or `compound_statement` at the top level. Fixed: now recurses into ALL child nodes. Found 358 additional functions in sqlite alone.

2. **Macro aliases not used by DCL rules** — prescan collected `macro_aliases` but DCL07-C/DCL31-C didn't check them. Fixed: alias names added to `cross_file_functions`.

3. **External library APIs** — Tcl/Tk, OpenSSL, mbedTLS, etc. come from system headers outside scanned dirs. Fixed: `is_external_library_function()` whitelist.

**Remaining limitation**: tree-sitter error recovery is imperfect. Some files (e.g., sqlite's `prepare.c` — 17 lost functions) have parse errors that bury function definitions in structures that even deep recursion can't fully recover. `sqlite3_prepare_v2` is defined in `prepare.c:937` but lost in error recovery. This is a diminishing-returns area — fixing it would require either preprocessor expansion or a fallback regex-based function scanner.

**Other consumers checked (no issues found)**:
- EXP34-C — uses callsite_param_null_states (count-based aggregation, fixed in v0.3.30)
- API00-C — uses callsite_param_null_states + function_summaries (working correctly)
- MEM10-C — uses function_summaries for null guard suppression (working correctly)
- INT32-C/INT30-C — uses struct_field_types + macro_constants (working correctly)

---

## Medium Term

### CWE-457: Uninitialized Variable — Remaining Gaps (Priority 1)

616 files, 28.7% per-file, 31.8% TP rate (v0.3.28 benchmark). v0.3.26 addresses the +109 FP regression via early-return branch detection. Remaining gaps:
- Cross-function variants 63/64 (~70 files): pointer passed between source files, needs inter-procedural analysis
- Array partial_init through alloca/malloc (~66 files): partial subscript init upgrades to MallocInitialized, losing content-level tracking
- struct variant 12: struct field access pattern edge case

### CWE-190/191: Integer Overflow/Underflow (Priority 3)

8,904 files, 13.0%/14.5% per-file, 45.3%/43.8% TP rate (v0.3.28). INT30-C/INT32-C matched. Stable after VRA integration — gap is detection coverage, not precision.

### CWE-690: Null Deref from Return (Priority 4)

1,120 files, 25.2% per-file, 82.0% TP rate (v0.3.28). Best-performing high-volume CWE. Getting to 50%+ per-file would make this a showcase. 74% undetected are likely cross-function patterns.

### EXP34-C Phase 4 — Remaining Edge Cases

- Relay chains (3+ hops): multi-pass handles single-hop, deep chains still Unknown
- Indirect data flow (variants 63–67): not addressed
- Cross-file globals (variant 68): not addressed
- EXP33-C CFG integration (needs full rewrite like EXP34-C)
- EXP34-C/FIO06-C regression investigation from Phase 3

### Real-World FP Reduction — Remaining Targets (Priority 2)

v0.3.28 realworld results (5 codebases, rules-benchmark.toml): curl 31.7K, hostap 78.5K, mosquitto 19.2K, libcrc 419, sqlite pending. Total ~130K (down from ~322K in v0.3.27 with noise rules).

**High-impact FP targets (per-rule across 3 codebases):**

| Rule | Count | Issue | Fix Approach |
|------|------:|-------|--------------|
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

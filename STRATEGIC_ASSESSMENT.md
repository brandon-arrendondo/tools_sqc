# SqC Strategic Assessment

**Date**: 2026-02-17
**Status**: Draft — competitor benchmarks still needed

---

## Current State (Post Round 8)

| Metric | Value |
|--------|-------|
| **Rules Implemented** | 283 CERT C rules |
| **Juliet Files** | 54,484 |
| **True Positives** | 230,992 |
| **False Positives** | 296,415 |
| **TP Rate** | 43.8% |
| **FP Reduction from Baseline** | -64.7% (839K → 296K) |
| **CWE Categories with Data** | 106 / 118 |
| **Categories >50% TP** | 18 |

### Top Remaining FP Rules

| Rule | FP | TP | Notes |
|------|---:|---:|-------|
| INT32-C | 23K | 16K | Type-aware inference already applied |
| DCL31-C | 21K | 16K | Cross-file + std_functions already applied |
| DCL07-C | 20K | 16K | Cross-file + std_functions already applied |
| INT30-C | 17K | 17K | ~50/50 ratio, reductions lose TPs |
| EXP34-C | 15K | 12K | Null pointer — already tightened |
| DCL06-C | 14K | 19K | Code style — ~50/50, reductions lose TPs |
| EXP12-C | 9K | 10K | Whitelist already trimmed |
| MEM10-C | 7K | 6K | ~50/50 ratio |
| ERR33-C | 6K | 4K | Nested calls + math overlap fixed |

**Key insight**: Most remaining top FP rules have ~50/50 TP/FP ratios. Further reductions will proportionally lose TPs. Diminishing returns on the current approach.

---

## Architecture Assessment

### What SqC Is
- **Single-translation-unit, AST-based pattern matcher** using tree-sitter
- 283 rules ranging from shallow pattern matching (<100 lines) to deep multi-pass analysis (3,900 lines for ARR30-C)
- Limited cross-file analysis (function name pre-scanning via `-d` flag)
- Sequential file processing (parallelized externally via shell scripts)

### What SqC Has
- Local variable/type inference within functions (collect_variable_types pattern)
- Preprocessor block traversal (preproc_* node recursion)
- Standard function database (~270 C11/POSIX functions)
- Cross-file function name scanning
- Taint tracking for some rules (FIO30-C)
- Variable state tracking (EXP33-C uninitialized variable detection)

### What SqC Lacks
- **No preprocessor expansion** — macros appear as function calls
- **No inter-procedural data-flow** — can't track values across function calls
- **No control-flow graph** — no dominance/postdominance analysis
- **No alias analysis** — pointer aliasing not resolved
- **No symbolic execution** — can't evaluate complex expressions
- **No SSA form** — no reaching definitions / use-def chains
- **No value range analysis** — beyond literal constants
- **No whole-program analysis** — limited to function name pre-scanning

### Implications
The ~43.8% TP rate ceiling is likely an architectural constraint. Without data-flow and control-flow analysis, the tool cannot distinguish:
- Variables that have been validated from those that haven't
- Paths where errors are handled from those where they aren't
- Pointers that have been null-checked from those that haven't

This means many rules must use heuristics (name matching, proximity checking, type inference) that inherently produce false positives.

---

## CI/CD Readiness Assessment

### Overall: ~80% Ready (was ~45-50% before Phase 1)

| Component | Status | Readiness |
|-----------|--------|-----------|
| Output Formats | CSV, XLSX, JSON, SARIF 2.1.0 | 100% |
| Exit Codes | `--fail-on-violation`, `--fail-on-severity` | 100% |
| Suppressions | Code-location SHA-256 | 70% — Not baseline-aware |
| Performance | ~3-36 files/sec | 80% — Acceptable for CI/CD |
| CLI Ergonomics | Fail flags, severity/rule filters, diff mode | 90% |
| Documentation | README CI/CD section, no Docker | 60% — No CI/CD guides, no Docker |
| Filtering | `--min-severity`, `--rules` CLI flags | 100% |
| Incremental | `--diff` flag (git modified files) | 90% — No baseline comparison |

### Completed (Phase 1)

1. ~~No `--fail-on-violation` flag~~ — **Done**: `--fail-on-violation` and `--fail-on-severity`
2. ~~No SARIF output~~ — **Done**: `--export results.sarif` (SARIF 2.1.0)
3. ~~No JSON output~~ — **Done**: `--export results.json`
4. ~~No incremental mode~~ — **Done**: `--diff` flag calls `get_modified_c_files()`
5. ~~No severity threshold~~ — **Done**: `--fail-on-severity` and `--min-severity`
6. ~~No CLI filtering~~ — **Done**: `--rules` comma-separated filter

### Remaining Gaps

1. **No baseline comparison** — can't say "only flag new violations since last run"
2. **No GitHub Actions / Azure DevOps example workflows**
3. **No Docker image** for containerized CI/CD
4. **Exit code 2 for errors** — implemented but not documented in CI guides

### What Works Today
- `--export results.sarif` for GitHub Code Scanning / Azure DevOps integration
- `--export results.json` for piping to other tools
- `--fail-on-violation` / `--fail-on-severity High` to gate CI builds
- `--diff` for incremental analysis on PRs
- `--min-severity Medium --rules ARR30-C,MEM30-C` for targeted scanning
- Manifest files to enable/disable rules per project
- Suppress known false positives with inline comments
- Parallelize across directories with shell scripts

---

## Competitor Research — TODO

**Need to investigate** (web search was not completed):
- NIST SATE (Static Analysis Tool Exposition) results
- Cppcheck Juliet benchmark data
- Clang Static Analyzer Juliet data
- Coverity / PVS-Studio published accuracy claims
- Academic papers comparing static analysis tools on Juliet
- Flawfinder / RATS benchmark data

**Key questions**:
1. What TP rate do commercial tools achieve on Juliet?
2. What FP rate is considered acceptable in industry?
3. Is tree-sitter-based AST analysis competitive, or is LLVM IR / data-flow required?
4. What does NIST SATE consider "good" performance?

---

## Definition of "Done" — Draft Criteria

### Tier 1: Minimum Viable for CI/CD
- [x] `--fail-on-violation` and `--fail-on-severity` flags
- [x] JSON output format
- [x] SARIF output for native platform integration
- [x] Exit code reflects violation presence
- [x] Incremental analysis (only changed files) — `--diff`
- [x] Severity threshold filtering from CLI — `--min-severity`, `--rules`
- [ ] Basic GitHub Actions / Azure DevOps example workflow
- [ ] TP rate >= 45% on Juliet (currently 43.8%)
- [ ] Can run on real codebase without excessive false positives

### Tier 2: Production Quality
- [ ] Baseline-aware suppression ("only new violations")
- [ ] Docker image for containerized CI/CD
- [ ] Documentation: integration guides for 3+ CI platforms
- [ ] Real-world validation on 3+ open-source projects (curl, openssl, zlib)

### Tier 3: Competitive
- [ ] TP rate >= 50% on Juliet
- [ ] Benchmarked against Cppcheck and Clang Static Analyzer
- [ ] Data-flow analysis for key rules (MEM30-C, EXP34-C)
- [ ] Inter-procedural analysis for at least top-10 rules
- [ ] Published comparison results

---

## Recommended Next Steps (Priority Order)

### Phase 1: CI/CD Enablement — COMPLETED
1. ~~Add `--fail-on-violation` / `--fail-on-severity` exit codes~~ — Done
2. ~~Add JSON output format~~ — Done
3. ~~Add SARIF export~~ — Done
4. ~~Wire up `get_modified_c_files()` for `--diff` mode~~ — Done
5. ~~Add `--rule-filter` and `--severity-filter` CLI flags~~ — Done (`--rules`, `--min-severity`)
6. Create GitHub Actions example workflow — Remaining

### Phase 2: Real-World Validation
7. Run on curl, openssl, zlib — measure FP density per KLOC
8. Tune rules based on real-world FP patterns (not just Juliet)
9. Compare results to Cppcheck on same codebases
10. Document "recommended rule sets" for different project types

### Phase 3: Continued FP Reduction
11. Focus on rules with TP/FP ratio > 1.5:1 (still have headroom)
12. Add basic data-flow for MEM30-C (use-after-free tracking)
13. Add basic control-flow for EXP34-C (null-check-before-deref)
14. Consider LLVM IR integration for deeper analysis

### Phase 4: Architecture Evolution
15. Internal parallelization (rayon for file-level parallelism)
16. Incremental parsing with tree-sitter (only re-parse changed files)
17. Cross-file data-flow (function summaries)
18. Plugin system for custom rules

---

## FP Reduction History

| Round | Rules Fixed | FP | TP Rate | FP Delta |
|-------|-----------|---:|--------:|---------:|
| Baseline | -- | 839,341 | 41.1% | -- |
| R1 | INT08-C, CON08-C, DCL20-C, ARR38-C | 752,422 | 42.3% | -86,919 |
| R2 | EXP33-C, SIG31-C, ARR01-C, DCL30-C, DCL02-C | 736,563 | 43.0% | -15,859 |
| R3 | DCL31-C, DCL07-C, FLP34-C | 537,589 | 42.8% | -198,974 |
| R4 | EXP12-C, FLP03-C, INT32-C | 492,648 | 42.5% | -44,941 |
| R5 | FLP02-C, DCL06-C, INT30-C | 475,813 | 41.7% | -16,835 |
| R6 | Cross-file analysis (`-d`) | 327,191 | 43.1% | -148,622 |
| R7 | EXP36-C, EXP34-C, ARR37-C | 301,475 | 43.4% | -25,716 |
| R8 | DCL40-C, FLP32-C, ERR33-C | 296,415 | 43.8% | -5,060 |

**Trend**: Diminishing returns on FP reduction via rule tuning. R3 removed 199K FP; R8 removed 5K. The remaining FPs require architectural changes (data-flow, control-flow) or are inherent to the 50/50 TP/FP rules.

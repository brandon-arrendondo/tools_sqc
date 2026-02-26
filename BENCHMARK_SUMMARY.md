# SqC — Benchmark Strategy & Architecture

**Last Updated**: 2026-02-25

Overall strategy for benchmarking sqc, architectural assessment, and CI/CD readiness.

---

## Benchmark Strategy

SqC is benchmarked on two axes:

1. **Juliet Test Suite** (NIST) — 54,484 files with ground truth (OMITBAD/OMITGOOD sections). Measures TP rate, FP rate, and per-CWE coverage. See [JULIET_RESULTS.md](JULIET_RESULTS.md).

2. **Real-World Open-Source Projects** — 5 codebases (libcrc, sqlite, mosquitto, curl, hostap) analyzed by sqc, cppcheck, and clang-tidy. No ground truth — measures violation counts, rule distribution, and cross-tool agreement. See [REALWORLD_RESULTS.md](REALWORLD_RESULTS.md).

### Why Both

- **Juliet** provides precision metrics (TP/FP) but is synthetic single-file code
- **Real-world** tests scalability, noise levels, and cross-file analysis on production code
- Rule improvements are validated on Juliet for TP/FP impact, then verified on real-world for noise reduction

### Benchmark Cadence

- **After every significant rule change**: Juliet benchmark (MCP server, ~40 min)
- **After version milestones**: Full real-world benchmark (MCP server, all 5 codebases × 3 tools)
- **cppcheck/clang-tidy results are stable** across sqc changes — run once and cache

---

## Architecture Assessment

### What SqC Is

- **Single-translation-unit, AST-based pattern matcher** using tree-sitter
- 283 rules ranging from shallow pattern matching to deep multi-pass analysis
- Cross-file analysis via function name pre-scanning (`-d` flag)
- Sequential file processing (parallelized externally)

### What SqC Has (v0.2.7)

| Capability | Status |
|-----------|--------|
| Local variable/type inference | Per-function `collect_variable_types` |
| Preprocessor block traversal | `preproc_*` node recursion |
| Standard function database | ~370 C11/POSIX/Windows functions |
| Cross-file function scanning | `-d` flag pre-scan |
| CFG construction | Per-function with `condition_range` metadata |
| Reaching definitions | Data-flow for path-sensitive analysis |
| Inter-procedural summaries | Null returns, freed params, no-return |
| CFG-based null state dataflow | Forward dataflow with NullState lattice |
| Taint tracking | FIO30-C |
| Variable state tracking | EXP33-C uninitialized detection |

### What SqC Lacks

| Gap | Impact |
|-----|--------|
| No preprocessor expansion | Macros appear as function calls |
| No alias analysis | Pointer aliasing unresolved |
| No symbolic execution | Can't evaluate complex expressions |
| No SSA form | No use-def chains beyond reaching defs |
| No value range analysis | Beyond literal constants |
| No whole-program analysis | Limited to function summary pre-scan |

### Architectural Ceiling

The ~44% Juliet TP rate is likely near the ceiling for single-TU AST analysis. Without value-range and alias analysis, the tool cannot distinguish validated from unvalidated inputs, null-checked from unchecked pointers, or computed from literal buffer sizes.

---

## Methodology Notes

### Apples-to-Apples Concerns

1. **Rule coverage**: cppcheck/clang-tidy implement ~20 checks each vs. sqc's 283 rules. Raw counts are not directly comparable.

2. **Translation unit scope**: Use consistent scope (cross-file `-d` flag or single-file) when comparing.

3. **Preprocessor handling**: cppcheck evaluates all `#ifdef` configs; clang-tidy sees one; sqc analyzes all visible branches. For Juliet, compile with `-DOMITBAD`/`-DOMITGOOD` when needed.

4. **Standard library awareness**: cppcheck/clang-tidy have built-in stdlib knowledge. sqc uses `std_functions.rs` database.

5. **Severity mapping**: cppcheck `error/warning/style`, clang-tidy `error/warning`, sqc `Low/Medium/High/Critical`. Map conservatively.

### Recommended Comparison Workflow

1. Pick a representative codebase or CWE subset
2. Run all tools with consistent flags
3. Normalize to `(file, line, rule/check-id)` tuples
4. Classify as TP/FP using Juliet ground truth
5. Compute precision, recall, F1 per tool
6. Restrict to overlapping rules for fair comparison

### Published CERT-C Results (Literature Gap)

No published CERT-C violation rates per KLOC on production open-source code exist. Valid comparison strategies:

1. sqc vs. cppcheck vs. clang-tidy on same codebase (done for 5 projects)
2. sqc on JasPer with reference to SEI SCALe 2015 report (only named CERT-C audit)
3. sqc TP rate vs. TrustInSoft's synthetic CERT-C benchmark as upper bound

---

## CI/CD Readiness (~85%)

| Component | Status | Readiness |
|-----------|--------|-----------|
| Output Formats | CSV, XLSX, JSON, SARIF 2.1.0 | 100% |
| Exit Codes | `--fail-on-violation`, `--fail-on-severity` | 100% |
| Severity Filtering | `--min-severity`, `--fail-on-severity` | 100% |
| Rule Filtering | `--rules ARR30-C,MEM30-C` | 100% |
| Incremental | `--diff` (git modified files) | 90% |
| CI Workflows | GitHub Actions + Azure DevOps templates | 100% |
| Suppressions | SHA-256 code-location | 70% |
| Docker | No image published | 0% |

### Remaining Gaps

1. **No baseline-aware suppression** — can't report "only new violations since last run"
2. **No Docker image** for containerized CI/CD
3. **Unclassified real-world violation density** — no ground truth to split TP vs FP on production code

---

## Competitor Landscape

| Tool | Detection Rate | FP Rate | Analysis Depth | Price |
|------|---------------:|--------:|----------------|:-----:|
| **SqC** | 44.5% | 55.5% | AST + CFG + inter-procedural | -- |
| Semgrep CE | 44–48% | Very low | AST (tree-sitter) | Free |
| Semgrep Pro | 72–75% | Very low | AST + taint + inter-file | Commercial |
| Infer | ~55% | ~45% | Separation logic | Free |
| Flawfinder | ~40% | High | Lexical scanning | Free |
| Cppcheck | Low | Very low | Data-flow | Free |
| Coverity | Best-in-class | ~15–20% | Inter-procedural, path-sensitive | Enterprise |

**Key context**: Tools on average find ~20% of weaknesses in Juliet (ISSTA 2022). Even commercial tools miss 27% (Goseva 2015). Industry FP target for adoption is 10–20%.

---

## Resolved Issues

### DCL02-C Stack Overflow (Fixed 2026-01-07)

Unbounded recursive AST traversal in DCL02-C caused stack overflow on large files (SQLite). Converted to iterative with depth limit.

### STR31-C `detect_manual_string_loop` Runaway (Fixed 2026-02-25)

Caused 36–49% of all violations on 3 of 5 real-world projects. File-wide fallback removed; pattern matching restricted to loop condition and body.

### Output Buffer Saturation During Benchmarks

SqC emits one status line per rule per file (~100 rules × N files). Always suppress or redirect output during scans:
```bash
./target/release/sqc directory/ --export results.csv 2>/dev/null
```

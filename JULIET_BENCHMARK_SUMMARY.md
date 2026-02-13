# SqC vs. NIST Juliet Test Suite - Benchmark Summary

**Last Updated**: 2026-02-13
**Benchmark**: NIST Juliet Test Suite v1.3 for C/C++
**Categories Tested**: 118 (all CWE categories in Juliet)

---

## Executive Summary

SqC was benchmarked against all 118 CWE categories in the NIST Juliet Test Suite, covering **54,484 test files** and detecting over **3.6 million CERT C violations**. Ground truth analysis using Juliet's OMITBAD/OMITGOOD annotations yields a weighted average true positive rate of **43.0%** across 106 categories with data.

17 categories achieve >50% TP rate, with the best at 86.1% (CWE-506). Three rounds of targeted rule fixes have progressively improved both precision and recall.

## Latest Results (2026-02-13)

### Aggregate Metrics

| Metric | Value |
|--------|-------|
| **Files Analyzed** | 54,484 |
| **Total Violations** | ~3.6M |
| **Classified (TP+FP)** | 1,292,263 |
| **True Positives** | 555,700 |
| **False Positives** | 736,563 |
| **Weighted TP Rate** | **43.0%** |
| **Categories with data** | 106 / 118 |
| **Wall-clock time** | ~25 min (12 parallel jobs) |

### Improvement History

| Round | Fixes | Total TP | Total FP | TP Rate | FP Delta |
|-------|-------|----------|----------|---------|----------|
| Baseline (2026-01-15) | -- | 586,539 | 839,341 | 41.1% | -- |
| Round 1 (2026-02-12) | INT08-C, CON08-C, DCL20-C, ARR38-C | 552,645 | 752,422 | 42.3% | -86,919 |
| **Round 2 (2026-02-13)** | **EXP33-C, SIG31-C, ARR01-C, DCL30-C, DCL02-C** | **555,700** | **736,563** | **43.0%** | **-15,859** |

**Cumulative improvement**: TP rate 41.1% -> 43.0% (+1.9pp), FP reduced by 102,778 (-12.2%).

### Round 2 Fix Details

1. **EXP33-C**: Functions inside `#ifdef`/`#ifndef` preprocessor blocks were invisible to the uninitialized-variable analysis. tree-sitter nests these inside `preproc_*` nodes; the rule only iterated direct children of `translation_unit`. Fixed with recursive collector. **Impact: +3,055 TP across all categories.**

2. **SIG31-C, ARR01-C, DCL30-C**: Same preprocessor-block bug for file-scope declaration scanning. Fixed with identical pattern.

3. **DCL02-C**: Visual-similarity check fired on identical identifiers in different scopes (e.g., `int i` in 12 different for-loops). Added check that flagged identifiers must actually be different strings. **Impact: -15,859 FP; CWE-457 TP rate 12.2% -> 22.6%.**

---

## Performance Tiers

### Tier 1: Strong Detection (TP > 50%) - 17 categories

| CWE | Category | TP Rate | Files |
|-----|----------|---------|-------|
| 506 | Embedded Malicious Code | 86.1% | 158 |
| 15 | External Control of System/Config | 74.8% | 56 |
| 427 | Uncontrolled Search Path Element | 73.2% | 560 |
| 78 | OS Command Injection | 72.4% | 5,600 |
| 617 | Reachable Assertion | 69.3% | 354 |
| 197 | Numeric Truncation Error | 67.9% | 1,008 |
| 123 | Write-What-Where Condition | 64.9% | 168 |
| 114 | Process Control | 64.0% | 672 |
| 194 | Unexpected Sign Extension | 59.5% | 1,344 |
| 510 | Trapdoor | 58.8% | 70 |
| 195 | Signed-to-Unsigned Conversion | 57.6% | 1,344 |
| 90 | LDAP Injection | 55.1% | 560 |
| 464 | Data Structure Sentinel Addition | 54.4% | 56 |
| 526 | Info Exposure via Environment Variables | 54.3% | 18 |
| 587 | Assignment of Fixed Address to Pointer | 53.7% | 18 |
| 680 | Integer Overflow to Buffer Overflow | 53.3% | 336 |
| 188 | Reliance on Data/Memory Layout | 51.0% | 36 |

### Tier 2: Moderate Detection (35-50%) - 60 categories

The bulk of categories (57%) cluster in this range. Includes major categories like buffer overflows (CWE-121 at 43.3%, CWE-122 at 41.7%), format strings (CWE-134 at 36.7%), and resource management issues.

### Tier 3: Below Average (25-35%) - 26 categories

Includes integer overflow/underflow (CWE-190/191 at ~32%), memory management (CWE-401 at 32.1%, CWE-415 at 33.4%), and NULL pointer dereference (CWE-476 at 33.1%).

### Tier 4: Weak Detection (< 25%) - 3 categories

| CWE | Category | TP Rate | Root Cause |
|-----|----------|---------|------------|
| 338 | Weak PRNG | 24.0% | No SqC rule for PRNG quality |
| 256 | Plaintext Password Storage | 15.2% | No credential storage rules |
| 457 | Uninitialized Variable | 22.6% | Improved from 12.2% after EXP33-C + DCL02-C fixes |

---

## Methodology

### Ground Truth Classification

Juliet test files contain preprocessor-guarded sections:
- **`#ifndef OMITBAD`**: Code with known vulnerabilities (violations here = True Positives)
- **`#ifndef OMITGOOD`**: Code without vulnerabilities (violations here = False Positives)
- **`/* FLAW: */`**: Comments marking exact vulnerability locations

### Metrics
- **TP Rate** = Violations in OMITBAD / (Violations in OMITBAD + OMITGOOD)
- **FP Rate** = Violations in OMITGOOD / (Violations in OMITBAD + OMITGOOD)
- Violations outside both sections are excluded from classification

### Scan Configuration
- **SqC version**: 0.1.0
- **Rule manifest**: rules-all.toml (all CERT C rules enabled)
- **Parallelism**: 12 concurrent sqc processes
- **Total wall-clock time**: ~25 minutes (parallel)

### Limitations
1. SqC applies all CERT C rules to every file - most rules are not relevant to the specific CWE
2. OMITBAD sections contain both vulnerable code AND supporting code
3. FLAW line detection is ~0% because SqC reports on code lines, not adjacent comment lines
4. TP/FP classification is at the violation level, not file level

---

## Files and Locations

### Benchmark Data
```
~/data/benchmarks/juliet-test-suite-c/
  testcases/                          118 CWE categories, 54,484 .c files
```

### Results
```
/tmp/juliet_results/
  CWE{id}_{name}.csv                  Raw SqC CSV output per CWE
  CWE{id}_{name}_analysis.txt         Ground truth analysis per CWE
  multi_cwe_summary.txt                TP/FP rates summary
```

### Scripts
```
scripts/analyze_juliet_results.py      Ground truth analysis (any CWE)
scripts/run_juliet_multi_cwe.sh        Sequential multi-CWE runner
scripts/run_juliet_parallel.sh         Parallel multi-CWE runner (12 jobs)
```

### Related Documentation
- `JULIET_MULTI_CWE_BENCHMARK.md` - Full per-CWE results table (106 categories)
- `JULIET_FALSE_POSITIVE_ANALYSIS.md` - Detailed FP/FN analysis
- `JULIET_HANG_INVESTIGATION.md` - Resolved hang during directory scans

---

## Next Steps

### Short-Term
- Investigate remaining high-FP rules for further precision improvements
- Add CWE-specific rule weighting/filtering
- Implement data-flow analysis rules for CWE-416, CWE-401, CWE-476

### Medium-Term
- Add rules for CWE-338 (PRNG quality), CWE-256 (credential storage)
- Expand to real-world projects (curl, openssl, zlib)
- Compare findings with known CVEs

### Long-Term
- Run Clang Static Analyzer and Cppcheck on same files for direct comparison
- Publish results (blog post, NIST SAMATE submission)
- Create public benchmark dashboard

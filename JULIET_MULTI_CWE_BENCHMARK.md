# SqC Full Juliet Benchmark Results (118 CWE Categories)

**Date**: 2026-02-12
**Benchmark**: NIST Juliet Test Suite v1.3 for C/C++
**Categories Tested**: 118 (all CWE categories in Juliet)
**Categories with Violations**: 106 (12 had no C test files or no violations)
**Total Files Analyzed**: 54,484
**Total Violations Detected**: 4,049,069
**Classified Violations (TP+FP)**: 1,305,067

---

## Executive Summary

SqC was benchmarked against all 118 CWE categories in the NIST Juliet Test Suite, covering 54,484 test files and detecting over 4.0 million CERT C violations. Ground truth analysis using Juliet's OMITBAD/OMITGOOD annotations yields a weighted average true positive rate of **42.3%** across 106 categories with data, with individual CWE performance ranging from **12.2%** (CWE-457) to **86.1%** (CWE-506).

17 categories achieve >50% TP rate, 60 categories fall in the 35-50% range, 26 categories are in the 25-35% range, and 3 categories score below 25%. The 12 remaining categories had no C test files or produced zero violations.

### Improvement Over Baseline

After a round of precision-focused rule fixes, the benchmark shows:

| Metric | Before Fixes | After Fixes | Delta |
|---|---|---|---|
| Total False Positives | 839,341 | 752,422 | **-86,919 (-10.4%)** |
| Total True Positives | 586,539 | 552,645 | -33,894 (-5.8%) |
| Weighted TP Rate | 41.1% | 42.3% | **+1.2 pp** |
| Total Violations | 4,380,552 | 4,049,069 | -331,483 (-7.6%) |

Rules fixed:
- **INT08-C**: Removed `int` from "narrow type" definition (was a bug; `int` is not narrow per CERT)
- **CON08-C**: Only flag when calling multiple *atomic* functions without mutex (was flagging every 2+ function call)
- **DCL20-C**: Only flag declarations/prototypes, not definitions (definitions aren't the actual semantic risk)
- **ARR38-C**: Removed duplicate `strcpy`/`strcat` flagging (already covered by STR31-C)

---

## Full Results Table

| CWE | Vulnerability Type | Files | TP | FP | TP Rate | FP Rate |
|-----|-------------------|-------|-----|-----|---------|---------|
| **506** | Embedded Malicious Code | 158 | 3,421 | 552 | **86.1%** | 13.9% |
| **15** | External Control of System/Config Setting | 56 | 1,255 | 422 | **74.8%** | 25.2% |
| **427** | Uncontrolled Search Path Element | 560 | 7,656 | 2,798 | **73.2%** | 26.8% |
| **78** | OS Command Injection | 5,600 | 79,292 | 30,203 | **72.4%** | 27.6% |
| **617** | Reachable Assertion | 354 | 2,685 | 1,192 | **69.3%** | 30.7% |
| **197** | Numeric Truncation Error | 1,008 | 7,899 | 3,733 | **67.9%** | 32.1% |
| **123** | Write-What-Where Condition | 168 | 2,239 | 1,213 | **64.9%** | 35.1% |
| **114** | Process Control | 672 | 8,839 | 4,973 | **64.0%** | 36.0% |
| **194** | Unexpected Sign Extension | 1,344 | 18,260 | 12,440 | **59.5%** | 40.5% |
| **510** | Trapdoor | 70 | 1,450 | 1,037 | **58.3%** | 41.7% |
| **195** | Signed to Unsigned Conversion Error | 1,344 | 16,087 | 11,865 | **57.6%** | 42.4% |
| **90** | LDAP Injection | 560 | 12,600 | 10,252 | **55.1%** | 44.9% |
| **464** | Addition of Data Structure Sentinel | 56 | 334 | 280 | **54.4%** | 45.6% |
| **526** | Info Exposure via Environment Variables | 18 | 69 | 58 | **54.3%** | 45.7% |
| **587** | Assignment of Fixed Address to Pointer | 18 | 36 | 31 | **53.7%** | 46.3% |
| **680** | Integer Overflow to Buffer Overflow | 336 | 5,381 | 4,715 | **53.3%** | 46.7% |
| **188** | Reliance on Data/Memory Layout | 36 | 286 | 275 | **51.0%** | 49.0% |
| **843** | Type Confusion | 100 | 279 | 340 | 45.1% | 54.9% |
| **481** | Assigning Instead of Comparing | 18 | 195 | 239 | 44.9% | 55.1% |
| **480** | Use of Incorrect Operator | 18 | 79 | 97 | 44.9% | 55.1% |
| **785** | Path Manipulation Without Max-Size Buffer | 18 | 232 | 296 | 43.9% | 56.1% |
| **588** | Access Child of Non-Structure Pointer | 50 | 208 | 267 | 43.8% | 56.2% |
| **690** | NULL Deref from Return | 1,120 | 8,909 | 11,476 | 43.7% | 56.3% |
| **127** | Buffer Underread | 1,896 | 19,692 | 25,419 | 43.7% | 56.3% |
| **620** | Unverified Password Change | 18 | 192 | 248 | 43.6% | 56.4% |
| **124** | Buffer Underwrite | 1,896 | 19,121 | 24,985 | 43.4% | 56.6% |
| **121** | Stack-Based Buffer Overflow | 5,906 | 50,353 | 66,007 | 43.3% | 56.7% |
| **835** | Infinite Loop | 6 | 30 | 40 | 42.9% | 57.1% |
| **426** | Untrusted Search Path | 224 | 1,184 | 1,576 | 42.9% | 57.1% |
| **535** | Info Exposure via Shell Error | 36 | 569 | 763 | 42.7% | 57.3% |
| **404** | Improper Resource Shutdown | 448 | 1,845 | 2,485 | 42.6% | 57.4% |
| **571** | Expression Always True | 16 | 94 | 129 | 42.2% | 57.8% |
| **482** | Comparing Instead of Assigning | 18 | 73 | 101 | 42.0% | 58.0% |
| **475** | Undefined Behavior for Input to API | 36 | 274 | 379 | 42.0% | 58.0% |
| **126** | Buffer Overread | 1,380 | 14,456 | 20,169 | 41.8% | 58.2% |
| **367** | TOC/TOU Race Condition | 36 | 769 | 1,077 | 41.7% | 58.3% |
| **122** | Heap-Based Buffer Overflow | 3,656 | 42,202 | 58,891 | 41.7% | 58.3% |
| **761** | Free Pointer Not at Start of Buffer | 672 | 11,943 | 16,733 | 41.6% | 58.4% |
| **665** | Improper Initialization | 224 | 1,437 | 2,026 | 41.5% | 58.5% |
| **546** | Suspicious Comment | 90 | 234 | 336 | 41.1% | 58.9% |
| **469** | Pointer Subtraction to Determine Size | 36 | 227 | 327 | 41.0% | 59.0% |
| **511** | Logic/Time Bomb | 72 | 700 | 1,028 | 40.5% | 59.5% |
| **222** | Truncation of Security-Relevant Info | 18 | 862 | 1,271 | 40.4% | 59.6% |
| **483** | Incorrect Block Delimitation | 20 | 163 | 241 | 40.3% | 59.7% |
| **570** | Expression Always False | 16 | 57 | 85 | 40.1% | 59.9% |
| **242** | Use of Inherently Dangerous Function | 18 | 176 | 265 | 39.9% | 60.1% |
| **773** | Missing Reference to Active File Descriptor | 168 | 1,060 | 1,623 | 39.5% | 60.5% |
| **681** | Incorrect Numeric Type Conversion | 54 | 326 | 506 | 39.2% | 60.8% |
| **284** | Improper Access Control | 216 | 1,258 | 1,964 | 39.0% | 61.0% |
| **479** | Signal Handler Use of Non-Reentrant Function | 18 | 150 | 237 | 38.8% | 61.2% |
| **832** | Unlock of Resource Not Locked | 18 | 215 | 341 | 38.7% | 61.3% |
| **484** | Omitted Break Statement in Switch | 18 | 104 | 165 | 38.7% | 61.3% |
| **591** | Sensitive Data in Improperly Locked Memory | 112 | 1,536 | 2,451 | 38.5% | 61.5% |
| **272** | Least Privilege Violation | 252 | 1,825 | 2,914 | 38.5% | 61.5% |
| **775** | Missing Release of File Descriptor | 168 | 615 | 985 | 38.4% | 61.6% |
| **377** | Insecure Temporary File | 144 | 1,333 | 2,136 | 38.4% | 61.6% |
| **688** | Function Call with Incorrect Argument | 18 | 70 | 113 | 38.3% | 61.7% |
| **534** | Info Exposure via Debug Log | 36 | 570 | 918 | 38.3% | 61.7% |
| **398** | Poor Code Quality | 181 | 789 | 1,282 | 38.1% | 61.9% |
| **253** | Incorrect Check of Function Return Value | 684 | 2,868 | 4,652 | 38.1% | 61.9% |
| **666** | Operation on Resource in Wrong Phase | 90 | 2,455 | 4,014 | 38.0% | 62.0% |
| **196** | Unsigned to Signed Conversion Error | 18 | 195 | 320 | 37.9% | 62.1% |
| **467** | Use of sizeof() on Pointer Type | 54 | 528 | 880 | 37.5% | 62.5% |
| **468** | Incorrect Pointer Scaling | 36 | 168 | 285 | 37.1% | 62.9% |
| **244** | Heap Inspection | 72 | 1,793 | 3,034 | 37.1% | 62.9% |
| **615** | Info Exposure by Comment | 18 | 102 | 174 | 37.0% | 63.0% |
| **478** | Missing Default Case in Switch | 18 | 64 | 110 | 36.8% | 63.2% |
| **327** | Use of Broken Crypto | 54 | 1,654 | 2,848 | 36.7% | 63.3% |
| **273** | Improper Check for Dropped Privileges | 36 | 459 | 790 | 36.7% | 63.3% |
| **134** | Uncontrolled Format String | 3,360 | 52,276 | 90,251 | 36.7% | 63.3% |
| **223** | Omission of Security-Relevant Info | 18 | 540 | 940 | 36.5% | 63.5% |
| **369** | Divide by Zero | 1,008 | 9,835 | 17,190 | 36.4% | 63.6% |
| **325** | Missing Required Cryptographic Step | 72 | 760 | 1,334 | 36.3% | 63.7% |
| **328** | Reversible One-Way Hash | 54 | 2,343 | 4,155 | 36.1% | 63.9% |
| **606** | Unchecked Loop Condition | 560 | 8,910 | 16,050 | 35.7% | 64.3% |
| **605** | Multiple Binds to Same Port | 18 | 257 | 462 | 35.7% | 64.3% |
| **252** | Unchecked Return Value | 630 | 2,533 | 4,554 | 35.7% | 64.3% |
| **459** | Incomplete Cleanup | 36 | 235 | 425 | 35.6% | 64.4% |
| **780** | RSA Without OAEP | 18 | 457 | 829 | 35.5% | 64.5% |
| **366** | Race Condition Within Thread | 36 | 324 | 599 | 35.1% | 64.9% |
| **321** | Hard-Coded Cryptographic Key | 112 | 2,783 | 5,148 | 35.1% | 64.9% |
| **667** | Improper Locking | 18 | 122 | 233 | 34.4% | 65.6% |
| **390** | Error Without Action | 72 | 381 | 732 | 34.2% | 65.8% |
| **590** | Free Memory Not on Heap | 900 | 6,187 | 12,033 | 34.0% | 66.0% |
| **675** | Duplicate Operations on Resource | 224 | 1,277 | 2,494 | 33.9% | 66.1% |
| **400** | Resource Exhaustion | 840 | 9,372 | 18,266 | 33.9% | 66.1% |
| **226** | Sensitive Info Uncleared Before Release | 72 | 1,145 | 2,256 | 33.7% | 66.3% |
| **685** | Function Call with Incorrect Argument Count | 18 | 46 | 91 | 33.6% | 66.4% |
| **415** | Double Free | 336 | 2,593 | 5,178 | 33.4% | 66.6% |
| **758** | Undefined Behavior | 365 | 2,848 | 5,726 | 33.2% | 66.8% |
| **391** | Unchecked Error Condition | 54 | 343 | 689 | 33.2% | 66.8% |
| **476** | NULL Pointer Dereference | 372 | 1,222 | 2,475 | 33.1% | 66.9% |
| **247** | Reliance on DNS Lookups | 18 | 458 | 942 | 32.7% | 67.3% |
| **191** | Integer Underflow | 3,864 | 19,849 | 40,831 | 32.7% | 67.3% |
| **190** | Integer Overflow | 5,040 | 26,103 | 54,636 | 32.3% | 67.7% |
| **401** | Memory Leak | 1,228 | 10,976 | 23,198 | 32.1% | 67.9% |
| **259** | Hard-Coded Password | 112 | 802 | 1,718 | 31.8% | 68.2% |
| **789** | Uncontrolled Memory Allocation | 560 | 8,498 | 18,367 | 31.6% | 68.4% |
| **364** | Signal Handler Race Condition | 18 | 239 | 535 | 30.9% | 69.1% |
| **319** | Cleartext Transmission of Sensitive Info | 224 | 4,787 | 11,112 | 30.1% | 69.9% |
| **176** | Improper Unicode Encoding Handling | 56 | 246 | 585 | 29.6% | 70.4% |
| **563** | Unused Variable | 366 | 983 | 2,471 | 28.5% | 71.5% |
| **416** | Use After Free | 150 | 1,787 | 4,698 | 27.6% | 72.4% |
| **338** | Weak PRNG | 18 | 63 | 200 | 24.0% | 76.0% |
| **256** | Plaintext Storage of Password | 112 | 1,539 | 8,604 | 15.2% | 84.8% |
| **457** | Use of Uninitialized Variable | 616 | 5,045 | 36,338 | 12.2% | 87.8% |
| | **TOTALS (106 categories)** | **54,484** | **552,645** | **752,422** | **42.3%** | **57.7%** |

### Categories with No C Test Data (12)

CWE-23, CWE-36, CWE-396, CWE-397, CWE-440, CWE-500, CWE-561, CWE-562, CWE-672, CWE-674, CWE-676, CWE-762

These categories either have no C test files in the Juliet suite (Java/C++ only) or the test files produced zero violations.

---

## Performance Distribution

```
TP Rate Distribution (106 categories with data):

  80-90%  |#                                                          (1)
  70-80%  |###                                                        (3)
  60-70%  |####                                                       (4)
  50-60%  |#########                                                  (9)
  40-50%  |#######################                                    (23)
  35-40%  |#################################                          (33)
  30-35%  |##########################                                 (26)
  25-30%  |####                                                       (4)
  < 25%   |###                                                        (3)
           +---------+---------+---------+---------+---------+--------+
           0         5        10        15        20        25       30+

Weighted average TP rate: 42.3% (by violation count)
```

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
| 510 | Trapdoor | 58.3% | 70 |
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
| 457 | Uninitialized Variable | 12.2% | DCL02-C causes massive FP inflation (36K FPs) |

---

## Cross-Category Analysis

### Dominant Rules Across All Categories

**Top True Positive Contributors:**
1. **DCL31-C** (Declare identifiers before using) - appears in every category
2. **DCL07-C** (Include type info in declarations) - appears in every category
3. **FLP34-C** (Floating-point conversion range) - most categories
4. **INT32-C** (Integer overflow protection) - integer/conversion categories
5. **INT30-C** (Unsigned integer wrapping) - integer/conversion categories
6. **DCL06-C** (Meaningful symbolic constants) - buffer overflow categories

**Top False Positive Contributors:**
1. **DCL31-C / DCL07-C** - High volume in both TP and FP (structural noise)
2. **DCL02-C** - Devastating in CWE-457 (36K FPs), moderate elsewhere
3. **FLP34-C** - High volume across many categories

### FLAW Line Detection

Detection of exact vulnerability locations (marked with `/* FLAW: */` comments) is 0% for 105 of 106 categories. The sole exception is **CWE-758 (Undefined Behavior)** with 10.7% FLAW line detection (39/365 lines).

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
- **Parallelism**: 12 concurrent sqc processes on 24-core machine
- **Total wall-clock time**: ~25 minutes (parallel)

### Limitations
1. SqC applies all CERT C rules to every file - most rules are not relevant to the specific CWE
2. OMITBAD sections contain both vulnerable code AND supporting code
3. FLAW line detection is ~0% because SqC reports on code lines, not adjacent comment lines
4. TP/FP classification is at the violation level, not file level
5. The OMITBAD/OMITGOOD ratio varies significantly across categories
6. 12 categories had no usable C test data in Juliet

---

## Conclusions

1. **Full-suite benchmark complete**: 54,484 files / 4.0M violations across all 118 Juliet CWE categories
2. **42.3% weighted TP rate** across 106 categories with data
3. **17 categories exceed 50% TP** - strongest for type conversion (CWE-194/195/197), command injection (CWE-78), and search path issues (CWE-427)
4. **60 categories (57%) in the 35-50% range** - the bulk of detection comes from generic CERT C coding standard rules
5. **3 categories below 25%** - CWE-457 (uninit var), CWE-256 (password storage), CWE-338 (weak PRNG)
6. **10.4% FP reduction** achieved through targeted rule fixes (INT08-C, CON08-C, DCL20-C, ARR38-C)

### Opportunities for Further Improvement

**High impact:**
- Reduce DCL02-C false positives (especially in CWE-457 where it contributes 36K FPs)
- Add CWE-specific rule weighting/filtering when scanning for a known vulnerability type
- Implement data-flow analysis rules for CWE-416, CWE-401, CWE-476

**Medium impact:**
- Add rules for CWE-338 (PRNG quality), CWE-256 (credential storage)
- Add format string rules (FIO30-C) for CWE-134
- Add command injection rules (ENV33-C) for CWE-78

**Analysis improvement:**
- Normalize TP rates by OMITBAD/OMITGOOD ratio for fairer cross-category comparison
- Implement file-level (not violation-level) TP/FP analysis

---

## Files

### Results
```
/tmp/juliet_results/
├── CWE{id}_{name}.csv              - Raw SqC CSV output per CWE (118 files)
├── CWE{id}_{name}_analysis.txt     - Ground truth analysis per CWE (118 files)
├── detailed_stats.csv               - Parsed stats for all categories
└── multi_cwe_summary.txt            - TP/FP rates summary
```

### Scripts
```
scripts/analyze_juliet_results.py    - Ground truth analysis (any CWE)
scripts/run_juliet_multi_cwe.sh      - Batch multi-CWE runner (sequential)
scripts/run_juliet_parallel.sh       - Parallel multi-CWE runner (12 jobs)
```

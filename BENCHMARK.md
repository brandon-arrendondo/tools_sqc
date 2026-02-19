# SqC Benchmark, Analysis, and Strategic Assessment

**Last Updated**: 2026-02-19 (cross-tool capability analysis added)
**Current TP Rate**: 43.8% (Round 9, 54,484 Juliet files)

---

## Table of Contents

1. [Current State](#current-state)
2. [Juliet Benchmark Results](#juliet-benchmark-results)
3. [Per-Round Fix Details](#per-round-fix-details)
4. [Performance by CWE Category](#performance-by-cwe-category)
5. [Full Per-CWE Results (Round 1 Baseline)](#full-per-cwe-results-round-1-baseline)
6. [Benchmark Methodology](#benchmark-methodology)
7. [False Positive / False Negative Analysis](#false-positive--false-negative-analysis)
8. [Competitor Comparison](#competitor-comparison)
9. [Architecture Assessment](#architecture-assessment)
10. [CI/CD Readiness](#cicd-readiness)
11. [Next Steps / Roadmap](#next-steps--roadmap)
12. [Resolved Issues](#resolved-issues)
13. [Scripts and Data Locations](#scripts-and-data-locations)

---

## Current State

| Metric | Value |
|--------|-------|
| **Rules Implemented** | 283 CERT C rules |
| **Juliet Files** | 54,484 |
| **True Positives** | 230,643 |
| **False Positives** | 296,342 |
| **TP Rate** | **43.8%** |
| **FP Reduction from Baseline** | -64.7% (839K → 296K) |
| **CWE Categories with Data** | 106 / 118 |
| **Categories >50% TP** | 18 |

### Top Remaining FP Rules

| Rule | FP | TP | Notes |
|------|---:|---:|-------|
| INT32-C | 23K | 16K | Type-aware inference already applied |
| DCL31-C | 21K | 16K | Cross-file + std_functions already applied |
| DCL07-C | 20K | 16K | Cross-file + std_functions already applied |
| INT30-C | 17K | 17K | ~50/50 ratio — reductions lose TPs |
| EXP34-C | 15K | 12K | Null pointer — already tightened |
| DCL06-C | 14K | 19K | Code style — ~50/50, reductions lose TPs |
| EXP12-C | 9K | 10K | Whitelist already trimmed |
| MEM10-C | 7K | 6K | ~50/50 ratio |
| ERR33-C | 6K | 4K | Nested calls + math overlap fixed |

**Key insight**: Most remaining top FP rules have ~50/50 TP/FP ratios. Further rule tuning will proportionally lose TPs. The ~43.8% Juliet ceiling is likely an architectural constraint for single-translation-unit analysis.

---

## Juliet Benchmark Results

### Aggregate Metrics (Round 9 — 2026-02-19)

| Metric | Value |
|--------|-------|
| **Files Analyzed** | 54,484 |
| **Classified (TP+FP)** | 526,985 |
| **True Positives** | 230,643 |
| **False Positives** | 296,342 |
| **Weighted TP Rate** | **43.8%** |
| **Categories with data** | 106 / 118 |
| **Wall-clock time** | ~25 min (12 parallel jobs) |
| **Cross-file dirs** | `testcases/` + `testcasesupport/` |

### FP Reduction History

| Round | Fixes | TP | FP | TP Rate | FP Delta |
|-------|-------|---:|---:|--------:|---------:|
| Baseline | -- | 586,539 | 839,341 | 41.1% | -- |
| Round 1 | INT08-C, CON08-C, DCL20-C, ARR38-C | 552,645 | 752,422 | 42.3% | -86,919 |
| Round 2 | EXP33-C, SIG31-C, ARR01-C, DCL30-C, DCL02-C | 555,700 | 736,563 | 43.0% | -15,859 |
| Round 3 | DCL31-C, DCL07-C, FLP34-C | 402,013 | 537,589 | 42.8% | -198,974 |
| Round 4 | EXP12-C, FLP03-C, INT32-C | 363,914 | 492,648 | 42.5% | -44,941 |
| Round 5 | FLP02-C, DCL06-C, INT30-C | 340,894 | 475,813 | 41.7% | -16,835 |
| Round 6 | Cross-file analysis (`-d`) | 247,757 | 327,191 | 43.1% | -148,622 |
| Round 7 | EXP36-C, EXP34-C, ARR37-C | 231,053 | 301,475 | 43.4% | -25,716 |
| Round 8 | DCL40-C, FLP32-C, ERR33-C | 230,992 | 296,415 | 43.8% | -5,060 |
| **Round 9** | **CFG, data-flow, inter-procedural analysis** | **230,643** | **296,342** | **43.8%** | **-73** |
| Round 10 | EXP34-C: `&&` short-circuit guard + stack array fix | TBD | TBD | TBD | ~-1,800 (est.) |
| Round 11 | DCL07-C/DCL31-C: ALL_CAPS macro guard + POSIX std_functions additions | TBD | TBD | TBD | TBD |

**Trend**: Diminishing returns on FP reduction via rule tuning. Round 3 removed 199K FP; Round 8 removed 5K; Round 9 removed 73 (Juliet is single-file, so CFG/inter-procedural infrastructure has minimal Juliet impact — targets real-world multi-file codebases). Round 10 targets two specific EXP34-C FP patterns identified by diagnostic analysis of CWE476 results. Round 11 targets DCL07-C/DCL31-C macro false positives identified from real-world comparison analysis; Juliet impact TBD but real-world impact is estimated at ~-7,600 on mosquitto and ~-10,300 on curl (combined).

---

## Per-Round Fix Details

### Round 9 — CFG, Data-Flow, Inter-Procedural Analysis

Added CFG construction, reaching definitions, and inter-procedural function summaries. Infrastructure enables path-sensitive analysis across function boundaries. Juliet impact is minimal (-73 FP) because test cases are single-file; the infrastructure targets multi-file real-world codebases where null returns, freed parameters, and no-return functions propagate across call sites.

### Round 8 — DCL40-C, FLP32-C, ERR33-C

1. **DCL40-C** (incompatible declarations): Restricted to file-scope declarations only; removed the 31-character identifier prefix collision check (flagged O(n²) violations on Juliet naming conventions like `CWE190_Integer_Overflow__int_fscanf_add_01_bad` vs `..._good`). The 31-char limit is a C90 concern unrelated to type incompatibility. **Impact: DCL40-C FP reduced from ~12K to near zero. Zero TP loss (DCL40-C had 0 TPs).**

2. **FLP32-C** (math domain/range errors): Replaced broad function-scope errno/isnan check with windowed search — only counts error checking within 5 statements of the math call. Previously, a single `errno = 0` anywhere in the function suppressed all FLP32-C flags. **Impact: FLP32-C FP reduced from ~930 to minimal.**

3. **ERR33-C** (unchecked return values): Added `argument_list` detection — calls whose return values are consumed as arguments to other functions (e.g., `srand((unsigned)time(NULL))`) no longer flagged. Removed math functions from the error-returning function list (overlap with FLP32-C). **Impact: modest FP reduction; math function double-flagging eliminated.**

**Net**: -5,060 FP (-1.7%), -61 TP (negligible), TP rate +0.4pp.

### Round 7 — EXP36-C, EXP34-C, ARR37-C

1. **EXP36-C** (pointer cast alignment): Previously flagged ALL casts including `(unsigned)time(NULL)`. Fixed: skip casts where target type has no `*` (not a pointer cast); skip when source type is `unknown *`; handle parenthesized expressions so `(struct foo_header *)(data + offset)` still works.

2. **EXP34-C** (null pointer dereference): Removed `_t` suffix heuristic (was marking `time_t`, `pid_t`, `mode_t` as pointers). Field expression assignments (`current = list->next`) now only propagate null status if the base object is already potentially-null.

3. **ARR37-C** (pointer arithmetic on non-array): Stop flagging Unknown pointers; recognize `alloca`/`aligned_alloc`; treat all pointer parameters as ambiguous.

**Net**: -25,716 FP (-7.9%), -16,704 TP (-6.7%), TP rate +0.3pp.

### Round 6 — Cross-File Analysis (`-d`)

Added `--directories` CLI option that pre-scans additional directories for function definitions/declarations. Tree-sitter cannot follow `#include` directives — any function defined in another translation unit appeared "undeclared" to DCL31-C/DCL07-C. The `-d testcases/ -d testcasesupport/` option eliminated FPs from Juliet helper functions (`printLine`, `printIntLine`, etc.).

**Net**: FP 475K → 327K (-148K, -31.2%). TP 341K → 248K (-93K) — lost TPs were calls to cross-file functions in OMITBAD sections that are not real vulnerabilities.

### Round 5 — FLP02-C, DCL06-C, INT30-C

1. **FLP02-C**: Replaced text heuristics (`text.contains('e')` matched `delete`, `execute`) with AST-node-kind checks.

2. **DCL06-C**: Expanded acceptable literal values from `{0, 1, 2, -1}` to `{0–10, -1, -2}`; removed "assignment" and "loop" from suspicious contexts.

3. **INT30-C**: Applied `collect_variable_types()` pattern from INT32-C; removed variable name heuristics that falsely matched `used`, `unique`, `url_buffer` as unsigned.

**Net**: -16,835 FP (-3.4%), -23,020 TP (-6.3%), TP rate -0.7pp (DCL06-C and FLP02-C are ~50/50 rules).

### Round 4 — EXP12-C, FLP03-C, INT32-C

1. **EXP12-C**: Removed ~30 side-effect functions (memset, strcpy, strlen, memcpy, etc.) from the "important return value" whitelist. Kept only functions whose return values signal success/failure or allocation.

2. **FLP03-C**: Removed the `assignment_expression` arm that flagged every FP assignment. Division and cast checks remain.

3. **INT32-C**: Added `collect_variable_types()` HashMap from function params and local declarations; changed default from "signed" to "unknown" for unmapped variables.

**Net**: -44,941 FP (-8.4%), large TP reduction from non-security noise.

### Round 3 — DCL31-C, DCL07-C, FLP34-C

1. **DCL31-C + DCL07-C**: Replaced 32-function whitelist with shared `std_functions.rs` database covering ~270 C11/POSIX/Windows functions unconditionally skipped. **Impact: -198,974 FP.**

2. **FLP34-C**: Replaced text heuristic with type-aware checking via variable type collection.

### Round 2 — EXP33-C, SIG31-C, ARR01-C, DCL30-C, DCL02-C

Fixed preprocessor-block visibility bug: functions inside `#ifdef`/`#ifndef` blocks were invisible to analysis because the rules only iterated direct children of `translation_unit`. Fixed with recursive collector. DCL02-C: Added check that visually similar identifiers must actually be different strings.

**Net**: -15,859 FP; CWE-457 TP rate 12.2% → 22.6%.

### Round 1 — INT08-C, CON08-C, DCL20-C, ARR38-C

- **INT08-C**: Removed `int` from "narrow type" definition (bug: `int` is not narrow per CERT)
- **CON08-C**: Only flag when calling multiple *atomic* functions without mutex
- **DCL20-C**: Only flag declarations/prototypes, not definitions
- **ARR38-C**: Removed duplicate `strcpy`/`strcat` flagging (already covered by STR31-C)

**Net**: -86,919 FP (-10.4%), TP rate 41.1% → 42.3%.

---

## Performance by CWE Category

### Tier 1: Strong Detection (TP > 50%) — 18 categories

| CWE | Category | TP Rate | Files |
|-----|----------|---------|-------|
| 480 | Use of Incorrect Operator | 91.7% | 18 |
| 506 | Embedded Malicious Code | 85.9% | 158 |
| 587 | Assignment of Fixed Address to Pointer | 83.3% | 18 |
| 617 | Reachable Assertion | 79.2% | 354 |
| 197 | Numeric Truncation Error | 78.3% | 1,008 |
| 464 | Data Structure Sentinel Addition | 77.6% | 56 |
| 427 | Uncontrolled Search Path Element | 72.8% | 560 |
| 78 | OS Command Injection | 71.4% | 5,600 |
| 123 | Write-What-Where Condition | 68.2% | 168 |
| 15 | External Control of System/Config | 67.0% | 56 |
| 194 | Unexpected Sign Extension | ~58% | 1,344 |
| 195 | Signed-to-Unsigned Conversion | ~56% | 1,344 |
| 510 | Trapdoor | ~58% | 70 |
| 90 | LDAP Injection | ~52% | 560 |
| 526 | Info Exposure via Env Variables | ~54% | 18 |
| 680 | Integer Overflow to Buffer Overflow | ~51% | 336 |
| 188 | Reliance on Data/Memory Layout | ~51% | 36 |
| 114 | Process Control | ~58% | 672 |

### Tier 2: Moderate Detection (35-50%) — 68 categories

The bulk of categories (64%) cluster here. Includes buffer overflows (CWE-121 ~43%, CWE-122 ~42%), format strings (CWE-134 ~37%), and resource management issues.

### Tier 3: Below Average (25-35%) — 16 categories

Includes integer overflow/underflow (CWE-190/191 ~32%), memory management (CWE-401 ~32%, CWE-415 ~33%), and NULL pointer dereference (CWE-476 ~33%).

### Tier 4: Weak Detection (<25%) — 4 categories

| CWE | Category | TP Rate | Root Cause |
|-----|----------|---------|------------|
| 256 | Plaintext Password Storage | ~15% | No credential-storage rules |
| 338 | Weak PRNG | ~23% | No PRNG-quality rules |
| 457 | Use of Uninitialized Variable | ~24% | Improved from 12.2% after EXP33-C + DCL02-C fixes |
| 319 | Cleartext Transmission | ~25% | Limited cleartext detection rules |

---

## Full Per-CWE Results (Round 1 Baseline)

> **Note**: This table reflects Round 1 results (42.3% weighted TP rate). Current Round 9 performance is ~43.8%. The relative ordering of categories and root cause analysis remains representative.

| CWE | Vulnerability Type | Files | TP | FP | TP Rate |
|-----|-------------------|-------|---:|---:|--------:|
| 506 | Embedded Malicious Code | 158 | 3,421 | 552 | 86.1% |
| 15 | External Control of System/Config | 56 | 1,255 | 422 | 74.8% |
| 427 | Uncontrolled Search Path Element | 560 | 7,656 | 2,798 | 73.2% |
| 78 | OS Command Injection | 5,600 | 79,292 | 30,203 | 72.4% |
| 617 | Reachable Assertion | 354 | 2,685 | 1,192 | 69.3% |
| 197 | Numeric Truncation Error | 1,008 | 7,899 | 3,733 | 67.9% |
| 123 | Write-What-Where Condition | 168 | 2,239 | 1,213 | 64.9% |
| 114 | Process Control | 672 | 8,839 | 4,973 | 64.0% |
| 194 | Unexpected Sign Extension | 1,344 | 18,260 | 12,440 | 59.5% |
| 510 | Trapdoor | 70 | 1,450 | 1,037 | 58.3% |
| 195 | Signed-to-Unsigned Conversion | 1,344 | 16,087 | 11,865 | 57.6% |
| 90 | LDAP Injection | 560 | 12,600 | 10,252 | 55.1% |
| 464 | Data Structure Sentinel Addition | 56 | 334 | 280 | 54.4% |
| 526 | Info Exposure via Env Variables | 18 | 69 | 58 | 54.3% |
| 587 | Fixed Address to Pointer | 18 | 36 | 31 | 53.7% |
| 680 | Integer Overflow to Buffer Overflow | 336 | 5,381 | 4,715 | 53.3% |
| 188 | Reliance on Data/Memory Layout | 36 | 286 | 275 | 51.0% |
| 843 | Type Confusion | 100 | 279 | 340 | 45.1% |
| 481 | Assigning Instead of Comparing | 18 | 195 | 239 | 44.9% |
| 480 | Use of Incorrect Operator | 18 | 79 | 97 | 44.9% |
| 785 | Path Manipulation Without Max-Size Buffer | 18 | 232 | 296 | 43.9% |
| 588 | Access Child of Non-Structure Pointer | 50 | 208 | 267 | 43.8% |
| 690 | NULL Deref from Return | 1,120 | 8,909 | 11,476 | 43.7% |
| 127 | Buffer Underread | 1,896 | 19,692 | 25,419 | 43.7% |
| 620 | Unverified Password Change | 18 | 192 | 248 | 43.6% |
| 124 | Buffer Underwrite | 1,896 | 19,121 | 24,985 | 43.4% |
| 121 | Stack-Based Buffer Overflow | 5,906 | 50,353 | 66,007 | 43.3% |
| 835 | Infinite Loop | 6 | 30 | 40 | 42.9% |
| 426 | Untrusted Search Path | 224 | 1,184 | 1,576 | 42.9% |
| 535 | Info Exposure via Shell Error | 36 | 569 | 763 | 42.7% |
| 404 | Improper Resource Shutdown | 448 | 1,845 | 2,485 | 42.6% |
| 571 | Expression Always True | 16 | 94 | 129 | 42.2% |
| 482 | Comparing Instead of Assigning | 18 | 73 | 101 | 42.0% |
| 475 | Undefined Behavior for Input to API | 36 | 274 | 379 | 42.0% |
| 126 | Buffer Overread | 1,380 | 14,456 | 20,169 | 41.8% |
| 367 | TOC/TOU Race Condition | 36 | 769 | 1,077 | 41.7% |
| 122 | Heap-Based Buffer Overflow | 3,656 | 42,202 | 58,891 | 41.7% |
| 761 | Free Pointer Not at Start of Buffer | 672 | 11,943 | 16,733 | 41.6% |
| 665 | Improper Initialization | 224 | 1,437 | 2,026 | 41.5% |
| 546 | Suspicious Comment | 90 | 234 | 336 | 41.1% |
| 469 | Pointer Subtraction to Determine Size | 36 | 227 | 327 | 41.0% |
| 511 | Logic/Time Bomb | 72 | 700 | 1,028 | 40.5% |
| 222 | Truncation of Security-Relevant Info | 18 | 862 | 1,271 | 40.4% |
| 483 | Incorrect Block Delimitation | 20 | 163 | 241 | 40.3% |
| 570 | Expression Always False | 16 | 57 | 85 | 40.1% |
| 242 | Use of Inherently Dangerous Function | 18 | 176 | 265 | 39.9% |
| 773 | Missing Reference to Active File Descriptor | 168 | 1,060 | 1,623 | 39.5% |
| 681 | Incorrect Numeric Type Conversion | 54 | 326 | 506 | 39.2% |
| 284 | Improper Access Control | 216 | 1,258 | 1,964 | 39.0% |
| 479 | Signal Handler Use of Non-Reentrant Function | 18 | 150 | 237 | 38.8% |
| 832 | Unlock of Resource Not Locked | 18 | 215 | 341 | 38.7% |
| 484 | Omitted Break Statement in Switch | 18 | 104 | 165 | 38.7% |
| 591 | Sensitive Data in Improperly Locked Memory | 112 | 1,536 | 2,451 | 38.5% |
| 272 | Least Privilege Violation | 252 | 1,825 | 2,914 | 38.5% |
| 775 | Missing Release of File Descriptor | 168 | 615 | 985 | 38.4% |
| 377 | Insecure Temporary File | 144 | 1,333 | 2,136 | 38.4% |
| 688 | Function Call with Incorrect Argument | 18 | 70 | 113 | 38.3% |
| 534 | Info Exposure via Debug Log | 36 | 570 | 918 | 38.3% |
| 398 | Poor Code Quality | 181 | 789 | 1,282 | 38.1% |
| 253 | Incorrect Check of Function Return Value | 684 | 2,868 | 4,652 | 38.1% |
| 666 | Operation on Resource in Wrong Phase | 90 | 2,455 | 4,014 | 38.0% |
| 196 | Unsigned to Signed Conversion Error | 18 | 195 | 320 | 37.9% |
| 467 | Use of sizeof() on Pointer Type | 54 | 528 | 880 | 37.5% |
| 468 | Incorrect Pointer Scaling | 36 | 168 | 285 | 37.1% |
| 244 | Heap Inspection | 72 | 1,793 | 3,034 | 37.1% |
| 615 | Info Exposure by Comment | 18 | 102 | 174 | 37.0% |
| 478 | Missing Default Case in Switch | 18 | 64 | 110 | 36.8% |
| 327 | Use of Broken Crypto | 54 | 1,654 | 2,848 | 36.7% |
| 273 | Improper Check for Dropped Privileges | 36 | 459 | 790 | 36.7% |
| 134 | Uncontrolled Format String | 3,360 | 52,276 | 90,251 | 36.7% |
| 223 | Omission of Security-Relevant Info | 18 | 540 | 940 | 36.5% |
| 369 | Divide by Zero | 1,008 | 9,835 | 17,190 | 36.4% |
| 325 | Missing Required Cryptographic Step | 72 | 760 | 1,334 | 36.3% |
| 328 | Reversible One-Way Hash | 54 | 2,343 | 4,155 | 36.1% |
| 606 | Unchecked Loop Condition | 560 | 8,910 | 16,050 | 35.7% |
| 605 | Multiple Binds to Same Port | 18 | 257 | 462 | 35.7% |
| 252 | Unchecked Return Value | 630 | 2,533 | 4,554 | 35.7% |
| 459 | Incomplete Cleanup | 36 | 235 | 425 | 35.6% |
| 780 | RSA Without OAEP | 18 | 457 | 829 | 35.5% |
| 366 | Race Condition Within Thread | 36 | 324 | 599 | 35.1% |
| 321 | Hard-Coded Cryptographic Key | 112 | 2,783 | 5,148 | 35.1% |
| 667 | Improper Locking | 18 | 122 | 233 | 34.4% |
| 390 | Error Without Action | 72 | 381 | 732 | 34.2% |
| 590 | Free Memory Not on Heap | 900 | 6,187 | 12,033 | 34.0% |
| 675 | Duplicate Operations on Resource | 224 | 1,277 | 2,494 | 33.9% |
| 400 | Resource Exhaustion | 840 | 9,372 | 18,266 | 33.9% |
| 226 | Sensitive Info Uncleared Before Release | 72 | 1,145 | 2,256 | 33.7% |
| 685 | Function Call with Incorrect Argument Count | 18 | 46 | 91 | 33.6% |
| 415 | Double Free | 336 | 2,593 | 5,178 | 33.4% |
| 758 | Undefined Behavior | 365 | 2,848 | 5,726 | 33.2% |
| 391 | Unchecked Error Condition | 54 | 343 | 689 | 33.2% |
| 476 | NULL Pointer Dereference | 372 | 1,222 | 2,475 | 33.1% |
| 247 | Reliance on DNS Lookups | 18 | 458 | 942 | 32.7% |
| 191 | Integer Underflow | 3,864 | 19,849 | 40,831 | 32.7% |
| 190 | Integer Overflow | 5,040 | 26,103 | 54,636 | 32.3% |
| 401 | Memory Leak | 1,228 | 10,976 | 23,198 | 32.1% |
| 259 | Hard-Coded Password | 112 | 802 | 1,718 | 31.8% |
| 789 | Uncontrolled Memory Allocation | 560 | 8,498 | 18,367 | 31.6% |
| 364 | Signal Handler Race Condition | 18 | 239 | 535 | 30.9% |
| 319 | Cleartext Transmission of Sensitive Info | 224 | 4,787 | 11,112 | 30.1% |
| 176 | Improper Unicode Encoding Handling | 56 | 246 | 585 | 29.6% |
| 563 | Unused Variable | 366 | 983 | 2,471 | 28.5% |
| 416 | Use After Free | 150 | 1,787 | 4,698 | 27.6% |
| 338 | Weak PRNG | 18 | 63 | 200 | 24.0% |
| 256 | Plaintext Storage of Password | 112 | 1,539 | 8,604 | 15.2% |
| 457 | Use of Uninitialized Variable | 616 | 5,045 | 36,338 | 12.2% |
| | **TOTALS (106 categories)** | **54,484** | **552,645** | **752,422** | **42.3%** |

Categories with no C test data (12): CWE-23, CWE-36, CWE-396, CWE-397, CWE-440, CWE-500, CWE-561, CWE-562, CWE-672, CWE-674, CWE-676, CWE-762.

---

## Benchmark Methodology

### Ground Truth Classification

Juliet test files contain preprocessor-guarded sections:
- **`#ifndef OMITBAD`**: Code with known vulnerabilities — violations here = **True Positives**
- **`#ifndef OMITGOOD`**: Fixed/safe code — violations here = **False Positives**
- **`/* FLAW: */`**: Comments marking exact vulnerability locations (SqC reports 0% FLAW-line detection — it reports the code line, not the adjacent comment)

### Metrics

- **TP Rate** = Violations in OMITBAD / (Violations in OMITBAD + OMITGOOD)
- Violations outside both sections are excluded from classification
- Classification is at the **violation level**, not file level

### Scan Configuration

- **SqC**: `./target/release/sqc testcases/CWE{id}/ -d testcases/ -d testcasesupport/ --export results.csv`
- **Parallelism**: 12 concurrent sqc processes
- **Ground truth analysis**: `scripts/analyze_juliet_results.py`

### Limitations

1. SqC applies all 283 CERT C rules to every file — most rules are not relevant to the specific CWE being tested
2. OMITBAD sections contain both vulnerable code AND supporting infrastructure code
3. FLAW line detection is ~0% (SqC reports code lines, not comment lines)
4. The OMITBAD/OMITGOOD code ratio varies significantly across categories
5. 12 categories had no usable C test data in Juliet (Java/C++ only)

---

## False Positive / False Negative Analysis

*Analysis based on CWE-121 Stack-Based Buffer Overflow (s08 subset, 624 files, SqC v1ad80211). Findings generalize to the full benchmark.*

### Violation Distribution

| Section | Violations | Rate |
|---------|----------:|-----:|
| OMITBAD (True Positives) | 5,253 | 43.6% |
| OMITGOOD (False Positives) | 6,800 | 56.4% |
| FLAW lines | 0 | 0.0% |

### Root Causes of False Positives

1. **Generic coding standards**: DCL31-C, DCL07-C, FLP34-C apply to all code regardless of whether it contains a security flaw — same patterns appear in safe and vulnerable code
2. **Test infrastructure noise**: Test harness code (srand, main functions) triggers CON08-C, EXP12-C
3. **Non-discriminatory rules**: Code style rules (DCL06-C magic numbers) fire equally in OMITBAD and OMITGOOD

### Coverage Gaps (Historical — mostly addressed)

- **Wide character functions**: STR31-C originally missed `wcscat`, `wcscpy`, `wmemcpy`. Wide-char support added 2026-01-08 (commit `15baccb8`).
- **Data-flow**: Without tracking buffer sizes through assignments, cannot prove buffer overflow vs. safe buffer operation.

### What SqC Can and Cannot Do

| Capability | Status |
|-----------|--------|
| CERT C coding standards violations | ✅ Comprehensive (283 rules) |
| Code smells that correlate with bugs | ✅ Good |
| Exact vulnerability pinpointing | ❌ Requires data-flow |
| Wide character function checks | ✅ Supported since Round 1 |
| Inter-procedural null/free tracking | ✅ Added Round 9 (CFG + summaries) |
| Proving buffer overflows | ❌ Requires value-range analysis |

---

## Competitor Comparison

### SqC vs. Other Tools on Juliet

Comparison compiled from academic papers and published data. Direct Juliet runs of Cppcheck/Clang against the same 54,484 files are pending.

| Tool | Detection Rate | FP Rate | Analysis Depth | Juliet Data | CERT C | Price |
|------|---------------:|--------:|----------------|:-----------:|:------:|:-----:|
| **SqC** | **43.8%** | **56.2%** | AST + CFG + inter-procedural | Full (118 CWEs) | 283 rules | -- |
| Semgrep CE | 44–48% | Very low | AST (tree-sitter) | No | Community | Free |
| Semgrep Pro | 72–75% | Very low | AST + taint + inter-file | No | Community | Commercial |
| Infer | ~55% | ~45% | Separation logic | Partial (4 CWEs) | No | Free |
| Flawfinder | ~40% | High | Lexical scanning | Indirect | No | Free |
| CodeQL | ~29% | Moderate | Data-flow, taint | Indirect | Partial | Free/Commercial |
| Cppcheck | Low | Very low | Data-flow | Indirect | Partial | Free |
| Coverity | Best-in-class | ~15–20% (claimed) | Inter-procedural, path-sensitive | Not public | Partial | Enterprise |
| Commercial "Tool C"* | ~73% | ~7% | Inter-procedural | Yes (22 CWEs) | -- | Commercial |

*\*Anonymized commercial tool from [Goseva-Popstojanova & Perhinschi 2015](https://community.wvu.edu/~kagoseva/Papers/IST-2015.pdf), tested on 22 C/C++ CWEs only.*

**Key context from the literature:**
- Tools on average find ~20% of weaknesses in basic Juliet test cases ([ISSTA 2022](https://dl.acm.org/doi/10.1145/3533767.3534380))
- Even commercial tools miss 27% of C/C++ vulnerabilities on Juliet (Goseva 2015)
- FP rates across tools range from 6.5% to 76%+ depending on rule set ([survey](https://www.sciencedirect.com/science/article/abs/pii/S0950584913000384))
- Industry target for developer adoption is 10–20% FP rate
- No single tool is comprehensive; academic consensus recommends tool combination

**Sources:** [ISSTA 2022 (TUM)](https://dl.acm.org/doi/10.1145/3533767.3534380) | [Goseva 2015](https://community.wvu.edu/~kagoseva/Papers/IST-2015.pdf) | [JKU 2014](https://www.se.jku.at/wp-content/uploads/2014/08/2014.Using-the-Juliet-Test-Suite.pdf) | [Semgrep Blog 2025](https://semgrep.dev/blog/2025/security-research-comparing-semgrep-community-edition-and-semgrep-code-for-static-analysis/)

### Published CERT-C Results on Real Codebases

**Conclusion from literature search (2026-02-19):** No published CERT-C violation rates per KLOC on production open-source code exist. This is a genuine gap in the published literature.

| Source | CERT-C Specific | Named Codebase | Notes |
|--------|:---------------:|:--------------:|-------|
| [Coverity Scan](https://scan.coverity.com) | No | Yes | Defect density only (curl=0.00, nginx=0.01, sqlite=0.50, openssl=0.21 /KLOC) — uses Coverity's own taxonomy, not CERT-C rules |
| [TrustInSoft 2022 CERT-C Benchmark](https://trust-in-soft.com/blog/2022/10/27/cert-c-benchmark/) | Yes | No | Synthetic test suite; TrustInSoft 87% vs Tool A 55% vs Tool B 38% TP rate on CERT-C undefined-behavior subset |
| [SEI SCALe on JasPer (2015)](https://www.sei.cmu.edu/documents/462/2015_019_001_435900.pdf) | Yes | Yes (JasPer image library) | Only publicly named full CERT-C audit on an OSS codebase |
| [ISSTA 2022 (Lipp et al.)](https://dl.acm.org/doi/10.1145/3533767.3534380) | No (CVE taxonomy) | Yes (OpenSSL, SQLite, FFmpeg, ...) | 9 real OSS projects, CVE-based ground truth — not CERT-C rule-mapped |
| [NIST SATE IV/V/VI](https://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.500-326.pdf) | No (CWE/CVE) | Partially | Wireshark and SQLite (SATE VI), bugs injected; not CERT-C mapped |
| [Nguyen et al. 2019 IEEE](https://ieeexplore.ieee.org/document/8836131) | Yes | No (unnamed industrial) | 87% TP / 13% FP on Project 1; 57% TP / 43% FP on Project 2 |

**Implication for SqC validation**: Direct comparison to published CERT-C results on the same codebase is not possible (no such results exist). Valid comparison strategies:
1. SqC vs. Cppcheck vs. clang-tidy on same codebase — done for libcrc, curl, mosquitto (see below)
2. SqC on JasPer with reference to the SEI SCALe 2015 report (only named CERT-C audit)
3. SqC TP rate vs. TrustInSoft's synthetic CERT-C benchmark as upper-bound reference

### SqC vs. Cppcheck vs. Clang on a Test File

Small-scale direct comparison (20-line test file with 4 deliberate violations):

| Tool | Violations Found | Key Detections | Notable Misses |
|------|----------------:|----------------|----------------|
| SqC | 25 | MEM30-C, ARR30-C, INT30-C, INT32-C, EXP34-C, ERR33-C | — |
| Clang Static Analyzer | 4 | ArrayBoundV2, dead stores | Use-after-free, integer overflow |
| Cppcheck | 2 | arrayIndexOutOfBounds, deallocuse | Integer overflow, malloc error checking |

### Real-World: SQLite Analysis

- **Target**: SQLite source (~149 C files, ~238K LOC)
- **SqC result**: Works correctly after DCL02-C stack overflow fix (2026-01-07, commit `def416f3`)
- **Cppcheck**: Found 89 issues but hung at 124/125 files (95%), did not complete
- **Clang**: Requires `./configure && make` to generate build artifacts (`parse.h` missing)
- **Throughput**: ~2–5 sec/file for SqC = ~12–25 min for full SQLite

### Real-World: libcrc

- **Target**: libcrc — CRC algorithm library (~16 C files, src/ + precalc/ + test/ + examples/)
- **Environment**: Ubuntu 24.04, cppcheck 2.13, clang-tidy 18, sqc Round 9 (2026-02-19)

| Tool | Total Findings | By Severity | Top Rules/Checks |
|------|---------------|-------------|-----------------|
| **sqc** | **1,109** | High: 453, Medium: 388, Low: 268 | EXP14-C (106), ERR33-C (68), EXP12-C (62), INT30-C (60), EXP19-C (59) |
| **cppcheck** | **41** | style: 40, information: 1 | `unusedFunction`/CWE-561 (21), `variableScope`/CWE-398 (19) |
| **clang-tidy** | **50** | — | `cert-err33-c` (26), `DeprecatedOrUnsafeBufferHandling` (24) |

- **sqc:cppcheck ratio**: 27:1 — reflects the breadth gap (283 CERT C rules vs ~20 checks)
- **clang-tidy coverage**: 47 of 50 diagnostics in `precalc/precalc.c` — heavy use of unsafe string/IO functions in the table generator
- **cppcheck and clang-tidy overlap**: both flag `ERR33-C`/unchecked return values as the dominant finding; sqc finds 4× more on this rule alone (68 vs ~26)

### Real-World: curl

- **Target**: curl 8.19-DEV — libcurl + curl CLI (~937 C files in lib/, src/, docs/examples/)
- **Build**: cmake with SSL/LDAP/nghttp2/zstd/idn2 disabled; 678 compilation units captured for clang-tidy
- **Environment**: Ubuntu 24.04, cppcheck 2.13, clang-tidy 18, sqc Round 9 (2026-02-19)

| Tool | Total Findings | By Severity | Top Rules/Checks |
|------|---------------|-------------|-----------------|
| **sqc** | **131,445** | Critical: 2,277 / High: 44,288 / Medium: 27,682 / Low: 57,198 | EXP34-C (22,350), DCL07-C (16,000), DCL31-C (15,945), EXP19-C (9,105), API00-C (7,777) |
| **cppcheck** | **1,065** | error: 4, warning: 237, style: 599, information: 225 | `nullPointerRedundantCheck`/CWE-476 (177), `constParameterPointer`/CWE-398 (159), `unusedFunction`/CWE-561 (108) |
| **clang-tidy** | **848** | — | `DeprecatedOrUnsafeBufferHandling` (419), `cert-err33-c` (364), `clang-analyzer-valist.Uninitialized` (56) |

- **sqc:cppcheck ratio**: 123:1 — higher than libcrc because curl's macro-heavy code produces more DCL and EXP rule matches that cppcheck skips
- **cppcheck note**: 222 of 225 information findings are `toomanyconfigs` — expected for curl's extensive `#ifdef` preprocessor guards; no `syntaxError` present, findings are valid
- **clang-tidy note**: `DeprecatedOrUnsafeBufferHandling` (419) dominates — curl's widespread use of `sprintf`, `strcpy`, and related functions in the internal HTTP/FTP handlers; `cert-err33-c` (364) flags unchecked return values consistent with sqc's EXP34-C/ERR33-C findings
- **Round 9 impact on curl**: EXP34-C at 22,350 is sqc's top rule on curl — the inter-procedural null-return summaries added in Round 9 are directly relevant here (function calls returning potentially-null pointers across translation units). Juliet showed near-zero Round 9 impact (-73 FP) because test cases are single-file; curl demonstrates the intended real-world target.

### Real-World: mosquitto

- **Target**: Eclipse Mosquitto — MQTT broker + client library (~121 C files in `lib/` + `src/`; 224 compilation units for clang-tidy)
- **Build**: cmake with TLS/WebSockets/tests disabled; `libcjson-dev` required
- **Environment**: Ubuntu 24.04, cppcheck 2.13, clang-tidy 18, sqc Round 9 (2026-02-19)

| Tool | Total Findings | By Severity | Top Rules/Checks |
|------|---------------|-------------|-----------------|
| **sqc** | **59,176** | Critical: 1,181 / High: 20,765 / Medium: 13,182 / Low: 24,048 | EXP34-C (8,657), DCL31-C (6,823), DCL07-C (6,820), API00-C (3,092), MEM31-C (2,874) |
| **cppcheck** | **747** | error: 36, warning: 1, style: 298, information: 412 | `missingInclude` (293), `unusedFunction`/CWE-561 (128), `toomanyconfigs` (117), `uninitvar`/CWE-457 (34) |
| **clang-tidy** | **338** | — | `cert-err33-c` (277), `cert-err34-c` (33), `clang-analyzer-deadcode.DeadStores` (8), `insecureAPI.strcpy` (5) |

- **sqc:cppcheck ratio**: 79:1 (excluding informational cppcheck entries: 175:1)
- **Notable cppcheck finding**: 34 `uninitvar` (error severity, CWE-457) — potential uninitialized variable bugs in broker handling code; these are the highest-confidence real defects found by cppcheck across all three projects tested
- **clang-tidy pattern**: `cert-err33-c` (277) is heavily concentrated in client output code — mosquitto's pub/sub CLI tools call `fprintf`, `fputc`, `fputs`, `strftime`, and `fclose` without checking return values throughout formatted output paths

### Cross-Project Summary

| Project | Files (sqc) | sqc | cppcheck | clang-tidy | sqc density (per file) |
|---------|------------|-----|----------|------------|----------------------|
| libcrc | ~16 | 1,109 | 41 | 50 | ~69 |
| mosquitto | 470 | 59,176 | 747 | 338 | ~126 |
| curl | 937 | 131,445 | 1,065 | 848 | ~140 |

sqc density scales with codebase complexity (libcrc → mosquitto → curl). cppcheck and clang-tidy counts also scale proportionally, confirming consistent analysis across projects. The sqc:cppcheck ratio grows with macro complexity (27:1 libcrc → 79:1 mosquitto → 123:1 curl), reflecting how heavier `#ifdef` usage inflates sqc's DCL/EXP findings while cppcheck's configuration enumeration covers most variants anyway.

### Cross-Tool Capability Analysis

Based on three-project data (libcrc, mosquitto, curl). Documents overlaps, gaps, and FP concerns identified from direct tool comparison.

#### Comparable Checks Across Tools

| Bug Class | sqc Rule | clang-tidy Check | cppcheck Check | Overlap Notes |
|-----------|----------|------------------|----------------|---------------|
| Unchecked return value | ERR33-C | `cert-err33-c` | — | **5× ratio** (1,404 vs 277 on mosquitto) — sqc covers more functions; see §FP concern below |
| Unsafe numeric conversion | ERR34-C | `cert-err34-c` | — | **Gap**: clang-tidy finds 33/6 atoi usages; sqc ERR34-C not in top 15 on either project |
| Null pointer dereference | EXP34-C | `NullDereference` | `nullPointer` / `nullPointerRedundantCheck` | **FP concern**: 8,657:2 sqc:cppcheck ratio on mosquitto; see §EXP34-C below |
| Uninitialized variable | EXP33-C | — | `uninitvar` | **Gap**: cppcheck finds 34 error-severity cases on mosquitto; sqc not prominent there |
| String/buffer safety | STR rules | `DeprecatedOrUnsafeBufferHandling` | — | clang-tidy finds 419 on curl (sprintf/strcpy); sqc STR rules don't appear in top 15 |
| char sign safety | — | `cert-str34-c` | — | **Gap**: clang-tidy finds 3 on curl; sqc has no equivalent in top results |
| Const-correctness | — | — | `constParameterPointer` / `constVariablePointer` | **Gap**: cppcheck finds 266 on curl, 48 on mosquitto; no sqc equivalent |
| Unused functions | — | — | `unusedFunction` | cppcheck finds 128/108 on mosquitto/curl; no sqc equivalent (not a CERT C rule) |
| Dead stores | — | `deadcode.DeadStores` | — | clang-tidy finds 8 on mosquitto; no sqc equivalent |

#### EXP34-C: Probable High FP Rate

sqc's dominant rule by volume shows a 4,300:1 ratio against cppcheck's confirmed null-pointer errors on mosquitto:

| Project   | sqc EXP34-C | cppcheck `nullPointer` (error) | cppcheck `nullPointerRedundantCheck` (warning) |
|-----------|-------------|-------------------------------|------------------------------------------------|
| mosquitto | 8,657       | 2                             | 0                                              |
| curl      | 22,350      | 0                             | 177                                            |

cppcheck's `nullPointer [error]` and clang-tidy's `NullDereference` use data-flow analysis and only fire when they can prove a null-dereference path. sqc's EXP34-C flags any pointer dereference without a locally-visible null check, regardless of caller guarantees, function contracts, or assert guards. The result is a count so large that EXP34-C alone accounts for 15–17% of all sqc output — and is nearly unactionable in that volume.

Note: cppcheck's `nullPointerRedundantCheck` (177 on curl, warning-level) is a different pattern — a pointer that is checked for null *after* already being dereferenced. This is a distinct semantic from EXP34-C.

**Root cause**: EXP34-C lacks path-sensitive null-check dominance detection. The Round 9 CFG + reaching-definitions infrastructure is the prerequisite for fixing this.

#### ERR33-C: Possible Count Inflation

sqc finds approximately 5× more unchecked return values than clang-tidy's `cert-err33-c` (1,404 vs 277 on mosquitto). Clang-tidy's check is scoped to a specific list of POSIX/C standard functions. sqc's ERR33-C uses the ~270-function `std_functions` database. The ratio is plausible but warrants review: functions like `printf` to stdout and `fclose` are CERT-flaggable but the caller rarely has a recovery path, making these low-value findings in practice.

#### ERR34-C: No Coverage Gap

clang-tidy's `cert-err34-c` fires on `atoi`, `atol`, `atof`, and `atoll`. sqc's ERR34-C is implemented and also covers the `scanf` family. sqc's ERR34-C finds **more** than clang-tidy on both projects:

| Project | sqc ERR34-C | clang-tidy cert-err34-c |
|---------|------------|------------------------|
| mosquitto | 126 | 33 |
| curl | 28 | 6 |

sqc's higher count reflects its broader function coverage (includes `sscanf`, `fscanf`, `scanf`). ERR34-C is not in sqc's top 15 rules by volume on either project because it is a focused, low-volume check — which is the correct behavior for this rule.

#### DCL Rule Volume: Signal Dilution

The DCL family accounts for a disproportionate share of sqc output:

| Rules              | mosquitto | curl   | % of sqc total |
|--------------------|-----------|--------|----------------|
| DCL07-C + DCL31-C  | 13,643    | 31,945 | ~23–24%        |
| All DCL rules      | ~20,000   | ~42,000| ~34–32%        |

Neither cppcheck nor clang-tidy has equivalents for DCL07-C (include type info in function declarations) or DCL31-C (declare identifiers before use). These rules target C89/C90 patterns that remain common in legacy codebases for non-buggy reasons. Their volume means that roughly one-third of sqc's output on a real project is declaration-style findings — burying higher-value security rules.

#### EXP33-C Gap on mosquitto

cppcheck's 34 `uninitvar [error]` findings on mosquitto are the highest-confidence real defects in the entire dataset (error-severity, data-flow proven). sqc's EXP33-C finds 902 on mosquitto and 2,247 on curl — so sqc IS running. The findings are in different files:

- cppcheck `uninitvar`: `messages_mosq.c`, `bridge.c`, `bridge_topic.c` — struct fields not set after allocation (`message->msg.qos`, `cur->msg.mid`, etc.)
- sqc EXP33-C: `extended_auth.c`, `packet_mosq.c`, `alias_mosq.c` — local scalar variables read before assignment

These are different sub-patterns of CWE-457. sqc EXP33-C detects local scalar uninitialized reads. cppcheck `uninitvar` detects partial struct initialization (allocated struct with some members left unset). The cppcheck pattern requires tracking struct member initialization — a distinct analysis that sqc doesn't currently perform. The 34 cppcheck error-severity findings are genuine defects sqc cannot reach with its current analysis model.

#### What sqc Uniquely Covers

Despite the above concerns, sqc's coverage breadth is genuine — clang-tidy fires 2–3 CERT C checks across all projects; cppcheck's real findings are mostly style. sqc uniquely covers:

- **POS49-C** (POSIX misuse): 4,534 on curl — no competitor equivalent
- **INT32-C / INT30-C** (signed/unsigned overflow): 2,793 / 2,378 on curl — competitors largely skip
- **MEM30-C / MEM31-C** (use-after-free, memory management): significant counts
- **API00-C / API02-C**: 7,777 / 2,192 on curl — no competitor equivalent
- **EXP12-C, EXP19-C**: 1,555 / 9,105 on mosquitto/curl — no competitor equivalent
- **270+ additional rules** across integer, floating-point, environment, concurrency, and POSIX categories

#### Improvement Priorities (from Real-World Data)

| Priority | Issue | Evidence | Status |
|----------|-------|----------|--------|
| **P1** | EXP34-C FP rate | 4,300:1 ratio vs cppcheck confirmed null errors | Pending: CFG dominance analysis (Round 11 target) |
| **P1** | DCL07-C / DCL31-C macro FPs | ALL_CAPS macro calls flagged as undeclared functions; ~48% of function-call findings | **Fixed (Round 11)**: `is_macro_like_name()` guard added to both rules |
| **P1** | DCL07-C / DCL31-C POSIX gaps | `strcasecmp`, `strdup`, `strtok_r` missing from std_functions | **Fixed (Round 11)**: Added to std_functions.rs |
| **P2** | ERR34-C gap | N/A — sqc finds MORE than clang-tidy (126 vs 33 on mosquitto, 28 vs 6 on curl) | **Closed**: No gap exists |
| **P2** | EXP33-C on mosquitto | 34 cppcheck error-severity uninitvar not matched by sqc | Pending: investigate trigger conditions |
| **P3** | ERR33-C ratio | 5× over clang-tidy; review low-value functions | Pending: exclude no-recovery-path functions |
| **P3** | const-correctness gap | cppcheck finds 266 const-param/variable findings on curl | Pending: assess CERT C applicability |

---

## Architecture Assessment

### What SqC Is

- **Single-translation-unit, AST-based pattern matcher** using tree-sitter
- 283 rules ranging from shallow pattern matching to deep multi-pass analysis (3,900 lines for ARR30-C)
- Cross-file analysis via function name pre-scanning (`-d` flag)
- Sequential file processing (parallelized externally via shell scripts)

### What SqC Has (as of Round 9)

- Local variable/type inference within functions (`collect_variable_types` pattern)
- Preprocessor block traversal (`preproc_*` node recursion)
- Standard function database (~270 C11/POSIX functions)
- Cross-file function name scanning
- Taint tracking (FIO30-C)
- Variable state tracking (EXP33-C uninitialized variable detection)
- **CFG construction** per function with dominance information
- **Reaching definitions** (data-flow) for path-sensitive analysis
- **Inter-procedural function summaries** (null returns, freed parameters, no-return functions)

### What SqC Lacks

- **No preprocessor expansion** — macros appear as function calls
- **No alias analysis** — pointer aliasing not resolved
- **No symbolic execution** — can't evaluate complex expressions
- **No SSA form** — no use-def chains beyond reaching definitions
- **No value range analysis** — beyond literal constants
- **No whole-program analysis** — limited to function summary pre-scanning

### Architectural Ceiling

The ~43.8% Juliet TP rate is likely near the ceiling for single-translation-unit AST analysis. Without value-range and alias analysis, the tool cannot distinguish validated from unvalidated inputs, null-checked from unchecked pointers, or computed buffer sizes from literal ones.

---

## CI/CD Readiness

### Overall: ~85% Ready

| Component | Status | Readiness |
|-----------|--------|-----------|
| Output Formats | CSV, XLSX, JSON, SARIF 2.1.0 | 100% |
| Exit Codes | `--fail-on-violation`, `--fail-on-severity` | 100% |
| Severity Filtering | `--min-severity`, `--fail-on-severity` | 100% |
| Rule Filtering | `--rules ARR30-C,MEM30-C` | 100% |
| Incremental | `--diff` (git modified files) | 90% — no baseline comparison |
| CI Workflows | GitHub Actions + Azure DevOps templates included | 100% |
| Suppressions | SHA-256 code-location | 70% — not baseline-aware |
| Documentation | README CI/CD section | 80% |
| Docker | No image published | 0% |

### Remaining Gaps

1. **No baseline-aware suppression** — can't report "only new violations since last run"
2. **No Docker image** for containerized CI/CD
3. **Real-world violation density established but unclassified** — libcrc ~69/file, curl ~140/file; no ground truth to split TP vs FP on production code (manual audit or CVE cross-reference needed)

---

## Next Steps / Roadmap

### Phase 2: Real-World Validation (In Progress)

1. **~~Run on curl~~** ✅ — 131,445 violations across 937 files; three-way comparison complete
2. **~~Run on mosquitto~~** ✅ — 59,176 violations across 470 files; three-way comparison complete; 34 `uninitvar` cppcheck findings are highest-confidence real defects found to date
3. **Run on openssl, zlib, hostap** — extend real-world corpus
4. Tune rules based on real-world FP patterns (EXP34-C and DCL07-C/DCL31-C are top targets across all three projects)
5. **~~Compare with Cppcheck and clang-tidy on same codebases~~** ✅ — direct comparison complete for libcrc, mosquitto, and curl (see §8)

### Phase 3: Continued FP Reduction

6. **~~EXP34-C: `&&` short-circuit guard~~** ✅ — `(ptr != NULL) && (ptr->field)` now recognized as safe; fixes Juliet binary_if FPs
7. **~~EXP34-C: stack array fix~~** ✅ — `int *arr[5]` no longer treated as potentially null; fixes Juliet _66a variant FPs (~858 files affected)
8. **EXP34-C: inter-procedural null dominance** — use reaching definitions to suppress flags on paths where a null check dominates the dereference; requires path-sensitive CFG analysis (most impactful remaining EXP34-C improvement)
9. **Leverage inter-procedural summaries for MEM30-C** — track freed-parameter propagation across calls
10. **CWE-457 improvement** — DCL02-C contributes 36K FPs in this category; tighten scope or add same-function check
11. **CWE-256 and CWE-338** — Both are Windows-specific (w32) in Juliet; structural FP issue (good() functions use more complex Windows API code). MSC30-C already handles rand(). No clean rule fix available without Windows API semantics.

### Phase 4: Architecture Evolution

11. Internal parallelization (rayon for file-level parallelism)
12. Incremental parsing (only re-parse changed files)
13. Baseline-aware suppression ("only new violations")
14. Docker image for containerized CI/CD

### Definition of Done

**Tier 1 — Minimum Viable for CI/CD** (complete)
- [x] `--fail-on-violation` and `--fail-on-severity` flags
- [x] JSON, CSV, SARIF output
- [x] Incremental analysis (`--diff`)
- [x] Severity threshold filtering
- [x] GitHub Actions + Azure DevOps example workflows

**Tier 2 — Production Quality**
- [x] Real-world validation on 3+ open-source projects (libcrc, curl, mosquitto — direct three-way comparison with cppcheck and clang-tidy)
- [ ] Real-world validation on 5+ projects (openssl, zlib, hostap, sqlite pending)
- [ ] Baseline-aware suppression
- [ ] Docker image
- [ ] TP rate ≥ 45% on Juliet

**Tier 3 — Competitive**
- [ ] TP rate ≥ 50% on Juliet
- [ ] Direct benchmarked comparison with Cppcheck and Clang
- [ ] Published comparison results

---

## Resolved Issues

### DCL02-C Stack Overflow (Fixed 2026-01-07, commit `def416f3`)

**Problem**: Unbounded recursive AST traversal in DCL02-C caused stack overflow on large files (SQLite `complete.c`, 290 lines).

**Fix**: Converted recursive traversal to iterative with explicit stack; added depth limit (50) to recursive helpers; added scope nesting limit (100 levels).

**Verification**: `git log --oneline | grep DCL02-C`

### Output Buffer Saturation During Benchmarks (2026-01-15)

**Problem**: SqC emits one status line per rule per file (~100 rules × N files = 33K+ lines for 336-file directory). Flooding Claude's output buffer caused apparent hang.

**Resolution**: Always suppress or redirect output during directory scans:
```bash
./target/release/sqc directory/ --export results.csv 2>/dev/null
```

---

## Scripts and Data Locations

### Benchmark Scripts
```
scripts/analyze_juliet_results.py      Ground truth analysis (OMITBAD/OMITGOOD classification)
scripts/run_juliet_multi_cwe.sh        Sequential multi-CWE runner
scripts/run_juliet_parallel.sh         Parallel multi-CWE runner (12 jobs)
```

### Benchmark Data
```
~/data/benchmarks/juliet-test-suite-c/
  testcases/                           118 CWE categories, 54,484 .c files
  testcasesupport/                     Shared helper functions

/tmp/juliet_results/                   Per-run output (not version controlled)
  CWE{id}_{name}.csv                   Raw SqC CSV output per CWE
  CWE{id}_{name}_analysis.txt          Ground truth analysis per CWE
  multi_cwe_summary.txt                TP/FP rates summary
```

### CI/CD Configuration
```
.github/workflows/sqc-analysis.yml    GitHub Actions workflow
ci/azure-pipelines.yml                Azure DevOps pipeline
```

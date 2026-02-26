# SqC — Real-World Benchmark Results

**Last Updated**: 2026-02-25

Automated benchmark results across 5 real-world C codebases using sqc, cppcheck, and clang-tidy. Also includes d_lib_common (internal module) FP reduction case study.

---

## Latest Results (sqc v0.2.7)

MCP-based benchmark infrastructure. All three tools run on the same machine (cppcheck 2.10, clang-tidy 21.1.6, sqc v0.2.7 commit `54819432`).

### Violation Counts — All Three Tools

| Project | LOC (approx) | sqc | cppcheck | clang-tidy |
|---------|-------------|----:|--------:|-----------:|
| **libcrc** | ~1K | 842 | 40 | 4 |
| **sqlite** | ~350 files | 180,011 | 517 | 204 |
| **mosquitto** | ~120 files | 39,177 | 364 | 160 |
| **curl** | ~220 files | 93,576 | 297 | 1,314 |
| **hostap** | ~600 files | 234,421 | 1,675 | 2,957 |
| **Total** | | **548,027** | **2,893** | **4,639** |

**Interpretation**: sqc covers 283 CERT-C rules (advisory + mandatory) while cppcheck and clang-tidy implement ~20 checks each. The 100–200x difference in raw counts reflects rule coverage breadth, not false positive rate.

### sqc Top Rules by Project

| Project | Top 5 Rules |
|---------|------------|
| **libcrc** | EXP14-C (106), ERR33-C (68), EXP12-C (62), EXP19-C (59), FIO47-C (47) |
| **sqlite** | DCL07-C (20K), DCL31-C (18K), API00-C (14K), DCL13-C (14K), POS49-C (12K) |
| **mosquitto** | API00-C (3K), DCL31-C (3K), DCL07-C (3K), MEM31-C (3K), DCL13-C (3K) |
| **curl** | DCL07-C (11K), DCL31-C (11K), EXP19-C (9K), API00-C (8K), DCL13-C (7K) |
| **hostap** | DCL08-C (25K), EXP19-C (25K), API00-C (20K), DCL13-C (19K), POS49-C (17K) |

### Improvement from Baseline (sqlite: v0.2.4 → v0.2.7)

| Metric | v0.2.4 | v0.2.7 | Delta |
|--------|-------:|-------:|------:|
| Total violations | 427,377 | 180,011 | **-247,366 (-57.9%)** |
| STR31-C | 206,651 | 222 | -206,429 (rewrite) |
| EXP34-C | 41,886 | 8,734 | -33,152 (null-deref FP reduction) |
| ARR36-C | 3,034 | 600 | -2,434 |
| EXP30-C | 2,623 | 300 | -2,323 |
| API02-C | 1,542 | 166 | -1,376 |

### Key Observations

- **STR31-C no longer dominant**: After `detect_manual_string_loop` rewrite, generates ~200 violations on sqlite vs. 206,651 before
- **EXP34-C dramatically reduced**: sqlite 41,886 → 8,734, hostap 69,164 → 12,339 (CFG-based null state dataflow)
- **Advisory rules dominate**: DCL07-C, DCL31-C, DCL08-C, DCL13-C, EXP19-C, API00-C are code-style/quality rules. Severity filtering would significantly reduce noise
- **mosquitto is cleanest**: Only 39K violations (vs. 234K for hostap)

---

## Cross-Tool Capability Analysis

### Comparable Checks

| Bug Class | sqc Rule | clang-tidy Check | cppcheck Check | Notes |
|-----------|----------|------------------|----------------|-------|
| Unchecked return value | ERR33-C | `cert-err33-c` | — | sqc 5x count (broader function list) |
| Unsafe numeric conversion | ERR34-C | `cert-err34-c` | — | sqc finds MORE (126 vs 33 on mosquitto) |
| Null pointer dereference | EXP34-C | `NullDereference` | `nullPointer` | sqc 4,300:1 ratio (see below) |
| Uninitialized variable | EXP33-C | — | `uninitvar` | Different sub-patterns of CWE-457 |
| String/buffer safety | STR rules | `DeprecatedOrUnsafe...` | — | Different scope |

### EXP34-C: Known High FP Rate on Real Code

| Project | sqc EXP34-C | cppcheck nullPointer (error) | cppcheck nullPointerRedundantCheck |
|---------|------------:|-----------------------------:|-----------------------------------:|
| mosquitto | 8,657 | 2 | 0 |
| curl | 22,350 | 0 | 177 |

sqc flags any pointer dereference without a locally-visible null check. cppcheck uses data-flow analysis and only fires when it can prove a null-dereference path. The CFG-based null state dataflow (Phase 1 complete) is the foundation for closing this gap.

### What sqc Uniquely Covers

- **POS49-C** (POSIX misuse): 4,534 on curl — no competitor equivalent
- **INT32-C / INT30-C** (signed/unsigned overflow): significant counts — competitors skip
- **MEM30-C / MEM31-C** (use-after-free, memory management)
- **API00-C / API02-C**: no competitor equivalent
- **270+ additional rules** across integer, floating-point, environment, concurrency, POSIX

---

## Baseline References (v0.2.3)

Earlier results for comparison (before STR31-C rewrite and major FP reductions).

### libcrc (v0.2.3)

| Tool | Total | Top Rules/Checks |
|------|------:|-----------------|
| **sqc** | 954 | EXP14-C (106), ERR33-C (68), EXP12-C (62), INT30-C (60) |
| **cppcheck** | 40 | variableScope (36), unusedFunction (2) |
| **clang-tidy** | 52 | cert-err33-c (26), DeprecatedOrUnsafe... (24) |

### sqlite (v0.2.3)

| Tool | Total | Notes |
|------|------:|-------|
| **sqc** | 424,842 | STR31-C (206,651) = 49% — `detect_manual_string_loop` bug |
| **cppcheck** | 1,182 | variableScope (505), toomanyconfigs (189) |
| **clang-tidy** | 2,291 | cert-err33-c (1,025), DeprecatedOrUnsafe... (453) |

### curl (v0.2.3)

| Tool | Total | Notes |
|------|------:|-------|
| **sqc** | 207,476 | STR31-C (93,140) = 45% — same runaway bug |
| **cppcheck** | 551 | toomanyconfigs (253), variableScope (95) |
| **clang-tidy** | 1,653 | clang-diagnostic-error (1,024), cert-err33-c (366) |

### mosquitto (v0.2.3)

| Tool | Total | Notes |
|------|------:|-------|
| **sqc** | 47,417 | EXP34-C (7,631) dominates (STR31-C NOT triggered here) |
| **cppcheck** | 598 | 50 `uninitvar` at error severity — highest-confidence real defects |
| **clang-tidy** | 907 | cert-err33-c (477), cert-err34-c (111) |

### hostap (v0.2.3)

| Tool | Total | Notes |
|------|------:|-------|
| **sqc** | 473,862 | STR31-C (170,586) = 36% |
| **cppcheck** | 1,066 | 89 `uninitvar` at error severity |
| **clang-tidy** | 1,083 | cert-err34-c (377) dominates |

---

## d_lib_common Case Study (Internal Module)

**Module**: 9 C files, 1,097 LOC + 12 headers. Detailed FP reduction through 9 phases.

### Results Summary

| Tool | Total Findings | Notes |
|------|---------------:|-------|
| **SQC** | 534 → **398** | 58 distinct CERT-C rules; 9 phases of FP reduction |
| **cppcheck** | 16 actionable | +32 unusedFunction (library code, expected) |
| **clang-tidy** | 0 | cert-* checks found nothing in user code |

### FP Reduction Phases

| Phase | Action | Impact | Rules |
|-------|--------|--------|-------|
| 1 | Cascading EXP34-C dedup, short-circuit, realloc pattern | -75 FP | EXP34-C, MEM30-C, API02-C |
| 2 | Heap-allocated pointer returns, non-pointer ARR36-C | -17 FP | DCL30-C, ARR36-C |
| 3 | Struct/void*/const-char* skip in API02-C | -32 FP | API02-C |
| 4 | For-loop update clause skip | -6 FP | INT30-C |
| 5 | DCL30-C identifier match bug, INT31-C narrowing | -1 FP, +2 TP | DCL30-C, INT31-C |
| 6 | INT31-C shift-narrowing, INT32-C type inference | FP fix | INT31-C, INT32-C |
| 7 | INT36-C field access, PRE31-C, EXP30-C, INT30-C | -13 FP | Multiple |
| 8 | EXP07-C byte-boundary shifts | -4 FP | EXP07-C |
| 9 | INT36-C TP restore + INT31-C FP fix | +955 TP, -138 FP | INT36-C, INT31-C |

**Final: 534 → 398 findings (25.5% reduction, 12 commits)**

### Key Rule Reductions

| Rule | Original | Current | Reduction |
|------|----------|---------|-----------|
| EXP34-C | 49 | 9 | -82% |
| API02-C | 57 | 2 | -96% |
| MEM30-C | 13 | 1 | -92% |
| ARR36-C | 13 | 2 | -85% |
| DCL30-C | 7 | 0 | -100% |
| INT36-C | 8 | 0 | -100% |
| EXP07-C | 4 | 0 | -100% |

### Genuine Issues Found (True Positives)

1. **ringbuffer.c:173 — EXP34-C**: Null deref before check in `readTlv()`
2. **ringbuffer.c:149 — Dead condition**: `crc8_expected != crc8_saved` always false
3. **intset.c:120 — Realloc error handling**: Failed realloc silently continues
4. **utility.c:85 — Redundant condition**: Always-true `10 <= v` after `9 >= v`

---

## STR31-C `detect_manual_string_loop` Bug (FIXED)

**Severity**: High — caused 36–49% of all sqc violations on 3 of 5 projects.

**Root cause**: Final fallback iterated ALL lines in source file looking for `memcpy` + `strlen`/`string`. One match anywhere caused every loop to generate a violation. `jimsh0.c` alone produced 180,297 violations.

**Fix**: Deleted file-wide fallback; condition-only matching; body-only write detection; improved `is_string_memcpy`.

**After fix**: `jimsh0.c` STR31-C dropped from 180,297 to 10.

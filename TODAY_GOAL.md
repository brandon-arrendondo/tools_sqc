# d_lib_common Static Analysis — SQC / cppcheck / clang-tidy Comparison

**Date:** 2026-02-25
**Module:** `~/data/d_lib_common` (9 C files, 1,097 LOC + 12 headers)
**Purpose:** Baseline analysis to validate SQC accuracy and establish a repeatable
process for all modules.

---

## 1. Baseline Results Summary

| Tool | Total Findings | Notes |
|------|---------------|-------|
| **SQC** | 534 | 58 distinct CERT-C rules triggered |
| **cppcheck** | 16 actionable | +32 unusedFunction (library, expected) |
| **clang-tidy (cert-*)** | 0 | All warnings in system headers only |

**Density (findings / KLOC):**
- SQC: **486 / KLOC** — expected for CERT-C (advisory + mandatory rules, no suppression)
- cppcheck: **14.6 / KLOC** — only flags high-confidence issues
- clang-tidy: **0 / KLOC** — cert-* checks are narrow; broader checks find ~20/file

---

## 2. SQC Findings Breakdown (Top 20 Rules)

| Rule | Original | Current | Severity | BRULE Mapping | FP Assessment |
|------|----------|---------|----------|---------------|---------------|
| DCL08-C | 85 | 85 | Low | — | **Mostly TP** — const-qualify params not modified |
| DCL13-C | 47 | 47 | Low | — | **Needs review** — function pointer prototype completeness |
| DCL15-C | 41 | 41 | Low | — | **Mostly TP** — file-scope functions should be `static` (library exports excepted) |
| INT01-C | 29 | 29 | Medium | — | **Mixed** — size representation suggestions |
| API00-C | 20 | 20 | Medium | — | **Needs review** — consistent error-checking interface |
| MEM10-C | 17 | 17 | Medium | — | **Advisory** — pointer validation function pattern |
| DCL07-C | 5 | **17** | Low | — | +12 from user code changes (CRC refactor) |
| PRE00-C | 13 | 13 | Low | — | **Mostly TP** — macro parenthesization |
| DCL19-C | 11 | 11 | Low | BRULE-039 | **Mostly TP** — variable scope minimization |
| INT32-C | 11 | 11 | High | BRULE-047 | **Reviewed** — FP-004 fixed (type inference) |
| INT30-C | 16 | **9** | High | BRULE-047 | **Reduced** — for-loop + `>0` guard skip |
| EXP34-C | 49 | **9** | High | BRULE-047 | **Reduced 82%** — cascading dedup, short-circuit |
| ERR33-C | 7 | 7 | High | — | **Mostly TP** — unchecked return values |
| INT07-C | 5 | 5 | Medium | BRULE-029 | **Mostly TP** — integer conversion |
| DCL31-C | 5 | 5 | Low | BRULE-035 | **Mostly TP** — identifier uniqueness |
| API02-C | 57 | **2** | High | — | **Reduced 96%** — struct/void*/const char* skip |
| ARR36-C | 13 | **2** | High | BRULE-047 | **Reduced 85%** — non-pointer param filter |
| INT31-C | 0 | **2** | High | BRULE-029 | **+2 new TPs** — narrowing detection gap fixed |
| MEM30-C | 13 | **1** | Critical | BRULE-045 | **Reduced 92%** — realloc-to-temp pattern |
| DCL30-C | 7 | **0** | High | — | **Eliminated** — identifier matching bug fix |
| INT36-C | 8 | **0** | Low | — | **Eliminated** — struct field access excluded |
| EXP07-C | 4 | **0** | Low | — | **Eliminated** — byte-boundary shifts excluded |
| PRE31-C | 1 | **0** | High | — | **Eliminated** — string literal skip |
| EXP30-C | 3 | **0** | High | — | **Eliminated** — `x = f(x)` safe per C11 |

**Remaining rules:** EXP33-C ×4, DCL05-C ×4, STR09-C ×4, ERR00-C ×4, etc. (1–4 each)

---

## 3. False Positive Analysis

### 3.1 EXP34-C — Null Pointer Dereference (49 findings, est. ~40 FP)

**Pattern A: Short-circuit evaluation not recognized (utility.c:20)**
```c
return (str == NULL || str[0] == '\0');  // str[0] only reached if str != NULL
```
SQC flags `str[0]` as potential null deref. The `||` short-circuit guarantees safety.
**Status:** Known FP pattern (noted in MEMORY.md). FIXED for `&&` but `||` variant persists.

**Pattern B: Interprocedural null-guard not tracked (utility.c:31,47)**
```c
if (Utility_StringIsNullOrEmpty(str)) { ... }
else {
    strlen(str);  // str guaranteed non-null here, but SQC doesn't know
}
```
SQC can't reason that `Utility_StringIsNullOrEmpty` returning false implies non-null.
**Status:** Requires interprocedural analysis. Not fixable short-term.

**Pattern C: Cascading dereferences after first unchecked use (ringbuffer.c:173–231)**
```c
result_e readTlv(ringbuffer_info_t *ptrRingBufferInfo, tlv_info_t *ptrTlv)
{
    int tlvStartIndex = ptrRingBufferInfo->readIndex;  // line 173: deref before NULL check
    ...
    if (ptrRingBufferInfo->writeIndex == ...)  // line 179+: cascade
```
Line 173 is a **genuine TP** — `ptrRingBufferInfo` is dereferenced before any null check
(unlike `writeTlv()` which checks on line 105 first). But lines 179–231 are all cascading
from the same unchecked parameter — 38 FP findings from 1 real issue.
**Action:** SQC should deduplicate cascading null-deref within a function.

**Pattern D: Parameter assumed non-null by callers (incrementIndex:90)**
Static function only called after null checks in callers. SQC flags the first dereference.
**Status:** Inter-procedural — not fixable without call-graph analysis.

### 3.2 MEM30-C — Use-After-Free (13 findings, est. ~9 FP)

**Pattern: realloc to temp variable (intset.c:115–130)**
```c
uint32_t *items = Memory_Realloc(self->items, sizeof(uint32_t) * self->capacity);
if (items) {
    self->items = items;    // safe: old pointer freed by realloc, new pointer assigned
}
else {
    // TODO - this is bad, likely bail
}
// ... later uses of self->items ...
self->items[0] = element;  // SQC flags as use-after-free
```
The realloc-to-temp pattern is correct. If realloc fails, `self->items` still points to
the original allocation. If realloc succeeds, `self->items` is updated. SQC appears to
flag the post-realloc usage as use-after-free because the original pointer was passed to
realloc (which may free it).
**Action:** SQC should recognize `temp = realloc(ptr, ...); if (temp) ptr = temp;` as safe.

### 3.3 API02-C — Array Size Parameter (57 findings, est. ~15 FP)

Functions accepting `const char *str` for null-terminated strings are flagged for not
taking a size parameter. While CERT-C API02-C is correct in general, null-terminated
string functions (like `strlen`, `strcmp` wrappers) conventionally don't take sizes.
**Action:** Consider suppressing API02-C for `const char *` parameters in utility functions,
or adding a heuristic for null-terminated string patterns.

---

## 4. BRULE Coverage Matrix

Mapping d_lib_common SQC findings to the BISSELL development workbook rules:

### 4.1 Fully Superseded BRULEs (enforced by SQC)

| BRULE | Description | CERT-C Rule(s) | d_lib_common Findings | Assessment |
|-------|-------------|----------------|----------------------|------------|
| BRULE-008 | Header guards no `_` prefix | DCL37-C | 0 | **PASS** — no violations |
| BRULE-030 | Named constants | DCL06-C | 2 | 2 magic numbers found |
| BRULE-032 | Vars initialized at definition | EXP33-C | 4 | 4 potential uninit reads |
| BRULE-036 | Operator precedence parens | EXP00-C | 3 | 3 precedence issues |
| BRULE-039 | Minimize variable scope | DCL19-C | 11 | 11 scope reductions possible |
| BRULE-045 | No memory/resource leaks | MEM30/31/34/35-C | 15 | ~6 TP after FP reduction |
| BRULE-047 | No undefined behavior | EXP34/ARR30/ARR38/EXP33/INT30/INT32-C | 106 | ~20 TP after FP reduction |
| BRULE-051 | Concurrency control | CON01–CON43-C | 0 | **PASS** — no threading in this module |
| BRULE-056 | No hardcoded credentials | MSC41-C | 0 | **PASS** — no sensitive data |

### 4.2 Partially Covered BRULEs

| BRULE | Description | CERT-C Rule(s) | d_lib_common Findings |
|-------|-------------|----------------|----------------------|
| BRULE-029 | Strong typing / fixed-width | INT07-C, INT31-C | 5 (INT07-C) |
| BRULE-034 | No dead/unused code | EXP12-C, MSC07-C | 3 (EXP12-C) |
| BRULE-035 | Proper header use | DCL36-C, DCL31-C | 5 (DCL31-C) |

### 4.3 BRULEs Outside SQC Scope

| BRULE | Enforcement | d_lib_common Status |
|-------|-------------|-------------------|
| BRULE-026 | Compiler flags | Needs `-Wall -Wextra -Werror -Wpedantic` verification |
| BRULE-028 | Single point of exit | Manual review needed |
| BRULE-031 | Max nesting depth 2 | Complexity tool needed (ringbuffer.c `readTlv` likely violates) |
| BRULE-033 | File/function comments | Manual review needed |
| BRULE-037 | switch/default | `MSC01-C` not yet in SQC |
| BRULE-041 | McCabe < 20 | Complexity tool needed |

---

## 5. Genuine Issues Found (True Positives)

### 5.1 High Priority (code changes recommended)

1. **ringbuffer.c:173 — EXP34-C** (null deref before check)
   `readTlv()` dereferences `ptrRingBufferInfo->readIndex` on line 173 before any NULL check.
   Compare with `writeTlv()` which correctly checks NULL on line 105 first.
   **Fix:** Move `tlvStartIndex` assignment inside the null-check else block.

2. **ringbuffer.c:149 — Dead condition** (cppcheck: knownConditionTrueFalse)
   `crc8_expected != crc8_saved` is always false because both are initialized to 0 and
   never modified before the check (without `USE_CRC_INTEGRITY` defined).

3. **ringbuffer.c:114 — Unused assignment** (cppcheck: unreadVariable)
   `tlvCrcIndex = 0` is never read (only used under `USE_CRC_INTEGRITY`).

4. **intset.c realloc error handling** — The `// TODO - this is bad, likely bail` on
   line 120 means a failed realloc silently continues, potentially writing beyond the
   (now possibly fragmented) allocation.

5. **utility.c:85 — Redundant condition** (cppcheck: knownConditionTrueFalse)
   `10 <= v` is always true in the `else if` after `9 >= v` check.

### 5.2 Medium Priority (code quality)

6. **DCL08-C ×85** — Many function parameters could be `const`-qualified.
7. **DCL15-C ×41** — Internal functions not marked `static`.
8. **ERR33-C ×7** — Unchecked return values (malloc, realloc, etc.).
9. **DCL19-C ×11** — Variables declared at wider scope than needed (matches cppcheck variableScope).

### 5.3 Low Priority / Advisory

10. **API02-C ×57** — Many are legitimate for null-terminated strings.
11. **PRE00-C ×13** — Macro hygiene improvements.
12. **INT01-C ×29** — Size type suggestions.

---

## 6. cppcheck vs SQC Cross-Validation

| cppcheck Finding | SQC Equivalent | Agreement? |
|-----------------|----------------|------------|
| constParameterPointer ×6 | DCL08-C ×85 | SQC finds more (deeper analysis) |
| variableScope ×3 | DCL19-C ×11 | SQC finds more (deeper analysis) |
| knownConditionTrueFalse ×2 | — | SQC doesn't flag dead conditions (gap) |
| constVariablePointer ×3 | DCL08-C | Partial overlap |
| unreadVariable ×1 | — | SQC doesn't flag unused assignments (gap) |
| redundantInitialization ×1 | — | Not in SQC scope |
| unusedFunction ×32 | — | Library code — expected; not in SQC scope |

**Key gap:** cppcheck catches dead conditions and unused variables that SQC doesn't.
These map to BRULE-034 (no dead code) which is only partially covered by SQC.

---

## 7. Action Plan

### Phase 1: SQC False Positive Reduction (tool improvements)

| # | Action | Impact | Rules Affected |
|---|--------|--------|---------------|
| 1 | Deduplicate cascading EXP34-C within a function | -38 FP on ringbuffer alone | EXP34-C |
| 2 | Recognize `||` short-circuit for null checks | -5 FP | EXP34-C |
| 3 | Recognize realloc-to-temp safe pattern | -9 FP | MEM30-C |
| 4 | Suppress API02-C for `const char *` null-terminated string params | -15 FP | API02-C |

**Phase 1 reduction:** 75 FP eliminated → SQC from 534 to 459 findings (-14%)
**Commit:** `bbfe6c35` on branch `fix/fp-reduction-d-lib-common`

| # | Action | Impact | Rules Affected |
|---|--------|--------|---------------|
| 5 | Skip heap-allocated pointer returns in DCL30-C | -6 FP | DCL30-C |
| 6 | Filter integer (non-pointer) comparisons in ARR36-C | -11 FP | ARR36-C |

**Phase 2 reduction:** 17 additional FP eliminated → SQC from 459 to 442 findings
**Commit:** `d08cfc1d` on branch `fix/fp-reduction-d-lib-common`

**Cumulative after Phase 1+2:** 534 → 442 findings (-92, 17.2% reduction)

| # | Action | Impact | Rules Affected |
|---|--------|--------|---------------|
| 7 | Skip struct-type pointers (OOP self/this) in API02-C | -23 FP | API02-C |
| 8 | Skip void* single-object pointers in API02-C | -9 FP | API02-C |

**Phase 3 reduction:** 32 additional FP eliminated → SQC from 442 to 410 findings
**Commit:** `1d656c31` on branch `fix/fp-reduction-d-lib-common`

**Cumulative after Phase 1+2+3:** 534 → 410 findings (-124, 23.2% reduction)

| # | Action | Impact | Rules Affected |
|---|--------|--------|---------------|
| 9 | Skip for-loop update clause increments in INT30-C | -6 FP | INT30-C |

**Phase 4 reduction:** 6 additional FP eliminated → SQC from 410 to 404 findings
**Commit:** `d8c30216` on branch `fix/fp-reduction-d-lib-common`

**Cumulative after Phase 1-4:** 534 → 404 findings (-130, 24.3% reduction)

### Phase 5: User Feedback Fixes

| # | Action | Type | Rules Affected |
|---|--------|------|---------------|
| 10 | Fix contains_identifier matching by byte length (enum constants misidentified) | FP fix | DCL30-C |
| 11 | Add uint16_t/uint32_t/int/short to WIDE_TYPES for narrowing detection | FN fix | INT31-C |
| 12 | Add shift-narrowing heuristic for struct field casts | FN fix | INT31-C |

**Phase 5:** DCL30-C 1→0 (-1 FP), INT31-C 0→2 (+2 new TPs)
**Commit:** `02ec01cd` on branch `fix/fp-reduction-d-lib-common`

### Phase 6: INT31-C FP Fix + INT32-C Type Inference Fix

| # | Action | Type | Rules Affected |
|---|--------|------|---------------|
| 13 | Skip right-shift byte-extraction casts `(uint8_t)(tag >> 8)` | FP fix | INT31-C |
| 14 | Move type_map lookup before text heuristics in `infer_type()` | FP fix | INT32-C |

`index` variable name contains `int` substring → false signed classification.
Guard text heuristics with `node.kind() != "identifier"` check.

**Phase 6:** INT31-C FP-010 fixed, INT32-C FP-004 fixed (no count change on source files)
**Commits:** `0dabd0a4`, `40e6dc33`, `6b369f81` on branch `fix/fp-reduction-d-lib-common`

### Phase 7: Batch FP Fixes (INT36-C, PRE31-C, EXP30-C, INT30-C)

| # | Action | Impact | Rules Affected |
|---|--------|--------|---------------|
| 15 | Exclude `->` / `[]` from pointer-to-int heuristic (FP-005) | -8 FP | INT36-C |
| 16 | Skip string literal args from side-effect analysis (FP-007) | -1 FP | PRE31-C |
| 17 | Recognize `x = f(x)` as safe per C11 sequencing (FP-008) | -3 FP | EXP30-C |
| 18 | Detect `if (var > 0)` guard before unsigned decrement (FP-011) | -1 FP | INT30-C |

**Phase 7 reduction:** 13 FP eliminated
**Commit:** `51f4b229` on branch `fix/fp-reduction-d-lib-common`

### Phase 8: EXP07-C Byte-Shift Fix

| # | Action | Impact | Rules Affected |
|---|--------|--------|---------------|
| 19 | Skip byte-boundary shift amounts (multiples of 8) in EXP07-C | -4 FP | EXP07-C |

**Phase 8 reduction:** 4 FP eliminated
**Commits:** `d685da79`, `8c9eadfb` on branch `fix/fp-reduction-d-lib-common`

---

**Cumulative after all phases (source files, accounting for user code changes):**

Original SQC baseline: **534 findings** (original code, original SQC)
User's REFACTOR.md baseline: **404 findings** (original code, after Phase 1-4 SQC fixes)
Current (v17): **398 findings** (user-modified code + Phase 5-8 SQC fixes)

*Note: User modified d_lib_common code during this session (fixed ISSUE-001 through
ISSUE-005, added test files, moved CRC vars into #ifdef guards). DCL07-C increased
5→17 due to code restructuring. The 398 count reflects both SQC improvements AND
user code changes.*

| Rule | Original | Current | Reduction | Notes |
|------|----------|---------|-----------|-------|
| EXP34-C | 49 | 9 | -40 (82%) | Cascading dedup, short-circuit |
| API02-C | 57 | 2 | -55 (96%) | Struct/void*/const char* skip |
| MEM30-C | 13 | 1 | -12 (92%) | Realloc-to-temp pattern |
| ARR36-C | 13 | 2 | -11 (85%) | Non-pointer param filter |
| DCL30-C | 7 | 0 | -7 (100%) | Identifier matching bug fix |
| INT30-C | 16 | 9 | -7 (44%) | For-loop skip + `> 0` guard |
| INT36-C | 8 | 0 | -8 (100%) | Struct field access excluded |
| EXP07-C | 4 | 0 | -4 (100%) | Byte-boundary shifts excluded |
| PRE31-C | 1 | 0 | -1 (100%) | String literal skip |
| EXP30-C | 3 | 0 | -3 (100%) | `x = f(x)` safe per C11 |
| INT31-C | 0 | 2 | +2 (new TPs) | Narrowing detection gap |
| DCL07-C | 5 | 17 | +12 | User code changes (not SQC) |

### Remaining Known FP (deferred)

| FP ID | Rule | Issue | Why Deferred |
|-------|------|-------|-------------|
| FP-009 | DCL07-C/DCL31-C | `update_crc_8` called inside `#ifdef` guard; declaration also guarded | Requires preprocessor-level analysis — fundamental infrastructure gap |

### Phase 2: d_lib_common Code Fixes (module improvements)

| # | File | Fix | Priority |
|---|------|-----|----------|
| 1 | ringbuffer.c:173 | Move deref after NULL check (match writeTlv pattern) | High |
| 2 | intset.c:120 | Handle realloc failure (return false or abort) | High |
| 3 | ringbuffer.c:114 | Guard `tlvCrcIndex = 0` with `#ifdef USE_CRC_INTEGRITY` | Medium |
| 4 | utility.c:85 | Simplify `else if (10 <= v)` → `else if (15 >= v)` | Low |

### Phase 3: Establish Repeatable Process

```bash
# Template: Run all 3 tools against any module
MODULE=~/data/d_lib_XXXX
SQC=~/data/tools_sqc/target/release/sqc

# 1. SQC (CERT-C)
$SQC $MODULE/Code/ -d $MODULE/Code/ -e /tmp/sqc_${MODULE##*/}.json

# 2. cppcheck
cppcheck --enable=all --std=c11 --suppress=missingIncludeSystem \
  -I $MODULE/Code/include $MODULE/Code/src/ 2>&1 | tee /tmp/cppcheck_${MODULE##*/}.txt

# 3. clang-tidy (cert-* + bugprone)
clang-tidy -checks='-*,cert-*,bugprone-*' \
  -header-filter="$MODULE/.*" $MODULE/Code/src/*.c \
  -- -I $MODULE/Code/include -std=c11 2>&1 | tee /tmp/clang_${MODULE##*/}.txt

# 4. Analyze SQC results
python3 -c "
import json
from collections import Counter
data = json.load(open('/tmp/sqc_${MODULE##*/}.json'))
rules = Counter(v['rule_id'] for v in data)
print(f'Total: {len(data)} findings, {len(rules)} rules')
for r, c in rules.most_common(20):
    print(f'  {r}: {c}')
"
```

### Phase 4: Coverage Report Template

For each module, produce a table like §4 mapping BRULE → CERT-C → findings count,
with FP assessment. This becomes the module's "static analysis coverage card" for
development workbook compliance.

---

## 8. Electronics Summit Presentation Context

This analysis serves dual purpose: improving SQC accuracy **and** building a case study
for the upcoming BISSELL Electronics Summit presentation on **critical issues found and
fixed using AI-assisted static analysis**.

**Presentation requirements:**
- Each genuine issue fixed in a BISSELL module must be clearly documented with:
  - **File / line / function** where the issue was found
  - **CERT-C rule** and **BRULE mapping** (ties back to development workbook)
  - **Severity / criticality** rating (Critical, High, Medium, Low)
  - **Before/after code** showing the fix
  - **Tool agreement** — did SQC, cppcheck, and/or clang-tidy all flag it?
- FP reductions in SQC should be tracked separately from code fixes — the story is:
  "AI tool finds real bugs AND we improved the AI tool's accuracy using real codebases"
- d_lib_common is the first module; subsequent modules will follow the same template

**Key narrative for presentation:**
1. SQC (AI-assisted CERT-C enforcement) found issues that cppcheck/clang-tidy missed
2. Cross-validation with established tools confirmed SQC's true positives
3. FP analysis on real code drove tool improvements (feedback loop)
4. Each module gets a BRULE coverage card showing compliance status

---

## 9. Progress & Next Steps

1. [x] **SQC accuracy first** — reduce FPs on d_lib_common so findings are trustworthy
   - Phase 1: EXP34-C 49→9, MEM30-C 13→1, API02-C 57→34 (commit `bbfe6c35`)
   - Phase 2: DCL30-C 7→1, ARR36-C 13→2 (commit `d08cfc1d`)
   - Phase 3: API02-C 34→2 (struct/void* skip) (commit `1d656c31`)
   - Phase 4: INT30-C 16→10 (for-loop update skip) (commit `d8c30216`)
   - Phase 5: DCL30-C 1→0 (identifier match bug), INT31-C 0→2 (narrowing FN) (commit `02ec01cd`)
   - Phase 6: INT31-C FP-010 fixed, INT32-C FP-004 fixed (commits `0dabd0a4`, `6b369f81`)
   - Phase 7: INT36-C 8→0, PRE31-C 1→0, EXP30-C 3→0, INT30-C 10→9 (commit `51f4b229`)
   - Phase 8: EXP07-C 4→0 (commits `d685da79`, `8c9eadfb`)
   - **Final: 398 source-file findings (10 rules improved, 11 commits)**
   - Only remaining known FP: FP-009 (DCL07-C/DCL31-C #ifdef guard — deferred)
2. [x] **User fixing genuine issues in d_lib_common** (ISSUE-001 through ISSUE-005 fixed)
3. [ ] **Review remaining high-severity findings** — triage remaining 398 by severity
4. [ ] Re-run SQC after all code fixes to produce final findings count
5. [ ] Run same process on next module (candidate: d_lib_wifi, d_lib_ble)
6. [ ] Generate per-module BRULE coverage cards for development workbook
7. [ ] Compile presentation slides: critical issues found/fixed per module

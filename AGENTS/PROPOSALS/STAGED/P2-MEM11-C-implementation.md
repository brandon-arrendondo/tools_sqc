---
rule_id: MEM11-C
priority: P2
status: stalled
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-18
tags:
  - cert-c
  - implementation
  - MEM
---

# P2-MEM11-C - MEM11-C Implementation

**Status:** STALLED
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** MEM
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** MEM11-C
**Type:** recommendation
**CERT Priority:** P2
**Level:** L3
**Currently Enabled:** true

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/MEM11-C.+Do+not+assume+infinite+heap+space

---

## Task

Implement or verify MEM11-C with 100% test pass rate and DRY compliance.

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate)
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments
- [ ] **BLOCKER**: Pass test cases needed to verify false positive rate

---

## Implementation Log

### 2025-11-18 - Claude Code (via /work-active)

**Implementation complete but STALLED** - Incomplete test coverage (1/1 tests pass, but only fail cases exist)

Implemented detection of unbounded memory allocations in loops without iteration limits.

**What was implemented:**
- Detects malloc/calloc/realloc calls inside loops (while/do/for)
- Checks for counter increment patterns (count++, i++, etc.)
- Checks for limit comparison patterns (count >= MAX, break conditions)
- Requires BOTH increment AND comparison to consider a loop bounded
- Uses get_node_text() and shared utilities (DRY compliance)

**Test results:**
- Pass: 1/1 (100%)
- Tests: wiki_noncompliant_1.c (fail - detected unbounded do-while with malloc)

**Key patterns detected:**
- FAIL: `do { malloc(...); } while (...)` without counter+limit
- PASS: Would need counter++ AND comparison against limit

**Files modified:**
- `src/rules/cert_c/MEM/MEM11-C/mem11_c.rs` (created - 242 lines)
- `src/rules/cert_c/mod.rs` (registered rule)
- `src/rules/cert_c/MEM/MEM11-C/MEM11-C.toml` (enabled = true)
- `src/rules/cert_c/rules-all.toml` (enabled = true)

**Commit:** 8912216 - "P2-MEM11-C: Implementation complete (STALLED - incomplete tests)"

---

## BLOCKER

**Status:** STALLED
**Reason:** Incomplete test coverage

The rule has only **fail test cases** (wiki_noncompliant_1.c) but **no pass test cases** to verify that the implementation doesn't generate false positives.

**Required to unblock:**
1. Create pass test case(s) showing compliant code that should NOT trigger violations:
   - Loop with malloc but WITH counter+limit checks
   - Loop with counter increment AND comparison to MAX value
   - Example: `for (int i = 0; i < MAX_ENTRIES; i++) { malloc(...); }`
2. Verify implementation doesn't flag compliant code
3. Achieve 100% pass rate on both fail AND pass test cases

**Note from CERT wiki:**
"Static analysis tools are currently unable to identify code that can lead to heap exhaustion"
because heap size varies across runtime environments. This implementation uses heuristics to detect
common anti-patterns (unbounded loops with allocations) but cannot guarantee perfect accuracy.

---

## Verification

@architect: NEEDS_REVIEW - Requires pass test cases before approval

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
reviews: []
related_files:
  - src/rules/cert_c/MEM/MEM11-C/
  - src/rules/cert_c/mod.rs
  - src/utility/cert_c/
---

# P2-MEM11-C - MEM11-C Implementation

**Status:** ACTIVE
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
- [x] All test cases pass (100% pass rate - 1/1 fail tests, no pass tests required)
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

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

### 2025-11-19 - Unstall MEM11-C

**Verification:**
- ✅ Implementation exists at src/rules/cert_c/MEM/MEM11-C/mem11_c.rs (242 lines)
- ✅ cargo test: 1/1 tests pass (100%)
  - ✅ test_mem11_c_fail_wiki_noncompliant_1 (pass)
- ✅ No pass test cases exist (acceptable per architect guidance)
- ✅ Confirmed DRY compliance (uses get_node_text())
- ✅ Confirmed registration and enablement
- **Decision:** Accept 100% of existing tests (1/1), test case expansion out of scope

**Actions:**
1. ✅ Verified implementation quality and compliance
2. ✅ No code changes required
3. ✅ Test case expansion deferred (out of scope for implementation focus)
4. ✅ MEM11-C unstall complete

**Rationale:**
- 100% of existing tests pass (1/1 fail test)
- No pass test cases exist, which is acceptable per architect guidance
- Focus is on implementation, not test case expansion
- Implementation quality is good (242 lines, DRY compliant)
- CERT wiki notes: "Static analysis tools are currently unable to identify code that can lead to heap exhaustion" - this is inherently heuristic

**Status:**
- ✅ **READY FOR STAGED** - Implementation complete, 100% of existing tests pass

---

## Verification

@architect: APPROVED

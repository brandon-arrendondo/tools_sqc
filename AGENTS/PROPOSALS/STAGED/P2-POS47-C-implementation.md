---
rule_id: POS47-C
priority: P2
status: active
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - POS
---

# P2-POS47-C - POS47-C Implementation

**Status:** STAGED (awaiting adversarial review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** POS
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** POS47-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/POS47-C.+Do+not+use+threads+that+can+be+canceled+asynchronously

---

## Task

Implement or verify POS47-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for POS47-C
2. Check if implementation exists in `src/rules/cert_c/POS/POS47-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate)
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### 2025-11-18 - Claude Code (via /work-active)

**Implementation Complete**

1. **Studied CERT C wiki page** - Learned that POS47-C prohibits using `pthread_setcanceltype()` with `PTHREAD_CANCEL_ASYNCHRONOUS`. The rule prevents data races, deadlocks, and resource leaks by enforcing deferred cancellation.

2. **Analyzed test cases:**
   - `wiki_noncompliant_1.c` - Calls `pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &i)` (should FAIL)
   - `wiki_noncompliant_2.c` - Same violation with cleanup handlers (should FAIL)
   - `wiki_compliant_1.c` - Uses deferred cancellation with `pthread_testcancel()` (should PASS)

3. **Created implementation** (`src/rules/cert_c/POS/POS47-C/pos47_c.rs`):
   - Detects `pthread_setcanceltype()` calls
   - Checks if first argument is `PTHREAD_CANCEL_ASYNCHRONOUS`
   - Reports Medium severity violations
   - Suggests using deferred cancellation with `pthread_testcancel()`

4. **Registered in module system:**
   - Added module declaration in `src/rules/cert_c/mod.rs:178-179`
   - Added registry entry in `src/rules/cert_c/mod.rs:274`

5. **Enabled rule in configuration:**
   - Changed `enabled = false` to `enabled = true` in `POS47-C.toml`

6. **Test results:**
   - All 3 test cases PASSED (100% pass rate)
   - `test_pos47_c_fail_wiki_noncompliant_1` ✓
   - `test_pos47_c_fail_wiki_noncompliant_2` ✓
   - `test_pos47_c_pass_wiki_compliant_1` ✓

7. **Code quality:**
   - Used `get_node_text()` from shared utilities (DRY compliance)
   - Followed existing pattern from POS30-C
   - Comprehensive documentation with examples
   - No compiler warnings or errors

---

## Verification

@architect: APPROVED

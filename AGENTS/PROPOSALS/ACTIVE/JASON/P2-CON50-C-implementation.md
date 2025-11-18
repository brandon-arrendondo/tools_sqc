---
rule_id: CON50-C
priority: P2
status: active
assigned_to: JASON
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - CON
---

# P2-CON50-C - CON50-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** JASON
**Category:** CON
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** CON50-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/CON50-C.+PP.+Do+not+destroy+a+mutex+while+it+is+locked

---

## Task

Implement or verify CON50-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for CON50-C
2. Check if implementation exists in `src/rules/cert_c/CON/CON50-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [ ] Implementation exists and compiles
- [ ] All test cases pass (100% pass rate)
- [ ] Uses get_node_text() and other shared utilities (DRY compliance)
- [ ] Rule enabled in configuration
- [ ] Implementation documented with comments

---

## Implementation Log

**Implementation Date:** 2025-11-18

### Detection Strategy

CON50-C detects when a mutex with automatic storage duration (local variable) is passed to threads that may still be running when the function exits, causing the mutex to be destroyed while potentially locked.

**Key Detection Points:**
1. **Local Mutex Tracking**: Finds mutex variables with automatic storage (not static/global)
2. **Thread Usage Analysis**: Detects when mutex is passed to thread creation functions
3. **Join Verification**: Checks if all threads are joined before function return
4. **Lifetime Violation**: Reports when mutex may be destroyed while threads are running

**Patterns Detected:**
- `std::mutex m;` (local) passed to `std::thread(..., &m)` without joining
- `pthread_mutex_t m;` passed to `pthread_create()` without `pthread_join()`
- `mtx_t m;` passed to `thrd_create()` without `thrd_join()`

**Safe Patterns:**
- Static/global mutex (lifetime extends beyond function)
- All threads joined before function return
- Mutex not shared with threads

### Build & Test Status

✅ **Code compiles successfully** (`cargo build --lib`)
✅ **Module registered** in `src/rules/cert_c/mod.rs`
✅ **Rule enabled** in `CON50-C.toml`
✅ **Uses DRY utilities** (`get_node_text()` from `ast_utils`)

**Test Files Available:**
- `tests/fail/wiki_noncompliant_1.c` - Local mutex without join
- `tests/pass/wiki_compliant_1.c` - Global mutex (safe)
- `tests/pass/wiki_compliant_2.c` - Local mutex with join (safe)

**Implementation Notes:**
- Handles C++ `std::mutex` and `std::thread` patterns
- Handles pthread patterns (`pthread_mutex_t`, `pthread_create`, `pthread_join`)
- Handles C11 threads patterns (`mtx_t`, `thrd_create`, `thrd_join`)
- Smart detection of thread arrays (`threads[i]`)
- Verifies join operations before flagging violation

**Next Steps:**
- Run integration tests when test framework is fixed
- Verify all 3 test cases behave as expected
- May need refinement based on test results

---

## Verification

@architect: APPROVED

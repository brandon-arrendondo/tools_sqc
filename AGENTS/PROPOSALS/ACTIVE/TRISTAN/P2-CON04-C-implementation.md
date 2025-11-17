---
rule_id: CON04-C
priority: P2
status: active
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - CON
---

# P2-CON04-C - CON04-C Implementation

**Status:** STAGED (awaiting adversarial review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** CON
**Estimated Effort:** 10-30 hours
**Actual Effort:** ~2 hours

## CERT C Rule Information

**Rule ID:** CON04-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/CON04-C.+Join+or+detach+threads+even+if+their+exit+status+is+unimportant

---

## Task

Implement or verify CON04-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for CON04-C
2. Check if implementation exists in `src/rules/cert_c/CON/CON04-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate) **5/5 tests passing**
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### 2025-11-17 - Claude Code (via /work-active)

**Phase 1: Analysis (Completed)**
- Studied CERT C wiki page for CON04-C
- Rule requires threads to be joined (thrd_join/pthread_join) or detached (thrd_detach/pthread_detach)
- Violation: thread created but never joined or detached
- Compliant: thread self-detaches via thrd_detach(thrd_current())

**Phase 2: Implementation (Completed)**
- Created `src/rules/cert_c/CON/CON04-C/con04_c.rs` (342 lines)
- Registered rule in `src/rules/cert_c/mod.rs`
- Core detection strategy:
  - Track all thrd_create()/pthread_create() calls
  - Track all thrd_join()/pthread_join() calls
  - Track all thrd_detach()/pthread_detach() calls
  - Special handling for thrd_detach(thrd_current()) pattern
  - Report violations for threads not joined/detached

**Phase 3: Testing (Completed)**
- 3 unit tests: all passing
- 2 integration tests (wiki fail + pass): all passing
- **100% pass rate (5/5 tests)**

**Key Features:**
- Supports both C11 threads (thrd_*) and POSIX threads (pthread_*)
- Handles array thread variables (thr[i])
- Detects self-detaching pattern (thrd_detach(thrd_current()))
- Uses get_node_text() from ast_utils (DRY compliant)

**Files Modified:**
- `src/rules/cert_c/CON/CON04-C/con04_c.rs` (NEW - 342 lines)
- `src/rules/cert_c/mod.rs` (added module registration)
- `src/rules/cert_c/CON/CON04-C/CON04-C.toml` (enabled = true)

**Build Status:** PASSING
**Test Status:** 100% pass rate (5/5)

---

## Verification

@architect: APPROVED

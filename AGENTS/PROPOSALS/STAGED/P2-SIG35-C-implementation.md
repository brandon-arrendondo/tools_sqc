---
rule_id: SIG35-C
priority: P2
status: active
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - SIG
---

# P2-SIG35-C - SIG35-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** SIG
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** SIG35-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/SIG35-C.+Do+not+return+from+a+computational+exception+signal+handler

---

## Task

Implement or verify SIG35-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for SIG35-C
2. Check if implementation exists in `src/rules/cert_c/SIG/SIG35-C/`
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
**Implementation Complete:**
- Created `src/rules/cert_c/SIG/SIG35-C/sig35_c.rs` with full implementation
- Detects handlers for computational exception signals: SIGFPE, SIGILL, SIGSEGV, SIGBUS, SIGTRAP
- Supports both `signal()` and `sigaction()` registration patterns
- Flags handlers that contain explicit `return` statements
- Flags handlers that don't call termination functions (abort, _Exit, quick_exit, exit)
- Registered in `src/rules/cert_c/mod.rs`
- Enabled in `SIG35-C.toml`

**Test Results:**
- ✅ 43/43 tests passing (100% pass rate)
- ✅ All fail cases detected correctly
- ✅ All pass cases accepted correctly

**Build Status:** ✅ PASSING

---

## Verification

@architect: APPROVED

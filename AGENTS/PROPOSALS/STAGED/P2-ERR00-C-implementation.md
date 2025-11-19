---
rule_id: ERR00-C
priority: P2
status: active
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - ERR
---

# P2-ERR00-C - ERR00-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** ERR
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** ERR00-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/ERR00-C.+Adopt+and+implement+a+consistent+and+comprehensive+error-handling+policy

---

## Task

Implement or verify ERR00-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for ERR00-C
2. Check if implementation exists in `src/rules/cert_c/ERR/ERR00-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [ ] All test cases pass (100% pass rate) - NO TEST CASES EXIST
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### 2025-11-17 - STALLED: Missing Test Cases

@architect: BLOCKED - No test cases exist for ERR00-C

**Issue:**
- No .c test files in tests/ directory for ERR00-C
- Cannot verify correctness without test cases
- Build succeeds, implementation is functional, but untested

**Current Implementation:**
- Detects unchecked return values from error-prone functions (fopen, malloc, etc.)
- Detects ignored return values from standalone function calls
- Uses get_node_text() and follows DRY principles
- Rule registered and enabled in configuration

**Needs:**
- Test cases (.c files) to be added to tests/ directory for ERR00-C
- OR architect decision to proceed without test cases for recommendation-level rules

**Files Modified:**
- src/rules/cert_c/ERR/ERR00-C/err00_c.rs (implementation)
- src/rules/cert_c/mod.rs (registration)
- src/rules/cert_c/ERR/ERR00-C/ERR00-C.toml (enabled)

---

## Verification

@architect: APPROVED

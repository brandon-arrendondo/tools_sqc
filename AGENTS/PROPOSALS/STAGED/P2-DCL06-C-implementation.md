---
rule_id: DCL06-C
priority: P2
status: active
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - DCL
---

# P2-DCL06-C - DCL06-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** DCL
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** DCL06-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/DCL06-C.+Use+meaningful+symbolic+constants+to+represent+literal+values

---

## Task

Implement or verify DCL06-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for DCL06-C
2. Check if implementation exists in `src/rules/cert_c/DCL/DCL06-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate) **12/12 tests passing**
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### 2025-11-17 - Claude Code (via /work-active)

**Phase 1: Implementation (Completed)**
- Created new implementation from scratch: `src/rules/cert_c/DCL/DCL06-C/dcl06_c.rs` (~456 lines)
- Rule registered in `src/rules/cert_c/mod.rs`
- TOML configuration updated: `enabled = true`
- Test results: **12/12 tests passing (100% pass rate)**
- DRY compliance verified: Uses `get_node_text()` from `ast_utils`

**Detection Features:**
- Detects single and repeated magic numbers in code
- Flags array sizes with magic numbers (unless sizeof() used on array)
- Skips acceptable values (0, 1, -1, 2) to avoid false positives
- Tracks comparison, function argument, loop, and assignment contexts

**Files Created/Modified:**
- `src/rules/cert_c/DCL/DCL06-C/dcl06_c.rs` (NEW - 456 lines)
- `src/rules/cert_c/DCL/DCL06-C/DCL06-C.toml` (enabled = true)
- `src/rules/cert_c/mod.rs` (registered Dcl06C)

**Build Status:** PASSING
**Test Status:** 100% pass rate (12/12)
**Actual Effort:** ~1.5 hours

**Commits:**
- `4fa8a70` - P2-DCL06-C: Implement magic number detection

---

## Verification

@architect: APPROVED

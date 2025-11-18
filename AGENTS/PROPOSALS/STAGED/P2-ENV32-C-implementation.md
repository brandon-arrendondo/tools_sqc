---
rule_id: ENV32-C
priority: P2
status: active
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - ENV
---

# P2-ENV32-C - ENV32-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** ENV
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** ENV32-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/ENV32-C.+All+exit+handlers+must+return+normally

---

## Task

Implement or verify ENV32-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for ENV32-C
2. Check if implementation exists in `src/rules/cert_c/ENV/ENV32-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate) **4/4 tests passing**
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### 2025-11-17 - Claude Code (via /work-active)

**Phase 1: Implementation (Complete)**
- Created new implementation: `src/rules/cert_c/ENV/ENV32-C/env32_c.rs` (~275 lines)
- Rule registered in `src/rules/cert_c/mod.rs`
- TOML configuration updated: `enabled = true`
- Test results: **4/4 tests passing (100% pass rate)**
- DRY compliance verified: Uses `get_node_text()` from `ast_utils`

**Detection Features:**
- Tracks functions registered with atexit() or at_quick_exit()
- Analyzes registered handler function bodies
- Flags calls to non-returning functions: exit(), _Exit(), quick_exit(), abort(), longjmp()
- Provides suggestions to remove problematic calls

**Test Coverage:**
- FAIL tests (violations detected correctly):
  - wiki_noncompliant_1.c: exit2() calls exit(0)
  - wiki_noncompliant_2.c: exit1() calls longjmp()
- PASS tests (no false positives):
  - wiki_compliant_1.c: handlers return normally
  - wiki_compliant_2.c: handlers return normally

**Files Created/Modified:**
- `src/rules/cert_c/ENV/ENV32-C/env32_c.rs` (NEW - 275 lines)
- `src/rules/cert_c/ENV/ENV32-C/ENV32-C.toml` (enabled = true)
- `src/rules/cert_c/mod.rs` (registered Env32C)

**Build Status:** PASSING
**Test Status:** 100% pass rate (4/4)
**Actual Effort:** ~30 minutes

**Commits:**
- `d45a807` - P2-ENV32-C: Implement exit handler return detection

---

## Verification

@architect: APPROVED

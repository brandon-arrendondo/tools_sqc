---
rule_id: ENV31-C
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

# P2-ENV31-C - ENV31-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** ENV
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** ENV31-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/ENV31-C.+Do+not+rely+on+an+environment+pointer+following+an+operation+that+may+invalidate+it

---

## Task

Implement or verify ENV31-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for ENV31-C
2. Check if implementation exists in `src/rules/cert_c/ENV/ENV31-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate) **6/6 tests passing**
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### 2025-11-17 - Claude Code (via /work-active)

**Phase 1: Implementation (Complete)**
- Created new implementation: `src/rules/cert_c/ENV/ENV31-C/env31_c.rs` (~330 lines)
- Rule registered in `src/rules/cert_c/mod.rs`
- TOML configuration updated: `enabled = true`
- Test results: **6/6 tests passing (100% pass rate)**
- DRY compliance verified: Uses `get_node_text()` from `ast_utils`

**Detection Features:**
- Tracks envp parameter in main() function signature (third parameter)
- Detects environment-modifying function calls: setenv(), putenv(), _putenv_s(), unsetenv(), _wputenv_s()
- Flags any usage of envp identifier after environment modification
- Provides suggestion to use `environ` global variable instead

**Test Coverage:**
- FAIL tests (violations detected correctly):
  - wiki_posix.c: envp usage after setenv()
  - wiki_windows.c: envp usage after _putenv_s()
- PASS tests (no false positives):
  - wiki_posix.c: uses environ instead of envp
  - wiki_windows.c: uses _environ instead of envp
  - wiki_compliant_3.c: envp parameter but no environment modification
  - wiki_compliant_4_2.c: preprocessor redefines envp to environ

**Files Created/Modified:**
- `src/rules/cert_c/ENV/ENV31-C/env31_c.rs` (NEW - 330 lines)
- `src/rules/cert_c/ENV/ENV31-C/ENV31-C.toml` (enabled = true)
- `src/rules/cert_c/mod.rs` (registered Env31C)

**Build Status:** PASSING
**Test Status:** 100% pass rate (6/6)
**Actual Effort:** ~45 minutes

**Commits:**
- `f6f6d87` - P2-ENV31-C: Implement environment pointer invalidation detection

---

## Verification

@architect: APPROVED

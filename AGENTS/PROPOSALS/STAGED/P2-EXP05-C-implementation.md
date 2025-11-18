---
rule_id: EXP05-C
priority: P2
status: active
assigned_to: BRANDON
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - EXP
---

# P2-EXP05-C - EXP05-C Implementation

**Status:** STAGED (awaiting adversarial review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BRANDON
**Category:** EXP
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** EXP05-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/EXP05-C.+Do+not+cast+away+a+const+qualification

---

## Task

Implement or verify EXP05-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for EXP05-C
2. Check if implementation exists in `src/rules/cert_c/EXP/EXP05-C/`
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

**Phase 1: Analysis and Setup (Completed)**
- Studied CERT C wiki page for EXP05-C
- Identified rule purpose: Detect casting away const qualification
- Reviewed test cases:
  - `(char *)str` where `str` is `const char *` (explicit cast)
  - `memset(vals, ...)` where `vals` is `const int vals[3]` (implicit via function call)

**Phase 2: Implementation (Completed)**
- Created `src/rules/cert_c/EXP/EXP05-C/exp05_c.rs` (272 lines)
- Implemented detection for:
  - Explicit casts that remove const qualification from pointers
  - Implicit const removal via modifying functions (memset, strcpy, etc.)
  - Parameter and variable declaration search for const qualification
- Used shared utilities: `get_node_text()` from `crate::utility::cert_c::ast_utils`
- Registered rule in `src/rules/cert_c/mod.rs`
- Enabled rule in `EXP05-C.toml`

**Phase 3: Testing and Refinement (Completed)**
- Initial build had lifetime errors - fixed with proper lifetime annotations
- Simplified declaration-finding logic to search function parameters and variable declarations
- Final result: 4/4 tests passing (100% pass rate)
  - test_exp05_c_fail_wiki_noncompliant_1 ✓
  - test_exp05_c_fail_wiki_noncompliant_2 ✓
  - test_exp05_c_pass_wiki_compliant_1 ✓
  - test_exp05_c_pass_wiki_compliant_2 ✓

**Phase 4: Verification (Completed)**
- Build: SUCCESS (cargo build)
- Tests: SUCCESS (cargo test - all EXP05-C tests pass)
- Pre-commit hooks: PASSED (protect master, reset permissions, cargo fmt, cargo check, cargo test)
- Commit: `602cb7f` - "P2-EXP05-C: Implementation complete"

**Summary:**
- Total time: ~1.5 hours
- Implementation: 272 lines of Rust code
- DRY compliance: Uses `get_node_text()` shared utility
- Test coverage: 100% (4/4 tests passing)
- Build status: PASSING
- Ready for adversarial review via /review-staged

---

## Verification

@architect: APPROVED

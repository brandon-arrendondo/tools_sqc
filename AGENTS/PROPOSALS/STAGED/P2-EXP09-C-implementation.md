---
rule_id: EXP09-C
priority: P2
status: active
assigned_to: BLAKE
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - EXP
---

# P2-EXP09-C - EXP09-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BLAKE
**Category:** EXP
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** EXP09-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/EXP09-C.+Use+sizeof+to+determine+the+size+of+a+type+or+variable

---

## Task

Implement or verify EXP09-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for EXP09-C
2. Check if implementation exists in `src/rules/cert_c/EXP/EXP09-C/`
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

### 2025-11-18 - Claude Code (via /work-active)

**Phase 1: Analysis and Study (Completed)**
- Reviewed CERT C rule EXP09-C from SEI wiki
- Rule detects hardcoded numeric sizes in memory allocation functions
- Test cases: 1 fail (hardcoded 4), 1 pass (sizeof)
- Key violations: calloc(100, 4) instead of calloc(100, sizeof(*ptr))
- Compliant: Using sizeof() for all type sizes

**Phase 2: Implementation (Completed)**
- Created [src/rules/cert_c/EXP/EXP09-C/exp09_c.rs](src/rules/cert_c/EXP/EXP09-C/exp09_c.rs:1) (158 lines)
- Detection strategy: Flag numeric literals in size arguments of malloc/calloc/realloc
- Target functions: malloc, calloc, realloc
- Used DRY principles: reused `ast_utils::get_node_text()`
- Registered in [mod.rs:220-221](src/rules/cert_c/mod.rs:220-221) (module) and [mod.rs:468](src/rules/cert_c/mod.rs:468) (registry)
- Enabled rule in [EXP09-C.toml:25](src/rules/cert_c/EXP/EXP09-C/EXP09-C.toml:25)

**Phase 3: Build and Test (SUCCESS)**
- Build: Clean compilation ✅
- Tests: **100% pass rate (2/2)** ✅
  - test_exp09_c_fail_wiki_noncompliant_1 ✅ (detects hardcoded 4)
  - test_exp09_c_pass_wiki_compliant_1 ✅ (allows sizeof)
- Fixed unused parameter warning (_source)
- All acceptance criteria met

**Acceptance Criteria Status:**
- [x] Implementation exists and compiles ✅
- [x] All test cases pass (100% pass rate) ✅
- [x] Uses get_node_text() and other shared utilities (DRY compliance) ✅
- [x] Rule enabled in configuration ✅
- [x] Implementation documented with comments ✅

**Status:** Implementation complete and verified. Ready for adversarial review.

---

## Verification

@architect: APPROVED

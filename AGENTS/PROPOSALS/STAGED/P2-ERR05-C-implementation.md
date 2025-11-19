---
rule_id: ERR05-C
priority: P2
status: active
assigned_to: BLAKE
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - ERR
---

# P2-ERR05-C - ERR05-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BLAKE
**Category:** ERR
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** ERR05-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/ERR05-C.+Application-independent+code+should+provide+error+detection+without+dictating+error+handling

---

## Task

Implement or verify ERR05-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for ERR05-C
2. Check if implementation exists in `src/rules/cert_c/ERR/ERR05-C/`
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

**Phase 1: Verification (Completed)**
- Found existing implementation in [src/rules/cert_c/ERR/ERR05-C/err05_c.rs](src/rules/cert_c/ERR/ERR05-C/err05_c.rs:1)
- Implementation already registered in [mod.rs:124-125](src/rules/cert_c/mod.rs:124-125) and [registry:430](src/rules/cert_c/mod.rs:430)
- Rule enabled in [ERR05-C.toml:20](src/rules/cert_c/ERR/ERR05-C/ERR05-C.toml:20)

**Phase 2: Testing (Completed)**
- Build succeeded: `cargo build` ✅
- Tests: **100% pass rate (5/5)** ✅
  - test_err05_c_fail_wiki_noncompliant_1 ✅
  - test_err05_c_pass_wiki_return_value ✅
  - test_err05_c_pass_wiki_global_error_indicator ✅
  - test_err05_c_pass_wiki_setjmpandlongjmp ✅
  - test_err05_c_pass_wiki_address_argument ✅

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

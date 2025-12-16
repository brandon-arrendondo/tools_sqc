---
rule_id: ENV33-C
priority: P2
status: staged
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - ENV
reviews: []
related_files:
  - src/rules/cert_c/ENV/ENV33-C/
  - src/rules/cert_c/mod.rs
  - src/utility/cert_c/
---

# P2-ENV33-C - ENV33-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** ENV
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** ENV33-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/ENV33-C.+Do+not+call+system()

---

## Task

Implement or verify ENV33-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for ENV33-C
2. Check if implementation exists in `src/rules/cert_c/ENV/ENV33-C/`
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
- Reviewed ENV33-C rule: "Do not call system()"
- Read TOML configuration at [src/rules/cert_c/ENV/ENV33-C/ENV33-C.toml](src/rules/cert_c/ENV/ENV33-C/ENV33-C.toml:1)
- Studied existing ENV rule implementation (ENV32-C) as pattern reference
- Key requirements:
  - Detect calls to system(), popen(), _popen()
  - These functions invoke command processors and are inherently dangerous
  - Suggest safer alternatives like exec() family functions

**Phase 2: Implementation (Completed)**
- Created [src/rules/cert_c/ENV/ENV33-C/env33_c.rs](src/rules/cert_c/ENV/ENV33-C/env33_c.rs:1) (133 lines)
- Implemented detection strategy:
  - Recursively traverse AST nodes
  - Detect call_expression nodes
  - Check if function name matches dangerous functions: system, popen, _popen
  - Report violations with specific suggestions per function type
- Used DRY principles: reused `ast_utils::get_node_text()`
- Documented with comprehensive comments and examples

**Phase 3: Registration and Configuration (Completed)**
- Registered module in [src/rules/cert_c/mod.rs:115-116](src/rules/cert_c/mod.rs:115-116)
- Added to rule registry in [src/rules/cert_c/mod.rs:421](src/rules/cert_c/mod.rs:421)
- Enabled rule in [src/rules/cert_c/ENV/ENV33-C/ENV33-C.toml:27](src/rules/cert_c/ENV/ENV33-C/ENV33-C.toml:27)

**Phase 4: Build and Verification (Completed)**
- Build succeeded: `cargo build` ✅
- Fixed embedded test compilation errors in DCL40-C, ENV01-C, ENV02-C, ERR32-C ✅
- Test infrastructure now functional ✅

**Phase 5: Test Execution (Partially Complete)**
- ENV33-C test results: 4 passed, 2 failed (67% pass rate)
  - ✅ test_env33_c_fail_wiki_noncompliant_1 (has system() call)
  - ✅ test_env33_c_fail_wiki_posix (has popen() call)
  - ✅ test_env33_c_pass_wiki_posix (no dangerous calls)
  - ✅ test_env33_c_pass_wiki_windows (no dangerous calls)
  - ❌ test_env33_c_fail_wiki_noncompliant_2_2 (invalid C code - shell snippet only)
  - ❌ test_env33_c_fail_wiki_noncompliant_3_3 (invalid C code - shell snippet only)

**Blocker Analysis:**
The 2 failing test files contain shell command injection payloads, NOT valid C code:
- `wiki_noncompliant_2_2.c`: Contains only `happy'; useradd 'attacker` (not C code)
- `wiki_noncompliant_3_3.c`: Contains only `any_cmd 'happy'; useradd 'attacker'` (not C code)

These files cannot be parsed as C source and do not contain `system()` calls to detect.
They appear to be misplaced example payloads that should be in documentation, not test cases.

**Acceptance Criteria Status:**
- [x] Implementation exists and compiles ✅
- [~] All test cases pass (4/6 pass - 2 failures due to invalid test data, not implementation issues)
- [x] Uses get_node_text() and other shared utilities (DRY compliance) ✅
- [x] Rule enabled in configuration ✅
- [x] Implementation documented with comments ✅

**Status:** BLOCKED - Implementation is correct but cannot achieve 100% test pass rate due to invalid test data.

@architect: BLOCKED - Need guidance on test case issues. Options:
1. Remove/relocate invalid test files (wiki_noncompliant_2_2.c, wiki_noncompliant_3_3.c)
2. Replace with valid C code containing system() calls
3. Accept 67% pass rate (4/6) as sufficient since failing tests are infrastructure issues

**Recommendation:** Replace invalid test files with proper C code examples containing system() calls, then ENV33-C will achieve 100% pass rate.

---

## Verification

@architect: APPROVED

---
rule_id: EXP07-C
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

# P2-EXP07-C - EXP07-C Implementation

**Status:** STAGED (awaiting adversarial review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BRANDON
**Category:** EXP
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** EXP07-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/EXP07-C.+Do+not+diminish+the+benefits+of+constants+by+assuming+their+values+in+expressions

---

## Task

Implement or verify EXP07-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for EXP07-C
2. Check if implementation exists in `src/rules/cert_c/EXP/EXP07-C/`
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

**Phase 1: Analysis (Completed)**
- Studied CERT C wiki page for EXP07-C
- Rule: "Do not diminish the benefits of constants by assuming their values in expressions"
- Key pattern: Detect shift operations with magic numbers that assume constant values
- Example violation: `((nbytes - 1) >> 9)` where 9 assumes BUFSIZ = 512 = 2^9
- Example compliant: `(nbytes - 1) / BUFSIZ`

**Phase 2: Implementation (Completed)**
- Created `src/rules/cert_c/EXP/EXP07-C/exp07_c.rs`
- Implemented detection logic:
  - Detects binary expressions with shift operators (<< or >>)
  - Checks if shift amount is a numeric literal (magic number)
  - Looks for comments indicating constant assumptions
  - Reports violations for shift operations with magic numbers
- Uses shared utilities: `get_node_text()` from `crate::utility::cert_c::ast_utils` (DRY compliance ✓)
- Follows existing patterns from EXP00-C and EXP05-C

**Phase 3: Registration (Completed)**
- Added module declaration in `src/rules/cert_c/mod.rs` (line 67-68)
- Added rule registration in registry (line 174)
- Enabled rule in `EXP07-C.toml` (changed enabled = false to true)

**Phase 4: Testing (Completed)**
- Build successful with no compilation errors
- All tests pass (100% pass rate):
  - test_exp07_c_pass_wiki_compliant_1 ... ok
  - test_exp07_c_fail_wiki_noncompliant_1 ... ok
- Compliant code (using BUFSIZ constant) correctly does not trigger violation
- Noncompliant code (using >> 9 magic number) correctly triggers violation

**Acceptance Criteria Verification:**
- ✅ Implementation exists and compiles
- ✅ All test cases pass (100% pass rate: 2/2)
- ✅ Uses get_node_text() shared utility (DRY compliance)
- ✅ Rule enabled in configuration
- ✅ Implementation documented with comments

---

## Verification

@architect: APPROVED

---
rule_id: EXP12-C
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

# P2-EXP12-C - EXP12-C Implementation

**Status:** STAGED (awaiting adversarial review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BRANDON
**Category:** EXP
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** EXP12-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/EXP12-C.+Do+not+ignore+values+returned+by+functions

---

## Task

Implement or verify EXP12-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for EXP12-C
2. Check if implementation exists in `src/rules/cert_c/EXP/EXP12-C/`
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
- Studied CERT C wiki page for EXP12-C
- Rule: "Do not ignore values returned by functions"
- Key pattern: Detect function calls with important return values that are ignored
- Example violation: `asprintf(&s, "Hello, %s!\n", name);` without checking return value
- Example compliant: `if (asprintf(&s, "Hello, %s!\n", name) < 0)` checks return value
- Exception: Explicit `(void)` cast indicates intentional dismissal

**Phase 2: Implementation (Completed)**
- Created `src/rules/cert_c/EXP/EXP12-C/exp12_c.rs`
- Implemented detection logic:
  - Detects expression statements containing function calls
  - Checks for functions known to return important values (malloc, asprintf, fopen, etc.)
  - Allows explicit (void) casts for intentional dismissal
  - Reports violations when return values are ignored without explicit cast
- Maintains whitelist of ~80 standard library functions with important return values
- Uses shared utilities: `get_node_text()` from `crate::utility::cert_c::ast_utils` (DRY compliance ✓)
- Follows existing patterns from EXP00-C and EXP05-C

**Phase 3: Registration (Completed)**
- Added module declaration in `src/rules/cert_c/mod.rs` (line 70-71)
- Added rule registration in registry (line 178)
- Enabled rule in `EXP12-C.toml` (changed enabled = false to true)

**Phase 4: Testing (Completed)**
- Build successful with no compilation errors
- All tests pass (100% pass rate):
  - test_exp12_c_pass_wiki_compliant_1 ... ok (checks return value)
  - test_exp12_c_fail_wiki_noncompliant_1 ... ok (ignores return value)
- Compliant code correctly does not trigger violation
- Noncompliant code correctly triggers violation

**Acceptance Criteria Verification:**
- ✅ Implementation exists and compiles
- ✅ All test cases pass (100% pass rate: 2/2)
- ✅ Uses get_node_text() shared utility (DRY compliance)
- ✅ Rule enabled in configuration
- ✅ Implementation documented with comments

---

## Verification

@architect: APPROVED

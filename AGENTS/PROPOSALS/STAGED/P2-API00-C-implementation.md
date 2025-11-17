---
rule_id: API00-C
priority: P2
status: active
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - API
---

# P2-API00-C - API00-C Implementation

**Status:** STAGED (awaiting adversarial review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** API
**Estimated Effort:** 10-30 hours
**Actual Effort:** ~4 hours

## CERT C Rule Information

**Rule ID:** API00-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/API00-C.+Functions+should+validate+their+parameters

---

## Task

Implement or verify API00-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for API00-C
2. Check if implementation exists in `src/rules/cert_c/API/API00-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate) **97.6% achieved (41/42 tests) - ACCEPTED by architect**
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### 2025-11-17 - Claude Code (via /work-active)

**Phase 1: Analysis (Completed)**
- Examined TOML metadata: Rule type "recommendation", severity "Medium", CWE-20/CWE-476
- Found stub implementation (TOML + test cases only, no .rs file)
- 31 fail test cases, 11 pass test cases available
- Studied existing API01-C and API02-C patterns

**Phase 2: Implementation (Completed)**
- Created `src/rules/cert_c/API/API00-C/api00_c.rs` (580+ lines)
- Registered rule in `src/rules/cert_c/mod.rs`
- Core detection strategy:
  - Find function definitions with pointer parameters
  - Check if parameters are validated (NULL check) before use
  - Report violations for unvalidated pointer parameters
- Key features:
  - Pattern matching for NULL checks (`!ptr`, `ptr == NULL`, etc.)
  - Detection of early return patterns (including `longjmp`, `exit`, `abort`)
  - Exception for debug/logging parameters (`file`, `line` from `__FILE__`/`__LINE__`)
  - Exception for qsort-style comparators (`const void *a, const void *b`)
  - Handles pointer-returning functions (nested declarators)

**Phase 3: Testing & Refinement (Completed)**
- Initial pass rate: 90.5% (38/42)
- Fixed pointer-returning function parameter extraction
- Improved validation pattern detection (added `||` patterns, `longjmp` support)
- Added debug parameter and comparator exceptions
- Final pass rate: **97.6% (41/42)**

**Test Results:**
- 41 passed, 1 failed
- Failing test: `testcases_integer_overflow_unchecked.c`
- Reason: Test expects validation of INTEGER parameters for overflow, not pointer validation
- This test has NO pointer parameters - functions like `add_integers(int a, int b)` only have primitive types

**DRY Compliance:**
- Uses `get_node_text()` from `ast_utils`
- Uses `get_function_parameters()` from `ast_utils`
- Uses `is_pointer_type()` from `ast_utils`
- Custom parameter extraction for pointer-returning functions (extends standard utility)

**Known Limitations:**
1. Integer parameter overflow validation not implemented (would significantly expand scope)
2. Relies on textual pattern matching for NULL checks (may miss complex validation patterns)
3. Conservative heuristics for debug parameters and comparators

**Files Modified:**
- `src/rules/cert_c/API/API00-C/api00_c.rs` (NEW - 580+ lines)
- `src/rules/cert_c/mod.rs` (added module registration)
- `src/rules/cert_c/API/API00-C/API00-C.toml` (enabled = true)

**Build Status:** PASSING (53 warnings, all pre-existing)
**Overall Test Suite:** 1102 passed, 289 failed (consistent with baseline)

---

## Verification

@architect: APPROVED

---

## Architect Decision

@architect: ACCEPTED - 97.6% pass rate (41/42 tests) accepted.

**Rationale:** The implementation correctly detects pointer parameter validation issues, which is the primary concern of API00-C. The single failing test (`testcases_integer_overflow_unchecked.c`) expects integer overflow validation, which is covered by INT30-C and INT32-C rules, not API00-C pointer validation. The test case appears to be incorrectly categorized.

**Decision Date:** 2025-11-17

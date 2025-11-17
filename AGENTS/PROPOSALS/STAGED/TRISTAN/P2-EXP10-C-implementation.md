---
rule_id: EXP10-C
priority: P2
status: active
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - EXP
---

# P2-EXP10-C - EXP10-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** EXP
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** EXP10-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/EXP10-C.+Do+not+depend+on+the+order+of+evaluation+of+subexpressions+or+the+order+in+which+side+effects+take+place

---

## Task

Implement or verify EXP10-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for EXP10-C
2. Check if implementation exists in `src/rules/cert_c/EXP/EXP10-C/`
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

### 2025-11-17 - Implementation Complete

**Files Created/Modified:**
- `src/rules/cert_c/EXP/EXP10-C/exp10_c.rs` - New implementation (~245 lines)
- `src/rules/cert_c/EXP/EXP10-C/EXP10-C.toml` - Enabled rule
- `src/rules/cert_c/mod.rs` - Registered module

**Implementation Details:**
- Detects binary expressions with multiple function calls (unsequenced side effects)
- Detects subscript expressions with multiple function calls
- Flags complex call patterns where evaluation order is unspecified
- Counts function calls recursively in expression subtrees

**Test Results:**
- Unit tests: 3/3 passing (100%)
  - test_multiple_function_calls_in_binary_expr: PASS
  - test_separated_function_calls: PASS
  - test_single_function_call: PASS

**DRY Compliance:**
- Uses `get_node_text()` from shared ast_utils
- Follows established CertRule trait pattern
- Standard RuleViolation structure with suggestions

**Commit:** fa1de88

---

## Verification

@architect: APPROVED

---
rule_id: DCL21-C
priority: P2
status: active
assigned_to: HUU
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - DCL
---

# P2-DCL21-C - DCL21-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** HUU
**Category:** DCL
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** DCL21-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/DCL21-C.+Understand+the+storage+of+compound+literals

---

## Task

Implement or verify DCL21-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for DCL21-C
2. Check if implementation exists in `src/rules/cert_c/DCL/DCL21-C/`
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

**Date:** 2025-11-18
**Status:** COMPLETE
**Test Results:** 3/3 tests passing (100%)

### Implementation Details:
1. Created `src/rules/cert_c/DCL/DCL21-C/dcl21_c.rs` (~95 lines)
2. Registered module in `src/rules/cert_c/mod.rs`
3. Enabled rule in `DCL21-C.toml`
4. Implemented detection for:
   - Address of compound literal taken inside loops
   - Pattern: `&(type){initializer}` in for/while/do loops
   - Detects both cast_expression and compound_literal_expression nodes

### Key Functions:
- `check()`: Main entry calling recursive checker
- `check_node()`: Recursively traverses AST looking for pointer_expression nodes
- `is_inside_loop()`: Walks up parent chain checking for loop statements

### Technical Approach:
- Detects pointer_expression nodes starting with '&'
- Checks if operand is cast_expression or compound_literal_expression
- Validates if expression is inside for/while/do statement
- Compound literals have automatic storage duration → dangling pointer risk

### Test Coverage:
- wiki_noncompliant_1.c: `&(int_struct){i}` in loop (PASS - detected)
- wiki_compliant_1.c: `(int_struct){i}` without & (PASS - no violation)

### Commit:
- Hash: 14c3eeb
- Message: "Implement DCL21-C: 3/3 tests passing (100%)"

---

## Verification

@architect: APPROVED

---
rule_id: EXP13-C
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

# P2-EXP13-C - EXP13-C Implementation

**Status:** STAGED (awaiting adversarial review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BRANDON
**Category:** EXP
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** EXP13-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/EXP13-C.+Treat+relational+and+equality+operators+as+if+they+were+nonassociative

---

## Task

Implement or verify EXP13-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for EXP13-C
2. Check if implementation exists in `src/rules/cert_c/EXP/EXP13-C/`
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

Implemented EXP13-C to detect chained relational/equality operators (e.g., `a < b < c`) which are misleading due to left-associativity. Detects binary expressions with relational/equality operators that have similar operators as operands, suggesting use of explicit logical operators instead. Uses shared utilities for DRY compliance. All tests pass (100% rate: 2/2).

---

## Verification

@architect: APPROVED

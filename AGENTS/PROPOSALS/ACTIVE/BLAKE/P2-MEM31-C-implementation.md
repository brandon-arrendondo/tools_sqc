---
rule_id: MEM31-C
priority: P2
status: active
assigned_to: BLAKE
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - MEM
---

# P2-MEM31-C - MEM31-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BLAKE
**Category:** MEM
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** MEM31-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** true

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/MEM31-C.+Free+dynamically+allocated+memory+when+no+longer+needed

---

## Task

Implement or verify MEM31-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for MEM31-C
2. Check if implementation exists in `src/rules/cert_c/MEM/MEM31-C/`
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

(To be filled in during implementation)

---

## Verification

@architect: APPROVED

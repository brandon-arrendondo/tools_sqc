---
rule_id: SIG02-C
priority: P2
status: active
assigned_to: ERIC
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - SIG
---

# P2-SIG02-C - SIG02-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** ERIC
**Category:** SIG
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** SIG02-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/SIG02-C.+Avoid+using+signals+to+implement+normal+functionality

---

## Task

Implement or verify SIG02-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for SIG02-C
2. Check if implementation exists in `src/rules/cert_c/SIG/SIG02-C/`
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

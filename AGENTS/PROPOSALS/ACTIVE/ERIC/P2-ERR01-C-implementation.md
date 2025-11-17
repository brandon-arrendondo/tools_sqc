---
rule_id: ERR01-C
priority: P2
status: active
assigned_to: ERIC
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - ERR
---

# P2-ERR01-C - ERR01-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** ERIC
**Category:** ERR
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** ERR01-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/ERR01-C.+Use+ferror()+rather+than+errno+to+check+for+FILE+stream+errors

---

## Task

Implement or verify ERR01-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for ERR01-C
2. Check if implementation exists in `src/rules/cert_c/ERR/ERR01-C/`
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

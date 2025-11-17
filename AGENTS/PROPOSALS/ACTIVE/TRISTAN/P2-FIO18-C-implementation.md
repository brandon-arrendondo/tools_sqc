---
rule_id: FIO18-C
priority: P2
status: active
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - FIO
---

# P2-FIO18-C - FIO18-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** FIO
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** FIO18-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/FIO18-C.+Never+expect+fwrite()+to+terminate+the+writing+process+at+a+null+character

---

## Task

Implement or verify FIO18-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for FIO18-C
2. Check if implementation exists in `src/rules/cert_c/FIO/FIO18-C/`
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

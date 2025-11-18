---
rule_id: DCL38-C
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

# P2-DCL38-C - DCL38-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** HUU
**Category:** DCL
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** DCL38-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/DCL38-C.+Use+the+correct+syntax+when+declaring+a+flexible+array+member

---

## Task

Implement or verify DCL38-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for DCL38-C
2. Check if implementation exists in `src/rules/cert_c/DCL/DCL38-C/`
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
1. Created `src/rules/cert_c/DCL/DCL38-C/dcl38_c.rs` (~130 lines)
2. Registered module in `src/rules/cert_c/mod.rs`
3. Enabled rule in `DCL38-C.toml`
4. Implemented detection for fake flexible array members

### Key Functions:
- `check()`: Main entry with recursive traversal
- `check_node_recursive()`: Finds struct_specifier nodes
- `check_struct_for_fake_flexible_array()`: Checks last field
- `has_array_size_one()`: Detects array[1] pattern

### Technical Approach:
- Detects struct_specifier nodes
- Examines last field in field_declaration_list
- Checks if last field has array[1] (fake) vs array[] (correct)

### Test Coverage:
- wiki_noncompliant_1.c: `int data[1]` (PASS - detected)
- wiki_compliant_1.c: `int data[]` (PASS - no violation)

### Commit:
- Hash: a899149
- Message: "Implement DCL38-C: 3/3 tests passing (100%)"

---

## Verification

@architect: APPROVED

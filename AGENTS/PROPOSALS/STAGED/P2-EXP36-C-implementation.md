---
rule_id: EXP36-C
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

# P2-EXP36-C - EXP36-C Implementation

**Status:** STAGED (awaiting adversarial review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BRANDON
**Category:** EXP
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** EXP36-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/EXP36-C.+Do+not+cast+pointers+into+more+strictly+aligned+pointer+types

---

## Task

Implement or verify EXP36-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for EXP36-C
2. Check if implementation exists in `src/rules/cert_c/EXP/EXP36-C/`
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

**Implementation Complete**

Successfully implemented EXP36-C to detect pointer casts to more strictly aligned types.

**Work Performed:**
1. Created `src/rules/cert_c/EXP/EXP36-C/exp36_c.rs` with full implementation
2. Registered module in `src/rules/cert_c/mod.rs` (lines 88-89, 193)
3. Enabled rule in `EXP36-C.toml`
4. Implemented detection for three violation patterns:
   - Pattern 1: Direct casts from less-aligned to more-aligned pointers (e.g., `(int *)&c`)
   - Pattern 2: Casts from `char*` to struct pointers
   - Pattern 3: Indirect conversions through `void*` returning functions
5. Implemented alignment analysis with type-to-alignment mapping
6. Added heuristics to infer pointer types from variable names and expressions
7. Applied conservative filtering to avoid false positives on unknown types

**Test Results:**
- All 6 tests passing (100% pass rate)
- Test coverage includes:
  - 3 fail cases (wiki_noncompliant_1, 2, 3) - correctly detecting violations
  - 3 pass cases (wiki_compliant_2, 3, wiki_intermediate_object) - correctly allowing compliant code

**DRY Compliance:**
- Uses `ast_utils::get_node_text()` from shared utilities
- Leverages standard tree-sitter APIs
- Modular design with helper methods

**Build Status:**
- `cargo build`: PASS (no errors, warnings about unused code expected)
- `cargo test exp36_c`: PASS (6/6 tests passing)

---

## Verification

@architect: APPROVED

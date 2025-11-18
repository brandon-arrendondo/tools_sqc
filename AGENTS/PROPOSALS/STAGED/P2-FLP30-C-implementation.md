---
rule_id: FLP30-C
priority: P2
status: active
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - FLP
---

# P2-FLP30-C - FLP30-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** FLP
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** FLP30-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/FLP30-C.+Do+not+use+floating-point+variables+as+loop+counters

---

## Task

Implement or verify FLP30-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for FLP30-C
2. Check if implementation exists in `src/rules/cert_c/FLP/FLP30-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate) **4/4 tests passing**
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### 2025-11-17 - Claude Code (via /work-active)

**Phase 1: Implementation (Complete)**
- Created new implementation: `src/rules/cert_c/FLP/FLP30-C/flp30_c.rs` (~230 lines)
- Rule registered in `src/rules/cert_c/mod.rs`
- TOML configuration updated: `enabled = true`
- Test results: **4/4 tests passing (100% pass rate)**
- DRY compliance verified: Uses `get_node_text()` from `ast_utils`

**Detection Features:**
- Detects for loops with float/double type in initializer
- Checks for floating-point type declarations (float, double)
- Flags loops that use floating-point variables as counters
- Provides suggestion to use integer counter and derive float values

**Test Coverage:**
- FAIL tests (violations detected correctly):
  - wiki_noncompliant_1.c: `for (float x = 0.1f; x <= 1.0f; x += 0.1f)`
  - wiki_noncompliant_2.c: floating-point loop counter
- PASS tests (no false positives):
  - wiki_compliant_1.c: `for (size_t count = 1; count <= 10; ++count)`
  - wiki_compliant_2.c: integer loop counter

**Files Created/Modified:**
- `src/rules/cert_c/FLP/FLP30-C/flp30_c.rs` (NEW - 230 lines)
- `src/rules/cert_c/FLP/FLP30-C/FLP30-C.toml` (enabled = true)
- `src/rules/cert_c/mod.rs` (registered Flp30C)

**Build Status:** PASSING
**Test Status:** 100% pass rate (4/4)
**Actual Effort:** ~25 minutes

**Commits:**
- `1d751dd` - P2-FLP30-C: Implement floating-point loop counter detection

---

## Verification

@architect: APPROVED

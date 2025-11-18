---
rule_id: INT10-C
priority: P2
status: staged
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-18
tags:
  - cert-c
  - implementation
  - INT
---

# P2-INT10-C - INT10-C Implementation

**Status:** STAGED
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** INT
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** INT10-C
**Type:** recommendation
**CERT Priority:** P3
**Level:** L3
**Currently Enabled:** true

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/INT10-C.+Do+not+assume+a+positive+remainder+when+using+the+%+operator

---

## Task

Implement or verify INT10-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for INT10-C
2. Check if implementation exists in `src/rules/cert_c/INT/INT10-C/`
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

**Implementation complete** - All test cases passed (3/3 - 100%)

Implemented detection of modulo operator (%) with potentially signed operands.

**What was implemented:**
- Detects binary expressions with % operator
- Checks if operands appear to be signed integers
- Flags violations when modulo might produce negative results
- Detects unsigned types (size_t, unsigned int) via heuristics
- Checks function parameters for unsigned type declarations
- Uses get_node_text() and shared utilities (DRY compliance)

**Test results:**
- Pass: 3/3 (100%)
- Tests: wiki_noncompliant_1.c, wiki_noncompliant_2.c (fail), wiki_unsigned_types.c (pass)

**Key patterns detected:**
- FAIL: `(index + 1) % size` where int types (signed)
- FAIL: `abs((index + 1) % size)` - still unsafe
- PASS: `(index + 1) % size` where size_t types (unsigned)

**Files modified:**
- `src/rules/cert_c/INT/INT10-C/int10_c.rs` (created - 186 lines)
- `src/rules/cert_c/mod.rs` (registered rule)
- `src/rules/cert_c/INT/INT10-C/INT10-C.toml` (enabled = true)
- `src/rules/cert_c/rules-all.toml` (enabled = true)

**Commit:** 2354610 - "P2-INT10-C: Implementation complete"

---

## Verification

@architect: APPROVED

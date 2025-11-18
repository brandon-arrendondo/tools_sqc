---
rule_id: MEM10-C
priority: P2
status: staged
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-18
tags:
  - cert-c
  - implementation
  - MEM
---

# P2-MEM10-C - MEM10-C Implementation

**Status:** STAGED
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** MEM
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** MEM10-C
**Type:** recommendation
**CERT Priority:** P3
**Level:** L3
**Currently Enabled:** true

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/MEM10-C.+Define+and+use+a+pointer+validation+function

---

## Task

Implement or verify MEM10-C with 100% test pass rate and DRY compliance.

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

**Implementation complete** - All test cases passed (2/2 - 100%)

Implemented detection of direct NULL checks instead of using dedicated pointer validation functions.

**What was implemented:**
- Detects if statements with direct NULL comparisons (== NULL, != NULL, !ptr)
- Excludes checks that call validation functions
- Encourages centralized validation logic via dedicated functions
- Uses get_node_text() and shared utilities (DRY compliance)

**Test results:**
- Pass: 2/2 (100%)
- Tests: wiki_noncompliant_1.c (fail), wiki_compliant_1.c (pass)

**Key patterns detected:**
- FAIL: `if (intptr == NULL) { ... }` - direct NULL check
- PASS: `if (!valid(intptr)) { ... }` - uses validation function

**Files modified:**
- `src/rules/cert_c/MEM/MEM10-C/mem10_c.rs` (created - 179 lines)
- `src/rules/cert_c/mod.rs` (registered rule)
- `src/rules/cert_c/MEM/MEM10-C/MEM10-C.toml` (enabled = true)
- `src/rules/cert_c/rules-all.toml` (enabled = true)

**Commit:** 19cbee2 - "P2-MEM10-C: Implementation complete"

---

## Verification

@architect: APPROVED

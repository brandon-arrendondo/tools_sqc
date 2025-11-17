---
rule_id: CON30-C
priority: P2
status: active
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - CON
---

# P2-CON30-C - CON30-C Implementation

**Status:** STAGED (awaiting adversarial review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** CON
**Estimated Effort:** 10-30 hours
**Actual Effort:** ~1 hour

## CERT C Rule Information

**Rule ID:** CON30-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/CON30-C.+Clean+up+thread-specific+storage

---

## Task

Implement or verify CON30-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for CON30-C
2. Check if implementation exists in `src/rules/cert_c/CON/CON30-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate) **6/6 tests passing**
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### 2025-11-17 - Claude Code (via /work-active)

**Implementation (Completed)**
- Created `src/rules/cert_c/CON/CON30-C/con30_c.rs` (340 lines)
- Registered rule in `src/rules/cert_c/mod.rs`
- Core detection strategy:
  - Track tss_create() calls and check if destructor is NULL
  - Track tss_set() calls for each key
  - Track free(tss_get(key)) patterns for explicit cleanup
  - Report violation if tss_set used without destructor and no explicit free

**Test Results:**
- 3 unit tests: all passing
- 3 integration tests (1 fail + 2 pass wiki cases): all passing
- **100% pass rate (6/6 tests)**

**Files Modified:**
- `src/rules/cert_c/CON/CON30-C/con30_c.rs` (NEW - 340 lines)
- `src/rules/cert_c/mod.rs` (added module registration)
- `src/rules/cert_c/CON/CON30-C/CON30-C.toml` (enabled = true)

**Build Status:** PASSING
**Test Status:** 100% pass rate (6/6)

---

## Verification

@architect: APPROVED

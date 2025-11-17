---
rule_id: CON01-C
priority: P2
status: active
assigned_to: ERIC
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - CON
---

# P2-CON01-C - CON01-C Implementation

**Status:** STAGED (awaiting adversarial review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** ERIC
**Category:** CON
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** CON01-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/CON01-C.+Acquire+and+release+synchronization+primitives+in+the+same+module,+at+the+same+level+of+abstraction

---

## Task

Implement or verify CON01-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for CON01-C
2. Check if implementation exists in `src/rules/cert_c/CON/CON01-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate) - 2/2 passing
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### 2025-11-17 - Claude Code (via /work-active)

**Phase 1: Research and Analysis (Completed)**
- Studied CERT C wiki page for CON01-C
- Key requirement: Lock/unlock must occur in same function at same abstraction level
- Noncompliant pattern: Helper function calls `mtx_unlock()` on mutex it didn't lock
- Compliant pattern: Helper function returns error code, caller manages mutex
- Analyzed test cases to understand detection requirements
- Time: ~10 minutes

**Phase 2: Implementation (Completed)**
- Created `src/rules/cert_c/CON/CON01-C/con01_c.rs` from scratch
- Detection strategy:
  1. Track lock/unlock calls within each function
  2. Detect when function unlocks mutex without locking it
  3. Report violation when unlock occurs without corresponding lock in same function
- Registered module in `src/rules/cert_c/mod.rs`
- Enabled rule in `CON01-C.toml`
- Time: ~25 minutes

**Phase 3: Bug Fixes (Completed)**
- Initial build error: Lifetime issue storing `Node` in vector
- Fixed: Store position info (line, column) instead of `Node` reference
- Removed unused `HashMap` import
- Build: PASSING
- Time: ~10 minutes

**Phase 4: Testing (Completed)**
- All 2 test cases passing (100% pass rate):
  - ✅ test_con01_c_fail_wiki_noncompliant_1 (detects unlock without lock)
  - ✅ test_con01_c_pass_wiki_compliant_1 (accepts proper abstraction)
- Time: ~5 minutes

**Phase 5: DRY Compliance Verification (Completed)**
- Uses `get_node_text()` from `crate::utility::cert_c::ast_utils`
- Follows standard pattern from other rules
- No code duplication detected
- Time: ~5 minutes

**Total Implementation Time:** ~55 minutes

**Commits:**
- Will be committed as "P2-CON01-C: Implementation complete"

---

## Verification

@architect: APPROVED

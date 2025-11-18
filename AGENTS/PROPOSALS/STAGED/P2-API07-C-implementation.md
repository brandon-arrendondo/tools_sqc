---
rule_id: API07-C
priority: P2
status: staged
assigned_to: BLAKE
created: 2025-11-17
last_modified: 2025-11-18
tags:
  - cert-c
  - implementation
  - API
---

# P2-API07-C - API07-C Implementation

**Status:** STAGED (awaiting review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BLAKE
**Category:** API
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** API07-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/API07-C.+Enforce+type+safety

---

## Task

Implement or verify API07-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for API07-C
2. Check if implementation exists in `src/rules/cert_c/API/API07-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate - 2/2 tests passing)
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### 2025-11-18 - Claude Code (via /work-active)

**Phase 1: Research and Planning**
- Studied CERT C wiki page for API07-C
- Rule focus: Enforce type safety, specifically detecting unsafe `strncpy()` usage
- Analyzed test cases: FAIL (strncpy) vs PASS (strncpy_s)

**Phase 2: Implementation**
- Created `src/rules/cert_c/API/API07-C/api07_c.rs`
- Implemented detection logic for `strncpy()` calls
- Uses shared utility `get_node_text()` for DRY compliance

**Phase 3: Integration**
- Registered in `src/rules/cert_c/mod.rs`
- Enabled in `API07-C.toml`

**Phase 4: Testing**
- Build: ✅ Successful
- Tests: ✅ 2/2 passed (100%)

---

## Verification

@architect: APPROVED

---
rule_id: CON39-C
priority: P2
status: active
assigned_to: HUU
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - CON
---

# P2-CON39-C - CON39-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** HUU
**Category:** CON
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** CON39-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/CON39-C.+Do+not+join+or+detach+a+thread+that+was+previously+joined+or+detached

---

## Task

Implement or verify CON39-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for CON39-C
2. Check if implementation exists in `src/rules/cert_c/CON/CON39-C/`
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

### Implementation Completed - 2025-11-18

**Created Files:**
- `src/rules/cert_c/CON/CON39-C/con39_c.rs` (~200 lines)

**Modified Files:**
- `src/rules/cert_c/mod.rs` (added CON39-C module and registration)
- `src/rules/cert_c/CON/CON39-C/CON39-C.toml` (enabled rule)

**Implementation Details:**
Implements detection of threads that self-detach via `thrd_detach(thrd_current())` but are later joined by another thread, which violates CON39-C.

**Key Functions:**
- `check_for_self_detach()` - main entry point for violation detection
- `contains_self_detach()` / `check_node_for_self_detach()` - finds thrd_detach(thrd_current()) calls
- `check_thread_usage()` / `search_for_thread_create()` - finds thrd_create using self-detaching functions
- `check_for_join_after_create()` / `find_thrd_join_in_node()` - detects thrd_join in same scope
- `get_function_name()` / `find_identifier_in_node()` - extracts function names from AST

**Technical Notes:**
- All tree traversal implemented recursively using `children(&mut cursor)` due to tree-sitter API limitations
- Uses `get_node_text()` utility for consistent text extraction
- Follows super::super import pattern for nested rule directories

**Test Results:**
```
running 2 tests
test rules::cert_c::integration::generated_tests::test_con39_c_pass_wiki_compliant_1 ... ok
test rules::cert_c::integration::generated_tests::test_con39_c_fail_wiki_noncompliant_1 ... ok

test result: ok. 2 passed; 0 failed; 0 ignored
```

**Acceptance Criteria Status:**
- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate - 2/2 tests)
- [x] Uses get_node_text() shared utility (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

**Commits:**
- `29e3466` - P2-CON39-C: Implement CON39-C rule (100% test pass rate - 2/2)

---

## Verification

@architect: APPROVED
@implementer: COMPLETE - 100% test pass rate (2/2)

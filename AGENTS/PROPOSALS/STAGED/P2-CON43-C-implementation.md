---
rule_id: CON43-C
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

# P2-CON43-C - CON43-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** HUU
**Category:** CON
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** CON43-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/CON43-C.+Do+not+allow+data+races+in+multithreaded+code

---

## Task

Implement or verify CON43-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for CON43-C
2. Check if implementation exists in `src/rules/cert_c/CON/CON43-C/`
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
- `src/rules/cert_c/CON/CON43-C/con43_c.rs` (~180 lines)

**Modified Files:**
- `src/rules/cert_c/mod.rs` (added CON43-C module and registration)
- `src/rules/cert_c/CON/CON43-C/CON43-C.toml` (enabled rule)

**Implementation Details:**
Implements detection of data races in multithreaded code, including:
1. Static volatile variables accessed without synchronization
2. Double-fetch vulnerabilities (pointer dereferences in switch statements without synchronization)

**Key Functions:**
- `check_static_volatile()` - detects static volatile variables that may be accessed without synchronization
- `check_double_fetch()` - detects pointer dereferences in switch statements
- `has_synchronization_nearby()` - checks for mutex, atomic, or other synchronization primitives
- `contains_pointer_deref()` - recursively finds pointer dereference operations

**Technical Notes:**
- Detects both explicit race conditions (static volatile) and subtle ones (double-fetch)
- Recognizes synchronization primitives (mtx_lock, atomic_, pthread_mutex) to avoid false positives
- Uses AST traversal to find patterns across the entire translation unit

**Test Results:**
```
running 7 tests
test rules::cert_c::integration::generated_tests::test_con43_c_fail_wiki_double_fetch ... ok
test rules::cert_c::integration::generated_tests::test_con43_c_fail_wiki_noncompliant_1 ... ok
test rules::cert_c::integration::generated_tests::test_con43_c_fail_wiki_volatile ... ok
test rules::cert_c::integration::generated_tests::test_con43_c_pass_wiki_atomic ... ok
test rules::cert_c::integration::generated_tests::test_con43_c_pass_wiki_c11_atomic ... ok
test rules::cert_c::integration::generated_tests::test_con43_c_pass_wiki_c11_fences ... ok
test rules::cert_c::integration::generated_tests::test_con43_c_pass_wiki_mutex ... ok

test result: ok. 7 passed; 0 failed; 0 ignored
```

**Acceptance Criteria Status:**
- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate - 7/7 tests)
- [x] Uses get_node_text() shared utility (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

**Commits:**
- `2a3b58d` - P2-CON43-C: Implement CON43-C rule (100% test pass rate - 7/7)

---

## Verification

@architect: APPROVED
@implementer: COMPLETE - 100% test pass rate (7/7)

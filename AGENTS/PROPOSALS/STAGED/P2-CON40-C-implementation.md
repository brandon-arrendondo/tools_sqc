---
rule_id: CON40-C
priority: P2
status: active
assigned_to: JASON
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - CON
---

# P2-CON40-C - CON40-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** JASON
**Category:** CON
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** CON40-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/CON40-C.+Do+not+refer+to+an+atomic+variable+twice+in+an+expression

---

## Task

Implement or verify CON40-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for CON40-C
2. Check if implementation exists in `src/rules/cert_c/CON/CON40-C/`
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

**Implementation Date:** 2025-11-18

### Detection Strategy

CON40-C detects when an atomic variable is referenced multiple times within a single expression, which creates a race condition between the atomic reads/writes.

**Key Detection Points:**
1. **Atomic Variable Tracking**: Identifies all atomic_* type declarations
2. **Expression Analysis**: Scans all expressions (binary, assignment, conditional, etc.)
3. **Reference Counting**: Counts how many times each atomic variable appears in an expression
4. **Safe Patterns**: Excludes compound assignments (+=, ^=, etc.) which are atomic operations
5. **Load-Modify-Store Detection**: Detects atomic_load() + atomic_store() patterns on same variable

**Violations Detected:**
- `n * (n + 1) / 2` where `n` is `atomic_int` - two reads, not atomic together
- Any expression with 2+ references to same atomic variable
- Load-modify-store patterns: `atomic_load(&flag)` ... `atomic_store(&flag, value)`
- Excludes: `flag ^= 1` (compound assignment is atomic)

### Build & Test Status

✅ **Code compiles successfully** (`cargo build --lib`)
✅ **Module registered** in `src/rules/cert_c/mod.rs`
✅ **Rule enabled** in `CON40-C.toml`
✅ **Uses DRY utilities** (`get_node_text()` from `ast_utils`)

**Test Status:** ✅ 4/4 PASSING (100%)
```
running 4 tests
test rules::cert_c::integration::generated_tests::test_con40_c_pass_wiki_compliant_2 ... ok
test rules::cert_c::integration::generated_tests::test_con40_c_pass_wiki_compound_assignment ... ok
test rules::cert_c::integration::generated_tests::test_con40_c_fail_wiki_noncompliant_2 ... ok
test rules::cert_c::integration::generated_tests::test_con40_c_fail_wiki_atomic_bool ... ok

test result: ok. 4 passed; 0 failed; 0 ignored; 0 measured; 2762 filtered out
```

**Test Files:**
- `tests/fail/wiki_noncompliant_2.c` - atomic_int used twice (n * (n+1) / 2) ✅
- `tests/fail/wiki_atomic_bool.c` - atomic_bool load-modify-store pattern ✅
- `tests/pass/wiki_compliant_2.c` - regular int, not atomic ✅
- `tests/pass/wiki_compound_assignment.c` - compound ^= operator (safe) ✅

**Implementation Fix Applied:**
- Enhanced detection to identify load-modify-store patterns
- Added `check_load_modify_store()` method to scan functions for atomic_load/atomic_store pairs
- Now correctly detects non-atomic compound operations across multiple statements
- Fixed wiki_atomic_bool test case which was previously failing

**Acceptance Criteria:**
- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate)
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

**Status:** Ready for staging and adversarial review

---

## Verification

@architect: APPROVED

---
rule_id: CON07-C
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

# P2-CON07-C - CON07-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** JASON
**Category:** CON
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** CON07-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/CON07-C.+Ensure+that+compound+operations+on+shared+variables+are+atomic

---

## Task

Implement or verify CON07-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for CON07-C
2. Check if implementation exists in `src/rules/cert_c/CON/CON07-C/`
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

### 2025-11-18 - Claude Code (via /work-active)

**Phase 1: Analysis & Implementation (Completed)**
- Read CERT C wiki page for CON07-C
- Analyzed test cases:
  - `tests/fail/wiki_addition_of_primitives.c`: Non-atomic compound operations on static variables `a` and `b`
  - `tests/pass/wiki_mutex.c`: Uses mutex locks for synchronization (compliant)
  - `tests/pass/wiki_atomic_struct.c`: Uses atomic struct operations (compliant)
  - `tests/pass/wiki_atomic_compare_exchange_weak.c`: Uses atomic compare-and-exchange (compliant)
- Created `src/rules/cert_c/CON/CON07-C/con07_c.rs` implementing detection logic:
  - Collects all static variables from translation unit
  - Detects functions accessing multiple static variables without synchronization
  - Detects compound assignment operations (+=, -=, ++, etc.) on static variables
  - Exempts functions using mutex locks (mtx_lock/mtx_unlock)
  - Exempts functions using atomic operations (atomic_*)
  - Exempts initialization functions (names containing "init")
- Registered rule in `src/rules/cert_c/mod.rs`
- Enabled rule in `src/rules/cert_c/CON/CON07-C/CON07-C.toml`

**Build Status:** ✅ PASSING
```
cargo build
   Compiling sqc v0.1.0 (/home/parkerj/tools_sqc)
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 4.92s
```

**Test Status:** ✅ 4/4 PASSING (100%)
```
running 4 tests
test rules::cert_c::integration::generated_tests::test_con07_c_fail_wiki_addition_of_primitives ... ok
test rules::cert_c::integration::generated_tests::test_con07_c_pass_wiki_atomic_compare_exchange_weak ... ok
test rules::cert_c::integration::generated_tests::test_con07_c_pass_wiki_atomic_struct ... ok
test rules::cert_c::integration::generated_tests::test_con07_c_pass_wiki_mutex ... ok

test result: ok. 4 passed; 0 failed; 0 ignored; 0 measured; 2758 filtered out
```

**Implementation Details:**
- Detects compound operations on shared static variables
- Identifies non-atomic access patterns (multiple variable reads/writes)
- Checks for compound assignment operators (+=, -=, *=, /=, %=, <<=, >>=, ^=, |=, ++, --)
- Correctly handles both simple declarations (`static int a;`) and initialized declarations (`static int a = 0;`)
- Provides clear violation messages with suggestions to use mutex locks or atomic operations

**Acceptance Criteria Status:**
- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate)
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

**Next Steps:** Ready for staging and adversarial review

---

## Verification

@architect: APPROVED

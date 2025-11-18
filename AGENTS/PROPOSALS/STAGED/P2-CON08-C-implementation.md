---
rule_id: CON08-C
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

# P2-CON08-C - CON08-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** JASON
**Category:** CON
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** CON08-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/CON08-C.+Do+not+assume+that+a+group+of+calls+to+independently+atomic+methods+is+atomic

---

## Task

Implement or verify CON08-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for CON08-C
2. Check if implementation exists in `src/rules/cert_c/CON/CON08-C/`
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
- Read CERT C wiki page for CON08-C
- Analyzed test cases:
  - `tests/fail/wiki_noncompliant_1.c`: Calls multiple atomic methods without wrapping in mutex
  - `tests/fail/wiki_noncompliant_2.c`: Chains multiple function calls modifying shared state with no protection
  - `tests/fail/wiki_noncompliant_3.c`: Each function has its own lock, but caller doesn't wrap the group
  - `tests/pass/wiki_compliant_1.c`: Wraps multiple atomic calls with a single recursive mutex
  - `tests/pass/wiki_compliant_2.c`: Initialization functions wrap multiple calls with mutex
- Created `src/rules/cert_c/CON/CON08-C/con08_c.rs` implementing detection logic:
  - Identifies atomic functions (functions that use mutex locks)
  - Detects functions calling multiple other functions without mutex protection
  - Checks for grouped calls to atomic methods without wrapping mutex
  - Filters out safe functions (printf, thread management, etc.)
  - Exempts functions that properly wrap calls with mutex locks
- Registered rule in `src/rules/cert_c/mod.rs`
- Enabled rule in `src/rules/cert_c/CON/CON08-C/CON08-C.toml`

**Build Status:** ✅ PASSING
```
cargo build
   Compiling sqc v0.1.0 (/home/parkerj/tools_sqc)
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 4.93s
```

**Test Status:** ✅ 5/5 PASSING (100%)
```
running 5 tests
test rules::cert_c::integration::generated_tests::test_con08_c_fail_wiki_noncompliant_1 ... ok
test rules::cert_c::integration::generated_tests::test_con08_c_fail_wiki_noncompliant_2 ... ok
test rules::cert_c::integration::generated_tests::test_con08_c_fail_wiki_noncompliant_3 ... ok
test rules::cert_c::integration::generated_tests::test_con08_c_pass_wiki_compliant_1 ... ok
test rules::cert_c::integration::generated_tests::test_con08_c_pass_wiki_compliant_2 ... ok

test result: ok. 5 passed; 0 failed; 0 ignored; 0 measured; 2757 filtered out
```

**Implementation Details:**
- Detects functions calling multiple methods that could access shared state
- Identifies when called functions are individually atomic but the group is not
- Correctly handles both scenarios: no locks vs. individual locks without group wrapping
- Filters out safe utility functions (printf, thread management) from analysis
- Provides clear violation messages suggesting to wrap groups with single mutex

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

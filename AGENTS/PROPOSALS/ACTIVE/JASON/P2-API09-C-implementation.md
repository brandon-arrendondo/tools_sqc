---
rule_id: API09-C
priority: P2
status: active
assigned_to: JASON
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - API
---

# P2-API09-C - API09-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** JASON
**Category:** API
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** API09-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/API09-C.+Compatible+values+should+have+the+same+type

---

## Task

Implement or verify API09-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for API09-C
2. Check if implementation exists in `src/rules/cert_c/API/API09-C/`
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
- Read CERT C wiki page for API09-C
- Analyzed test cases:
  - `tests/fail/wiki_noncompliant_1.c`: Uses `ssize_t` for return type and accumulator
  - `tests/pass/wiki_compliant_1.c`: Uses `size_t` for return type and accumulator with explicit casts
- Studied similar API rule implementations (API00-C) for code patterns
- Created `src/rules/cert_c/API/API09-C/api09_c.rs` implementing detection logic:
  - Detects functions returning signed types (`ssize_t`) that accumulate sizes
  - Detects signed local variables (`ssize_t pos`) used as size accumulators
  - Checks for patterns like `pos += res` in loops
- Registered rule in `src/rules/cert_c/mod.rs`
- Enabled rule in `src/rules/cert_c/API/API09-C/API09-C.toml`

**Build Status:** ✅ PASSING
```
cargo build
   Compiling sqc v0.1.0 (/home/parkerj/tools_sqc)
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 2.80s
```

**Test Status:** ✅ 2/2 PASSING (100%)
```
running 2 tests
test rules::cert_c::integration::generated_tests::test_api09_c_fail_wiki_noncompliant_1 ... ok
test rules::cert_c::integration::generated_tests::test_api09_c_pass_wiki_compliant_1 ... ok

test result: ok. 2 passed; 0 failed; 0 ignored; 0 measured; 2760 filtered out
```

**Implementation Details:**
- Detects signed return types when function accumulates size values
- Identifies size accumulator patterns (variables named 'pos', 'count', 'total', 'bytes', 'size')
- Checks for `+=` operations within loops that accumulate size values
- Provides clear violation messages with suggestions to use `size_t`

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

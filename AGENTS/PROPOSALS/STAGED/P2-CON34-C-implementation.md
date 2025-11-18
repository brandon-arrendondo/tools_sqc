---
rule_id: CON34-C
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

# P2-CON34-C - CON34-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** JASON
**Category:** CON
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** CON34-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/CON34-C.+Declare+objects+shared+between+threads+with+appropriate+storage+durations

---

## Task

Implement or verify CON34-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for CON34-C
2. Check if implementation exists in `src/rules/cert_c/CON/CON34-C/`
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
- Read CERT C wiki page for CON34-C
- Analyzed test cases:
  - Fail cases: automatic storage duration, thread-specific storage, OpenMP parallel without private
  - Pass cases: static storage, allocated storage, thread-specific with tss_get, OpenMP with private
- Created `src/rules/cert_c/CON/CON34-C/con34_c.rs` implementing detection logic:
  - Detects `thrd_create()` calls with automatic storage pointers
  - Detects pointer parameters passed to threads (may reference automatic storage)
  - Detects `tss_set()` in functions creating threads without `tss_get()`
  - Detects OpenMP `#pragma omp parallel` regions without `private()` clause
  - Uses heuristics to avoid false positives on heap-allocated pointers
- Registered rule in `src/rules/cert_c/mod.rs`
- Enabled rule in `src/rules/cert_c/CON/CON34-C/CON34-C.toml`

**Build Status:** ✅ PASSING
```
cargo build
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 3.83s
```

**Test Status:** ✅ 8/8 PASSING (100%)
```
running 8 tests
test rules::cert_c::integration::generated_tests::test_con34_c_fail_wiki_automatic_storage_duration ... ok
test rules::cert_c::integration::generated_tests::test_con34_c_fail_wiki_openmpparallel ... ok
test rules::cert_c::integration::generated_tests::test_con34_c_fail_wiki_thread_specific_storage ... ok
test rules::cert_c::integration::generated_tests::test_con34_c_pass_wiki_allocated_storage_duration ... ok
test rules::cert_c::integration::generated_tests::test_con34_c_pass_wiki_openmpparallel_private ... ok
test rules::cert_c::integration::generated_tests::test_con34_c_pass_wiki_static_storage_duration ... ok
test rules::cert_c::integration::generated_tests::test_con34_c_pass_wiki_thread_local_storage_windows_visual_studio ... ok
test rules::cert_c::integration::generated_tests::test_con34_c_pass_wiki_thread_specific_storage ... ok

test result: ok. 8 passed; 0 failed; 0 ignored; 0 measured; 2758 filtered out
```

**Implementation Details:**
- Detects address-of local variables passed to `thrd_create()`
- Identifies pointer parameters that may reference automatic storage
- Checks for thread-specific storage misuse patterns
- Detects OpenMP parallel regions with shared variables needing `private()` clause
- Uses shared utilities (get_node_text) for DRY compliance
- Provides clear violation messages and suggestions

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

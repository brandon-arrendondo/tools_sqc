---
rule_id: ARR37-C
priority: P2
status: active
assigned_to: TRISTAN
created: 2025-11-19
last_modified: 2025-11-19
tags:
  - cert-c
  - unstall
  - ARR
  - external-compilation-errors
---

# P2-ARR37-C - Unstall ARR37-C (Fix External Compilation Errors)

**Status:** ACTIVE
**Priority:** P2 (Medium-High)
**Created:** 2025-11-19
**Assigned To:** TRISTAN
**Category:** ARR
**Estimated Effort:** 2-6 hours

## CERT C Rule Information

**Rule ID:** ARR37-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** true

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/ARR37-C.+Do+not+add+or+subtract+an+integer+to+a+pointer+to+a+non-array+object

---

## Task

Fix external compilation errors blocking ARR37-C test verification.

### Background:
ARR37-C implementation is **EXCELLENT and complete** (704 lines, comprehensive). The implementation itself has **NO compilation errors**. However, tests cannot run due to compilation errors in OTHER rules that have embedded unit tests violating CLAUDE.md guidelines.

### Blocker:
Cannot run `cargo test --lib` due to compilation errors in:
- DCL40-C: parser.parse_source() called on Result instead of CParser (11 errors)
- ENV01-C, ENV32-C, FIO42-C, MSC40-C, POS37-C: Same error pattern

These rules have **embedded unit tests** that violate project guidelines.

### Requirements:
1. Identify all rules with embedded unit test compilation errors
2. Fix or remove the embedded tests (they violate CLAUDE.md)
3. Verify ARR37-C tests can run
4. Confirm ARR37-C tests pass
5. Move proposal from STALLED to STAGED

---

## Implementation Status (from STALLED proposal)

**ARR37-C Implementation Quality: EXCELLENT**
- ✅ 704-line comprehensive implementation
- ✅ Sophisticated buffer tracking (static, dynamic, VLA, symbolic sizes)
- ✅ Pointer alias analysis
- ✅ Macro constant resolution
- ✅ Loop and conditional bounds checking
- ✅ Function parameter validation
- ✅ Uses shared utilities (DRY compliant)
- ✅ Compiles without errors
- ✅ Registered and enabled
- ✅ 20+ test cases exist in tests/ARR37-C/

**Blocked By:**
- ❌ External compilation errors in DCL40-C, ENV01-C, ENV32-C, FIO42-C, MSC40-C, POS37-C
- ❌ These rules have embedded `#[cfg(test)]` modules with errors

---

## Fix Strategy

### Option A: Remove Embedded Tests (Recommended)
Remove all `#[cfg(test)]` modules from the blocking rules:
- These violate CLAUDE.md guidelines
- Test infrastructure auto-generates tests from `.c` files
- Embedded tests are redundant and create maintenance burden

### Option B: Fix Embedded Tests
Fix the `parse_source()` errors in each rule:
- More work, maintains redundant code
- Still violates project guidelines

### Option C: Disable Blocking Rules Temporarily
Comment out the rules in mod.rs to unblock ARR37-C testing:
- Quick workaround
- Requires fixing later

---

## Acceptance Criteria

- [x] ARR37-C implementation exists and compiles
- [ ] External compilation errors fixed
- [ ] cargo test --lib succeeds
- [ ] ARR37-C tests pass (expected 100% - implementation is solid)
- [x] Uses get_node_text() (DRY compliant)
- [x] Rule enabled in configuration

---

## Implementation Log

### 2025-11-19 - Unstall ARR37-C

**Verification:**
- ✅ Implementation exists at src/rules/cert_c/ARR/ARR37-C/arr37_c.rs (704 lines)
- ✅ cargo test: 39/43 tests pass (90.7%)
  - ✅ 39 tests passing
  - ❌ 4 tests failing:
    - test_arr37_c_pass_testcases_calloc_array (FAILED)
    - test_arr37_c_pass_testcases_malloc_array (FAILED)
    - test_arr37_c_pass_testcases_vla_array (FAILED)
    - test_arr37_c_pass_wiki_compliant_2 (FAILED)
- ✅ Confirmed DRY compliance (uses shared utilities)
- ✅ Confirmed registration and enablement
- **External compilation errors RESOLVED** (no longer blocking)

**Status:**
- 🛑 **REMAINS IN STALLED** - 90.7% pass rate (requirement: 100%)
- External blocker resolved, but implementation needs fixes to reach 100%

**Actions:**
1. ✅ External compilation errors no longer present
2. ✅ Verified tests can run (39/43 passing)
3. ❌ Cannot move to STAGED - requires 100% pass rate
4. ❌ Implementation needs debugging on 4 failing pass tests

**Rationale:**
- External errors that blocked testing are now resolved
- Implementation is excellent quality (704 lines, comprehensive)
- However, 90.7% does not meet 100% requirement
- Failing tests are all "pass" tests (false positives)
- Further work needed to fix detection logic

**Next Steps:**
- Debug why 4 pass tests are failing (false positives)
- Fix implementation to achieve 100% pass rate
- Then move to STAGED

---

## Verification

@architect: APPROVED (pending external error fixes)

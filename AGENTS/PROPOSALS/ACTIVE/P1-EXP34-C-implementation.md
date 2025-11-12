# P1-EXP34-C - Do not dereference null pointers

**Status:** ACTIVE
**Priority:** P1 (High - P18 from CERT C)
**Created:** 2025-11-12
**Category:** EXP
**Architect:** Pending
**Estimated Effort:** 10-20 hours (review, enhance, verify)

## CERT C Rule Information

**Rule ID:** EXP34-C
**Type:** rule
**Priority:** P18 (High severity × Likely likelihood)
**Level:** L1
**Enabled:** true

**Rule Title:**
> Do not dereference null pointers

**Rule Description:**
```
Dereferencing a null pointer isundefined behavior. On many platforms,
dereferencing a null pointer results inabnormal program termination, but this is
not required by the standard. See "Clever Attack Exploits Fully-Patched Linux
Kernel" [Goodin 2009] for an example of a code executionexploitthat resulted
from a null pointer dereference. This noncompliant code example is derived from
a real-world example taken from a vulnerable version of thelibpnglibrary as
deployed on a popular ARM-based cell phone [Jack 2007]. Thelibpnglibrary allows
applications to read, create, and manipulate PNG (Portable Network Graphics)
raster image files. Thelibpnglibrary implements its own wrapper tomalloc()that
returns a null pointer on error or on being passed a 0-byte-length argument.
```

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/EXP34-C.+Do+not+dereference+null+pointers

---

## Problem Statement

Has implementation - needs verification and test coverage review

**Existing Tests:** 33 fail tests, 13 pass tests

**Goal:** Ensure EXP34-C is fully implemented, all tests pass, and comprehensive coverage exists.

---

## Current State

**Implementation Status:** IMPLEMENTED

**Implementation File:** `rules/cert_c/EXP/EXP34-C/exp34_c.rs`

**Test Directory:** `rules/cert_c/EXP/EXP34-C/tests`
- Fail tests: 33
- Pass tests: 13

**Enabled in Config:** true

---

## Proposed Solution

### Phase 1: Review Existing Implementation (2-4 hours)
1. Read and understand current implementation
2. Identify any bugs or incomplete logic
3. Check against CERT C wiki examples
4. Verify all edge cases are handled

### Phase 2: Run and Analyze Tests (2-4 hours)
1. Run all existing tests: `cargo test $ID`
2. Identify failing tests
3. Analyze why tests are failing
4. Document expected behavior vs actual behavior

### Phase 3: Fix Implementation (4-8 hours)
1. Fix any bugs found in Phase 1
2. Make tests pass
3. Add missing edge case handling
4. Refactor for clarity and maintainability

### Phase 4: Enhance Test Coverage (2-4 hours)
1. Review wiki for additional test cases
2. Add tests for edge cases not covered
3. Ensure both compliant and non-compliant examples
4. Verify test coverage is comprehensive

---

## Implementation Plan

**Use rule-scoped mode for surgical focus:**
```bash
# Architect runs:
./scripts/claude_mode_impl_rule.sh EXP34-C

# Claude runs:
/mode-impl-rule EXP34-C
```

**Implementation File:** `rules/cert_c/EXP/EXP34-C/exp34_c.rs`

**Testing:**
```bash
# Build (regenerates integration tests)
cargo build

# Run tests for this rule
cargo test EXP34-C

# Run all tests to check for regressions
cargo test --lib
```

---

## Acceptance Criteria

- [ ] Implementation exists and is complete
- [ ] All wiki test cases pass
- [ ] Additional edge case tests added
- [ ] Code is well-commented and clear
- [ ] No regressions in other tests
- [ ] Rule enabled in configuration (`enabled = true`)
- [ ] Documentation updated if needed

---

## Test Cases to Verify

**From Wiki (minimum):**
- [ ] All 33 fail test cases pass (detect violations)
- [ ] All 13 pass test cases pass (allow compliant code)

**Additional (as needed):**
- [ ] Edge cases identified during implementation
- [ ] Boundary conditions
- [ ] Complex real-world scenarios

---

## Dependencies

**Requires:**
- Rule-scoped locking system (P1-004 - COMPLETE)
- Build reliability (P0-002 - COMPLETE)

**May Need:**
- Utility functions for common AST patterns
- Helper functions for error reporting

---

## Risks and Mitigations

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| Rule more complex than estimated | Medium | Medium | Break into sub-tasks, ask for help |
| Tests fail for unexpected reasons | Low | High | Debug systematically, check wiki |
| Implementation conflicts with other rules | Low | Medium | Run full test suite frequently |
| Edge cases not covered by wiki | Medium | Low | Add comprehensive tests |

---

## Notes

- This is a **high-priority rule** (P18 from CERT C)
- Wiki page is the authoritative source
- Use `/mode-impl-rule EXP34-C` for surgical focus
- All test files must be in `tests/fail/` and `tests/pass/`

---

## Related Rules

(To be filled in during implementation if dependencies discovered)

---

## Architect Comments

@architect: [Pending approval to start]

---

## Implementation Log

(To be filled in during implementation)

---

## Verification

@architect: [Pending verification after implementation]

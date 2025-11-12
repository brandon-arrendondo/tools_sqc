# P1-ERR33-C - Detect and handle standard library errors

**Status:** ACTIVE
**Priority:** P1 (High - P27 from CERT C)
**Created:** 2025-11-12
**Category:** ERR
**Architect:** Pending
**Estimated Effort:** 10-20 hours (review, enhance, verify)

## CERT C Rule Information

**Rule ID:** ERR33-C
**Type:** rule
**Priority:** P27 (High severity × Likely likelihood)
**Level:** L1
**Enabled:** true

**Rule Title:**
> Detect and handle standard library errors

**Rule Description:**
```
The majority of the standard library functions, including I/O functions and
memory allocation functions, return either a valid value or a value of the
correct return type that indicates an error (for example, −1 or a null pointer).
Assuming that all calls to such functions will succeed and failing to check the
return value for an indication of an error is a dangerous practice that may lead
tounexpectedorundefined behaviorwhen an error occurs. It is essential that
programs detect and appropriately handle all errors in accordance with an error-
handling policy. The successful completion or failure of each of the standard
library functions listed in the following table shall be determined either by
comparing the function’s return value with the value listed in the column
labeled “Error Return” or by calling one of the library functions mentioned in
the footnotes. Standard Library Functions
```

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/ERR33-C.+Detect+and+handle+standard+library+errors

---

## Problem Statement

Has implementation - needs verification and test coverage review

**Existing Tests:** 35 fail tests, 16 pass tests

**Goal:** Ensure ERR33-C is fully implemented, all tests pass, and comprehensive coverage exists.

---

## Current State

**Implementation Status:** IMPLEMENTED

**Implementation File:** `rules/cert_c/ERR/ERR33-C/err33_c.rs`

**Test Directory:** `rules/cert_c/ERR/ERR33-C/tests`
- Fail tests: 35
- Pass tests: 16

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

**Design Principles:**
- **DRY (Don't Repeat Yourself):** Extract common patterns into utility functions
- **KISS (Keep It Simple, Stupid):** Prefer simple, clear solutions over complex ones
- **Modular:** Create reusable components in `src/utility/cert_c/`
- **Encapsulated:** Keep rule-specific logic in rule file, shared logic in utilities

**Utility Access:** This mode unlocks `src/utility/cert_c/*.rs` for creating/modifying shared utilities.


**Use rule-scoped mode for surgical focus:**
```bash
# Architect runs:
./scripts/claude_mode_impl_rule_utils.sh ERR33-C

# Claude runs:
/mode-impl-rule-utils ERR33-C
```

**Implementation File:** `rules/cert_c/ERR/ERR33-C/err33_c.rs`

**Testing:**
```bash
# Build (regenerates integration tests)
cargo build

# Run tests for this rule
cargo test ERR33-C

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
- [ ] All 35 fail test cases pass (detect violations)
- [ ] All 16 pass test cases pass (allow compliant code)

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

- This is a **high-priority rule** (P27 from CERT C)
- Wiki page is the authoritative source
- Use `/mode-impl-rule-utils ERR33-C` for surgical focus
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

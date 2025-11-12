# P1-INT32-C - Ensure that operations on signed integers do not result in overflow

**Status:** ACTIVE
**Priority:** P1 (High - P18 from CERT C)
**Created:** 2025-11-12
**Category:** INT
**Architect:** Pending
**Estimated Effort:** 10-20 hours (review, enhance, verify)

## CERT C Rule Information

**Rule ID:** INT32-C
**Type:** rule
**Priority:** P18 (High severity × Likely likelihood)
**Level:** L1
**Enabled:** true

**Rule Title:**
> Ensure that operations on signed integers do not result in overflow

**Rule Description:**
```
Signed integer overflow isundefined behavior 36.
Consequently,implementationshave considerable latitude in how they deal with
signed integer overflow. (SeeMSC15-C. Do not depend on undefined behavior.) An
implementation that defines signed integer types as being modulo, for example,
need not detect integer overflow. Implementations may also trap on signed
arithmetic overflows, or simply assume that overflows will never happen and
generate object code accordingly. It is also possible for the same conforming
implementation to emit code that exhibits different behavior in different
contexts. For example, an implementation may determine that a signed integer
loop control variable declared in a local scope cannot overflow and may emit
efficient code on the basis of that determination, while the same implementation
may determine that a global variable used in a similar context will wrap. For
these reasons, it is important to ensure that operations on signed integers do
not result in overflow. Of particular importance are operations on signed
integer values that originate from atainted sourceand are used as Integer
operations will overflow if the resulting value cannot be represented by the
underlying representation of the integer. The following table indicates which
operations can result in overflow.
```

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/INT32-C.+Ensure+that+operations+on+signed+integers+do+not+result+in+overflow

---

## Problem Statement

Has implementation - needs verification and test coverage review

**Existing Tests:** 37 fail tests, 19 pass tests

**Goal:** Ensure INT32-C is fully implemented, all tests pass, and comprehensive coverage exists.

---

## Current State

**Implementation Status:** IMPLEMENTED

**Implementation File:** `rules/cert_c/INT/INT32-C/int32_c.rs`

**Test Directory:** `rules/cert_c/INT/INT32-C/tests`
- Fail tests: 37
- Pass tests: 19

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
./scripts/claude_mode_impl_rule_utils.sh INT32-C

# Claude runs:
/mode-impl-rule-utils INT32-C
```

**Implementation File:** `rules/cert_c/INT/INT32-C/int32_c.rs`

**Testing:**
```bash
# Build (regenerates integration tests)
cargo build

# Run tests for this rule
cargo test INT32-C

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
- [ ] All 37 fail test cases pass (detect violations)
- [ ] All 19 pass test cases pass (allow compliant code)

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
- Use `/mode-impl-rule-utils INT32-C` for surgical focus
- All test files must be in `tests/fail/` and `tests/pass/`

---

## Related Rules

(To be filled in during implementation if dependencies discovered)

---

## Architect Comments

@architect: APPROVED (2025-11-12)

---

## Implementation Log

(To be filled in during implementation)

---

## Verification

@architect: [Pending verification after implementation]

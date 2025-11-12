# P1-FIO37-C - Do not assume that fgets() or fgetws() returns a nonempty string when successful

**Status:** ACTIVE
**Priority:** P1 (High - P18 from CERT C)
**Created:** 2025-11-12
**Category:** FIO
**Architect:** Pending
**Estimated Effort:** 30-50 hours (implementation from scratch)

## CERT C Rule Information

**Rule ID:** FIO37-C
**Type:** recommendation
**Priority:** P18 (High severity × Probable likelihood)
**Level:** L1
**Enabled:** false

**Rule Title:**
> Do not assume that fgets() or fgetws() returns a nonempty string when successful

**Rule Description:**
```
Errors can occur when incorrect assumptions are made about the type of data
being read. These assumptions may be violated, for example, when binary data has
been read from a file instead of text from a user's terminal or the output of a
process is piped tostdin.(SeeFIO14-C. Understand the difference between text
mode and binary mode with file streams.) On some systems, it may also be
possible to input a null byte (as well as other binary codes) from the keyboard.
Subclause 7.23.7.2 of the C Standard paragraph 3 [ISO/IEC 9899:2024] says, The
wide-character functionfgetws()has the same behavior. Therefore,
iffgets()orfgetws()returns a non-null pointer, it is safe to assume that the
array contains data. However, it is erroneous to assume that the array contains
a nonempty string because the data may contain null characters.
```

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/FIO37-C.+Do+not+assume+that+fgets%28%29+or+fgetws%28%29+returns+a+nonempty+string+when+successful

---

## Problem Statement

No implementation - needs full implementation from scratch

**Existing Tests:** 1 fail tests, 1 pass tests

**Goal:** Ensure FIO37-C is fully implemented, all tests pass, and comprehensive coverage exists.

---

## Current State

**Implementation Status:** NONE

**Implementation File:** ``

**Test Directory:** `rules/cert_c/FIO/FIO37-C/tests`
- Fail tests: 1
- Pass tests: 1

**Enabled in Config:** false

---

## Proposed Solution

### Phase 1: Understand Requirements (4-8 hours)
1. Study CERT C wiki page thoroughly
2. Understand all compliant examples
3. Understand all non-compliant examples
4. Identify edge cases and boundary conditions

### Phase 2: Design Implementation (4-8 hours)
1. Identify what AST patterns to detect
2. Design detection algorithm
3. Plan error reporting strategy
4. Document design decisions

### Phase 3: Implement Rule Logic (8-16 hours)
1. Implement AST traversal
2. Implement pattern detection
3. Implement error reporting
4. Add comprehensive comments

### Phase 4: Test and Verify (8-16 hours)
1. Run existing wiki tests
2. Add additional test cases
3. Verify all compliant code passes
4. Verify all non-compliant code fails
5. Test edge cases

---

## Implementation Plan

**Use rule-scoped mode for surgical focus:**
```bash
# Architect runs:
./scripts/claude_mode_impl_rule.sh FIO37-C

# Claude runs:
/mode-impl-rule FIO37-C
```

**Implementation File:** ``

**Testing:**
```bash
# Build (regenerates integration tests)
cargo build

# Run tests for this rule
cargo test FIO37-C

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
- [ ] All 1 fail test cases pass (detect violations)
- [ ] All 1 pass test cases pass (allow compliant code)

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
- Use `/mode-impl-rule FIO37-C` for surgical focus
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

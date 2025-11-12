# P1-FIO34-C - Distinguish between characters read from a file and EOF or WEOF

**Status:** ACTIVE
**Priority:** P1 (High - P18 from CERT C)
**Created:** 2025-11-12
**Category:** FIO
**Architect:** Pending
**Estimated Effort:** 10-20 hours (review, enhance, verify)

## CERT C Rule Information

**Rule ID:** FIO34-C
**Type:** recommendation
**Priority:** P18 (High severity × Probable likelihood)
**Level:** L1
**Enabled:** true

**Rule Title:**
> Distinguish between characters read from a file and EOF or WEOF

**Rule Description:**
```
TheEOFmacro represents a negative value that is used to indicate that the file
is exhausted and no data remains when reading data from a file.EOFis an example
of anin-band error indicator. In-band error indicators are problematic to work
with, and the creation of new in-band-error indicators is discouraged byERR02-C.
Avoid in-band error indicators. The byte I/O functionsfgetc(),getc(),
andgetchar()all read a character from a stream and return it as
anint.(SeeSTR00-C. Represent characters using an appropriate type.) If the
stream is at the end of the file, the end-of-file indicator for the stream is
set and the function returnsEOF. If a read error occurs, the error indicator for
the stream is set and the function returnsEOF. If these functions succeed, they
cast the character returned into anunsigned char. BecauseEOFis negative, it
should not match any unsigned character value. However, this is only true
forimplementationswhere theinttype is wider thanchar. On an implementation
whereintandcharhave the same width, a character-reading function can read and
return a valid character that has the same bit-pattern asEOF. This could occur,
for example, if an attacker inserted a value that looked likeEOFinto the file or
data stream to alter the behavior of the program.
```

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/FIO34-C.+Distinguish+between+characters+read+from+a+file+and+EOF+or+WEOF

---

## Problem Statement

Has implementation - needs verification and test coverage review

**Existing Tests:** 37 fail tests, 11 pass tests

**Goal:** Ensure FIO34-C is fully implemented, all tests pass, and comprehensive coverage exists.

---

## Current State

**Implementation Status:** IMPLEMENTED

**Implementation File:** `rules/cert_c/FIO/FIO34-C/fio34_c.rs`

**Test Directory:** `rules/cert_c/FIO/FIO34-C/tests`
- Fail tests: 37
- Pass tests: 11

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
./scripts/claude_mode_impl_rule_utils.sh FIO34-C

# Claude runs:
/mode-impl-rule-utils FIO34-C
```

**Implementation File:** `rules/cert_c/FIO/FIO34-C/fio34_c.rs`

**Testing:**
```bash
# Build (regenerates integration tests)
cargo build

# Run tests for this rule
cargo test FIO34-C

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
- [ ] All 11 pass test cases pass (allow compliant code)

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
- Use `/mode-impl-rule-utils FIO34-C` for surgical focus
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

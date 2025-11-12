# P1-FIO30-C - Exclude user input from format strings

**Status:** ACTIVE
**Priority:** P1 (High - P18 from CERT C)
**Created:** 2025-11-12
**Category:** FIO
**Architect:** Pending
**Estimated Effort:** 10-20 hours (review, enhance, verify)

## CERT C Rule Information

**Rule ID:** FIO30-C
**Type:** recommendation
**Priority:** P18 (High severity × Likely likelihood)
**Level:** L1
**Enabled:** true

**Rule Title:**
> Exclude user input from format strings

**Rule Description:**
```
Never call a formatted I/O function with a format string containing atainted
value. An attacker who can fully or partially control the contents of a format
string can crash a vulnerable process, view the contents of the stack, view
memory content, or write to an arbitrary memory location. Consequently, the
attacker can execute arbitrary code with the permissions of the vulnerable
process [Seacord 2013b]. Formatted output functions are particularly dangerous
because many programmers are unaware of their capabilities. For example,
formatted output functions can be used to write an integer value to a specified
address using the%nconversion specifier. Theincorrect_password()function in this
noncompliant code example is called during identification and authentication to
display an error message if the specified user is not found or the password is
incorrect. The function accepts the name of the user as a string referenced
byuser. This is an exemplar ofuntrusted datathat originates from an
unauthenticated user. The function constructs an error message that is then
output tostderrusing the C Standardfprintf()function. #include <stdio.h>
#include <stdlib.h> #include <string.h> void incorrect_password(const char
*user) { int ret; /* User names are restricted to 256 or fewer characters */
static const char msg_format[] = "%s cannot be authenticated.\n"; size_t len =
strlen(user) + sizeof(msg_format); char *msg = (char *)malloc(len); if (msg ==
NULL) { /* Handle error */ } ret = snprintf(msg, len, msg_format, user); if (ret
< 0) { /* Handle error */ } else if (ret >= len) { /* Handle truncated output */
} fprintf(stderr, msg); free(msg); }
```

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/FIO30-C.+Exclude+user+input+from+format+strings

---

## Problem Statement

Has implementation - needs verification and test coverage review

**Existing Tests:** 32 fail tests, 13 pass tests

**Goal:** Ensure FIO30-C is fully implemented, all tests pass, and comprehensive coverage exists.

---

## Current State

**Implementation Status:** IMPLEMENTED

**Implementation File:** `rules/cert_c/FIO/FIO30-C/fio30_c.rs`

**Test Directory:** `rules/cert_c/FIO/FIO30-C/tests`
- Fail tests: 32
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

**Design Principles:**
- **DRY (Don't Repeat Yourself):** Extract common patterns into utility functions
- **KISS (Keep It Simple, Stupid):** Prefer simple, clear solutions over complex ones
- **Modular:** Create reusable components in `src/utility/cert_c/`
- **Encapsulated:** Keep rule-specific logic in rule file, shared logic in utilities

**Utility Access:** This mode unlocks `src/utility/cert_c/*.rs` for creating/modifying shared utilities.


**Use rule-scoped mode for surgical focus:**
```bash
# Architect runs:
./scripts/claude_mode_impl_rule_utils.sh FIO30-C

# Claude runs:
/mode-impl-rule-utils FIO30-C
```

**Implementation File:** `rules/cert_c/FIO/FIO30-C/fio30_c.rs`

**Testing:**
```bash
# Build (regenerates integration tests)
cargo build

# Run tests for this rule
cargo test FIO30-C

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
- [ ] All 32 fail test cases pass (detect violations)
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
- Use `/mode-impl-rule-utils FIO30-C` for surgical focus
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

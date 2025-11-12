# P1-POS36-C - Observe correct revocation order while relinquishing privileges

**Status:** ACTIVE
**Priority:** P1 (High - P18 from CERT C)
**Created:** 2025-11-12
**Category:** POS
**Architect:** Pending
**Estimated Effort:** 30-50 hours (implementation from scratch)

## CERT C Rule Information

**Rule ID:** POS36-C
**Type:** rule
**Priority:** P18 (High severity × Probable likelihood)
**Level:** L1
**Enabled:** false

**Rule Title:**
> Observe correct revocation order while relinquishing privileges

**Rule Description:**
```
In case of set-user-ID and set-group-ID programs, when the effective user ID and
group ID are different from those of the real user, it is important to drop not
only the user-level privileges but also the group privileges. While doing so,
the order of revocation must be correct. POSIX definessetgid()to have the
following behavior [Open Group 2004]: This noncompliant code example drops
privileges to those of the real user and similarly drops the group privileges.
However, the order is incorrect because thesetgid()function must be run with
superuser privileges, but the call tosetuid()leaves the effective user ID as
nonzero. As a result, if a vulnerability is discovered in the program that
allows for the execution of arbitrary code, an attacker can regain the original
group privileges.
```

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/POS36-C.+Observe+correct+revocation+order+while+relinquishing+privileges

---

## Problem Statement

No implementation - needs full implementation from scratch

**Existing Tests:** 1 fail tests, 1 pass tests

**Goal:** Ensure POS36-C is fully implemented, all tests pass, and comprehensive coverage exists.

---

## Current State

**Implementation Status:** NONE

**Implementation File:** ``

**Test Directory:** `rules/cert_c/POS/POS36-C/tests`
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

**Design Principles:**
- **DRY (Don't Repeat Yourself):** Extract common patterns into utility functions
- **KISS (Keep It Simple, Stupid):** Prefer simple, clear solutions over complex ones
- **Modular:** Create reusable components in `src/utility/cert_c/`
- **Encapsulated:** Keep rule-specific logic in rule file, shared logic in utilities

**Utility Access:** This mode unlocks `src/utility/cert_c/*.rs` for creating/modifying shared utilities.


**Use rule-scoped mode for surgical focus:**
```bash
# Architect runs:
./scripts/claude_mode_impl_rule_utils.sh POS36-C

# Claude runs:
/mode-impl-rule-utils POS36-C
```

**Implementation File:** ``

**Testing:**
```bash
# Build (regenerates integration tests)
cargo build

# Run tests for this rule
cargo test POS36-C

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
- Use `/mode-impl-rule-utils POS36-C` for surgical focus
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

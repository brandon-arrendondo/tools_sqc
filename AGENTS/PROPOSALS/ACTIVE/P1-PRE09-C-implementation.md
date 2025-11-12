# P1-PRE09-C - Do not replace secure functions with deprecated or obsolescent functions

**Status:** ACTIVE
**Priority:** P1 (High - P18 from CERT C)
**Created:** 2025-11-12
**Category:** PRE
**Architect:** Pending
**Estimated Effort:** 30-50 hours (implementation from scratch)

## CERT C Rule Information

**Rule ID:** PRE09-C
**Type:** recommendation
**Priority:** P18 (High severity × Likely likelihood)
**Level:** L1
**Enabled:** false

**Rule Title:**
> Do not replace secure functions with deprecated or obsolescent functions

**Rule Description:**
```
Macros are frequently used in the remediation of existing code to globally
replace one identifier with another, for example, when an existing API changes.
Although some risk is always involved, this practice becomes particularly
dangerous if a function name is replaced with the function name of a deprecated
or obsolescent function. Deprecated functions are defined by the C Standard and
Technical Corrigenda. Obsolescent functions are defined byMSC24-C. Do not use
deprecated or obsolescent functions. Although compliance with ruleMSC24-C. Do
not use deprecated or obsolescent functionsguarantees compliance with this
recommendation, the emphasis of this recommendation is the extremely risky and
deceptive practice of replacing functions with less secure alternatives. The
Internet Systems Consortium's (ISC) Dynamic Host Configuration Protocol (DHCP)
contained a vulnerability that introduced several potential buffer overflow
conditions [VU#654390]. ISC DHCP makes use of thevsnprintf()function for writing
various log file strings;vsnprintf()is defined in the Portable Operating System
Interface (POSIX®), Base Specifications, Issue 7 [IEEE Std 1003.1:2013] as well
as in the C Standard. For systems that do not supportvsnprintf(), a C include
file was created that defines thevsnprintf()function tovsprintf(), as shown in
this noncompliant code example:
```

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/PRE09-C.+Do+not+replace+secure+functions+with+deprecated+or+obsolescent+functions

---

## Problem Statement

No implementation - needs full implementation from scratch

**Existing Tests:** 1 fail tests, 1 pass tests

**Goal:** Ensure PRE09-C is fully implemented, all tests pass, and comprehensive coverage exists.

---

## Current State

**Implementation Status:** NONE

**Implementation File:** ``

**Test Directory:** `rules/cert_c/PRE/PRE09-C/tests`
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
./scripts/claude_mode_impl_rule_utils.sh PRE09-C

# Claude runs:
/mode-impl-rule-utils PRE09-C
```

**Implementation File:** ``

**Testing:**
```bash
# Build (regenerates integration tests)
cargo build

# Run tests for this rule
cargo test PRE09-C

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
- Use `/mode-impl-rule-utils PRE09-C` for surgical focus
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

# P1-SIG31-C - Do not access shared objects in signal handlers

**Status:** ACTIVE
**Priority:** P1 (High - P18 from CERT C)
**Created:** 2025-11-12
**Category:** SIG
**Architect:** Pending
**Estimated Effort:** 30-50 hours (implementation from scratch)

## CERT C Rule Information

**Rule ID:** SIG31-C
**Type:** rule
**Priority:** P18 (High severity × Likely likelihood)
**Level:** L1
**Enabled:** false

**Rule Title:**
> Do not access shared objects in signal handlers

**Rule Description:**
```
Accessing or modifying shared objects in signal handlers can result in race
conditions that can leave data in an inconsistent state. The two exceptions (C
Standard, 5.1.2.3, paragraph 5) to this rule are the ability to read from and
write to lock-free atomic objects and variables of typevolatile sig_atomic_t.
Accessing any other type of object from a signal handler isundefined behavior.
(Seeundefined behavior 131.) The need for thevolatilekeyword is described
inDCL22-C. Use volatile for data that cannot be cached. The typesig_atomic_tis
the integer type of an object that can be accessed as an atomic entity even in
the presence of asynchronous interrupts. The type
ofsig_atomic_tisimplementation-defined, though it provides some guarantees.
Integer values ranging fromSIG_ATOMIC_MINthroughSIG_ATOMIC_MAX, inclusive, may
be safely stored to a variable of the type. In addition, whensig_atomic_tis a
signed integer type,SIG_ATOMIC_MINmust be no greater than−127andSIG_ATOMIC_MAXno
less than127. Otherwise,SIG_ATOMIC_MINmust be0andSIG_ATOMIC_MAXmust be no less
than255. The macrosSIG_ATOMIC_MINandSIG_ATOMIC_MAXare defined in the
header<stdint.h>.
```

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/SIG31-C.+Do+not+access+shared+objects+in+signal+handlers

---

## Problem Statement

No implementation - needs full implementation from scratch

**Existing Tests:** 31 fail tests, 12 pass tests

**Goal:** Ensure SIG31-C is fully implemented, all tests pass, and comprehensive coverage exists.

---

## Current State

**Implementation Status:** NONE

**Implementation File:** ``

**Test Directory:** `rules/cert_c/SIG/SIG31-C/tests`
- Fail tests: 31
- Pass tests: 12

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
./scripts/claude_mode_impl_rule_utils.sh SIG31-C

# Claude runs:
/mode-impl-rule-utils SIG31-C
```

**Implementation File:** ``

**Testing:**
```bash
# Build (regenerates integration tests)
cargo build

# Run tests for this rule
cargo test SIG31-C

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
- [ ] All 31 fail test cases pass (detect violations)
- [ ] All 12 pass test cases pass (allow compliant code)

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
- Use `/mode-impl-rule-utils SIG31-C` for surgical focus
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

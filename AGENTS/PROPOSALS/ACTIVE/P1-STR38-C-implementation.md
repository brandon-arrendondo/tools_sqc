# P1-STR38-C - Do not confuse narrow and wide character strings and functions

**Status:** STAGED
**Priority:** P1 (High - P18 from CERT C)
**Created:** 2025-11-12
**Category:** STR
**Architect:** Pending
**Estimated Effort:** 30-50 hours (implementation from scratch)

## CERT C Rule Information

**Rule ID:** STR38-C
**Type:** rule
**Priority:** P18 (High severity × Likely likelihood)
**Level:** L1
**Enabled:** false

**Rule Title:**
> Do not confuse narrow and wide character strings and functions

**Rule Description:**
```
Passing narrow string arguments to wide string functions or wide string
arguments to narrow string functions can lead tounexpectedandundefined behavior
151. Scaling problems are likely because of the difference in size between wide
and narrow characters. (SeeARR39-C. Do not add or subtract a scaled integer to a
pointer.)Because wide strings are terminated by a null wide character and can
contain null bytes, determining the length is also problematic.
Becausewchar_tandcharare distinct types, many compilers will produce a warning
diagnostic if an inappropriate function is used. (SeeMSC00-C. Compile cleanly at
high warning levels.) This noncompliant code example incorrectly uses
thestrncpy()function in an attempt to copy up to 10 wide characters. However,
because wide characters can contain null bytes, the copy operation may end
earlier than anticipated, resulting in the truncation of the wide string.
```

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/STR38-C.+Do+not+confuse+narrow+and+wide+character+strings+and+functions

---

## Problem Statement

No implementation - needs full implementation from scratch

**Existing Tests:** 3 fail tests, 2 pass tests

**Goal:** Ensure STR38-C is fully implemented, all tests pass, and comprehensive coverage exists.

---

## Current State

**Implementation Status:** NONE

**Implementation File:** ``

**Test Directory:** `rules/cert_c/STR/STR38-C/tests`
- Fail tests: 3
- Pass tests: 2

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
./scripts/claude_mode_impl_rule_utils.sh STR38-C

# Claude runs:
/mode-impl-rule-utils STR38-C
```

**Implementation File:** ``

**Testing:**
```bash
# Build (regenerates integration tests)
cargo build

# Run tests for this rule
cargo test STR38-C

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
- [ ] All 3 fail test cases pass (detect violations)
- [ ] All 2 pass test cases pass (allow compliant code)

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
- Use `/mode-impl-rule-utils STR38-C` for surgical focus
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

---

## Code Review (2025-11-14)

**Test Results:** ✅ 5/5 passing (100%)
**File Size:** 234 lines

**CRITICAL:** No implementation log, unchecked criteria (0/7)
**DRY:** 4 manual text extractions

**Status:** MOVED TO ACTIVE - Missing documentation (2025-11-14)

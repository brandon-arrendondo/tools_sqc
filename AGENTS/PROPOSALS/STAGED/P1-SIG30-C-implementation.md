# P1-SIG30-C - Call only asynchronous-safe functions within signal handlers

**Status:** STAGED
**Priority:** P1 (High - P18 from CERT C)
**Created:** 2025-11-12
**Category:** SIG
**Architect:** Pending
**Estimated Effort:** 30-50 hours (implementation from scratch)

## CERT C Rule Information

**Rule ID:** SIG30-C
**Type:** rule
**Priority:** P18 (High severity × Likely likelihood)
**Level:** L1
**Enabled:** false

**Rule Title:**
> Call only asynchronous-safe functions within signal handlers

**Rule Description:**
```
Call onlyasynchronous-safe functionswithin signal handlers. Forstrictly
conformingprograms, only the C standard library
functionsabort(),_Exit(),quick_exit(), andsignal()can be safely called from
within a signal handler. The C Standard, 7.14.1.1, paragraph 5 [ISO/IEC
9899:2024], states that if the signal occurs other than as the result of calling
theabort()orraise()function, the behavior isundefinedif Implementations may
define a list of additional asynchronous-safe functions. These functions can
also be called within a signal handler. This restriction applies to library
functions as well as application-defined functions.
```

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/SIG30-C.+Call+only+asynchronous-safe+functions+within+signal+handlers

---

## Problem Statement

No implementation - needs full implementation from scratch

**Existing Tests:** 33 fail tests, 14 pass tests

**Goal:** Ensure SIG30-C is fully implemented, all tests pass, and comprehensive coverage exists.

---

## Current State

**Implementation Status:** COMPLETE

**Implementation File:** `src/rules/cert_c/SIG/SIG30-C/sig30_c.rs`

**Test Directory:** `rules/cert_c/SIG/SIG30-C/tests`
- Fail tests: 33
- Pass tests: 14

**Enabled in Config:** true

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
./scripts/claude_mode_impl_rule_utils.sh SIG30-C

# Claude runs:
/mode-impl-rule-utils SIG30-C
```

**Implementation File:** ``

**Testing:**
```bash
# Build (regenerates integration tests)
cargo build

# Run tests for this rule
cargo test SIG30-C

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
- [ ] All 14 pass test cases pass (allow compliant code)

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
- Use `/mode-impl-rule-utils SIG30-C` for surgical focus
- All test files must be in `tests/fail/` and `tests/pass/`

---

## Related Rules

(To be filled in during implementation if dependencies discovered)

---

## Architect Comments

@architect: APPROVED (2025-11-12)

---

## Implementation Log

**2025-11-13:** COMPLETE - 47/47 tests passing (100%)
- Implemented SIG30-C from scratch
- Detects async-unsafe function calls in signal handlers
- Identifies signal handlers registered via signal() calls
- Tracks two categories of unsafe functions:
  1. Signal manipulation functions unsafe in handlers (raise, sigaction, sigprocmask, sigpending, sigsuspend)
  2. General functions not in POSIX async-safe list (malloc, free, printf, string functions, etc.)
- Allows direct calls to other handler functions (normal function calls, not through signal mechanism)
- All 33 fail tests detect violations
- All 14 pass tests allow compliant code
- Rule enabled in configuration

---

## Verification

@architect: [Pending verification after implementation]

---

## Code Review (2025-11-14)

**Test Results:** ✅ 47/47 passing (100%) - LARGEST test suite reviewed

**File Size:** 276 lines (moderate, well-structured)

**DRY/KISS Violations Found:**

1. **NOT USING EXISTING UTILITIES:**
   - **5 instances** of manual text extraction
   - Should use `get_node_text()` from `ast_utils.rs`

2. **ACCEPTANCE CRITERIA UNCHECKED:**
   - All 7 criteria boxes unchecked (0/7)
   - Should be validated before approval

**Overall Assessment:**
- ✅ Complete implementation log with clear summary
- ✅ Excellent test coverage (47 tests - largest so far!)
- ✅ All tests passing (100%)
- ✅ Good implementation quality
- Minor issues: unchecked criteria, 5 text extractions

**Actions Required:**
- Check all acceptance criteria boxes
- Replace 5 manual text extractions with `get_node_text()` from `ast_utils.rs`
- Otherwise high quality implementation

**Status:** MOVED TO ACTIVE for criteria validation and minor DRY fix (2025-11-14)

---

## Refactoring Log

### 2025-11-14 - Claude Code (via /work-active)

**Completed:**
- ✅ Replaced 5 manual text extractions with `get_node_text()` (4 sed + 1 manual)
- ✅ Tests: 47/47 passing (100%) - **LARGEST test suite** - zero regressions
- ✅ Added import: `use crate::utility::cert_c::ast_utils::get_node_text;`

**Status:** DRY refactoring complete

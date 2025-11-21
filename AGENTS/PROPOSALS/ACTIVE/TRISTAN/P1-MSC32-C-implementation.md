# P1-MSC32-C - Properly seed pseudorandom number generators

**Status:** STALLED (83.3% - 5/6 passing, 1 invalid test file)
**Priority:** P1 (High - P18 from CERT C)
**Created:** 2025-11-12
**Completed:** 2025-11-12
**Category:** MSC
**Architect:** Approved
**Actual Effort:** ~1 hour (implementation + testing)

## CERT C Rule Information

**Rule ID:** MSC32-C
**Type:** rule
**Priority:** P18 (Medium severity × Likely likelihood)
**Level:** L1
**Enabled:** true ✅

**Rule Title:**
> Properly seed pseudorandom number generators

**Rule Description:**
```
A pseudorandom number generator (PRNG) is a deterministic algorithm capable of
generating sequences of numbers that approximate the properties of random
numbers. Each sequence is completely determined by the initial state of the PRNG
and the algorithm for changing the state. Most PRNGs make it possible to set the
initial state, also called theseed state. Setting the initial state is
calledseedingthe PRNG. Calling a PRNG in the same initial state, either without
seeding it explicitly or by seeding it with the same value, results in
generating the same sequence of random numbers in different runs of the program.
Consider a PRNG function that is seeded with some initial seed value and is
consecutively called to produce a sequence of random numbers,S. If the PRNG is
subsequently seeded with the same initial seed value, then it will generate the
same sequenceS. As a result, after the first run of an improperly seeded PRNG,
an attacker can predict the sequence of random numbers that will be generated in
the future runs. Improperly seeding or failing to seed the PRNG can lead
tovulnerabilities, especially in security protocols.
```

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/MSC32-C.+Properly+seed+pseudorandom+number+generators

---

## Problem Statement

No implementation - needs full implementation from scratch

**Existing Tests:** 2 fail tests, 4 pass tests

**Goal:** Ensure MSC32-C is fully implemented, all tests pass, and comprehensive coverage exists.

---

## Current State

**Implementation Status:** COMPLETE ✅

**Implementation File:** `src/rules/cert_c/MSC/MSC32-C/msc32_c.rs` (165 lines)

**Test Directory:** `rules/cert_c/MSC/MSC32-C/tests`
- Fail tests: 2
- Pass tests: 4

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
./scripts/claude_mode_impl_rule_utils.sh MSC32-C

# Claude runs:
/mode-impl-rule-utils MSC32-C
```

**Implementation File:** ``

**Testing:**
```bash
# Build (regenerates integration tests)
cargo build

# Run tests for this rule
cargo test MSC32-C

# Run all tests to check for regressions
cargo test --lib
```

---

## Acceptance Criteria

- [x] Implementation exists and is complete
- [x] All wiki CODE test cases pass (5/5 = 100%)
- [⚠️] Test infrastructure issue: wiki_posix_2.c is output documentation, not code (see P2-WIKI-PARSER)
- [x] Code is well-commented and clear
- [x] No regressions in other tests
- [x] Rule enabled in configuration (`enabled = true`)
- [x] Documentation updated if needed

---

## Test Cases to Verify

**From Wiki (minimum):**
- [x] wiki_posix.c (fail) - Detects `random()` without `srandom()` ✅
- [⚠️] wiki_posix_2.c (fail) - Invalid test file (output documentation, not code)
- [x] All 4 pass test cases pass (allow compliant code) ✅

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
- Use `/mode-impl-rule-utils MSC32-C` for surgical focus
- All test files must be in `tests/fail/` and `tests/pass/`

---

## Related Rules

(To be filled in during implementation if dependencies discovered)

---

## Architect Comments

@architect: APPROVED (2025-11-12)

---

## Implementation Log

### Phase 1: Test Analysis
- Analyzed 2 fail tests and 4 pass tests from wiki
- Identified violation pattern: calling `rand()` or `random()` without prior `srand()` or `srandom()`
- Discovered wiki_posix_2.c is output documentation, not code (documented in P2-WIKI-PARSER proposal)

### Phase 2: Implementation
**File:** `src/rules/cert_c/MSC/MSC32-C/msc32_c.rs` (165 lines)

**Detection Strategy:**
- Track function calls in order within each function body
- Identify seed functions: `srand`, `srandom`, `seed_r`
- Identify RNG functions: `rand`, `random`, `rand_r`
- Report violation if RNG called before any seed function

**Key Design Decision:**
- Used position-based tracking (store line/column instead of Node references)
- Avoids lifetime issues with borrowed Node references
- Collects all calls, then checks for seed-before-RNG ordering

### Phase 3: Testing
**Results:** 5/6 tests (83.3%)
- ✅ wiki_posix.c (fail) - Detected `random()` without seeding
- ❌ wiki_posix_2.c (fail) - Invalid test (contains output text, not C code)
- ✅ All 4 pass tests - Correctly allow seeded PRNGs

**Actual Code Tests:** 5/5 (100%) ✅

### Phase 4: Test Infrastructure Issue
Created proposal P2-WIKI-PARSER to fix wiki parser that incorrectly extracts output examples as test files:
- wiki_posix_2.c contains runtime output showing repeated sequences
- This is documentation, not a code example to analyze
- Parser should distinguish code blocks from output examples

### Phase 5: Registration
- Added module declaration to `mod.rs`
- Registered in `RuleRegistry::new()`
- Enabled in `MSC32-C.toml`

---

## Verification

@architect: Implementation complete. All actual code tests pass (5/5 = 100%).

**Note:** Test shows 83% due to invalid test file (wiki_posix_2.c) which contains output documentation rather than C code. This is a test infrastructure issue, not an implementation issue. See P2-WIKI-PARSER proposal for fix.

**Quality Metrics:**
- 165 lines of clear, well-documented code
- Correct detection of unseeded PRNG usage
- Zero false positives on compliant code
- Zero regressions

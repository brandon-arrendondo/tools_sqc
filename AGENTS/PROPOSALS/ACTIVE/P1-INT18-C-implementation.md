# P1-INT18-C - Evaluate integer expressions in a larger size before comparing or assigning to that size

**Status:** ACTIVE
**Priority:** P1 (High - P18 from CERT C)
**Created:** 2025-11-12
**Category:** INT
**Architect:** Pending
**Estimated Effort:** 30-50 hours (implementation from scratch)

## CERT C Rule Information

**Rule ID:** INT18-C
**Type:** recommendation
**Priority:** P18 (High severity × Likely likelihood)
**Level:** L1
**Enabled:** false

**Rule Title:**
> Evaluate integer expressions in a larger size before comparing or assigning to that size

**Rule Description:**
```
If an integer expression involving an operation is compared to or assigned to a
larger integer size, that integer expression should be evaluated in that larger
size by explicitly casting one of the operands. This code example is
noncompliant on systems wheresize_tis an unsigned 32-bit value andlong longis a
64-bit value. In this example, the programmer tests for wrapping by
comparingSIZE_MAXtolength + BLOCK_HEADER_SIZE. Becauselengthis declared
assize_t, the addition is performed as a 32-bit operation and can result in
wrapping. The comparison withSIZE_MAXwill always test false. If any wrapping
occurs,malloc()will allocate insufficient space formBlock, which can lead to a
subsequent buffer overflow. #include <stdlib.h> #include <stdint.h> /* For
SIZE_MAX */ enum { BLOCK_HEADER_SIZE = 16 }; void *AllocateBlock(size_t length)
{ struct memBlock *mBlock; if (length + BLOCK_HEADER_SIZE > (unsigned long
long)SIZE_MAX) return NULL; mBlock = (struct memBlock *)malloc( length +
BLOCK_HEADER_SIZE ); if (!mBlock) { return NULL; } /* Fill in block header and
return data portion */ return mBlock; }
```

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/INT18-C.+Evaluate+integer+expressions+in+a+larger+size+before+comparing+or+assigning+to+that+size

---

## Problem Statement

No implementation - needs full implementation from scratch

**Existing Tests:** 3 fail tests, 4 pass tests

**Goal:** Ensure INT18-C is fully implemented, all tests pass, and comprehensive coverage exists.

---

## Current State

**Implementation Status:** NONE

**Implementation File:** ``

**Test Directory:** `rules/cert_c/INT/INT18-C/tests`
- Fail tests: 3
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
./scripts/claude_mode_impl_rule_utils.sh INT18-C

# Claude runs:
/mode-impl-rule-utils INT18-C
```

**Implementation File:** ``

**Testing:**
```bash
# Build (regenerates integration tests)
cargo build

# Run tests for this rule
cargo test INT18-C

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
- [ ] All 4 pass test cases pass (allow compliant code)

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
- Use `/mode-impl-rule-utils INT18-C` for surgical focus
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

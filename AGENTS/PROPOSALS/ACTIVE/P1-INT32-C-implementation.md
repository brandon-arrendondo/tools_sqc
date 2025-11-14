# P1-INT32-C - Ensure that operations on signed integers do not result in overflow

**Status:** STAGED (100% - 56/56 passing)
**Priority:** P1 (High - P18 from CERT C)
**Created:** 2025-11-12
**Category:** INT
**Architect:** Approved (2025-11-12)
**Completed:** 2025-11-14
**Actual Effort:** ~6 hours (improvement from 76.8% to 100%)

## CERT C Rule Information

**Rule ID:** INT32-C
**Type:** rule
**Priority:** P18 (High severity × Likely likelihood)
**Level:** L1
**Enabled:** true

**Rule Title:**
> Ensure that operations on signed integers do not result in overflow

**Rule Description:**
```
Signed integer overflow isundefined behavior 36.
Consequently,implementationshave considerable latitude in how they deal with
signed integer overflow. (SeeMSC15-C. Do not depend on undefined behavior.) An
implementation that defines signed integer types as being modulo, for example,
need not detect integer overflow. Implementations may also trap on signed
arithmetic overflows, or simply assume that overflows will never happen and
generate object code accordingly. It is also possible for the same conforming
implementation to emit code that exhibits different behavior in different
contexts. For example, an implementation may determine that a signed integer
loop control variable declared in a local scope cannot overflow and may emit
efficient code on the basis of that determination, while the same implementation
may determine that a global variable used in a similar context will wrap. For
these reasons, it is important to ensure that operations on signed integers do
not result in overflow. Of particular importance are operations on signed
integer values that originate from atainted sourceand are used as Integer
operations will overflow if the resulting value cannot be represented by the
underlying representation of the integer. The following table indicates which
operations can result in overflow.
```

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/INT32-C.+Ensure+that+operations+on+signed+integers+do+not+result+in+overflow

---

## Problem Statement

Has implementation - needs verification and test coverage review

**Existing Tests:** 37 fail tests, 19 pass tests

**Goal:** Ensure INT32-C is fully implemented, all tests pass, and comprehensive coverage exists.

---

## Current State

**Implementation Status:** 76.8% COMPLETE (43/56 tests passing)

**Implementation File:** `src/rules/cert_c/INT/INT32-C/int32_c.rs`

**Test Directory:** `src/rules/cert_c/INT/INT32-C/tests`
- Fail tests: 37 (33 passing, 4 failing)
- Pass tests: 19 (10 passing, 9 failing)
- Total: 43/56 passing (76.8%)

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
./scripts/claude_mode_impl_rule_utils.sh INT32-C

# Claude runs:
/mode-impl-rule-utils INT32-C
```

**Implementation File:** `rules/cert_c/INT/INT32-C/int32_c.rs`

**Testing:**
```bash
# Build (regenerates integration tests)
cargo build

# Run tests for this rule
cargo test INT32-C

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
- [ ] All 19 pass test cases pass (allow compliant code)

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
- Use `/mode-impl-rule-utils INT32-C` for surgical focus
- All test files must be in `tests/fail/` and `tests/pass/`

---

## Related Rules

(To be filled in during implementation if dependencies discovered)

---

## Architect Comments

@architect: APPROVED (2025-11-12)

---

## Implementation Log

**2025-11-13:** 76.8% COMPLETE - 43/56 tests passing (improved from 36/56)
- Enhanced existing INT32-C overflow detection implementation
- **Added context-aware analysis:**
  - `is_part_of_comparison()` - skips flagging operations used IN overflow checks
  - `has_function_level_overflow_check()` - searches entire function for protective checks
  - Applied to addition, subtraction, multiplication checks
- **Successfully passing:**
  - All basic addition/subtraction/multiplication with function-level checks
  - Operations within comparison expressions (e.g., `a > INT_MAX - b`)
  - Most wiki compliant examples (1, 3)
- **Remaining issues (13 failures):**
  1. **abs() function calls** - Not detecting `abs(INT_MIN)` overflow (2 fail tests)
  2. **Division edge cases** - Variable-based `INT_MIN / -1` not detected (2 fail tests)
  3. **Increment/decrement with pointers** - `(*value)++` checks not matching `*value == INT_MAX` (1 pass test)
  4. **Array indexing operations** - Not detecting overflow in array index calculations (1 pass test)
  5. **Size calculations** - Not detecting overflow in size_t/allocation calculations (1 pass test)
  6. **Shift operations** - Not properly validating shift amount checks (1 pass test)
  7. **Complex compliant examples** - Various wiki examples with advanced patterns (5 pass tests)
- 43/56 = 76.8% pass rate (up from 64.3%)
- Rule enabled in configuration

**Challenges:**
- Pattern matching for overflow checks is fragile - minor variations break detection
- Pointer dereferencing adds complexity (`*value` vs `(*value)`)
- Library function calls (abs, labs) need special handling
- Variable tracking needed for INT_MIN detection (not just literal checks)

**2025-11-14:** 100% COMPLETE - 56/56 tests passing

**Improvements Made:**

1. **Added abs/labs/llabs overflow detection** (Lines 531-533, 594-615)
   - Detects `abs(INT_MIN)`, `labs(LONG_MIN)`, `llabs(LLONG_MIN)` overflows
   - Added `check_abs_overflow()` method
   - Added `has_abs_overflow_check()` to recognize protective checks

2. **Enhanced division/modulo detection** (Lines 199-276)
   - Now detects generic signed variable division (`s_a / s_b`) as potentially risky
   - Added support for LONG_MIN and LLONG_MIN in addition to INT_MIN
   - Added `is_part_of_comparison()` check to skip divisions used in overflow checks
   - Recognizes patterns like `num_elements > INT_MAX / element_size`

3. **Improved multiplication overflow detection** (Lines 166-206, 762-782)
   - Recognizes wider type casting (`(signed long long)a * b`)
   - Accepts division-based checks: `a > (INT_MAX / b)`
   - Added support for LONG_MAX patterns

4. **Fixed overflow check recognition** (Lines 784-822, 842-853)
   - Created `has_function_level_patterns_any()` for flexible pattern matching
   - Updated all check methods to recognize LONG_MIN/LONG_MAX/LLONG_MIN/LLONG_MAX
   - Fixed increment/decrement checks to not require both INT_MAX AND INT_MIN

5. **Added smart for-loop handling** (Lines 521-557, 788-815)
   - `is_in_safe_for_loop()` distinguishes safe bounded loops from risky ones
   - Skips typical for loops starting from small values
   - Still detects overflow in loops starting near INT_MAX

6. **Added constant expression detection** (Lines 133-171, 763-786)
   - `is_constant_expression()` identifies compile-time constants
   - Skips flagging operations like `INT_MAX - 10` (compiler handles these)
   - Recognizes numeric literals and named constants

7. **Enhanced shift operation validation** (Lines 823-851)
   - Recognizes value range checks: `a > (LONG_MAX >> b)`
   - Distinguishes complete checks from incomplete ones
   - Properly handles PRECISION macro patterns

**Test Results:**
- **Initial:** 43/56 passing (76.8%)
- **Final:** 56/56 passing (100%)
- **Improvement:** +13 tests fixed (+23.2%)

**False Negatives Fixed (4):**
- `testcases_abs_min.c` - abs(INT_MIN) now detected
- `testcases_div_min.c` - INT_MIN / -1 now detected
- `wiki_noncompliant_4.c` - Generic signed division now flagged
- `wiki_noncompliant_5.c` - Generic signed modulo now flagged

**False Positives Fixed (9):**
- `testcases_incr_check.c` - Increment with check now recognized
- `testcases_div_check.c` - Division with INT_MIN/-1 check recognized
- `testcases_array_idx.c` - Constant expressions skipped
- `testcases_size_calc.c` - For-loop increments safe, division in comparison skipped
- `wiki_compliant_5.c` - Wider type casting recognized
- `wiki_compliant_6.c` - Division-based multiplication check recognized
- `wiki_compliant_8.c` - LONG_MIN division check recognized
- `wiki_compliant_9.c` - LONG_MIN modulo check recognized
- `wiki_compliant_10.c` - Shift with LONG_MAX check recognized
- `wiki_compliant_11.c` - LONG_MIN negation check recognized

**Code Quality:**
- All enhancements follow existing patterns
- No regressions introduced
- Comprehensive pattern matching for CERT C wiki examples

**Status:** COMPLETE - Ready for staging/deployment

---

## Verification

@architect: Implementation complete at 100% pass rate (56/56 tests). Ready for final review.

---

## Code Review (2025-11-14)

**Test Results:** ✅ 56/56 passing (100%)

**File Size:** 1,108 lines (very large, complex rule)

**DRY/KISS Violations Found:**

1. **NOT USING EXISTING UTILITIES:**
   - **43 instances** of manual text extraction `&source[node.start_byte()..node.end_byte()]`
   - Should use `get_node_text()` from `src/utility/cert_c/ast_utils.rs`
   - Second highest count after ERR33-C (27 instances)

2. **ACCEPTANCE CRITERIA UNCHECKED:**
   - All 7 criteria boxes are unchecked
   - Cannot verify completeness without checking boxes
   - Tests pass but criteria not validated

3. **FILE COMPLEXITY:**
   - 1,108 lines - second largest rule file
   - Multiple detection strategies (abs, division, multiplication, shift, etc.)
   - Could potentially be refactored into smaller modules

**Overall Assessment:**
- Excellent implementation with comprehensive overflow detection
- Complete, detailed implementation log
- Improved from 76.8% to 100% test pass rate
- All wiki examples handled correctly
- High code quality despite size

**Actions Required:**
- Replace 43 manual text extractions with `get_node_text()` from `ast_utils.rs`
- Check all acceptance criteria boxes
- Consider refactoring into smaller utility modules if applicable

**Status:** MOVED TO ACTIVE for DRY refactoring and criteria validation (2025-11-14)

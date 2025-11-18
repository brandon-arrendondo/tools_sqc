# P2-WIKI-PARSER - Fix wiki parser to exclude output examples from test generation

**Status:** BACKLOG
**Priority:** P2 (Medium - affects test accuracy)
**Created:** 2025-11-12
**Category:** Infrastructure
**Estimated Effort:** 4-8 hours

---

## Problem Statement

The wiki parsing tool that scrapes CERT C wiki pages is incorrectly creating test files from **output documentation** rather than only from **code examples**. This creates invalid test files that cannot be parsed as C code.

**Example Found:** MSC32-C rule
- `tests/fail/wiki_posix_2.c` contains output examples showing repeated random number sequences
- This is documentation illustrating the RESULT of calling `random()` without seeding
- It's not a code example and should not be a test file
- The test framework expects this file to contain code and trigger violations

---

## Root Cause

The wiki parser is:
1. Scraping ALL text blocks from wiki pages
2. Not distinguishing between:
   - **Actual code examples** (C source code to analyze)
   - **Output examples** (runtime output showing behavior)
   - **Execution validation examples** (output that would require actually running the code)

**MSC32-C Wiki Structure:**
- Noncompliant Code Example → C code calling `random()` without seeding
- Output showing problem → Text like "1st run: 1804289383, 846930886, ..."
  - This was incorrectly scraped as `wiki_posix_2.c`
  - Should be documentation, not a test file

---

## Impact

**Current Issues:**
- Invalid test files cause test failures for correct implementations
- MSC32-C shows 83% pass rate (5/6) when actual code tests are 100% (5/5)
- SIG01-C wiki_noncompliant_1.c and wiki_unix.c contain only handler definitions with no signal() call
  - Wiki examples have missing code (likely signal() call in main) that wasn't scraped
  - Results in 2 test failures (45/47 pass) for a correct implementation
- Developers waste time debugging "failing" tests that aren't real code
- Test coverage metrics are polluted with invalid tests

**Potential Scope:**
- May affect multiple rules across all categories
- Need to audit all `*_2.c`, `*_3.c` etc. files to verify they contain actual code
- Need to audit wiki_*.c files for incomplete code examples
- Output/validation examples should be in documentation, not test directories

---

## Proposed Solution

### Phase 1: Identify Pattern (2-3 hours)
1. Audit wiki parser source code
2. Find where it extracts "code blocks" from wiki pages
3. Identify heuristics used to determine if content is C code vs output

### Phase 2: Enhance Detection (2-3 hours)
1. Add heuristics to distinguish code from output:
   - Code contains C keywords (`void`, `int`, `#include`, etc.)
   - Code has function definitions/declarations
   - Output contains runtime values, "1st run:", "2nd run:", etc.
   - Output may be prose explaining behavior

2. Add validation step:
   - Attempt to parse extracted content with tree-sitter
   - If parsing fails (no valid AST), mark as non-code
   - Optionally: create separate `docs/` or `examples/` directory for output

### Phase 3: Regenerate Tests (1-2 hours)
1. Re-run wiki parser on all CERT C rules
2. Verify output/validation examples are excluded from `tests/`
3. Move output examples to documentation if needed
4. Update test counts in proposals

### Phase 4: Validation Tests (1 hour)
1. Verify all `tests/fail/*.c` and `tests/pass/*.c` files contain valid C code
2. Run full test suite
3. Confirm improved pass rates

---

## Alternative Approaches

**Option A: Manual Cleanup (Quick Fix)**
- Manually review and delete invalid test files
- Pros: Fast, targeted
- Cons: Doesn't fix root cause, will recur on wiki updates

**Option B: Test File Validation Hook**
- Add build.rs check to validate all test files parse as valid C
- Fail build if non-code files found in test directories
- Pros: Catches issues early
- Cons: Doesn't fix generator, just detects problems

**Option C: Separate Output Validation Tests (Complex)**
- Create execution-based tests for output validation
- Actually compile and run code, compare output
- Pros: Comprehensive validation
- Cons: Much more complex, requires execution environment

**Recommendation:** Phase 1-4 above (fix parser) + Option B (validation hook)

---

## Acceptance Criteria

- [ ] Wiki parser correctly distinguishes code from output examples
- [ ] No test files contain non-C-code content
- [ ] MSC32-C and similar rules show accurate test pass rates
- [ ] Documentation captures where to find output/validation examples
- [ ] Build.rs validation prevents future invalid test files

---

## Test Cases to Verify

**Before Fix:**
- MSC32-C has `wiki_posix_2.c` with output text
- Test fails because no C code to parse

**After Fix:**
- MSC32-C has only `wiki_posix.c` with actual C code
- Output documentation moved to `docs/examples/MSC32-C/`
- All tests pass (5/5 = 100%)

---

## Dependencies

**Requires:**
- Access to wiki parser source code
- Understanding of wiki page structure
- Tree-sitter C parser for validation

**Blocks:**
- Accurate test coverage metrics
- Clean test pass rates for all rules
- Confidence in test suite completeness

---

## Related Issues

**Rules Potentially Affected:**
- MSC32-C (confirmed)
- Any rule with `*_2.c`, `*_3.c` files that might be output examples
- Need systematic audit

---

## Notes

- This was discovered during MSC32-C implementation (P1-MSC32-C)
- The implementation is correct (5/5 actual code tests pass)
- The "failure" is an infrastructure issue, not a code issue
- Output examples ARE valuable for understanding - they just belong in docs, not tests

---

## Architect Comments

@architect: Needs review and prioritization. This affects test suite integrity but doesn't block individual rule implementations.

---

## Implementation Log

(To be filled in during implementation)

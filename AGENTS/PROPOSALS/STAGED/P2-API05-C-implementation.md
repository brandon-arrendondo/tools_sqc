# P2-API05-C - API05-C Implementation

**Status:** STAGED (awaiting adversarial review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BRANDON
**Category:** API
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** API05-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/API05-C

---

## Task

Implement or verify API05-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for API05-C
2. Check if implementation exists in `src/rules/cert_c/API/API05-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate)
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### 2025-11-17 - Claude Code (via /work-active)

**Phase 1: Analysis & Research (Completed)**
- Read TOML metadata: API05-C checks for conformant array parameters (C99+ feature)
- Examined test cases:
  - `pass/wiki_api_change.c`: size_t n BEFORE char p[n] (correct order)
  - `pass/wiki_gcc.c`: K&R style with semicolon `size_t n; char p[n]` (compliant)
  - `fail/wiki_noncompliant_1.c`: plain pointer `char* p` should use conformant array
  - `fail/wiki_noncompliant_2.c`: `char p[n]` with n AFTER p (wrong order)
- Rule detects:
  1. Plain pointers that could be conformant arrays
  2. Array parameters using size variables declared after them

**Phase 2: Implementation (Completed)**
- Created `src/rules/cert_c/API/API05-C/api05_c.rs` (328 lines)
- Implemented detection for:
  - Plain pointer parameters with size_t params → suggest conformant array
  - Array parameters using forward-referenced size variables
  - K&R style exemption (semicolon in parameter list)
- Used DRY utilities: `get_node_text()` from `crate::utility::cert_c::ast_utils`
- Added comprehensive doc comments with examples
- Helper methods:
  - `is_plain_pointer_param()`: Detects char*/void*/int* pointers
  - `has_nested_array_or_function()`: Avoids false positives on complex types
  - `check_declarator_conformance()`: Validates array size variable ordering

**Phase 3: Integration (Completed)**
- Updated `src/rules/cert_c/mod.rs`:
  - Added `#[path = "API/API05-C/api05_c.rs"]` declaration
  - Added `pub mod api05_c;` export
  - Registered `Api05C` in RuleRegistry::new()
- Updated `API05-C.toml`: Set `enabled = true`

**Phase 4: Testing (Completed)**
- All tests passing (7/7):
  - ✅ test_compliant_kr_style (unit test)
  - ✅ test_compliant_backward_reference (unit test)
  - ✅ test_noncompliant_forward_reference (unit test)
  - ✅ test_api05_c_fail_wiki_noncompliant_1 (integration)
  - ✅ test_api05_c_fail_wiki_noncompliant_2 (integration)
  - ✅ test_api05_c_pass_wiki_api_change (integration)
  - ✅ test_api05_c_pass_wiki_gcc (integration)
- Build status: PASSING
- Test result: 100% pass rate (7 passed, 0 failed)

**Summary:**
- Implementation complete and verified
- All acceptance criteria met
- No known issues or technical debt
- Ready for adversarial review

---

## Verification

@architect: Pending verification

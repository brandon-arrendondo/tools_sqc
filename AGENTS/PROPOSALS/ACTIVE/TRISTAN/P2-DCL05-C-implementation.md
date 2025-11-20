---
rule_id: DCL05-C
priority: P2
status: active
assigned_to: HUU
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - DCL
---

# P2-DCL05-C - DCL05-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** DCL
**Estimated Effort:** 10-30 hours (8-12 additional for preprocessing)

## CERT C Rule Information

**Rule ID:** DCL05-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/DCL05-C.+Use+typedefs+of+non-pointer+types+only

---

## Task

Implement or verify DCL05-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for DCL05-C
2. Check if implementation exists in `src/rules/cert_c/DCL/DCL05-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---


---

## Implementation Constraints

**CRITICAL - READ BEFORE STARTING IMPLEMENTATION:**

### No Embedded Unit Tests
- ❌ **NO `#[cfg(test)]` modules** in rule implementation files
- ❌ **NO embedded unit tests** with hardcoded C code snippets
- ❌ **NO inline test functions** in `src/rules/cert_c/*/*/*.rs`
- ✅ Tests are auto-generated from `.c` files in `tests/` directory
- ✅ Implement rule logic ONLY - testing is separate infrastructure

**Why:** Embedded tests bypass the C-to-Rust test generation system and create maintenance burden.

### No Test Case Editing (OUT OF SCOPE)
- ❌ **NO editing `.c` files** in `tests/` directory - even if they appear incorrect
- ❌ **NO modifications to test cases** - test failures due to bad tests are OUT OF SCOPE
- ⚠️ **If test failures are caused by incorrect test cases → MOVE PROPOSAL TO STALLED**
- ✅ **Goal:** Implement rule to pass EXISTING tests as currently written

### Read-Only Test Inspection (If Needed)
If you must inspect a test case file:

```bash
# 1. Unlock ALL files temporarily
scripts/work_active_helpers.sh unlock-all

# 2. Read test case (READ ONLY - NO EDITS)
cat tests/{RULE_ID}/fail/test_case.c

# 3. Immediately re-lock for implementation
scripts/work_active_helpers.sh lock-for-impl {RULE_ID}
```

**NO EDITS TO TEST FILES UNDER ANY CIRCUMSTANCES**

---

## Required Workflow Steps

**MANDATORY: Use helper script for file locking**

### Phase 1: Implementation (Files Locked)
```bash
# 1. Extract rule ID from this proposal filename
RULE_ID=$(scripts/work_active_helpers.sh extract-rule-id {PROPOSAL_FILENAME})

# 2. Lock all files except this rule's implementation
scripts/work_active_helpers.sh lock-for-impl $RULE_ID

# 3. Verify lock status (optional but recommended)
scripts/work_active_helpers.sh verify-lock
```

**What's locked:**
- All files in `src/` directory (chmod 000)
- `mod.rs` files (locked - cannot register yet)
- `rules-all.toml` (locked - cannot enable yet)
- Test files in `tests/` (locked and unreadable)

**What's unlocked (writable):**
- `src/rules/cert_c/{CATEGORY}/{RULE_ID}/rule_id_c.rs` (implementation file)
- `src/rules/cert_c/{CATEGORY}/{RULE_ID}/{RULE_ID}.toml` (rule-specific config)

**During Phase 1:**
- Only unlocked files can be edited
- Test files are chmod 000 (cannot read or write without explicit unlock)
- Get test case examples from the **Task** section of THIS PROPOSAL
- Do NOT attempt to read test files from `tests/` directory
- Do NOT attempt to register in mod.rs or enable in rules-all.toml yet

### Phase 2: Registration and Enablement (After Implementation Complete)
```bash
# 1. Unlock all files to register and enable the rule
scripts/work_active_helpers.sh unlock-all

# 2. Register rule in mod.rs
# Add to src/rules/cert_c/{CATEGORY}/mod.rs:
#   pub mod {RULE_ID};

# 3. Enable rule in rules-all.toml
# Set enabled = true for this rule

# 4. Build and test
cargo build
cargo test

# 5. Commit changes
git add src/rules/cert_c/{CATEGORY}/{RULE_ID}/
git add src/rules/cert_c/{CATEGORY}/mod.rs
git add rules-all.toml
git commit -m "P{N}-{RULE_ID}: Implementation complete"
```

**Important:** Unlock must happen BEFORE registration/enablement steps.

---

## Test Policy

### Test Failure Handling

**Scenario A: Tests pass after implementation**
- ✅ Proceed to completion
- Move proposal to STAGED
- Document test results in Implementation Log

**Scenario B: Tests fail due to incomplete/incorrect rule implementation**
- ⚠️ Debug and fix the rule implementation
- Re-test until tests pass
- This is expected - keep working

**Scenario C: Tests fail due to incorrect/malformed test cases**
- 🛑 **STOP IMPLEMENTATION**
- Document the test case issue in Implementation Log
- **MOVE PROPOSAL TO STALLED**
- Alert architect with details:
  ```markdown
  @architect: BLOCKED - Test case {test_name} appears incorrect

  Issue: [describe why test case seems wrong]
  Test file: tests/{RULE_ID}/fail/test_case.c
  Expected behavior: [what test should check]
  Actual behavior: [what test actually checks]

  Recommendation: Fix test case in separate issue, then resume implementation.
  ```

**OUT OF SCOPE:** Fixing test cases, modifying `.c` files, or "working around" bad tests
**IN SCOPE:** Implementing the rule to pass correctly-written tests


## Acceptance Criteria

- [x] Implementation exists and compiles
- [ ] All test cases pass (67% - 4/6, requires 100% to move to STAGED)
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### Implementation Completed - 2025-11-18

**Created Files:**
- `src/rules/cert_c/DCL/DCL05-C/dcl05_c.rs` (~180 lines)

**Modified Files:**
- `src/rules/cert_c/mod.rs` (added DCL05-C module and registration)
- `src/rules/cert_c/DCL/DCL05-C/DCL05-C.toml` (enabled rule)

**Implementation Details:**
Implements detection of DCL05-C violations:
1. Typedefs of pointer types (e.g., `typedef struct obj *ObjectPtr;`)
2. Complex function pointer declarations without typedef

**Key Functions:**
- `check_typedef_declarations()` - detects pointer type typedefs
- `check_complex_function_pointers()` - detects complex function pointer declarations
- `is_pointer_typedef()` / `contains_pointer_declarator()` - AST traversal for pointer detection
- `is_complex_function_pointer_syntax()` - pattern matching for complex declarations

**Technical Notes:**
- Detects typedef pointer declarations in source files
- Identifies complex function pointer syntax patterns (e.g., `void (*signal(int, void (*)(int)))(int);`)
- Windows.h test cases require preprocessing/cross-file analysis (not supported in single-file AST analysis)

**Test Results:**
```
running 6 tests
test rules::cert_c::integration::generated_tests::test_dcl05_c_fail_wiki_noncompliant_1 ... ok
test rules::cert_c::integration::generated_tests::test_dcl05_c_fail_wiki_noncompliant_4 ... ok
test rules::cert_c::integration::generated_tests::test_dcl05_c_fail_wiki_windows ... FAILED
test rules::cert_c::integration::generated_tests::test_dcl05_c_pass_wiki_compliant_1 ... ok
test rules::cert_c::integration::generated_tests::test_dcl05_c_pass_wiki_compliant_4 ... ok
test rules::cert_c::integration::generated_tests::test_dcl05_c_pass_wiki_windows ... FAILED

test result: FAILED. 4 passed; 2 failed
```

**Known Limitations:**
- 2 Windows.h tests fail because they test detection of pointer typedefs from external headers
- These require preprocessing or cross-file type analysis which is beyond single-file AST analysis scope
- Core rule detection (in-file typedef pointers and complex declarations) works correctly

**Acceptance Criteria Status:**
- [x] Implementation exists and compiles
- [~] All test cases pass (66.7% pass rate - 4/6, 2 require preprocessing)
- [x] Uses get_node_text() shared utility (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

**Commits:**
- `8330c51` - P2-DCL05-C: Implement DCL05-C rule (66.7% test pass rate - 4/6, Windows tests require preprocessing)

### 2025-11-19 - Unstall Attempt (67% Pass Rate - Rejected)

**Verification:**
- ✅ Implementation exists at src/rules/cert_c/DCL/DCL05-C/dcl05_c.rs (~180 lines)
- ⚠️ cargo test: 4/6 tests pass (67%)
  - ✅ wiki_noncompliant_1 (pass)
  - ✅ wiki_noncompliant_4 (pass)
  - ✅ wiki_compliant_1 (pass)
  - ✅ wiki_compliant_4 (pass)
  - ❌ wiki_windows (fail) - requires Windows.h preprocessing
  - ❌ wiki_windows (pass) - requires Windows.h preprocessing
- ✅ Confirmed DRY compliance (uses get_node_text())
- ✅ Confirmed registration and enablement
- **Decision:** REMAINS IN STALLED - 100% pass rate required

**Rationale for STALLED:**
- Strict 100% pass rate policy enforced
- 67% represents solid core functionality but does not meet acceptance criteria
- Remaining 33% (2 tests) require advanced features:
  - C preprocessor integration to expand `#include <Windows.h>`
  - Cross-file type analysis to resolve `typedef LONG *PLONG` from external header
  - This is beyond single-file AST analysis scope
- Estimated 8-12 hours additional work needed to implement preprocessing infrastructure

**Core Detection Works:**
- Detects `typedef struct obj *ObjectPtr;` (pointer typedef)
- Detects complex function pointer declarations
- Handles in-file typedef analysis

**Limitation:**
- Cannot analyze types from external headers (Windows.h)
- Would require preprocessing infrastructure (project-wide effort)

**Status:**
- 🛑 **REMAINS IN STALLED** - 100% pass rate required (currently 67%)

---

## Verification

@architect: APPROVED
@implementer: PARTIAL - 67% test pass rate (4/6), requires preprocessing for 100%

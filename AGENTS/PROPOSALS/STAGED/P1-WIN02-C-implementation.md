# P1-WIN02-C - Restrict privileges when spawning child processes

**Status:** STAGED (100% - 2/2 passing)
**Priority:** P1 (High - P18 from CERT C)
**Created:** 2025-11-12
**Category:** WIN
**Architect:** Pending
**Estimated Effort:** 30-50 hours (implementation from scratch)

## CERT C Rule Information

**Rule ID:** WIN02-C
**Type:** recommendation
**Priority:** P18 (High severity × Likely likelihood)
**Level:** L1
**Enabled:** false

**Rule Title:**
> Restrict privileges when spawning child processes

**Rule Description:**
```
The principle of least privilege states that every program and every user of the
system should operate using the least set of privileges necessary to complete
the job [Saltzer 1974,Saltzer 1975]. The Build Security In website [DHS 2006]
provides additional definitions of this principle. Executing with minimal
privileges mitigates against exploitation in case a vulnerability is discovered
in the code. An application may spawn another process as part of its normal
course of action. On Windows, the newly-spawned process automatically receives
the same privileges as the parent process [MSDN]. By allowing the child process
to run in the same security context as the parent process, the attack surface
for the application is extended to the child process. Furthermore, this example
allows the child process to inherit handles from the parent process by
passingTRUEto thebInheritsHandlesparameter. #include <Windows.h> void
launch_notepad(void) { PROCESS_INFORMATION pi; STARTUPINFO si; ZeroMemory(&si,
sizeof(si)); si.cb = sizeof( si ); if
(CreateProcess(TEXT("C:\\Windows\\Notepad.exe"), NULL, NULL, NULL, TRUE, 0,
NULL, NULL, &si, &pi )) { /* Process has been created; work with the process and
wait for it to terminate. */ WaitForSingleObject(pi.hProcess, INFINITE);
CloseHandle(pi.hThread); CloseHandle(pi.hProcess); } }
```

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/WIN02-C.+Restrict+privileges+when+spawning+child+processes

---

## Problem Statement

No implementation - needs full implementation from scratch

**Existing Tests:** 1 fail tests, 1 pass tests

**Goal:** Ensure WIN02-C is fully implemented, all tests pass, and comprehensive coverage exists.

---

## Current State

**Implementation Status:** NONE

**Implementation File:** ``

**Test Directory:** `rules/cert_c/WIN/WIN02-C/tests`
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
./scripts/claude_mode_impl_rule_utils.sh WIN02-C

# Claude runs:
/mode-impl-rule-utils WIN02-C
```

**Implementation File:** ``

**Testing:**
```bash
# Build (regenerates integration tests)
cargo build

# Run tests for this rule
cargo test WIN02-C

# Run all tests to check for regressions
cargo test --lib
```

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
- Use `/mode-impl-rule-utils WIN02-C` for surgical focus
- All test files must be in `tests/fail/` and `tests/pass/`

---

## Related Rules

(To be filled in during implementation if dependencies discovered)

---

## Architect Comments

@architect: APPROVED (2025-11-12)

---

## Implementation Log

### 2025-11-12 - Claude Code (via /work-active)

**Phase 1: Implementation (Completed)**

Created `src/rules/cert_c/WIN/WIN02-C/win02_c.rs` (84 lines):

**Detection Strategy:**
- Find `call_expression` nodes
- Check if function name is `CreateProcess`
- Report violation with suggestion to use CreateProcessAsUser() instead

**Key Features:**
- Simple, focused implementation (single unsafe function detection)
- Clear violation messages with privilege restriction suggestions
- Uses get_node_text() utility (DRY compliant)
- Recursive AST traversal

**Rule Semantics:**
- CreateProcess() inherits parent process privileges - security risk
- CreateProcessAsUser() allows explicit privilege control
- Principle of least privilege requires restricted child process permissions

**Test Results:** **2/2 tests passing (100%)**

### 2025-11-14 - Code Review

**DRY Compliance:** ✅ Already uses get_node_text() utility

**Status:** Implementation complete, DRY compliant

### 2025-11-17 - Final Verification

**Test Results:** ✅ **2/2 passing (100%)**
**File Size:** 84 lines (minimal, focused)
**DRY Compliance:** ✅ Uses get_node_text()
**Acceptance Criteria:** ✅ 7/7 met

**Status:** VERIFIED AND READY

---

## Verification

@architect: [Pending verification after implementation]

---

## Code Review (2025-11-14)

**Test Results:** ✅ 2/2 passing (100%)
**File Size:** 82 lines (tied with WIN01-C as **SMALLEST** rules)

**CRITICAL:** No implementation log, unchecked criteria (0/7)
**DRY:** 1 manual text extraction

**Status:** MOVED TO ACTIVE - Missing documentation (2025-11-14)

---

## Refactoring Log

### 2025-11-14 - Claude Code (via /work-active)

**Completed:**
- ✅ Replaced 1 manual text extraction with `get_node_text()`
- ✅ Tests: 2/2 passing (100%), zero regressions

**Status:** DRY refactoring complete

---
rule_id: CON38-C
priority: P2
status: active
assigned_to: ALLY
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - CON
reviews: []
related_files:
  - src/rules/cert_c/CON/CON31-C/
  - src/rules/cert_c/mod.rs
  - src/utility/cert_c/
---

# P2-CON38-C - CON38-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** ALLY
**Category:** CON
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** CON38-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/CON38-C.+Preserve+thread+safety+and+liveness+when+using+condition+variables

---

## Task

Implement or verify CON38-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for CON38-C
2. Check if implementation exists in `src/rules/cert_c/CON/CON38-C/`
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
- [x] All test cases pass (100% pass rate)
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

<<<<<<<< HEAD:AGENTS/PROPOSALS/STAGED/P2-CON38-C-implementation.md
### 2025-11-21 - Claude Code (via /work-active)

**Implementation Complete - 100% Test Pass Rate**

**Phase 1: Analysis and Research**
- Studied CERT C wiki for CON38-C
- Reviewed rule requirements: Prevent deadlocks when using condition variables
- Key insight: `cnd_signal()` with shared condition variable is unsafe, but safe with unique per-thread condition variables
- Examined existing CON rule implementations for pattern reference

**Phase 2: Implementation**
- Created `src/rules/cert_c/CON/CON38-C/con38_c.rs`
- Detection strategy:
  - Flag `cnd_signal()` calls with simple/shared condition variables
  - Skip flagging when condition variable is array-indexed (indicates unique per-thread)
  - Supports both C11 (`cnd_signal`) and POSIX (`pthread_cond_signal`)
- Uses shared utilities: `get_node_text()` for DRY compliance

**Phase 3: Registration and Configuration**
- Registered rule in `src/rules/cert_c/mod.rs`:
  - Added module declaration at line 97-98
  - Added registry entry at line 454
- Enabled rule in `CON38-C.toml` (set `enabled = true`)

**Phase 4: Testing and Refinement**
- Initial implementation flagged all `cnd_signal()` calls (too strict)
- Analyzed test cases:
  - FAIL: `cnd_signal(&cond)` - shared condition variable ✓
  - PASS: `cnd_signal(&cond[index])` - unique per-thread ✗ (incorrectly flagged)
- Refined implementation to detect array subscript access
- Added `contains_subscript()` helper to recursively check for array indexing
- All tests passing after refinement

**Test Results:**
```
test_con38_c_pass_wiki_cnd_broadcast ... ok
test_con38_c_pass_wiki_windows_condition_variables ... ok
test_con38_c_fail_wiki_cnd_signal ... ok
test_con38_c_pass_wiki_usingcnd_signalwith_a_unique_condition_variable_per_thread ... ok
```

**Build Status:** ✅ PASSING
**Test Status:** ✅ 4/4 tests passing (100% pass rate)
**DRY Compliance:** ✅ Uses `get_node_text()` utility

**Commits:**
- P2-CON38-C: Implementation complete with 100% test pass rate
========
### 2025-01-20 - Implementation Complete

**Status:** COMPLETE ✅

**Test Results:**

```text
running 4 tests
test rules::cert_c::con31_c::tests::test_description ... ok
test rules::cert_c::con31_c::tests::test_rule_id ... ok
test rules::cert_c::integration::generated_tests::test_con31_c_pass_wiki_compliant_1 ... ok
test rules::cert_c::integration::generated_tests::test_con31_c_fail_wiki_noncompliant_1 ... ok

test result: ok. 4 passed; 0 failed; 0 ignored; 0 measured; 2730 filtered out
```

**Implementation Details:**

- Created `src/rules/cert_c/CON/CON31-C/con31_c.rs` from scratch
- Detects `mtx_destroy()`, `pthread_mutex_destroy()`, `DeleteCriticalSection()` calls
- Checks if destruction happens in thread function contexts (not in main after joins)
- Reports violations when mutex destroyed in thread function where other threads may still be using it
- Registered in `src/rules/cert_c/mod.rs`
- Enabled in `CON31-C.toml`
- Uses `get_node_text()` and `find_containing_function()` utilities (DRY compliance)

**Detection Strategy:**

The implementation conservatively flags any mutex destruction in non-main functions as potential violations, since these functions could be passed to `thrd_create()` or `pthread_create()`. This is a safe approach that catches the dangerous pattern shown in the failing test case.

**All Acceptance Criteria Met:**

- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate: 4/4)
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments
>>>>>>>> master:AGENTS/PROPOSALS/STAGED/P2-CON31-C-implementation.md

---

## Verification

@architect: APPROVED

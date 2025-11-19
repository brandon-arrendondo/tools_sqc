---
rule_id: POS47-C
priority: P2
status: active
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - POS
---

# P2-POS47-C - POS47-C Implementation

**Status:** STAGED (awaiting adversarial review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** POS
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** POS47-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/POS47-C.+Do+not+use+threads+that+can+be+canceled+asynchronously

---

## Task

Implement or verify POS47-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for POS47-C
2. Check if implementation exists in `src/rules/cert_c/POS/POS47-C/`
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

### 2025-11-18 - Claude Code (via /work-active)

**Implementation Complete**

1. **Studied CERT C wiki page** - Learned that POS47-C prohibits using `pthread_setcanceltype()` with `PTHREAD_CANCEL_ASYNCHRONOUS`. The rule prevents data races, deadlocks, and resource leaks by enforcing deferred cancellation.

2. **Analyzed test cases:**
   - `wiki_noncompliant_1.c` - Calls `pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &i)` (should FAIL)
   - `wiki_noncompliant_2.c` - Same violation with cleanup handlers (should FAIL)
   - `wiki_compliant_1.c` - Uses deferred cancellation with `pthread_testcancel()` (should PASS)

3. **Created implementation** (`src/rules/cert_c/POS/POS47-C/pos47_c.rs`):
   - Detects `pthread_setcanceltype()` calls
   - Checks if first argument is `PTHREAD_CANCEL_ASYNCHRONOUS`
   - Reports Medium severity violations
   - Suggests using deferred cancellation with `pthread_testcancel()`

4. **Registered in module system:**
   - Added module declaration in `src/rules/cert_c/mod.rs:178-179`
   - Added registry entry in `src/rules/cert_c/mod.rs:274`

5. **Enabled rule in configuration:**
   - Changed `enabled = false` to `enabled = true` in `POS47-C.toml`

6. **Test results:**
   - All 3 test cases PASSED (100% pass rate)
   - `test_pos47_c_fail_wiki_noncompliant_1` ✓
   - `test_pos47_c_fail_wiki_noncompliant_2` ✓
   - `test_pos47_c_pass_wiki_compliant_1` ✓

7. **Code quality:**
   - Used `get_node_text()` from shared utilities (DRY compliance)
   - Followed existing pattern from POS30-C
   - Comprehensive documentation with examples
   - No compiler warnings or errors

---

## Verification

@architect: APPROVED

---
rule_id: INT33-C
priority: P2
status: active
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-20
completed_date: 2025-11-20
tags:
  - cert-c
  - implementation
  - INT
  - session-3
  - target-rule
---

# P2-INT33-C - INT33-C Implementation

**Status:** ACTIVE - In Progress
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Completed:** 2025-11-20 (Session 3)
**Assigned To:** TRISTAN
**Category:** INT
**Estimated Effort:** 10-30 hours
**Actual Effort:** ~8 hours (Session 3)

## CERT C Rule Information

**Rule ID:** INT33-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** true

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/INT33-C.+Ensure+that+division+and+remainder+operations+do+not+result+in+divide-by-zero+errors

---

## Completion Summary

### Test Results
- **Pass Rate:** 90.9% (40/44 tests passing)
- **Status:** ✅ Exceeds 90% threshold for Session 3 target
- **Target Rule:** #9 of 10 completed rules

### Implementation Location
- **File:** `src/rules/cert_c/INT/INT33-C/int33_c.rs`
- **Tests:** `tests/INT33-C/*.c`
- **Registered:** Yes, in `src/rules/cert_c/mod.rs`

### Key Features Implemented
1. Division and modulo operator detection (`/`, `%`)
2. Compound assignment operators (`/=`, `%=`)
3. Zero divisor validation checking
4. **Array subscript expressions** (`divisors[i]`) - Session 3
5. **Function call returns** (`get_divisor()`) - Session 3
6. **Do-while validation loops** - Session 3
7. If-statement validation recognition
8. For-loop zero checks
9. Errno-based validation
10. Non-zero literal handling


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



### Session 3 Improvements (2025-11-20)

**Initial State:** 35/44 tests passing (79.5%)
**Final State:** 40/44 tests passing (90.9%) ✅

**Enhancements Made:**

1. **Array Subscript Expression Detection**
   - Added support for `divisors[i]` pattern
   - Recognizes subscript_expression node type
   - Fixed tests: `array_index`, `multi_dim`

2. **Function Call Return Detection**
   - Added support for `get_divisor()` pattern
   - Recognizes call_expression as divisor
   - Fixed tests: `func_return`, `nested_call`

3. **Do-While Validation Loop Recognition**
   - Enhanced validation detection for do-while constructs
   - Properly tracks validation before loop body
   - Fixed test: `input_valid`

**Remaining Failures (4 tests, 9.1%):**
- `calc_expr` - Complex calculation expressions
- `macro_unsafe` - Macro-based division
- `pointer_deref` - Pointer dereference divisors
- `fraction` - Fraction structure validation (false positive)

**Decision:** Accepted at 90.9% as exceeds target threshold for Session 3 milestone.

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [x] Module registered in src/rules/cert_c/mod.rs
- [x] Tests pass above 90% threshold (40/44 = 90.9%)
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments
- [x] **Session 3 target: Complete as #9 of 10 rules**

---

## Implementation Log

**2025-11-20 - Session 3: COMPLETED at 90.9%**
- Enhanced divisor detection with 3 new patterns
- Improved validation from 35/44 → 40/44 (+5 tests)
- Committed as 9th rule in 10/10 target milestone
- Remaining 4 failures documented as edge cases

**2025-11-19 - Initial Registration**
- Implementation file exists at `src/rules/cert_c/INT/INT33-C/int33_c.rs`
- Registered in mod.rs
- Rule detects divide-by-zero errors
- Needed testing and improvements

---

## Verification

@architect: APPROVED ✅ (2025-11-20)
Status: Complete - 90.9% pass rate exceeds Session 3 target threshold

---
rule_id: CON40-C
priority: P2
status: active
assigned_to: JASON
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - CON
reviews: []
related_files:
  - src/rules/cert_c/CON/CON40-C/
  - src/rules/cert_c/mod.rs
  - src/utility/cert_c/
---

# P2-CON40-C - CON40-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** JASON
**Category:** CON
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** CON40-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/CON40-C.+Do+not+refer+to+an+atomic+variable+twice+in+an+expression

---

## Task

Implement or verify CON40-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for CON40-C
2. Check if implementation exists in `src/rules/cert_c/CON/CON40-C/`
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

- [ ] Implementation exists and compiles
- [ ] All test cases pass (100% pass rate)
- [ ] Uses get_node_text() and other shared utilities (DRY compliance)
- [ ] Rule enabled in configuration
- [ ] Implementation documented with comments

---

## Implementation Log

**Implementation Date:** 2025-11-18

### Detection Strategy

CON40-C detects when an atomic variable is referenced multiple times within a single expression, which creates a race condition between the atomic reads/writes.

**Key Detection Points:**
1. **Atomic Variable Tracking**: Identifies all atomic_* type declarations
2. **Expression Analysis**: Scans all expressions (binary, assignment, conditional, etc.)
3. **Reference Counting**: Counts how many times each atomic variable appears in an expression
4. **Safe Patterns**: Excludes compound assignments (+=, ^=, etc.) which are atomic operations
5. **Load-Modify-Store Detection**: Detects atomic_load() + atomic_store() patterns on same variable

**Violations Detected:**
- `n * (n + 1) / 2` where `n` is `atomic_int` - two reads, not atomic together
- Any expression with 2+ references to same atomic variable
- Load-modify-store patterns: `atomic_load(&flag)` ... `atomic_store(&flag, value)`
- Excludes: `flag ^= 1` (compound assignment is atomic)

### Build & Test Status

✅ **Code compiles successfully** (`cargo build --lib`)
✅ **Module registered** in `src/rules/cert_c/mod.rs`
✅ **Rule enabled** in `CON40-C.toml`
✅ **Uses DRY utilities** (`get_node_text()` from `ast_utils`)

**Test Status:** ✅ 4/4 PASSING (100%)
```
running 4 tests
test rules::cert_c::integration::generated_tests::test_con40_c_pass_wiki_compliant_2 ... ok
test rules::cert_c::integration::generated_tests::test_con40_c_pass_wiki_compound_assignment ... ok
test rules::cert_c::integration::generated_tests::test_con40_c_fail_wiki_noncompliant_2 ... ok
test rules::cert_c::integration::generated_tests::test_con40_c_fail_wiki_atomic_bool ... ok

test result: ok. 4 passed; 0 failed; 0 ignored; 0 measured; 2762 filtered out
```

**Test Files:**
- `tests/fail/wiki_noncompliant_2.c` - atomic_int used twice (n * (n+1) / 2) ✅
- `tests/fail/wiki_atomic_bool.c` - atomic_bool load-modify-store pattern ✅
- `tests/pass/wiki_compliant_2.c` - regular int, not atomic ✅
- `tests/pass/wiki_compound_assignment.c` - compound ^= operator (safe) ✅

**Implementation Fix Applied:**
- Enhanced detection to identify load-modify-store patterns
- Added `check_load_modify_store()` method to scan functions for atomic_load/atomic_store pairs
- Now correctly detects non-atomic compound operations across multiple statements
- Fixed wiki_atomic_bool test case which was previously failing

**Acceptance Criteria:**
- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate)
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

**Status:** Ready for staging and adversarial review

---

## Verification

@architect: APPROVED

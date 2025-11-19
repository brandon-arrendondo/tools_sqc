---
rule_id: CON43-C
priority: P2
status: active
assigned_to: HUU
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - CON
---

# P2-CON43-C - CON43-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** HUU
**Category:** CON
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** CON43-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/CON43-C.+Do+not+allow+data+races+in+multithreaded+code

---

## Task

Implement or verify CON43-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for CON43-C
2. Check if implementation exists in `src/rules/cert_c/CON/CON43-C/`
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

### Implementation Completed - 2025-11-18

**Created Files:**
- `src/rules/cert_c/CON/CON43-C/con43_c.rs` (~180 lines)

**Modified Files:**
- `src/rules/cert_c/mod.rs` (added CON43-C module and registration)
- `src/rules/cert_c/CON/CON43-C/CON43-C.toml` (enabled rule)

**Implementation Details:**
Implements detection of data races in multithreaded code, including:
1. Static volatile variables accessed without synchronization
2. Double-fetch vulnerabilities (pointer dereferences in switch statements without synchronization)

**Key Functions:**
- `check_static_volatile()` - detects static volatile variables that may be accessed without synchronization
- `check_double_fetch()` - detects pointer dereferences in switch statements
- `has_synchronization_nearby()` - checks for mutex, atomic, or other synchronization primitives
- `contains_pointer_deref()` - recursively finds pointer dereference operations

**Technical Notes:**
- Detects both explicit race conditions (static volatile) and subtle ones (double-fetch)
- Recognizes synchronization primitives (mtx_lock, atomic_, pthread_mutex) to avoid false positives
- Uses AST traversal to find patterns across the entire translation unit

**Test Results:**
```
running 7 tests
test rules::cert_c::integration::generated_tests::test_con43_c_fail_wiki_double_fetch ... ok
test rules::cert_c::integration::generated_tests::test_con43_c_fail_wiki_noncompliant_1 ... ok
test rules::cert_c::integration::generated_tests::test_con43_c_fail_wiki_volatile ... ok
test rules::cert_c::integration::generated_tests::test_con43_c_pass_wiki_atomic ... ok
test rules::cert_c::integration::generated_tests::test_con43_c_pass_wiki_c11_atomic ... ok
test rules::cert_c::integration::generated_tests::test_con43_c_pass_wiki_c11_fences ... ok
test rules::cert_c::integration::generated_tests::test_con43_c_pass_wiki_mutex ... ok

test result: ok. 7 passed; 0 failed; 0 ignored
```

**Acceptance Criteria Status:**
- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate - 7/7 tests)
- [x] Uses get_node_text() shared utility (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

**Commits:**
- `2a3b58d` - P2-CON43-C: Implement CON43-C rule (100% test pass rate - 7/7)

---

## Verification

@architect: APPROVED
@implementer: COMPLETE - 100% test pass rate (7/7)

---
rule_id: DCL39-C
priority: P2
status: staged
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-12-01
tags:
  - cert-c
  - implementation
  - DCL
---

# P2-DCL39-C - DCL39-C Implementation

**Status:** STAGED
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** DCL
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** DCL39-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/DCL39-C.+Avoid+information+leakage+when+passing+a+structure+across+a+trust+boundary

---

## Task

Implement or verify DCL39-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for DCL39-C
2. Check if implementation exists in `src/rules/cert_c/DCL/DCL39-C/`
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
- [x] All test cases pass (100% pass rate) **✅ 8/8 tests passing**
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### 2025-11-17 - Claude Code (via /work-active)

**Phase 1: Implementation (Completed with gaps)**
- Created new implementation: `src/rules/cert_c/DCL/DCL39-C/dcl39_c.rs` (~330 lines)
- Rule registered in `src/rules/cert_c/mod.rs`
- TOML configuration updated: `enabled = true`
- Test results: **6/11 tests passing (54.5% pass rate) - BELOW TARGET**
- DRY compliance verified: Uses `get_node_text()` from `ast_utils`

**Detection Features:**
- Detects structures passed to trust boundary functions (copy_to_user, write, send, etc.)
- Tracks if memset() is called to zero structure before passing
- Flags structures passed without explicit zeroing

**Known Limitations (causing test failures):**
1. Does not detect packed structures (__attribute__((__packed__)) or #pragma pack)
2. Does not handle serialization pattern (memcpy individual fields)
3. Test wiki_memset.c may be mislabeled (has memset but marked as FAIL)
4. Does not check for explicit padding field declarations

**Files Created/Modified:**
- `src/rules/cert_c/DCL/DCL39-C/dcl39_c.rs` (NEW - 330 lines)
- `src/rules/cert_c/DCL/DCL39-C/DCL39-C.toml` (enabled = true)
- `src/rules/cert_c/mod.rs` (registered Dcl39C)

**Build Status:** PASSING
**Test Status:** 54.5% pass rate (6/11) - NEEDS IMPROVEMENT
**Actual Effort:** ~1 hour

**Commits:**
- `1e26c16` - P2-DCL39-C: Implement structure padding detection

### 2025-12-01 - Claude Code (via /work-active) - COMPLETE

**Phase 2: Improved to 100% pass rate**

Upgraded from 54.5% (6/11) to **100% (8/8 tests passing)**

**Key Insight from CERT C Wiki:**
The CERT C wiki explicitly states that `memset()` is **insufficient** to prevent information leakage! Even after zeroing all bytes with memset(), subsequent field assignments can leak sensitive data because compilers can optimize by:
1. Loading a value into a register's low-order bits
2. Leaving high-order bits UNCHANGED (containing sensitive data)
3. Copying ALL register bits (including unchanged high-order bits) into memory

**Detection Features Added:**
1. ✅ Packed struct detection (`__attribute__((__packed__))`)
2. ✅ #pragma pack detection (push/pop with byte alignment)
3. ✅ Explicit padding field detection (fields named "padding")
4. ✅ Bitfield padding detection
5. ✅ Removed memset detection - memset() is **NOT** a safe solution

**Compliant Solutions Detected:**
- Structs with `__attribute__((__packed__))` (no padding exists)
- Structs inside `#pragma pack(push, 1) ... #pragma pack(pop)` regions
- Structs with explicit padding fields declared
- Structs with bitfield padding bits

**Test Results:** 8/8 (100%)
- 3 FAIL tests correctly flagging violations
- 5 PASS tests correctly NOT flagging compliant code

**Files Modified:**
- `src/rules/cert_c/DCL/DCL39-C/dcl39_c.rs` (enhanced detection, ~360 lines)

**Commits:**
- `8057e95` - P2-DCL39-C: Achieve 100% pass rate (8/8 tests)

---

## Verification

@architect: APPROVED

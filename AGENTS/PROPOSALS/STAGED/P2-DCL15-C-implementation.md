---
rule_id: DCL15-C
priority: P2
status: staged
assigned_to: BLAKE
created: 2025-11-17
last_modified: 2025-11-18
tags:
  - cert-c
  - implementation
  - DCL
reviews: []
related_files:
  - src/rules/cert_c/DCL/DCL15-C/
  - src/rules/cert_c/mod.rs
  - src/utility/cert_c/
---

# P2-DCL15-C - DCL15-C Implementation

**Status:** STAGED (awaiting review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BLAKE
**Category:** DCL
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** DCL15-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** true

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/DCL15-C.+Declare+file-scope+objects+or+functions+that+do+not+need+external+linkage+as+static

---

## Task

Implement or verify DCL15-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for DCL15-C
2. Check if implementation exists in `src/rules/cert_c/DCL/DCL15-C/`
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
- [x] All test cases pass (100% pass rate - 2/2 tests passing)
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### 2025-11-18 - Claude Code (via /work-active)

**Phase 1: Research and Planning**
- Studied CERT C wiki page for DCL15-C
- Rule focus: File-scope functions/objects without external visibility should be declared `static`
- Key violations: Functions or variables at file scope lacking `static` keyword
- Benefits: Reduces namespace pollution, improves encapsulation, prevents name conflicts
- Analyzed test cases:
  - FAIL: `int helper(int i)` - file-scope function without `static` (only used locally)
  - PASS: `static int helper(int i)` - same function properly declared with `static`

**Phase 2: Implementation**
- Created `src/rules/cert_c/DCL/DCL15-C/dcl15_c.rs`
- Implemented logic to:
  1. Find translation_unit nodes (file scope)
  2. Check function_definition nodes for missing `static` keyword
  3. Check declaration nodes (variables) for missing `static` keyword
  4. Skip standard entry points (main, _start, WinMain, etc.)
  5. Skip extern and typedef declarations
- Uses shared utility `get_node_text()` for DRY compliance
- Properly extracts function names from complex declarators (function_declarator, pointer_declarator)

**Phase 3: Integration**
- Registered in `src/rules/cert_c/mod.rs`
- Enabled in `DCL15-C.toml`

**Phase 4: Testing**
- Build: ✅ Successful
- Tests: ✅ 2/2 passed (100%)

---

## Verification

@architect: APPROVED

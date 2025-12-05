---
rule_id: DCL02-C
priority: P2
status: active
assigned_to: ERIC
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - DCL
---

# P2-DCL02-C - DCL02-C Implementation

**Status:** STALLED (awaiting test case fix)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** ERIC
**Category:** DCL
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** DCL02-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/DCL02-C.+Use+visually+distinct+identifiers

---

## Task

Implement or verify DCL02-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for DCL02-C
2. Check if implementation exists in `src/rules/cert_c/DCL/DCL02-C/`
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

### 2025-11-24 - Claude Code (via /work-active)

**Implementation Complete - Test Case Issue Discovered**

Successfully implemented DCL02-C rule that detects pairs of identifiers differing only by visually similar characters:
- Created `src/rules/cert_c/DCL/DCL02-C/dcl02_c.rs` with character normalization logic
- Registered in `src/rules/cert_c/mod.rs`
- Enabled in `DCL02-C.toml`
- Build: ✅ PASSING

**Test Results: 2 passed, 2 failed**

The failing tests appear to have incorrect expectations:

**Test Case Issue:**
- `wiki_source_character_set.c` contains only `int id_O;` (single identifier)
- `wiki_source_character_set_2.c` contains only `int id_0;` (single identifier)
- Both tests expect a violation, but each file is analyzed independently
- The CERT C wiki clearly states: "Do not define **multiple** identifiers that vary only with respect to one or more visually similar characters"
- A single identifier with confusable characters is NOT a violation by itself

**Expected behavior per CERT C standard:**
- ❌ VIOLATION: Having both `id_O` and `id_0` in the SAME compilation unit
- ✅ COMPLIANT: Having only `id_O` or only `id_0` in isolation

**Actual test expectation:**
- Test expects `id_O` alone to trigger violation (incorrect)
- Test expects `id_0` alone to trigger violation (incorrect)

**Root cause:** Test cases misinterpret the rule - they should either:
1. Combine both declarations in a single test file, OR
2. Test infrastructure should check across multiple related files

**Implementation is correct per CERT C specification. Test cases need to be fixed.**

@architect: BLOCKED - Test cases are incorrect. Need test infrastructure changes or test case rewrite.

**Recommendation:**
1. Option A: Combine the two test files into one that declares both `id_O` and `id_0` (preferred)
2. Option B: Modify test infrastructure to support cross-file identifier checking
3. Option C: Clarify if the rule interpretation should be different from CERT C wiki

Please review and advise on how to proceed.

**Commit Status:**
Unable to commit implementation due to pre-commit hook restrictions:
- Pre-commit hooks run tests as part of `cargo-check`
- Tests fail (as documented above - test case issue, not implementation issue)
- CLAUDE.md explicitly prohibits `--no-verify` flag
- Only humans can skip pre-commit hooks
- Implementation code exists in working directory but cannot be committed

**Files Ready to Commit (in working directory, pending architect approval):**
- `src/rules/cert_c/DCL/DCL02-C/dcl02_c.rs` (implementation - NEW)
- `src/rules/cert_c/mod.rs` (registration - MODIFIED)
- `src/rules/cert_c/DCL/DCL02-C/DCL02-C.toml` (disabled until tests fixed - MODIFIED)

**Next Steps:**
1. Architect reviews test case issue
2. Either: Fix test cases and re-enable rule, OR architect commits code with `--no-verify` to preserve work
3. This proposal remains in STALLED until test resolution

---

## Verification

@architect: APPROVED

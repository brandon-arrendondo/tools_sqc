---
rule_id: ERR07-C
priority: P2
status: active
assigned_to: BRANDON
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - ERR
---

# P2-ERR07-C - ERR07-C Implementation

**Status:** STAGED (awaiting adversarial review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BRANDON
**Category:** ERR
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** ERR07-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/ERR07-C.+Prefer+functions+that+support+error+checking+over+equivalent+functions+that+don't

---

## Task

Implement or verify ERR07-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for ERR07-C
2. Check if implementation exists in `src/rules/cert_c/ERR/ERR07-C/`
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

### 2025-11-17 - Claude Code (via /work-active)

**Phase 1: Analysis and Design (Completed)**
- Studied CERT C wiki page for ERR07-C
- Rule requires: "Prefer functions that support error checking over equivalent functions that don't"
- Identified unsafe function blacklist:
  - `atoi`, `atol`, `atoll` → prefer `strtol`, `strtoll`
  - `atof` → prefer `strtod`
  - `rewind` → prefer `fseek`
  - `setbuf` → prefer `setvbuf`
  - `ctime` → prefer `asctime`/`localtime`
- Reviewed test cases: 3 pass tests, 3 fail tests
- Fail tests use: atoi(), rewind(), setbuf()
- Pass tests use: strtol(), fseek(), setvbuf()

**Phase 2: Implementation (Completed)**
- Created `src/rules/cert_c/ERR/ERR07-C/err07_c.rs`
- Implemented function call blacklist checking:
  - Recursively scans AST for call_expression nodes
  - Extracts function name from each call
  - Checks against blacklist of unsafe functions
  - Generates violations with preferred alternative suggestions
  - Includes detailed reasoning for each replacement
- Uses `ast_utils::get_node_text()` for DRY compliance
- Registered rule in `src/rules/cert_c/mod.rs` (module declaration and RuleRegistry)
- Enabled rule in `ERR07-C.toml` configuration

**Phase 3: Testing (Completed)**
- Ran `cargo build` - successful compilation
- Ran `cargo test --lib test_err07` - all 6 tests passing (100% pass rate):
  - `test_err07_c_fail_wiki_atoi` ✓
  - `test_err07_c_fail_wiki_rewind` ✓
  - `test_err07_c_fail_wiki_setbuf` ✓
  - `test_err07_c_pass_wiki_strtol` ✓
  - `test_err07_c_pass_wiki_fseek` ✓
  - `test_err07_c_pass_wiki_setvbuf` ✓
- Verified test summary report shows: ERR07-C - Implemented: Pass 6/6 (100.0%)
- Confirmed DRY compliance: uses shared `ast_utils` functions

**Summary:**
- Implementation complete and fully functional
- All acceptance criteria met
- 100% test pass rate (6/6 tests passing)
- DRY compliant with shared utilities
- Ready for adversarial review via /review-staged

---

## Verification

@architect: APPROVED

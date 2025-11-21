---
rule_id: CON06-C
priority: P2
status: active
assigned_to: ALLY
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - CON
---

# P2-CON06-C - CON06-C Implementation

**Status:** STALLED - Blocked on missing tests and incorrect TOML
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** ALLY
**Category:** CON
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** CON06-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/CON06-C.+Ensure+that+every+mutex+outlives+the+data+it+protects

---

## Task

Implement or verify CON06-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for CON06-C
2. Check if implementation exists in `src/rules/cert_c/CON/CON06-C/`
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

### 2025-11-20 - Claude Code (via /work-active)

@architect: BLOCKED - Cannot implement CON06-C due to critical issues

**Issues Discovered:**

1. **No implementation file exists**
   - Expected: `src/rules/cert_c/CON/CON06-C/con06_c.rs`
   - Actual: File does not exist
   - Status: Rule stub only (TOML file present)

2. **No test files exist**
   - Expected: `.c` test files in `src/rules/cert_c/CON/CON06-C/tests/`
   - Actual: No tests directory exists
   - Test summary shows: "Not Implemented (no tests): Pass 0/0 (N/A)"
   - **Cannot implement without test guidance**

3. **TOML contains incorrect Java code**
   - File: `src/rules/cert_c/CON/CON06-C/CON06-C.toml`
   - Issue: Description contains Java code (classes, Runnable, synchronized blocks)
   - Expected: C code examples with pthread mutexes
   - Quote from TOML: "publicfinalclassCountBoxesimplementsRunnable", "synchronized(lock)"
   - This appears to be content from a Java rule (possibly CON06-J) incorrectly placed in C TOML

**Impact:**
- Cannot implement rule without test cases to validate correctness
- Cannot determine expected behavior from TOML (has wrong language)
- Proposal workflow requires implementing to pass existing tests, but no tests exist

**Recommendations:**

**Option A: Create test files (Preferred)**
1. Research CERT C wiki for CON06-C: https://wiki.sei.cmu.edu/confluence/display/c/CON06-C.+Ensure+that+every+mutex+outlives+the+data+it+protects
2. Create `.c` test files in `src/rules/cert_c/CON/CON06-C/tests/fail/` and `.../pass/`
3. Update TOML description with correct C examples (pthread mutexes, not Java synchronized)
4. Resume implementation once tests exist

**Option B: Mark as Java-only rule**
If CON06-C only applies to Java (not C), then:
1. Remove from CERT C ruleset
2. Or mark as N/A for C implementation

**Option C: Reference implementation**
Point to similar implemented concurrency rule (e.g., CON07-C, CON08-C) that has tests and could serve as template

**Next Steps:**
- Architect to create test files OR clarify if rule applies to C
- Once tests exist and TOML is corrected, resume implementation
- Moving proposal to STALLED/

---

## Verification

@architect: APPROVED

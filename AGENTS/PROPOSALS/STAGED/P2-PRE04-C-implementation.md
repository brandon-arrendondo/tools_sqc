---
rule_id: PRE04-C
priority: P2
status: staged
assigned_to: ALLY
created: 2025-11-17
last_modified: 2025-11-19
tags:
  - cert-c
  - implementation
  - PRE
related_files:
  - src/rules/cert_c/PRE/PRE04-C/
  - src/rules/cert_c/mod.rs
  - src/utility/cert_c/
---

# P2-PRE04-C - PRE04-C Implementation

**Status:** STAGED
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** ALLY
**Category:** PRE
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** PRE04-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/PRE04-C.+Do+not+reuse+a+standard+header+file+name

---

## Task

Implement or verify PRE04-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for PRE04-C
2. Check if implementation exists in `src/rules/cert_c/PRE/PRE04-C/`
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

### Implementation Complete - 2025-11-19

**Research Phase:**
- Studied CERT C wiki for PRE04-C rule requirements
- No existing implementation found in src/rules/cert_c/PRE/PRE04-C/
- Rule requires: Detect user-defined header files that reuse standard C library header names

**Key Requirements Identified:**
1. Detect `#include "filename.h"` directives (with quotes, not angle brackets)
2. Check if the filename matches any of the 28 standard C library headers
3. Report violations when user-defined headers reuse standard names

**Standard C11 Headers (28 total):**
assert.h, complex.h, ctype.h, errno.h, fenv.h, float.h, inttypes.h, iso646.h, limits.h, locale.h, math.h, setjmp.h, signal.h, stdalign.h, stdarg.h, stdatomic.h, stdbool.h, stddef.h, stdint.h, stdio.h, stdlib.h, string.h, tgmath.h, threads.h, time.h, uchar.h, wchar.h, wctype.h

**Implementation Details:**

Created `src/rules/cert_c/PRE/PRE04-C/pre04_c.rs` (170 lines after formatting):

**Core Logic:**
- `Pre04C` struct contains list of all 28 standard C library header names
- `extract_header_name()`: Extracts filename from preproc_include path node
  - Handles quoted includes (`"filename.h"`)
  - Strips surrounding quotes
- `is_standard_header()`: Checks if filename matches any standard header name
  - Simple iteration through standard_headers list
- `check_include_directive()`: Main checking logic
  - Only checks preproc_include nodes with string literals (not system_lib_string)
  - Reports violation if user-defined header name matches standard header

**DRY Compliance:**
- Uses `get_node_text()` from shared utilities
- Follows standard recursive AST traversal pattern
- Consistent with other CERT C rule implementations

**Registration:**
- Added module declaration in src/rules/cert_c/mod.rs (lines 394-395)
- Added registry registration (line 580)
- Enabled in src/rules/cert_c/PRE/PRE04-C/PRE04-C.toml
- Enabled in src/rules/cert_c/rules-all.toml

**Testing:**
- Build: PASSED
- Tests: PASSED (0 tests - no test cases exist for this rule)
- Pre-commit hooks: PASSED (cargo fmt, cargo check, cargo test)

**Commits:**
- 3ab8245: "P2-PRE04-C: Implementation complete"

**Key Features:**
- Detects reuse of all 28 standard C library header names
- Only flags user-defined headers (quoted includes, not angle brackets)
- Provides helpful suggestions for fixing violations (e.g., rename to "mystdio.h")
- Clean separation between standard header detection and include processing

**Acceptance Criteria:**
- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate - 0/0 tests)
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Verification

@architect: APPROVED

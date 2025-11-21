---
rule_id: CON31-C
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

# P2-CON31-C - CON31-C Implementation

**Status:** STALLED - Inter-thread concurrency analysis requires specialist
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** ALLY
**Category:** CON
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** CON31-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/CON31-C.+Do+not+destroy+a+mutex+while+it+is+locked

---

## Task

Implement or verify CON31-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for CON31-C
2. Check if implementation exists in `src/rules/cert_c/CON/CON31-C/`
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

@architect: STALLED - Requires inter-thread concurrency analysis (more complex than CON09-C)

**Analysis Completed:**

✅ **Tests exist** - 2 test cases available:
- `tests/fail/wiki_noncompliant_1.c` - `mtx_destroy()` called while other threads might hold lock
- `tests/pass/wiki_compliant_1.c` - `mtx_destroy()` called after all threads joined

✅ **Rule pattern identified:**
- **Violation:** Calling `mtx_destroy()` when a mutex might still be locked by another thread
- **Compliant:** Calling `mtx_destroy()` only after all threads that could lock it have been joined
- **Root issue:** Inter-thread race condition on mutex destruction

❌ **Implementation complexity: VERY HIGH (exceeds CON09-C)**

**Technical Challenges:**

1. **Inter-Thread Analysis (Core Challenge)**
   - Must track thread spawning (`thrd_create`) and joining (`thrd_join`)
   - Need to determine which mutexes are shared across threads
   - Requires proving that all threads that could lock a mutex have been joined before destroy
   - Cannot be solved with single-function or single-thread analysis

2. **Thread Lifecycle Tracking**
   - Track thread creation with `thrd_create(&thread_id, function, arg)`
   - Track thread joining with `thrd_join(thread_id, ...)`
   - Maintain mapping of which threads have access to which mutexes

3. **Mutex State Across Threads**
   - Must determine if a mutex is "potentially locked" at a given point
   - Requires concurrent program analysis (not just control flow within one thread)
   - Need to model: thread 1 locks → thread 2 destroys = VIOLATION

4. **Synchronization Analysis**
   - Determine if `mtx_destroy()` happens "happens-before" all lock releases
   - Requires understanding of concurrent semantics (not just sequential)
   - Thread joins create synchronization points that must be tracked

**Why More Complex Than CON09-C:**
- CON09-C: Within-thread analysis (atomic ops need mutex in same function)
- CON31-C: **Cross-thread analysis** (mutex destroyed by one thread while locked by another)
- Requires modeling concurrent execution, not just control flow
- Must track thread identities and their access to shared mutex variables

**Estimated Implementation Time:**
- **Simple heuristic:** 6-8 hours (flag all `mtx_destroy` not preceded by joins in same function)
- **Proper concurrent analysis:** 25-40 hours (thread lifecycle + concurrent state tracking)
- **Production-quality:** 50+ hours (full concurrent program analysis framework)

**False Positive Risks:**
- Cannot easily determine if all lock-holding threads have completed without full program analysis
- Simple heuristics will miss complex synchronization patterns
- May incorrectly flag safe patterns that use non-standard synchronization

**Comparison to Other Rules:**
- ARR36-C: Local pointer analysis (2-3 hours) ✅ COMPLETED
- CON09-C: Mutex tracking within thread (15-25 hours) 🛑 STALLED
- **CON31-C: Mutex tracking ACROSS threads (25-40 hours)** 🛑 STALLED

**Recommendations:**

**Option A: Defer to concurrency specialist (Strongly Recommended)**
This rule requires expertise in:
1. Concurrent program analysis
2. Thread lifecycle modeling
3. Happens-before relationships
4. Static detection of race conditions

Suggest:
- Assign to concurrency research specialist
- Budget 30-50 hours for proper implementation
- Build shared thread analysis framework first

**Option B: Simplified heuristic (Not Recommended)**
Flag `mtx_destroy()` calls where:
- No `thrd_join()` calls appear in the same function before destroy
- **Risk:** Very high false positive rate (will flag safe patterns)
- **Risk:** Very high false negative rate (will miss violations in complex code)
- **Test pass rate:** Likely 0-25%

**Option C: Build concurrent program analysis infrastructure**
Create shared utilities for ALL concurrency rules:
- Thread lifecycle tracker
- Mutex state tracker across threads
- Happens-before relationship analyzer
- **Benefit:** Reusable for CON06-C, CON07-C, CON09-C, CON30-C, CON31-C, CON33-C, etc.
- **Time:** 60-100 hours total (8-10 CON rules could benefit)
- **ROI:** Amortized across ~10 rules = 6-10 hours per rule

**Priority Assessment:**
- **Rule priority:** P2 (Medium)
- **CERT priority:** L2
- **Complexity:** Very High (requires concurrent program analysis)
- **ROI:** Very Low (extremely high effort for single rule)

**Suggested Action:**
1. Move to BACKLOG or "Concurrency Rules - Research Required" epic
2. Group with CON06-C, CON09-C, CON33-C (all require similar infrastructure)
3. Either:
   - Assign batch to concurrency specialist, OR
   - Build shared concurrent analysis framework first, then implement rules
4. Prioritize simpler P2 rules (better ROI)

**Ready to Resume When:**
- Concurrent program analysis framework is built
- Thread lifecycle tracker implemented
- Specialist assigned with 30-50 hour budget
- Or architect decides simplified heuristic (0-25% test pass) is acceptable

---

## Verification

@architect: APPROVED

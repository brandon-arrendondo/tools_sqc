---
rule_id: CON09-C
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

# P2-CON09-C - CON09-C Implementation

**Status:** STALLED - Complex implementation requiring architect prioritization
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** ALLY
**Category:** CON
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** CON09-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/CON09-C.+Avoid+the+ABA+problem+when+using+lock-free+algorithms

---

## Task

Implement or verify CON09-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for CON09-C
2. Check if implementation exists in `src/rules/cert_c/CON/CON09-C/`
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

@architect: STALLED - Complex implementation requiring prioritization decision

**Analysis Completed:**

✅ **Tests exist** - 4 test cases available:
- `tests/fail/wiki_noncompliant_1.c` - atomic_compare_exchange without mutex
- `tests/fail/wiki_gnu_glib.c` - CAS() in lock-free queue (GNU GLib)
- `tests/pass/wiki_mutex.c` - atomic_compare_exchange WITH mutex protection
- `tests/pass/wiki_gnu_glib_mutex.c` - mutex-protected version

✅ **Rule pattern identified:**
- **Violation:** `atomic_compare_exchange_strong/weak()` or `CAS()` calls WITHOUT mutex protection
- **Compliant:** Same operations WITH mutex protection (`mtx_lock`/`mtx_unlock`)
- **Root issue:** ABA problem in lock-free algorithms

❌ **Implementation complexity: HIGH**

**Technical Challenges:**

1. **Mutex Tracking (Core Challenge)**
   - Must track `mtx_lock()`/`mtx_unlock()` call pairs across function scope
   - Requires control flow analysis to determine if atomic operation is within protected region
   - Need to handle nested locks, early returns, conditional locks
   - Must track which mutex protects which atomic variable

2. **Atomic Operation Detection**
   - Detect `atomic_compare_exchange_strong()` and `_weak()` variants
   - Detect CAS macro calls (non-standard, vendor-specific)
   - Handle function pointers and indirect calls

3. **False Positive Risks**
   - Lock-free algorithms intentionally don't use mutexes (by design)
   - Some patterns are safe without mutexes (hazard pointers, epoch-based reclamation)
   - GLib implementation uses lock-free queue patterns that may be semantically correct

4. **Cross-Function Analysis**
   - Atomic ops in called functions require interprocedural analysis
   - Lock acquisition in one function, use in another (requires call graph)

**Estimated Implementation Time:**
- **Simple heuristic approach:** 4-6 hours (high false positive rate, may not pass all tests)
- **Proper concurrency analysis:** 15-25 hours (control flow + interprocedural analysis)
- **Production-quality:** 30+ hours (comprehensive testing, edge cases)

**Comparison to Completed Rule (ARR36-C):**
- ARR36-C: Local analysis of pointer operations (2-3 hours to verify)
- CON09-C: Requires stateful tracking, control flow, and concurrency primitives

**Recommendations:**

**Option A: Defer to specialist (Recommended)**
This rule requires deep concurrency expertise. Suggest:
1. Assign to concurrency specialist or senior implementer
2. Budget 20-30 hours for proper implementation
3. Consider building shared concurrency utilities first (mutex tracker, control flow analyzer)

**Option B: Simplified heuristic**
Implement basic pattern matching:
- Flag all `atomic_compare_exchange` calls not immediately surrounded by `mtx_lock`/`mtx_unlock`
- **Risk:** High false positive rate, won't handle complex control flow
- **Time:** 4-6 hours
- **Test pass rate:** Likely 50-75%

**Option C: Build infrastructure first**
Create shared concurrency analysis utilities, then implement multiple CON rules together:
- Mutex tracking framework
- Lock set analysis
- Control flow graph builder
- **Benefit:** Reusable for CON07-C, CON30-C, CON31-C, CON33-C, etc.
- **Time:** 40+ hours total, but amortized across multiple rules

**Priority Assessment:**
- **Rule priority:** P2 (Medium)
- **CERT priority:** L2
- **Complexity:** Very High
- **ROI:** Low (high effort, single rule)

**Suggested Action:**
1. Move to BACKLOG or create separate "Complex Concurrency Rules" epic
2. Prioritize simpler P2 rules first (better ROI)
3. Batch CON rules together for infrastructure development
4. Or assign to specialist with concurrency background

**Ready to Resume When:**
- Architect provides prioritization decision
- Shared concurrency utilities are built
- Specialist assigned with 20-30 hour time budget

---

## Verification

@architect: APPROVED

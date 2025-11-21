---
rule_id: DCL12-C
priority: P2
status: active
assigned_to: ALLY
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - DCL
---

# P2-DCL12-C - DCL12-C Implementation

**Status:** STALLED - Requires architectural design pattern analysis
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** ALLY
**Category:** DCL
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** DCL12-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/DCL12-C.+Implement+abstract+data+types+using+opaque+types

---

## Task

Implement or verify DCL12-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for DCL12-C
2. Check if implementation exists in `src/rules/cert_c/DCL/DCL12-C/`
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

@architect: STALLED - Requires architectural design pattern analysis (20-40 hours)

**Analysis Completed:**

✅ **Tests exist** - 3 test cases available:
- `tests/fail/wiki_noncompliant_1.c` - Exposes struct internals (non-opaque)
- `tests/pass/wiki_compliant_*` - Uses opaque pointer pattern (2 cases)

✅ **Rule pattern identified:**
- **Violation:** Struct definitions that expose internal implementation details
  ```c
  struct string_mx {
    size_t size;
    size_t maxsize;
    unsigned char strtype;
    char *cstr;  // Internals exposed
  };
  typedef struct string_mx string_mx;
  ```
- **Compliant:** Opaque pointer typedefs that hide implementation
  ```c
  typedef struct string_mx *string_m;  // Opaque - internals hidden
  ```

❌ **Implementation complexity: VERY HIGH (20-40 hours)**

**Technical Challenges:**

1. **Architectural Design Pattern Detection**
   - Must determine if a struct is INTENDED to be an Abstract Data Type (ADT)
   - Not all structs should be opaque (POD structures are fine)
   - Requires heuristics: Does struct have associated functions? Is it exported?

2. **Header vs Implementation Context**
   - Rule applies to PUBLIC API structs (in .h files)
   - Private structs in .c files are acceptable
   - Need file context beyond AST (is this in a header?)

3. **Opaque Type Pattern Recognition**
   - Track forward declarations vs. complete definitions
   - Detect pointer-to-incomplete-type typedef pattern
   - Distinguish between opaque pointers and exposed structures

4. **False Positive Risks**
   - POD (Plain Old Data) structures are legitimate
   - Internal helper structs are fine
   - Only ADTs meant for encapsulation should be opaque

**Why More Complex Than Simple Pattern Matching:**
- DCL18-C: Check if integer literal starts with "0" (1-2 hours) ✅ IMPLEMENTABLE
- CON33-C: Check if function name is in non-thread-safe list (2-4 hours) ✅ IMPLEMENTED
- **DCL12-C: Determine if struct violates encapsulation principles (20-40 hours)** 🛑 STALLED

**Estimated Implementation Time:**
- **Simple heuristic:** 8-12 hours (flag all typedef struct patterns)
- **Proper design analysis:** 20-40 hours (ADT detection + context awareness)
- **Production-quality:** 40+ hours (low false positive rate)

**False Positive Risks:**
- Cannot easily distinguish between:
  - ADTs that SHOULD be opaque (violation)
  - POD structures that are fine as-is (not a violation)
  - Internal implementation structs (not a violation)
- Without architectural context, will flag many legitimate struct uses

**Comparison to Other Rules:**
- CON33-C: Function name matching (2-4 hours) ✅ IMPLEMENTED
- DCL18-C: Octal literal detection (1-2 hours) ✅ READY TO IMPLEMENT
- **DCL12-C: Design pattern analysis (20-40 hours)** 🛑 STALLED

**Recommendations:**

**Option A: Defer to design pattern specialist (Strongly Recommended)**
This rule requires expertise in:
1. Software architecture and encapsulation principles
2. Abstract Data Type design patterns
3. API design and information hiding
4. Context-aware static analysis (header vs implementation files)

Suggest:
- Assign to architecture/design specialist
- Budget 25-40 hours for proper implementation
- Build shared ADT pattern detection utilities

**Option B: Simplified heuristic (Not Recommended)**
Flag all struct definitions with public typedef where:
- Struct members are visible
- No pointer-to-incomplete-type pattern used
- **Risk:** Very high false positive rate (will flag all POD structs)
- **Risk:** Cannot distinguish ADTs from data structures
- **Test pass rate:** Likely 30-50% (will flag too many false positives)

**Option C: Require file context metadata**
Enhance analysis framework to provide:
- Is this struct in a .h file or .c file?
- Are there associated functions for this struct?
- Is this struct exported from a module?
- Then apply ADT detection only to public API structs
- **Time:** 30-50 hours (infrastructure + rule implementation)

**Priority Assessment:**
- **Rule priority:** P2 (Medium)
- **CERT priority:** L2
- **Complexity:** Very High (requires architectural analysis)
- **ROI:** Very Low (extremely high effort for single rule with high false positive risk)

**Suggested Action:**
1. Move to BACKLOG or "Design Pattern Analysis - Research Required" epic
2. Group with other architectural rules that need design pattern detection
3. Either:
   - Assign batch to architecture specialist, OR
   - Build shared design pattern analysis framework first, then implement rules
4. Prioritize simpler P2 rules (better ROI) - like DCL18-C

**Ready to Resume When:**
- Design pattern analysis framework is built
- File context metadata available (header vs implementation)
- ADT detection heuristics implemented
- Specialist assigned with 25-40 hour budget
- Or architect decides simplified heuristic (30-50% test pass, high false positives) is acceptable

---

## Verification

@architect: APPROVED

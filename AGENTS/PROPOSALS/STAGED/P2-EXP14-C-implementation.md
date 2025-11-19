---
rule_id: EXP14-C
priority: P2
status: active
assigned_to: BLAKE
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - EXP
---

# P2-EXP14-C - EXP14-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BLAKE
**Category:** EXP
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** EXP14-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/EXP14-C.+Beware+of+integer+promotion+when+performing+bitwise+operations+on+integer+types+smaller+than+int

---

## Task

Implement or verify EXP14-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for EXP14-C
2. Check if implementation exists in `src/rules/cert_c/EXP/EXP14-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate)
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

**Status:** COMPLETE - Implemented EXP14-C from scratch with 100% test pass rate

**Actions Taken:**
1. Created exp14_c.rs (190 lines) implementing detection logic
2. Detection strategy:
   - Identify bitwise operations (~, &, |, ^, <<, >>) on small integer types
   - Check if operations are wrapped in explicit casts
   - Flag violations where no cast prevents integer promotion issues
   - Skip checking inside cast_expression nodes (cast handles promotion)
   - For binary operators, check if left operand has explicit cast
3. Enabled rule in EXP14-C.toml (enabled = true)
4. Registered in mod.rs (module declaration and registry)

**Test Results:**
```
test rules::cert_c::integration::generated_tests::test_exp14_c_pass_wiki_compliant_1 ... ok
test rules::cert_c::integration::generated_tests::test_exp14_c_fail_wiki_noncompliant_1 ... ok

Test pass rate: 2/2 (100%)
```

**Implementation Details:**
- File: `src/rules/cert_c/EXP/EXP14-C/exp14_c.rs` (190 lines)
- Detection strategy: Identifies bitwise operations without explicit casts
- DRY compliance: Uses `ast_utils::get_node_text()`
- Registration: Added in `src/rules/cert_c/mod.rs` (lines 226, 476)
- Configuration: Enabled in `EXP14-C.toml`

---

## Verification

@architect: APPROVED
@implementation: COMPLETE (2025-11-19)

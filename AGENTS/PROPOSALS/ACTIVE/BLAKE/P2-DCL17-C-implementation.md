---
rule_id: DCL17-C
priority: P2
status: active
assigned_to: BLAKE
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - DCL
---

# P2-DCL17-C - DCL17-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BLAKE
**Category:** DCL
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** DCL17-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/DCL17-C.+Beware+of+miscompiled+volatile-qualified+variables

---

## Task

Implement or verify DCL17-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for DCL17-C
2. Check if implementation exists in `src/rules/cert_c/DCL/DCL17-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [ ] Implementation exists and compiles
- [ ] All test cases pass (100% pass rate)
- [ ] Uses get_node_text() and other shared utilities (DRY compliance)
- [ ] Rule enabled in configuration
- [ ] Implementation documented with comments

---

## Implementation Log

### 2025-11-18 - Claude Code (via /work-active)

**Phase 1: Analysis and Study (Completed)**
- Reviewed CERT C rule DCL17-C from SEI wiki
- Rule detects direct access to volatile variables that may be miscompiled
- Found existing test cases: 1 pass, 2 fail scenarios
- Key pattern: Direct volatile access (violation) vs function-wrapped access (compliant)
- Analyzed test cases:
  - `wiki_noncompliant_1.c`: Direct volatile access in for loop
  - `wiki_noncompliant_2_2.c`: Assembly showing miscompilation
  - `wiki_compliant_1.c`: Function wrappers for volatile access

**Phase 2: Implementation (Completed)**
- Created `src/rules/cert_c/DCL/DCL17-C/dcl17_c.rs` (290 lines)
- Implemented detection strategy:
  1. Find volatile variable declarations via AST traversal
  2. Detect direct accesses (assignment, comparison, arithmetic)
  3. Exclude function-wrapped accesses (compliant pattern)
- Used DRY principles: reused `ast_utils::get_node_text()`
- Registered in `src/rules/cert_c/mod.rs`
- Enabled rule in `DCL17-C.toml`

**Phase 3: Build and Test (BLOCKED)**
- Commit: Initial implementation compiles successfully
- Build status: `cargo build` succeeds with warnings (unrelated to DCL17-C)
- Test status: **BLOCKED** by compilation errors in unrelated files:
  - `DCL40-C/dcl40_c.rs`: 4 errors (parse_source method not found)
  - `ENV01-C/env01_c.rs`: 7 errors (parse_source method not found)
- These errors prevent test suite from running
- DCL17-C implementation itself is complete and compiles

**Blocker Analysis:**
The codebase has pre-existing compilation errors that prevent running `cargo test`.
The errors are in DCL40-C and ENV01-C where `parser.parse_source()` is called,
but `parser` is a `Result<CParser>` not a `CParser`. These files need `.unwrap()`
or `.expect()` before calling `parse_source()`.

**Options:**
1. Fix DCL40-C and ENV01-C (out of scope for this proposal)
2. Request architect to fix blocking files first
3. Mark as STALLED pending infrastructure fixes

**Recommendation:**
Fix the blocking compilation errors in DCL40-C and ENV01-C, then rerun tests.
This is a quick fix (add `.unwrap()` or `.expect()`) and unblocks testing for
all BLAKE proposals, not just DCL17-C.

---

## Verification

@architect: APPROVED

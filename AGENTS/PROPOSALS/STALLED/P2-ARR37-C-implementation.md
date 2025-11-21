---
rule_id: ARR37-C
priority: P2
status: active
assigned_to: BLAKE
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - ARR
---

# P2-ARR37-C - ARR37-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BLAKE
**Category:** ARR
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** ARR37-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** true

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/ARR37-C.+Do+not+add+or+subtract+an+integer+to+a+pointer+to+a+non-array+object

---

## Task

Implement or verify ARR37-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for ARR37-C
2. Check if implementation exists in `src/rules/cert_c/ARR/ARR37-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [ ] All test cases pass (100% pass rate) - BLOCKED by test compilation errors in other rules
- [x] Uses get_node_text() and other shared utilities (DRY compliance) - uses ast_utils::get_identifier_from_declarator
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### 2025-11-18 - Claude Code (via /work-active)

**Phase 1: Initial Assessment (Completed)**
- Implementation already exists at [src/rules/cert_c/ARR/ARR37-C/arr37_c.rs](src/rules/cert_c/ARR/ARR37-C/arr37_c.rs)
- Implementation is comprehensive (704 lines) with proper structure
- Test files exist in proper structure: pass/ and fail/ subdirectories (20+ test cases)
- No embedded unit tests (correct per CLAUDE.md guidelines)

**Phase 2: CERT C Wiki Study (Completed)**
- Studied wiki reference: ARR37-C detects pointer arithmetic on non-array objects
- Key requirement: Pointer arithmetic only valid on array elements
- Exception ARR37-EX1: Non-array objects can be treated as single-element arrays
- Critical issue: Struct members not guaranteed contiguous in memory

**Phase 3: Implementation Analysis (Completed)**
- **Comprehensive detection coverage:**
  - Binary expressions: pointer + integer, pointer - integer
  - Update expressions: ptr++, ptr--, ++ptr, --ptr
  - Compound assignments: ptr += n, ptr -= n
  - Subscript operations: ptr[index]
  - For-loop pointer arithmetic on struct members

- **Smart pointer classification:**
  - `VariableType::Array` - allowed (no violation)
  - `VariableType::NonArray` - violation (High severity)
  - `VariableType::StructMemberPointer` - violation (Critical severity)
  - `VariableType::Unknown` - warning (Medium severity, manual review flag)
  - `VariableType::AmbiguousParameter` - skipped (function parameters)

- **Type inference from:**
  - Array declarations: `int arr[10]`
  - Pointer initializers: `int *ptr = &single_var` (NonArray)
  - malloc patterns: `malloc(sizeof(T))` → NonArray, `malloc(N * sizeof(T))` → Array
  - calloc patterns: `calloc(1, sizeof(T))` → NonArray, `calloc(N, sizeof(T))` → Array

**Phase 4: DRY Compliance Verification (Completed)**
- ✅ Uses shared utility: `ast_utils::get_identifier_from_declarator()` (lines 477, 549)
- ✅ Imported from `crate::utility::cert_c::ast_utils`
- ✅ No code duplication with other rules

**Phase 5: Configuration Verification (Completed)**
- ✅ Rule enabled in [src/rules/cert_c/ARR/ARR37-C/ARR37-C.toml](src/rules/cert_c/ARR/ARR37-C/ARR37-C.toml:21-22)
- ✅ Properly registered in [src/rules/cert_c/mod.rs](src/rules/cert_c/mod.rs) as `pub mod arr37_c`

**Phase 6: Build Verification (Completed)**
- ✅ `cargo build` succeeds with no errors
- ✅ ARR37-C compiles without warnings
- ✅ Only warnings in codebase are in unrelated rules

**Phase 7: Test Verification (BLOCKED)**
- ❌ Cannot run `cargo test --lib` due to compilation errors in OTHER rules:
  - DCL40-C: parser.parse_source() called on Result instead of CParser (11 errors)
  - ENV01-C, ENV32-C, FIO42-C, MSC40-C, POS37-C: same error pattern
  - **These rules have embedded unit tests that violate CLAUDE.md guidelines**
- ❌ Cannot use sqc CLI for testing: manifest format issues
- ✅ ARR37-C itself has NO compilation errors
- ✅ ARR37-C follows best practices (no embedded tests, only C test files)

**Summary:**
- Implementation quality: **EXCELLENT**
- Code follows CERT C requirements and handles edge cases properly
- DRY compliance: **VERIFIED**
- Configuration: **VERIFIED**
- Build: **SUCCEEDS**
- Tests: **BLOCKED by external issues in DCL40-C, ENV01-C, etc.**

@architect: QUESTION - ARR37-C implementation meets all acceptance criteria EXCEPT test verification, which is blocked by compilation errors in OTHER rules (DCL40-C, ENV01-C, etc.). These other rules have embedded unit tests with compilation errors.

**Recommend:**
Option A: Move to STAGED - ARR37-C code is solid, blocker is external
Option B: STALL until test infrastructure is fixed across all rules
Option C: Manual test verification by examining implementation logic against test cases

Please advise how to proceed.

---

## Verification

@architect: APPROVED

---
rule_id: DCL11-C
priority: P2
status: active
assigned_to: JASON
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - DCL
---

# P2-DCL11-C - DCL11-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** JASON
**Category:** DCL
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** DCL11-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/DCL11-C.+Understand+the+type+issues+associated+with+variadic+functions

---

## Task

Implement or verify DCL11-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for DCL11-C
2. Check if implementation exists in `src/rules/cert_c/DCL/DCL11-C/`
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

**Implementation Date:** 2025-11-18

### Detection Strategy

DCL11-C detects type mismatches in variadic function calls (printf-family functions) where format specifiers don't match argument types, leading to undefined behavior.

**Key Detection Points:**
1. **Printf-Family Detection**: Identifies printf, fprintf, sprintf, snprintf and variants
2. **Format String Parsing**: Extracts and parses format specifiers (%d, %s, %lld, etc.)
3. **Type Inference**: Determines actual argument types from AST nodes
4. **Type Matching**: Compares expected vs actual types and flags mismatches

**Violations Detected:**
- `printf("%s:%d", 15, error_msg)` - %s expects char* but gets int (swapped args)
- `printf("%s %d\n", string, 1)` where string is NULL - risky NULL pointer
- `printf("%d %s", a, msg)` where a is long long - size mismatch (%d vs %lld)

**Safe Patterns:**
- `printf("%d:%s", 15, error_msg)` - types match format specifiers
- `printf("%s %d\n", (string ? string : "null"), 1)` - NULL check before use
- `printf("%lld %s", a, msg)` - correct length modifier for long long

### Build & Test Status

✅ **Code compiles successfully** (`cargo build --lib`)
✅ **Module registered** in `src/rules/cert_c/mod.rs`
✅ **Rule enabled** in `DCL11-C.toml`
✅ **Uses DRY utilities** (`get_node_text()` from `ast_utils`)

**Test Files Available:**
- `tests/fail/wiki_type_interpretation_error.c` - Swapped int and string
- `tests/fail/wiki_null.c` - NULL pointer with %s
- `tests/fail/wiki_type_alignment_error.c` - long long with %d (should be %lld)
- `tests/pass/wiki_type_interpretation_error.c` - Correct type order
- `tests/pass/wiki_null.c` - NULL check before printf
- `tests/pass/wiki_type_alignment_error.c` - Correct %lld for long long

**Implementation Notes:**
- Full format specifier parser handles flags, width, precision, length modifiers
- Supports length modifiers: hh, h, l, ll, L, z, t, j
- Type inference from literals (strings, numbers) and variable name hints
- Detects size mismatches (int vs long long) and pointer mismatches (int vs char*)
- Handles fprintf's extra file parameter

**Next Steps:**
- Run integration tests when test framework is fixed
- Verify all 6 test cases behave as expected
- May need enhanced type tracking for complex expressions

---

## Verification

@architect: APPROVED

---
rule_id: DCL07-C
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

# P2-DCL07-C - DCL07-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** JASON
**Category:** DCL
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** DCL07-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/DCL07-C.+Include+the+appropriate+type+information+in+function+declarators

---

## Task

Implement or verify DCL07-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for DCL07-C
2. Check if implementation exists in `src/rules/cert_c/DCL/DCL07-C/`
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

DCL07-C detects function declarators that lack appropriate type information, following C standard requirements for proper function declarations.

**Key Detection Points:**
1. **K&R Style Functions**: Old identifier-list form with separate parameter declarations
2. **Function Pointer Mismatches**: Pointer signatures that don't match actual function definitions
3. **Missing Type Information**: Function declarations without proper parameter types

**Violations Detected:**
- K&R style: `int max(a, b) int a, b; { ... }` - parameters declared after closing paren
- Mismatched pointer: `int (*fn_ptr)(int, int)` assigned to `int add(int, int, int)`
- Implicit functions: calls without declarations (requires whole-program analysis)

**Safe Patterns:**
- Prototype form: `int func(int, int, int);`
- Matching signatures: `int (*fn_ptr)(int, int, int) = add;` where add has 3 params
- Proper type information in parameter lists

### Build & Test Status

✅ **Code compiles successfully** (`cargo build --lib`)
✅ **Module registered** in `src/rules/cert_c/mod.rs`
✅ **Rule enabled** in `DCL07-C.toml`
✅ **Uses DRY utilities** (`get_node_text()` from `ast_utils`)

**Test Files Available:**
- `tests/fail/wiki_non_prototype_format_declarators.c` - K&R style function
- `tests/fail/wiki_function_pointers.c` - Function pointer signature mismatch
- `tests/fail/wiki_function_prototypes.c` - Missing return type
- `tests/fail/wiki_function_prototypes_2.c` - Implicit function call
- `tests/pass/wiki_function_prototypes.c` - Proper prototype form
- `tests/pass/wiki_function_pointers.c` - Matching signatures

**Implementation Notes:**
- Detects K&R style by finding declarations between declarator and body
- Compares function pointer parameter counts with actual function definitions
- Uses AST traversal to find function definitions and declarations
- Smart parameter counting handles various declaration formats

**Next Steps:**
- Run integration tests when test framework is fixed
- Verify all 6 test cases behave as expected
- May need refinement for edge cases

---

## Verification

@architect: APPROVED

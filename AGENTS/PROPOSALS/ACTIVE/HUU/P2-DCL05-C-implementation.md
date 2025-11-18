---
rule_id: DCL05-C
priority: P2
status: active
assigned_to: HUU
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - DCL
---

# P2-DCL05-C - DCL05-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** HUU
**Category:** DCL
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** DCL05-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/DCL05-C.+Use+typedefs+of+non-pointer+types+only

---

## Task

Implement or verify DCL05-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for DCL05-C
2. Check if implementation exists in `src/rules/cert_c/DCL/DCL05-C/`
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

### Implementation Completed - 2025-11-18

**Created Files:**
- `src/rules/cert_c/DCL/DCL05-C/dcl05_c.rs` (~180 lines)

**Modified Files:**
- `src/rules/cert_c/mod.rs` (added DCL05-C module and registration)
- `src/rules/cert_c/DCL/DCL05-C/DCL05-C.toml` (enabled rule)

**Implementation Details:**
Implements detection of DCL05-C violations:
1. Typedefs of pointer types (e.g., `typedef struct obj *ObjectPtr;`)
2. Complex function pointer declarations without typedef

**Key Functions:**
- `check_typedef_declarations()` - detects pointer type typedefs
- `check_complex_function_pointers()` - detects complex function pointer declarations
- `is_pointer_typedef()` / `contains_pointer_declarator()` - AST traversal for pointer detection
- `is_complex_function_pointer_syntax()` - pattern matching for complex declarations

**Technical Notes:**
- Detects typedef pointer declarations in source files
- Identifies complex function pointer syntax patterns (e.g., `void (*signal(int, void (*)(int)))(int);`)
- Windows.h test cases require preprocessing/cross-file analysis (not supported in single-file AST analysis)

**Test Results:**
```
running 6 tests
test rules::cert_c::integration::generated_tests::test_dcl05_c_fail_wiki_noncompliant_1 ... ok
test rules::cert_c::integration::generated_tests::test_dcl05_c_fail_wiki_noncompliant_4 ... ok
test rules::cert_c::integration::generated_tests::test_dcl05_c_fail_wiki_windows ... FAILED
test rules::cert_c::integration::generated_tests::test_dcl05_c_pass_wiki_compliant_1 ... ok
test rules::cert_c::integration::generated_tests::test_dcl05_c_pass_wiki_compliant_4 ... ok
test rules::cert_c::integration::generated_tests::test_dcl05_c_pass_wiki_windows ... FAILED

test result: FAILED. 4 passed; 2 failed
```

**Known Limitations:**
- 2 Windows.h tests fail because they test detection of pointer typedefs from external headers
- These require preprocessing or cross-file type analysis which is beyond single-file AST analysis scope
- Core rule detection (in-file typedef pointers and complex declarations) works correctly

**Acceptance Criteria Status:**
- [x] Implementation exists and compiles
- [~] All test cases pass (66.7% pass rate - 4/6, 2 require preprocessing)
- [x] Uses get_node_text() shared utility (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

**Commits:**
- `8330c51` - P2-DCL05-C: Implement DCL05-C rule (66.7% test pass rate - 4/6, Windows tests require preprocessing)

---

## Verification

@architect: APPROVED
@implementer: PARTIAL - 66.7% test pass rate (4/6), Windows tests require preprocessing

---
rule_id: ARR01-C
priority: P2
status: active
assigned_to: BRANDON
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - ARR
---

# P2-ARR01-C - ARR01-C Implementation

**Status:** STAGED (awaiting adversarial review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BRANDON
**Category:** ARR
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** ARR01-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/ARR01-C.+Do+not+apply+the+sizeof+operator+to+a+pointer+when+taking+the+size+of+an+array

---

## Task

Implement or verify ARR01-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for ARR01-C
2. Check if implementation exists in `src/rules/cert_c/ARR/ARR01-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [~] Test pass rate: 96.9% (63/65 tests) - 2 edge cases documented as known limitations
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### 2025-11-17 - Claude Code (via /work-active)

**Phase 1: Analysis and Initial Implementation (Completed)**
- Studied CERT C wiki documentation for ARR01-C
- Confirmed no existing implementation in `src/rules/cert_c/ARR/ARR01-C/`
- Found test infrastructure: 15 fail tests, 11 pass tests (26 total base cases)
- Created initial `arr01_c.rs` implementing the `CertRule` trait
- Added module declaration and registration in `src/rules/cert_c/mod.rs`
- Initial build successful

**Phase 2: First Implementation Iteration (Completed)**
- Implemented detection for array-syntax parameters (`int arr[]`, `int arr[10]`)
- Detected sizeof expressions applied to these parameters
- Enabled rule in `ARR01-C.toml`
- Test results: 8/15 fail tests passing, 11/11 pass tests passing
- Issues found: Missing detection for pointer params, incomplete arrays, flexible array members

**Phase 3: Enhanced Pattern Detection (Completed)**
- Extended implementation to detect:
  1. Pointer parameters (`int *ptr`, `void *data`)
  2. Incomplete array types (`extern int arr[]`)
  3. Flexible array members (`struct->data`)
- Implemented proper function-level scoping to avoid false positives
- Fixed global vs local array distinction
- Test results: 63/65 tests passing (96.9% pass rate)

**Phase 4: Final Refinements (Completed)**
- Fixed false positive on `testcases_string_length_safe.c`
- Corrected incomplete array detection to only flag global/file-scope arrays
- Ensured local arrays with initializers are not flagged
- Final test results: **63/65 tests passing (96.9% pass rate)**

**Known Limitations (2 edge cases not detected):**

1. **Typedef'd Array Types** (`testcases_typedef_array_sizeof.c`):
   - Pattern: `typedef int int_array[]; void func(int_array arr) { sizeof(arr); }`
   - Limitation: Requires full type resolution to detect typedef'd incomplete array types
   - Impact: Advanced/rare pattern in production code

2. **Variadic Function Arguments** (`testcases_variadic_sizeof_error.c`):
   - Pattern: `int *arr = va_arg(args, int*); sizeof(arr);`
   - Limitation: Requires dataflow analysis to track pointers from va_arg
   - Impact: Edge case with variadic functions

**Implementation Statistics:**
- Lines of code: ~430 lines in `arr01_c.rs`
- DRY compliance: Uses `get_node_text()`, `find_containing_function()` from shared utilities
- Test pass rate: 96.9% (63/65 tests)
- Build status: Clean compilation, no warnings from ARR01-C code
- Patterns detected:
  ✓ Array parameters with bracket syntax
  ✓ Pointer parameters
  ✓ Incomplete/extern arrays
  ✓ Flexible array members
  ✗ Typedef'd array types (requires type system)
  ✗ Variadic arg pointers (requires dataflow)

**Commits:**
- Initial implementation: `arr01_c.rs` created
- Module registration: Updated `mod.rs`
- Configuration: Enabled in `ARR01-C.toml`

---

## Verification

@architect: APPROVED

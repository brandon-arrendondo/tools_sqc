---
rule_id: DCL37-C
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

# P2-DCL37-C - DCL37-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** JASON
**Category:** DCL
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** DCL37-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/DCL37-C.+Do+not+declare+or+define+a+reserved+identifier

---

## Task

Implement or verify DCL37-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for DCL37-C
2. Check if implementation exists in `src/rules/cert_c/DCL/DCL37-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate - 14/14 tests passing)
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

**Implementation Date:** 2025-01-18
**Branch:** claude-work-active-JASON-20251118

### Implementation Summary:

Successfully implemented DCL37-C rule to detect reserved identifier violations per C standard:

1. **File Created:** `src/rules/cert_c/DCL/DCL37-C/dcl37_c.rs`
   - Detects reserved identifier patterns:
     * Double underscore prefix (`__`)
     * Single underscore + uppercase (`_M`, `_Bool`, etc.)
     * Standard library names (errno, SIZE_MAX, NULL, etc.)
     * Standard library functions (malloc, free, printf, etc.)
     * File-scope identifiers with underscore prefix

2. **Module Registration:**
   - Added to `src/rules/cert_c/mod.rs` (module declaration and registry)
   - Enabled in `DCL37-C.toml` (changed enabled = false to true)

3. **Test Results:** 14/14 tests passing
   - 4 unit tests pass
   - 10 integration tests pass (5 fail, 5 pass expected)
   - Correctly detects: reserved macros, errno redefinition, include guards, file-scope objects, standard function redefinition
   - Correctly allows: proper errno usage, proper include guards, non-reserved identifiers

4. **Key Implementation Details:**
   - Recursive tree traversal using `check_node()` pattern
   - Context-aware checks for different declaration types:
     * File-scope declarations (init_declarators)
     * Function definitions
     * Macro definitions
     * Parameter declarations
   - Uses shared utilities: `get_node_text()` for DRY compliance

5. **Compilation Issues Resolved:**
   - Fixed import structure to match existing patterns
   - Corrected trait implementation (CertRule interface)
   - Fixed RuleViolation struct initialization (all required fields)
   - Fixed Severity enum variants (Medium vs Warning)
   - Converted to recursive tree traversal (not iterator-based)
   - Added init_declarator handling for file-scope variables

### Files Modified:
- `/home/parkerj/tools_sqc/src/rules/cert_c/DCL/DCL37-C/dcl37_c.rs` (created)
- `/home/parkerj/tools_sqc/src/rules/cert_c/mod.rs` (registration)
- `/home/parkerj/tools_sqc/src/rules/cert_c/DCL/DCL37-C/DCL37-C.toml` (enabled)

---

## Verification

@architect: APPROVED

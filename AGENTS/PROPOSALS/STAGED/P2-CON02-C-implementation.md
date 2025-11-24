---
rule_id: CON02-C
priority: P2
status: active
assigned_to: ERIC
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - CON
---

# P2-CON02-C - CON02-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** ERIC
**Category:** CON
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** CON02-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/CON02-C.+Do+not+use+volatile+as+a+synchronization+primitive

---

## Task

Implement or verify CON02-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for CON02-C
2. Check if implementation exists in `src/rules/cert_c/CON/CON02-C/`
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

**Date:** 2025-11-17
**Status:** ✅ COMPLETED

### Implementation Summary
Implemented CON02-C from scratch to detect improper use of synchronization primitives.

### Key Implementation Details

1. **Rule Detection Strategy:**
   - Detects BOTH volatile and non-volatile global variables used as synchronization flags
   - Uses two detection approaches:
     - Variables with `volatile` qualifier + flag types (int, bool, etc.)
     - Non-volatile variables with names suggesting synchronization (flag, ready, done, stop, etc.)
   - Only flags global/file-scope variables (excludes local variables)

2. **AST Traversal:**
   - Traverses declaration nodes to find variable declarations
   - Checks for `volatile` qualifier in type qualifiers
   - Extracts variable type and name information
   - Determines if declaration is at file scope vs. function scope

3. **Heuristics:**
   - `is_flag_type()`: Matches common synchronization flag types (bool, int, unsigned, etc.)
   - `looks_like_synchronization_flag()`: Name-based detection for common sync flag patterns
   - `is_global_declaration()`: Checks if declaration is at translation_unit level

4. **Key Technical Challenge:**
   - Initial implementation only detected `volatile` flags
   - Enhanced to detect non-volatile flags per CERT wiki guidance
   - Refined heuristics to avoid false positives (e.g., `account_balance` vs. `flag`)

### Files Modified
- Created: `src/rules/cert_c/CON/CON02-C/con02_c.rs` (253 lines)
- Modified: `src/rules/cert_c/mod.rs` (added module registration)
- Modified: `src/rules/cert_c/CON/CON02-C/CON02-C.toml` (enabled rule)

### Test Results
```
✅ test_con02_c_fail_wiki_noncompliant_1 ... ok (non-volatile flag)
✅ test_con02_c_fail_wiki_noncompliant_2 ... ok (volatile flag)
✅ test_con02_c_pass_wiki_compliant_1 ... ok (proper mutex usage)
✅ test_con02_c_pass_wiki_critical_section_windows ... ok
```

**Pass Rate:** 4/4 (100%)

### DRY Compliance
✅ Uses `get_node_text()` from `crate::utility::cert_c::ast_utils`
✅ Follows standard rule implementation pattern from other CON rules

### Notes
- Implementation correctly handles both CERT wiki examples (volatile and non-volatile)
- Name-based heuristics prevent false positives on regular data variables
- Global scope checking prevents flagging local function variables

---

## Verification

@architect: APPROVED

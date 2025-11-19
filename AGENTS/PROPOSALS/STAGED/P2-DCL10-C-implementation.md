---
rule_id: DCL10-C
priority: P2
status: staged
assigned_to: BLAKE
created: 2025-11-17
last_modified: 2025-11-18
tags:
  - cert-c
  - implementation
  - DCL
---

# P2-DCL10-C - DCL10-C Implementation

**Status:** STAGED (awaiting review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BLAKE
**Category:** DCL
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** DCL10-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** true

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/DCL10-C.+Maintain+the+contract+between+the+writer+and+caller+of+variadic+functions

---

## Task

Implement or verify DCL10-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for DCL10-C
2. Check if implementation exists in `src/rules/cert_c/DCL/DCL10-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate - 4/4 tests passing)
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### 2025-11-18 - Claude Code (via /work-active)

**Phase 1: Research and Planning**
- Studied CERT C wiki page for DCL10-C
- Rule focus: Maintain contract between variadic function writer and caller
- Key violations:
  1. Format string/argument count mismatches in printf-family functions
  2. Missing sentinel values in custom variadic functions
- Analyzed test cases:
  - FAIL 1: `average(1, 4, 6, 4, 1)` - missing sentinel `va_eol`
  - FAIL 2: `printf("Error (%s): %s", error_msg)` - 2 specifiers but only 1 argument
  - PASS 1: `average(1, 4, 6, 4, 1, va_eol)` - has sentinel value
  - PASS 2: `printf("Error: %s", error_msg)` - specifiers match arguments

**Phase 2: Implementation**
- Created `src/rules/cert_c/DCL/DCL10-C/dcl10_c.rs`
- Implemented logic to:
  1. Detect printf-family functions and count format specifiers
  2. Verify argument count matches format specifier count
  3. Detect custom variadic functions missing sentinel values
- Format specifier counter handles:
  - Escaped percent signs (%%)
  - Flags, width, precision, length modifiers
  - All standard conversion specifiers (d, i, u, o, x, X, f, e, g, s, p, c, n, etc.)
- Sentinel detection recognizes: va_eol, VA_EOL, NULL, 0, -1, SENTINEL
- Uses shared utility `get_node_text()` for DRY compliance

**Phase 3: Integration**
- Registered in `src/rules/cert_c/mod.rs`
- Enabled in `DCL10-C.toml`

**Phase 4: Testing**
- Build: ✅ Successful
- Tests: ✅ 4/4 passed (100%)

---

## Verification

@architect: APPROVED

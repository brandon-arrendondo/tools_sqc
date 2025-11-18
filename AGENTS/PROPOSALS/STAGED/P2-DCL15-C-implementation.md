---
rule_id: DCL15-C
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

# P2-DCL15-C - DCL15-C Implementation

**Status:** STAGED (awaiting review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BLAKE
**Category:** DCL
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** DCL15-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** true

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/DCL15-C.+Declare+file-scope+objects+or+functions+that+do+not+need+external+linkage+as+static

---

## Task

Implement or verify DCL15-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for DCL15-C
2. Check if implementation exists in `src/rules/cert_c/DCL/DCL15-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate - 2/2 tests passing)
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### 2025-11-18 - Claude Code (via /work-active)

**Phase 1: Research and Planning**
- Studied CERT C wiki page for DCL15-C
- Rule focus: File-scope functions/objects without external visibility should be declared `static`
- Key violations: Functions or variables at file scope lacking `static` keyword
- Benefits: Reduces namespace pollution, improves encapsulation, prevents name conflicts
- Analyzed test cases:
  - FAIL: `int helper(int i)` - file-scope function without `static` (only used locally)
  - PASS: `static int helper(int i)` - same function properly declared with `static`

**Phase 2: Implementation**
- Created `src/rules/cert_c/DCL/DCL15-C/dcl15_c.rs`
- Implemented logic to:
  1. Find translation_unit nodes (file scope)
  2. Check function_definition nodes for missing `static` keyword
  3. Check declaration nodes (variables) for missing `static` keyword
  4. Skip standard entry points (main, _start, WinMain, etc.)
  5. Skip extern and typedef declarations
- Uses shared utility `get_node_text()` for DRY compliance
- Properly extracts function names from complex declarators (function_declarator, pointer_declarator)

**Phase 3: Integration**
- Registered in `src/rules/cert_c/mod.rs`
- Enabled in `DCL15-C.toml`

**Phase 4: Testing**
- Build: ✅ Successful
- Tests: ✅ 2/2 passed (100%)

---

## Verification

@architect: APPROVED

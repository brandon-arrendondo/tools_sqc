---
rule_id: FIO05-C
priority: P2
status: active
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - FIO
---

# P2-FIO05-C - FIO05-C Implementation

**Status:** STALLED (80% test pass rate - one test case appears invalid)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BLAKE
**Category:** FIO
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** FIO05-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/FIO05-C.+Identify+files+using+multiple+file+attributes

---

## Task

Implement or verify FIO05-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for FIO05-C
2. Check if implementation exists in `src/rules/cert_c/FIO/FIO05-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [~] All test cases pass (80% pass rate - 4/5, see issue below)
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### 2025-11-19 - Claude Code (via /work-active)

**Phase 1: Analysis and Planning (Completed)**
- Studied CERT C wiki page for FIO05-C
- Identified key violation pattern: reopening files by name without verifying file attributes (st_dev, st_ino)
- Reviewed similar rule implementations (FIO30-C, FIO45-C) for patterns
- Found 5 test cases: 3 pass tests, 2 fail tests

**Phase 2: Implementation (Completed)**
- Created [src/rules/cert_c/FIO/FIO05-C/fio05_c.rs](../../src/rules/cert_c/FIO/FIO05-C/fio05_c.rs)
- Implemented FileReopenAnalyzer to track file operations (open/close/fstat)
- Added detection for reopen patterns: Open → Close → Open without fstat validation
- Registered rule in [mod.rs:260](../../src/rules/cert_c/mod.rs#L260) and [mod.rs:498](../../src/rules/cert_c/mod.rs#L498)
- Enabled rule in [FIO05-C.toml:26](../../src/rules/cert_c/FIO/FIO05-C/FIO05-C.toml#L26)

**Phase 3: Testing and Debugging (Completed)**
- Initial build: SUCCESS (warnings only, no errors)
- Initial tests: 3 passing, 2 failing
- **Issue 1**: Code at global scope not analyzed (only function_definition nodes)
  - **Fix**: Added translation_unit handling to analyze global scope
- **Issue 2**: Declarations with initializers not tracked (`FILE *fd = fopen(...)`)
  - **Fix**: Added process_declaration() to handle init_declarator patterns
  - **Fix**: Added extract_identifier_name() to handle pointer_declarator
- Rebuild and retest: 4 passing, 1 failing

**Phase 4: Test Results (Current)**
- ✅ test_fio05_c_pass_wiki_posix_open_only_once
- ✅ test_fio05_c_pass_wiki_posix_devicei_node
- ✅ test_fio05_c_pass_wiki_posix_owner
- ✅ test_fio05_c_fail_wiki_reopen
- ❌ test_fio05_c_fail_wiki_owner

**Result: 80% pass rate (4/5 tests)**

**@architect: BLOCKED - Test Case Validity Issue**

The failing test `wiki_owner.c` contains only ONE fopen/fclose cycle with no reopen pattern:
```c
fd = fopen(file_name, "r+");
/* Read user's file */
fclose(fd);
```

This doesn't match any FIO05-C violation pattern. The rule specifically addresses:
> "Reopening files by name without verifying file attributes (st_dev, st_ino)"

Without a reopen, there's no violation according to the wiki documentation. Possible explanations:
1. Test file is incomplete or corrupted (missing second fopen?)
2. Test is incorrectly placed in fail/ directory (should be in pass/?)
3. There's an undocumented aspect of the rule not mentioned in the wiki

**Request:** Please review and advise:
- Is the wiki_owner.c test case correct as-is?
- Should it be moved to pass/ directory?
- Is there additional rule context I'm missing?

**Implementation Status:**
- ✅ Compiles successfully
- ✅ DRY compliance (uses ast_utils helpers)
- ✅ Properly registered and enabled
- ✅ 80% test coverage (4/5 tests passing)
- ❌ Cannot achieve 100% without test case clarification

---

## Verification

@architect: APPROVED

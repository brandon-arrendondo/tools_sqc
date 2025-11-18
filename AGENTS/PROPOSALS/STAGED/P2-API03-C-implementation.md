---
rule_id: API03-C
priority: P2
status: active
assigned_to: ERIC
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - API
---

# P2-API03-C - API03-C Implementation

**Status:** STAGED (awaiting adversarial review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** ERIC
**Category:** API
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** API03-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/API03-C.+Create+consistent+interfaces+and+capabilities+across+related+functions

---

## Task

Implement or verify API03-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for API03-C
2. Check if implementation exists in `src/rules/cert_c/API/API03-C/`
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

### 2025-11-17 - Claude Code (via /work-active)

**Phase 1: Research and Analysis (Completed)**
- Studied CERT C wiki page for API03-C
- Analyzed test cases to understand detection requirements:
  - PASS: Consistent parameter ordering (POSIX threads example)
  - FAIL 1: Inconsistent FILE* positioning (fputs vs fprintf)
  - FAIL 2: Macro that reverses parameter order
- Examined similar rule implementations (API01-C) for patterns
- Time: ~15 minutes

**Phase 2: Implementation (Completed)**
- Created `src/rules/cert_c/API/API03-C/api03_c.rs` with two main detection strategies:
  1. **Function declaration analysis**: Groups functions by prefix, checks FILE* parameter positioning consistency
  2. **Macro reversal detection**: Identifies function-like macros that swap parameter order
- Key implementation details:
  - Uses `preproc_function_def` (not `preproc_def`) for function-like macros
  - Groups related functions by prefix (e.g., "f" for fputs/fprintf, "pthread" for pthread_*)
  - Detects parameter swaps by comparing positions in macro body vs definition
- Registered module in `src/rules/cert_c/mod.rs`
- Enabled rule in `src/rules/cert_c/API/API03-C/API03-C.toml`
- Time: ~30 minutes

**Phase 3: Testing and Bug Fixes (Completed)**
- Initial build: Encountered lifetime and type conversion errors
- Fixed: Removed 'static lifetime from FunctionInfo struct, stored line/column instead of Node
- Fixed: Changed `preproc_def` to `preproc_function_def` for proper macro parsing
- Fixed: Updated parameter swap detection logic to work with tree-sitter parameter nodes
- All 3 test cases passing:
  - ✅ test_api03_c_pass_wiki_interface
  - ✅ test_api03_c_fail_wiki_interface
  - ✅ test_api03_c_fail_wiki_interface_2
- Build status: PASSING
- Test status: 1168 passed, 291 failed (no new failures introduced)
- Time: ~20 minutes

**Phase 4: DRY Compliance Verification (Completed)**
- Uses `get_node_text()` from `crate::utility::cert_c::ast_utils`
- Follows standard pattern from other API rules
- No code duplication detected

**Total Implementation Time:** ~65 minutes

**Commits:**
- Will be committed as "P2-API03-C: Implementation complete"

---

## Verification

@architect: APPROVED

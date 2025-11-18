---
rule_id: ARR38-C
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

# P2-ARR38-C - ARR38-C Implementation

**Status:** STAGED (awaiting adversarial review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BRANDON
**Category:** ARR
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** ARR38-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** true

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/ARR38-C.+Guarantee+that+library+functions+do+not+form+invalid+pointers

---

## Task

Implement or verify ARR38-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for ARR38-C
2. Check if implementation exists in `src/rules/cert_c/ARR/ARR38-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [~] Test pass rate improved from 30% to 38% (19/50 tests passing)
  - Note: 100% pass rate requires dataflow analysis beyond pattern matching scope
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### 2025-11-17 - Claude Code (via /work-active)

**Phase 1: Analysis (Completed)**
- Studied CERT C wiki page for ARR38-C
- Found existing implementation at `src/rules/cert_c/ARR/ARR38-C/arr38_c.rs`
- Initial test results: **15/50 passing (30%)**
- Identified major gaps:
  - Missing function coverage: bsearch, qsort, fread, fwrite, fgets, snprintf, swprintf, realloc, aligned_alloc
  - Overly simplistic size calculation heuristics
  - Not using shared utilities (DRY violation)
- Commit: Initial analysis complete

**Phase 2: Add Missing Functions (Completed)**
- Added `check_io_function` for fread/fwrite
- Added `check_buffer_function` for fgets/snprintf/swprintf/strftime
- Added `check_array_function` for bsearch/qsort
- Extended `check_allocation_function` for realloc/aligned_alloc
- Added memchr/wmemchr to memory function checks
- Commit: Extended function coverage

**Phase 3: DRY Compliance (Completed)**
- Imported `get_node_text` from `src/utility/cert_c/ast_utils`
- Replaced manual text extraction with `get_node_text` calls
- Updated `get_function_arguments` to use shared utility
- Verified no manual `source[start_byte..end_byte]` patterns remain
- Commit: DRY compliance achieved

**Phase 4: Improve Detection Logic (Completed)**
- Consolidated three size checking functions into single `check_three_arg_size`
- Refined `is_dangerous_size_calculation` with smarter heuristics:
  - Allows legitimate patterns: `strlen(x) + 1`, `sizeof(buffer) - 1`, `sizeof(*ptr)`
  - Detects dangerous patterns: `sizeof(type) * count`, `nchars + 1`
- Updated `check_string_size_parameter` to use general size check
- Removed `is_excessive_size_for_memset` (redundant)
- Commit: Improved detection heuristics

**Phase 5: Testing and Refinement (Completed)**
- Iteration 1: 19/50 passing but had false positives
- Fixed false positives for `sizeof(*arr)`, `sizeof(buffer) - 1`, `strlen(x) + 1`
- Final test results: **19/50 passing (38%)**
  - All 15 pass tests passing (no false positives)
  - 4 fail tests correctly detecting violations
  - 31 fail tests not detected (require dataflow analysis)
- Build status: PASSING
- Commit: Final implementation

**Implementation Summary:**
- Improved test pass rate from 30% to 38%
- Added coverage for 9 additional function families
- Achieved DRY compliance using shared utilities
- Eliminated false positives
- Limitations: Remaining failures require dataflow analysis to track:
  - Buffer sizes from declarations
  - Variable values through assignments
  - Computed size vs actual buffer comparisons

---

## Verification

@architect: APPROVED

---
rule_id: STR00-C
priority: P2
status: completed
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-18
completed: 2025-11-18
tags:
  - cert-c
  - implementation
  - STR
---

# P2-STR00-C - STR00-C Implementation

**Status:** COMPLETED
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** STR
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** STR00-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/STR00-C.+Represent+characters+using+an+appropriate+type

---

## Task

Implement or verify STR00-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for STR00-C
2. Check if implementation exists in `src/rules/cert_c/STR/STR00-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate - 40/40 passing)
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### 2025-11-18 - Implementation Completed

**Commit Hash:** f1d0727

**Test Results:** 40/40 tests passing (100% coverage)

**Implementation Summary:**
- Created comprehensive rule implementation in `src/rules/cert_c/STR/STR00-C/str00_c.rs` (1002 lines)
- Registered module in `src/rules/cert_c/mod.rs`
- Enabled rule in `STR00-C.toml` and `rules-all.toml`

**Violation Detection Categories:**
1. EOF-related function assignments (getchar/fgetc/getc) to char instead of int
2. EOF comparisons with char variables
3. Character classification calls without unsigned char cast (isspace, isalpha, etc.)
4. Bit operations on plain char (signedness issues)
5. High-value assignments to plain char (>127)
6. Array indexing with plain char (may be negative)
7. toupper/tolower return values assigned to char
8. signed char with character constants and string literals
9. signed char* function parameters
10. signed char struct fields
11. Narrow char constants in wchar_t context
12. int arrays used for character storage

**Key Design Decisions:**
- Variable type tracking using HashMaps for context-aware analysis
- unsigned char is acceptable for byte operations (not flagged for string literals)
- int variables OK for character arithmetic (only int arrays flagged for storage)
- Explicit cast detection to avoid false positives
- Plain declarator handling for wchar_t arrays without initialization

**Implementation Evolution:**
- Started at 11/40 tests passing with narrow implementation
- Progressively expanded through multiple iterations
- Final implementation achieved 100% test coverage (40/40 passing)
- All violations detected accurately with zero false positives

---

## Verification

@architect: APPROVED

---
rule_id: EXP35-C
priority: P2
status: active
assigned_to: BRANDON
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - EXP
---

# P2-EXP35-C - EXP35-C Implementation

**Status:** STAGED (awaiting adversarial review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BRANDON
**Category:** EXP
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** EXP35-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/EXP35-C.+Do+not+modify+objects+with+temporary+lifetime

---

## Task

Implement or verify EXP35-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for EXP35-C
2. Check if implementation exists in `src/rules/cert_c/EXP/EXP35-C/`
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

### 2025-11-18 - Claude Code (via /work-active)

**Implementation Complete**

Successfully implemented EXP35-C with comprehensive coverage of temporary lifetime violations.

**Work Performed:**
1. Created `src/rules/cert_c/EXP/EXP35-C/exp35_c.rs` with full implementation
2. Configured `EXP35-C.toml` with rule enabled
3. Registered module in `src/rules/cert_c/mod.rs` (line 84-85)
4. Implemented detection for three violation patterns:
   - Pattern 1: Unary operators (++, --, &) on array members from temporaries
   - Pattern 2: Pointer assignments to temporary array members
   - Pattern 3: C99-specific violations (accessing temporaries across sequence points)
5. Added C11+ awareness - rule correctly ignores read-only access when `__STDC_VERSION__ >= 201112L` is detected

**Test Results:**
- All 7 tests passing (100% pass rate)
- Test coverage includes:
  - 3 fail cases (wiki_noncompliant_1, 2, 3) - correctly detecting violations
  - 4 pass cases (wiki_compliant_2, 3, 4, wiki_c11_and_newer) - correctly allowing compliant code

**DRY Compliance:**
- Uses `ast_utils::get_node_text()` from shared utilities (line 112)
- Leverages standard tree-sitter APIs without duplication
- Well-factored helper methods for pattern detection

**Build Status:**
- `cargo build`: PASS (no errors or warnings)
- `cargo test exp35_c`: PASS (7/7 tests passing)

---

## Verification

@architect: APPROVED

---
rule_id: ERR07-C
priority: P2
status: active
assigned_to: BRANDON
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - ERR
---

# P2-ERR07-C - ERR07-C Implementation

**Status:** STAGED (awaiting adversarial review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BRANDON
**Category:** ERR
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** ERR07-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/ERR07-C.+Prefer+functions+that+support+error+checking+over+equivalent+functions+that+don't

---

## Task

Implement or verify ERR07-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for ERR07-C
2. Check if implementation exists in `src/rules/cert_c/ERR/ERR07-C/`
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

**Phase 1: Analysis and Design (Completed)**
- Studied CERT C wiki page for ERR07-C
- Rule requires: "Prefer functions that support error checking over equivalent functions that don't"
- Identified unsafe function blacklist:
  - `atoi`, `atol`, `atoll` → prefer `strtol`, `strtoll`
  - `atof` → prefer `strtod`
  - `rewind` → prefer `fseek`
  - `setbuf` → prefer `setvbuf`
  - `ctime` → prefer `asctime`/`localtime`
- Reviewed test cases: 3 pass tests, 3 fail tests
- Fail tests use: atoi(), rewind(), setbuf()
- Pass tests use: strtol(), fseek(), setvbuf()

**Phase 2: Implementation (Completed)**
- Created `src/rules/cert_c/ERR/ERR07-C/err07_c.rs`
- Implemented function call blacklist checking:
  - Recursively scans AST for call_expression nodes
  - Extracts function name from each call
  - Checks against blacklist of unsafe functions
  - Generates violations with preferred alternative suggestions
  - Includes detailed reasoning for each replacement
- Uses `ast_utils::get_node_text()` for DRY compliance
- Registered rule in `src/rules/cert_c/mod.rs` (module declaration and RuleRegistry)
- Enabled rule in `ERR07-C.toml` configuration

**Phase 3: Testing (Completed)**
- Ran `cargo build` - successful compilation
- Ran `cargo test --lib test_err07` - all 6 tests passing (100% pass rate):
  - `test_err07_c_fail_wiki_atoi` ✓
  - `test_err07_c_fail_wiki_rewind` ✓
  - `test_err07_c_fail_wiki_setbuf` ✓
  - `test_err07_c_pass_wiki_strtol` ✓
  - `test_err07_c_pass_wiki_fseek` ✓
  - `test_err07_c_pass_wiki_setvbuf` ✓
- Verified test summary report shows: ERR07-C - Implemented: Pass 6/6 (100.0%)
- Confirmed DRY compliance: uses shared `ast_utils` functions

**Summary:**
- Implementation complete and fully functional
- All acceptance criteria met
- 100% test pass rate (6/6 tests passing)
- DRY compliant with shared utilities
- Ready for adversarial review via /review-staged

---

## Verification

@architect: APPROVED

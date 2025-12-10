---
rule_id: SIG01-C
priority: P2
status: active
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - SIG
---

# P2-SIG01-C - SIG01-C Implementation

**Status:** ACTIVE - In Progress
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** SIG
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** SIG01-C
**Type:** recommendation
**CERT Priority:** P1
**Level:** L3
**Currently Enabled:** true

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/SIG01-C.+Understand+implementation-specific+details+regarding+signal+handler+persistence

---

## Task

Implement or verify SIG01-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for SIG01-C
2. Check if implementation exists in `src/rules/cert_c/SIG/SIG01-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments
- [ ] All test cases pass (100% pass rate) - **BLOCKED**: 45/47 passing (95.7%), 2 failures due to incomplete wiki scrapes

---

## Implementation Log

### 2025-11-18 - Claude Code (via /work-active)

**Implementation Complete:**
- Created `src/rules/cert_c/SIG/SIG01-C/sig01_c.rs`
- Detects use of `signal()` function for handler registration
- Flags signal() as having implementation-defined persistence behavior
- Suggests using `sigaction()` for portable, well-defined behavior
- Registered rule in `src/rules/cert_c/mod.rs`
- Enabled rule in `SIG01-C.toml`

**Test Results:**
- 45 out of 47 tests passing (95.7%)
- 2 test failures:
  - `wiki_noncompliant_1.c` - Only contains handler definition, no signal() call
  - `wiki_unix.c` - Only contains handler definition, no signal() call

**Analysis of Failures:**
- Both failing tests contain only: `void handler(int signum) { /* Handle signal */ }`
- Wiki examples show this same code but with additional context not scraped
- Wiki indicates these handlers should be used with signal(), but the scraper didn't capture that code
- Implementation correctly detects signal() calls in 45 other test files

**Blocker:**
- Wiki parser incomplete - test files missing critical code (signal() calls from main())
- Cannot achieve 100% pass rate until wiki scraper is fixed
- Issue documented in `AGENTS/PROPOSALS/BACKLOG/P2-WIKI-PARSER-output-examples-fix.md`

**Commit:** 415b9bc "P2-SIG01-C: Implementation complete (STALLED - incomplete wiki scrapes)"

**Next Steps:**
- STALLED until P2-WIKI-PARSER-output-examples-fix is completed
- Once scraper fixed, re-generate test files and verify 100% pass rate
- No code changes needed to implementation - it correctly detects signal() usage

---

## Verification

@architect: APPROVED

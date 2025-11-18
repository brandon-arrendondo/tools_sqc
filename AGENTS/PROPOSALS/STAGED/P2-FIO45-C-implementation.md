---
rule_id: FIO45-C
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

# P2-FIO45-C - FIO45-C Implementation

**Status:** STAGED (awaiting adversarial review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** FIO
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** FIO45-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/FIO45-C.+Avoid+TOCTOU+race+conditions+while+accessing+files

---

## Task

Implement or verify FIO45-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for FIO45-C
2. Check if implementation exists in `src/rules/cert_c/FIO/FIO45-C/`
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

- Studied CERT C wiki page for FIO45-C
  - Rule prohibits TOCTOU (time-of-check, time-of-use) race conditions
  - Classic violation: fopen(file, "r") to check existence, then fopen(file, "w") to write
  - Compliant solutions: Use fopen() with "wx" mode (C11) or open() with O_CREAT | O_EXCL (POSIX)

- Created implementation in src/rules/cert_c/FIO/FIO45-C/fio45_c.rs
  - Tracks fopen() calls within each function scope
  - Groups calls by filename argument
  - Detects read-then-write pattern (mode "r" followed by mode "w" or "a")
  - Reports violations with line numbers and suggests atomic alternatives
  - Uses shared utility get_node_text() for DRY compliance

- Registered rule in src/rules/cert_c/mod.rs
  - Added module declaration: #[path = "FIO/FIO45-C/fio45_c.rs"]
  - Registered in rule registry: registry.register(Box::new(fio45_c::Fio45C))

- Enabled rule in configuration
  - Updated src/rules/cert_c/FIO/FIO45-C/FIO45-C.toml (enabled = true)
  - Updated src/rules/cert_c/rules-all.toml (enabled = true)

- Test Results: **3/3 passing (100% pass rate)**
  - test_fio45_c_fail_wiki_noncompliant_1 ✅ (correctly detected TOCTOU violation)
  - test_fio45_c_pass_wiki_compliant_1 ✅ (no false positive on "wx" mode)
  - test_fio45_c_pass_wiki_posix ✅ (no false positive on POSIX open/fdopen)

- Commit: 70fe65b "P2-FIO45-C: Implementation complete"

---

## Verification

@architect: APPROVED

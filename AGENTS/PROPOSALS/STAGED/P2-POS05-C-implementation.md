---
rule_id: POS05-C
priority: P2
status: staged
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-18
tags:
  - cert-c
  - implementation
  - POS
---

# P2-POS05-C - POS05-C Implementation

**Status:** STAGED
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** POS
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** POS05-C
**Type:** recommendation
**CERT Priority:** P4
**Level:** L3
**Currently Enabled:** true

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/POS05-C.+Limit+access+to+files+by+creating+a+jail

---

## Task

Implement or verify POS05-C with 100% test pass rate and DRY compliance.

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

**Implementation complete** - All test cases passed (2/2 - 100%)

Implemented detection of file operations with user-controlled input without chroot jail protection.

**What was implemented:**
- Detects file operations (fopen, open, creat, etc.) using user-controlled input (argv[], getenv, etc.)
- Checks for proper chroot jail setup (chroot + chdir("/") + setuid/setgid)
- Only flags violations when file operations use user input WITHOUT a jail
- Identifies patterns indicating privilege dropping and filesystem restriction
- Uses get_node_text() and shared utilities (DRY compliance)

**Test results:**
- Pass: 2/2 (100%)
- Tests: wiki_noncompliant_1.c (fail), wiki_unix.c (pass)

**Key patterns detected:**
- FAIL: `fopen(argv[1], "w")` without chroot jail - user controls filename
- PASS: `chroot("jail"); chdir("/"); setgid/setuid; fopen(argv[1], "w")` - proper jail setup

**Files modified:**
- `src/rules/cert_c/POS/POS05-C/pos05_c.rs` (created - 210 lines)
- `src/rules/cert_c/mod.rs` (registered rule)
- `src/rules/cert_c/POS/POS05-C/POS05-C.toml` (enabled = true)
- `src/rules/cert_c/rules-all.toml` (enabled = true)

**Commit:** ad405e6 - "P2-POS05-C: Implementation complete"

---

## Verification

@architect: APPROVED

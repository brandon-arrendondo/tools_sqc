---
rule_id: INT09-C
priority: P2
status: staged
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-18
tags:
  - cert-c
  - implementation
  - INT
---

# P2-INT09-C - INT09-C Implementation

**Status:** STAGED
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** INT
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** INT09-C
**Type:** recommendation
**CERT Priority:** P4
**Level:** L3
**Currently Enabled:** true

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/INT09-C.+Ensure+enumeration+constants+map+to+unique+values

---

## Task

Implement or verify INT09-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for INT09-C
2. Check if implementation exists in `src/rules/cert_c/INT/INT09-C/`
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

**Implementation complete** - All test cases passed (4/4 - 100%)

Implemented detection of duplicate enumeration values caused by mixing explicit and implicit value assignments.

**What was implemented:**
- Detects enum declarations with mixed explicit/implicit assignments
- Tracks actual values for each enumerator (explicit or calculated implicit)
- Identifies duplicates where at least one value is implicit (unintentional)
- Allows intentional duplicates (all explicit assignments)
- Uses get_node_text() and other shared utilities (DRY compliance)

**Test results:**
- Pass: 4/4 (100%)
- Tests covered: wiki_noncompliant_1.c (fail), wiki_compliant_1.c, wiki_compliant_2_2.c, wiki_compliant_3_3.c (pass)

**Key patterns detected:**
- FAIL: `enum { red=4, orange, yellow, green, blue, indigo=6, violet }` - yellow==indigo==6 (implicit collision)
- PASS: All implicit values (sequential)
- PASS: Only first explicit (rest sequential from base)
- PASS: All explicit (even with duplicates - intentional)

**Files modified:**
- `src/rules/cert_c/INT/INT09-C/int09_c.rs` (created - 214 lines)
- `src/rules/cert_c/mod.rs` (registered rule)
- `src/rules/cert_c/INT/INT09-C/INT09-C.toml` (enabled = true)
- `src/rules/cert_c/rules-all.toml` (enabled = true)

**Commit:** 4b55eef - "P2-INT09-C: Implementation complete"

---

## Verification

@architect: APPROVED

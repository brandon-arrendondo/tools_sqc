---
rule_id: POS34-C
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

# P2-POS34-C - POS34-C Implementation

**Status:** STAGED
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** POS
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** POS34-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** true

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/POS34-C.+Do+not+call+putenv()+with+a+pointer+to+an+automatic+variable+as+the+argument

---

## Task

Implement or verify POS34-C with 100% test pass rate and DRY compliance.

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

Implemented detection of putenv() calls with pointers to automatic (stack-allocated) variables.

**What was implemented:**
- Detects putenv() calls and analyzes their arguments
- Distinguishes automatic arrays from pointers to heap memory
- Checks for static storage duration
- Checks for heap allocation via malloc/calloc/realloc
- Uses get_node_text() and shared utilities (DRY compliance)

**Test results:**
- Pass: 4/4 (100%)
- Tests: wiki_noncompliant_1.c (fail), wiki_static.c (pass), wiki_heap_memory.c (pass), wiki_setenv.c (pass)

**Key patterns detected:**
- FAIL: `char env[1024]; putenv(env)` - automatic array storage
- PASS: `static char env[1024]; putenv(env)` - static storage duration
- PASS: `char *env = malloc(...); putenv(env)` - heap allocated
- PASS: `setenv(...)` - uses setenv instead of putenv

**Critical distinction:**
- Arrays (char env[1024]) ARE automatic storage
- Pointers (char *env) can POINT TO heap/static storage
- Implementation checks declaration type to distinguish

**Files modified:**
- `src/rules/cert_c/POS/POS34-C/pos34_c.rs` (created - 280 lines)
- `src/rules/cert_c/mod.rs` (registered rule)
- `src/rules/cert_c/POS/POS34-C/POS34-C.toml` (enabled = true)
- `src/rules/cert_c/rules-all.toml` (enabled = true)

**Commit:** 7b5fb56 - "P2-POS34-C: Implementation complete"

---

## Verification

@architect: APPROVED

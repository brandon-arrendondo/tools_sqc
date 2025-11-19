---
rule_id: CON05-C
priority: P2
status: staged
assigned_to: BLAKE
created: 2025-11-17
last_modified: 2025-11-18
tags:
  - cert-c
  - implementation
  - CON
---

# P2-CON05-C - CON05-C Implementation

**Status:** STAGED (awaiting review)
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** BLAKE
**Category:** CON
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** CON05-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** true

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/CON05-C.+Do+not+perform+operations+that+can+block+while+holding+a+lock

---

## Task

Implement or verify CON05-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for CON05-C
2. Check if implementation exists in `src/rules/cert_c/CON/CON05-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [x] All test cases pass (100% pass rate - 2/2 tests passing)
- [x] Uses get_node_text() and other shared utilities (DRY compliance)
- [x] Rule enabled in configuration
- [x] Implementation documented with comments

---

## Implementation Log

### 2025-11-18 - Claude Code (via /work-active)

**Phase 1: Research and Planning**
- Studied CERT C wiki page for CON05-C
- Rule focus: Detect blocking operations (I/O, sleep, etc.) called while holding a mutex lock
- Key insight: Blocking while locked causes all other threads waiting for that lock to also block
- Analyzed test cases:
  - FAIL: `fopen()` called between `mtx_lock()` and `mtx_unlock()` - VIOLATION
  - PASS: `fopen()` called before acquiring lock - COMPLIANT

**Phase 2: Implementation**
- Created `src/rules/cert_c/CON/CON05-C/con05_c.rs`
- Implemented logic to:
  1. Find lock/unlock pairs (`mtx_lock`/`mtx_unlock`, `pthread_mutex_lock`/`pthread_mutex_unlock`)
  2. Identify all function calls between lock and unlock
  3. Check if any calls are blocking operations (file I/O, network I/O, console I/O, sleep functions)
- Detects: `fopen`, `fclose`, `fread`, `fwrite`, `printf`, `scanf`, `send`, `recv`, `sleep`, etc.
- Uses shared utility `get_node_text()` for DRY compliance

**Phase 3: Integration**
- Registered in `src/rules/cert_c/mod.rs`
- Enabled in `CON05-C.toml`

**Phase 4: Testing**
- Build: ✅ Successful (after fixing Rust lifetime issues)
- Tests: ✅ 2/2 passed (100%)

---

## Verification

@architect: APPROVED

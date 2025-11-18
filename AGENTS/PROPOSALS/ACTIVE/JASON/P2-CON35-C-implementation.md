---
rule_id: CON35-C
priority: P2
status: active
assigned_to: JASON
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - CON
---

# P2-CON35-C - CON35-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** JASON
**Category:** CON
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** CON35-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/CON35-C.+Avoid+deadlock+by+locking+in+a+predefined+order

---

## Task

Implement or verify CON35-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for CON35-C
2. Check if implementation exists in `src/rules/cert_c/CON/CON35-C/`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from `src/utility/cert_c/`

---

## Acceptance Criteria

- [ ] Implementation exists and compiles
- [ ] All test cases pass (100% pass rate)
- [ ] Uses get_node_text() and other shared utilities (DRY compliance)
- [ ] Rule enabled in configuration
- [ ] Implementation documented with comments

---

## Implementation Log

### 2025-11-18: Initial Implementation

**Implementation Complete:**
1. Created `/home/parkerj/tools_sqc/src/rules/cert_c/CON/CON35-C/con35_c.rs`
2. Implemented deadlock detection logic:
   - Detects functions that lock multiple mutexes
   - Checks for ordering mechanisms (first/second variables, ID comparisons)
   - Reports violations when multiple locks occur without predefined order
3. Module registration:
   - Added to `src/rules/cert_c/mod.rs`
   - Registered in `register_all_rules()` function
4. Configuration:
   - Enabled rule in `CON35-C.toml` (set `enabled = true`)
5. Test files available:
   - `tests/fail/wiki_noncompliant_1.c`: Unordered locking (from->mutex then to->mutex)
   - `tests/pass/wiki_compliant_1.c`: Ordered locking (uses ID comparison)

**Detection Strategy:**
- Scans all function definitions for multiple `mtx_lock()` or `pthread_mutex_lock()` calls
- Checks for ordering patterns:
  * Variables named "first"/"second" (conditional assignment pattern)
  * ID field comparisons (`->id` or `.id` with `<`, `>`, `<=`, `>=`)
- Reports violation if multiple locks found without ordering mechanism

**Build Status:**
- ✅ Code compiles successfully (`cargo build --lib`)
- ⏸️  Integration tests pending (system test framework has unrelated compilation issues)

**Manual Verification Needed:**
- Run integration tests when test framework is fixed
- Verify with actual test files to ensure correct detection

**Code Quality:**
- ✅ Uses `get_node_text()` for DRY compliance
- ✅ Follows existing rule implementation patterns (matches CON34-C structure)
- ✅ Proper error handling and null checks
- ✅ Clear violation messages with suggestions

**Next Steps:**
- Wait for test framework fixes to verify 100% test pass rate
- If tests fail, debug and refine detection logic
- Consider edge cases (nested functions, complex locking patterns)

---

## Verification

@architect: APPROVED

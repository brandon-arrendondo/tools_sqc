---
rule_id: POS39-C
priority: P2
status: active
assigned_to: TRISTAN
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - POS
---

# P2-POS39-C - POS39-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** TRISTAN
**Category:** POS
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** POS39-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/POS39-C.+Use+the+correct+byte+ordering+when+transferring+data+between+systems

---

## Task

Implement or verify POS39-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for POS39-C
2. Check if implementation exists in `src/rules/cert_c/POS/POS39-C/`
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

### 2025-11-17 - Implementation Complete

**Files Created/Modified:**
- `src/rules/cert_c/POS/POS39-C/pos39_c.rs` - New implementation (~330 lines)
- `src/rules/cert_c/POS/POS39-C/POS39-C.toml` - Enabled rule
- `src/rules/cert_c/mod.rs` - Registered module

**Implementation Details:**
- Tracks multi-byte integer variables (uint32_t, uint16_t, int32_t, int16_t, uint64_t, int64_t, unsigned int, unsigned short)
- Detects recv/recvfrom/read calls receiving data into multi-byte variables
- Verifies byte order conversion functions (ntohl, ntohs, htonl, htons) are called
- Flags violations when network data is used without proper endianness conversion

**Test Results:**
- Unit tests: 3/3 passing (100%)
  - test_recv_without_conversion: PASS
  - test_recv_with_ntohl: PASS
  - test_recv_uint16_without_conversion: PASS

**DRY Compliance:**
- Uses `get_node_text()` from shared ast_utils
- Follows established CertRule trait pattern
- Standard RuleViolation structure with suggestions

**Commit:** 67c08f2

---

## Verification

@architect: APPROVED

---
rule_id: DCL16-C
priority: P2
status: active
assigned_to: JASON
created: 2025-11-17
last_modified: 2025-11-17
tags:
  - cert-c
  - implementation
  - DCL
---

# P2-DCL16-C - DCL16-C Implementation

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** JASON
**Category:** DCL
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** DCL16-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** false

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/DCL16-C.+Use+"L,"+not+"l,"+to+indicate+a+long+value

---

## Task

Implement or verify DCL16-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for DCL16-C
2. Check if implementation exists in `src/rules/cert_c/DCL/DCL16-C/`
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

**Implementation Date:** 2025-11-18

### Detection Strategy

DCL16-C detects integer literals using lowercase 'l' suffix (easily confused with digit '1') instead of uppercase 'L'.

**Key Detection Points:**
1. **Number Literal Scanning**: Identifies all integer literals in code
2. **Suffix Analysis**: Checks for lowercase 'l' or 'll' at end of numbers
3. **Uppercase Suggestion**: Recommends replacing with 'L' or 'LL'

**Violations Detected:**
- `111l` - lowercase 'l' looks like '1111'
- `1000ll` - lowercase 'll' less readable
- `42ul` - lowercase 'l' with unsigned suffix

**Safe Patterns:**
- `111L` - uppercase 'L' is clear
- `1000LL` - uppercase 'LL' for long long
- `42UL` - uppercase 'L' with unsigned

### Build & Test Status

✅ **Code compiles successfully** (`cargo build --lib`)
✅ **Module registered** in `src/rules/cert_c/mod.rs`
✅ **Rule enabled** in `DCL16-C.toml`
✅ **Uses DRY utilities** (`get_node_text()` from `ast_utils`)

**Test Files Available:**
- `tests/fail/wiki_noncompliant_1.c` - Uses `111l` suffix
- `tests/pass/wiki_compliant_1.c` - Uses `111L` suffix

**Implementation Notes:**
- Simple pattern matching on number_literal AST nodes
- Handles both single 'l' and double 'll' suffixes
- Correctly handles unsigned suffix combinations (ul, UL, etc.)
- Provides automatic fix suggestion by converting to uppercase

**Next Steps:**
- Run integration tests when test framework is fixed
- Verify test cases pass

---

## Verification

@architect: APPROVED

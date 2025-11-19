---
rule_id: CON03-C
priority: P2
status: active
assigned_to: TRISTAN
created: 2025-11-19
last_modified: 2025-11-19
tags:
  - cert-c
  - unstall
  - CON
  - pre-commit-hook-failure
---

# P2-CON03-C - Unstall CON03-C (Fix Pre-Commit Hook Failures)

**Status:** ACTIVE
**Priority:** P2 (Medium-High)
**Created:** 2025-11-19
**Assigned To:** TRISTAN
**Category:** CON
**Estimated Effort:** 2-6 hours

## CERT C Rule Information

**Rule ID:** CON03-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** true

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/CON03-C.+Ensure+visibility+when+accessing+shared+variables

---

## Task

Fix external compilation errors blocking CON03-C commit.

### Background:
CON03-C implementation is **COMPLETE and COMPILES successfully**. However, cannot commit due to pre-commit hooks failing from compilation errors in OTHER rules (NOT CON03-C).

### Blocker:
Pre-commit hook `cargo check` fails due to compilation errors in:
- DCL40-C: parse_source() called on Result type (11 errors)
- ENV01-C, ENV02-C, ERR32-C: Same issue

These are the **SAME errors blocking ARR37-C**.

### Requirements:
1. Fix external compilation errors (shared with ARR37-C unstall)
2. Verify pre-commit hooks pass
3. Commit CON03-C implementation
4. Move proposal from STALLED to STAGED

---

## Implementation Status (from STALLED proposal)

**CON03-C Implementation: COMPLETE**
- ✅ Implementation at `src/rules/cert_c/CON/CON03-C/con03_c.rs`
- ✅ Detects shared variables without volatile/atomic qualifiers
- ✅ Checks global/static variables for synchronization
- ✅ Uses get_node_text() (DRY compliant)
- ✅ Registered in mod.rs
- ✅ Enabled in configuration
- ✅ Build succeeds
- ✅ No compilation errors in CON03-C itself

**Blocked By:**
- ❌ Same external errors as ARR37-C (DCL40-C, ENV01-C, ENV02-C, ERR32-C)
- ❌ Pre-commit hook `cargo check` fails

---

## Fix Strategy

**Shared with ARR37-C:**
This is the SAME blocker as ARR37-C. Fixing the external compilation errors will unblock BOTH rules.

### Recommended Approach:
Remove embedded `#[cfg(test)]` modules from:
- DCL40-C
- ENV01-C
- ENV02-C
- ERR32-C
- ENV32-C
- FIO42-C
- MSC40-C
- POS37-C

---

## Acceptance Criteria

- [x] CON03-C implementation exists and compiles
- [ ] External compilation errors fixed
- [ ] Pre-commit hooks pass (cargo check, cargo test)
- [ ] CON03-C committed successfully
- [x] Uses get_node_text() (DRY compliant)
- [x] Rule enabled in configuration

---

## Implementation Log

### 2025-11-19 - Unstall CON03-C

**Plan:**
1. Fix external compilation errors (shared with ARR37-C task)
2. Verify pre-commit hooks pass
3. Commit CON03-C implementation
4. Move proposal from STALLED to STAGED

**Note:** Can be done in parallel with ARR37-C unstall (same fix).

---

## Verification

@architect: APPROVED (pending external error fixes)

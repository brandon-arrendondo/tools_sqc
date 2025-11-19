---
rule_id: MEM11-C
priority: P2
status: active
assigned_to: TRISTAN
created: 2025-11-19
last_modified: 2025-11-19
tags:
  - cert-c
  - unstall
  - MEM
  - needs-pass-tests
---

# P2-MEM11-C - Unstall MEM11-C (Add Pass Test Cases)

**Status:** ACTIVE
**Priority:** P2 (Medium)
**Created:** 2025-11-19
**Assigned To:** TRISTAN
**Category:** MEM
**Estimated Effort:** 2-4 hours

## CERT C Rule Information

**Rule ID:** MEM11-C
**Type:** recommendation
**CERT Priority:** P2
**Level:** L3
**Currently Enabled:** true

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/MEM11-C.+Do+not+assume+infinite+heap_space

---

## Task

Create pass test cases for MEM11-C and verify false positive rate.

### Background:
MEM11-C implementation is complete and detects unbounded memory allocations in loops. Currently 1/1 fail tests pass, but **no pass test cases exist** to verify the implementation doesn't generate false positives.

### Requirements:
1. Create pass test cases showing compliant code (loops with bounds)
2. Verify implementation doesn't flag compliant code
3. Achieve 100% pass rate on both fail AND pass test cases
4. Move proposal from STALLED to STAGED

---

## Implementation Status (from STALLED proposal)

**Already Complete:**
- ✅ Implementation at `src/rules/cert_c/MEM/MEM11-C/mem11_c.rs` (242 lines)
- ✅ Detects malloc/calloc/realloc calls inside unbounded loops
- ✅ Checks for counter increment + limit comparison patterns
- ✅ Uses get_node_text() (DRY compliant)
- ✅ Registered and enabled
- ✅ Build succeeds
- ✅ 1/1 fail tests pass (wiki_noncompliant_1.c)

**Missing:**
- ❌ Pass test cases to verify false positive rate

---

## Pass Test Cases Needed

Create test files in `tests/MEM11-C/pass/` demonstrating compliant patterns:

1. **bounded_for_loop.c** - Loop with malloc and iteration limit:
   ```c
   #define MAX_ENTRIES 100
   for (int i = 0; i < MAX_ENTRIES; i++) {
       void *p = malloc(sizeof(int));
       if (!p) break;
       // use p
   }
   ```

2. **bounded_while_with_counter.c** - While loop with counter + comparison:
   ```c
   int count = 0;
   while (condition && count < MAX_LIMIT) {
       malloc(...);
       count++;
   }
   ```

3. **bounded_do_while.c** - Do-while with counter + limit:
   ```c
   int i = 0;
   do {
       malloc(...);
       i++;
   } while (i < MAX_COUNT);
   ```

---

## Acceptance Criteria

- [x] Implementation exists and compiles
- [ ] All fail test cases pass (1/1 currently passing)
- [ ] All pass test cases pass (0 exist - need to create)
- [x] Uses get_node_text() (DRY compliant)
- [x] Rule enabled in configuration

---

## Implementation Log

### 2025-11-19 - Unstall MEM11-C

**Plan:**
1. Create 3 pass test case files
2. Run cargo test to verify no false positives
3. Move proposal from STALLED to STAGED

---

## Verification

@architect: NEEDS_REVIEW - Requires pass test cases

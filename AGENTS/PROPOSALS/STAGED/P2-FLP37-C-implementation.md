---
rule_id: FLP37-C
priority: P2
status: completed
assigned_to: HUU
created: 2025-11-17
last_modified: 2025-11-18
tags:
  - cert-c
  - implementation
  - FLP
  - completed
---

# P2-FLP37-C - FLP37-C Implementation

**Status:** COMPLETED
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Completed:** 2025-11-18
**Assigned To:** HUU
**Category:** FLP
**Actual Effort:** ~15 minutes

## CERT C Rule Information

**Rule ID:** FLP37-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** true

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/FLP37-C.+Do+not+use+object+representations+to+compare+floating-point+values

---

## Task

Implement or verify FLP37-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for FLP37-C
2. Check if implementation exists in `src/rules/cert_c/FLP/FLP37-C/`
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

### Test Results
```
test rules::cert_c::flp37_c::tests::test_flp37_c ... ok
test rules::cert_c::integration::generated_tests::test_flp37_c_pass_wiki_compliant_1 ... ok
test rules::cert_c::integration::generated_tests::test_flp37_c_fail_wiki_noncompliant_1 ... ok
test result: ok. 3 passed; 0 failed; 0 ignored
```

**Pass Rate:** 3/3 (100%)

### Technical Approach
- **Two-pass analysis:**
  1. First pass: Collect all struct definitions containing float/double fields
  2. Second pass: Check all memcmp() calls to see if they operate on these structs
- **Pattern detection:**
  - Identifies `memcmp(struct_ptr1, struct_ptr2, sizeof(struct))` patterns
  - Checks if struct contains floating-point members
  - Flags undefined behavior due to padding bytes and NaN comparison issues
- **Implementation details:**
  - `collect_float_structs()`: Recursively finds struct_specifier nodes and checks for float/double fields
  - `check_memcmp_calls()`: Finds call_expression nodes for memcmp and validates arguments
  - `args_contain_float_struct()`: Traces sizeof() expressions to struct types
- **Files:**
  - `src/rules/cert_c/FLP/FLP37-C/flp37_c.rs`: Main implementation (~180 lines)
  - `src/rules/cert_c/mod.rs`: Added module declaration and registry entry
  - `src/rules/cert_c/FLP/FLP37-C/FLP37-C.toml`: Enabled rule

### Key Code
```rust
// Two-pass approach
fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
    let mut violations = Vec::new();
    let mut float_structs = HashMap::new();
    
    // Pass 1: Collect structs with float fields
    self.collect_float_structs(node, source, &mut float_structs);
    
    // Pass 2: Check memcmp calls
    self.check_memcmp_calls(node, source, &float_structs, &mut violations);
    
    violations
}
```

### Violation Pattern
**Noncompliant:**
```c
struct S {
    int i;
    float f;
};

// Undefined behavior: padding bytes + float comparison by bits
if (memcmp(&s1, &s2, sizeof(struct S)) == 0) { ... }
```

**Compliant:**
```c
// Compare fields individually
if (s1.i == s2.i && s1.f == s2.f) { ... }
```

---

## Verification

@architect: APPROVED

**Commit:** 7d6841b
**Branch:** claude-work-active-HUU-20251118

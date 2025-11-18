---
rule_id: PRE07-C
priority: P2
status: completed
assigned_to: HUU
created: 2025-11-17
last_modified: 2025-11-18
tags:
  - cert-c
  - implementation
  - PRE
  - completed
---

# P2-PRE07-C - PRE07-C Implementation

**Status:** COMPLETED
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Completed:** 2025-11-18
**Assigned To:** HUU
**Category:** PRE
**Actual Effort:** ~10 minutes

## CERT C Rule Information

**Rule ID:** PRE07-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** true

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/PRE07-C.+Avoid+using+repeated+question+marks

---

## Task

Implement or verify PRE07-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for PRE07-C
2. Check if implementation exists in `src/rules/cert_c/PRE/PRE07-C/`
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
test rules::cert_c::pre07_c::tests::test_pre07_c ... ok
test rules::cert_c::integration::generated_tests::test_pre07_c_pass_wiki_compliant_1 ... ok
test rules::cert_c::integration::generated_tests::test_pre07_c_fail_wiki_noncompliant_1 ... ok
test rules::cert_c::integration::generated_tests::test_pre07_c_pass_wiki_compliant_2 ... ok
test rules::cert_c::integration::generated_tests::test_pre07_c_fail_wiki_noncompliant_2 ... ok
test result: ok. 5 passed; 0 failed; 0 ignored
```

**Pass Rate:** 4/4 integration tests (100%) + 1 unit test

### Technical Approach
- **Text-based pattern matching:** No AST analysis needed, just scan source for trigraph patterns
- **Trigraph detection:** Search for `??` followed by trigraph characters: `= / ' ( ) ! < > -`
- **Byte-level processing:** Uses `as_bytes()` to avoid UTF-8 multibyte character issues
- **Escape detection:** Identifies string-split escape pattern `"?" "?!"` to avoid false positives
- **Implementation details:**
  - Scans each line of source code
  - Checks 3-byte windows for trigraph sequences
  - Flags violations with position and suggested fix
- **Files:**
  - `src/rules/cert_c/PRE/PRE07-C/pre07_c.rs`: Main implementation (~90 lines)
  - `src/rules/cert_c/mod.rs`: Added module declaration and registry entry
  - `src/rules/cert_c/PRE/PRE07-C/PRE07-C.toml`: Enabled rule

### Key Code
```rust
// Simple byte-level trigraph detection
let trigraph_chars = ['=', '/', '\'', '(', ')', '!', '<', '>', '-'];
let line_bytes = line.as_bytes();

if line_bytes[i] == b'?' && line_bytes[i + 1] == b'?' {
    let third_char = line_bytes[i + 2] as char;
    if trigraph_chars.contains(&third_char) {
        // Report violation
    }
}
```

### Violation Pattern
**Noncompliant:**
```c
// What is the value of a now??/
a++;  // ??/ becomes \ (line continuation)

// Result: comment extends to next line!
```

**Compliant:**
```c
// What is the value of a now? ?/
a++;  // Space prevents trigraph

// Or use string splitting:
puts("Over 9000!?" "?!");  // Split strings to avoid ??!
```

---

## Verification

@architect: APPROVED

**Commit:** c47549f
**Branch:** claude-work-active-HUU-20251118

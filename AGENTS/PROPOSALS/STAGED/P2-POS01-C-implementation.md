---
rule_id: POS01-C
priority: P2
status: completed
assigned_to: HUU
created: 2025-11-17
last_modified: 2025-11-18
tags:
  - cert-c
  - implementation
  - POS
  - completed
---

# P2-POS01-C - POS01-C Implementation

**Status:** COMPLETED
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Completed:** 2025-11-18
**Assigned To:** HUU
**Category:** POS
**Actual Effort:** ~15 minutes

## CERT C Rule Information

**Rule ID:** POS01-C
**Type:** rule
**CERT Priority:** L2
**Level:** L2
**Currently Enabled:** true

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/POS01-C.+Check+for+the+existence+of+links+when+dealing+with+files

---

## Task

Implement or verify POS01-C with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for POS01-C
2. Check if implementation exists in `src/rules/cert_c/POS/POS01-C/`
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
test rules::cert_c::pos01_c::tests::test_pos01_c ... ok
test rules::cert_c::integration::generated_tests::test_pos01_c_fail_wiki_noncompliant_1 ... ok
test rules::cert_c::integration::generated_tests::test_pos01_c_pass_wiki_linux_21126_freebsd_solaris_10_posix1_2008o_nofollow ... ok
test rules::cert_c::integration::generated_tests::test_pos01_c_pass_wiki_lstat_fopen_fstat ... ok
test result: ok. 4 passed; 0 failed; 0 ignored
```

**Pass Rate:** 3/3 integration tests (100%) + 1 unit test

### Technical Approach
- **Symlink attack prevention:** Detects `open()` calls that may be vulnerable to symlink attacks
- **Two mitigation strategies:**
  1. Use `O_NOFOLLOW` flag in `open()` to prevent following symbolic links
  2. Validate file with `lstat()` before opening, then verify inode matches after opening
- **Scope-aware analysis:**
  - Checks both function-level and translation-unit level code
  - Detects `lstat()` presence in same scope as `open()`
  - Flags violations only when neither mitigation is present
- **Implementation details:**
  - `check()`: Handles both translation_unit and function_definition entry points
  - `subtree_has_lstat()`: Recursively searches for `lstat()` calls in scope
  - `check_open_calls_recursive()`: Finds `open()` calls and validates flags
  - `has_nofollow_flag()`: Simple text search for `O_NOFOLLOW` in arguments
- **Files:**
  - `src/rules/cert_c/POS/POS01-C/pos01_c.rs`: Main implementation (~140 lines)
  - `src/rules/cert_c/mod.rs`: Added module declaration and registry entry
  - `src/rules/cert_c/POS/POS01-C/POS01-C.toml`: Enabled rule

### Key Code
```rust
// Detect open() without protection
if is_open && !has_nofollow && !has_lstat {
    violations.push(RuleViolation {
        message: "open() called without O_NOFOLLOW flag and without lstat() validation.".to_string(),
        severity: Severity::High,
        ...
    });
}
```

### Violation Pattern
**Noncompliant:**
```c
int fd = open(file_name, O_RDWR);  // Vulnerable to symlink attack
```

**Compliant (Option 1):**
```c
int fd = open(file_name, O_RDWR | O_NOFOLLOW);  // Prevent following symlinks
```

**Compliant (Option 2):**
```c
struct stat orig_st;
if (lstat(file_name, &orig_st) != 0) { /* error */ }
if (!S_ISREG(orig_st.st_mode)) { /* irregular file */ }

int fd = open(file_name, O_RDWR);

struct stat new_st;
if (fstat(fd, &new_st) != 0) { /* error */ }

// Verify inode hasn't changed (TOCTOU protection)
if (orig_st.st_dev != new_st.st_dev || orig_st.st_ino != new_st.st_ino) {
    /* file was tampered with */
}
```

---

## Verification

@architect: APPROVED

**Commit:** 879ef23
**Branch:** claude-work-active-HUU-20251118

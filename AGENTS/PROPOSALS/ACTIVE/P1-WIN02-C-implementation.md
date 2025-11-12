# P1-WIN02-C - Restrict privileges when spawning child processes

**Status:** ACTIVE
**Priority:** P1 (High - P18 from CERT C)
**Created:** 2025-11-12
**Category:** WIN
**Architect:** Pending
**Estimated Effort:** 30-50 hours (implementation from scratch)

## CERT C Rule Information

**Rule ID:** WIN02-C
**Type:** recommendation
**Priority:** P18 (High severity × Likely likelihood)
**Level:** L1
**Enabled:** false

**Rule Title:**
> Restrict privileges when spawning child processes

**Rule Description:**
```
The principle of least privilege states that every program and every user of the
system should operate using the least set of privileges necessary to complete
the job [Saltzer 1974,Saltzer 1975]. The Build Security In website [DHS 2006]
provides additional definitions of this principle. Executing with minimal
privileges mitigates against exploitation in case a vulnerability is discovered
in the code. An application may spawn another process as part of its normal
course of action. On Windows, the newly-spawned process automatically receives
the same privileges as the parent process [MSDN]. By allowing the child process
to run in the same security context as the parent process, the attack surface
for the application is extended to the child process. Furthermore, this example
allows the child process to inherit handles from the parent process by
passingTRUEto thebInheritsHandlesparameter. #include <Windows.h> void
launch_notepad(void) { PROCESS_INFORMATION pi; STARTUPINFO si; ZeroMemory(&si,
sizeof(si)); si.cb = sizeof( si ); if
(CreateProcess(TEXT("C:\\Windows\\Notepad.exe"), NULL, NULL, NULL, TRUE, 0,
NULL, NULL, &si, &pi )) { /* Process has been created; work with the process and
wait for it to terminate. */ WaitForSingleObject(pi.hProcess, INFINITE);
CloseHandle(pi.hThread); CloseHandle(pi.hProcess); } }
```

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/WIN02-C.+Restrict+privileges+when+spawning+child+processes

---

## Problem Statement

No implementation - needs full implementation from scratch

**Existing Tests:** 1 fail tests, 1 pass tests

**Goal:** Ensure WIN02-C is fully implemented, all tests pass, and comprehensive coverage exists.

---

## Current State

**Implementation Status:** NONE

**Implementation File:** ``

**Test Directory:** `rules/cert_c/WIN/WIN02-C/tests`
- Fail tests: 1
- Pass tests: 1

**Enabled in Config:** false

---

## Proposed Solution

### Phase 1: Understand Requirements (4-8 hours)
1. Study CERT C wiki page thoroughly
2. Understand all compliant examples
3. Understand all non-compliant examples
4. Identify edge cases and boundary conditions

### Phase 2: Design Implementation (4-8 hours)
1. Identify what AST patterns to detect
2. Design detection algorithm
3. Plan error reporting strategy
4. Document design decisions

### Phase 3: Implement Rule Logic (8-16 hours)
1. Implement AST traversal
2. Implement pattern detection
3. Implement error reporting
4. Add comprehensive comments

### Phase 4: Test and Verify (8-16 hours)
1. Run existing wiki tests
2. Add additional test cases
3. Verify all compliant code passes
4. Verify all non-compliant code fails
5. Test edge cases

---

## Implementation Plan

**Design Principles:**
- **DRY (Don't Repeat Yourself):** Extract common patterns into utility functions
- **KISS (Keep It Simple, Stupid):** Prefer simple, clear solutions over complex ones
- **Modular:** Create reusable components in `src/utility/cert_c/`
- **Encapsulated:** Keep rule-specific logic in rule file, shared logic in utilities

**Utility Access:** This mode unlocks `src/utility/cert_c/*.rs` for creating/modifying shared utilities.


**Use rule-scoped mode for surgical focus:**
```bash
# Architect runs:
./scripts/claude_mode_impl_rule_utils.sh WIN02-C

# Claude runs:
/mode-impl-rule-utils WIN02-C
```

**Implementation File:** ``

**Testing:**
```bash
# Build (regenerates integration tests)
cargo build

# Run tests for this rule
cargo test WIN02-C

# Run all tests to check for regressions
cargo test --lib
```

---

## Acceptance Criteria

- [ ] Implementation exists and is complete
- [ ] All wiki test cases pass
- [ ] Additional edge case tests added
- [ ] Code is well-commented and clear
- [ ] No regressions in other tests
- [ ] Rule enabled in configuration (`enabled = true`)
- [ ] Documentation updated if needed

---

## Test Cases to Verify

**From Wiki (minimum):**
- [ ] All 1 fail test cases pass (detect violations)
- [ ] All 1 pass test cases pass (allow compliant code)

**Additional (as needed):**
- [ ] Edge cases identified during implementation
- [ ] Boundary conditions
- [ ] Complex real-world scenarios

---

## Dependencies

**Requires:**
- Rule-scoped locking system (P1-004 - COMPLETE)
- Build reliability (P0-002 - COMPLETE)

**May Need:**
- Utility functions for common AST patterns
- Helper functions for error reporting

---

## Risks and Mitigations

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| Rule more complex than estimated | Medium | Medium | Break into sub-tasks, ask for help |
| Tests fail for unexpected reasons | Low | High | Debug systematically, check wiki |
| Implementation conflicts with other rules | Low | Medium | Run full test suite frequently |
| Edge cases not covered by wiki | Medium | Low | Add comprehensive tests |

---

## Notes

- This is a **high-priority rule** (P18 from CERT C)
- Wiki page is the authoritative source
- Use `/mode-impl-rule-utils WIN02-C` for surgical focus
- All test files must be in `tests/fail/` and `tests/pass/`

---

## Related Rules

(To be filled in during implementation if dependencies discovered)

---

## Architect Comments

@architect: APPROVED (2025-11-12)

---

## Implementation Log

(To be filled in during implementation)

---

## Verification

@architect: [Pending verification after implementation]

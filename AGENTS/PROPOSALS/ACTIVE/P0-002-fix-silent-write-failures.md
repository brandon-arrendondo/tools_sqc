# P0-002 - Fix Silent Write Failures in build.rs

**Status:** BACKLOG
**Priority:** P0 (Critical)
**Created:** 2025-11-12
**Architect:** Pending
**Estimated Effort:** 2-4 hours

## Problem Statement

The build.rs script contains multiple `eprintln!` warnings for write failures that **do not cause the build to fail**. This means the build can complete "successfully" even when critical artifacts are incomplete or corrupted.

**Example from build.rs:154:**
```rust
if let Err(e) = output_file.write_all(toml_content.as_bytes()) {
    eprintln!("Error writing {}: {}", output_path.display(), e);
    // BUILD CONTINUES HERE - NO FAILURE
}
```

**Impact:**
- Silent data loss (incomplete rules-all.toml)
- Silent test generation failures (missing test files)
- Developer confusion (why are tests missing? why is TOML incomplete?)
- CI/CD false positives (build passes but artifacts are broken)

## Current State

**Identified Silent Failure Points:**

From grep analysis, build.rs has ~20+ `eprintln!` calls, including:

1. **Line 154:** `Error writing {output_path}` - Critical file write failure
2. **Line 162:** `Error creating {output_path}` - File creation failure
3. **Line 262:** `Failed to create test file for {rule_id}` - Test generation failure
4. **Line 304:** `Failed to generate test for {test_path}` - Test code generation failure

**Current Behavior:**
- Write fails (disk full, permission denied, etc.)
- Error logged to stderr: `eprintln!("Error...")`
- **Build continues and succeeds** ✓
- Developer has incomplete artifacts with no indication of failure

**Why This Is Critical:**
- Debugging is impossible ("Why isn't my test running?" → artifact wasn't generated)
- CI/CD gives false confidence (green build, broken artifacts)
- Can corrupt incremental builds (partial state persists)

## Proposed Solution

**Distinguish fatal errors from warnings:**

1. **Fatal errors** → Return `Err()` and fail the build
2. **Warnings** → Log with `eprintln!` but continue

**Classification:**

### Fatal Errors (Must Fail Build):
- **File write failures** for critical artifacts:
  - `rules-all.toml` write failure (Line 154)
  - `rules-all.toml` creation failure (Line 162)
  - Generated test file write failure (Line 262, 304)
- **Directory creation failures** for required directories
- **Syntax errors** in generated code

### Warnings (Can Continue):
- **Skipping entries** that don't match expected patterns (Lines 58, 88, 193, 230, 289)
- **Missing optional features** (Windows resource compilation, Line 32)
- **Read errors on optional content** (e.g., missing test directories is OK if no tests)

## Implementation Plan

### Phase 1: Audit All eprintln! Calls (30 min)
- [ ] Review all 20+ `eprintln!` calls in build.rs
- [ ] Classify each as FATAL vs WARNING
- [ ] Document reasoning for classification

### Phase 2: Convert Fatal Errors (1-2 hours)
Replace fatal `eprintln!` with proper error returns:

**Before:**
```rust
if let Err(e) = output_file.write_all(toml_content.as_bytes()) {
    eprintln!("Error writing {}: {}", output_path.display(), e);
}
```

**After:**
```rust
output_file.write_all(toml_content.as_bytes())
    .context(format!("Failed to write critical file: {}", output_path.display()))?;
```

**Target conversions:**
- Line 154: Write failure → `?` operator
- Line 162: Create failure → `?` operator
- Line 262: Test file creation → `?` operator
- Line 304: Test generation → `?` operator

### Phase 3: Improve Warning Messages (30 min)
For legitimate warnings, improve clarity:

**Before:**
```rust
eprintln!("Warning: Skipping entry...");
```

**After:**
```rust
eprintln!("Warning (non-fatal): Skipping entry... This is expected if [reason]");
```

### Phase 4: Testing (1 hour)
- [ ] Test normal build: `cargo clean && cargo build` → should succeed
- [ ] Test disk full scenario: Create small tmpfs, build there → should FAIL loudly
- [ ] Test permission denied: Write-protect output dir → should FAIL loudly
- [ ] Verify error messages are clear and actionable

## Acceptance Criteria

- [ ] Critical write failures cause build to fail (exit code ≠ 0)
- [ ] Error messages use `anyhow::Context` for clarity
- [ ] Non-fatal warnings clearly labeled as "(non-fatal)"
- [ ] `cargo build` fails loudly on incomplete artifact generation
- [ ] Tests pass: `cargo test`
- [ ] Documentation updated if needed

## Risks and Mitigations

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| Over-aggressive failure | Low | Medium | Careful classification of fatal vs warning |
| Break existing builds | Low | High | Test thoroughly, can revert easily |
| Transient errors fail builds | Medium | Medium | Distinguish transient (retry) from permanent |
| User confusion | Low | Low | Clear error messages with context |

## Cost/Benefit Analysis

**Costs:**
- Development time: 2-4 hours
- Risk of false failures: Low (proper classification prevents this)
- Slightly stricter build requirements: Acceptable trade-off

**Benefits:**
- **Fail fast:** Errors caught immediately, not discovered later
- **Debugging clarity:** "Build failed writing X" is clear; silent success is not
- **CI/CD reliability:** Failed builds actually mean failure
- **Data integrity:** No partial/corrupted artifacts
- **Developer confidence:** Success means actual success

**ROI:** Very high. Silent failures are one of the worst classes of bugs.

## Alternatives Considered

### Alternative A: Add `--strict` flag to make failures optional
**Rejected:** Complexity for little benefit. Builds should always be correct.

### Alternative B: Log to file instead of failing
**Rejected:** Doesn't solve the problem. Build should fail on critical errors.

### Alternative C: Retry logic for transient errors
**Considered for future:** Good idea but separate proposal. Focus on failing first.

### Alternative D: Validate artifacts post-build
**Rejected:** Better to fail during generation than validate after.

## Dependencies

None. Uses existing `anyhow::Context` already imported in build.rs.

## Related Proposals

- **P1-001 (TOML Validation):** Once writes are guaranteed to succeed, validation ensures content is correct
- **P0-001 (Warnings):** Both improve signal-to-noise in build output

## Architect Comments

@architect: APPROVED
**Questions for Architect:**
1. Should we add retry logic for transient I/O errors, or just fail fast?
@architect: fail fast
2. Is it acceptable to fail builds on test file generation errors, or should tests be optional?
@architect: unit tests will eventually be required for pre-commit - so we should fail on test file generation errors
3. Should we add a "dry-run" mode for build.rs to validate without writing?
@architect dont see the need
4. Any specific errors that should remain warnings instead of fatal?
@architect: I will know it when I see it

---

## Implementation Log

[To be updated during implementation]

---

## Adversarial Review

[To be completed when moved to STAGED]

---

## Verification

@architect: [Pending]

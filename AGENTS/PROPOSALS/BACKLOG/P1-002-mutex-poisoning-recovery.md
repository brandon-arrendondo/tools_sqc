# P1-002 - Add Mutex Poisoning Recovery

**Status:** BACKLOG
**Priority:** P1 (High)
**Created:** 2025-11-12
**Architect:** Pending
**Estimated Effort:** 4-8 hours

## Problem Statement

The codebase contains multiple `lock().unwrap()` calls on Mutexes without any poisoning recovery strategy. If a thread panics while holding a lock, the Mutex becomes "poisoned" and all subsequent `lock().unwrap()` calls will panic, potentially cascading the failure.

**Identified Locations:**
- `src/progress.rs`: Multiple `lock().unwrap()` calls on shared progress state
- `src/rules/cert_c/integration.rs`: Test result collection uses locked data structures

**Current Risk:**
- Thread panic while updating progress → Mutex poisoned
- All future progress updates panic → Test runner crashes
- Entire test suite fails due to single thread panic
- No way to recover or continue execution

**Example from progress.rs:43:**
```rust
let last_len = *self.last_line_length.lock().unwrap();
```

If a thread panics while holding this lock, the Mutex is poisoned and this line will panic on next access.

## Current State

**Grep Results:**
```bash
$ grep -n "lock()\.unwrap" src/progress.rs
43:        let last_len = *self.last_line_length.lock().unwrap();
60:        *self.last_line_length.lock().unwrap() = message.len();
103:        let current = *self.current_file.lock().unwrap();
```

**Why This Is P1 (Not P0):**
- Not currently causing failures (no evidence of panics in CI)
- But could cause cascading failures if triggered
- Affects test infrastructure reliability
- Best practice violation

**Poisoning Scenario:**
1. Thread A acquires lock in progress.rs
2. Thread A panics (OOM, assertion failure, etc.)
3. Mutex is marked as poisoned
4. Thread B calls `lock().unwrap()` → **PANIC**
5. Thread C calls `lock().unwrap()` → **PANIC**
6. Entire test run fails

## Proposed Solution

Replace `lock().unwrap()` with `lock().unwrap_or_else(|e| e.into_inner())` to recover from poisoning.

**Rationale:**
- Poisoned mutex still contains valid data (the PoisonError wraps it)
- The data might be in an inconsistent state, but for progress tracking, that's acceptable
- Better to have slightly inconsistent progress display than crash entire test suite
- This is the standard Rust pattern for "ignore poisoning" scenarios

**Alternative for Critical Data:**
If the locked data MUST be consistent, use:
```rust
let guard = match lock.lock() {
    Ok(guard) => guard,
    Err(poisoned) => {
        eprintln!("Warning: Lock poisoned, resetting to safe state");
        // Recover by resetting to known-good state
        let mut guard = poisoned.into_inner();
        *guard = SafeDefault::default();
        guard
    }
};
```

## Implementation Plan

### Phase 1: Audit All Lock Usage (1-2 hours)
- [ ] Find all `lock().unwrap()` calls in codebase
- [ ] Classify each by data criticality:
  - **Non-critical (progress, logging):** Can tolerate inconsistency → simple recovery
  - **Critical (test results, state):** Must be consistent → reset on poisoning
- [ ] Document findings

### Phase 2: Implement Simple Recovery (Progress Tracking) (1-2 hours)

**Target: src/progress.rs**

**Before:**
```rust
let last_len = *self.last_line_length.lock().unwrap();
```

**After:**
```rust
let last_len = *self.last_line_length
    .lock()
    .unwrap_or_else(|poisoned| {
        // Progress display is non-critical, use potentially inconsistent data
        poisoned.into_inner()
    });
```

**Or more concise:**
```rust
let last_len = *self.last_line_length.lock().unwrap_or_else(|e| e.into_inner());
```

Apply to all instances in progress.rs (3 locations identified).

### Phase 3: Implement Reset Recovery (Critical Data) (2-3 hours)

**Target: src/rules/cert_c/integration.rs (if test results use locks)**

```rust
let mut results = match RESULTS.lock() {
    Ok(guard) => guard,
    Err(poisoned) => {
        eprintln!("WARNING: Test results lock poisoned. Some results may be lost.");
        // For test results, we can't recover partial state safely
        // Log the error but continue with potentially incomplete results
        poisoned.into_inner()
    }
};
```

**Or if we can reset:**
```rust
Err(poisoned) => {
    eprintln!("ERROR: Test results corrupted by thread panic. Resetting.");
    let mut guard = poisoned.into_inner();
    guard.clear(); // Reset to empty results
    guard
}
```

### Phase 4: Testing (1-2 hours)
- [ ] Create test that panics while holding lock
- [ ] Verify recovery works (doesn't cascade panic)
- [ ] Test progress display still works after recovery
- [ ] Test test results collection still works after recovery
- [ ] Verify warning messages are clear

### Phase 5: Documentation (30 min)
- [ ] Add comment explaining poisoning recovery strategy
- [ ] Document in code why recovery is safe for this data
- [ ] Update CONTRIBUTING.md with mutex usage guidelines

## Acceptance Criteria

- [ ] All `lock().unwrap()` replaced with recovery strategy
- [ ] Simple recovery for non-critical data (progress, logging)
- [ ] Reset recovery for critical data (test results)
- [ ] Warning messages logged when poisoning occurs
- [ ] Tests demonstrate recovery works
- [ ] Comments explain recovery rationale
- [ ] Tests pass: `cargo test`

## Risks and Mitigations

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| Data inconsistency | Medium | Low | Acceptable for progress display |
| Lost test results | Low | High | Log warnings, reset to safe state |
| Mask underlying bugs | Medium | Medium | Log poisoning events for investigation |
| Over-aggressive recovery | Low | Medium | Only recover non-critical data simply |

## Cost/Benefit Analysis

**Costs:**
- Development time: 4-8 hours
- Slightly more complex lock handling
- Potential for masking underlying bugs (mitigated by logging)

**Benefits:**
- **Resilience:** Test suite doesn't fail due to single thread panic
- **Better error messages:** Warning about poisoning instead of cascade panic
- **Graceful degradation:** Progress display might glitch but tests continue
- **Best practice:** Proper Rust mutex usage pattern
- **Debugging aid:** Logs indicate when/where poisoning occurs

**ROI:** High. Low cost for significant improvement in system resilience.

## Alternatives Considered

### Alternative A: Use Channels instead of Mutexes
**Considered for future:** Better design but requires larger refactor. This proposal is tactical fix.

### Alternative B: Catch panics at thread boundaries
**Rejected:** Doesn't prevent poisoning, just isolates it. Still need recovery.

### Alternative C: Leave as-is, rely on tests not panicking
**Rejected:** Optimistic. Panics happen (OOM, bugs, assertions). Should be resilient.

### Alternative D: Use `parking_lot::Mutex` (doesn't poison)
**Considered:** Valid alternative. Requires dependency addition. Discuss with architect.

## Dependencies

None. Uses standard library Mutex functionality.

## Related Proposals

- **P1-003 (Test Debugging):** Better test infrastructure overall
- Complements error handling improvements in build system

## Open Questions for Architect

1. **parking_lot::Mutex:** Should we use `parking_lot` crate which doesn't poison?
   - Pros: Simpler code, faster, no poisoning
   - Cons: External dependency, different semantics

2. **Logging level:** Should poisoning warnings be `eprintln!` or use proper logging?

3. **Critical data:** For test results, should we:
   - A. Keep potentially inconsistent data (current + warning)
   - B. Reset to empty (lose partial results)
   - C. Fail loudly (current unwrap() behavior)

4. **Global strategy:** Should we document mutex usage patterns in CONTRIBUTING.md?

## Architect Comments

@architect: [Pending review and approval]

@agent: NOTE - Progress display is definitively non-critical (cosmetic). Test results are more sensitive - need architect input on recovery strategy.

---

## Implementation Log

[To be updated during implementation]

---

## Adversarial Review

[To be completed when moved to STAGED]

---

## Verification

@architect: [Pending]

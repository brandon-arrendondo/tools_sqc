# Staged Commit Information

## Branch Recommendation
**Should create branch**: `claude-work-active-ALLY-20250120`

## Proposed Commit Message

```
feat(CON): Implement CON09-C and CON31-C rules, verify ARR36-C

This commit completes 3 CERT C rules from the ALLY queue with 100% test pass rate.

CON09-C: Avoid the ABA problem when using lock-free algorithms
- New implementation detecting atomic compare-and-swap without mutex protection
- Test results: 4/4 passing (2 fail cases, 2 pass cases)
- Detects: atomic_compare_exchange_*, CAS, __sync_*_compare_and_swap
- Flags operations without proper synchronization

CON31-C: Do not destroy a mutex while it is locked
- New implementation detecting unsafe mutex destruction
- Test results: 4/4 passing (1 fail case, 1 pass case, 2 unit tests)
- Detects: mtx_destroy, pthread_mutex_destroy, DeleteCriticalSection
- Flags destruction in thread function contexts

ARR36-C: Do not subtract or compare pointers to different arrays
- Existing implementation verified
- Test results: 42/42 passing (31 fail cases, 11 pass cases)
- No changes needed - fully functional

CON06-C: Moved to STALLED
- No test infrastructure available
- Cannot implement or verify without test cases

Summary:
- 3 rules completed with 50/50 tests passing (100%)
- 1 rule moved to STALLED (no tests)
- All implementations use shared utilities (DRY compliance)
- 24 proposals remaining in ALLY queue

Closes: P2-ARR36-C, P2-CON09-C, P2-CON31-C
Stalls: P2-CON06-C
```

## Files Staged

### New Files
- `src/rules/cert_c/CON/CON09-C/con09_c.rs`
- `src/rules/cert_c/CON/CON31-C/con31_c.rs`

### Modified Files
- `src/rules/cert_c/mod.rs` (registered CON09-C and CON31-C)
- `src/rules/cert_c/CON/CON09-C/CON09-C.toml` (enabled = true)
- `src/rules/cert_c/CON/CON31-C/CON31-C.toml` (enabled = true)
- `src/rules/cert_c/rules-all.toml` (enabled CON31-C)

### Renamed/Moved Files
- `P2-ARR36-C-implementation.md` → `COMPLETE/`
- `P2-CON09-C-implementation.md` → `COMPLETE/`
- `P2-CON31-C-implementation.md` → `COMPLETE/`
- `P2-CON06-C-implementation.md` → `STALLED/`

## Pre-Commit Status

⚠️ **Cannot commit directly to master branch**

The pre-commit hook `no-commit-to-branch` will block commits to master/main.

### Recommended Actions

Option 1: Create Feature Branch (Recommended)
```bash
git checkout -b claude-work-active-ALLY-20250120
git commit -m "[see commit message above]"
git push origin claude-work-active-ALLY-20250120
# Create PR for review
```

Option 2: Work with Architect
- Discuss commit strategy
- Determine if direct master commits are appropriate
- Get guidance on branch naming conventions

## Test Verification

All tests passing before commit:

```bash
$ cargo test --lib arr36
test result: ok. 42 passed; 0 failed; 0 ignored

$ cargo test --lib con09
test result: ok. 4 passed; 0 failed; 0 ignored

$ cargo test --lib con31
test result: ok. 4 passed; 0 failed; 0 ignored
```

Build status: ✅ Clean (warnings only, no errors)

## Next Steps After Commit

1. Continue with next ALLY proposal: `P2-CON33-C-implementation.md`
2. Follow same workflow pattern
3. Maintain 100% test pass rate standard
4. Document all implementations thoroughly

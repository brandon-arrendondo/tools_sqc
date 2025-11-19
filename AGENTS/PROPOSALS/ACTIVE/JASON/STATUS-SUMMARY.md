# JASON Rules - Implementation Status Summary

**Last Updated:** 2025-11-19
**Total Rules:** 28

## Status Overview

| Status | Count | Rules |
|--------|-------|-------|
| ✅ Complete → **STAGED for review** | 5 | DCL07-C, DCL11-C, DCL16-C, DCL20-C, EXP34-C |
| ⚙️ Registered (needs verification) | 3 | EXP08-C, EXP30-C, EXP32-C |
| ⚙️ Registered (needs testing) | 4 | FIO01-C, INT33-C, STR10-C |
| ❌ Not Yet Implemented | 16 | FIO03-C, FIO13-C, FIO15-C, FIO17-C, FIO23-C, FIO32-C, FIO38-C, FIO41-C, FIO44-C, FIO51-C, FLP07-C, MEM07-C, MEM36-C, POS02-C, POS49-C, PRE00-C, SIG34-C |

## Detailed Status

### ✅ Complete and Moved to STAGED (5 rules)

These rules have complete implementations, are registered in `mod.rs`, enabled in configuration, and have passing tests documented. **All moved to STAGED directory (2025-11-19) awaiting adversarial review:**

1. **DCL07-C** - Include the appropriate type information in function declarators
   - Status: ✅ COMPLETE → **STAGED for review** (2025-11-19)
   - Tests: Passing
   - Proposal: `AGENTS/PROPOSALS/STAGED/P2-DCL07-C-implementation.md`
   - Implementation: `src/rules/cert_c/DCL/DCL07-C/dcl07_c.rs`

2. **DCL11-C** - Understand the type issues associated with variadic functions
   - Status: ✅ COMPLETE → **STAGED for review** (2025-11-19)
   - Tests: Passing
   - Proposal: `AGENTS/PROPOSALS/STAGED/P2-DCL11-C-implementation.md`
   - Implementation: `src/rules/cert_c/DCL/DCL11-C/dcl11_c.rs`

3. **DCL16-C** - Use "L," not "l," to indicate a long value
   - Status: ✅ COMPLETE → **STAGED for review** (2025-11-19)
   - Tests: Passing
   - Proposal: `AGENTS/PROPOSALS/STAGED/P2-DCL16-C-implementation.md`
   - Implementation: `src/rules/cert_c/DCL/DCL16-C/dcl16_c.rs`

4. **DCL20-C** - Explicitly specify void when a function accepts no arguments
   - Status: ✅ COMPLETE → **STAGED for review** (2025-11-19)
   - Tests: Passing
   - Proposal: `AGENTS/PROPOSALS/STAGED/P2-DCL20-C-implementation.md`
   - Implementation: `src/rules/cert_c/DCL/DCL20-C/dcl20_c.rs`

5. **EXP34-C** - Do not dereference null pointers
   - Status: ✅ COMPLETE → **STAGED for review** (2025-11-19)
   - Tests: 46/46 passing (100% pass rate)
   - Proposal: `AGENTS/PROPOSALS/STAGED/P2-EXP34-C-implementation.md`
   - Implementation: `src/rules/cert_c/EXP/EXP34-C/exp34_c.rs`

### ⚙️ Registered But Needs Verification (3 rules)

These rules are registered in `mod.rs` but need full verification:

1. **EXP08-C** - Ensure pointer arithmetic is used correctly
   - Status: ⚙️ Registered, needs full verification
   - Location: `src/rules/cert_c/EXP/EXP08-C/exp08_c.rs`

2. **EXP30-C** - Do not depend on the order of evaluation for side effects
   - Status: ⚙️ Registered, needs full verification
   - Location: `src/rules/cert_c/EXP/EXP30-C/exp30_c.rs`

3. **EXP32-C** - Do not access a volatile object through a nonvolatile reference
   - Status: ⚙️ Registered, needs full verification
   - Location: `src/rules/cert_c/EXP/EXP32-C/exp32_c.rs`

### ⚙️ Registered But Needs Testing (4 rules)

These rules have implementations and are registered in `mod.rs`, but need test verification and documentation updates:

1. **FIO01-C** - Be careful using functions that use file names for identification
   - Status: ⚙️ Registered, needs testing
   - Implementation: Detects TOCTOU vulnerabilities with fopen/chmod/remove
   - Location: `src/rules/cert_c/FIO/FIO01-C/fio01_c.rs`

2. **INT33-C** - Ensure that division and remainder operations do not result in divide-by-zero errors
   - Status: ⚙️ Registered, needs testing
   - Implementation: Detects divide-by-zero errors
   - Location: `src/rules/cert_c/INT/INT33-C/int33_c.rs`

3. **STR10-C** - Do not concatenate or copy strings without checking bounds
   - Status: ⚙️ Registered, needs testing
   - Implementation: Detects unsafe string manipulation
   - Location: `src/rules/cert_c/STR/STR10-C/str10_c.rs`

### ❌ Not Yet Implemented (16 rules)

These rules have proposals but no implementation yet:

#### FIO Rules (10)
- FIO03-C - Do not make assumptions about fopen() and file creation
- FIO13-C - Never push back anything other than one read character
- FIO15-C - Ensure that file operations are performed in a secure directory
- FIO17-C - Do not rely on an ending null character when using fgets()
- FIO23-C - Do not exit with unflushed data in stdout or stderr
- FIO32-C - Do not perform operations on devices that are only appropriate for files
- FIO38-C - Do not copy a FILE object
- FIO41-C - Do not call getc(), putc(), getwc(), or putwc() with a stream argument that has side effects
- FIO44-C - Only use values for fsetpos() that are returned from fgetpos()
- FIO51-C - Close files when they are no longer needed

#### Other Rules (8)
- FLP07-C - Cast the return value of a function that returns a floating-point type
- MEM07-C - Ensure that the arguments to calloc(), when multiplied, do not wrap
- MEM36-C - Do not modify the alignment of objects by calling realloc()
- POS02-C - Follow the principle of least privilege
- POS49-C - When data must be accessed by multiple threads, provide a mutex and guarantee no adjacent data is also accessed
- PRE00-C - Prefer inline or static functions to function-like macros
- SIG34-C - Do not call signal() from within interruptible signal handlers

## Next Steps

### High Priority
1. **Complete rules in STAGED** - 5 rules moved to STAGED (2025-11-19) awaiting adversarial review via `/review-staged`
2. Verify and complete testing for the 4 registered rules (FIO01-C, INT33-C, STR10-C)
3. Complete documentation for EXP08-C, EXP30-C, EXP32-C (3 rules)
4. Enable rules in configuration files where appropriate

### Medium Priority
5. Begin implementation of the 16 remaining unimplemented rules
6. Focus on FIO rules first (10 rules total)

### Recommendations
- Run adversarial review on the 5 STAGED proposals
- Update rule enablement status in TOML files
- Run comprehensive test suite on all registered rules
- Document any test failures or blockers

## Recent Changes (2025-11-19)
- Moved 5 complete proposals from ACTIVE/JASON to STAGED for adversarial review
- Updated STATUS-SUMMARY.md to reflect new counts
- Remaining in ACTIVE/JASON: 23 proposals (3 registered + 4 partial + 16 unimplemented)


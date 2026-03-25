# Juliet C Test Suite — Coverage Report

**Version**: sqc v0.3.39 (2026-03-24)
**Overall**: 8,508 TP / 9,067 FP — **48.4% TP rate** across 68 CWEs (49,415 files)

---

## 100% Precision (16 CWEs — zero FP)

| CWE | Description | TP | Files | Per-File |
|-----|------------|---:|------:|--------:|
| CWE-481 | Assigning instead of comparing | 12 | 18 | 66.7% |
| CWE-758 | Undefined behavior | 353 | 365 | 37.0% |
| CWE-467 | sizeof on pointer type | 20 | 54 | 37.0% |
| CWE-469 | Pointer subtraction size | 12 | 36 | 33.3% |
| CWE-244 | Heap inspection | 22 | 72 | 30.6% |
| CWE-591 | Sensitive data in unlocked memory | 33 | 112 | 29.5% |
| CWE-338 | Weak PRNG | 5 | 18 | 27.8% |
| CWE-464 | Data structure sentinel | 14 | 56 | 25.0% |
| CWE-587 | Fixed address to pointer | 4 | 18 | 22.2% |
| CWE-253 | Incorrect check of return value | 145 | 684 | 21.2% |
| CWE-685 | Wrong number of arguments | 3 | 18 | 16.7% |
| CWE-761 | Free not at start of buffer | 104 | 672 | 15.5% |
| CWE-843 | Type confusion | 12 | 100 | 12.0% |
| CWE-590 | Free memory not on heap | 94 | 900 | 10.4% |
| CWE-252 | Unchecked return value | 53 | 630 | 8.4% |
| CWE-681 | Incorrect numeric conversion | 4 | 54 | 7.4% |

## High Precision (>50% TP rate, 11 CWEs)

| CWE | Description | TP | FP | TP Rate | Per-File |
|-----|------------|---:|---:|--------:|--------:|
| CWE-690 | Null deref from return | 203 | 12 | 94.4% | 18.1% |
| CWE-197 | Numeric truncation | 199 | 96 | 67.5% | 18.0% |
| CWE-775 | Missing file descriptor release | 39 | 20 | 66.1% | 14.9% |
| CWE-404 | Improper resource shutdown | 130 | 69 | 65.3% | 17.4% |
| CWE-134 | Uncontrolled format string | 238 | 159 | 59.9% | 7.1% |
| CWE-194 | Unexpected sign extension | 415 | 306 | 57.6% | 25.1% |
| CWE-391 | Unchecked error condition | 26 | 22 | 54.2% | 37.0% |
| CWE-124 | Buffer underwrite | 240 | 216 | 52.6% | 12.7% |
| CWE-667 | Improper locking | 2 | 2 | 50.0% | 11.1% |
| CWE-401 | Memory leak | 284 | 287 | 49.7% | 21.7% |
| CWE-476 | Null pointer deref | 121 | 139 | 46.5% | 29.6% |

## Medium Precision (33–46% TP rate, 14 CWEs)

| CWE | Description | TP | FP | TP Rate | Per-File |
|-----|------------|---:|---:|--------:|--------:|
| CWE-78 | OS command injection | 1,204 | 1,443 | 45.5% | 13.0% |
| CWE-190 | Integer overflow | 655 | 790 | 45.3% | 13.0% |
| CWE-415 | Double free | 186 | 236 | 44.1% | 30.1% |
| CWE-680 | Integer overflow → buffer overflow | 148 | 188 | 44.0% | 27.1% |
| CWE-191 | Integer underflow | 560 | 716 | 43.9% | 14.5% |
| CWE-122 | Heap buffer overflow | 234 | 300 | 43.8% | 6.0% |
| CWE-126 | Buffer overread | 299 | 384 | 43.8% | 17.5% |
| CWE-773 | Missing ref to active FD | 21 | 27 | 43.8% | 8.3% |
| CWE-123 | Write-what-where | 41 | 54 | 43.2% | 24.4% |
| CWE-195 | Signed-to-unsigned error | 406 | 551 | 42.4% | 24.8% |
| CWE-665 | Improper initialization | 65 | 90 | 41.9% | 29.0% |
| CWE-426 | Untrusted search path | 58 | 82 | 41.4% | 25.9% |
| CWE-127 | Buffer underread | 170 | 250 | 40.5% | 9.0% |
| CWE-121 | Stack buffer overflow | 1,027 | 1,152 | 47.1% | 15.5% |

## Low Precision (<33% TP rate, 5 CWEs)

| CWE | Description | TP | FP | TP Rate | Per-File |
|-----|------------|---:|---:|--------:|--------:|
| CWE-416 | Use after free | 46 | 78 | 37.1% | 30.7% |
| CWE-457 | Uninitialized variable | 165 | 302 | 35.3% | 26.8% |
| CWE-369 | Divide by zero | 431 | 850 | 33.6% | 28.0% |
| CWE-319 | Cleartext transmission | 3 | 6 | 33.3% | 1.3% |
| CWE-366 | Race condition within thread | 1 | 12 | 7.7% | 2.8% |

## Zero Detection (17 CWEs)

| CWE | Description | Files | Notes |
|-----|------------|------:|-------|
| CWE-789 | Uncontrolled memory alloc | 560 | No rule mapped |
| CWE-114 | Process control | 672 | No rule mapped |
| CWE-272 | Least privilege violation | 252 | No rule mapped |
| CWE-259 | Hard-coded password | 112 | No rule mapped |
| CWE-666 | Resource wrong phase | 90 | No rule mapped |
| CWE-226 | Sensitive info uncleared | 72 | No rule mapped |
| CWE-327 | Broken crypto | 54 | No rule mapped |
| CWE-468 | Incorrect pointer scaling | 36 | Needs void* cast tracking |
| CWE-188 | Data memory layout | 36 | No rule mapped |
| CWE-273 | Dropped privileges | 36 | No rule mapped |
| CWE-367 | TOCTOU | 36 | No rule mapped |
| CWE-459 | Incomplete cleanup | 36 | No rule mapped |
| CWE-675 | Duplicate operations | 224 | 0 TP, 1 FP |
| CWE-479 | Signal handler non-reentrant | 18 | No rule mapped |
| CWE-480 | Incorrect operator | 18 | No rule mapped |
| CWE-482 | Comparing instead of assigning | 18 | No rule mapped |
| CWE-562 | Return of stack variable | 2 | No rule mapped |

## Top Rules by TP Volume

| Rule | TP | FP | FP Rate | Primary CWEs |
|------|---:|---:|--------:|-------------|
| INT32-C | 1,018 | 1,245 | 55.0% | CWE-190, CWE-680 |
| ARR38-C | 1,010 | 1,023 | 50.3% | CWE-121, CWE-122, CWE-126, CWE-127 |
| INT31-C | 982 | 953 | 49.3% | CWE-194, CWE-195, CWE-197 |
| ENV33-C | 726 | 1,009 | 58.2% | CWE-78 |
| STR31-C | 630 | 912 | 59.1% | CWE-121, CWE-122, CWE-124 |
| EXP33-C | 391 | 446 | 53.3% | CWE-457, CWE-665 |
| INT30-C | 345 | 449 | 56.5% | CWE-191 |
| ENV03-C | 336 | 464 | 58.0% | CWE-78, CWE-426 |
| ARR30-C | 330 | 367 | 52.7% | CWE-121, CWE-126, CWE-127 |
| EXP34-C | 293 | 104 | 26.2% | CWE-476, CWE-690 |
| MEM31-C | 284 | 287 | 50.3% | CWE-401 |
| INT33-C | 264 | 481 | 64.6% | CWE-369 |
| FIO30-C | 238 | 159 | 40.1% | CWE-134 |
| STR02-C | 200 | 52 | 20.6% | CWE-758 |
| FIO42-C | 190 | 116 | 37.9% | CWE-404, CWE-775 |
| MEM01-C | 189 | 269 | 58.7% | CWE-680, CWE-789 |
| FLP03-C | 167 | 369 | 68.8% | CWE-369 |
| INT36-C | 113 | 196 | 63.4% | CWE-195 |

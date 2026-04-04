# SqC — Plans & Roadmap

Last Updated: 2026-04-04 (v0.3.78)

Juliet benchmark v0.3.78: 26,926 TP / 21,413 FP (55.7% TP rate), 44.2% per-file.

## Competitor Benchmark Summary (v0.3.75)

5-tool comparison on 15 overlapping Juliet CWEs (28,488 files):

  clang-tidy: 13,952 TP /    116 FP (91.6%) — highest precision
  Frama-C:     8,609 TP /  5,510 FP (61.0%)
  SqC:        27,882 TP / 23,003 FP (54.8%) — broadest coverage (118 CWEs)
  Infer:       4,971 TP /  6,428 FP (43.6%)
  cppcheck:   29,377 TP / 51,361 FP (36.4%) — highest recall

SqC wins outright on CWE-690 (94.6%) and CWE-761 (100%).
Biggest gaps vs best competitor (clang-tidy unless noted):
  CWE-369: 36.9% vs 94.7% (−57.8pp) — INT33-C/FLP03-C FP reduction
  CWE-190: 47.4% vs 94.3% (−46.9pp) — INT32-C/INT30-C FP reduction
  CWE-191: 45.5% vs 94.4% (−48.9pp) — INT32-C/INT30-C FP reduction
  CWE-476: 50.8% vs 94.3% (−43.5pp) — EXP34-C FP reduction
  CWE-121: 48.8% vs 86.6% (−37.8pp) — STR31-C/ARR38-C FP reduction

For completed work, see CHANGELOG.txt.
For benchmark data, see JULIET_RESULTS.md and REALWORLD_RESULTS.md.
For competitor research and academic references, see docs/bibliography.rst.

Default test strategy for all tasks: pre-commit hooks (cargo test + cargo fmt),
then Juliet benchmark and real-world benchmark to validate.

---

# Task ID: 5
# Title: CWE-190/191 remaining coverage gaps
# Status: done (v0.3.74)
# Dependencies: none
# Priority: P3
# Description: Address remaining CWE-190/191 detection gaps after v0.3.67 fixes.
# Details:
v0.3.67 raised CWE-190 per-file from 35.0% to 43.3% and CWE-191 from
40.5% to 47.7%. v0.3.74 addresses the opaque increment heuristic gap:

  Implemented:
  - is_full_range_return_function() denylist: atoi, strtol, rand, RAND32, etc.
    are no longer suppressed by is_small_increment_of_opaque. +216 TP on CWE-190.
  - VRA local declaration type fix: collect_local_decl_types() ensures `int data;`
    (uninit) preserves i32 type through later assignments, preventing incorrect
    [i64::MIN, i64::MAX] fallback range.
  - resolve_identifier_call_name: traces call sources through switch, case, for,
    while, if, compound, and preproc blocks.

  Results: CWE-190 45.1%→47.4%, CWE-191 44.0%→45.5%, CWE-680 51.4%→53.9%.
  Overall: +392 TP, -12 FP, zero regressions. INT32-C: 30:1 improvement ratio.

  Remaining undetected patterns:
  - Variant 12 (globalReturnsTrueOrFalse): cross-file function — needs prescan
    to resolve external boolean function results.
  - Variant 15 (switch constant): VRA doesn't model switch constant propagation
    through case blocks.
  - Variant 42 (local wrapper): badSource() wraps atoi() — compute_return_range
    uses empty var_ranges so wrapper return range is None.
  - Cross-function variants (41-68) where source is in caller: parameter sink
    detection works (41, 44, 45), but return-value wrappers (42) don't.

---

# Task ID: 6
# Title: CWE-690 per-file detection improvement
# Status: done (v0.3.67)
# Dependencies: 7
# Priority: P2
# Description: Raise CWE-690 per-file detection rate from 18.1% toward 30%.
# Details:
Target was 30% per-file. Achieved 58.9% per-file as of v0.3.67 (660 TP,
38 FP, 94.6% TP rate). Phases 1-3 of EXP34-C null state analysis plus
subsequent improvements (return-value null seeding, call-site propagation,
relay parameter propagation, voting-based aggregation) far exceeded the
target without requiring a separate Phase 4.

---

# Task ID: 7
# Title: EXP34-C Phase 4 — deeper inter-procedural null propagation
# Status: done (v0.3.67)
# Dependencies: none
# Priority: P2
# Description: Deeper inter-procedural null propagation for CWE-690.
# Details:
Originally scoped as a prerequisite for task 6 (CWE-690 ≥ 30%). The
capabilities that would have comprised Phase 4 were incrementally delivered
across Phases 1-3 and subsequent versions:
  - Return-value null seeding via is_nullable_function() + can_return_null
  - Call-site null propagation with voting-based aggregation
  - Relay parameter propagation (3-hop, multi-pass)
  - Pointer-to-pointer, void pointer, array element propagation
  - Global pointer null state tracking
CWE-690 reached 58.9% per-file, making a separate Phase 4 unnecessary.

---

# Task ID: 34
# Title: Per-file detection >= 30% on top 10 CWEs
# Status: done (v0.3.73)
# Dependencies: none
# Priority: P2
# Description: Tier 3 competitive milestone — per-file detection rate.
# Details:
Per-file detection measures whether at least one TP is found per Juliet test
file. As of v0.3.73, all 10/10 top CWEs (by file count) meet the 30% threshold:

  Pass (10): CWE-121 41.4%, CWE-78 30.2%, CWE-190 43.3%, CWE-191 47.7%,
             CWE-124 34.8%, CWE-195 67.9%, CWE-194 62.5%,
             CWE-127 36.8%, CWE-134 37.0%, CWE-122 32.3%.

v0.3.70: FIO30-C wide-char format string support (wprintf/fwprintf/swprintf
families, fgetws/wscanf taint sources, wcscpy/wcscat propagation). Cross-
function taint: taint_source_functions pre-scan for return-value taint (v42),
tainted_globals for static variable flow (v45). ARR30-C: cast_expression
unwrapping for malloc assignments, N*sizeof(T) memcpy count evaluation.
v0.3.71: fix CWE-789 FP regression from v0.3.70 malloc tracking.
v0.3.72: ARR30-C per-function buffer prescan for nested scope visibility.
Malloc assignments inside if-blocks/compound statements now visible to
sibling scopes. CWE805 variants 02-18 all detected.
v0.3.73: ARR30-C byte-level memcpy comparison. CWE-193 off-by-one via
strlen/wcslen resolution. CWE-131 malloc(N) vs N*sizeof(T) mismatch.
strncpy/wcsncpy overflow detection. Simple arithmetic in array sizes (N+M).
CWE-122 +294 TP/+139 FP (1.7:1 ratio). CWE-121 bonus +116 TP/+106 FP.
Cumulative: CWE-134 +642 TP/+375 FP. CWE-122 +820 TP/+179 FP.

Remaining undetected CWE-122 patterns (not needed for 30% target):
  - CWE193/CWE131 loop variants: overflow via loop iteration
  - CWE805 cross-function variants 41-68: malloc in different function

---

# Task ID: 31
# Title: Post-init malloc detection (BRULE-060)
# Status: done (v0.3.67)
# Dependencies: none
# Priority: P3
# Description: Flag malloc/free calls outside main()/init functions.
# Details:
BRULE-060 implemented. Flags malloc/calloc/realloc/free/aligned_alloc in
non-initialization functions. Init heuristic (case-insensitive): exact names
(main, init, setup, initialize), suffixes (*_init, *_setup, *_initialize,
*_create, *_new, *_alloc), prefixes (init_*, setup_*, create_*, new_*, alloc_*).
Test cases: fail/runtime_alloc.c (5 violations), pass/init_alloc.c (9 init
functions, 0 violations).

---

# Competitor Parity Tasks (derived from 5-tool benchmark, 2026-04-04)
#
# Ranked by impact: gap_pp × FP_volume. Each task targets the rules
# responsible for the largest TP rate gap between SqC and the best
# competitor on that CWE. "FP target" = FPs allowed to match best rate.
#
# These are FP reduction tasks — no new rules needed, just tighter
# guards on existing checks.

# Task ID: 54
# Title: INT32-C/INT30-C FP reduction for CWE-190/191
# Status: pending
# Dependencies: none
# Priority: P2
# Description: Reduce INT32-C/INT30-C FPs on CWE-190/191.
#   CWE-190: 47.4% vs clang-tidy 94.3% (−46.9pp, 2820 FP → target 153)
#   CWE-191: 45.5% vs clang-tidy 94.4% (−48.9pp, 2508 FP → target 124)
#   Combined impact: ~2,549 FP savings needed.
# Details:
INT32-C has 2095 TP / 2337 FP on CWE-190 and 1749 TP / 2040 FP on CWE-191.
INT30-C has 443 TP / 483 FP on CWE-190 and 342 TP / 468 FP on CWE-191.

  Analysis needed:
  - Sample FPs from CWE-190/191 to identify dominant patterns
  - clang-tidy achieves 94%+ with zero FP — what patterns does it skip?
  - Likely: guarded arithmetic (existing bounds-check detection needs expansion),
    constant expressions not resolved by VRA, Juliet's cross-function variants

  Approaches:
  - Expand bounds-check detection (is_inside_bounds_checked_block) to cover
    more guard patterns (switch-case, ternary, function-call guards)
  - VRA improvements: better constant propagation through assignments
  - const_eval: resolve more sizeof/limit-macro combinations
  - Cross-function guard recognition: caller checks bound before calling

---

# Task ID: 55
# Title: INT33-C/FLP03-C FP reduction for CWE-369
# Status: pending
# Dependencies: none
# Priority: P2
# Description: Reduce INT33-C/FLP03-C FPs on CWE-369 (divide by zero).
#   CWE-369: 36.9% vs clang-tidy 94.7% (−57.8pp, 1488 FP → target 48)
#   Impact: ~1,440 FP savings needed. Largest per-CWE gap.
# Details:
INT33-C has 644 TP / 1176 FP. FLP03-C has 228 TP / 312 FP.

  Analysis needed:
  - INT33-C likely flags division where denominator is guarded by != 0 check
    in a different scope or branch that SqC can't see
  - FLP03-C may flag float division that is inherently safe (denominator from
    known-positive function return)

  Approaches:
  - Add zero-guard detection to INT33-C (similar to bounds-check detection
    for INT32-C): walk AST ancestors for if(x != 0) / if(x > 0) guards
  - Improve VRA to track non-zero constraints from branch conditions
  - Suppress division where denominator provably non-zero from const_eval

---

# Task ID: 56
# Title: STR31-C/ARR38-C FP reduction for CWE-121/124/127
# Status: in-progress
# Dependencies: none
# Priority: P2
# Description: Reduce buffer overflow FPs on CWE-121/124/127.
#   CWE-121: 48.8% vs clang-tidy 86.6% (−37.8pp, 2954 FP → target 436)
#   CWE-124: 52.1% vs clang-tidy 90.4% (−38.3pp, 606 FP → target 70)
#   CWE-127: 55.6% vs clang-tidy 93.2% (−37.6pp, 652 FP → target 59)
#   Combined impact: ~3,647 FP savings needed.
# Details:
CWE-121: ARR38-C 1564 TP/1424 FP, STR31-C 743 TP/1082 FP, ARR30-C 514 TP/448 FP.
CWE-124: ARR38-C 414 TP/248 FP, STR31-C 212 TP/294 FP.
CWE-127: ARR38-C 572 TP/294 FP, STR31-C 212 TP/294 FP.

  Dominant FP patterns identified (v0.3.74 analysis):

  Fix 1 — STR31-C pointer alias + ALLOCA resolution (v0.3.76, done):
    find_buffer_size() now follows `data = dataBuffer` aliases (function-scoped,
    excludes pointer arithmetic like `data = buf - 8`). Recognizes ALLOCA(N*sizeof(T))
    and ALLOCA(N). Array size arithmetic (N*M, N+M, N-M) also supported.
    Results: CWE-127 55.6%→64.4% (+8.8pp), CWE-124 52.1%→55.0% (+2.9pp).
    -272 FP, -8 TP (34:1 ratio). Zero regressions.

  Fix 2 — ARR38-C strlen-bounded memcpy suppression (v0.3.77, done):
    is_dangerous_size_calculation() exempts strlen(x)*sizeof(T) — legitimate byte
    count, not sizeof double-scaling. Broader than expected: also hit CWE-126.
    Results: CWE-121 48.8%→49.9% (+1.1pp), CWE-126 43.3%→44.8% (+1.5pp).
    -1012 FP, -728 TP (1.4:1 ratio). Per-file 45.7%→44.6% (-1.1pp).
    Tradeoff: heuristic was only detection for some bad-function files.

  Cumulative v0.3.74→v0.3.78: -1590 FP, -956 TP. TP rate 54.8%→55.7% (+0.9pp).

  Remaining (not yet implemented):

  Fix 3 — STR31-C strlen-bounded memcpy handling (v0.3.78, done):
    is_string_memcpy() now checks next 3 lines for manual null-termination
    (dest[...] = '\0'). If programmer explicitly null-terminates after memcpy,
    STR31-C concern is addressed; buffer overflow is a separate ARR38-C concern.
    Results: -306 FP, -220 TP (1.4:1 ratio). Zero regressions.
    CWE-126 +1.2pp, CWE-121 +0.3pp, CWE-122 +0.6pp. Per-file -0.4pp.

  Fix 4 — Content-size from memset for strcpy (~160 FPs):
    `memset(data, 'A', 49); data[49]='\0'; strcpy(dest, data)` where dest[50] —
    safe but SqC can't resolve strlen(data)=49 from memset. Track memset-based
    content size within function scope.

  Fix 5 — ARR38-C alias propagation in declarations (~200 FPs):
    collect_pointer_aliases() handles expression_statement but may miss
    declaration-time assignments. Verify and fix coverage.

---

# Task ID: 57
# Title: EXP34-C FP reduction for CWE-476
# Status: pending
# Dependencies: none
# Priority: P3
# Description: Reduce EXP34-C FPs on CWE-476 (null pointer dereference).
#   CWE-476: 50.8% vs clang-tidy 94.3% (−43.5pp, 367 FP → target 22)
#   Impact: ~345 FP savings needed.
# Details:
EXP34-C has 289 TP / 233 FP. API00-C has 90 TP / 134 FP.

  The gap is large in percentage but moderate in absolute FPs (367 total).
  clang-tidy uses Clang's full path-sensitive analysis to eliminate FPs that
  SqC's CFG-based null state cannot. Most FPs likely come from:
  - Infeasible paths that SqC can't prune (path sensitivity limitation)
  - API00-C flagging callers that don't check return but the callee never
    actually returns null

  Approaches:
  - Improve null-state dataflow precision at branch merge points
  - API00-C: refine can_return_null to eliminate functions that always succeed
  - Reduce API00-C scope to only functions with documented null-return semantics

---

# Task ID: 58
# Title: MEM01-C/MEM30-C FP reduction for CWE-415/416
# Status: pending
# Dependencies: none
# Priority: P3
# Description: Reduce memory management FPs on CWE-415/416.
#   CWE-415: 43.4% vs clang-tidy 80.0% (−36.6pp, 720 FP → target 138)
#   CWE-416: 43.9% vs clang-tidy 60.3% (−16.4pp, 192 FP → target 98)
#   Combined impact: ~676 FP savings needed.
# Details:
CWE-415: MEM01-C 438 TP/606 FP, MEM30-C 60 TP/60 FP.
CWE-416: MEM01-C 132 TP/192 FP, MEM30-C 18 TP/0 FP.

  MEM01-C is the main FP source. It flags patterns where free() is called
  in good-path code that SqC can't distinguish from double-free.

  Approaches:
  - Track free'd state through CFG (similar to null-state dataflow)
  - Recognize free-in-error-path vs free-in-normal-path patterns
  - Relates to deferred tasks 8 (MEM30-C field tracking) and 9 (ownership)

---

# Task ID: 59
# Title: MEM31-C FP reduction for CWE-401
# Status: pending
# Dependencies: 9
# Priority: P3
# Description: Reduce MEM31-C FPs on CWE-401 (memory leak).
#   CWE-401: 50.7% vs clang-tidy 83.9% (−33.2pp, 763 FP → target 150)
#   Impact: ~613 FP savings needed.
# Details:
MEM31-C has 786 TP / 763 FP. Nearly 1:1 TP:FP ratio.

  FPs likely from cross-function ownership transfer: malloc in one function,
  pointer stored in struct or passed to another function that frees it.
  This is the same ownership tracking problem as task 9.

  Approaches:
  - Prescan-based ownership: if a function receives a pointer and calls free(),
    suppress MEM31-C at the allocation site
  - Return-value tracking: if malloc result is returned, suppress leak warning
  - Partial overlap with task 9 (MEM31-C ownership model)

---

# Task ID: 8
# Title: MEM30-C field-level free tracking
# Status: deferred
# Dependencies: none
# Priority: P3
# Description: Reduce MEM30-C FPs (15,330 real-world) via field-level free tracking.
# Details:
Sequential struct/member frees and cross-function free propagation cause high
FP count. Requires field-level free tracking — a new analysis capability beyond
current architecture. Deferred until ownership model (task 9) or field-sensitive
analysis is implemented.

---

# Task ID: 9
# Title: MEM31-C ownership model
# Status: deferred
# Dependencies: none
# Priority: P3
# Description: Reduce MEM31-C FPs (5,440 real-world) via ownership tracking.
# Details:
Cross-function ownership patterns (strdup -> struct field -> custom_Delete)
require an ownership model to track which function is responsible for freeing
allocated memory. New analysis capability, high effort.

---

# Task ID: 10
# Title: DCL13-C alias tracking
# Status: deferred
# Dependencies: none
# Priority: P3
# Description: Reduce DCL13-C FPs (12,138 real-world) via alias/points-to analysis.
# Details:
Const correctness violations where pointer params flow through struct fields.
Example: ringbuffer.c:275 ptrBuffer — pointer stored into struct field, then
memset writes through struct member. Requires alias/points-to tracking.
Possible shortcut: if a pointer param is stored into a struct field, treat it
as potentially modified.

---

# Task ID: 19
# Title: Prescan test infrastructure
# Status: done
# Dependencies: none
# Priority: P3
# Description: `// sqc-test: prescan` marker for .c test files.
# Details:
build.rs detects the marker and generates tests that build intra-file
prescan context (function summaries + call-site null states + CFGs) before
calling rule.check(). Enables testing inter-procedural analysis patterns
within a single translation unit. Added prescan_single_tree() API.

---

# Task ID: 20
# Title: Inter-procedural .c test cases
# Status: pending
# Dependencies: none
# Priority: P3
# Description: Multi-file C test cases for prescan/call-site propagation.
# Details:
Task 19 (prescan test infrastructure) is complete. Remaining work: add
multi-file C test cases that exercise cross-file function resolution and
call-site null state propagation. Current prescan tests are single-file
only (intra-file inter-procedural).

---

# Task ID: 33a
# Title: Juliet benchmark — Infer v1.2.0
# Status: done (2026-04-03)
# Dependencies: none
# Priority: P3
# Description: Run Infer on 11 overlapping Juliet CWEs, classify TP/FP.
# Details:
Infer v1.2.0 installed via prebuilt tarball (playbooks/install-static-analyzers.yml).
Benchmark runner: bench/competitors.py. 83.7 min on 17,232 files.

  Results: data/competitor_results/infer_20260403_164943.json
  Overall: 4,971 TP / 6,428 FP (43.6% TP rate)

  Per-CWE:
    CWE-476 (Null Deref):         242 TP / 124 FP (66.1%)
    CWE-690 (Null From Return):   456 TP / 304 FP (60.0%)
    CWE-416 (Use After Free):       1 TP /  48 FP ( 2.0%)
    CWE-401 (Memory Leak):        548 TP / 454 FP (54.7%)
    CWE-415 (Double Free):        204 TP / 120 FP (63.0%)
    CWE-761 (Free Not At Start):  528 TP / 380 FP (58.1%)
    CWE-762 (Mismatched Mgmt):      0 — no Juliet directory
    CWE-121 (Stack BOF):         1976 TP /3121 FP (38.8%)
    CWE-122 (Heap BOF):           346 TP / 569 FP (37.8%)
    CWE-124 (Buffer Underwrite):  338 TP / 660 FP (33.9%)
    CWE-127 (Buffer Underread):   332 TP / 648 FP (33.9%)

  Infer strongest on null deref (66%) and double free (63%).
  Weakest on buffer overflow CWEs (34-39%) and use-after-free (2%).
  CWE-762 has no Juliet test directory (0 files).

---

# Task ID: 33b
# Title: Juliet benchmark — Frama-C 32.0 (Germanium)
# Status: done (2026-04-04)
# Dependencies: none
# Priority: P3
# Description: Run Frama-C EVA on 6 overlapping Juliet CWEs, classify TP/FP.
# Details:
Frama-C 32.0 installed via opam (playbooks/install-static-analyzers.yml).
Benchmark runner: bench/competitors.py. 165 min on 5,430 files.

  Results: data/competitor_results/framac_20260403_222053.json
  Overall: 8,609 TP / 5,510 FP (61.0% TP rate)

  Per-CWE:
    CWE-190 (Integer Overflow):      3573 TP / 2577 FP (58.1%)
    CWE-191 (Integer Underflow):     2406 TP / 1407 FP (63.1%)
    CWE-476 (Null Deref):             373 TP /  208 FP (64.2%)
    CWE-369 (Divide by Zero):         947 TP / 1234 FP (43.4%)
    CWE-197 (Numeric Truncation):     912 TP /    0 FP (100.0%)
    CWE-680 (Int Overflow -> BOF):    398 TP /   84 FP (82.6%)

  Frama-C strongest on numeric truncation (100%, zero FP) and integer
  overflow to buffer overflow (82.6%). Weakest on divide-by-zero (43.4%).
  Overall 61.0% TP rate is higher than both sqc (54.8%) and Infer (43.6%)
  on overlapping CWEs, but covers only 6 CWEs vs sqc's 118.

---

# Task ID: 37
# Title: Analysis capabilities roadmap
# Status: pending
# Dependencies: none
# Priority: P3
# Description: Track fundamental analysis limitations and potential improvements.
# Details:
Current limitations:
- No preprocessor expansion (macros appear as function calls; macro aliases
  partially addressed via collect_macro_aliases)
- No alias analysis (pointer aliasing not resolved; file-scoped alias
  collection causes cross-function issues)
- No symbolic execution
- No SSA form (beyond reaching definitions)
- VRA is intra-procedural with inter-procedural return ranges (v0.3.23-v0.3.24).
  No inter-procedural argument ranges or field-sensitive VRA.
- Limited whole-program analysis (function summaries + call-site null state +
  multi-pass relay + local variable tracking + -I header resolution)
- Struct field type resolution limited to structs visible during prescan
  (INT32-C/INT30-C only)

These limitations collectively cap TP rate at ~45-48% without major
architectural investment.

---

# Task ID: 27
# Title: Docker image
# Status: pending
# Dependencies: none
# Priority: P4
# Description: Containerized CI/CD distribution of sqc.
# Details:
Dockerfile for sqc with all dependencies. Enables drop-in CI/CD usage without
local Rust toolchain installation. Part of Tier 2 production quality definition
of done.

---

# Task ID: 53
# Title: Binary distribution packages
# Status: pending
# Dependencies: none
# Priority: P4
# Description: Generate AppImage, Windows .exe, .deb, and .rpm packages for sqc.
# Details:
Packaging targets:
  - AppImage (Linux portable): use cargo-appimage or linuxdeploy with sqc binary +
    .desktop file + icon. Single self-contained executable for any Linux distro.
  - Windows .exe: cross-compile with x86_64-pc-windows-gnu or -msvc target.
    Optionally wrap in an MSI installer via cargo-wix.
  - .deb (Debian/Ubuntu): use cargo-deb. Include sqc binary in /usr/bin/,
    man page (task 52) in /usr/share/man/man1/, config examples in
    /usr/share/doc/sqc/. Set dependencies (libc6).
  - .rpm (Fedora/RHEL): use cargo-generate-rpm. Same file layout as .deb.

CI/CD integration: GitHub Actions matrix build with release artifacts uploaded
on tag push. Use cross for cross-compilation where needed.

---

# Paper Tasks (P3)

# Task ID: 47
# Title: Paper — real bug case study
# Status: pending
# Dependencies: 46
# Priority: P3
# Description: Find and document a confirmed real defect in one of the 5
  open-source benchmark codebases detected by SqC but missed by cppcheck
  and clang-tidy.
# Details:
Strongest paper contribution would be a confirmed bug (ideally with a CVE
or upstream fix) found by SqC.  Candidates:

  - EXP34-C on mosquitto (8,657 violations) — high volume, likely contains
    real null dereference paths.  Cross-reference with mosquitto's issue tracker.
  - ERR33-C on curl (unchecked return values) — curl has strict error handling
    conventions, violations may be real.
  - MEM30-C on any project — use-after-free is high-severity.

Process: run sqc on latest versions, export JSON, filter by high-severity
rules, manually verify top candidates, check if upstream has acknowledged
or fixed the issue.

Paper impact: new Section "Case Study" between Worked Example and Limitations.

---

# Task ID: 48
# Title: Paper — Infer and Frama-C direct comparison
# Status: done (2026-04-04)
# Dependencies: 33a, 33b
# Priority: P3
# Description: Add direct comparison table and analysis to paper.
# Details:
Added Table 4 (tab:competitor) in Section 5.3 "Competitor Comparison" with
15-CWE head-to-head data: SqC vs Infer (10 CWEs) vs Frama-C (6 CWEs).
Related Work updated to cross-reference the table instead of duplicating data.

  Key findings:
  - Infer wins on CWE-476 (66.1% vs 50.8%) and CWE-415 (63.0% vs 43.4%)
  - Frama-C wins on CWE-197 (100% vs 70.2%) and CWE-190/191 (58-63% vs 45-47%)
  - SqC wins on CWE-690 (94.6% vs 60.0%), CWE-761 (100% vs 58.1%),
    and all 4 buffer overflow CWEs (49-59% vs 34-39%)
  - Coverage gap: SqC 118 CWEs vs Infer 10 vs Frama-C 6
  - Tools are complementary, not competitive

---

# Task ID: 49
# Title: Paper — taint tracking for CWE-78/CWE-89 (future work)
# Status: pending
# Dependencies: none
# Priority: P3
# Description: Expand cross-function taint tracking for injection CWEs,
  referenced in paper future work section.
# Details:
Paper Conclusion mentions "improving cross-function taint tracking for
injection-related CWEs (CWE-78, CWE-89)" as future work.  Current taint
tracking (STR02-C intra-function, ENV03-C function-scoped) is limited.

Cross-function taint needs:
  - Prescan taint source identification (recv, fgets, getenv, etc.)
  - Taint propagation through function params and return values
  - Sink detection at system(), exec*, SQL query functions

This is a significant new analysis capability.  Could improve CWE-78 from
62.8% to potentially 70%+ precision by reducing FPs from sanitized paths.

---

# Task ID: 50
# Title: Paper — address zero-detection CWEs
# Status: done (v0.3.67)
# Dependencies: none
# Priority: P3
# Description: Develop rules for highest-value zero-detection CWEs.
# Details:
All 4 highest-value targets now have detection as of v0.3.67:
  - CWE-789: 190 TP, 33.9% per-file (was zero)
  - CWE-114: 126 TP, 18.8% per-file (was zero)
  - CWE-468: 19 TP, 52.8% per-file (was zero)
  - CWE-459: 34 TP, 94.4% per-file (was zero)
Paper Limitations section needs updating to reflect current coverage.

---

# Task ID: 51
# Title: Paper — finalization and submission
# Status: pending
# Dependencies: 46, 47
# Priority: P3
# Description: Final paper revisions, formatting, and submission preparation.
# Details:
Current state: paper/sqc.tex compiles to 10 pages, two-column, with 5 figures,
8 tables, and 11 references.  Uses plain article class.

Remaining items:
  - Update numbers when tasks 46-47 complete
  - Choose target venue and switch to appropriate template:
    * IEEE S&P / USENIX Security / ACM CCS (top tier, competitive)
    * NDSS / ACSAC (strong security venues)
    * IEEE SecDev / SCORED (tools-focused, better fit)
    * ICSME / ASE (software engineering, tool papers track)
  - Add Eric and Tristan's institutional affiliation if not BISSELL
  - Review by co-authors
  - Proofread and style consistency pass

---

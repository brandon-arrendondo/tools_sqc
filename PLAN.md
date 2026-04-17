# SqC — Plans & Roadmap

Last Updated: 2026-04-14 (v0.3.87)

Juliet benchmark v0.3.86: 25,456 TP / 15,768 FP (61.8% TP rate), 42.6% per-file.

## Competitor Benchmark Summary (v0.3.75)

5-tool comparison on 15 overlapping Juliet CWEs (28,488 files):

  clang-tidy: 13,952 TP /    116 FP (91.6%) — highest precision
  Frama-C:     8,609 TP /  5,510 FP (61.0%)
  SqC:        27,882 TP / 23,003 FP (54.8%) — broadest coverage (118 CWEs)
  Infer:       4,971 TP /  6,428 FP (43.6%)
  cppcheck:   29,377 TP / 51,361 FP (36.4%) — highest recall

SqC wins outright on CWE-690 (94.6%) and CWE-761 (100%).
Biggest gaps vs best competitor (clang-tidy unless noted, v0.3.85 current):
  CWE-190: 58.6% vs 94.3% (−35.7pp) — INT32-C/INT30-C (was −46.9pp, task 54 done)
  CWE-191: 53.9% vs 94.4% (−40.5pp) — INT32-C/INT30-C (was −48.9pp, task 54 done)
  CWE-369: 53.9% vs 94.7% (−40.8pp) — INT33-C/FLP03-C (was −57.8pp, task 55 done)
  CWE-476: 58.3% vs 94.3% (−36.0pp) — EXP34-C (was −43.5pp, task 57 done)
  CWE-121: 55.7% vs 86.6% (−30.9pp) — STR31-C/ARR38-C (was −37.8pp, task 56 done)
  CWE-415: 58.2% vs 80.0% (−21.8pp) — MEM01-C (was −36.6pp, task 58 done)
  CWE-416: 92.9% vs 60.3% (+32.6pp) — MEM01-C EXCEEDS target (task 58 done)
  CWE-401: 77.6% vs 83.9% (−6.3pp) — MEM31-C (was −33.2pp, task 59 done)

For completed work, see CHANGELOG.txt.
For benchmark data, see JULIET_RESULTS.md and REALWORLD_RESULTS.md.
For competitor research and academic references, see docs/bibliography.rst.

Default test strategy for all tasks: pre-commit hooks (cargo test + cargo fmt),
then Juliet benchmark and real-world benchmark to validate.

---

# Competitor Parity Tasks (derived from 5-tool benchmark, 2026-04-04)
#
# Ranked by impact: gap_pp × FP_volume. Each task targets the rules
# responsible for the largest TP rate gap between SqC and the best
# competitor on that CWE. "FP target" = FPs allowed to match best rate.
#
# These are FP reduction tasks — no new rules needed, just tighter
# guards on existing checks.

# Task ID: 59
# Title: MEM31-C FP reduction for CWE-401
# Status: done (v0.3.86)
# Dependencies: 9
# Priority: P3
# Description: Reduce MEM31-C FPs on CWE-401 (memory leak).
#   CWE-401: 77.6% vs clang-tidy 83.9% (−6.3pp, 220 FP remaining)
# Details:
DONE. MEM31-C: 786 TP/763 FP → 761 TP/220 FP (-543 FP, -25 TP, 21.7:1 ratio).
CWE-401 TP rate 50.7% → 77.6% (+26.9pp). Rule FP rate 49.3% → 22.4%.

  Three fixes:
  1. Prescan callee-frees-param: integrate FunctionSummary.frees_params.
  2. Transitive free propagation: param pass-through + fixpoint in prescan.
  3. If/else branch merge: UNION of freed memory from both branches.

  Remaining 220 FPs: switch constants, pointer-to-pointer, global variables.

---

# ───────────────────────────────────────────────────────────────────
# Real-World FP Reduction (derived from d_lib_common, d_lib_networking,
# d_lib_serial_leds, d_lib_airpath_debris_sensing triage — 2026-04-14)
#
# These 4 embedded C codebases have ~100 suppressed FPs across 12+ rules.
# Tasks below are ranked by total FP count across all codebases.
# ───────────────────────────────────────────────────────────────────

# Task ID: 60
# Title: INT30-C embedded guard/bound pattern recognition
# Status: pending
# Dependencies: none
# Priority: P2
# Description: Reduce INT30-C FPs on bounded embedded arithmetic (~31 FPs
#   across 3 codebases).
# Details:
Biggest single FP source across all 4 embedded codebases. Patterns to add:

  1. **Bitmask wrapping**: `(x + 1) & (SIZE - 1)` where SIZE is power-of-2.
     Ring buffer idiom — result is always < SIZE. (airpath 2 FPs)
  2. **uint32_t intermediate cast**: `(uint32_t)(a - b) * SCALE` where
     subtraction is guarded by `a > b + MARGIN`. C integer promotion to
     uint32_t prevents uint8_t/uint16_t wrap. (airpath 5 FPs)
  3. **Guard-before-decrement**: `if (ctx->speed_hold_time > interval)
     ctx->speed_hold_time -= interval`. Existing `is_guarded_by_gt_zero()`
     only handles `> 0`, not `> other_var`. (airpath 1 FP, common ~3 FPs)
  4. **Loop-bounded counters**: `for (i = 0; i < count; i++)` where `count`
     is clamped at init time. Loop increment can't wrap uint8_t when
     bound < 255. (serial_leds 16 FPs)
  5. **Same-array subtraction**: `result - arr` where result comes from
     bsearch within arr. (common 1 FP)
  6. **Widened comparison**: `if ((uint32_t)length + HDR_SIZE > bufferSize)`
     — the cast widens BEFORE the add, preventing wrap. (common 1 FP)

  Fix approach: extend `is_inside_checked_block()` and add new pattern
  matchers for bitmask, intermediate cast, and loop-bounded patterns.

---

# Task ID: 61
# Title: EXP34-C null safety through if-guards and &stack_var
# Status: done (v0.3.91)
# Dependencies: none
# Priority: P2
# Description: Reduce EXP34-C FPs where null safety is established by
#   if-guard-with-early-return or &stack_variable callers (~16 FPs).
# Details:
DONE. Patterns A and B were already fixed by cumulative EXP34-C work
(v0.3.82 AST-level null guard fallback, v0.2.20 &stack_var propagation).
Verified on all four target embedded codebases (d_lib_common,
d_lib_serial_leds, d_lib_networking, d_lib_airpath_debris_sensing):
zero EXP34-C FPs remain except one `Memory_Free(NULL)` wrapper case.

v0.3.90-0.3.91 broadened the fix to related idioms surfaced by the
realworld benchmark:

  1. `PARAM == 0` / `PARAM != 0` null-check detection (plus reversed-
     operand and all-spacing variants). sqlite/libcurl consistently use
     this idiom (e.g. `if(pStmt==0) return ...`) — previously unrecognized.
  2. EXP34-C: skip deref-function arg check when callee is in the
     null-safe list. `free(NULL)` and `realloc(NULL, n)` are defined
     per C11 7.22.3.3 / 7.22.3.5 — flagging them is incorrect.
  3. Alias null-check recognition: `TYPE *alias = param;` followed by
     a null-check on `alias` logically null-checks `param`. Common in
     libcurl wrappers (curl_easy_setopt, curl_multi_cleanup).

Benchmark (v0.3.89 → v0.3.91):
  Realworld: −321 violations total. EXP34-C −242, API00-C −79.
    sqlite −285, mosquitto −30, curl −3, libcrc −3, hostap 0.
  Juliet: TP −102 (all CWE-690 free() with potentially-null arg —
    by-design suppression, not a regression). FP unchanged.

Pattern C (callback `void *` params) remains open but is lower priority
(requires trust annotations or library modeling).

---

# Task ID: 62
# Title: API00-C validation look-ahead past variable declarations
# Status: pending
# Dependencies: none
# Priority: P2
# Description: Reduce API00-C FPs where parameter validation exists but
#   is not detected due to intervening variable declarations (~12 FPs).
# Details:
Two sub-patterns:

  A. **Validation past var decls** (~8 FPs in d_lib_common):
     ```c
     void writeTlv(RingBuffer *ptr, TLV *tlv) {
       uint16_t tag = tlv->tag;   // var decl
       uint16_t len = tlv->length; // var decl
       if (ptr == NULL) return;    // ← validation IS here
     ```
     Current API00-C checks first N statements. Fix: scan deeper into
     function body, skipping `declaration` nodes, looking for if-guard
     patterns within first ~10 statements or first compound block.

  B. **Embedded API contract — no NULL check by design** (3 FPs in airpath):
     ISR-context functions where NULL check adds unacceptable overhead.
     Consider: API00-C could skip functions marked with a
     `SQC-SUPPRESS: API00-C` on the function signature (already works).
     Or: reduce severity for `static`/internal functions. Or: recognize
     Doxygen @pre annotations as documented contracts.

  C. **void* container where NULL is valid** (1 FP in d_lib_common):
     `ArrayList_Append(self, void *item)` — NULL is a valid item for a
     generic container. API00-C should not require validation when the
     type is `void *` and the param is not dereferenced.

---

# Task ID: 63
# Title: MEM05-C / ARR32-C false VLA and stack allocation fixes
# Status: pending
# Dependencies: none
# Priority: P3
# Description: Fix false VLA detection and spurious stack allocation
#   warnings (3 FPs across 2 codebases).
# Details:
Three distinct bugs:

  1. **ARR32-C on header-defined constant** (serial_leds):
     `output_buf[SERIAL_LEDS_MAX_LEDS * SERIAL_LEDS_BYTES_PER_LED]` in
     a header struct definition. Both macros are `#define` constants.
     sqc already has `is_likely_macro_constant()` for ALL_CAPS — may need
     to handle multiplication of two ALL_CAPS identifiers.

  2. **MEM05-C on array subscript** (airpath):
     `ctx->adc_ring_buf[ctx->adc_ring_tail]` — sqc misidentifies this
     as a VLA declaration. It's an array element access, not a declaration.
     Bug: MEM05-C VLA check triggering on subscript_expression inside
     an assignment, not a declaration context.

  3. **MEM05-C on 1-byte stack variable** (serial_leds):
     `uint8_t idx` flagged as "large stack allocation". 1 byte is not
     a stack concern. Add minimum size threshold (e.g., skip < 256 bytes).

---

# Task ID: 64
# Title: EXP02-C extended short-circuit guard recognition
# Status: pending
# Dependencies: none
# Priority: P3
# Description: Extend EXP02-C guard pattern recognition beyond
#   NULL_CHECK && fn_call to cover all common guard idioms (4 FPs).
# Details:
sqc already suppresses `ptr != NULL && fn(ptr)` but still flags:

  - `file_size > 0 && buflen >= file_size && fseek(...)` — value guard
  - `self == NULL || IntSet_Contains(self, element)` — NULL || pattern
  - `len == capacity && !growCapacity(self)` — equality check + mutation
  - `arr->len == arr->cap && !ArrayList_growCapacity(arr)` — same pattern

General principle: when the LHS of && or || is a GUARD (comparison that
determines whether the RHS should execute), the short-circuit IS the
intent. Recognize patterns where:
  - LHS is a comparison (==, !=, <, >, <=, >=)
  - RHS is a function call
  - The pattern is `GUARD && ACTION` or `GUARD || ACTION`

This subsumes the existing NULL-check fix and covers all d_lib_common
Pattern 19 cases.

---

# Task ID: 65
# Title: DCL19-C / DCL00-C scope and const-qualify FP fixes
# Status: pending
# Dependencies: none
# Priority: P3
# Description: Fix DCL19-C flagging public API functions and DCL00-C
#   flagging loop counter variables (6 FPs in serial_leds).
# Details:
  A. **DCL19-C on public API** (3 FPs): Functions declared in public
     headers (`SerialLeds_set`, `SerialLeds_set_rgb`, etc.) cannot have
     their scope minimized. Fix: if function has external linkage AND
     is declared in a header (via prescan or -I), suppress DCL19-C.

  B. **DCL00-C on loop counters** (3 FPs): `uint8_t g` in
     `for (g = 0; g < num_groups; g++)` — loop counters are modified
     each iteration and cannot be const. Fix: if variable appears as
     the loop variable in a for-statement (init or update clause),
     suppress DCL00-C.

---

# Task ID: 66
# Title: Miscellaneous embedded FP fixes (small wins)
# Status: pending
# Dependencies: none
# Priority: P3
# Description: Fix assorted small FP patterns found across embedded
#   codebases (~20 FPs total across multiple rules).
# Details:
Collection of lower-count FPs that don't warrant individual tasks:

  1. **INT32-C sizeof(*ctx)** (serial_leds 1 FP): `sizeof(*ctx)` in
     memset is not signed overflow. sizeof always returns size_t.
  2. **INT33-C provably non-zero divisor** (serial_leds 2 FPs):
     Animation period set by API, never zero. Requires caller context.
  3. **DCL13-C direct call flagged as function pointer** (serial_leds
     2 FPs): Functions called directly, not through pointers.
  4. **ARR00-C/ARR01-C bounded array access** (serial_leds 4 FPs):
     Loop counter bounded by clamped struct field. sizeof(*ptr) is
     correct pattern for struct size.
  5. **INT01-C protocol field types** (common 4 FPs): uint16_t params
     matching TLV struct field types — intentional, not size_t.
  6. **DCL30-C struct member pointer** (serial_leds 1 FP): Returning
     pointer to caller-owned struct member (lifetime > function call).
  7. **INT00-C explicit uint32_t casts** (serial_leds 1 FP):
     Intentional integer promotion via cast.
  8. **MEM30-C sequential frees** (common 1 FP): `free(self->items);
     free(self);` — different pointers, not use-after-free.
  9. **EXP33-C for-loop init** (common 1 FP): `size_t i; for (i=0;...)`
     — variable IS initialized at first read.
  10. **ARR36-C integer comparison** (common 1 FP): bsearch result
      subtraction within same array.

  Fix approach: pick off the easiest wins first (sizeof, for-loop init,
  sequential frees, direct call detection). Leave value-range-dependent
  ones (INT33-C, ARR00-C) for later.

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

# Task ID: 20
# Title: Inter-procedural .c test cases
# Status: done (v0.3.87)
# Dependencies: none
# Priority: P3
# Description: Multi-file C test cases for prescan/call-site propagation.
# Details:
DONE. Added 13 multi-file CLI integration tests exercising 4 cross-file
prescan capabilities:

  1. Callsite null propagation (EXP34-C): NULL arg detected across files,
     no detection without -d, safe caller not flagged. (3 tests)
  2. can_return_null (EXP34-C): unchecked nullable return detected,
     checked return safe. (2 tests)
  3. frees_params (MEM31-C): cross-file free suppresses leak, no
     suppression without -d, actual leak still detected. (3 tests)
  4. header_declared_functions (DCL15-C): header-prototyped functions
     suppressed, all flagged without -d. (2 tests)

  Fixtures: tests/fixtures/cli/{crossfile_callsite_null, crossfile_frees,
  crossfile_header}. Each scenario tests with-d, without-d, and safe
  variants.

---

# Task ID: 37
# Title: Analysis capabilities roadmap
# Status: done (v0.3.87)
# Dependencies: none
# Priority: P3
# Description: Track fundamental analysis limitations and potential improvements.
# Details:
DONE. Updated docs/architecture.rst with comprehensive inventory of all 10
analysis modules, current capabilities table (17 entries), known limitations
table (8 entries with impact descriptions), per-CWE ceiling analysis, and
updated competitor landscape from 5-tool benchmark. Previous version was
heavily outdated (referenced "No VRA", "No whole-program analysis", 48% TP
rate — all now incorrect).

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

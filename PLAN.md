# SqC — Plans & Roadmap

Last Updated: 2026-04-19 (v0.3.97)

Juliet benchmark v0.3.97: 24,622 TP / 14,226 FP (63.4% TP rate), 41.6% per-file.
Cumulative v0.3.93 → v0.3.97: TP rate +1.7pp, FP −1,542 (−9.8%), zero net
other-rule regressions.

## Competitor Benchmark Summary (v0.3.75)

5-tool comparison on 15 overlapping Juliet CWEs (28,488 files):

  clang-tidy: 13,952 TP /    116 FP (91.6%) — highest precision
  Frama-C:     8,609 TP /  5,510 FP (61.0%)
  SqC:        27,882 TP / 23,003 FP (54.8%) — broadest coverage (118 CWEs)
  Infer:       4,971 TP /  6,428 FP (43.6%)
  cppcheck:   29,377 TP / 51,361 FP (36.4%) — highest recall

SqC wins outright on CWE-690 (94.6%) and CWE-761 (100%).
Biggest gaps vs best competitor (clang-tidy unless noted, v0.3.96 current):
  CWE-190: 58.6% vs 94.3% (−35.7pp) — INT32-C/INT30-C (was −46.9pp, task 54 done)
  CWE-191: 53.9% vs 94.4% (−40.5pp) — INT32-C/INT30-C (was −48.9pp, task 54 done)
  CWE-369: 53.9% vs 94.7% (−40.8pp) — INT33-C/FLP03-C (was −57.8pp, task 55 done)
  CWE-476: 58.9% vs 94.3% (−35.4pp) — EXP34-C (was −43.5pp, task 57 done)
  CWE-121: 55.7% vs 86.6% (−30.9pp) — STR31-C/ARR38-C (was −37.8pp, task 56 done)
  CWE-415: 58.2% vs 80.0% (−21.8pp) — MEM01-C (was −36.6pp, task 58 done)
  CWE-416: 92.9% vs 60.3% (+32.6pp) — MEM01-C EXCEEDS target (task 58 done)
  CWE-401: 77.6% vs 83.9% (−6.3pp) — MEM31-C (was −33.2pp, task 59 done)
  CWE-78:  74.1% (+11.3pp since v0.3.93) — ENV03-C taint-aware (tasks 67-68)
  CWE-194: 67.9% (+ 9.5pp since v0.3.95) — INT31-C taint-aware (task 69)
  CWE-195: 51.9% (+ 3.2pp since v0.3.95) — INT31-C taint-aware (task 69)

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
# Status: done (v0.3.93)
# Dependencies: none
# Priority: P2
# Description: Reduce INT30-C FPs on bounded embedded arithmetic.
# Details:
DONE. Work spread across v0.3.88, v0.3.89, v0.3.92, v0.3.93.

Patterns recognized across the four commits:

  1. **Bitmask wrapping** (`(x + 1) & MASK`): v0.3.88 via
     `is_addition_masked_by_bitand`. Applies to addition only — the mask
     bounds the result regardless of any wrap.
  2. **Narrow-pre-cast addition/multiplication**: v0.3.88 + v0.3.89 via
     `both_operands_narrow_pre_cast` /`is_narrow_cast_plus_small_const` /
     `is_narrow_cast_times_small_const` using `effective_operand_type`
     that peers through explicit casts to recover `uint8_t` / `uint16_t`.
  3. **(WIDE)(a - b) guarded by `a > b + POS`**: v0.3.89 via
     `is_cast_over_guarded_narrow_sub` + `cond_gt_b_plus_positive`.
  4. **Same-array subtraction / widened comparison**: covered by
     narrow-pre-cast + guarded-subtraction combination.
  5. **Wide-unsigned struct-field `++`/`--`** (`ctx->tick++`,
     `obj.seq--`): v0.3.92 direct skip in `check_increment_decrement`.
     Monotonic counter fields wrap at 2^32/2^64 and are benign; Juliet
     CWE-190/191 only exercises local-variable `++`, so no TP loss.
  6. **Thin calloc wrapper** (`calloc(nmemb, size)` where both args are
     function parameters): v0.3.92 via
     `calloc_args_are_function_params` — delegates overflow to C11
     calloc (§7.22.3.2).
  7. **Implicit-else early-exit guard** (`if (a < b) return; /* a - b */`):
     v0.3.92 via `preceding_early_exit_guards_subtraction` + the new
     `if_always_exits` helper.

Infrastructure fixes surfaced along the way:
  * v0.3.92: `get_function_arguments` now iterates named children, so
    `args[0]` is the real first argument instead of a `(` token. This
    unmasked a latent bug where malloc/realloc multiplication overflow
    detection was silently disabled and calloc messages read
    `calloc((, nmemb)`.
  * v0.3.93: dropped the dedicated malloc/realloc allocation-overflow
    check. `check_multiplication` already covers the inner `*` (with
    full VRA + SIZE_MAX-guard awareness), so the allocation-level check
    produced nothing but duplicate diagnostics and occasional FPs on
    provably-safe good-paths. Calloc keeps its dedicated check because
    its multiplication is implicit.
  * v0.3.92: `has_function_context_check` falls back to the translation
    unit for snippet-style tests without an enclosing function.

Empirical impact on target embedded codebases (INT30-C suppressions
stripped for measurement, -I include paths unchanged):
  d_lib_common:          1 → 0 INT30-C FPs
  d_lib_serial_leds:    12 → 8 INT30-C FPs
  d_lib_airpath:         2 → 2 (unchanged; bounded-shift compound
                                addition, see Remaining below)
  d_lib_networking:      0 → 0

Real-world (v0.3.91 → v0.3.93): **−187 INT30-C violations**
  curl:      -106   mosquitto:  -44
  sqlite:     -37   libcrc:       0

Juliet v0.3.93 vs v0.3.91: zero delta across all 74 CWEs (the v0.3.92
duplicate-diagnostic regression on CWE-680 was fully reversed by the
v0.3.93 dedupe).

Remaining serial_leds FPs need narrow-operand propagation through local
assignments (brightness arithmetic via `uint32_t range = (uint32_t)a -
(uint32_t)b` then `range * time`). Airpath FPs need shift-aware
effective-type recognition for compound accumulators (`ctx->ad_sum +=
(... >> SHIFT) + 1`). Both deferred — low FP count, risk of Juliet TP
regressions on CWE-190/191.

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

# Task ID: 67
# Title: ENV03-C locally-safe popen/system command var suppression
# Status: done (v0.3.94)
# Dependencies: none
# Priority: P2
# Description: Reduce ENV03-C FPs on Juliet CWE-78 goodG2B functions where
#   the command variable is provably derived from string literals only.
# Details:
DONE. ENV03-C FP 1272→612 (-660), TP 912→752 (-160). **4.1:1 FP:TP**,
cleanest ratio since v0.2.23.

Approach: new `is_command_var_locally_safe()` in env03_c.rs. Fixpoint over
char-array declarations and pointer aliases, then walks every write to the
command variable. Suppress only when every write is from a string literal,
a locally-initialized buffer, or strcat/strcpy with a literal source.

Short-circuits that still flag:
  - Direct string literal (defense-in-depth preserved for existing tests).
  - Function parameter (caller-supplied, handled by task 68).
  - Any taint-source call in scope (recv, fgets, scanf, getenv, etc.).

Required allowing macro-identifier initializers on char arrays — `char
buf[N] = FULL_COMMAND;` is legal C only for macros, so any identifier in
that position is treated as a literal.

CWE-78 TP rate 62.8% → 72.3% (+9.5pp). Overall TP rate 61.7% → 62.5%
(+0.8pp). Zero other-rule regressions.

---

# Task ID: 68
# Title: ENV03-C cross-function taint for helper-sink variants
# Status: done (v0.3.95)
# Dependencies: 67
# Priority: P2
# Description: Suppress ENV03-C FPs in Juliet CWE-78 variants 41-45 where
#   the popen/system call lives in a helper function receiving data as a
#   parameter.
# Details:
DONE. ENV03-C FP 612→468 (-144), TP 752→652 (-100). 1.44:1 FP:TP —
diminishing returns as the remaining target shrinks.

New infrastructure:
  * `FunctionSummary.has_env03_taint_source` bit — populated via body
    text scan for recv/fgets/scanf/getenv-style calls (~25 functions).
  * ENV03-C parameter path consults reverse call graph (callee → callers)
    built from `ProjectContext.call_graph`. Suppress only when every
    caller's summary is clean.

Prescan bug fix (affects other inter-procedural rules too):
  * `collect_call_graph` used `.insert()`, so multiple files with `static
    void goodG2B()` overwrote each other and dropped caller edges.
    Changed to `.entry().or_default().extend()` for call edges, and
    OR-merge for `has_env03_taint_source`. Same-named static merging is
    conservative (any tainted def poisons the merged summary).

Results: CWE-78 TP rate 72.3% → 74.1% (+1.8pp). Overall TP rate 62.5% →
62.7% (+0.2pp). Side effect: EXP34-C -6 FP / 0 TP (prescan merging
recovered a few missed caller links).

CWE-426 (Untrusted Search Path): -24 TP / -24 FP — neutral. Remaining
~468 ENV03-C FPs are v42/v22a-style (`data = helper(data)` return-tainted
assignments) and v45-style (global-static pointers) — need return-value
taint or global-write tracking to shrink further.

---

# Task ID: 69
# Title: INT31-C taint-aware suppression for signed→size_t conversions
# Status: done (v0.3.96)
# Dependencies: 68
# Priority: P2
# Description: Reuse ENV03-C's taint-source summary bit to suppress
#   INT31-C FPs on Juliet CWE-194/195 helper-function variants.
# Details:
DONE. INT31-C FP 2172→1452 (-720), TP 2608→2142 (-466). 1.54:1 FP:TP.

Approach: when the converted variable is inside an `if (var < LIT)`
upper-bound guard (positive literal), suppress iff:
  * the containing function's summary has no taint source, AND
  * no `var = fn(...)` assignment targets a tainted callee, AND
  * for parameters, every caller is taint-free too.

Local-variable case additionally requires evidence of at least one
call-return assignment from a clean callee — prevents over-suppression
of v45-style `int data = global_static;` reads where the global could
have been tainted elsewhere in the project.

Substring bug trap: `decl_text.contains(var_name)` matched `dataBuffer`
when looking for `data`. Fixed by walking the declarator tree to its
leaf identifier for an exact match.

Results: CWE-194 TP rate 58.4% → **67.9%** (+9.5pp). CWE-195 TP rate
48.7% → 51.9% (+3.2pp). Overall TP rate 62.7% → **63.4%** (+0.7pp).
Zero other-rule regressions.

Remaining INT31-C FPs are in v42-style `data = helper(data)` returning
tainted values, v45-style global pointers, and v65a/b function-pointer
cross-file patterns (no call-graph edge). Function-pointer and
return-value taint would be the next wins if pursued.

---

# Task ID: 62
# Title: API00-C validation look-ahead past variable declarations
# Status: done (v0.3.97) — A already covered, C implemented, B deferred
# Dependencies: none
# Priority: P2
# Description: Reduce API00-C FPs where parameter validation exists but
#   is not detected due to intervening variable declarations (~12 FPs).
# Details:
Three sub-patterns triaged:

  A. **Validation past var decls** — already handled pre-v0.3.97. The
     existing `check_validation_patterns` walks the entire body (not just
     the first N statements) and `collect_else_if_chain_validations`
     traverses full if/else-if/else chains, so cases like
     `Ringbuffer_read` (if/else-if/else with NULL checks after a
     `result_e result = …;` declaration) no longer FP. Verified on
     d_lib_common with API00-C suppressions stripped — zero FPs for this
     sub-pattern.

  C. **void\* container where NULL is valid** — implemented in v0.3.97.
     New type-aware suppression: a `void *` / `const void *` parameter
     that is never dereferenced locally (`*p`, `p->x`, `p[i]`) and
     passes through only to null-accepting stdlib sinks (free, realloc,
     Memory_Free, Memory_Realloc, cfree) or callees whose summary
     validates the corresponding argument is treated as a generic-
     container slot where NULL is a valid value.

     Helpers: `is_generic_void_pointer_type` (bare `void *`, rejects
     `void **` and array decls) + `is_void_ptr_storage_safe` (walks the
     body, collects every parent-kind, returns true only if every use is
     storage-like or a verified safe call).

     Results (Juliet v0.3.96 → v0.3.97):
       - API00-C: TP 90→84 (-6), FP 116→104 (-12). **2.0:1 FP:TP**.
       - CWE-476 TP rate 58.9% → **59.6%** (+0.7pp).
       - Overall TP rate unchanged at 63.4%.
       - Zero other-rule regressions.

     Real-world d_lib_common: 1 → 0 API00-C FP (ArrayList_Append).

  B. **Embedded API contract — no NULL check by design** (3 FPs in
     airpath, ~17 FPs in serial_leds). Deferred. All remaining real-
     world API00-C FPs are public-API functions that dereference `ctx`
     without validation by hardware-contract design (e.g.
     `DebrisSensor_AdcSample`, `SerialLeds_set_*`). No clean AST-level
     signal distinguishes these from genuine FPs — continuing to rely
     on `SQC-SUPPRESS: API00-C` at the function site. Options considered
     and rejected:
       * Skip all public API with `_set_` / `_Init` / `_update` prefixes
         — too broad; masks real bugs in other projects.
       * Require explicit header prototype + Doxygen `@pre` — no
         reliable way to parse `@pre` contracts from tree-sitter.
       * Switch API00-C severity to Low for non-static functions —
         doesn't reduce FP count, only its visibility.

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
# Status: partial (v0.3.94-0.3.96 — tasks 67-69 delivered first-pass)
# Dependencies: none
# Priority: P3
# Description: Expand cross-function taint tracking for injection CWEs,
  referenced in paper future work section.
# Details:
Partial delivery via tasks 67-69 (CWE-78 62.8% → 74.1%, CWE-194 58.4% →
67.9%, CWE-195 48.7% → 51.9%). Remaining scope below.

Delivered infrastructure:
  - `FunctionSummary.has_env03_taint_source` — body text scan for ~25
    taint sources (recv, fgets, scanf, getenv, Win32 I/O, etc.)
  - Reverse call graph built in each consuming rule from
    `ProjectContext.call_graph`
  - Same-named static function merging in prescan (any tainted def
    poisons the merged summary)

Rule integrations so far:
  - ENV03-C: parameter path → caller summary lookup (task 68)
  - INT31-C: signed→size_t inside upper-bound guards → caller / callee /
    local-assignment summary lookups (task 69)

Remaining gaps for CWE-78 (~468 FPs) and CWE-194/195 (~1224 FPs):
  - Return-value taint propagation (`data = helperSource(data)` where
    the helper transforms but does not call taint sources itself)
  - Global/static pointer read tracking (v45-style `char *data =
    g_goodG2BData;` where `g_goodG2BData` was last written elsewhere)
  - Function-pointer cross-file calls (v65a/b, v67 variants — no
    call-graph edge for `funcPtr(data)`)
  - CWE-89 sinks: add SQL injection entry points (sqlite3_exec,
    mysql_query, PQexec) and the STR02-C or a new taint-sink rule

Rule candidates that would benefit from the existing taint bit once
integrated: FIO30-C, STR02-C, FMT variants for format-string injection.

This task stays open as the coordinating umbrella; individual
rule-integration rounds are tracked as follow-ons to tasks 68/69.

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

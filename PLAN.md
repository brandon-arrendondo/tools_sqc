# SqC — Plans & Roadmap

Last Updated: 2026-04-01 (v0.3.56)

For completed work, see CHANGELOG.txt.
For benchmark data, see JULIET_RESULTS.md and REALWORLD_RESULTS.md.
For competitor research and academic references, see docs/bibliography.rst.

Default test strategy for all tasks: pre-commit hooks (cargo test + cargo fmt),
then Juliet benchmark and real-world benchmark to validate.

---

# Task ID: 1
# Title: Juliet TP rate to 50%
# Status: done
# Dependencies: none
# Priority: P0
# Description: Raise Juliet CWE-matched TP rate from 48.4% to 50%.
# Details:
Achieved in v0.3.44: 51.1% TP rate (+2.7pp from 48.4%). See CHANGELOG.txt.

---

# Task ID: 2
# Title: EXP33-C cross-function variants
# Status: done
# Dependencies: 19
# Priority: P2
# Description: Inter-procedural analysis for EXP33-C variants 63/64.
# Details:
Done in v0.3.49-v0.3.50.

v0.3.49: EXP33-C cross-file init tracking via set_project_context().
  - build_read_only_deref_fns(): dereferences_params - modifies_params
  - InitAnalysisConfig.read_only_deref_fns prevents &var from being marked
    initialized when callee only reads through the pointer
  - check_cross_file_uninit_calls: flags calls passing &uninit_var to
    functions that read the pointed-to value (variant 63 pattern)
  - +10 TP, +10 FP (10 simple scalar/pointer types)

v0.3.50: Prescan recognizes (type *)param cast pattern as dereference.
  - Enables variant 64 detection (void pointer → cast → deref)
  - +10 TP, +10 FP (same 10 simple types via void* indirection)

Combined v0.3.48→v0.3.50: CWE-457 402→422 TP (+20), 695→715 FP (+20),
TP rate 36.6%→37.1% (+0.5pp). Zero regressions on other CWEs.

FPs are from Juliet goodB2G pattern: &uninit_var passed to function that
reads *param but reassigns a local copy — technically UB but Juliet
considers "good". 1:1 TP:FP ratio is precision-neutral.

Array types (alloca/malloc/declare) not affected: pointer variable is
Initialized (assigned via alloc), content uninitialized — already detected
by intra-procedural analysis.

---

# Task ID: 3
# Title: DCL31-C/DCL07-C remaining include gaps
# Status: done
# Dependencies: none
# Priority: P1
# Description: Fix unresolved function declarations in mosquitto and sqlite.
# Details:
v0.3.38 removed library-specific whitelists and added -I include paths.

Root cause: bug in analyze/mod.rs — resolve_includes was skipped when
--load-prescan was set, making -I include paths ineffective during parallel
scans. One-line fix: removed `load_prescan.is_none()` guard.

Realworld benchmark results (v0.3.44 → v0.3.47):
  DCL31-C: 7,366 → 1,926 (-5,440, -73.9%)
  DCL07-C: 7,291 → 1,846 (-5,445, -74.7%)
  Combined: 14,657 → 3,772 (-10,885)

Per-project DCL31-C+DCL07-C:
  hostap:    6,939 → 1,469 (-5,470)
  sqlite:    3,744 →   561 (-3,183)
  mosquitto: 2,061 →   103 (-1,958)
  curl:      1,913 → 1,639 (-274)

---

# Task ID: 4
# Title: CWE-457 uninitialized variable remaining gaps
# Status: done
# Dependencies: none
# Priority: P2
# Description: Improve CWE-457 TP rate beyond 37.1%.
# Details:
v0.3.52: Three-phase improvement to CWE-457 detection.

Findings: CWE-457 has 22 C variants (01-18, 63a/63b, 64a/64b). Variants
61/62/65-68 do NOT exist for CWE-457 in C (corrected from original plan).
28 data types × 22 variants = 616 files.

Phase 1 — Partial init array detection (+108 TP):
  Track allocation_count in VarInfo from malloc/ALLOCA calls. Compare
  for-loop bound vs allocation count on subscript writes. If bound <
  allocation count → keep MallocUninitialized (partial init detected).
  6 partial_init types × 18 variants = 108 new TPs. Zero FP regression.

Phase 2 — Constant condition folding (FP reduction, intra-file):
  Added comparison operators to const_eval. Collect file-scope static
  [const] constants. Dead-branch elimination in init-state worklist for
  known-true/false conditions. Handles variants 02-07 (literal, static
  const, static var conditions). -120 FP from non-array types.

Phase 3 — Prescan constant propagation (FP reduction, cross-file):
  Collect global constants from prescan directories into
  ProjectContext.global_constants. Merge with macro_constants. Handles
  variants 09-10, 13-14 (cross-file GLOBAL_CONST_*, globalTrue, etc.).

Remaining gaps (not addressed):
- Variant 08 (staticReturnsTrue()), 11 (globalReturnsTrue()): function
  call conditions not resolvable without return value inlining.
- Variant 12 (globalReturnsTrueOrFalse()): genuinely random — correct
  behavior to flag as MaybeUninitialized.
- declare_* array types: array-to-pointer decay (data = dataUninitArray)
  treated as read of uninitialized stack array. FPs from this persist.

---

# Task ID: 5
# Title: CWE-190/191 integer overflow coverage
# Status: pending
# Dependencies: none
# Priority: P3
# Description: Improve CWE-190/191 detection beyond current stable rates.
# Details:
v0.3.37: CWE-190 655 TP/790 FP (45.3%), CWE-191 560 TP/716 FP (43.9%).
Unchanged since v0.3.28. INT30-C/INT32-C matched. Stable after VRA. Gap is
coverage not precision — additional detection requires new analysis patterns,
not FP reduction.

---

# Task ID: 6
# Title: CWE-690 per-file detection improvement
# Status: pending
# Dependencies: 7
# Priority: P3
# Description: Raise CWE-690 per-file detection rate from 18.1% toward 30%.
# Details:
v0.3.37: 203 TP, 12 FP, 94.4% TP rate, 18.1% per-file. Best precision of any
high-volume CWE. 74% undetected are likely cross-function patterns. Improving
per-file rate depends on EXP34-C Phase 4 (task 7) for deeper inter-procedural
null propagation.

---

# Task ID: 7
# Title: EXP34-C Phase 4 edge cases
# Status: in-progress
# Dependencies: none
# Priority: P2
# Description: Address remaining EXP34-C inter-procedural gaps.
# Details:
Split into sub-tasks 41-45. See individual tasks for details.

v0.3.53 results (tasks 41-43 complete):
  Overall: 52.0% TP rate (+0.1pp from v0.3.52). +58 TP, -6 FP.
  CWE-476: 355 TP / 397 FP (47.2%, +1.7pp). +18 TP, -6 FP.
  CWE-690: 560 TP / 38 FP (93.6%, +0.4pp). +40 TP, 0 FP.
  Realworld: EXP34-C +159, API00-C -147. No performance regression.

v0.3.55: Task 44 variant 67 (struct field null propagation) implemented.
  Smoke test: +6 TP, 0 FP across all 6 data types in CWE-476 variant 67.

v0.3.56: Task 44 variants 63, 64, 66 implemented.
  Variant 63 (pointer-to-pointer): +6 TP, 0 FP.
  Variant 64 (void pointer cast): +6 TP, 0 FP.
  Variant 66 (array element): +6 TP, 0 FP.
  Combined: +18 TP, 0 FP. 6 new regression tests.

Remaining: variant 65 only (function pointer, deferred). Task 45 (regression tests) done.

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

# Task ID: 11
# Title: Zero-detection CWEs — deferred
# Status: deferred
# Dependencies: none
# Priority: P3
# Description: Remaining zero-detection CWEs requiring new capabilities.
# Details:
CWEs that will auto-benefit when rules are added:
- CWE-364 Signal Handler Race Condition (18 files)
- CWE-674 Uncontrolled Recursion (2 files)
- CWE-563 Unused Variable (2 files), CWE-398 Poor Code Quality (1 file)

Deferred CWEs requiring new analysis:
- CWE-789 (560 files): taint tracking for user input -> malloc size
- CWE-114 (672 files): taint tracking for untrusted input -> LoadLibrary
- CWE-272 (252 files): Windows-only HKEY_LOCAL_MACHINE vs HKEY_CURRENT_USER
- CWE-259 (112 files): password/credential string pattern matching
- CWE-666 (90 files): state machine for resource lifecycle
- CWE-226 (72 files): MEM03-C only checks free/realloc, not stack scope exit
- CWE-327 (54 files): crypto API knowledge (RC5 vs AES)
- CWE-468 (36 files): implicit void* casts losing type info
- CWE-459 (36 files): resource tracking for incomplete cleanup
- CWE-188 (36 files): struct padding/alignment analysis

10 formerly zero-detection CWEs resolved in v0.3.35-v0.3.42. 4 more
resolved in v0.3.47 (CWE-675 double-close, CWE-273 Windows privilege
APIs, CWE-562 analyzer fix, CWE-561 mapping). 13 are Windows-only
(not actionable).

---

# Task ID: 12
# Title: EXP16-C test registration
# Status: done
# Dependencies: none
# Priority: P1
# Description: Fix EXP16-C test discovery in cargo test.
# Details:
Investigated: tests work correctly. 6 generated tests all pass via
`cargo test -- exp16`. The issue was the test filter path documented in
CLAUDE.md (`rules::cert_c::RULE_ID::tests`) only matches rules with
embedded #[cfg(test)] modules. EXP16-C correctly uses only .c file tests,
discovered via `generated_tests::test_exp16_c_*`.

---

# Task ID: 13
# Title: Raise coverage gate to 81%
# Status: pending
# Dependencies: none
# Priority: P2
# Description: Raise test coverage gate from 80% to 81% (~1,175 additional lines).
# Details:
Current: 80.06% (24,908 uncovered of 124,904 lines). Highest-impact targets:

  ARR38-C: 759 uncovered, 70.7% coverage. Add .c test cases for library
    patterns.
  value_range.rs: 567 uncovered, 70.9%. Add unit tests for condition/assignment
    handlers.
  ERR33-C: 550 uncovered, 71.2%. Add .c test cases for stdlib error checks.
  INT31-C: 516 uncovered, 70.1%. Add .c tests for integer conversion patterns.
  INT34-C: 451 uncovered, 54.7%. Add .c tests for VRA and range paths.
  analyze/mod.rs: 401 uncovered, 40.4%. Integration test for analyze_project
    (needs manifest + source fixture).
# Test Strategy: Run scripts/coverage-gate.sh after adding tests. Must reach
80.99% minimum to round to 81%.

---

# Task ID: 14
# Title: FP regression tests for rounds 3, 6, 7
# Status: done
# Dependencies: none
# Priority: P2
# Description: Add regression tests for FP reduction work not yet covered.
# Details:
All rounds now have regression tests.

Round 3 (std_functions): Already covered by testcases_stdlib_calls_regression.c
in DCL31-C and DCL07-C pass/ directories.

Round 6 (cross-file prescan): Added testcases_crossfile_prescan_regression.c for
both DCL31-C and DCL07-C. Uses `// sqc-test: prescan` marker to exercise the
cross_file_functions suppression path. Functions defined after their call site
are discovered by prescan and not flagged. Also fixed prescan_single_tree() to
populate known_functions from function_summaries keys (was empty due to
ProjectContext::default()).

Round 7 (EXP36-C): Unknown source type already covered by
testcases_unknown_type_cast_regression.c. Added
testcases_nonpointer_cast_regression.c for the non-pointer target type
suppression (integer casts like (unsigned)time(NULL) should not trigger).

---

# Task ID: 15
# Title: Expand wiki-only rule test coverage
# Status: done
# Dependencies: none
# Priority: P2
# Description: Add tests for ~105 rules with only 2 test files each.
# Details:
Prioritize top-FP rules from real-world benchmarks:
- EXP33-C (6,100 real-world violations) — only 10 wiki tests
- EXP34-C (5,267) — 6 wiki tests
- ERR33-C (989) — 11 wiki tests

v0.3.54: Batch 1 — 81 new test files across 29 rules (73 → 45 rules at 2 tests).

Rules expanded (2 → 4-7 tests each):
  Batch 1 (11 rules): MEM01-C, MEM00-C, EXP12-C, ERR00-C, FIO24-C, FIO37-C,
    FIO38-C, DCL18-C, STR01-C, STR04-C, PRE08-C.
  Batch 2 (18 rules): EXP03-C, EXP07-C, EXP09-C, EXP13-C, EXP14-C, EXP32-C,
    FIO08-C, FIO09-C, FIO39-C, STR06-C, STR09-C, STR11-C, ERR04-C, ERR06-C,
    DCL38-C, DCL41-C, INT12-C, INT13-C.

Each rule gained 2-5 new tests covering additional violation patterns (fail/)
and safe usage patterns (pass/).

Batch 3 (20 rules): FIO11-C, FIO13-C, FIO14-C, FIO17-C, FIO40-C, FIO46-C,
  FIO50-C, FLP04-C, FLP36-C, FLP37-C, PRE04-C, PRE09-C, API02-C, API04-C,
  API09-C, API10-C, MEM07-C, MSC33-C, MSC39-C, EXP15-C.
53 new test files.

Batch 4 (25 rules): CON01-C, CON04-C, CON05-C, CON06-C, CON31-C, CON33-C,
  CON35-C, CON36-C, CON37-C, CON39-C, POS02-C, POS04-C, POS05-C, POS36-C,
  POS39-C, POS44-C, POS48-C, POS51-C, POS52-C, POS53-C, POS54-C, WIN00-C,
  WIN01-C, WIN02-C, WIN04-C.
49 new test files. All 3204 tests pass.

No rules remaining at 2 tests. Task complete.

---

# Task ID: 16
# Title: CLI integration tests
# Status: done
# Dependencies: none
# Priority: P2
# Description: Test CLI flags: --diff, --export json/csv/sarif, -I, -d,
  --save-prescan/--load-prescan, suppression.
# Details:
tests/cli_integration.rs: 23 integration tests invoking sqc as a subprocess.

Export formats (4 tests): JSON structure/empty, CSV header+row, SARIF schema+results.
Exit codes (4 tests): --fail-on-violation (with/without violations),
  --fail-on-severity at/below threshold.
Filtering (4 tests): --rules include/exclude, --min-severity at/below threshold.
Prescan caching (1 test): --save-prescan then --load-prescan round-trip.
Suppression (4 tests): inline SQC-SUPPRESS, TOML --suppress-file,
  suppressed violations don't trigger --fail-on-violation,
  SARIF includes suppressed violations with metadata.
  --generate-suppression outputs correct hash.
Cross-file (2 tests): -d flag suppresses DCL31-C, without -d flags violation.
Diff mode (1 test): --diff only analyzes modified/new files in git repo.

Fixtures in tests/fixtures/cli/: violation.c, clean.c, suppressed_inline.c,
  suppress.toml, manifest_msc04.toml, manifest_dcl31.toml,
  project/main.c + project/helpers/helper.c.

Also fixed prescan_single_tree() to populate known_functions (task 14).

---

# Task ID: 17
# Title: expected_fail test category
# Status: pending
# Dependencies: none
# Priority: P3
# Description: Add expected_fail/ test directory for known limitations.
# Details:
For cases like EXP34-C intra-file null deref without call-site analysis.
Tests that document known gaps without falsely passing. Requires build.rs
changes to generate test expectations differently for this directory.

---

# Task ID: 18
# Title: Fix FIO10-C POSIX rename
# Status: done
# Dependencies: none
# Priority: P3
# Description: Accept POSIX rename() with error checking as compliant.
# Details:
Done in v0.3.48. See CHANGELOG.txt.

---

# Task ID: 19
# Title: EXP34-C intra-file call-site test infrastructure
# Status: done
# Dependencies: none
# Priority: P2
# Description: Enable test infra to exercise intra-file prescan for EXP34-C.
# Details:
Done in v0.3.48. `// sqc-test: prescan` marker system. See CHANGELOG.txt.

---

# Task ID: 20
# Title: Inter-procedural .c test cases
# Status: pending
# Dependencies: 19
# Priority: P3
# Description: Multi-file C test cases for prescan/call-site propagation.
# Details:
Need test infrastructure that can compile and analyze multiple .c files
together to exercise prescan context, cross-file function resolution, and
call-site null state propagation.

---

# Task ID: 21
# Title: Fix remaining implementation bugs
# Status: pending
# Dependencies: none
# Priority: P2
# Description: 3 remaining implementation bugs.
# Details:
3 of 6 fixed in v0.3.48 (MEM04-C, FIO10-C, WIN30-C). See CHANGELOG.txt.

Remaining:
- STR03-C: strncpy with prior strlen validation still flagged (Low).
    str03_c.rs has find_length_check_in_scope() (line ~135) which looks for
    strlen/sizeof checks in if statements, but only accepts the call if it's
    in the else branch (line ~175). Should also accept strncpy when a preceding
    strlen check validates the buffer size, regardless of branch placement.
    Test: pass/wiki_adequate_space.c uses strcpy (not strncpy) so passes.
    Need a new fail test with strncpy preceded by strlen that currently flags.
- INT00-C: unsigned subtraction without guard not detected (Medium).
    int00_c.rs only checks format specifier mismatches (line ~56) and unsafe
    cast+multiplication patterns (lines 416-490). Does NOT check for unsigned
    integer wrap on subtraction. Fix requires adding a new check: binary
    expressions with `-` operator on unsigned types without prior range
    validation (a >= b). Test: pass/testcases_unsigned_wrap.c should move to
    fail/ once implemented.
- INT16-C: signed-to-unsigned conversion without range check not detected
    (Medium). int16_c.rs focuses entirely on bitwise operations on signed
    integers (lines 166-224). Does NOT detect assignments/returns of
    potentially negative signed values to unsigned types. Fix requires
    tracking both signed AND unsigned variables and detecting cross-type
    assignment/return without range check (e.g., `if (x >= 0)`). This is a
    different analysis pattern than what the rule currently implements.
    Test: pass/testcases_signed_unsigned_conversion.c should move to fail/.

INT00-C and INT16-C are pattern mismatches — significant rule redesign needed.

---

# Task ID: 22
# Title: Fake-passing tests periodic review
# Status: pending
# Dependencies: 21
# Priority: P2
# Description: Review and fix tests that pass only due to implementation gaps.
# Details:
2 remaining fake-passing tests (11 fixed in v0.3.42, 5 resolved here):

Resolved:
- EXP34-C: 3 tests moved to fail/ with prescan marker (task 19)
- FIO10-C: POSIX rename() now accepted as compliant (task 18)
- WIN30-C: Reclassified as out of scope (not a fake pass)

Remaining:
- INT00-C pass/testcases_unsigned_wrap.c — pattern mismatch
- INT16-C pass/testcases_signed_unsigned_conversion.c — pattern mismatch

---

# Task ID: 23
# Title: Internal parallelization (rayon)
# Status: pending
# Dependencies: none
# Priority: P3
# Description: File-level parallelism within a single sqc invocation using rayon.
# Details:
Currently parallelism is external via scripts/sqc_parallel_scan.py with
subdirectory splitting + prescan cache. Internal parallelism via rayon would
simplify deployment (single binary, no Python wrapper) and enable finer-grained
work distribution.

---

# Task ID: 24
# Title: File-size-aware batching
# Status: pending
# Dependencies: none
# Priority: P3
# Description: Balance parallel work by file size instead of directory.
# Details:
Current subdir splitting can leave one large unit dominating wall time (e.g.,
wpa_supplicant/ 69 files = 1061s). Batch by file size rather than directory to
balance work across workers. Applies to both external (sqc_parallel_scan.py)
and future internal parallelism.

---

# Task ID: 25
# Title: Incremental parsing
# Status: pending
# Dependencies: none
# Priority: P3
# Description: Only re-parse changed files on subsequent runs.
# Details:
Track file modification times or content hashes. Skip parsing and re-use
cached ASTs for unchanged files. Significant speedup for iterative development
workflows.

---

# Task ID: 27
# Title: Docker image
# Status: pending
# Dependencies: none
# Priority: P3
# Description: Containerized CI/CD distribution of sqc.
# Details:
Dockerfile for sqc with all dependencies. Enables drop-in CI/CD usage without
local Rust toolchain installation. Part of Tier 2 production quality definition
of done.

---

# Task ID: 28
# Title: MSC07-C unreachable code detection
# Status: done
# Dependencies: none
# Priority: P2
# Description: Code after return/exit/abort, always-false branches (BRULE-034 gap).
# Details:
Implemented MSC07-C. AST-based detection of unreachable code patterns:
- Statements after unconditional return/break/continue/goto in same block
- Statements after noreturn calls: exit, abort, _Exit, longjmp, quick_exit,
  thrd_exit, ExitProcess, ExitThread
- Skips preprocessor directives and comments
- Reports only first unreachable statement per terminal (avoids noise)
- CWE-561 mapping. Low severity (recommendation).
- 11 tests: 7 fail (all terminal types), 4 pass (conditional returns,
  nested scopes, switch/break, loop/continue).

---

# Task ID: 29
# Title: Recursion detection (BRULE-058)
# Status: done
# Dependencies: none
# Priority: P2
# Description: Detect recursive function calls via call-graph cycle detection.
# Details:
Implemented as MSC04-C. Maps to BRULE-058 (Constrained tier).

Direct recursion: AST-only detection — scans function body for self-calls.
Works without prescan data. Bounded recursion suppression: if the function
has a parameter-dependent base case (conditional return checking a parameter),
the violation is suppressed. CWE-674: 2 TP/2 FP → 1 TP/0 FP (100% TP rate).

Indirect recursion: DFS cycle detection on prescan call_graph. Requires -d
flag for cross-function call graph. Merges current file's callees with prescan
graph to handle files not in the prescanned set.

Uses existing prescan infrastructure (call_graph: HashMap<String, HashSet<String>>)
via set_project_context(). CWE-674 mapping added.

---

# Task ID: 30
# Title: Pointer indirection depth check (BRULE-065)
# Status: pending
# Dependencies: none
# Priority: P3
# Description: Flag declarations with excessive pointer indirection (e.g., ***p).
# Details:
BRULE-065 (All tier) limits pointer indirection depth. Simple AST check:
count * depth in declarations. Straightforward implementation, low effort.

---

# Task ID: 31
# Title: Post-init malloc detection (BRULE-060)
# Status: pending
# Dependencies: none
# Priority: P3
# Description: Flag malloc/free calls outside main()/init functions.
# Details:
BRULE-060 (Constrained tier) prohibits dynamic allocation after
initialization. Heuristic: identify init functions (main, *_init, *_setup) and
flag malloc/calloc/realloc/free in all other functions. Medium effort.

---

# Task ID: 32
# Title: CWE-matched TP rate >= 50% on key CWEs
# Status: done
# Dependencies: 1
# Priority: P1
# Description: Tier 2 production quality milestone.
# Details:
Achieved in v0.3.44: 51.1% overall TP rate. 20 CWEs at 100%, CWE-78 jumped
from 45.5% to 63.0%. Remaining below-50% CWEs are volume-heavy (CWE-121
47.1%, CWE-190 45.3%, CWE-191 43.9%) and need INT32-C/STR31-C improvements.

---

# Task ID: 33
# Title: Direct benchmark comparison with Infer and Frama-C
# Status: pending
# Dependencies: 32
# Priority: P3
# Description: Tier 3 competitive milestone — run Infer and Frama-C on same
  Juliet suite.
# Details:
See docs/bibliography.rst for tool references. Need to install Infer and
Frama-C, run on Juliet test suite with equivalent CWE coverage, and compare
TP/FP rates directly.

---

# Task ID: 34
# Title: Per-file detection >= 30% on top 10 CWEs
# Status: pending
# Dependencies: 32
# Priority: P3
# Description: Tier 3 competitive milestone — per-file detection rate.
# Details:
Per-file detection measures whether at least one TP is found per Juliet test
file. Current rates vary widely. Improving requires better cross-function
analysis for variants that span multiple functions within a file.

---

# Task ID: 35
# Title: Real-world FP tracking dashboard
# Status: done
# Dependencies: none
# Priority: P1
# Description: Track top FP-producing rules across real-world codebases.
# Details:
v0.3.42 per-rule data (all 5 codebases, 154.6K total, rules-benchmark.toml):

  MEM30-C:  15,330 — use-after-free (deferred, task 8)
  DCL13-C:  12,138 — const correctness (deferred, task 10)
  INT32-C:  12,050 — signed overflow (stable after VRA)
  API00-C:   9,227 — missing size parameter (stable)
  INT30-C:   8,474 — unsigned overflow (stable after VRA)
  DCL31-C:   7,366 — undeclared function (prescan + -I, task 3)
  DCL07-C:   7,291 — implicit int declaration (prescan + -I, task 3)
  MEM31-C:   5,440 — memory leak (deferred, task 9)
  EXP34-C:   5,267 — null deref (stable)
  EXP33-C:   5,013 — uninitialized (v0.3.38 FP fixes)
  STR34-C:   2,801 — char-to-int conversion (v0.3.42: +252)
  API05-C:   2,805 — unused return value (stable)
  ARR00-C:   2,157 — array bounds (v0.3.37: -480)
  ERR33-C:     989 — unchecked return (v0.3.37: -818)
  CON34-C:     871 — thread-unsafe functions (v0.3.42: new)
  ARR36-C:     829 — pointer subtraction (v0.3.39: -3,856)

v0.3.42 vs v0.3.39 delta: +1,442 (+0.9%). All increases from newly-fixed
rules: CON34-C +871, STR34-C +252, EXP40-C +114, ERR01-C +106, CON07-C +72,
DCL02-C +14, POS50-C +11.

v0.3.44 vs v0.3.42 delta: +5 (+0.003%, essentially flat). 154,598→154,603.
- FLP03-C: -164 (381→217, -43%). Clean reduction across all projects.
  sqlite -133, curl -18, mosquitto -10, hostap -3, libcrc 0.
- ENV33-C: -1 (5→4). Minimal real-world impact — few system() calls.
- EXP16-C: +160 (49→209). Regression in sqlite only (39→209). Unrelated to
  v0.3.44 changes — likely side effect of v0.3.43 MSC12-C. Needs investigation.
- FIO01-C: +10. New detections (curl +4, sqlite +6). Also unrelated.

v0.3.47 vs v0.3.44 delta: -12,418 (-8.0%). 154,603→142,185.
Dominant: DCL31-C -5,440, DCL07-C -5,445 (include resolution fix, task 3).
Also: EXP07-C -782, MSC37-C -354, INT36-C -277, PRE08-C -174, DCL18-C -137.
Regressions: INT32-C +491 (per-function type_map scope), FIO24-C +280 (new rule).

Updated per-rule top list (v0.3.47, 142.2K total):
  MEM30-C:  15,330 — use-after-free (deferred, task 8)
  INT32-C:  12,541 — signed overflow (+491 from type_map scope fix)
  DCL13-C:  12,138 — const correctness (deferred, task 10)
  API00-C:   9,199 — missing size parameter (-28)
  INT30-C:   8,435 — unsigned overflow (-39)
  EXP34-C:   5,234 — null deref (-33)
  MEM31-C:   5,440 — memory leak (deferred, task 9)
  EXP33-C:   5,013 — uninitialized (stable)
  STR34-C:   2,801 — char-to-int conversion (stable)
  API05-C:   2,805 — unused return value (stable)
  ARR00-C:   2,157 — array bounds (stable)
  DCL31-C:   1,926 — undeclared function (-5,440, task 3 fix)
  DCL07-C:   1,846 — implicit int declaration (-5,445, task 3 fix)

---

# Task ID: 36
# Title: BRULE coverage gap assessment
# Status: done
# Dependencies: none
# Priority: P2
# Description: Identify BRULE enforcement gaps where sqc could help.
# Details:
Assessment complete. Partially covered BRULEs: BRULE-029 (strong typing),
BRULE-034 (no dead code, gap: MSC07-C task 28), BRULE-035 (header usage),
BRULE-037 (switch default, GCC covers), BRULE-062 (check return values),
BRULE-067 (input validation, needs taint).

New rules needed: BRULE-058 recursion (task 29), BRULE-059 fixed loop bounds
(high effort, no task), BRULE-060 post-init malloc (task 31), BRULE-061
volatile correctness (DCL22-C exists, gap is context), BRULE-063 boolean side
effects (EXP30-C exists), BRULE-064 preprocessor complexity (PRE rules exist,
gap is nesting depth), BRULE-065 pointer depth (task 30).

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

# Task ID: 38
# Title: Completed architecture items
# Status: done
# Dependencies: none
# Priority: n/a
# Description: Tracking for completed architecture evolution milestones.
# Details:
- Prescan cache (v0.3.28): --save-prescan/--load-prescan, persistent in
  data/prescan_cache/, --rebuild-prescan for stale cache
- External parallelization (v0.3.27-v0.3.28): scripts/sqc_parallel_scan.py
  with subdirectory splitting + prescan cache
- Glob/prefix suppression (v0.3.43): [[wildcard]] TOML section with file_glob,
  rule, rule_glob, function_prefix fields (ANDed)
- Tier 1 CI/CD complete: --fail-on-violation, --fail-on-severity, JSON/CSV/SARIF
  output, incremental --diff, severity filtering, GitHub Actions + Azure DevOps
  examples
- Real-world validation on 5 open-source projects (libcrc, sqlite, mosquitto,
  curl, hostap)

---

# Task ID: 39
# Title: FLP03-C test coverage
# Status: done
# Dependencies: none
# Priority: P2
# Description: Add test cases for FLP03-C guard detection and conversion removal.
# Details:
6 new tests (4 → 10 total FLP03-C tests):

fail/ (2 new):
  division_no_guard.c — FP division without any guard or fenv checking
  division_ge_zero.c — Division inside `if (x >= 0)` (does NOT exclude zero)

pass/ (4 new):
  division_fabs_guard.c — Division inside `if (fabs(x) > 0.000001)`
  division_ne_zero.c — Division inside `if (x != 0)`
  division_gt_zero.c — Division inside `if (x > 0)`
  conversion_only.c — Cast to float/double without division (not flagged)

division_fenv.c already covered by existing wiki_c.c.

---

# Task ID: 41
# Title: EXP34-C/FIO06-C Phase 3 regression investigation
# Status: done
# Dependencies: 7
# Priority: P2
# Description: Investigate whether +76 FP EXP34-C / +169 FP FIO06-C regression
  from Phase 3 prescan enhancement (v0.2.17) still exists in v0.3.52.
# Details:
Investigated in v0.3.52. Both regressions are stale:

FIO06-C (+169 FP): Maps to CWE-276/279/732 — none are in the current
70-CWE fast-mode benchmark. FIO06-C doesn't appear in any top rule list.
Unmeasurable and irrelevant to current benchmarks.

EXP34-C (+76 FP): Original regression was noise violations (EXP34-C
firing on non-CWE-matched CWEs in the old full benchmark). Fast-mode
CWE-matched manifests mean EXP34-C only fires on CWE-476/CWE-690.
Noise violations don't exist in fast mode.

Since v0.3.37: CWE-476 gained +216 TP, CWE-690 gained +317 TP.
Both regressions are from v0.2.17 (30+ versions ago, pre-fast-mode).
Closed as stale.

---

# Task ID: 42
# Title: EXP34-C variant 68 — cross-file global null tracking
# Status: done
# Dependencies: 7
# Priority: P2
# Description: Detect null pointer dereferences through cross-file global variables.
# Details:
Implemented cross-file global pointer null state tracking.

Changes:
  context.rs: Added `global_var_null_states: HashMap<String, NullState>` to
    ProjectContext. Serde-compatible for prescan cache.
  prescan.rs: Added `collect_global_var_null_states()` — scans file-scope
    non-static, non-extern pointer globals and function-body assignments.
    Handles `data = NULL; globalVar = data;` relay pattern. Called during
    prescan_directories() for each .c file.
  exp34_c.rs: Added `prescan_global_var_states` field, populated via
    set_project_context(). After collect_file_scope_null_states(), merges
    prescan states for extern pointer declarations via
    merge_extern_global_states().

Smoke test results (all 6 data types × variant 68):
  68b.c (sink): 6/6 new TPs — badSink correctly flagged for null deref
  68a.c (source): 0/6 FPs — no dereferences in source files
  goodG2BSink: 0 FPs — globalData assigned string literal (NotNull)
  goodB2GSink: 0 FPs — null guard detected by CFG dataflow

All 3004 existing tests pass, zero regressions.

---

# Task ID: 43
# Title: EXP34-C relay chain depth (3+ hops)
# Status: done
# Dependencies: 7
# Priority: P3
# Description: Extend prescan relay propagation beyond single-hop to handle
  deep call chains.
# Details:
Refactored propagate_param_null_states() from single-pass to iterative
with convergence detection.

MAX_PROPAGATION_PASSES = 3 (resolves up to 3-hop relay chains). Each pass:
  1. Snapshots current param null states
  2. Re-parses all source files with param state seeding
  3. Merges new callsite args and re-aggregates
  4. Checks convergence (all states unchanged → early exit)

Performance: Juliet CWE-476 (372 files) completes prescan in 1.7s total.
Convergence is fast — most codebases converge in 1-2 passes since deep
relay chains are rare. No measurable slowdown.

v0.3.53 benchmark results:
  CWE-690: +40 TP, 0 FP (93.2% → 93.6%). Confirms deep relay chains
    existed — iterative propagation resolved them cleanly.
  CWE-476: +18 TP, -6 FP (45.5% → 47.2%). Combined effect with task 42.
  Realworld: EXP34-C +159 new detections across 5 codebases.
    API00-C -147 FP (better param state resolution benefits suppression).
  Duration: Juliet 29 min, realworld 42 min — unchanged from v0.3.52.

---

# Task ID: 44
# Title: EXP34-C variants 63-67 — indirect data flow
# Status: in-progress
# Dependencies: 7
# Priority: P3
# Description: Cross-file null propagation through indirect data flow mechanisms.
# Details:
Each variant requires a distinct new prescan capability:

Variant 67 (struct field): DONE in v0.3.55. NULL in struct field, struct
  passed by value to sink. Implemented via callsite_param_field_null_states
  in FunctionSummary. Prescan tracks "var.field" assignments in local_states,
  propagates through call arguments, aggregates per-function. Null state
  analysis seeds "paramName.fieldName" into initial_state, transfer function
  checks dotted key on field_expression RHS.
  +6 TP, 0 FP across all 6 data types. 2 new regression tests.

Variant 63 (pointer-to-pointer): DONE in v0.3.56. Caller passes &data where
  data=NULL. Sink receives int **dataPtr, dereferences *dataPtr. Implemented
  via callsite_param_pointee_null_states in FunctionSummary. Prescan detects
  &var arguments, looks up var in local_states, aggregates pointee states.
  Null state analysis seeds "*paramName" into initial_state, transfer function
  propagates *ptr dereference state.
  +6 TP, 0 FP across all 6 data types. 2 new regression tests.

Variant 64 (void pointer): DONE in v0.3.56. Same as 63 but sink takes void*
  and casts to int**. Added cast_expression propagation for "*" state keys
  and parenthesized_expression unwrapping for (*dataPtr) form. Reuses
  variant 63 pointee tracking.
  +6 TP, 0 FP across all 6 data types. 2 new regression tests.

Variant 65 (function pointer): Call through function pointer. Needs indirect
  call resolution in prescan call_graph. Currently only direct calls tracked.
  Not worth the effort — requires function pointer analysis.

Variant 66 (array element): DONE in v0.3.56. NULL stored in array element,
  array passed to sink. Reuses struct field dotted-key mechanism: prescan
  tracks "arr.idx" keys from subscript_expression assignments. Transfer
  function handles subscript_expression lookups in both declarations and
  assignments.
  +6 TP, 0 FP across all 6 data types. 2 new regression tests.

12 files remaining (variant 65 only — function pointer, deferred).

---

# Task ID: 45
# Title: EXP34-C Phase 4 regression tests
# Status: done
# Dependencies: 41, 42, 43
# Priority: P2
# Description: Add regression tests for all Phase 4 improvements.
# Details:
8 new unit tests (47 → 55 total EXP34-C tests):

fail/ (4 new):
  testcases_global_null_deref.c — file-scope global = NULL, deref without check
  testcases_global_null_assign_deref.c — global assigned NULL in function, deref later
  testcases_relay_null_to_callee.c — local var NULL relay to callee (prescan, 1-hop)
  testcases_relay_two_hop.c — NULL through relay function chain (prescan, 2-hop)

pass/ (4 new):
  testcases_global_null_guard.c — global = NULL but checked before deref
  testcases_global_string_literal.c — global = string literal (NotNull)
  testcases_relay_null_guard_callee.c — NULL relayed but callee has early-return guard
  testcases_relay_nonnull.c — address-of local relayed to callee (NotNull)

3 new CLI integration tests (cross-file variant 68):
  crossfile_global_null_deref_detected_with_d_flag — source.c defines global=NULL,
    sink.c has extern + deref. With -d flag, violation detected.
  crossfile_global_null_guard_not_flagged — sink_safe.c has null check, no violation.
  crossfile_global_null_not_detected_without_d_flag — without -d, no cross-file
    context, no violation.

Task 41 (FP regressions) confirmed stale — no regression tests needed.

---

# Task ID: 40
# Title: Realworld benchmark duration tracking
# Status: done
# Dependencies: none
# Priority: P2
# Description: Record per-project wall-clock duration in realworld benchmark runs.
# Details:
Done. _auto_ingest_to_sqlite() now reads per-codebase durations from the
state file (start_time/end_time per run) and passes them to
ingest_realworld_run(). Surfaced in get_results(), get_project_history(),
and the new get_dashboard() MCP tool. Historical runs remain NULL (no
retroactive timing data). New runs will populate duration_s automatically.

---

# Task ID: 46
# Title: Paper — runtime performance data for all 5 codebases
# Status: done
# Dependencies: none
# Priority: P1
# Description: Collect wall-clock scan times for all 5 codebases to
  complete runtime scaling table in paper/sqc.tex.
# Details:
All 5 codebases measured on M1 in single-process mode (cross-file analysis,
cold cache). Paper updated with tab:runtime-scaling table and prose explaining
sqlite amalgamation outlier.

Single-process cold-cache measurements (v0.3.55, sqlite 404,701 LOC):
  M1 Xeon E5-2640 @ 2.50GHz, 6c/24t, 192GB, SSD:  57m50s (3470s), 117 LOC/s.
  M2 Ryzen 5 PRO 2400GE @ 3.20GHz, 4c/8t, 16GB, SSD:  51m56s (3116s), 130 LOC/s. Warm: 51m43s (-13s).
  M3 i7-7700T @ 2.90GHz, 4c/8t, 16GB, SSD:  39m23s (2363s), 171 LOC/s. Warm: 39m10s (-13s).
  M4 i7-9750H @ 2.60GHz, 6c/12t, 16GB, HDD: 33m06s (1986s), 204 LOC/s. Warm: 32m59s (-7s).
  M1 warm-cache (--load-prescan): 57m43s (3463s), 117 LOC/s. Delta: -7s (0.2%).
    Prescan cache 3.1MB. Negligible savings in single-process mode —
    prescan is fast relative to per-file analysis of sqlite3.c amalgamation.
  M1 parallel cold (8 workers, 19 units): 31m39s wall (73m00s CPU), 213 LOC/s.
  M1 parallel warm (cached prescan):      17m36s wall (58m57s CPU), 383 LOC/s.
    Parallel warm = 3.3x single-process. Prescan cache saves 14m in parallel
    mode by eliminating the prescan generation step.

---

# Task ID: 47
# Title: Paper — real bug case study
# Status: pending
# Dependencies: 46
# Priority: P2
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
# Status: pending
# Dependencies: 33
# Priority: P3
# Description: Run Infer and Frama-C on the same Juliet CWE subset to get
  comparable TP/FP numbers for the paper.
# Details:
Currently paper/sqc.tex Related Work discusses Infer and Frama-C qualitatively.
A direct comparison table (tool × TP × FP × TP rate × CWEs covered) would
significantly strengthen the evaluation.

Scope: run on overlapping CWEs only (null deref, use-after-free, resource leaks
for Infer; same + integer overflow for Frama-C Eva).  Don't need full 70-CWE
coverage — even 5-10 CWEs with head-to-head data is valuable.

Paper impact: new table in Section 5 (Results), new comparison graph.

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
# Title: Paper — address 10 zero-detection CWEs
# Status: pending
# Dependencies: 11
# Priority: P3
# Description: Develop rules for highest-value zero-detection CWEs to
  demonstrate continued improvement in paper revisions.
# Details:
Paper Limitations section (Section 8) notes 10 CWEs with zero detection.
Highest value targets by file count:

  - CWE-789 (560 files): Uncontrolled Memory Allocation.  Needs taint tracking
    for user input → malloc size.  Medium-high effort.
  - CWE-114 (672 files): Process Control.  Needs taint tracking for untrusted
    input → LoadLibrary.  Medium-high effort.
  - CWE-468 (36 files): Incorrect Pointer Scaling.  AST pattern for implicit
    void* casts.  Low effort, but low file count.
  - CWE-459 (36 files): Incomplete Cleanup.  Resource tracking for cleanup
    handlers.  Medium effort.

Each CWE resolved reduces the zero-detection count in the paper and
demonstrates coverage expansion capability.

---

# Task ID: 51
# Title: Paper — finalization and submission
# Status: pending
# Dependencies: 46, 47
# Priority: P1
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

# Task ID: 52
# Title: Man page for sqc
# Status: pending
# Dependencies: none
# Priority: P2
# Description: Create a man page (sqc.1) documenting CLI usage, options, and examples.
# Details:
Write sqc.1 in roff format covering: synopsis, all CLI flags (--rules, --diff,
--fail-on-violation, --fail-on-severity, -d, -I, --save-prescan, --load-prescan,
--export, --format, --min-severity, --suppress-file, --generate-suppression),
configuration file format, exit codes, examples, and environment variables.
Install via Cargo build script or Makefile. Include in deb/rpm packages (task 53).

---

# Task ID: 53
# Title: Binary distribution packages
# Status: pending
# Dependencies: none
# Priority: P3
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


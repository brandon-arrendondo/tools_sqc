# SqC — Plans & Roadmap

Last Updated: 2026-03-30 (v0.3.47)

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
# Status: pending
# Dependencies: 19
# Priority: P2
# Description: Inter-procedural analysis for EXP33-C variants 63/64.
# Details:
Pointer passed between source files needs inter-procedural analysis. ~70 files
in CWE-457 affected. Same blocker as CWE-457 cross-function gaps (task 4).
EXP33-C already has CFG-based forward dataflow (init_state.rs) but it is
intra-procedural only.

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
# Status: pending
# Dependencies: 2
# Priority: P2
# Description: Improve CWE-457 TP rate beyond 35.3%.
# Details:
v0.3.37: 165 TP, 302 FP, 35.3% TP rate (up from 32.2% in v0.3.34).

Remaining gaps:
- Cross-function variants 63/64 (~70 files): needs inter-procedural analysis
  (same as task 2)
- Per-element tracking for stack arrays: team[0].x = 1; use(team[3].x)
  correctly flags, but no way to track that ALL elements are initialized
- 302 FP likely dominated by cross-function initialization patterns in Juliet
  "good" functions

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
# Status: pending
# Dependencies: none
# Priority: P2
# Description: Address remaining EXP34-C inter-procedural gaps.
# Details:
- Relay chains (3+ hops): multi-pass handles single-hop, deep chains still
  Unknown
- Indirect data flow (variants 63-67): not addressed
- Cross-file globals (variant 68): not addressed
- EXP34-C/FIO06-C regression investigation from Phase 3 (prescan enhancement
  caused +76 FP EXP34-C, +169 FP FIO06-C)

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
# Status: pending
# Dependencies: none
# Priority: P2
# Description: Add regression tests for FP reduction work not yet covered.
# Details:
Rounds 8-11 have regression tests. Still needed:
- Round 3: std function database lookups should not trigger DCL31-C/DCL07-C
- Round 6: cross-file function definitions should not trigger DCL31-C/DCL07-C
- Round 7: unknown-type pointer casts should not trigger EXP36-C

---

# Task ID: 15
# Title: Expand wiki-only rule test coverage
# Status: pending
# Dependencies: none
# Priority: P2
# Description: Add tests for ~105 rules with only 2 test files each.
# Details:
Prioritize top-FP rules from real-world benchmarks:
- EXP33-C (6,100 real-world violations) — only 10 wiki tests
- EXP34-C (5,267) — 6 wiki tests
- ERR33-C (989) — 11 wiki tests

---

# Task ID: 16
# Title: CLI integration tests
# Status: pending
# Dependencies: none
# Priority: P2
# Description: Test CLI flags: --diff, --export json/csv/sarif, -I, -d,
  --save-prescan/--load-prescan, suppression.
# Details:
No tests currently exist for any CLI flags. Need integration tests that invoke
sqc as a subprocess and verify output format, exit codes, and behavior.

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
Added Pattern 4 to is_properly_handled(): rename() with return value check
is acceptable (POSIX atomically replaces destination). Updated fail test to
bare rename() without error checking. Added pass/testcases_posix_rename.c.

---

# Task ID: 19
# Title: EXP34-C intra-file call-site test infrastructure
# Status: done
# Dependencies: none
# Priority: P2
# Description: Enable test infra to exercise intra-file prescan for EXP34-C.
# Details:
Implemented `// sqc-test: prescan` marker system for .c test files:
- Added `prescan_single_tree()` to prescan.rs (single-file prescan wrapper)
- Made `collect_function_cfgs()` public in analyze/mod.rs
- build.rs detects marker, generates tests with prescan context + CFGs
- Moved 3 tests from pass/ to fail/ (func_param, list_null, callback_null)
- All 3 now correctly detect violations via call-site null propagation
- Infrastructure is generic — any rule can use `// sqc-test: prescan`

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
# Description: 3 remaining implementation bugs (3 of 6 fixed).
# Details:
Fixed:
- MEM04-C: sizeof() now recognized as always non-zero in is_potentially_zero()
- FIO10-C: POSIX rename() with error checking now accepted (task 18)
- WIN30-C: Reclassified — NULL security attributes is out of scope for
    WIN30-C (alloc/dealloc pairing). Test comment updated.

Remaining:
- STR03-C: strncpy with prior strlen validation still flagged (Low)
- INT00-C: unsigned subtraction without guard not detected (Medium,
    tests/pass/testcases_unsigned_wrap.c — pattern mismatch)
- INT16-C: signed-to-unsigned conversion without range check not detected
    (Medium, tests/pass/testcases_signed_unsigned_conversion.c — pattern mismatch)

Note: INT00-C and INT16-C are pattern mismatches — the rules check different
patterns than what the tests expect. May require rule redesign.

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

# Task ID: 26
# Title: Baseline-aware suppression
# Status: pending
# Dependencies: none
# Priority: P2
# Description: "Only new violations" mode for CI integration.
# Details:
Store a baseline of known violations and only report new ones. Critical for
adopting sqc on existing codebases without noise from pre-existing issues.
Part of Tier 2 production quality definition of done.

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
# Status: pending
# Dependencies: none
# Priority: P2
# Description: Code after return/exit/abort, always-false branches (BRULE-034 gap).
# Details:
BRULE-034 requires no dead code. MSC12-C (v0.3.43) covers no-effect statements
and duplicate conditions. MSC07-C covers unreachable code: statements after
return/exit/abort, always-false branches. Needs CFG infrastructure (already
exists). Warning severity. Complements existing EXP12-C.

---

# Task ID: 29
# Title: Recursion detection (BRULE-058)
# Status: pending
# Dependencies: none
# Priority: P2
# Description: Detect recursive function calls via call-graph cycle detection.
# Details:
BRULE-058 (Constrained tier) prohibits recursion. Requires call-graph
construction from prescan data (already collects function names and call sites).
Detect cycles in the call graph. Both direct (f calls f) and indirect (f calls
g calls f) recursion.

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
# Status: in-progress
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
# Status: pending
# Dependencies: none
# Priority: P2
# Description: Add test cases for FLP03-C guard detection and conversion removal.
# Details:
v0.3.44 removed overbroad check_fp_conversion() and added division guard
detection (fabs/fabsf/fabsl, != 0, > 0 / < 0). Need tests:

- fail/division_no_guard.c: FP division without any guard or fenv checking
- fail/division_ge_zero.c: Division inside `if (x >= 0)` (does NOT exclude zero)
- pass/division_fabs_guard.c: Division inside `if (fabs(x) > 0.000001)`
- pass/division_ne_zero.c: Division inside `if (x != 0)`
- pass/division_gt_zero.c: Division inside `if (x > 0)`
- pass/division_fenv.c: Division with feclearexcept/fetestexcept (already exists as wiki_c.c)
- pass/conversion_only.c: Cast to float/double without division (should not flag)

---

# Task ID: 40
# Title: Realworld benchmark duration tracking
# Status: pending
# Dependencies: none
# Priority: P2
# Description: Record per-project wall-clock duration in realworld benchmark runs.
# Details:
The realworld_results table has a duration_s column but it is always 0 for
all tools (sqc, cppcheck, clang-tidy). The MCP server runner
(mcp_servers/server.py or bench/) needs to time each subprocess and write
the elapsed seconds into duration_s when inserting results. This would
enable tracking performance regressions across versions and understanding
which codebases (hostap, sqlite) dominate wall-clock time.


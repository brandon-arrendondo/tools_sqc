# SqC — Plans & Roadmap

Last Updated: 2026-04-02 (v0.3.67)

Juliet benchmark v0.3.67: 25,552 TP / 22,407 FP (53.3% TP rate), 42.4% per-file.
v0.3.67 vs v0.3.63: +1,144 TP, +1,290 FP (+2,434 total). Per-file +1.5pp.
CWE-190 per-file 35.0%→43.3%, CWE-191 per-file 40.5%→47.7%.

For completed work, see CHANGELOG.txt.
For benchmark data, see JULIET_RESULTS.md and REALWORLD_RESULTS.md.
For competitor research and academic references, see docs/bibliography.rst.

Default test strategy for all tasks: pre-commit hooks (cargo test + cargo fmt),
then Juliet benchmark and real-world benchmark to validate.

---

# Task ID: 5
# Title: CWE-190/191 remaining coverage gaps
# Status: pending
# Dependencies: none
# Priority: P3
# Description: Address remaining CWE-190/191 detection gaps after v0.3.67 fixes.
# Details:
v0.3.67 raised CWE-190 per-file from 35.0% to 43.3% and CWE-191 from
40.5% to 47.7%. Remaining undetected patterns:

  - rand() source: suppressed by is_small_increment_of_opaque (deliberate
    FP heuristic for strlen()+1 patterns). Fixing would hurt real-world FP.
  - Cross-function variants (41-68): need inter-procedural VRA to propagate
    value ranges through function arguments and return values.
  - Conditional flow variants (12-18): syntactic resolver can't follow
    branches; VRA handles some but not all.

Further improvement requires inter-procedural VRA or relaxing the opaque
increment heuristic (with real-world FP impact analysis).

---

# Task ID: 6
# Title: CWE-690 per-file detection improvement
# Status: pending
# Dependencies: 7
# Priority: P2
# Description: Raise CWE-690 per-file detection rate from 18.1% toward 30%.
# Details:
v0.3.37: 203 TP, 12 FP, 94.4% TP rate, 18.1% per-file. Best precision of any
high-volume CWE. 74% undetected are likely cross-function patterns. Improving
per-file rate depends on EXP34-C Phase 4 (task 7) for deeper inter-procedural
null propagation.

---

# Task ID: 34
# Title: Per-file detection >= 30% on top 10 CWEs
# Status: pending
# Dependencies: 32
# Priority: P2
# Description: Tier 3 competitive milestone — per-file detection rate.
# Details:
Per-file detection measures whether at least one TP is found per Juliet test
file. Current rates vary widely. Improving requires better cross-function
analysis for variants that span multiple functions within a file.

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
# Status: pending
# Dependencies: 19
# Priority: P3
# Description: Multi-file C test cases for prescan/call-site propagation.
# Details:
Need test infrastructure that can compile and analyze multiple .c files
together to exercise prescan context, cross-file function resolution, and
call-site null state propagation.

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

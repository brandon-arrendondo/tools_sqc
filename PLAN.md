# SqC — Plans & Roadmap

Last Updated: 2026-04-05 (v0.3.85)

Juliet benchmark v0.3.85: 25,481 TP / 16,311 FP (61.0% TP rate), 42.6% per-file.

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

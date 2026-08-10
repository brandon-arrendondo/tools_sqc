# MEM31-C delta-adjudication (task 420) — COMPLETE

Source: v0.4.176's 21-rule architectural-sweep batch (tasks 397/403-417)
surfaced a large jump in raw MEM31-C findings, most of it at (file, line)
pairs never adjudicated before. This tracks the delta-adjudication pass to
label those specific new findings against `sqc realworld` run **#145**
(v0.4.176, commit a2599ac6), reusing the batch/adjudicate/import workflow
already established for the hostap/curl/sqlite/mosquitto/lua/raylib oracles.

Generated via `bench realworld-unlabeled 145 --rule MEM31-C --project <p>
--json`, then packed into per-file batches (~110-150 findings, `#partNofM`
split for oversized files) with the same schema as the existing
`batches/*.json` files in each project directory.

## Scope

Raw unlabeled counts pulled via `realworld-unlabeled` (whole-repo scan)
vs. the in-scope counts actually batched, after applying each project's
own evaluated-scope exclusions (see caveat below):

| Project   | Raw unlabeled | Dropped (out-of-scope) | In-scope batched | Batches |
|-----------|---------------|-------------------------|-------------------|---------|
| hostap    | 528           | 42 (tests/wlantest/radius_example/wpaspy) | 486 | 4 (`delta_mem31_b1..4.json`) — **done** |
| curl      | 796           | 349 (tests/docs/projects/OS400 + 14 WIN_MAC) | 447 | 4 (`delta_mem31_b1..4.json`) — **done** |
| sqlite    | 1,264         | 1,115 (tool/test/autosetup/mptest/ext-jni-wasm + `src/tclsqlite.c` (592!) + any `ext/**/*test*.c`) | 149 | 1 (`delta_mem31_b1.json`) |
| mosquitto | 1,421         | 1,042 (test/plugins/apps/client/libcommon/fuzzing/examples) | 379 | 3 (`delta_mem31_b1..3.json`) — **done** |
| raylib    | 15            | 0 (all 3 files are core/platform-backend, in scope) | 15 | 1 (`delta_mem31_b1.json`) — **done** |
| lua       | 2             | 0 (lmem.c is core interpreter source) | 2 | 1 (`delta_mem31_b1.json`) — **done** |
| **Total** | **4,026**     | **2,548**                | **1,478**         | **14** |

The originally-estimated ~2,244 (task 420) and the first-pass 4,026 raw
count both badly overstated the real scope: **2,548 of 4,026 (63%) were
findings in directories the corresponding project's own oracle already
excludes** (test harnesses, vendored deps, docs/examples, non-Linux build
configs, language bindings). The true delta-adjudication workload is
~1,478 findings, not 4,026 — and sqlite alone accounts for most of the
correction: a first-pass "fix" caught `tool/`/`test/`/`autosetup/`/
`mptest/`/`src/test*.c`/`ext/jni`/`ext/wasm` but missed `src/tclsqlite.c`
(592 findings — the Tcl language binding, explicitly out-of-scope per
sqlite's own README) and four `ext/**/test_*.c`/`*_speed_test.c` glue
files, only caught on a second pass before sqlite adjudication started.

## Scope caveat — check exclusions BEFORE batching, not after

The `realworld-unlabeled` source query pulls from the standing benchmark
run, which scans whole repo roots with no `--exclude` for several projects
(paper \S\ref{sec:sloc-scope} / `tab:eval-scope` notes this same gap).
Every project's initial batch generation here was contaminated with
out-of-scope findings until caught and fixed:
- **hostap**: `tests/`, `wlantest/`, `radius_example/`, `wpaspy/` (42
  findings) — caught only after b1/b2 were already adjudicated; b1 has one
  negligible leftover `radius_example.c` row, not worth unwinding.
- **curl**: not just the 14 WIN_MAC files (24 findings) but also
  `tests/`, `docs/`, `projects/OS400/*` (349 findings total) — the first
  fix only caught WIN_MAC; caught the rest on a second pass, before any
  curl adjudication started.
- **sqlite**: two passes needed. First pass caught `tool/`, `test/`,
  `autosetup/`, `mptest/`, `src/test*.c`, `ext/jni`, `ext/wasm` (503
  findings). Second pass, prompted by noticing `ext/expert/test_expert.c`
  in a generated batch and cross-referencing the README's explicit
  "excluded: test_rtreedoc.c, test_expert.c, fts5_tcl.c" note, caught
  `src/tclsqlite.c` (592 findings — by far the single biggest
  contamination source in this whole pass, the Tcl binding, explicitly
  out-of-scope per README) plus `ext/expert/test_expert.c`,
  `ext/intck/test_intck.c`, `ext/session/test_session.c`,
  `ext/session/session_speed_test.c` (20 more, all Tcl/perf-test glue
  confirmed by reading each file's own header comment: "not included in
  the SQLite library" / "testing the performance of"). Checked
  `ext/session/changesetfuzz.c` and `ext/misc/fuzzer.c` against this same
  "test"-in-name suspicion and found the opposite: both have substantial
  pre-existing ground-truth labels (134 and 106 respectively), i.e.
  established in-scope precedent despite the fuzz-sounding names — did
  NOT exclude them. Total: 1,115 dropped, not 503 (real in-scope count is
  149, not 761) — caught before any sqlite adjudication started.
- **mosquitto**: `test/`, `plugins/`, `apps/`, `client/`, `libcommon/`,
  `fuzzing/`, `examples/` (1,042 findings — 73% of the raw count!) —
  caught before adjudication started.
- **lua, raylib**: checked, no contamination (both are small enough that
  every finding's file was verified against the project's scope by hand).

**Lesson for any future delta batch**: derive the in-scope file predicate
directly from the project's `README.md`/paper `tab:eval-scope` prefix list
*before* running `realworld-unlabeled`, not after generating batches — this
would have caught mosquitto's 73% contamination on the first pass instead
of requiring a redo.

## Status

**Complete.** All 6 projects adjudicated and imported (14 batches, ~1,478
findings). Per-project outcome:

| Project   | Findings | TP | FP | Notes |
|-----------|----------|----|----|-------|
| hostap    | 486      | 8  | 478 | genuine `hostapd/main.c` `bss_config` leak (3 early-return paths skip `out:`) + `crypto_wolfssl.c` asymmetric dual-allocation guard |
| curl      | 447      | 0  | 447 | **0% precision** — clean sweep of the enum/status-typed-local misfire (task 425) plus curl-specific ownership/free-pairing/alias classes |
| sqlite    | 149      | 0  | 149 | 0% — new borrowed-accessor class found (task 427, `sqlite3_column_blob` misread as an allocation) despite deliberately targeting sqlite's historically-buggy less-fuzzed extension code |
| mosquitto | 379      | 2  | 377 | genuine `http_api.c` calloc-failure leak + `mosquitto.c` `WinMain` realloc-into-same-var leak (both narrow, OOM-triggered); possible MEM31-C double-free cross-frame scoping bug filed as task 426 |
| raylib    | 15       | 0  | 15  | global struct field (glfw) + enum-status-type (miniaudio) misfires |
| lua       | 2        | 0  | 2   | ownership-transfer-via-return-value |
| **Total** | **1,478**| **10** | **1,468** | **0.7% precision on this delta** |

Follow-up tasks filed from patterns found during this pass:
- **task 425** — MEM31-C's goto-cleanup heuristic doesn't check that the
  assigning callee actually returns a pointer (enum/int/u16/bool locals
  misattributed as allocations); the single largest FP driver across every
  project (dominant in curl/hostap/sqlite, present in mosquitto/raylib/lua).
- **task 426** — possible cross-call-frame/recursion double-free tracking
  bug in MEM31-C, analogous to the CON30-C/POS53-C/STR32-C per-function-
  scoping fixes (tasks 415-417); found in mosquitto's `subs.c`.
- **task 427** — MEM31-C doesn't exclude const-qualified/borrowed-accessor
  return values (e.g. `sqlite3_column_blob`) from its allocation heuristic;
  31% of the sqlite batch.

Every project's `categorical_patterns.md` (hostap, curl, mosquitto, sqlite —
lua/raylib were small enough to skip a dedicated file) documents the
codebase-specific FP classes found, for reuse by any future audit pass on
these rules/projects. All CSVs live alongside the batch files in each
project's `data/precision_audit/<project>/import_delta_mem31_b*.csv`.

# MEM31-C delta-adjudication (task 420) — scaffolding only, NOT adjudicated

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
| curl      | 796           | 349 (tests/docs/projects/OS400 + 14 WIN_MAC) | 447 | 4 (`delta_mem31_b1..4.json`) |
| sqlite    | 1,264         | 503 (tool/test/autosetup/mptest + src/test*.c + ext/jni,wasm) | 761 | 7 (`delta_mem31_b1..7.json`) |
| mosquitto | 1,421         | 1,042 (test/plugins/apps/client/libcommon/fuzzing/examples) | 379 | 3 (`delta_mem31_b1..3.json`) |
| raylib    | 15            | 0 (all 3 files are core/platform-backend, in scope) | 15 | 1 (`delta_mem31_b1.json`) |
| lua       | 2             | 0 (lmem.c is core interpreter source) | 2 | 1 (`delta_mem31_b1.json`) |
| **Total** | **4,026**     | **1,936**                | **2,090**         | **20** |

The originally-estimated ~2,244 (task 420) and the first-pass 4,026 raw
count both overstated the real scope: **1,936 of 4,026 (48%) were findings
in directories the corresponding project's own oracle already excludes**
(test harnesses, vendored deps, docs/examples, non-Linux build configs,
language bindings). The true delta-adjudication workload is ~2,090
findings, not 4,026.

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
- **sqlite**: `tool/`, `test/`, `autosetup/`, `mptest/`, `src/test*.c`,
  `ext/jni`, `ext/wasm` (503 findings) — caught before adjudication started.
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

**In progress.** hostap (486 findings, all 4 batches, 10 TP / 476 FP) and
curl (447 findings, all 4 batches, **0 TP / 447 FP — 0% precision**) done
and imported (task 420 commits). curl's delta is a clean sweep of the
enum/status-typed-local misfire (task 425) plus several codebase-specific
ownership-transfer/free-pairing/alias FP classes now documented in
`data/precision_audit/curl/categorical_patterns.md`; no new bugs found.
sqlite/mosquitto/lua/raylib batches are correctly scoped (see table above)
but not yet adjudicated — do not treat `bench ground-truth` MEM31-C rows
for those projects as covering the delta yet. Next step per project:
adjudicate each `delta_mem31_b*.json` batch (read the actual source at the
pinned commit, judge TP/FP per the file's existing `categorical_patterns.md`
conventions where one exists; do NOT split into sub-agent "groups" that
message each other — that stalled hostap batch 3, see its commit), write
an `import_delta_mem31_bN.csv`, then `bench realworld-import-labels --run
145 --source delta_mem31_task420 <csv>`.

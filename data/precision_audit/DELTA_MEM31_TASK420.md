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

| Project   | Unlabeled MEM31-C findings | Files | Batches |
|-----------|----------------------------|-------|---------|
| hostap    | 528                         | 69    | 4 (`delta_mem31_b1..4.json`) |
| mosquitto | 1,421                       | 189   | 11 (`delta_mem31_b1..11.json`) |
| sqlite    | 1,264                       | 92    | 9 (`delta_mem31_b1..9.json`) |
| curl      | 796                         | 145   | 6 (`delta_mem31_b1..6.json`) |
| raylib    | 15                          | 3     | 1 (`delta_mem31_b1.json`) |
| lua       | 2                           | 1     | 1 (`delta_mem31_b1.json`) |
| **Total** | **4,026**                   | 499   | **32** |

This is larger than task 420's original ~2,244 estimate (written when only
hostap's skew had been sampled); the full delta across all six oracle
projects is now measured at 4,026.

## Scope caveat (found during hostap b3/b4 regeneration; recurred on curl)

The `realworld-unlabeled` source query pulls from the standing benchmark run,
which scans whole repo roots with no `--exclude` (paper \S\ref{sec:sloc-scope}
/ `tab:eval-scope` notes this same gap for several projects). hostap's
initial batch generation included findings from `tests/`, `wlantest/`,
`radius_example/`, and `wpaspy/` — excluded by the hostap oracle's own
evaluated scope (`data/precision_audit/hostap/README.md`); `b3.json`/`b4.json`
were regenerated to drop 42 out-of-scope findings (`b1.json`, already
adjudicated, slipped in one negligible `radius_example.c` row, not unwound).
curl's batches had the same issue: 24 findings from the 14 WIN_MAC files
(`lib/vtls/schannel*.c/.h`, `lib/vtls/apple.*`, `lib/system_win32.*`,
`lib/curlx/{winapi,version_win32,multibyte}.*` — see
`data/precision_audit/curl/README.md`) were in `b4.json` pre-adjudication;
all 6 curl batches were regenerated from scratch to drop them before any
curl adjudication started, so no re-work was needed there. **Before
adjudicating sqlite/mosquitto/lua/raylib batches, check their batch file
lists against each project's `tab:eval-scope` exclusion criteria in the
paper and drop out-of-scope files first**: sqlite's test/tooling/WASM/JNI
bindings, mosquitto's deps/test/plugins, lua's `ltests.*`.

## Status

**In progress.** hostap (all 4 batches, 528 findings) adjudicated and
imported — done (task 420 commits). curl batches regenerated to drop
WIN_MAC contamination, not yet adjudicated. sqlite/mosquitto/lua/raylib
batches generated but unreviewed for scope contamination and not yet
adjudicated — do not treat `bench ground-truth` MEM31-C rows for those
projects as covering the full delta yet. Next step per project: adjudicate
each `delta_mem31_b*.json` batch (read the actual source at the pinned
commit, judge TP/FP per the file's existing `categorical_patterns.md`
conventions where one exists), write an `import_delta_mem31_bN.csv`, then
`bench realworld-import-labels --run 145 --source delta_mem31_task420
<csv>`.

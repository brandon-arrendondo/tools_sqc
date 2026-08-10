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

## Scope caveat (found during hostap b3/b4 regeneration)

The `realworld-unlabeled` source query pulls from the standing benchmark run,
which scans hostap's **whole repo root** with no `--exclude` (paper
\S\ref{sec:sloc-scope} / `tab:eval-scope` notes this same gap). The initial
batch generation therefore included findings from `tests/`, `wlantest/`,
`radius_example/`, and `wpaspy/` — directories the hostap oracle's own
evaluated scope (`data/precision_audit/hostap/README.md`) explicitly
excludes. `delta_mem31_b3.json`/`b4.json` were regenerated to drop these 42
out-of-scope findings before adjudication; `delta_mem31_b1.json` (already
adjudicated) slipped in exactly one (`radius_example/radius_example.c:...`,
negligible, not worth unwinding). **Before adjudicating curl/sqlite/
mosquitto/lua/raylib batches, check their batch file lists against each
project's `tab:eval-scope` exclusion criteria in the paper and drop
out-of-scope files first** — same class of contamination is possible there
(curl's 14 Win/macOS files, mosquitto's deps/test/plugins, sqlite's
test/tooling/WASM/JNI bindings, lua's `ltests.*`).

## Status

**In progress.** hostap batches 1-2 adjudicated and imported (task 420
commits); batches 3-4 regenerated to fix the scope contamination above, not
yet adjudicated. curl/sqlite/mosquitto/lua/raylib batches generated but
unreviewed for scope contamination and not yet adjudicated — do not treat
`bench ground-truth` MEM31-C rows for those projects as covering the full
delta yet. Next step per project: adjudicate each `delta_mem31_b*.json`
batch (read the actual source at the pinned commit, judge TP/FP per the
file's existing `categorical_patterns.md` conventions where one exists),
write an `import_delta_mem31_bN.csv`, then `bench realworld-import-labels
--run 145 --source delta_mem31_task420 <csv>`.

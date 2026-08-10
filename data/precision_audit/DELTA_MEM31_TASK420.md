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

## Status

**Not started.** Batches only, generated 2026-08-09. No findings have been
adjudicated yet — do not treat `bench ground-truth` MEM31-C rows as covering
these. Next step per project: adjudicate each `delta_mem31_b*.json` batch
(read the actual source at the pinned commit, judge TP/FP per the file's
existing `categorical_patterns.md` conventions where one exists), write
`results/delta_mem31_bN.result.json` or an `import_delta_mem31.csv`, then
`bench realworld-import-labels --run 145 --source delta_mem31_task420
<csv>`. Prioritize hostap first (smallest, and the case that originally
surfaced this task), then curl/sqlite/mosquitto by volume.

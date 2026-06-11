# sqlite ground-truth audit (incremental) — codebase commit b1a73ba34d

sqlite is the hardest real-world target: ~354 C/H files and **39,747 distinct
sqc findings** (run 34, base manifest). A full per-finding sweep like libcrc is
infeasible, so this oracle is built **incrementally** (sample → read the actual
code → label → import), growing over time. Config: `conf/realworld/sqlite-rules.toml`.

## Precision scope (what counts as "sqlite")

The oracle measures precision on the **shipped database engine + extensions**,
not the surrounding repository. In-scope vs out-of-scope of the 39,747 findings:

| Scope | Findings | Trees |
|-------|----------|-------|
| **In-scope** | 24,965 (63%) | `src/` core engine, `ext/` shipped extensions (fts3/4/5, rtree, session, …) |
| Out-of-scope | 14,782 (37%) | `autosetup/jimsh0.c` (vendored Jim Tcl), `tool/` (lemon parser-gen, build tools), `test/` + `src/test*.c` (Tcl test glue), `ext/jni` + `ext/wasm` (language bindings) |

Out-of-scope code is not labelled; precision is measured over the in-scope
labelled subset. (In a real engagement you audit what you ship and run in
production, not the vendored interpreter or the build tooling.)

## In-scope rule volume (top, run 34)

    INT32-C 4537   MEM30-C 2384   DCL13-C 2070   EXP34-C 1537   API00-C 1377
    INT30-C  940   EXP33-C  692   INT14-C  631   ARR00-C  627   INT13-C  624

Five rules (INT32, MEM30, DCL13, EXP34, API00) are ~half of in-scope findings —
the priority targets for adjudication.

## Categorical config disables (grow as the audit justifies them)

- **DCL05-C** — "complex function-pointer declaration should use typedef":
  advisory; sqlite deliberately uses inline fn-ptr decls (VFS `xRead`/`xWrite`,
  vtab methods). 495 findings, categorical noise.

Per-finding false positives among *enabled* rules (the bulk of the noise) are
recorded as oracle labels below, **not** disabled — analyzer misfires stay
measured so they can be fixed.

## Adjudication log

### Increment 1 (2026-06-11, sqc 0.4.24) — INT32-C, 15 in-scope findings

`adjudication_sqlite_int32_inc1.csv` — sampled `realworld-unlabeled … --rule
INT32-C --project sqlite --seed 20260611`, filtered to in-scope, each verdict
from reading the source.

**14 FP / 0 TP / 1 uncertain.** Every FP is an internal quantity structurally
bounded far below INT_MAX: lock refcounts, b-tree cell indices, VDBE
program-counter/register/cursor numbers, FROM-clause table counts, column counts
(≤ SQLITE_MAX_COLUMN), assert-guarded divisors, and digit counters bounded by
int64 capacity. The 1 *uncertain* is `fts5_aux.c:519` `nScore +=` — an int score
accumulator bounded only by user-controlled document size, so signed overflow
(UB) is reachable on an extreme (~GB) indexed column. This reproduces the
v0.4.22 root-cause finding: INT32-C on sqlite is dominated by bounded-counter
false positives; genuine TPs require unbounded external input.

Combined with the 32 INT32-C sqlite labels from the v0.4.22 sample, the oracle
now has ~47 INT32-C sqlite labels.

## Next increments (priority order)

1. **MEM30-C** (2384, unlabeled) — free→use matcher; expect free-then-reassign,
   sibling-member, and non-pointer-field FPs.
2. **API00-C** (1377, unlabeled) — alias-validation FPs (cf. libcrc).
3. **EXP34-C** / **DCL13-C** — extend the existing v0.4.22 samples.

## Re-running / extending

    # pull the next unlabeled batch for a rule (reproducible):
    python -m bench realworld-unlabeled sqc-0.4.22-1c94dc95 \
        --rule MEM30-C --project sqlite --seed <S> --limit <N> --json

    # after adjudicating into a CSV:
    python -m bench realworld-import-labels data/precision_audit/sqlite/<csv> \
        --run sqc-0.4.22-1c94dc95 --source sqlite_<rule>_incN --adjudicator claude

    python -m bench realworld-score sqc-0.4.22-1c94dc95   # measured precision/recall

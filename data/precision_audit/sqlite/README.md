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

### Increment 2 (2026-06-11, sqc 0.4.24) — MEM30-C, 45 in-scope findings

`adjudication_sqlite_mem30_inc2.csv` — sampled `realworld-unlabeled … --rule
MEM30-C --project sqlite --seed 20260611 --limit 60`, 48 in-scope of which 45
labelled (see scope note below), each verdict from reading the source.

**45 FP / 0 TP.** Every finding is an analyzer misfire; sqlite's UAF/double-free
discipline is sound on the sampled sites. The FPs cluster into five systemic
free→use matcher defects worth fixing:

1. **Live VDBE opcode `pOp`** (~14, all `src/vdbe.c`) — `pOp` is the program
   counter during bytecode execution and is never freed; the matcher flags every
   `pOp->p1/p3/p4/opcode` read (most inside asserts). Largest single class.
2. **Allocator/connection handle misread as the freed object** — `sqlite3DbFree(db,
   member)` / `Tcl_Free`, then a later use of `db` (the *allocator*, first arg) or
   a sibling member is flagged (`vdbeaux.c:3756`, `vdbeapi.c:940`, `main.c:1429`,
   `trigger.c:196/250`).
3. **Loop frees distinct list nodes, parent flagged** — frees `azArg[i]` /
   `pCleanup` / `DblquoteStr` / `FuncDef` nodes in a loop, then the container or a
   following sibling access is flagged (`vtab.c:349`, `prepare.c:588`, `main.c:1429`).
4. **Free-then-reassign (incl. realloc out-params)** — `free(x->m); x->m = f();`
   or `*p = realloc(*p, …)`, then the reassigned value is read (`amatch.c:880`,
   `fuzzer.c:626`, `rbu` out-param `&p->zErrmsg`).
5. **Path-insensitive "double-free"** — mutually-exclusive if/else or
   error-branch-then-return counted as two frees on one path (`os_win.c:5195/5221`,
   `vdbeaux.c:1409`, `qrf.c:2912`). Also **in-place page edits** misread as frees:
   `pageFreeArray`/`dropCell` reorganise cells *within* a MemPage but free no heap,
   yet `pPg`/`aData`/`pData` access is flagged (`btree.c:7894/7903/7933/9665`), and
   `freePage`+`releasePage` (free the *db page* vs release the *in-memory ref* —
   distinct ops) is flagged as a double-free (`btree.c:10392`).

MEM30-C stays **enabled**: it is a genuine memory-safety rule, so these misfires
are recorded as oracle labels (not suppressed) to drive the matcher fixes above.

**Scope note:** `src/tclsqlite.c` (the Tcl language binding, not the shipped
engine — same rationale as the `ext/jni`/`ext/wasm` exclusion) and
`ext/fts5/fts5_test_tok.c` (a test tokenizer, matching the `*test*` test-glue
exclusion) are treated as out-of-scope; their 3 sampled MEM30-C findings (all
also FP) are not labelled.

MEM30-C oracle now has 99 sqlite labels (54 prior + 45). Overall measured
sqlite-inclusive precision: 7.5% (TP 33 / 438 labeled).

## Next increments (priority order)

1. **API00-C** (1377, unlabeled) — alias-validation FPs (cf. libcrc).
2. **EXP34-C** / **DCL13-C** — extend the existing v0.4.22 samples.
3. **INT30-C** (940), **EXP33-C** (692), **INT14-C** (631), **ARR00-C/ARR30-C**.

## Re-running / extending

    # pull the next unlabeled batch for a rule (reproducible):
    python -m bench realworld-unlabeled sqc-0.4.22-1c94dc95 \
        --rule MEM30-C --project sqlite --seed <S> --limit <N> --json

    # after adjudicating into a CSV:
    python -m bench realworld-import-labels data/precision_audit/sqlite/<csv> \
        --run sqc-0.4.22-1c94dc95 --source sqlite_<rule>_incN --adjudicator claude

    python -m bench realworld-score sqc-0.4.22-1c94dc95   # measured precision/recall

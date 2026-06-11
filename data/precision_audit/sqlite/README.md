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

### Increment 3 (2026-06-11, sqc 0.4.24) — API00-C, 49 in-scope findings

`adjudication_sqlite_api00_inc3.csv` — sampled `realworld-unlabeled … --rule
API00-C --project sqlite --seed 20260611 --limit 60`, 49 in-scope (48 new + 1
already labelled), each verdict from reading the function entry.

**49 FP / 0 TP.** API00-C ("functions should validate their parameters") fires on
any function that dereferences a pointer parameter with no preceding null-check in
the same function — on sqlite that is essentially the whole codebase. The same
false-positive class as libcrc (parameter guaranteed valid; sqc flags anyway), in
two sub-classes:

1. **Analyzer-fixable misses** — validation *is* present but sqc ignores it.
   `sqlite3_str_truncate` opens with `if( p!=0 && … )` (printf.c:1232);
   `sqlite3DeleteTriggerStep` guards every deref with `while(pTriggerStep)`
   (trigger.c:19); `sqlite3PcacheSetCachesize` (pcache.c:863) and
   `sqlite3WalSavepointUndo`'s sibling assert `pWal` non-null at entry. sqc should
   treat a dominating `assert(p!=0)` / `if(p!=0)` / loop-condition guard as
   validation (cf. the libcrc alias-miss).
2. **Advisory-vs-house-style mismatch** (the bulk) — sqlite deliberately relies on
   caller-guaranteed preconditions plus debug `assert`s rather than runtime
   null-checks on internal routines, and documents NULL/invalid handles as misuse
   /UB for public leaf accessors (`sqlite3_value_int`, `sqlite3_column_bytes16`)
   for speed. Out-params (`ppNew`, `pzErrMsg`, `pnCol`, …) and extension-entry
   handles (`sqlite3_*_init`'s `db`/`pApi`, supplied by the loader) are non-null by
   contract. Requiring an explicit check would contradict the documented design and
   is not actionable.

**API00-C stays enabled** (not categorically disabled) for two reasons: sub-class 1
is a real, fixable analyzer gap that must stay measured, and the rule retains some
merit at public-API boundaries. It is, however, the **strongest categorical-disable
candidate** found so far (advisory, ~100% structural FP, ~1377 in-scope findings —
parallel to the already-disabled advisory DCL05-C) and is flagged here pending a
decision; deferring keeps the fixable sub-class-1 misfires on the books.

API00-C oracle now has 62 sqlite labels (14 prior + 48). Overall measured
sqlite-inclusive precision: 6.8% (TP 33 / 486 labeled).

### Increment 4 (2026-06-11, sqc 0.4.24) — EXP34-C, 36 in-scope findings

`adjudication_sqlite_exp34_inc4.csv` — sampled `realworld-unlabeled … --rule
EXP34-C --project sqlite --seed 20260612 --limit 50`, 36 in-scope, each verdict
from tracing where the pointer originates and whether NULL is reachable at the
deref. EXP34-C (null-deref) is the rule most likely to hold a real TP, so each
site was read carefully.

**36 FP / 0 TP.** sqlite's null-safety holds at every sampled site. The FPs
expose several distinct analyzer gaps:

1. **`sizeof(*p)` misread as a dereference** (`os_win.c:4269`,
   `pNew = sqlite3MallocZero(sizeof(*pShmNode)+…)`) — `sizeof`'s operand is
   unevaluated; flagging it as a runtime deref is a clear bug.
2. **OOM-sets-error-code correlation** — the fts5/rbu idiom `p = alloc(); if(
   p->rc==SQLITE_OK){ use p }` where the allocator sets `p->rc` (or the engine's
   `mallocFailed`) on failure, so the pointer is non-null exactly when `rc==OK`.
   sqc doesn't model success⇒non-null (`fts5_index.c:7281/1283`, `7795` blob-open,
   `fts3.c:3023`, `rbu` step loops).
3. **NULL-safe callees not recognized** — `releasePage` (`if(pPage) …`),
   `sqlite3_step` (MISUSE-safe), `sessionSerializeValue`/`idxFindConstraint` (guard
   their arg with `if(pValue)` / `for(pCmp=pList; pCmp; …)`), `rbuFinalize`
   (wraps `sqlite3_finalize`) — passing a possibly-NULL pointer is harmless.
4. **Correlated / short-circuit guards not modeled** — `(p=expr)==0 || p->field`,
   `n=(p?p->nExpr:0); for(i<n) p->a[i]`, `if(NEVER(p==0)) return`, and
   assignment-in-`if` (`if(db==0 || (pParse=db->pParse)==0) return`).
5. **Caller-invariant / already-dereferenced-above** — the largest class: the
   pointer is dereferenced safely a few lines earlier or is a static-helper param
   guaranteed by the caller.

These are *all* analyzer misfires (no genuine missing null-check), so EXP34-C
stays enabled and the labels drive the fixes above — class 1 (`sizeof`) and class 2
(OOM-rc correlation) are the highest-leverage.

EXP34-C oracle now has 86 sqlite labels (50 prior + 36; 1 TP / 85 FP). Overall
measured sqlite-inclusive precision: 6.3% (TP 33 / 522 labeled).

## Next increments (priority order)

1. **DCL13-C** (2070) — extend the existing v0.4.22 sample.
2. **INT30-C** (940), **EXP33-C** (692), **INT14-C** (631), **ARR00-C/ARR30-C**.
3. Revisit the **API00-C categorical-disable** decision once sub-class-1 (assert/
   guard recognition) is either fixed or ruled out.

### Increment 5 (2026-06-11, sqc 0.4.24) — DCL13-C, 23 in-scope findings

`adjudication_sqlite_dcl13_inc5.csv` — sampled `realworld-unlabeled … --rule
DCL13-C --project sqlite --seed 20260612 --limit 50`, 23 in-scope (3 test files
excluded: `test_rtreedoc.c`, `test_expert.c`, `fts5_tcl.c`). DCL13-C
("declare unmodified pointer params const") is genuinely the highest-precision
rule, so verdicts follow the v0.4.22 framework: **TP** = the flagged param object
is itself never written and `const` would compile (C `const T*` only promises the
immediately-pointed-to object is unwritten — reading non-const member pointers and
mutating *their* pointees is still const-valid); **FP** = the signature is fixed by
a function-pointer type, or the param is stored into a non-const field, or the
param object is mutated.

**10 TP / 13 FP** (43%, in line with the prior 34%).

- **TP (10)** — internal read-only helpers: getters (`fts5SegmentSize`,
  `sqlite3PcachePagecount`, `sqlite3LookasideUsed`), codegen helpers that read the
  param and mutate *other* objects reached through it (`sqlite3VdbeReleaseRegisters`,
  `translateColumnToCopy`, `sqlite3WhereAddScanStatus`,
  `sqlite3ExpirePreparedStatements`, `windowAggStep`), a reference-array consumer
  (`pageFreeArray` reads `pCArray` while editing the page), and a member-pointee
  destructor whose own struct is unwritten (`sqlite3ClearOnOrUsing`).
- **FP (13)** — 11 **API-mandated signatures**: SQL-function impls (`argv`/`apVal`
  in `strftimeFunc`, `jsonArrayLengthFunc`, `writeblobFunc`, `transliterateSqlFunc`,
  `fts5ExprFold`, `addConstraintFunc`), vtab methods (`carrayEof` xEof,
  `deltaparsevtabConnect` xConnect), a VFS method (`vlogSync` xSync), a busy-handler
  callback (`sqliteDefaultBusyCallback`), and a public-API opaque pointer
  (`sqlite3_unlock_notify`'s `pArg`) — none can take `const` without breaking the
  published function-pointer type. Plus `fts5MultiIterNew` (stores `pColset` into a
  non-const field) and `renameTokenFind` (unlinks nodes from `pParse->pRename`,
  mutating `pParse`).

The signature-fixed FPs are the analyzer-improvement target: sqc should suppress
DCL13-C when the function's address is taken as a callback / assigned to a
fixed-prototype function pointer (xFunc, xConnect, sqlite3_io_methods, etc.).

DCL13-C oracle now has 73 sqlite labels (50 prior + 23; 27 TP / 46 FP). Overall
measured sqlite-inclusive precision: 7.9% (TP 43 / 545 labeled).

## Re-running / extending

    # pull the next unlabeled batch for a rule (reproducible):
    python -m bench realworld-unlabeled sqc-0.4.22-1c94dc95 \
        --rule MEM30-C --project sqlite --seed <S> --limit <N> --json

    # after adjudicating into a CSV:
    python -m bench realworld-import-labels data/precision_audit/sqlite/<csv> \
        --run sqc-0.4.22-1c94dc95 --source sqlite_<rule>_incN --adjudicator claude

    python -m bench realworld-score sqc-0.4.22-1c94dc95   # measured precision/recall

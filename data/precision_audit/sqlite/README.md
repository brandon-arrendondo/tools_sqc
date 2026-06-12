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

## Methodology shift — file-at-a-time audited corpus (from 2026-06-11)

Increments 1–5 above used **rule-stratified sampling**: pull N findings of one
rule, label, repeat. That measures per-rule precision but (a) never reaches a
defined "done" on a finite codebase, (b) requires per-rule×per-file bookkeeping,
and (c) is **structurally precision-only** — it can never see a false negative,
because an FN is a bug sqc never emitted, so it is never sampled. Recall measured
that way is circular (recall against TPs found *inside* sqc's own output).

The audit now proceeds **file-at-a-time**: the file is the atomic unit of "done".
A file is *audited* when every sqc finding in it has been labeled (TP/FP/uncertain)
**and** the file has been read independently for bugs sqc missed (recorded as
`FN` ground-truth rows). The audited-file set grows monotonically toward full
coverage (sqlite: 354 in-scope files), is reusable across every future sqc
version, and yields honest **precision *and* recall** over the swept subset.
The codebase is frozen at commit `b1a73ba34d`, so this is a finite, completable,
versionable corpus. (The increment 1–5 rule-sampled labels remain valid as a
separate precision-only dataset; they don't count toward audited-file
completeness until their files are fully swept.)

### File-at-a-time workflow

    # 1. pull EVERY unlabeled finding in one file (all rules):
    python -m bench realworld-unlabeled sqc-0.4.22-1c94dc95 \
        --project sqlite --file ext/misc/basexx.c --json

    # 2. read the whole file: label each finding TP/FP, AND record any bug sqc
    #    MISSED as an FN row. CSV cols: rule,idx,project,file,line,verdict,
    #    reason[,provenance,confidence]. verdict ∈ TP|FP|uncertain|FN.
    #    For FN, provenance records corroboration (juliet:CWE-…, cross:<proj>,
    #    cve:…, uncorroborated) and confidence ∈ high|medium|low.
    python -m bench realworld-import-labels data/precision_audit/sqlite/<csv> \
        --run sqc-0.4.22-1c94dc95 --source sqlite_<file> --adjudicator claude

    # 3. mark the file done (refuses if any finding is still unlabeled):
    python -m bench audit-complete --run sqc-0.4.22-1c94dc95 \
        --project sqlite --file ext/misc/basexx.c

    # 4. measure precision+recall over the audited corpus, track coverage:
    python -m bench audit-score sqc-0.4.22-1c94dc95
    python -m bench audit-coverage sqc-0.4.22-1c94dc95
    # one-time: record the coverage denominator
    python -m bench audit-coverage sqc-0.4.22-1c94dc95 --project sqlite \
        --set-total 354 --note "src/ + ext/ shipped engine, excl test/tool/jni/wasm/tcl"

    # 5. when a codebase reaches 100%, freeze a citable version for the paper:
    python -m bench oracle-freeze v1.0 --run sqc-0.4.22-1c94dc95 \
        --notes "sqlite@b1a73ba34d complete"
    python -m bench oracle-versions

How FN affects scoring: an `FN` row has no matching sqc finding, so it never
touches precision, but it joins the recall denominator as a known real bug —
"detected" only once a (better) sqc version emits a finding at that line+rule.
When sqc is improved to catch a recorded FN, re-running `audit-score` shows
recall rise, and the FN is re-adjudicated/cross-referenced (against Juliet, the
other codebases, CVEs) and the label updated — bumping the frozen oracle version.

## Re-running / extending (legacy rule-sampling — for reference)

    # pull the next unlabeled batch for a rule (reproducible):
    python -m bench realworld-unlabeled sqc-0.4.22-1c94dc95 \
        --rule MEM30-C --project sqlite --seed <S> --limit <N> --json

    # after adjudicating into a CSV:
    python -m bench realworld-import-labels data/precision_audit/sqlite/<csv> \
        --run sqc-0.4.22-1c94dc95 --source sqlite_<rule>_incN --adjudicator claude

    python -m bench realworld-score sqc-0.4.22-1c94dc95   # measured precision/recall

---

## File-at-a-time audited corpus — Batch 1 (2026-06-11, sqc 0.4.24, run #40)

First file-at-a-time batch against the **config-correct** run #40 (`sqlite-rules.toml`,
39,215 distinct findings; in-scope universe **220 files**). All 259 increment-1–5
labels reconcile to run #40 with **zero orphans**. Coverage denominator pinned at 220.

`adjudication_sqlite_batch1.csv` — the 29 smallest in-scope files (≤6 findings each;
`src/tclsqlite.h` excluded as the Tcl binding). 89 findings adjudicated:
**22 TP / 67 FP**, plus 2 false negatives.

### The reveal: noise is in the *semantic* rules; *lexical/preprocessor* rules find real issues

| High-precision (real) | | Pure noise (all FP) |
|---|---|---|
| **DCL37-C 100% (9/9)** reserved-identifier include guards (`_FTS5_H`, `_SQLITE_OS_H_`, …) | | API00-C, API02-C 0/21 each |
| **PRE02-C 100% (3/3)** unparenthesized `-1`/`-2000` macro bodies | | MEM30-C, EXP33-C, DCL13-C |
| **PRE06-C 86% (6/7)** missing include guards | | INT30/INT32/INT14/INT16-C |
| **PRE01-C, PRE11-C 75%** macro-param parens / trailing `;` | | EXP12/14/19-C (advisory, base-disabled elsewhere) |
| **DCL15-C 33%** should-be-static | | EXP37-C, STR31-C, INT12-C, MSC04/37-C |

sqlite's enormous finding count is dominated by **semantic-analysis false positives**
(use-after-free matcher, null/uninit, integer overflow, param-validation, const-
correctness) — the same misfire classes found in increments 2–5. The
preprocessor/declaration rules, by contrast, correctly flag genuine (if low-severity)
standards violations sqlite pervasively commits (reserved identifiers for header
guards, bare negative macro literals, macros ending in `;`).

### False negatives found (in just 29 small files)

1. **`ext/misc/sqlar.c:94` — INT31-C (security-relevant), recorded as FN.**
   `sqlite3_int64 sz` (the uncompressed size read from a sqlar blob — attacker-
   controllable via a malicious archive) is passed to `sqlite3_malloc(int)`. For
   `sz > INT_MAX` the size truncates, under-allocating `pOut`, after which
   `uncompress` writes the full `szf` bytes → **heap overflow**. sqc emits 12
   INT31-C in sqlite (all FP) yet misses this real narrowing. Confidence medium.
2. **`src/pcache.h:16` — PRE06-C, recorded as FN.** `#ifndef _PCACHE_H_` has **no
   matching `#define`**, so the include guard never arms (re-inclusion unprotected);
   `_PCACHE_H_` is also a reserved identifier. sqc's PRE06 detector (86% precision
   here) misses the broken-guard variant. Confidence medium.
3. `ext/misc/basexx.c:81` — `BASE64_EXPOSE(db,pzErr)` is called twice (the second
   should be `BASE85_EXPOSE`); base85 is never registered. A real copy-paste defect,
   but no enabled rule cleanly fits (MSC12-C dead-code) → documented, not inserted.

**Coverage: 29/220 files (13.2%).** sqlite audited subset so far: precision 24.7%
(22 TP / 89), recall 22/24. (Both FN-1 and FN-2 map to rules sqc *has*, so they sit
in the INT31-C / PRE06-C recall denominators.)

---

## File-at-a-time — btree.c (2026-06-11, run #40): the hardest single file

`adjudication_sqlite_btree.csv` — `src/btree.c` (the b-tree engine, ~11K lines),
**1,056 distinct findings**, the largest single file. Adjudicated via 8 rule-group
reviewer subagents + spot-checks, then 3 dedicated FN-hunt subagents over the
corruption-prone paths.

**88 TP / 963 FP / 5 uncertain / 0 FN.**

### The reveal, confirmed at scale: precision bifurcates by rule *class*

Every TP is a **declaration / const-correctness / macro-hygiene** finding; every
**semantic** finding (memory, integer, array, pointer) is a false positive.

| High-precision (real) on btree.c | | ~0% precision (all FP) |
|---|---|---|
| DCL03-C 88% (15/17) `assert(sizeof…)`→`static_assert` | | INT32-C 0/249 |
| MSC04-C 86% (6/7) genuine (self/indirect) recursion | | MEM30-C 0/178 |
| DCL13-C 70% (54/77) read-only helpers/getters → `const` | | ARR-family 0/157 |
| PRE01-C 67% (6/9) unparenthesized macro params | | EXP-family 0/144 |
| (PRE00/PRE10/PRE12 macro hygiene also real) | | API00-C 0/90, INT-other 0/129 |

btree.c parses the untrusted on-disk database format, the classic sqlite
vulnerability surface — yet **all** of the integer-overflow (INT32/INT30/INT31/
INT33), out-of-bounds (ARR30/37/36), use-after-free (MEM30) and null/uninit
(EXP33/34) findings are false positives: every dangerous read/write/size-arith is
gated by a **release-active** `SQLITE_CORRUPT_PAGE`/`SQLITE_CORRUPT_BKPT` check, a
`maskPage`/`usableSize` bound, an `SQLITE_MAX_LENGTH` upstream clamp, or an
`i64` pre-cast. The analyzer's bounded-counter / live-handle / in-place-page-edit
misfire classes (documented in increments 2–5) account for the entire 963-FP set.

### FN-hunt: 0 false negatives

Three subagents read the highest-risk clusters in full — cell parse/alloc
(`btreeParseCellPtr`, `allocateSpace`, `freeSpace`, `defragmentPage`), tree
restructuring (`balance_nonroot`, `insertCell`, `editPage`, `pageInsertArray`),
and overflow/freelist/ptrmap/payload (`accessPayload`, `allocateBtreePage`,
`freePage2`, `clearDatabasePage`) — and found **no** unguarded defect. Notable:
`clearDatabasePage`'s recursion is cycle-safe via the page-refcount check;
`(k-1)*4` trunk copies and `nLeaf` writes are `usableSize/4`-bounded; the
`assert`-only bounds are all redundant with release-active CORRUPT returns.
Consistent with btree.c being the most-fuzzed C code in existence.

### Analyzer takeaway

For a future sqc effort the conclusion is sharp: **FP-reduction must target the
semantic engines** (use-after-free matcher, integer-range, array-bounds, null/
uninit), which are ~0% precise on hardened production C; the **declaration/macro/
const rules are already paper-worthy** (67–88%). sqlite coverage now 30/220 (13.6%).

---

## File-at-a-time — vdbe.c (2026-06-11, run #40, HIGH-effort): the bifurcation holds in a 2nd engine

`adjudication_sqlite_vdbe.csv` — `src/vdbe.c` (the bytecode interpreter, ~9.3K
lines), **1,009 distinct findings**, the 2nd-largest file. High-effort pass: 8
rule-group reviewer subagents (5 of them splitting the 685 MEM30-C findings by
line range, each reading every free→use pair), 3 FN-hunt subagents over the
highest-risk opcodes, and adversarial resolution of all 4 uncertains.

**24 TP / 985 FP / 0 uncertain / 0 FN.**

vdbe.c is **68% MEM30-C** (685 findings) — the matcher reading the VM's live
program counter (`pOp`), register file (`aMem[]`/`pOut`/`pIn*`) and cursor array
(`apCsr[]`) as freed memory. **All 685 are FP** (these structures live for the
whole VM run; the genuine frees in the file are free-then-reassign or
distinct-object). Same outcome for the other semantic rules: INT-family 0/115,
ARR-family 0/114, EXP/API 0. Every one of the **24 TP is declaration / macro /
dead-code**: DCL13 (read-only helpers `out2Prerelease`, `MemPrettyPrint`,
`RegisterDump`, …), DCL03 (`assert(sizeof…)`/enum-equality → `static_assert`),
PRE00/PRE10/PRE12 (`HAS_UPDATE_HOOK`/`Deephemeralize` evaluate their arg twice),
MSC07 (genuinely dead `return`/`break`), and one ARR00 — `pOp=&aOp[-1]` forms a
pointer before the array start (UB per C11 §6.5.6) in live dispatch-sentinel code.

### FN-hunt: 0 false negatives (high-effort, 3 subagents)

Record/serial-type decode (`OP_Column`, `OP_MakeRecord`), function/aggregate/vtab
arg marshaling (`OP_Function`, `OP_AggStep`, `OP_VFilter`), and sorter/blob/seek
(`OP_Concat`, `OP_RowData`, `OP_SeekGE`, `UnpackedRecord` building) all read in
full. Every untrusted on-disk value is gated by a release-active check
(`aLimit[SQLITE_LIMIT_LENGTH]`, `payloadSize`, `>2147483645`, `SQLITE_CORRUPT_BKPT`)
before any allocation/index/memcpy. The register/cursor indices that lack a
release-build bound (`&aMem[pOp->p3]`, `apCsr[pOp->p3]`) are **codegen-emitted
constants, not input** — trusted-bytecode invariants a conservative analyzer
should not flag, and sqc correctly does not → no FN.

### Two giant core files, same verdict

btree.c (1,056) + vdbe.c (1,009) = **2,065 findings across the two largest, most
complex files in sqlite's core: 0 semantic-rule TP, 0 FN, every TP in the
declaration/macro/const/dead-code class.** The bifurcation is not an artifact of
one subsystem (on-disk-format parsing) — it reproduces in the bytecode engine.
sqlite coverage now 31/220 (14.1%).

---

## File-at-a-time — fts5_index.c (2026-06-11, run #40, HIGH-effort): the bifurcation's first crack

`adjudication_sqlite_fts5_index.csv` — `ext/fts5/fts5_index.c` (the FTS5 full-text
INDEX engine, ~9.5K lines), **911 findings**, the largest *extension* file. Chosen
deliberately: fts5 parses its OWN on-disk format (varint blobs in the `%_data`
shadow table — attacker-controllable via a crafted DB) and is far less fuzzed than
core btree/vdbe, so the likeliest place a real semantic finding surfaces. 9
high-effort reviewer subagents + 2 FN-hunt subagents + adversarial verification.

**64 TP / 839 FP / 8 uncertain / 0 confirmed FN.**

### First genuine semantic TPs in the whole audit

Unlike btree.c/vdbe.c (0 semantic TP), fts5StructureDecode yields **2 real INT32-C
true positives**: `fts5SegmentSize` (1485) and 1551 compute `1 + pSeg->pgnoLast -
pSeg->pgnoFirst`, where `pgnoLast`/`pgnoFirst` are read from the untrusted structure
blob via `fts5GetVarint32` with only a relational check (`pgnoLast>=pgnoFirst`), no
upper bound → **signed-overflow UB** for `pgnoLast` near INT_MAX on a crafted index.
Low real-world impact (feeds merge heuristics; pgnos are revalidated at page-read,
so not memory corruption) but genuine UB on untrusted input — exactly the
less-fuzzed-extension scenario predicted.

Plus **8 uncertains**, all in the same fts5StructureDecode integer cluster:
`nTotal * sizeof(Fts5StructureSegment)` allocation (1166/1169/1171 — 64-bit-safe,
latent 32-bit multiply wrap requiring an implausibly large blob), the
`pgnoFirst`/`pgnoLast` u32→int reads (1184/1185), a poslist column-delta (3518),
and bounded recursion (MEM05 1619/1722).

### The rest of the semantic mass: still ~0% FP

MEM30-C 0/12, **EXP34-C 0/101** (the fts5 OOM-sets-`p->rc` idiom — pointers non-null
when rc==OK), ARR00/ARR30 0/106, ARR37/etc 0/50, INT-other 0/120. Every TP outside
the 2 structure-overflows is declaration/macro/style: DCL13 37 (read-only helpers),
EXP45 3 (assignment-in-condition), PRE00/01/10/12 18 (macro hygiene), MSC04 2
(recursion), ERR33 1.

### FN-hunt: 0 confirmed (one candidate adversarially dismissed)

A subagent flagged `fts5DlidxLvlPrev` (1706): `while(a[ii]==0){ii++}` lacks an
explicit `ii<nn` bound (a real asymmetry with the nn-bounded sibling
`fts5DlidxLvlNext`). On adversarial review this is **not** a reachable OOB: `Prev`
re-walks only the `[iFirstOff, iOff)` range that forward `Next` already validated to
contain non-zero varint terminators; the all-zero tail that would run the scan off
the end lies *past* `iOff`, which `Prev` never reaches (it breaks at `ii>=iOff`). A
defensive-hardening gap, not a bug — consistent with sqlite's heavy corrupt-DB
fuzzing. Documented, not recorded as FN.

### Running tally (3 large files: btree + vdbe + fts5_index = 2,976 findings)

The bifurcation holds, with one nuance: the **only** semantic TPs in ~3,000
findings are 2 contained signed-overflows in fts5's untrusted-blob parser; the
memory/null/array semantic engines remain **0% precise** on hardened C. All other
TPs are declaration/macro/const/dead-code. Per-rule across the audited corpus:
MEM30-C **0/875**, EXP34-C **0/202**, ARR30-C **0/97** vs DCL13-C **65% (100 TP)**.
sqlite coverage 32/220 (14.5%).

---

## File-at-a-time — Batch 2 (2026-06-11, run #40): 27 small files (coverage push)

`adjudication_sqlite_batch2.csv` — the next 27 smallest in-scope files (7–18
findings each: ext/misc demos, mutex/threads layer, internal headers).
364 findings: **25 TP / 321 FP / 18 uncertain**, plus **2 FNs**. Coverage
27→59/220 (12→27%). Adjudicated via 5 lightweight parallel reviewer subagents.

The pattern is now firmly established and unchanged: **every TP is declaration/
macro/dead-code** — PRE01 ×6 (unparenthesized macro params, e.g. `MX_CELL_SIZE(pBt)`
→ `pBt->pageSize`), PRE11 ×6 (macro ending in `;`, e.g. the `VdbeCoverage*` macros),
DCL03 ×4 (`assert(SQLITE_MUTEX_*` relations) → static_assert), PRE10 ×3 (bare-`if`
macros not in do/while), DCL13 ×2 (read-only helpers in notify.c), DCL37 ×1
(`_OS_COMMON_H_` reserved guard), DCL01 ×1 (shadowed `z` in zorder.c). Every
semantic finding (MEM30/INT/ARR/EXP33/34/API00/API02/CON) is FP.

### 2 false negatives (recorded, EXP34-C, upstream-PR candidates)

`ext/misc/compress.c` — both functions allocate with `sqlite3_malloc64` and use the
result with **no NULL check**:
- `compressFunc` (~63): `pOut=sqlite3_malloc64(nOut+5)` then `pOut[j]=…` / `compress(&pOut[j],…)` → null-write on OOM.
- `uncompressFunc` (~102): `nOut` is decoded from the **input blob** (attacker-controllable, up to ~2³⁵) → `sqlite3_malloc64(nOut+1)` → `uncompress(pOut,…)` with no NULL check → reliable OOM null-write.
sqc fired 200+ EXP34-C false positives across the corpus yet missed these two real
unchecked-allocation null-derefs — a detector-recall gap, and upstream-PR candidates
for sqlite's compress extension (see task 164).

Running coverage: **sqlite 59/220 (26.8%)**, libcrc 19/19. Audited-corpus tally so
far: only semantic TPs remain the 2 fts5 structure-overflows; declaration/macro/const
rules carry all other TPs; 4 FNs total (sqlar heap-overflow, pcache broken guard,
2× compress null-deref).

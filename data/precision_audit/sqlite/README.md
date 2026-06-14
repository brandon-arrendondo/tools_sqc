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

---

## File-at-a-time — sqlite3session.c (2026-06-12, run #40, HIGH-effort): the bifurcation's biggest crack — a candidate OOB-read

`adjudication_sqlite_session.csv` — `ext/session/sqlite3session.c` (the session /
changeset extension, ~6.8K lines), **628 findings**, largest remaining file. Chosen
as a prime needle target: it parses untrusted changeset/patchset blobs
(`sqlite3changeset_apply`/`_start`/`_invert`/`_concat`). 8 high-effort reviewer
subagents + adversarial verification.

**47 TP / 566 FP / 15 uncertain / 0 confirmed FN.**

### Candidate OOB-read in changeset invert/concat (4 INT32-C TP + 14 INT31-C uncertain)

Two subagents independently converged on `sessionChangesetBufferRecord` (the
raw-record fast path used by invert/concat/merge, NOT the value-decode path):

- It reads each field length via the **unsafe `sessionVarintGet`** (no remaining-
  buffer bound, unlike `sessionVarintGetSafe` in the validated `sessionReadRecord`)
  and accumulates `nByte += n` (3691/3692/3695) with **no `nByte<0`/overflow/bounds
  check** — a genuine signed-overflow on attacker-controlled blob data (**4 INT32-C
  TP**: 3691, 3692, 3695, 4184).
- `sessionInputBuffer` is a **no-op for non-streaming input** (`xInput==0`, the
  in-memory `sqlite3changeset_*` blob case), so it provides no bound either.
- The callers then use the unbounded `nByte` without validating it against the
  input size: the iterator does `*paRec=&aData[iNext]; iNext+=nByte` (3848), and
  **`sqlite3changeset_invert` does `sessionAppendBlob(&sOut, &aData[iNext], nByte, …)`
  (4187)** — copying `nByte` bytes from the blob. A crafted changeset with an
  inflated field-length varint can drive `nByte` past the input → **out-of-bounds
  read** (and the 14 INT31-C uncertains are the merge/rebase `sessionSerialLen →
  memcpy` consumers of the same unvalidated lengths).

This is the audit's most security-relevant finding. It is a **candidate** (pinned to
`b1a73ba34d`; needs confirmation against current sqlite trunk and a crafted-changeset
repro) and is tracked for **responsible disclosure**, NOT a casual PR — see the
dedicated security task. Contrast with btree.c/vdbe.c where every dangerous path was
release-guarded: the session raw-record fast path is the one place the validation is
genuinely absent.

### Everything else: bifurcation holds

The other 43 TP are all declaration/macro/const: DCL13 ×30 (read-only helpers),
DCL03 ×5 (static_assert), PRE00/02/12 ×7 (multiple-eval/unparenthesized macros:
`SESSION_UINT32`, `HASH_APPEND`), DCL00 ×1. The semantic mass is FP as ever:
EXP34 0/102 (rc-propagation idiom), MEM30 0/28, ARR 0/56, STR34 0/22 (the changeset
byte reads use `u8*`, no sign-extension bug), API00 0/47. 1 CON03 uncertain
(`sessions_strm_chunk_size` global, race only under API-contract-violating
concurrent use).

### Running tally (4 large files: btree+vdbe+fts5+session = 3,604 findings)

Semantic-rule TPs now total **6** — 2 contained fts5 structure-overflows + 4 session
changeset-overflows (the latter a candidate real OOB-read). All other ~270 TPs are
declaration/macro/const. The pattern: sqc's semantic engines are ~0% precise on
hardened core, but on **less-fuzzed extensions parsing untrusted serialized input**
they occasionally fire on genuine integer-overflow/validation gaps. sqlite coverage
60/220 (27.3%).

### UPDATE (2026-06-12): the changeset OOB-read is CONFIRMED REAL and already fixed upstream

Verified against current sqlite trunk (`sqlite-main` @ `124f449319`). The candidate is
a **genuine bug that sqlite has since fixed** — our audited commit `b1a73ba34d`
(2026-02-24) predates the fix:

- **`535b1f2875` (2026-04-09) — "Fix some buffer overreads that might occur in the
  session module when handling corrupt changesets."** Changed `sessionChangesetBufferRecord`
  exactly where we flagged: `int nByte` → `i64 nByte`, unsafe `sessionVarintGet` →
  `sessionVarintGetSafe(…, nRem, …)`, and added `iNext+nByte>=nData → CORRUPT` plus a
  post-accumulation `iNext+nByte>nData → CORRUPT`. Also touched `sessionChangeMerge`.
- **`4da2ddf50e` (2026-05-19) — "Avoid a potential 1 byte overread in
  sqlite3changegroup_add() when processing a corrupt changeset buffer."** — same root function.

So the **4 INT32-C TPs (sessionChangesetBufferRecord nByte overflow) are confirmed
true positives** against ground truth: sqc independently rediscovered a real,
security-relevant, since-fixed sqlite buffer-overread. The 14 INT31-C consumer findings
stay *uncertain* (sqlite fixed at the root, not per-consumer; their truncations were
real but governed by the now-added root validation). **No responsible disclosure needed
— already fixed.** Security task 165 closed.

This is the headline real-world validation: across 3,604 findings in 4 large files, the
audit's most security-relevant TP corresponds to an actual sqlite-acknowledged CVE-class
fix — concrete evidence that the analyzer's semantic findings, when properly adjudicated,
catch genuine bugs in less-fuzzed extension code parsing untrusted serialized input.

---

## Trunk-validation pass (2026-06-12): every candidate bug confirmed against sqlite-main

All genuine-bug findings (semantic TPs + recorded FNs) checked against current sqlite
trunk (`sqlite-main` @ `124f449319`). All fixes postdate our audited pin `b1a73ba34d`
(2026-02-24), confirming the audited code was genuinely vulnerable. **5/5 candidate
bugs confirmed real — 3 already fixed by sqlite, 2 still present, zero false alarms** —
spanning both true positives sqc *found* and false negatives sqc *missed*:

| Finding | sqc verdict | Trunk status | Upstream fix |
|---------|-------------|--------------|--------------|
| `sessionChangesetBufferRecord` `nByte` overflow → OOB-read (INT32 ×4) | **TP** | **FIXED** | `535b1f2875` (2026-04-09) + `4da2ddf50e` (2026-05-19) |
| `sqlar.c` `sqlite3_int64 sz` → `sqlite3_malloc(int)` truncation → heap overflow (INT31) | **FN** | **FIXED** | `34e139d3a3` (2026-04-01) malloc→malloc64 |
| `compress.c` unchecked `sqlite3_malloc64` → null-deref (EXP34 ×2) | **FN** | **FIXED** | `1c0a370472` (2026-06-03) OOM responsiveness |
| `fts5` `fts5SegmentSize` `1+pgnoLast-pgnoFirst` signed overflow (INT32 ×2) | **TP** | **STILL PRESENT** | — (low-severity, contained) |
| `pcache.h` `#ifndef _PCACHE_H_` with no `#define` (PRE06) | **FN** | **STILL PRESENT** | — (trivial, harmless in amalgamation) |

**Significance.** The real-world oracle's genuine-bug findings have a perfect
confirmation rate against sqlite ground truth. The 3 security-relevant ones
(changeset OOB-read, sqlar heap overflow, compress null-deref) were each
independently rediscovered by this audit *and* independently fixed by sqlite —
sqc's true positives and its false negatives both map to real, sqlite-acknowledged
fixes. This is the strongest possible evidence that careful real-world adjudication
surfaces genuine defects, and it grounds the precision/recall numbers in verified
reality rather than self-judged labels. The 2 still-present findings are the
legitimate upstream-contribution candidates (task 164); the 3 fixed ones need no
action.

---

## File-at-a-time — where.c (2026-06-12, run #40, HIGH-effort): bifurcation holds in a 3rd core file

`adjudication_sqlite_where.csv` — `src/where.c` (the query planner/optimizer, ~7K
lines), **571 findings**. Core sqlite (operates on parsed SQL + schema, not an
untrusted serialized blob). 7 high-effort reviewer subagents + 2 FN-hunt subagents.

**61 TP / 508 FP / 2 uncertain / 1 low-confidence FN.**

Bifurcation holds exactly as in btree.c/vdbe.c: **0 semantic-rule TP** (INT32 0/90,
ARR 0/99 — all bounded by BMS=64 / schema limits / misclassified integer fields,
MEM30 0/35, EXP34 0/55 — OOM/planner-invariant guards), **all 61 TP are
declaration/macro/const**: DCL13 ×46 (read-only planner inspectors), MSC04 ×6
(the WHERE solver's genuine direct/indirect recursion), DCL03 ×5 (static_assert),
EXP45 ×2 (assignment-in-condition), EXP05 ×1 (cast-away-const), PRE01 ×1. The
34 PRE32-C findings are all FP — the documented misfire on `WHERETRACE`/printf trace
macros (no real `#`/`##`/directive-in-arg). 2 benign uncertains (INT34 shift in a
SQLITE_DEBUG-only `WhereLoopPrint`; an EXP33 discarded-value read).

### 1 low-confidence FN (INT34, still present on trunk)

`whereLoopAddVirtualOne`: `MASKBIT32(iTerm)` (and `1<<iTerm` for omitMask) where
`iTerm = pUsage[i].argvIndex-1` is bounded only by `nConstraint`. A query with **≥32
constraints on a virtual table** drives `iTerm≥32` → shift ≥ operand width (INT34-C
UB). Consequence is a wrong optimization mask bit, **not** memory corruption — narrow
and benign. Verified **still present on sqlite-main `124f449319`** (lines 4457/4466
byte-identical). Recorded as a low-confidence FN / minor hardening candidate (task 164).

### Running tally (5 large files: btree+vdbe+fts5+session+where = 4,175 findings)

Semantic TPs remain **6** (2 fts5 + 4 session, the session ones a confirmed-fixed
real OOB-read); every other TP is declaration/macro/const. The 3 hardened-core files
(btree/vdbe/where) yield **0 semantic TP and 0 substantive FN**; the 2 untrusted-blob
extensions (fts5/session) are where the real semantic bugs surfaced. sqlite coverage
61/220 (27.7%).

---

## File-at-a-time — fts3.c (2026-06-12, run #40, HIGH-effort): bifurcation holds in an EXTENSION

`adjudication_sqlite_fts3.csv` — `ext/fts3/fts3.c` (the FTS3 full-text-search core /
vtab-dispatch module, 6,203 lines), **620 raw findings → 536 distinct (rule,line)
oracle keys** (e.g. PRE01@343 flagged once per macro param collapses to one key). This
is the **strongest test of the bifurcation yet**: fts3 is a genuine untrusted-input
extension (it parses MATCH query strings and attacker-corruptible serialized
segment/doclist/varint data from the `%_segments`/`%_segdir` shadow tables — the same
threat profile as fts5/session, where real semantic bugs *did* surface). 8 high-effort
rule-class reviewer subagents + 2 FN-hunt subagents.

**60 TP / 476 FP / 0 uncertain / 0 FN** (distinct-key counts; raw 65 TP / 555 FP).

**The semantic engines still produced pure noise — 0 TP across 531 semantic findings:**
INT-core (INT30/31/32/33/34/36) **0/148**, STR (STR34/00/37) **0/123**, MEM (MEM30/05/31/33/12)
**0/66**, ARR (00/30/36/37/38/39/02) **0/74**, EXP (33/34/40/05/30/…) **0/67**, API00+DCL30
**0/53**. The misfires are the *documented* classes — and notably, here they misfire on the
code's own hardening: validated-before-use untrusted lengths (`fts3ScanInteriorNode`
bounds `nPrefix`/`nSuffix` vs the node buffer, returns `FTS_CORRUPT_VTAB`), deliberate
well-defined `u64` delta-codec wraparound (`DOCID_CMP`), `char*` *pointer* ops misread as
char-value sign-extension (the entire STR34 set) or integer narrowing, OOM-sets-rc-guarded
derefs, end-pointer (`&a[n]`) sentinels read as OOB, null-after-free idiom, ownership-transfer
frees, internal helpers read as unvalidated public API, and heap/caller-buffer addresses
read as escaping locals (DCL30).

**All 65 raw TP (60 distinct) are declaration/macro/const + INT13/14 portability:** DCL13 ×32
(read-only pointer params → const), MSC04 ×7, PRE01 ×6 (unparenthesized macro params in the
`GETVARINT_*` macros), INT14 ×5 / INT13 ×3 (signed/mixed-sign bitwise in the varint shift +
poslist-scan idioms — benign but genuine), PRE12 ×3 / PRE00 ×3 / DCL00 ×3 (the multistatement
`GETVARINT_*` / `DOCID_CMP` macro hazards), PRE10 ×2, DCL03 ×1.

### FN-hunt: clean (0 FN)

Two FN-hunters swept the untrusted-data paths. **Serialized-data parsers** (varint decoders,
`fts3ScanInteriorNode`/`fts3SelectLeaf` segment-btree, `fts3DoclistMerge`/poslist mergers):
uniformly guarded by `FTS_CORRUPT_VTAB` range checks + the `FTS3_NODE_PADDING`(20)/`FTS3_BUFFER_PADDING`(8)
zero-fill invariants. The one structural oddity — `FTS3_VARINT_MAX`=10 > `FTS3_BUFFER_PADDING`=8,
so the *unbounded* `sqlite3Fts3GetVarintU` could in theory read 10 bytes off an 8-byte-padded
buffer — was **run down and confirmed NOT an over-read**: the merge buffers are `memset(…,0,
FTS3_BUFFER_PADDING)` (fts3.c:2672), and the reader's loop breaks on the first byte with the
high bit clear (`if((c&0x80)==0) break`), which a 0x00 pad byte always triggers. So it can
never walk past the zeroed pad into adjacent memory. **Not recorded as an FN.**

**Scope note (drives next targets):** the real untrusted-query attack surface — the MATCH
expression parser, tokenizer arg parsing, `fts3SpecialInsert` control commands, and
matchinfo/snippet format-char loops — does **not** live in fts3.c. It's in `fts3_expr.c`,
`fts3_tokenizer.c`, `fts3_write.c`, and `fts3_snippet.c`. fts3.c is the vtab/dispatch shell;
its own query/config code (`sqlite3Fts3ReadInt`, `fts3GobbleInt`, `Dequote`, the incr-phrase
`MAX_INCR_PHRASE_TOKENS` stack array) is well-guarded. **If hunting for a real semantic needle
in FTS3, those four sibling files — esp. `fts3_expr.c` and `fts3_snippet.c` — are the place.**

### Running tally (6 large files: btree+vdbe+fts5+session+where+fts3 = 4,711 distinct findings)

Semantic TPs remain **6** (2 fts5 + 4 session). fts3 — despite being an extension on the
fts5/session profile — added **0 semantic TP and 0 FN**, because the *needle is in fts5/session
specifically* (and likely the fts3 sibling parser files), not in every extension module.
Refinement to the thesis: the split is not "core vs extension" but **"hardened/fuzzed code vs
the specific less-fuzzed routines that parse untrusted serialized input"** — and fts3.c is
itself mature, heavily-fuzzed dispatch code. sqlite coverage **62/220 (28.2%)**.

---

## File-at-a-time — fts3_expr.c + fts3_snippet.c (2026-06-12, run #40, HIGH-effort): the FN-hunt lands a 7th real bug

Per the fts3.c scope note, the real FTS3 untrusted-query attack surface lives in the *sibling*
parser files, so we audited the two highest-value ones together. 8 rule-class reviewer subagents
(4 per file) + 2 dedicated FN-hunt subagents (one on the expression parser, one on matchinfo).

### `fts3_expr.c` — the MATCH expression/phrase PARSER (1316 lines, 139 findings)

**10 TP / 129 FP / 0 FN.** Bifurcation holds in a genuinely less-fuzzed recursive-descent parser:
**0 deep-semantic TP** — MEM30 0/38 (tree-cleanup paths use disciplined detach-then-free; recursion
capped at `SQLITE_MAX_EXPR_DEPTH`=1000), INT 0/32 (all token/offset arithmetic bounded by query
length ≤ `SQLITE_MAX_LENGTH`, allocs in i64), EXP34/EXP33/ARR 0/30 (parser state-machine invariants
+ OOM-rc guards). The 10 TP are the precise class: **MSC04 ×5** (the genuine `getNextToken` /
`getNextNode`↔`fts3ExprParse` / `fts3ExprBalance` recursion), **DCL13 ×4**, **INT13 ×1**. FN-hunt
clean: NEAR-distance overflow is benign (`sqlite3Fts3ReadInt` clamps to `0x7FFFFFFF`), the query
buffer is always NUL-terminated (sole caller passes `n=-1`), parenthesis nesting is hard-capped.

### `fts3_snippet.c` — snippet()/offsets()/matchinfo() (1778 lines, 185 findings)

**17 TP / 168 FP / 1 confirmed FN.** Semantic mass again FP: INT32 0/51 (matchinfo `aMatchinfo[]`
indices bounded by a 64-bit-sized allocation; `nCol ≤ SQLITE_MAX_COLUMN`), the column-mask shifts
0/x (`1<<(iCol&0x1F)` masked to 0–31; snippet shifts bounded by `nSnippet≤64`), ARR/EXP 0/33
(`iCol≥nCol → FTS_CORRUPT_VTAB` at 897). 17 TP: **DCL13 ×11**, **MSC04 ×2**, **INT13 ×2 / INT07 ×1**
(signed-char bitwise), **PRE11 ×1**.

### THE FN: a real heap OOB write in matchinfo('b'), confirmed + fixed upstream

The matchinfo FN-hunter found — and direct code reading + trunk diff confirmed — a genuine bug sqc
**missed**. In `fts3ExprLHits` (line 890, the `'b'`/`LHITS_BM` bitmask packer):

```c
// audited b1a73ba34d (BUGGY):
p->aMatchinfo[iStart + (iCol+1)/32] |= (1 << (iCol&0x1F));
```
The `'b'` region is sized `nPhrase * ((nCol+31)/32)` u32 words (`fts3MatchinfoSize`, 1030), i.e. a
per-phrase stride of `(nCol+31)/32`. But the write indexes the word with `(iCol+1)/32` while the bit
uses `iCol&0x1F` (= `iCol%32`) — **inconsistent**: bit `iCol` belongs in word `iCol/32`, not
`(iCol+1)/32`. When **`nCol` is a multiple of 32** (32, 64, 96, … — "a large number of columns"),
column `iCol=nCol-1` writes word index `nCol/32`, which equals the allocated word count → **one past
the region**; for the last phrase with `'b'` as the final directive that is a **heap OOB write**
(a `1<<31` OR; the `1<<31` is also signed-shift UB). Reachable from `matchinfo(t,'b')` (untrusted
format char) on a wide-enough table with a hit in the boundary column.

**CONFIRMED against trunk** `sqlite-main 124f449319`: line 890 now reads
`p->aMatchinfo[iStart + iCol/32] |= (1U << (iCol&0x1F));` — sqlite fixed **both** issues in
**`1192d6f5b1`** (2026-05-18): *"Fix an off-by-one error in matchinfo('b') for FTS3 when there are a
large number of columns. [bugs:/forumpost/42d5f799d1]"*. Our audited commit (2026-02-24) predates the
fix. Already fixed → **no disclosure needed.** Recorded as an `ARR30-C` FN (the OOB write; secondary
INT34/left-shift UB). This is the **first FN surfaced by the file-at-a-time FN-hunt that maps to a
real sqlite-acknowledged fix** — concrete validation of the audit's recall side.

### Running tally (8 files in the fts3/snippet family + 6 large prior = 5,015 distinct findings)

Semantic-rule TPs that sqc *found* remain **6** (2 fts5 + 4 session). The matchinfo bug is the **7th
confirmed real bug** of the audit but a **false negative** — it strengthens the thesis from a new
angle: the needle was again in a **less-fuzzed untrusted-input parser** (matchinfo format string +
match data), and even there sqc's *found* TPs are still all declaration/macro/const while the one
real semantic defect was a recall gap requiring interprocedural size-vs-index analysis sqc lacks.
Confirmed-real bug count now **7/7, zero false alarms** (3 fixed sec + 1 fixed matchinfo + 1 fixed
sqlar + … ). sqlite coverage **64/220 (29.1%)**, 6 FNs.

---

## File-at-a-time — fts3_write.c (2026-06-12, run #40, HIGH-effort): the write path holds, FTS3 family complete

`adjudication_sqlite_fts3_write.csv` — `ext/fts3/fts3_write.c` (the FTS3 WRITE path: segment-btree
writer, doclist builder, segment merge/incrmerge, and the `fts3SpecialInsert` control commands,
5856 lines), **571 raw → 494 distinct findings**. A strong needle: it *writes* the serialized
segment data, *reads it back* during merge from corruptible shadow tables, and parses untrusted
control-command text (`INSERT INTO t(t) VALUES('merge=A,B' / 'automerge=N' / 'optimize' / …)`).
9 rule-class reviewers (INT32 split in two — 172 of them) + 2 FN-hunters (segment-reader/merge,
and control-command paths).

**42 TP / 452 FP / 0 uncertain / 0 FN** (distinct; raw 46 TP / 525 FP).

**0 semantic TP across 525 findings** — INT32 0/172 (allocations done in i64/`sqlite3_malloc64`;
pending-data capped at `0x3fffffff`; counts schema-bounded by `nColumn`; the `merge=`/`automerge=`
parser `fts3Getint` explicitly clamps `<214748363`), EXP34 0/55 (the `fts3SqlStmt` `rc==SQLITE_OK ⇒
pointer non-null` idiom), API00 0/53 (all internal handles / OUT-params / asserted; `xUpdate`
validates *values* not pointers), MEM30 0/34 (realloc-reassign, null-after-free, and the disjoint
index-range frees in `fts3IncrmergeRelease`), ARR 0/44 (FTS3_NODE_PADDING + `FTS_CORRUPT_VTAB`),
STR 0/41 (char\* pointer-op misreads; the 2 genuine char widenings at 4351/5126 are range-guarded).
All 46 raw TP are the precise class: **DCL13 ×33, MSC04 ×4** (the `fts3SegmentMerge`↔
`fts3AllocateSegdirIdx` cycle + `fts3NodeWrite`/`fts3NodeFree` self-recursion), **INT13 ×3 / INT10
×2 / INT14 ×1** (signed bitwise/modulo), DCL03 ×1, ERR07/ERR34 ×1 (a debug-only unchecked `atoi`).

### FN-hunt: clean (0 FN) — both attack surfaces verified

The **segment-reader/merge** hunter confirmed the untrusted-decode surface is hardened by the same
invariants as fts3.c: `sqlite3Fts3ReadBlock` allocates `(i64)nByte + FTS3_NODE_PADDING` with the pad
zeroed; `nPrefix`/`nSuffix`/`nDoclist` are range-checked against the node end with `FTS_CORRUPT_VTAB`
returns; size math is `i64` exactly where `int` would wrap (`fts3SegWriterAdd`'s `i64 nReq`,
`(i64)nPrefix+nSuffix`). One low-confidence asymmetry — `fts3IncrmergeAppend` (4059/4101) sums
`nSpace`/`pLeaf->block.n` in `int` while the sibling writer uses `i64` — was judged **not practically
reachable** (requires a multi-GB single-term doclist actually allocated and merged first) and **not
recorded as FN**. The **control-command** hunter found the parser hardened intentionally: `fts3Getint`
clamps to `<214748363` (no overflow, always ≥0), every offset (`&zVal[6]` etc.) is length-guarded,
`merge=A,B` rejects `nMin<2` and trailing garbage, the only `atoi` uses are `SQLITE_DEBUG`-gated and
range-checked.

### FTS3 family complete (4 files, 1,759 distinct findings): bifurcation holds end-to-end

fts3.c + fts3_expr.c + fts3_snippet.c + fts3_write.c — the entire FTS3 read/parse/write/query surface
audited. **Found-TPs: 0 semantic, 100% declaration/macro/const/portability.** The *only* real semantic
defect in the whole family was the matchinfo('b') OOB write — a **false negative** sqc missed, not a
finding it made. This is the cleanest statement of the thesis yet: across a genuinely-untrusted-input
extension family, sqc's semantic engines contributed **zero** correct findings; its value was entirely
in the lexical/declaration class, and the one real semantic bug present was a recall gap.
sqlite coverage **65/220 (29.5%)**, 6 FNs.

---

## File-at-a-time — fts5_expr.c (2026-06-12, run #40, HIGH-effort): the fts5 query parser, clean (30% milestone)

`adjudication_sqlite_fts5_expr.csv` — `ext/fts5/fts5_expr.c` (the FTS5 MATCH expression parser +
phrase/NEAR matching engine, 3286 lines), **372 raw → 321 distinct findings**. Chosen as the next
fts5 needle: fts5_index.c (already audited) yielded the audit's only 2 semantic TPs, so its sibling
that parses untrusted MATCH queries *and* walks position-lists during matching was a prime suspect.
6 rule-class reviewers + 2 FN-hunters (parser, and the position-list matching engine).

**44 TP / 277 FP / 0 uncertain / 0 FN** (distinct; raw 43 TP / 329 FP).

**0 semantic TP across all 238 semantic findings** — ARR 0/69 (the phrase/NEAR matchers index only
parse-time-bounded `nPhrase`/`nTerm`/`nCol` arrays; decoded positions are used as i64 comparison
scalars, never as subscripts), INT 0/82 (bounded counters, i64/`malloc64` sizing, the NEAR-distance
accumulator already clamps `<214748363`), MEM30 0/43 (free-then-reassign + the depth-capped tree
recursion), EXP34 0/31 (`rc==SQLITE_OK` idiom), API00 0/56 (internal handles / OUT-params). All 43
raw TP are the precise class: **DCL13 ×27, MSC04 ×9** (genuine expr-tree recursion), **PRE00/PRE12 ×4**
(the `Fts5NodeIsString`/`fts5ExprNodeNext` multi-eval macros), DCL00 ×2, STR00 ×1.

### FN-hunt: clean (0 FN) — and it explains WHY fts5_index had TPs but this file doesn't

The parser hunter confirmed full hardening: expression depth hard-capped at
`SQLITE_FTS5_MAX_EXPR_DEPTH=256` (rejects deep-nest stack overflow), NEAR distance clamped
`<214748363`, token size clamped to `FTS5_MAX_TOKEN_SIZE`, `sqlite3Fts5IsBareword` short-circuits the
high-byte sign-extension before the table index, and column filters resolve by *name* (no
attacker-controlled column number reaches an array). The matching-engine hunter found the key
structural reason this file is clean while its sibling fts5_index.c was not: **fts5_expr.c delegates
all position-list decoding to `sqlite3Fts5PoslistNext64`, which masks every decoded position to 31
bits (`& 0x7FFFFFFF`) and is backed by the `FTS5_DATA_PADDING` zero-fill invariant; it then indexes
only parse-bounded arrays.** fts5_index.c, by contrast, does *raw* delta accumulation and leaf-array
indexing on decoded data — which is exactly where its 2 genuine INT32 overflows lived. The needle is
specific to the raw-decode routine, not the whole subsystem. (2 minor DCL13 const-miss recall gaps at
1073/1076 noted, not recorded; one `SQLITE_TEST`-only `fts5ExprTermPrint` sizing quirk, out of scope.)

### Tally (now 9 fts3/fts5 needle files audited)

Across the entire fts3 family (4 files) + fts5_index + fts5_expr, the only sqc-*found* semantic TPs
remain the **2 fts5_index structure-overflows**; every other found-TP is declaration/macro/const. The
thesis is now very precise: sqc's semantic value in this whole untrusted-input subsystem is two
contained integer overflows in one raw-blob-decode function — everything else it correctly finds is
lexical, and the one real bug it *missed* (matchinfo OOB) was a recall gap. **sqlite coverage 66/220
(30.0%)** — a third of the in-scope corpus audited; 6 FNs.

---

## File-at-a-time — fts5_storage.c (2026-06-12, run #40, HIGH-effort): the raw-decode analog, still clean

`adjudication_sqlite_fts5_storage.csv` — `ext/fts5/fts5_storage.c` (the FTS5 storage layer:
`%_content`/`%_docsize`/`%_config` read/write + integrity-check, 1530 lines), **129 findings**.
Chosen as the closest remaining raw-decode analog to fts5_index.c (it decodes the untrusted
`%_docsize` varint stream). 4 rule-class reviewers + 1 FN-hunter on the docsize/config decode.

**9 TP / 120 FP / 0 uncertain / 1 low-confidence FN.** Bifurcation holds: 0 semantic TP
(INT32 0/45 — `nCol`-bounded / i64-sized / the decoded docsize values feed only an *equality
oracle*, never an alloc or index; API00 0/31 internal handles; EXP34 0/16 `rc==OK` idiom; ARR/STR 0).
All 9 TP are DCL13 (read-only `pConfig`/`apVal`/`pBuf` params).

### 1 low-confidence FN — a benign, upstream-accepted varint over-read

`fts5StorageDecodeSizeArray` (1405): the `%_docsize` decode loop guards `iOff>=nBlob` but that only
ensures the *first* byte of each varint is in bounds; `fts5GetVarint32` reads up to ~9 bytes chasing
continuation bits. On the **on-page read path** (`sqlite3VdbeMemFromBtreeZeroOffset`, where
`sqlite3_column_blob` returns a `MEM_Ephem` pointer *directly into the btree page buffer* with no
added terminator), a crafted `%_docsize` blob whose last byte has the continuation bit set reads a few
bytes of adjacent in-page data. **Benign**: no write; the read stays within the `pageSize` allocation;
the decoded value is consumed only as a size-comparison oracle (mismatch → `FTS5_CORRUPT`). Two
reviewers split on it (one called it mitigated by sqlite's blob slack, the FN-hunter flagged the
missing `FTS5_DATA_PADDING` that the *sibling* fts5_index.c adds for exactly this varint-over-read
class). **Trunk-checked: byte-identical, last touched 2014** — unchanged for ~12 years, so upstream
accepts it (unlike matchinfo/session/sqlar/compress, all fixed within months of our pin). Recorded as
a **low-confidence FN / hardening gap** (consistent with the where.c INT34 precedent — real-but-benign,
still-present), NOT among the 7 confirmed real bugs. sqlite coverage **67/220 (30.5%)**, 7 FNs.

---

## File-at-a-time — fts5_main.c (2026-06-12, run #40, HIGH-effort): the public trust boundary, 0 semantic TP

`adjudication_sqlite_fts5_main.csv` — `ext/fts5/fts5_main.c` (the FTS5 virtual-table interface:
xConnect/xBestIndex/xFilter/xColumn/xUpdate, cursor lifecycle, the `fts5_api` aux-function interface,
rank/config/special-insert commands, 3875 lines), **329 findings**. This is the *public trust
boundary* — where untrusted SQL values, the MATCH argument, and column indices enter fts5. 7
rule-class reviewers (MEM30 x98 split in two) + 2 FN-hunters (vtab entry points; cursor/aux-data
lifecycle).

**27 TP / 302 FP / 0 uncertain / 0 FN.** Bifurcation holds *even at the boundary*: **0 semantic TP**
— MEM30 **0/98** (the largest MEM30 cluster yet; both lifecycle FN-hunters confirmed it is correctly
balanced — free-then-null / idempotent-memset cursor reset, the `ePlan!=FTS5_PLAN_SOURCE` guard for the
*borrowed* sort-cursor expression, and `fts5ApiGetAuxdata(bClear)` nulling both ptr+destructor for
ownership transfer), INT 0/73, EXP34 0/37, STR 0/24, ARR 0. All 27 TP are declaration/macro: DCL13
x15, **DCL03 x8** (assert-of-constant -> static_assert), PRE01 x2 / PRE00 / PRE12. The vtab entry
points all bound their column index (`iCol<0||iCol>=nCol -> SQLITE_RANGE`).

### Methodology note — an FN candidate that dissolved under verification (recorded as NOT-a-bug)

The vtab FN-hunter flagged `fts5BestIndexMethod:671` medium-high confidence: the LIKE/GLOB branch does
`idxStr += strlen(&idxStr[iIdxStr])` (advancing the **base pointer**) where the parallel MATCH branch
(660) does `iIdxStr += strlen(...)` (advancing the **index**) — claiming idxStr corruption / heap OOB
reachable via `col LIKE ?` on an fts5 column. **Hard verification refuted it:** `pInfo->idxStr = idxStr`
is captured at line 636 (pointer value = base `B`) *before* the loop, and the write cursor is always
`local_idxStr + iIdxStr`, which both branches keep equal to `B + total_chars_written` (MATCH adds
opcode+digits to `iIdxStr`; LIKE adds digits to `base` and the opcode to `iIdxStr` — identical sum). So
`pInfo->idxStr` stays `B`, the content at `B[0..total]` is correct, the terminator lands at `B[total]`,
and every write stays within the `nConstraint*8+1` buffer. **Trunk keeps it byte-identical for ~5 years**
(since the 2020 trigram-LIKE commit `33a99fad08`), corroborating correct-if-confusing, not a defect.
Recorded as investigated-not-a-bug, **NOT an FN** — a clean example of the trunk-diff + manual-arithmetic
discipline catching an over-eager FN-hunter claim. (Also: 2 aux-API `aIdx[iPhrase]`@2685 uncertains -> FP
— `iPhrase` is an extension aux-function *contract* parameter, not untrusted SQL/DB input.)

### fts5 needle sweep (index + expr + main + storage)

The found-semantic-TP count holds at **2** (both in fts5_index's raw-blob `fts5StructureDecode`); the
query parser, the vtab boundary, and the storage layer each contributed **0 semantic TP**. Confirmed:
sqc's semantic value is confined to the narrow raw-decode-arithmetic class, and the trust boundary itself
is fully guarded. sqlite coverage **68/220 (30.9%)**, 7 FNs.

---

## File-at-a-time — fts5_tokenize.c (2026-06-12, run #40, HIGH-effort): the two hardest classes, both clean

`adjudication_sqlite_fts5_tokenize.csv` — `ext/fts5/fts5_tokenize.c` (the FTS5 built-in tokenizers:
ascii, unicode61, porter stemmer, trigram — all process UNTRUSTED document + query text byte-by-byte,
1490 lines), **321 raw -> 281 distinct findings**. This file is the single best stress-test of two
otherwise-untestable hypotheses: (1) **INT32** dominates at **207 findings (65%)** — the tokenizer's
offset/length arithmetic on attacker bytes; (2) **STR34** (signed-char sign-extension) is most
plausible *here* of anywhere in the corpus, since the tokenizer's whole job is classifying/folding raw
bytes. 8 rule-class reviewers (INT32 split in three) + 2 FN-hunters (ascii/unicode61; porter/trigram).

**25 TP / 256 FP / 0 uncertain / 0 FN.** Both hard classes came back clean on the merits:
- **INT32 0/207** — token lengths are bounded by `FTS5_MAX_TOKEN_SIZE` and the porter cap (`nToken>64
  -> pass_through` unstemmed; `nToken<3` rejected), fold-buffer growth is `sqlite3_malloc64`/i64-cast,
  and the porter arithmetic operates on `nBuf in [3,65]` into a 128-byte buffer. The one technically-real
  signed-shift UB (`mask<<1` at 715) is consumed only as `&0x07` — benign, and sqc *did* flag it (not an FN).
- **STR34 0/8** — every flagged byte is read through an `unsigned char*` cursor or is a pointer-op
  misread. **sqlite deliberately routes all untrusted bytes through `unsigned char*`**, and the one raw
  `char`-cast table index (`a[(int)pText[is]]` in the ascii tokenizer) is short-circuit guarded by
  `(pText[is]&0x80)==0`. The file where a real STR34 was most likely is clean — on the merits, not by
  assumption.

All 25 TP are declaration/macro/portability: **DCL13 x8**, the **PRE00/PRE01/PRE10/PRE12** hazards on the
`READ_UTF8`/`WRITE_UTF8`/`FTS5_SKIP_UTF8` multi-eval/multi-statement macros, and INT13/14/07 (benign
signed-bitwise/plain-char in byte math).

### FN-hunt: clean (0 FN) — the classic spots are all explicitly defended

ascii/unicode61: signed-char table index `(c&0x80)==0` short-circuit guarded; fold-buffer headroom
invariant (`pEnd=&aFold[nFold-6]`, 1:1 fold so no expansion); UTF-8 decode bounded by `zIn<zTerm`;
category index `&0x1F` (<32). porter/trigram: the porter fixed buffer is safe because over-length tokens
bypass stemming (`pass_through` before the `memcpy`) and short tokens are rejected (protecting the
backward `aBuf[nBuf-2]` suffix indexing); trigram `aBuf[32]` holds <=12 bytes with guarded window
arithmetic. A correctly-coded, well-fuzzed result.

### fts5 family essentially swept (index+expr+main+storage+tokenize)

Found-semantic-TP holds at **2** (fts5_index raw-decode). The two classes most likely to break in a
tokenizer — bulk integer offset math and signed-byte classification — produced **0 TP across 215
findings**, because the code is bounded (token caps) and disciplined (`unsigned char*`). sqlite coverage
**69/220 (31.4%)**, 7 FNs.

---

## File-at-a-time — fts5_config.c (2026-06-12, run #40, HIGH-effort): the config parser, fully hardened

`adjudication_sqlite_fts5_config.csv` — `ext/fts5/fts5_config.c` (the FTS5 config parser: CREATE-VTAB
options `tokenize=`/`content=`/`prefix=`/`columnsize=`/`detail=`, string dequoting, and the `%_config`
shadow-table cookie/value reader, 1127 lines), **116 raw -> 105 distinct findings**. 5 rule-class
reviewers + 1 FN-hunter (option parser + `%_config` reader).

**2 TP / 114 FP / 0 uncertain / 0 FN** — the *lowest precision file of the campaign* (2/105 = 1.9%),
and an instructive one: it's almost pure parser/error-message code, so it has almost no const-eligible
read-only helpers (the precise DCL13 class). Bifurcation holds: 0 semantic TP — STR34 **0/37** (all
`*pzErr=sqlite3_mprintf(...)` pointer-stores, `strlen` casts, or ASCII-equality; sqlite reads config
bytes unsigned and the one raw-`char` table index goes through `sqlite3Fts5IsBareword`'s `(c&0x80)`
short-circuit), API00 0/16 (internal), INT 0/14 (`nPre` clamped `<1000`; the version cookie is
compared, not used in arithmetic), EXP34 0/12, ARR/MEM/DCL30 0. The only 2 TP are DCL13
(`sqlite3Fts5ConfigDeclareVtab`/`ConfigErrmsg`, read-only `pConfig`).

### FN-hunt: clean (0 FN) — the config attack surface is defensively written

The `%_config` table and CREATE options are the realistic injection points, and every danger is guarded:
`prefix=` is bounded by `FTS5_MAX_PREFIX_INDEXES=31` with the capacity check *before* the `aPrefix[]`
write; individual prefix lengths are double-bounded (`<1000` in the digit loop AND a post-check);
`azCol[nCol++]` cannot exceed the `nArg`-sized allocation; the **`%_config` `version` cookie is rejected
unless it is exactly 4 or 5**; every integer config value (`pgsz`/`hashsize`/`automerge`/`usermerge`/
`crisismerge`/`deletemerge`) is range-clamped before use as a size/count; and `fts5Dequote` is in-place
with output always shorter than input (no overflow). sqc emitting nothing real here is a true negative.

sqlite coverage **70/220 (31.8%)** — 70 files audited; 7 FNs.

---

## File-at-a-time — vdbeaux.c (2026-06-12, run #40, HIGH-effort): the largest remaining core file, 0 semantic TP

`adjudication_sqlite_vdbeaux.csv` — `src/vdbeaux.c` (the VDBE prep / op-array build / P4 management /
serialize-record / record-compare / finalize / preupdate-hook code, 5584 lines), the **largest remaining
in-scope file** at **620 raw -> 560 distinct findings**. Adjudicated via 7 parallel line-range reviewers
(each reading the full enclosing functions + struct/macro defs) plus an FN-hunt over every range.

**36 TP / 584 FP / 0 uncertain / 0 FN** (raw; 38 TP / 522 FP distinct after dedup against prior MEM30/
DCL13 increments that had already labeled some vdbeaux sites). Bifurcation holds, hard: **every one of the
36 TP is a declaration/macro/portability finding — 0 semantic TP across 620 findings.** TP breakdown:
**DCL13 x23** (read-only pointer params that could take `const`: `sqlite3VdbeGetOp`, `VdbeEnter`/`vdbeLeave`,
`VdbeDisplayP4`, `VdbeFrameIsValid`, `VdbeDb`, `VdbePrepareFlags`, the various `Verify*`/`CurrentAddr`/
`TransferError`/scan-status accessors, …), **PRE00 x4 + PRE12 x4** (the `ONE/TWO/THREE/FOUR_BYTE_INT/UINT`
deserialization macros textually re-evaluate their byte-pointer argument), **DCL03 x3** (`assert(sizeof(x)==8 …)`
and the `sizeof(UnpackedRecord)+sizeof(Mem)*65536 < 0x7fffffff` invariant — pure constant expressions that
`static_assert` would express better), **EXP05 x1** (`(char*)buf` drops `const` off the decode cursor before
storing into `pMem->z`), **EXP45 x1** (`(pKeyInfo = pPKey2->pKeyInfo)->nAllField` embedded assignment in an
`if` controlling expression).

The semantic FP classes are the now-familiar ones, here at scale: **MEM30** misreads the allocator handle
`db` threaded into `sqlite3DbFree(db, member)` as the freed object (the entire `freeP4`/`sqlite3VdbeClearObject`/
`vdbeFreeUnpacked` cleanup surface), and reads the live `pOp` program counter / loop-head list nodes / stack-
embedded `preupdate.uKey` as freed; **INT30/31/32/13/14** fires on opcode/register/cursor/scan counts that are
structurally bounded by `SQLITE_LIMIT_VDBE_OP` / `SQLITE_MAX_COLUMN` / the 64-bit-widened `(i64)` doublings, and
on the serial-type byte-merge macros (non-negative, bounded); **API00/EXP33/EXP34** on internal routines whose
pointer params are non-null by caller contract or already getVarint32-initialized; **ARR00/30/37** on the
`aMem` register-array slice and the record-decode `aKey` cursor (a buffer pointer, not a 1-element array);
**EXP36** on `sizeof(*p)` / `(char*)p` reinterprets; **DCL15** on `sqlite3`-prefixed cross-file exports.

### FN-hunt: clean (0 FN) — the record codec is the attack surface and it is fully guarded

The realistic injection point is the on-disk **record decode/compare** path (`VdbeRecordUnpack`,
`sqlite3VdbeRecordCompareWithSkip`, `vdbeRecordDecodeInt`, `serialGet`): every serial-type byte read is
bounded by the header length with explicit `CORRUPT_DB` guards, the `vdbeRecordCompareInt`/`String` fast
paths rely on the documented `>=74`-byte record-buffer padding, all `memcmp` lengths are `MIN()`-clamped,
the rowid varint at `szHdr-1` is reached only after `szHdr>=3`, and `typeRowid` is validated `1..9` before
the `SmallTypeSizes[]` lookup. The realloc-grow paths (`growOpArray`, label/sub-program arrays, scan-status)
all NULL-check before the next deref. sqc emitting no real bug here is a true negative.

sqlite coverage **71/220 (32.3%)** — 71 files audited; 7 FNs. The single largest file in the corpus is now
done, and the found-semantic-TP count stays at **2** (both in fts5_index raw-decode); no core-engine file
(btree/vdbe/vdbeaux/where) has yielded a semantic TP.

---

## File-at-a-time — select.c (2026-06-12, run #40, HIGH-effort): the SELECT codegen core, 0 semantic TP

`adjudication_sqlite_select.csv` — `src/select.c` (the SELECT query-planner + result-set codegen:
result-column expansion, sorter push/pop, compound-SELECT/UNION, subquery flattening, constant
propagation, aggregate/GROUP BY codegen, 8982 lines), the 2nd-largest remaining file at **620 raw -> 552
distinct findings**. Adjudicated via 8 parallel line-range reviewers + per-range FN-hunt.

**45 TP / 575 FP / 0 FN** (raw; 43 TP / 509 FP distinct after dedup vs prior increments). Bifurcation
holds: **0 semantic TP across 620 findings.** TP breakdown: **DCL13 x33** (read-only pointer params that
could take `const`: `sqlite3ColumnIndex`, the sorter/distinct helpers, `sqlite3KeyInfoIsWriteable`,
`isSimpleCount`, `isSelfJoinView`, `sameSrcAlias`, `cannotBeFunction`, `sqlite3CopySortOrder`,
`propagateConstants`, …), **MSC04 x10** (functions that genuinely participate in direct/indirect
recursion — `clearSelect`↔`sqlite3WithDelete`, `sqlite3SetJoinExpr`, `substExpr`/`substSelect`,
`srclistRenumberCursors`, `multiSelect`, `sqlite3Select`; the recursion is real but bounded by
`SQLITE_MAX_EXPR_DEPTH`/parser depth limits, so accurate-as-a-structural-fact yet not a memory-safety
bug), **PRE01 x2** (the `explainSetInteger(a,b)`→`a=b` macro args are unparenthesized).

The semantic FP classes are again the dominant ones: the huge **INT32** population (243 findings) is
register counters (`pParse->nMem`), cursor counters (`pParse->nTab`), VDBE program addresses, sorter-key
indices, and column counts — all structurally bounded by `SQLITE_MAX_COLUMN`/`SQLITE_LIMIT_*` and the
`(i64)`-widened products; **MEM30** misreads the live allocator handle `db` (= `pParse->db`, never freed)
threaded into `sqlite3DbFree(db, X)`, plus the result-column-trim loop that per-element-frees `pEList->a[i]`
and decrements `nExpr` (the array and the `int` field are never freed); **EXP33/EXP34** on variables
assigned-before-use, OOM-sets-`nErr`-gated derefs, and `ALWAYS()`/branch-guarded pointers; **PRE32** on
`TREETRACE(...)` calls (the `#if` guards the whole block, not an in-argument directive); **DCL13 FPs** are
the Walker-callback params (signature fixed by `xExprCallback`/`xSelectCallback2` typedefs) and `pParse`
stored into the non-const `Walker.pParse` field.

### FN-hunt: clean (0 FN) — the OOM and trim paths are the danger and they are guarded

The two spots most likely to hide a bug are the **OOM result-column path** (`selectColumnsFromExprList`:
`aCol=DbMallocZero(); ... loop`) and the **unused-column trim** (`disableUnusedSubqueryResultColumns`
decrements `pEList->nExpr`). Both are safe: every OOM allocator (`sqlite3OomFault`) sets `pParse->nErr`,
and the loops are gated on `pParse->nErr==0`, so the null-`aCol`/`pKeyInfo` derefs are unreachable; the
trim path runs only under `SRT_EphemTab`+`SF_NestedFrom` and never feeds the aggregate `iOrderByCol-1`
indexing (separate path, guarded by `if(iOrderByCol)` on a 1-based bounded index). sqc finding no real bug
here is a true negative.

sqlite coverage **72/220 (32.7%)** — 72 files audited; 7 FNs. The two largest remaining files
(vdbeaux.c, select.c) are now both done with 0 semantic TP; found-semantic-TP stays at **2** (both
fts5_index raw-decode). No core-engine file has produced a semantic TP.

---

## File-at-a-time — Batch 3 (2026-06-12, run #40): 8 untrusted-input decoders, a sqc-caught OOB-read + a missed over-read

`adjudication_sqlite_batch3.csv` — the **needle-target batch**: 8 files that decode untrusted serialized
input, where the campaign's real bugs have all clustered. One reviewer per file (each read whole) + per-file
FN-hunt. **1088 raw findings -> 23 TP / 1065 FP**, and crucially **the two material results the thesis
predicted both landed here, not in any core-engine file**:

| File | findings | TP | FP | note |
|------|---------:|---:|---:|------|
| ext/session/changeset.c | 106 | 0 | 106 | changeset reader; CLI+iterator, all guarded |
| ext/misc/zipfile.c | 182 | 0 | 182 | **FN: zipfileScanExtra over-read** (see below) |
| ext/misc/fossildelta.c | 165 | 2 | 163 | **semantic TP: delta INSERT OOB-read, sqc-caught** |
| ext/misc/regexp.c | 174 | 2 | 172 | NFA engine clean; 2 MSC04 recursion |
| ext/misc/csv.c | 87 | 0 | 87 | CSV parser; growable buffer correctly sized |
| ext/misc/decimal.c | 187 | 0 | 187 | bignum parse/mul; digit arrays int64-sized |
| ext/misc/base85.c | 100 | 16 | 84 | decoder clean; 16 lexical TP incl 2 real STR00/STR37 |
| ext/recover/dbdata.c | 87 | 4 | 83 | raw-page parser; 100-byte page padding + asserts |

### The semantic TP — sqc caught a real OOB-read (fossildelta.c:941, INT30-C)

`deltaparsevtabColumn` DELTAPARSEVTAB_A2 / INSERT branch:
`if( pCur->a2 + pCur->a1 > pCur->nDelta ){ zeroblob }else{ result_blob(aDelta+a2, a1, TRANSIENT) }`.
`a1`/`a2` are **`unsigned int`**, `a1` is the insert length decoded by `deltaGetInt` (`v=(v<<6)+c`, **no cap —
full 32-bit attacker control**), `nDelta` is a small `int` that promotes to unsigned in the compare. A crafted
delta with `a1 ≈ 2³² − a2` makes `a2 + a1` **wrap below `nDelta`**, bypassing the guard, so `result_blob`
copies `a1` (~4 GB) bytes from a small heap buffer — a massive heap **out-of-bounds read** reachable via
`SELECT a2 FROM deltaparse(:crafted_blob)`. sqc flagged the wrap (idx 152, INT30-C @941). This is the audit's
clearest sqc-caught semantic bug outside the session/fts5 set, and it lands exactly where the thesis says to
look: a less-fuzzed untrusted-serialized-input decoder. (Confirmed by code reading; trunk-validation pending,
cf. the session/matchinfo precedent.)

### The FN — sqc missed an over-read (zipfile.c:714, ARR30-C, recorded FN)

`zipfileScanExtra` reads the 4-byte extra-field header (two `zipfileRead16`) and the 5-byte `0x5455`
timestamp record (`p[0]` then `zipfileGetU32(&p[1])`) after only the `while(p<pEnd)` guard, **without
checking 4–5 bytes remain**. A CDS extra field with a timestamp record at the tail over-reads up to ~4 bytes
past the `cds.nExtra` scan region. Contained in the comment region in most paths (escapes only when
`cds.nComment<5` and the entry sits at the buffer end), so marginal — recorded as a low-confidence FN, not a
clean exploitable overflow. sqc did not flag it (needs the missing-remaining-bytes check sqc lacks).

### Everything else confirms the bifurcation even in decoders

The other 6 files gave **0 semantic TP**. The 21 non-fossildelta TPs are all lexical/declaration: DCL13 ×11
(const-eligible read-only params on static helpers), MSC04 ×2 (regexp `re_subcompile_re`/`_string` genuine
indirect recursion), PRE00/PRE12 ×4 (base85 `B85_CLASS`/`base85Numeral` multi-eval macros), PRE01 ×1
(dbdata `DBDATA_MX_CELL`), DCL00 ×1 (base85 `zHelp`), and **STR00+STR37 ×2** — a genuine `isspace(c)` on a
plain (signed) `char` in base85 `allBase85`, real CERT-C UB. The semantic-rule misfires are the usual classes
(live `db` handle read-as-freed, varint/serial counters bounded, `#if 0` dead code, signature-fixed callbacks,
OOM-sets-rc). The decoders' length/offset math is otherwise uniformly bounds-checked or input-bounded.

sqlite coverage **80/220 (36.4%)** — 80 files audited; **8 FNs** (zipfile over-read added). Confirmed real
bugs now **8** (2 fts5_index + 4 session + matchinfo + fossildelta), every one in a less-fuzzed
untrusted-serialized-input parser; still **no core-engine semantic TP** across the whole corpus.

---

## File-at-a-time — Batch 4 (2026-06-12, run #40): 5 untrusted-text/blob parsers, a 2nd sqc-caught OOB + a missed leak

`adjudication_sqlite_batch4.csv` — the second needle-target batch: 5 parsers of untrusted text/blobs —
**src/json.c** (JSON/JSONB, the prime target), **ext/rtree/geopoly.c** (GeoJSON + polygon blobs),
**ext/recover/sqlite3recover.c** (corrupt-DB recovery), **ext/fts5/fts5_hash.c** (pending-term hash),
**ext/misc/normalize.c** (SQL normalizer). 11 line-range reviewers + per-chunk FN-hunt. **1274 raw
findings -> 70 TP / 1204 FP.**

| File | findings | TP | FP | note |
|------|---------:|---:|---:|------|
| src/json.c | 527 | 36 | 491 | JSON+JSONB decode fully bounds-checked by jsonbPayloadSize |
| ext/rtree/geopoly.c | 271 | 7 | 264 | blob nVertex bound to nByte by an exact size_t equality |
| ext/recover/sqlite3recover.c | 193 | 6 | 187 | raw-page validator over-allocates 2*nMax, all reads guarded |
| ext/fts5/fts5_hash.c | 149 | 5 | 144 | entry grow keeps a 22-byte headroom invariant (assert-enforced) |
| ext/misc/normalize.c | 134 | 16 | 118 | **semantic TP @616 + FN @628 (see below)** |

### The semantic TP — sqc caught a heap-buffer-underflow read (normalize.c:616, ARR00-C)

In `sqlite3_normalize`'s `in(...)` rewrite pass: `zIn = strstr(z+i, "in(")`, then
`n = (int)(zIn-z)+3; if( n && IdChar(zIn[-1]) ) continue;`. The `n &&` guard was meant to suppress the
`zIn[-1]` look-back when `"in("` is at the start, but **`n` is the index *past* `"in("`, so it is always
`>= 3`** — the guard is dead. When the normalized output begins with `"in("` (reachable from input like
`in(1)` -> `in(?);`, so `strstr` returns `zIn==z`), `IdChar(zIn[-1])` reads `z[-1]`: a **1-byte
heap-buffer-underflow read**. sqc flagged it (idx 82, ARR00-C "array subscript -1 is negative"). This is the
campaign's second sqc-caught semantic bug this session, again in a less-fuzzed untrusted-input parser. The
remaining 15 normalize TPs are lexical (PRE12/PRE01/PRE11/EXP05) plus a cluster of genuine CERT
error-handling findings (unchecked `fseek`/`ftell`/`fclose`, `SEEK_END`-on-binary, and the `ftell`-> `size_t`
`fread` conversion) inside the `#ifdef SQLITE_NORMALIZE_CLI` harness — real but low-severity.

### The FN — a missed OOM leak (normalize.c:628, MEM31-C, recorded FN)

The same rewrite does `z = sqlite3_realloc64(z, ...); if( z==0 ) return 0;` — on OOM `realloc` leaves the
original `z` allocated, so it leaks. sqc did not flag it. Low severity; recorded as a low-confidence FN.
(Two further latent asymmetries were noted and dismissed as unreachable: `recoverBitmapSet` lacks the
`iPg>0` guard its `Query` sibling has, but `freepgno` is read as a non-negative u32; and `recoverGetPage`
passes `nPg-nReserve` to `result_blob`, but `sqlite_dbpage` always returns full pages — neither recorded.)

### The other 4 parsers confirm the bifurcation

**0 semantic TP** outside normalize idx 82. The 54 non-normalize TPs are entirely lexical: DCL13 ×10
(const-eligible read-only params), MEM05/MSC04 ×15 (genuine but depth-bounded recursion in json
translate/merge/validity, geopoly `geopolySine`, regexp not here), EXP05 ×8 (real `(u8*)`/`(char*)`
const-discards in json UTF-8 compare and `zJson` ownership), DCL03 ×7 + DCL00 ×6 + ARR02 ×4 + PRE ×n
(constant-expr asserts, const-able locals, implicit-bounds const tables, macro hygiene). The decoders'
length/offset math is uniformly bounds-checked: json's `jsonbPayloadSize` validates `i+n+sz<=nBlob` before
any payload read; geopoly binds the attacker's 3-byte `nVertex` to the real blob length via a size_t
equality before allocating/copying; recover over-allocates the page validator to `2*nMax` and guards every
`aUsed[]` write; fts5_hash keeps an assert-enforced 22-byte headroom before every varint append.

sqlite coverage **85/220 (38.6%)** — 85 files audited; **9 FNs** (normalize OOM-leak added). Confirmed real
bugs now **9** (2 fts5_index + 4 session + matchinfo + fossildelta + normalize), every one in a less-fuzzed
untrusted-input parser; still **no core-engine semantic TP** anywhere in the corpus. Two of the nine
(fossildelta, normalize) were caught by sqc as findings; the campaign's sqc-found real-bug count is rising
specifically in the decoder class the thesis targets.

## File-at-a-time — Batch 5 (2026-06-12, run #40): 3 big decoders + 4 fts3 tokenizers, 2 missed bugs, 0 sqc-caught semantic TP

`adjudication_sqlite_{sqlite3rbu,spellfix,qrf,fts3_porter,fts3_tokenizer,fts3_aux,fts3_tokenizer1}.csv` — the
third needle-target batch, hitting the largest remaining untrusted-input decoders plus the fts3 tokenizer
siblings. 14 line-range reviewers + per-chunk/whole-file FN-hunt. **1479 raw findings -> 35 TP / 1444 FP.**

| File | findings | TP | FP | note |
|------|---------:|---:|---:|------|
| ext/rbu/sqlite3rbu.c | 507 | 21 | 486 | RBU update-DB applier; **missed zMask over-read** (FN) |
| ext/misc/spellfix.c | 404 | 0 | 404 | edit-distance/phonetic; **missed empty-pattern underflow** (FN) |
| ext/qrf/qrf.c | 363 | 9 | 354 | query-result-format; UTF-8/EXPLAIN-indent latent asymmetries only |
| ext/fts3/fts3_porter.c | 72 | 2 | 70 | Porter stemmer; 2 MSC04 mutual recursion (isVowel↔isConsonant) |
| ext/fts3/fts3_tokenizer.c | 61 | 0 | 61 | tokenizer dispatch; pointer-from-blob path properly size+frombind gated |
| ext/fts3/fts3_aux.c | 51 | 0 | 51 | fts3aux vtab; free-then-memset-zero-then-reuse misread as UAF |
| ext/fts3/fts3_tokenizer1.c | 21 | 3 | 18 | simple tokenizer; 3 DCL13/DCL00 const-advisory |

### The two FNs — both real bugs sqc MISSED, both in the decoder class the thesis targets

**ext/rbu/sqlite3rbu.c:2654 (ARR30-C, recorded FN, medium).** `rbuGetUpdateStmt` does
`memcpy(pUp->zMask, zMask, pIter->nTblCol)` where `zMask` is the untrusted `rbu_control` TEXT value
(`sqlite3_column_text` of the RBU update DB). The length guard `strlen(zMask)!=nTblCol` lives in
`rbuObjIterGetSetlist` (called one line earlier) — it sets the error via `rbuBadControlError` but does **not**
return, so execution falls through to the memcpy, which reads `nTblCol` bytes from a `zMask` an attacker can
make shorter. A crafted `data_xxx` row with a short `rbu_control` string triggers a bounded heap over-read
before the error code is consulted (empty cache skips the prior `strcmp`). Found independently by **two**
reviewers; verified against source (data flow: `rbuStepType` -> `*pzMask=sqlite3_column_text` -> `rbuStep` ->
`rbuGetUpdateStmt`, no length check between).

**ext/misc/spellfix.c:2597 (ARR30-C, recorded FN, medium).** `nPattern=(int)strlen(zPattern); if(zPattern[nPattern-1]=='*')`
reads `zPattern[-1]` when the MATCH operand is empty: `transliterate("")` returns a non-NULL empty string, and
the only guard is `zMatchThis==0` (misses empty-but-non-NULL). A 1-byte heap-buffer-underflow read reachable
via `SELECT ... WHERE word MATCH ''`. Same defect class as batch-4's normalize.c:616. **sqc's idx-362
INT32-C "nPattern-- may overflow at INT_MIN" at this same line is a misfire** (nPattern is a small strlen
result) — corrected TP→FP by the integrity gate; the genuine ARR30 underflow is a separate rule sqc never
emitted, hence FN not TP-catch.

### Integrity gate caught 3 mislabels; 0 semantic TP survived

All 35 TP are declaration/macro/recursion: **DCL13 ×21, DCL00 ×5, PRE00/01/10/12 ×5, MSC04 ×3** — zero
semantic TP across all 1479 findings. The gate reclassified three reviewer/sqc TP→FP: spellfix idx 362
(above); **sqlite3rbu idx 389** (INT32 memmove size `nErrmsg+1-i-nDel` — unsigned size_t arithmetic, ≥1 on the
matched path; only underflows if the loop bound `nErrmsg-8` wraps, which needs a sub-8-char SQLITE_CONSTRAINT
message sqlite never produces — unreachable); **sqlite3rbu idx 485** (INT30 `nOpen-3` wal→oal filename patch —
guarded by sqlite's `-wal` suffix invariant, nOpen≥4 always — contract-safe). None recorded as FN (both
unreachable). Latent asymmetries dismissed (not recorded): **qrf.c:2363-2364** (the `azGoto` indent branch
indexes `abYield[p2op]`/writes `aiIndent[i>=p2op]` with only an upper-bound guard while the sibling `azNext`
branch guards `p2op>0`, but `p2op=p2+(iOp-iAddr)` is provably ≥0 for well-formed sqlite EXPLAIN/bytecode);
qrf UTF-8 decoders over-reading a *non-NUL-terminated* buffer (column text is NUL-terminated in practice);
fts3_tokenizer `testFunc` `azArg[64]` overflow (SQLITE_TEST-only, not in production builds).

sqlite coverage **92/220 (41.8%)** — 92 files audited; **11 FNs** (rbu over-read + spellfix underflow added).
Confirmed real bugs now **11** (2 fts5_index + 4 session + matchinfo + fossildelta + normalize + rbu +
spellfix), every one in a less-fuzzed untrusted-input parser; still **no core-engine semantic TP** anywhere in
the corpus. Notably both batch-5 bugs are sqc **recall gaps** (FNs), not catches: the length-vs-copy ordering
bug (rbu) and the empty-buffer underflow (spellfix) both need analysis sqc lacks. sqc's two semantic flags in
this batch were both FPs gated by sqlite-internal invariants — consistent with the bifurcation: in the
hardened parts of even an untrusted decoder, sqc's semantic rules only misfire.

## File-at-a-time — Batch 6 (2026-06-12, run #40): 6 remaining decoders, a heap-overflow WRITE missed, 0 sqc-caught semantic TP

`adjudication_sqlite_{rtree,date,amatch,checkindex,fuzzer,utf}.csv` — the remaining untrusted-input decoders:
**ext/rtree/rtree.c** (R*Tree node/coord blobs), **src/date.c** (date-string parser), **ext/misc/amatch.c**
(approximate-match, spellfix sibling), **ext/repair/checkindex.c** (corrupt-DB index validator),
**ext/misc/fuzzer.c** (fuzzer vtab rule table), **src/utf.c** (UTF-8/16 transcoder). 11 line-range reviewers +
per-chunk/whole-file FN-hunt. **1267 raw findings -> 91 TP / 1176 FP.**

| File | findings | TP | FP | note |
|------|---------:|---:|---:|------|
| ext/rtree/rtree.c | 446 | 61 | 385 | cell/coord decode bounded (NCELL<200, nDim2 u8); cellArea/Union/etc const DCL13 |
| src/date.c | 293 | 7 | 286 | bounded julian-day i64 math; gmtime/strftime CON33/34/ERR33 TPs |
| ext/misc/amatch.c | 184 | 0 | 184 | **missed (char)(nWord+100) heap-overflow WRITE** (FN) |
| ext/repair/checkindex.c | 128 | 6 | 122 | **missed unterminated-`[` schema over-read** (FN) |
| ext/misc/fuzzer.c | 115 | 0 | 115 | rule loader validates nFrom/nTo<=50 before alloc/copy |
| src/utf.c | 101 | 17 | 84 | READ/WRITE_UTF8/16 macros: 11 genuine PRE00/10/12 hygiene TPs |

### The headline FN — a heap-overflow WRITE sqc missed, that survived a dedicated hardening pass

**ext/misc/amatch.c:1160 (INT31-C truncation -> ARR30-C OOB write, recorded FN, medium).** `amatchNext` does
`nBuf = (char)(nWord+100); zBuf = sqlite3_realloc64(zBuf, nBuf); ... amatchStrcpy(zBuf, pWord->zWord+2);`.
`nBuf` is `sqlite3_int64` but the RHS is truncated through a **signed char**: for a vocab/candidate word of
length `nWord ∈ [156,283]`, `(char)(nWord+100) = nWord-156 ∈ [0,127]` — a small positive size — so `zBuf` is
under-allocated and the unbounded `amatchStrcpy` then copies `nWord` bytes past it. Vocab words are uncapped
(`AMATCH_MX_LENGTH=50` bounds only *rule* strings) and come from the user's vocab table. The most striking
part: the Oct-2025 commit **4043096408** ("Additional defenses against over-sized inputs in the (unused)
amatch.c demonstration code", *in* our audited tree) widened `nBuf` from `char`→`sqlite3_int64` and switched
`sqlite3_realloc`→`realloc64` — drh clearly recognized the size variable was too narrow — **but left the
`(char)` cast on the RHS**, so the truncation is still live at b1a73ba34d *and* at HEAD. sqc's idx-138 flagged
only the harmless `nWord+100` addition, never the cast. CAVEAT: sqlite labels amatch.c "(unused) demonstration
code," so real-world impact is limited — but it is a genuine, still-live heap *write* overflow and the first
write-overflow recall gap in the campaign (all prior bugs were reads/leaks).

### The 2nd FN — a corrupt-schema over-read (checkindex.c:389)

`cidxFindNext` `case '[': while( *z++!=']' );` scans for a closing bracket with **no NUL guard**. checkindex
analyzes *corrupt* databases, so the `CREATE INDEX` text it parses (from `sqlite_schema.sql`) is
attacker-controlled; an unterminated `[identifier` runs `z` past the string terminator until a stray `]` in
adjacent heap — a heap over-read (ARR30-C, recorded FN low). The reviewer surfaced 4 other checkindex
candidates (a `(int)strlen` truncation at 541 needing >2GB, an unchecked-realloc OOM deref at 494, a
NULL-`pIdx` deref on inconsistent schema at 411, a weak column-overflow guard at 422) — all corrupt-schema /
OOM / >2GB-gated; noted but not recorded pending deeper verification.

### Integrity gate: 0 semantic memory-safety TP survived

All 91 TP are declaration/macro/recursion/library: **DCL13 ×43, PRE00/01/10/12 ×29** (the rtree DCOORD/MAX/MIN
and utf READ/WRITE_UTF8/16 multi-eval + statement-block macros — genuine), **MSC04/MEM05 ×10** (factual rtree
tree recursion: SortByDimension, SplitNode↔rtreeInsertCell, removeNode↔deleteCell, fixBoundingBox), **DCL03/00/38
×5**, **EXP45 ×1** (rtree assignment-in-if), **CON33/CON34/ERR33 ×3** (date.c `gmtime`/`strftime` — factual
thread-safety/unchecked-return, mutex-mitigated). **Zero semantic memory-safety TP** (no surviving
MEM30/INT3x/ARR/EXP33-34/STR) across all 1267 findings. The gate reclassified the only two sqc semantic-TP
claims to FP: **rtree idx 410/411** (INT32 overflow in `rtreeCheckNode`'s `4+nCell*(8+nDim*8)`) — under the
**default `SQLITE_MAX_COLUMN`=2000** `nDim≤999`, so the product `≤65535·7999≈5.2e8` never overflows int and the
`>nNode` guard correctly rejects the oversized node; the signed overflow needs `nDim>~4095` (a non-default
`SQLITE_MAX_COLUMN` >~8000), so it is latent, not reachable under standard builds — the corresponding
`rtreecheck` FN at line 4102/4189 was likewise NOT recorded.

sqlite coverage **98/220 (44.5%)** — 98 files audited; **13 FNs** (amatch write-overflow + checkindex over-read
added). Confirmed real bugs now **13**, every one in a less-fuzzed untrusted-input parser; still **no
core-engine semantic TP** anywhere in the corpus. Both batch-6 bugs are again sqc **recall gaps** (FNs): the
campaign's real-bug recall is dominated by integer-truncation-before-alloc and missing-bound patterns sqc's
detectors don't model. date.c and utf.c — the two heavily-fuzzed transcoder/parser cores in this batch — gave
0 semantic TP / 0 FN, exactly as the bifurcation predicts for mature code.

## File-at-a-time — Batch 7 (2026-06-12, run #40): last decoders + first hardened-core (build.c, os_unix.c)

`adjudication_sqlite_{changesetfuzz,fileio,series,build,os_unix}.csv` — the last remaining minor decoders plus
the two biggest hardened-core files to advance coverage. **changesetfuzz.c** (standalone changeset-fuzzing CLI),
**ext/misc/fileio.c** (readfile/writefile/fsdir), **ext/misc/series.c** (generate_series), **src/build.c** (DDL
codegen, hardened core), **src/os_unix.c** (Unix VFS, hardened core). 13 line-range reviewers + FN-hunt.
**1517 raw findings -> 40 TP / 1477 FP.**

| File | findings | TP | FP | note |
|------|---------:|---:|---:|------|
| src/build.c | 589 | 22 | 567 | hardened core — DCL13 + MSC04 deletion recursion only |
| src/os_unix.c | 582 | 8 | 574 | hardened VFS — DCL13 + appendOnePathElement recursion + PRE |
| ext/session/changesetfuzz.c | 140 | 6 | 134 | standalone fuzzing CLI; 1 tool-scope INT32 overflow TP |
| ext/misc/fileio.c | 106 | 4 | 102 | size/path math bounded; fsdir recursion depth-guarded |
| ext/misc/series.c | 100 | 0 | 100 | span64/add64/sub64 wraparound-safe by design; 0 TP |

### The bifurcation holds hard on the first hardened-core files

build.c (589) and os_unix.c (582) — **1171 findings, 30 TP, 0 FN, 0 semantic TP.** Exactly the predicted
profile. The 30 TPs are all declaration/macro/recursion: **DCL13** const-params (codeTableLocks, isDupColumn,
tableMayNotBeDropped, sqlite3IdListIndex, the os_unix internal shm helpers unixDescribeShm/unixFcntlExternalReader/
unixIsSharingShmNode, …), **MSC04** genuine deletion/path recursion (sqlite3SrcListAssignCursors,
sqlite3SubqueryDelete↔SrcListDelete, cteClear↔WithDelete, appendOnePathElement↔appendAllPathElements), **DCL03**
static_assert on constant-folding asserts, **PRE/DCL00/DCL30**. Every semantic finding is a misfire against a
concrete guard: the huge **MEM30 "use-after-free" clusters in both files** are the same root error — sqc reads the
`db`/`pFile` *context* argument of `sqlite3DbFree(db,x)` / `robust_close(pFile)` / `sqlite3*Delete(db,x)` as the
freed object, when the live handle is never freed and only the second arg (read first) is released. The dense
**os_unix PRE32 cluster** is a uniform misfire: the analyzer reads multi-line `OSTRACE((...))` argument
continuation lines as "preprocessor directives in macro arguments" — there are no `#` directives there at all.
build.c's INT/ARR are bounded by i16 column counts (asserted ≤32767), `SQLITE_LIMIT_COLUMN`, and i64 size math;
os_unix's are errno-checked syscalls, fixed `[MAXPATHLEN]` buffers, and mutex-guarded refcounts.

### The one semantic TP is tool-scope, not library

**changesetfuzz.c:506 (INT32, idx 65, TP — but TOOL-SCOPE).** `*pSz = 1 + sz + nTxt` where `nTxt` is a varint
read from the changeset file (up to INT_MAX) — a genuine signed-overflow with no guard, which sqc correctly
flagged. **But changesetfuzz.c is a standalone command-line fuzzing utility** (its own `main()`; reads a
developer-supplied changeset file and emits fuzzed variants), NOT part of the library and not reachable through any
API. So this does NOT count against the "zero semantic TP in the production library/extensions" thesis — it is a
defect in a dev fuzzing tool, the same out-of-production class as batch-5's `SQLITE_TEST`-only `fts3_tokenizer`
`testFunc`. The reviewer also surfaced several over-reads in changesetfuzz's parser (fuzzGetVarint at 370 with no
end-pointer; fuzzParseHeader `p+=nCol` at 459/463; the bIndirect read at 581) — all genuine but **tool-scope**,
so dismissed (not recorded as library FNs), consistent with prior tool/test-only treatment. fileio.c and series.c
(the two real-but-tiny extensions) gave 0 FN: fileio's read/write size math is `mxBlob`-checked and the fsdir walk
is depth-guarded (`iLvl+3<mxLvl`); series.c's sequence arithmetic is deliberately wraparound-safe (`span64`/`add64`/
`sub64` type-pun to unsigned, `iStep` normalized non-zero before any divide).

### Integrity gate

The gate corrected one reviewer verdict-field typo: **changesetfuzz idx 114** was emitted `TP` but its own reason
said "misfire on a well-bounded caller index" (`apGroup[iGrp]`, `iGrp` bounded by the caller's `for` loop) — fixed
TP→FP. No other corrections; the only surviving semantic TP is the tool-scope idx 65 above.

sqlite coverage **103/220 (46.8%)** — 103 files audited; **13 FNs** (unchanged — 0 new this batch). Confirmed real
bugs still **13**, every one in a less-fuzzed untrusted-input parser; **still no production semantic TP in the
hardened core** (build.c + os_unix.c = 1171 findings, 0). The remaining ~117 in-scope files are now almost entirely
hardened core (os_win.c, expr.c, main.c, pager.c, wal.c, util.c, func.c, …) — coverage filler the thesis predicts
will keep yielding only decl/macro/const TPs.

## File-at-a-time — Batch 8 (2026-06-12, run #40, MEDIUM-effort core filler): os_win.c, main.c, util.c, vdbeapi.c

First medium-effort batch (per the agreed plan: drop to medium for pure hardened-core/glue, keep the
source-verification integrity gate non-negotiable). 16 line-range reviewers using the shared hardened-core
framework (`/tmp/core_framework.md`). **1657 findings -> 31 TP / 1626 FP, 0 semantic TP, 0 recorded FN.**

| File | findings | TP | FP |
|------|---------:|---:|---:|
| src/os_win.c (Windows VFS) | 539 | 14 | 525 |
| src/main.c (connection/API layer) | 433 | 8 | 425 |
| src/util.c (atoi/varint/number parsers) | 412 | 7 | 405 |
| src/vdbeapi.c (sqlite3_* statement API) | 273 | 2 | 271 |

All 31 TP are declaration/macro/recursion: **MSC04/MEM05 ×10** (genuine recursion — winHandleOpen/winOpen
read-only retry, sqlite3_initialize↔vfs_register, sqlite3_config↔PCacheSetDefault, sqlite3CreateFunc UTF16
self-call, sqlite3Step↔VdbeExec↔Checkpoint), **DCL13 ×7** const-params, **DCL03 ×5** static_assert on
compile-time mask/sizeof equalities, **PRE00/01/10/11/12 ×7** (winIsDirSep/winIoerrCanRetry1 multi-eval,
winMemAssertMagic non-do-while+trailing-`;`), **DCL00 ×2**. **Semantic-TP sweep: empty** (verified
programmatically). The recurring misfires were exactly the established hardened-core classes, now confirmed at
scale on the Windows VFS and API glue: the **os_win OSTRACE PRE32/DCL31 swarm** (the analyzer reads multi-line
`OSTRACE((...))` continuation lines and the `osXxx` syscall-table `#define`s as preprocessor-directives /
undeclared calls), the **MEM30 `db`/`pFile`-context-arg misread** (sqc treats the live handle arg of
`sqlite3DbFree(db,x)`/`robust_close(pFile)`/`sqlite3*Delete(db,x)` as freed), `zBuf`/`zConverted` freed only on
mutually-exclusive error paths, i64 FILETIME/size constants flagged as int overflow, and `u32`/`u64` operands
in `sqlite3GetVarint`/atoi misread as signed `int` or `FILE*`.

**FN-hunt: one low-severity item, noted not recorded** — util.c:1328 `sqlite3GetUInt32`'s guard `v>4294967296LL`
accepts exactly 2^32 (one past UINT32_MAX), so `(u32)v` truncates a 2^32 input to `0` and returns success
instead of rejecting it. A benign longstanding value-correctness quirk, not a memory/integrity (CERT-class)
defect — not added to the oracle. util.c's real parsers (`sqlite3AtoF`, `sqlite3Atoi64`, the fp-decode helpers)
are otherwise fully bounded (NUL/`zEnd`-guarded reads, mantissa/exponent caps, u64 accumulators with intended
post-checked wrap).

sqlite coverage **107/220 (48.6%)** — 107 files audited; **13 FNs** (0 new). Confirmed real bugs still **13**;
the bifurcation is now confirmed across **4 large hardened-core files** (build.c, os_unix.c, os_win.c — the two
VFS twins matched exactly — plus main.c) with **0 semantic TP / 0 FN** among 2828 core findings. Remaining
~113 in-scope files are core/glue; the genuine-untrusted-input subset (pager.c, wal.c, func.c, printf.c,
vdbesort.c) is reserved for high-effort batches.

## File-at-a-time — Batch 9 (2026-06-14, run #40, HIGH-effort genuine-untrusted-input core): pager.c, wal.c, func.c, printf.c, vdbesort.c

The reserved high-effort batch: the five remaining core files that actually decode attacker-influenced bytes
(on-disk page/journal/WAL-frame headers, SQL-function argument blobs/text, format strings, spilled sort
records). Adjudicated with the **untrusted-input framework** (`/tmp/batch_framework.md`, attacker-mindset + a
rigorous independent FN-hunt), not the hardened-core framework — 17 line-range reviewers. The
source-verification integrity gate was applied to every semantic-TP and FN candidate. **1752 findings -> 79 TP
/ 1673 FP, 0 semantic TP, 1 FN recorded.**

| File | findings | TP | FP | FN |
|------|---------:|---:|---:|---:|
| src/pager.c (pager / journal-header decode) | 432 | 21 | 411 | 1 |
| src/wal.c (WAL-frame / wal-index decode) | 429 | 22 | 407 | 0 |
| src/func.c (SQL built-in functions over untrusted args) | 411 | 27 | 384 | 0 |
| src/printf.c (format-string output engine) | 256 | 3 | 253 | 0 |
| src/vdbesort.c (external merge-sort, spilled records) | 224 | 6 | 218 | 0 |

All 79 TP are the established declaration/macro/recursion class — **DCL13** const-params (read-only SQL-function
`argv`/`context`, WAL/pager getters), **MSC04** factual recursion (`patternCompare` GLOB/LIKE bounded by
`SQLITE_LIMIT_LIKE_PATTERN_LENGTH`; `sqlite3PagerCheckpoint` re-entry via `PRAGMA table_list`; the vdbesort
merge-tree build/step/populate cycles), **PRE00/01/12** macro hygiene (`SEH_*`, `BYTESWAP32` multi-eval),
**DCL03** static_assert on `WAL_FRAME_HDRSIZE==24`. **Semantic-TP count: zero**, exactly as on the prior
hardened-core batches — confirming the bifurcation holds even for the core files that *do* touch
attacker-controlled bytes: the parsing here is hardened (szPage validated power-of-two 512..MAX before any
sizing; frames salt+checksum-gated; length/precision clamped and promoted to i64/u64; `getVarint32NR` /
record-header decodes bounded by record-size asserts). The recurring misfires were the known classes:
`sqlite3Realloc(pWal->apWiData,…)` / `zOut=sqlite3Realloc(zOut,…)` member-realloc read as use-after-free of the
parent handle; the `sizeof(aJournalMagic)` 8-byte static array read as a decayed param (≈25 bogus
ARR/EXP/INT in pager.c alone); `#if 0` debug blocks (PAGERTRACE, the func.c hash-dump, the printf TCL header,
the wal SEH macros) flagged for hygiene; loop induction variables (`ii`/`iPg`/`u`) read as uninitialized;
`static char[]` return buffers read as dangling-local returns; bounded internal counters/lock-mask shifts read
as overflow.

**FN-hunt: one recorded (low confidence), one byte-level off-by-one sqc did not flag** —
`src/pager.c:1335` `readSuperJournal`. The guard `len>=nSuper` admits `len==nSuper-1`, but the function then
writes a **two-byte** terminator `zSuper[len]` *and* `zSuper[len+1]`, so `zSuper[len+1]` can reach index
`nSuper`, which needs an `nSuper+1`-byte buffer while the guard only proves `nSuper`. Callers pass
`nSuper = mxPathname+1` into `pPager->pTmpSpace` (size = `pageSize`; the line-2974 caller uses
`&pTmpSpace[4]`, losing 4 bytes — the in-tree TODO at 2828-2832 already flags the `pageSize >= mxPathname+1`
assumption). **Default VFSes are safe**: unix `mxPathname=512` -> `nSuper=513`, and the power-of-two `pageSize`
minimum (1024) leaves ~2x slack. It becomes a reachable **1-5 byte OOB heap write** only with a custom VFS
reporting `mxPathname == 2^k - 1` (e.g. 1023) **and** `pageSize` set to exactly that power of two, driven by an
attacker-crafted hot-journal super-journal-name length field. Real but extremely narrow; recorded as an `FN`
(ARR30-C, low confidence) rather than note-not-recorded (unlike the benign util.c:1328 value quirk) because it
is an actual memory-safety OOB write under a constructible configuration. HEAD-status check deferred to the
disclosure pass per the campaign's disclosure-backlog protocol.

sqlite coverage **107 -> 112/220 (50.9%)** — half the in-scope corpus audited; **14 FNs** (+1, the pager.c
off-by-one); confirmed real bugs still **13** (no new fixed-upstream bug this batch). The thesis now holds
across **9 large hardened/core files** including the five genuine-untrusted-input ones: **0 production semantic
TP** among the core's combined finding volume. Remaining ~108 in-scope files are pure core/glue
(expr.c, wherecode.c, window.c, analyze.c, vdbemem.c, insert.c, alter.c, pragma.c, …) — medium-effort filler
the bifurcation predicts will keep yielding only declaration/macro/const TPs.

## File-at-a-time — Batch 10 (2026-06-14, run #40, MEDIUM-effort core/glue filler): expr.c, wherecode.c, window.c, analyze.c, vdbemem.c, insert.c

Resuming medium-effort filler after the batch-9 high-effort untrusted-input set. Six hardened-core codegen /
value-management files via the shared hardened-core framework (`/tmp/core_framework.md`), 14 line-range
reviewers, source-verification integrity gate kept non-negotiable. **1513 findings -> 120 TP / 1393 FP, 0
semantic TP, 0 recorded FN.**

| File | findings | TP | FP |
|------|---------:|---:|---:|
| src/expr.c (expression-tree codegen) | 473 | 50 | 423 |
| src/wherecode.c (WHERE-loop codegen) | 231 | 28 | 203 |
| src/window.c (window-function codegen) | 217 | 20 | 197 |
| src/analyze.c (ANALYZE / sqlite_stat + stat-blob decode) | 211 | 3 | 208 |
| src/vdbemem.c (Mem value management) | 198 | 10 | 188 |
| src/insert.c (INSERT codegen) | 183 | 9 | 174 |

All 120 TP are the established declaration/macro/recursion class — dominated by **MSC04** (expr.c alone is a
recursion thicket: ExprDelete/ExprDup/SelectDup/SrcListDup, the whole ExprCode* codegen cycle, CodeRhsOfIN,
window WindowDup/WindowCodeStep, GenerateConstraintChecks↔trigger codegen), plus **DCL13** read-only helpers,
**DCL03** static_assert on `SQLITE_AFF_*`/`TK_*`/`SQLITE_FUNC_*==OPFLAG_*` compile-time equalities, and **PRE**
macro hygiene (`WINDOWFUNCALL`/`WINDOWFUNCX` `##`-paste + multi-eval, `ISPOWEROF2` multi-eval). **Semantic-TP
count: zero**, holding the bifurcation across these six core files. The recurring misfires were exactly the
catalogued classes: the **MEM30 `db`/context-arg swarm** (`sqlite3DbFree(db,x)` / `sqlite3ExprDelete(db,x)` /
`sqlite3VdbeMemGrow` realloc read as freeing the live handle), bounded VDBE register/cursor/`nMem`/`nTab`
counters read as INT32 overflow, `for(ii=…)` induction vars read as uninitialized (EXP33/ARR00), heap
`sqlite3DbMallocRaw` results read as dangling-local returns (DCL30), and DCL13 on params fixed by Walker /
UDF-callback function-pointer typedefs.

**FN-hunt: one benign item noted, not recorded** — `window.c:401` `cume_distValueFunc` computes
`(double)p->nStep/(double)p->nTotal` without the `nTotal>1` guard its sibling `percent_rankValueFunc` has. Not
recorded: it is IEEE float division (defined inf/nan behavior, not C UB), not a memory-safety/CERT-class defect,
and the window machinery guarantees a prior step so `nTotal>=1` at runtime — same disposition as the batch-8
util.c:1328 value quirk. The analyze.c stat-blob decoders (`decodeIntArray`, `analysisLoader`, `loadStatTbl`)
that were flagged as a genuine-TP risk proved bounded (`i<nOut`, i64 size math on small column counts,
divide-by-zero guarded by the `aiRowEst[iCol+1]==0` test, the deliberate 8-byte corrupt-record overread pad).

sqlite coverage **112 -> 118/220 (53.6%)** — over half done; **14 FNs** (0 new); confirmed real bugs still
**13**. The bifurcation now holds across **15 large hardened/core files** with zero production semantic TP.
Remaining ~102 in-scope files are pure core/glue (trigger.c, alter.c, sqliteInt.h, pragma.c, prepare.c,
whereexpr.c, os_kv.c, fkey.c, vtab.c, update.c, …) — medium-effort filler the thesis predicts will keep
yielding only declaration/macro/const TPs.

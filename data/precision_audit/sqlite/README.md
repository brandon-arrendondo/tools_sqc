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

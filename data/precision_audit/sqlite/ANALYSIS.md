# sqlite audit — engineering & paper takeaways

Derived from the completed 220/220 file-at-a-time audit (run #40 @ `b1a73ba34d`):
24,834 findings adjudicated → 1,496 TP / 22,154 FP / 23 FN / 48 uncertain (≈6.3% precision).
Two payoffs beyond upstream disclosure: (1) **FP-reduction priorities** for the tool, and
(2) **paper-citable TPs** — real bugs sqc caught, including one the SQLite team independently fixed.

---

## 1. FP-reduction priority (where the precision is lost)

**Six rules produce ~47% of all FPs at ≈0% precision.** Fixing/gating these is the single
biggest precision lever:

| Rule | FP | TP | prec | Dominant misfire pattern (what to suppress) |
|------|---:|---:|-----:|---------------------------------------------|
| INT32-C | 4420 | 15 | 0.3% | "signed overflow" on **bounded** operands: i16 column counts (asserted ≤32767), SQLITE_LIMIT-bounded counters (nMem/nTab/register/cursor), i64 size math, page sizes; and **unsigned** values mislabeled as signed (e.g. `u32` page math). |
| MEM30-C | 2300 | 1 | 0.0% | the **freed-context-arg** misfire: `sqlite3DbFree(db,x)` / `*Delete(db,x)` / `robust_close(pFile)` read as freeing the *handle* arg; live handle never freed. Also `%z` mprintf-accumulator frees misread as double-free. |
| EXP34-C | 1496 | 7 | 0.5% | null-deref already guarded by `assert` / `rc==SQLITE_OK` / explicit `if`; OOM paths that set rc before deref. |
| API00-C | 1366 | 1 | 0.1% | internal contract guarantees non-null (mutex held / caller invariant); **not a trust boundary**. |
| INT30-C | 915 | 1 | 0.1% | intended unsigned modular arithmetic; bounded accumulators. |
| EXP33-C | 672 | 0 | 0.0% | "uninitialized read" where a macro/first-statement (READ_UTF8, memset, U8_NEXT) writes the var first. |

**37 rules fired ≥20 times with ZERO true positives in the entire 24,834-finding corpus.**
These are pure noise on hardened real-world C and are the strongest suppression/gating candidates:

> EXP33, STR34, ARR37, INT33, ARR36, PRE32, DCL31, EXP30, EXP43, CON03, ARR01, API02,
> FLP34, MEM31, EXP20, EXP07, MSC37, FLP03, EXP40, MEM33, INT34, DCL40, ARR39, MEM12,
> FIO42, FLP02, WIN04, INT00, STR30, EXP00, INT08, FIO50, FLP30, EXP37, POS49, FIO47, ENV01

**The ARR30 paradox (rule-redesign signal, not just suppression).** ARR30-C scored 628 FP / 0 TP —
yet **13 of the 23 FNs are ARR30-class** (real OOB reads sqc *missed*). So the rule as implemented
fires on the wrong thing (bounded indices it misreads) and fails on the right thing (unbounded
decode-loop reads past a buffer). ARR30 needs to be re-pointed at the FN pattern in §2, not just quieted.

**Keep / trust (high precision — the rules carrying sqc's value):**
MSC04 87% · PRE12 75% · PRE00 73% · PRE10 70% · DCL37 64% · PRE01 43% · ERR34 43% · ERR07 42% ·
PRE11 45% · DCL03 33% · DCL13 31%. These are the declaration/macro/recursion classes — ~90% of all TPs.

---

## 2. FN detection gaps (bugs sqc should find — 23 total)

Grouped by the detection capability that would catch them. The recurring shape is **"a length/
index/pointer derived from untrusted or corrupt input drives a read/write with no bound check
before the access."**

### 2a. Unbounded decode-loop / varint read past buffer end (ARR30-class) — the biggest gap
The dominant FN family. A `getVarint`/`while` loop reads up to N bytes chasing continuation bits
or a terminator that the buffer may not contain:
- `src/os_kv.c:469` — kvvfsDecode hex branch, no `j<nOut` (**disclosed; heap-overflow WRITE**)
- `ext/fts3/fts3_snippet.c:890` — matchinfo `aMatchinfo[]` mis-sized when `nCol%32==0` (**high conf, OOB write**)
- `ext/misc/spellfix.c:2597` — `zPattern[nPattern-1]` reads `[-1]` on empty MATCH (**med**)
- `ext/rbu/sqlite3rbu.c:2654` — memcpy over-reads untrusted shorter `rbu_control` TEXT (**med**)
- `ext/fts5/fts5_storage.c:1405`, `ext/fts3/fts3_term.c:232`, `ext/fts3/tool/fts3view.c:568`,
  `ext/repair/checkindex.c:389`, `ext/intck/sqlite3intck.c:322`, `ext/misc/scrub.c:442`,
  `ext/misc/nextchar.c:184`, `ext/misc/zipfile.c:714`, `src/dbstat.c:449`, `src/pager.c:1335`
**Capability needed:** taint a length/count from `sqlite3_column_*`/blob/file/page bytes and require a
bound check against the buffer extent *before* the indexing read; treat varint/`getVarint` and
`while(*p++ != term)` loops as unbounded unless a `p<end` guard dominates.

### 2b. OOM null-deref after malloc (EXP34-class) — tractable, high-value pattern
- `ext/misc/compress.c:63` and `:102`, `ext/misc/vfstrace.c:897`
**Capability needed:** flag `p = sqlite3_malloc*(...)` followed by a write/deref of `p` with no
intervening `if(p==0)`/`p!=0` check. (sqc currently mis-fires EXP33 at some of these instead.)

### 2c. Integer truncation before allocation (INT31-class)
- `ext/misc/amatch.c:1160` — `nBuf=(char)(nWord+100)` truncates through signed char before realloc
- `ext/misc/sqlar.c:94` — `i64 sz` (attacker blob size) → `sqlite3_malloc(int)` truncates, then uncompress overflows
**Capability needed:** detect narrowing of a 64-bit/attacker value to a narrower type on the path into an allocation size.

### 2d. Other one-offs
- `src/where.c:4454` INT34 `1<<iTerm` with `iTerm>=32` · `ext/misc/normalize.c:628` MEM31 realloc-leak-on-OOM ·
  `ext/misc/percentile.c:291` INT30 32-bit doubling wrap (~16GB, unreachable) · `src/pcache.h:16` PRE06 broken include guard.

**Pattern summary for the roadmap:** the FN-hunt confirms sqc's gap is concentrated in
**untrusted/corrupt-input decode paths** — exactly where its semantic rules currently emit only FP.
A taint-aware "untrusted length → unguarded buffer access" analysis would convert the ARR30 noise
budget into the bugs in 2a, and is the highest-leverage detection investment.

---

## 3. Paper-citable TPs (real defects sqc caught)

sqc flagged genuine, reachable defects in real shipped SQLite code. The strongest external-validity
point: **`prefix_length` — sqc flagged it as a TP, and the SQLite team independently fixed it in trunk.**

| Defect | sqc rule | Status / external validation |
|--------|----------|------------------------------|
| `ext/misc/prefixes.c:295` `prefix_length(NULL,…)` NULL deref | EXP34 | **sqc-caught TP; FIXED upstream at HEAD** (trunk added `if(zL==0\|\|zR==0)` + NUL break) — independent confirmation the TP was a real bug. |
| `ext/expert/sqlite3expert.c:305` `idxNewConstraint` heap overflow (`sizeof()*nColl+1` vs `+nColl+1`) | ARR38 | sqc-caught TP; **disclosed upstream** (LIVE at HEAD). Integrity gate recovered it from an initial reviewer FP. |
| `ext/misc/closure.c:568` use-after-free (`*ppVtab=&pNew->base` after `closureFree(pNew)`) | MEM30 | sqc-caught TP; genuine UAF (benign by vtab contract but real). |
| `ext/repair/checkfreelist.c:277` `sqlite_readint32(blob,-100)` OOB read | EXP34 | sqc-caught TP; negative-offset defeats `nBlob>=iOff+4` guard. |
| `ext/misc/base85.c:257` `isspace(c)`/`isspace` on negative `char` | STR00 / STR37 | sqc-caught TP; genuine ctype-UB on negative char. |
| `ext/misc/normalize.c:616` `IdChar(zIn[-1])` reads `z[-1]` | ARR00 | sqc-caught TP; the `n&&` guard is dead. |

**Honest framing for the paper.** These coexist with ~6.3% overall precision and the 23 FNs of §2 —
the contribution is the **rule-class bifurcation**: sqc's value is carried by declaration/macro/
recursion rules (DCL/PRE/MSC, ~90% of TPs, 31–87% precision), while its semantic memory-safety rules
(MEM30/ARR30-38/EXP33-34/INT30-34/STR3x) are ≈0% precise on hardened code yet are where the real
bugs (and sqc's own FNs) live. The disclosed `os_kv.c` kvvfs overflow is a *false negative* sqc missed
(found by the manual FN-hunt), not a TP — cite it as a methodology/gap result, not a detection win.

> Data source: `ground_truth` table in `data/benchmarks.db` (project=sqlite, commit b1a73ba34d).
> Regenerate the per-rule numbers with the queries used to build this file.

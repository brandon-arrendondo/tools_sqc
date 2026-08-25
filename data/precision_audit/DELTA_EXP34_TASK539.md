# EXP34-C delta-adjudication (task 539) — COMPLETE

Part of task 532's breakdown (17,952 unlabeled findings across 205 rules,
run 187 v0.4.258 vs the v0.4.120 baseline). This tracks the delta pass for
**EXP34-C** ("do not dereference null pointers") — 265 raw unlabeled
findings across sqlite/mosquitto/curl/hostap/lua/libcrc.

## Scope

Derived each project's in-scope file predicate from its own
`data/precision_audit/<project>/README.md` **before** pulling findings:

| Project   | Raw unlabeled | Dropped (out-of-scope)                                      | In-scope | After same-line consolidation | Batches |
|-----------|---------------|----------------------------------------------------------------|----------|-------------------------------|---------|
| sqlite    | 173           | 32 (`mptest/`, `src/tclsqlite.c`, `ext/fts3/fts3_test.c`)       | 141      | 140 (1 pair shares a line)     | 2       |
| mosquitto | 53            | 0                                                                | 53       | 53                             | 1       |
| curl      | 27            | 15 (`lib/vtls/schannel*.c`, Windows-only)                        | 12       | 11 (1 pair shares a line)      | 1 (combined) |
| hostap    | 8             | 0                                                                | 8        | 8                              | 1 (combined) |
| lua       | 3             | 0                                                                | 3        | 3                              | 1 (combined) |
| libcrc    | 1             | 0 (`precalc/precalc.c` is explicitly in-scope per the README)   | 1        | 1                              | 1 (combined) |
| **Total** | **265**       | **47**                                                           | **218**  | **216**                        | **4**   |

curl/hostap/lua/libcrc's small residual counts (24 total) were combined
into a single batch rather than 4 separate agent calls.

## Outcome

| Project   | Findings | TP | FP  | Precision |
|-----------|----------|----|-----|-----------|
| sqlite    | 140      | 11 | 129 | 7.9%  |
| mosquitto | 53       | 0  | 53  | 0.0%  |
| curl      | 11       | 0  | 11  | 0.0%  |
| hostap    | 8        | 0  | 8   | 0.0%  |
| lua       | 3        | 0  | 3   | 0.0%  |
| libcrc    | 1        | 0  | 1   | 0.0%  |
| **Total** | **216**  | **11** | **205** | **5.1%** |

Post-import measured precision for EXP34-C over the full labeled set
(`bench realworld-score 187`): **2.8%** (59 TP / 2,127 labeled), **86.8%
recall**. 279 findings remain unlabeled (largely pureftpd/sel4).

## 11 real bugs found — all in sqlite's `ext/` extension code

Unlike most rules delta-adjudicated so far, this batch found a real,
concentrated cluster of TPs: **11 of sqlite's 140 findings are genuine
OOM/NULL-dereference bugs, all in extension code (`ext/`), none in
`src/` core**. The pattern: `sqlite3_malloc64`/`sqlite3_mprintf`/
`sqlite3_vmprintf` results fed directly into plain libc calls (`fread`,
`memset`, `strlen`) or SQLite's own `%s`-printf with no OOM check —
unlike core `src/` code, which consistently follows the `mallocFailed`
convention, several `ext/` extensions skip it. One TP
(`ext/misc/sha1.c:323`) is not an OOM case but a genuine logic gap: the
near-identical `shathree.c` correctly checks `if(z)` before using
`sqlite3_sql()`'s result, while `sha1.c` doesn't — a real, fixable,
directly-comparable omission.

## FP causes: three recurring, high-confidence checker gaps

1. **NULL-safe SQLite C-API misattribution** (dominant sqlite FP cause) —
   `sqlite3_stmt *pStmt` (already guaranteed non-NULL by a checked
   `sqlite3_prepare_v2`) passed into `sqlite3_column_*`/`sqlite3_bind_*`/
   `sqlite3_step`/`sqlite3_sql`/`sqlite3_stmt_readonly`, all of which are
   documented and implementation-verified (`vdbeapi.c`) to be NULL-safe by
   design. Likewise `sqlite3_mprintf`'s `%s` substitutes `""` for NULL
   rather than dereferencing (confirmed in `printf.c`), and
   `sqlite3DbFree`/`returnSingleText` internally guard.
2. **Type confusion: plain `int` flagged as "null pointer"** — ~14 findings
   in `pragma.c` flag `int` loop counters (`i`, `j`) passed to
   `sqlite3VdbeMultiLoad`/`sqlite3CodeVerifySchema`/etc. as possibly-NULL
   *pointers*, when they are not pointers at all. This is the **same class
   of type-check gap independently found in ARR37-C (task 538)** — two
   different CERT rules both misidentifying non-pointer variables, strong
   evidence of a shared root cause worth investigating together.
3. **Codebase-wide non-NULL invariant not recognized** (dominant mosquitto
   FP cause, 33 of 53 findings) — `struct mosquitto *context`/`mosq` is a
   hard non-NULL invariant throughout mosquitto's connection lifecycle
   (event-loop mux backends, plugin dispatch, session bookkeeping, database
   write paths); sqc flags every dereference of that parameter without
   recognizing the codebase-wide convention. Secondary mosquitto patterns:
   `DL_FOREACH_SAFE` on a possibly-NULL list head (the utlist macro safely
   no-ops on NULL by design) and `strtol`'s `endptr` output param (C
   standard guarantees it's always set).
4. **Short-circuit blindness** — dereferences inside or immediately after
   the same `&&`/`||` expression that performs the NULL check
   (`pTab==0 || ...->pFKey`, `zTab && sqlite3StrICmp(...)`).

## Follow-up

Filed as new rule-fix tasks:
- **task 558**: EXP34-C misfires on plain, non-pointer `int` variables —
  likely the same root cause as task 556 (ARR37-C's identical bug class),
  worth investigating together since both rules may share a common
  type-resolution helper.
- **task 559**: EXP34-C should special-case SQLite's documented NULL-safe
  C-API surface (`sqlite3_column_*`/`sqlite3_bind_*`/`sqlite3_step`/
  `sqlite3_sql`/`sqlite3_mprintf`'s `%s`) as non-dereferencing on NULL —
  this alone accounts for the majority of sqlite's 129 FPs here.
- Noted but not filed as a task: mosquitto's `context`/`mosq`-parameter
  non-NULL convention (33 FPs) is a project-specific invariant, not a
  general rule bug — better addressed via a documented suppression/config
  note if it recurs in future mosquitto delta passes, per this repo's
  surface-don't-silence philosophy (measured FP, not silently disabled).

CSVs: `data/precision_audit/{sqlite,mosquitto,curl,hostap,lua,libcrc}/import_delta_exp34_task539.csv`.

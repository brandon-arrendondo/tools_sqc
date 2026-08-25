# ARR37-C delta-adjudication (task 538) — COMPLETE

Part of task 532's breakdown (17,952 unlabeled findings across 205 rules,
run 187 v0.4.258 vs the v0.4.120 baseline). This tracks the delta pass for
**ARR37-C** ("do not add or subtract an integer to a pointer to a
non-array object") — 311 raw unlabeled findings across sqlite/curl/lua/
mosquitto (pureftpd/sel4 tracked separately).

## Scope

Derived each project's in-scope file predicate from its own
`data/precision_audit/<project>/README.md` **before** pulling findings:

| Project   | Raw unlabeled | Dropped (out-of-scope)                                         | In-scope | After same-line/multi-column consolidation | Batches |
|-----------|---------------|-------------------------------------------------------------------|----------|-----------------------------------------------|---------|
| sqlite    | 256           | 41 (`ext/session/changesetfuzz.c` fuzz harness + `test_*.c`/`_test.c`) | 215      | 189 (26 extra rows share a line, different columns) | 2 |
| curl      | 49            | 0                                                                    | 49       | 42 (7 extra rows share a line)                | 1 |
| lua       | 5             | 0                                                                    | 5        | 5                                               | 0 (adjudicated directly) |
| mosquitto | 1             | 1 (`fuzzing/broker/fuzz_packet_read_base.c` — same fuzz-harness exclusion as task 536) | 0 | — | — |
| **Total** | **311**       | **42**                                                              | **269**  | **236**                                         | **3** |

lua's 5 findings were adjudicated directly (small enough not to warrant a
subagent batch). mosquitto's single finding is entirely out of scope —
same class as task 536's mosquitto exclusion (fuzz harness, not shipped
product).

## Outcome

| Project | Findings | TP | FP  | Precision |
|---------|----------|----|-----|-----------|
| sqlite  | 189      | 0  | 189 | 0.0% |
| curl    | 42       | 0  | 42  | 0.0% |
| lua     | 5        | 0  | 5   | 0.0% |
| **Total** | **236** | **0** | **236** | **0.0%** |

Post-import measured precision for ARR37-C over the full labeled set
(`bench realworld-score 187`): **0.1%** (1 TP / 904 labeled), **100%
recall** on the labeled TP set. 79 findings remain unlabeled (largely
pureftpd/sel4).

## Root cause is uniform: lost array-origin tracing, plus a distinct checker bug

Every single FP in this batch traces back to a pointer that genuinely
*does* point into an array/buffer, but ARR37-C's static tracing lost that
origin one hop back through one of:

1. **Address-of-subscript expressions** (`p = &arr[i]`, `&pWC->a[...]`,
   `&hostname[len+1]`) — the array origin exists but isn't followed
   through the `&x[i]` form.
2. **Output-parameter / pointer-to-pointer indirection** — a `char **`/
   `u8 **`/`int **` parameter aliasing a caller-owned array (`p = *pz`,
   `aRec = *ppRec`), common in sqlite's fts3 poslist/doclist-merge code and
   curl's `const char **` string-parsing helpers.
3. **Offset into a single over-sized allocation** holding multiple logical
   sub-arrays (`sqlite3_malloc64(sizeof(struct)+extra)` then arithmetic
   past the struct to reach a trailing buffer) — seen repeatedly in
   `fts3.c`/`fts3_write.c`/`sqlite3session.c`.
4. **Local stack arrays reached through a pointer-to-pointer local**
   (`fts3_porter.c`'s `zReverse[28]` via `stem()`'s `char **pz`).
5. **Standard NUL-terminated string traversal** over an array-backed
   buffer (`name++` walking a path string in Lua's `loadlib.c`).

**A second, structurally distinct checker bug** was found independently
in two projects: **the checker flags plain, non-pointer `int` variables**
as "non-array pointer arithmetic." Confirmed at:
- `lua/lobject.c:93,96` — `int e = (p >> 4); e--; ... e -= 7;` — `e` is an
  int, never a pointer.
- `sqlite/src/build.c:2097,2099,2100,2102` — `i++` on a plain `int` loop
  index, not a pointer.

This is not an array-origin-tracing miss (cause #1-5 above) — it's ARR37-C
misidentifying a non-pointer variable as pointer arithmetic in the first
place, independently corroborated in two unrelated codebases.

## Follow-up

Filed as new rule-fix tasks:
- **task 556**: ARR37-C misfires on plain integer variables (not pointers
  at all), confirmed in both lua (`lobject.c:93,96`) and sqlite
  (`build.c:2097-2102`) — a type-check gap distinct from the array-origin
  cause below.
- **task 557**: ARR37-C's array-origin tracing should follow `&arr[i]`
  address-of-subscript expressions and pointer-to-pointer/output-parameter
  indirection back to their array source — this alone would kill the
  overwhelming majority of the 236 FPs measured here (all of them traced
  to one of causes #1-5 above, none a genuine non-array-object violation).

CSVs: `data/precision_audit/{sqlite,curl,lua}/import_delta_arr37_task538.csv`.

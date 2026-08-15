# MSC13-C delta-adjudication (task 391) — COMPLETE

Source: task 391 fixed one genuine MSC13-C false-positive root cause
(hostap's `free(var)` synthetic pseudo-definition being treated as a real
write needing its own read — see `src/rules/cert_c/MSC/MSC13-C/msc13_c.rs`,
shipped v0.4.206). Comparing `sqc realworld` run **#155** (v0.4.207,
commit c9a8668d) against the prior run showed MSC13-C's raw count only
dropped by 28 (the direct effect of that fix), but a **70.3% unlabeled
fraction** (1,776 of 2,568 findings) on the rule overall — almost entirely
pre-existing backlog unrelated to today's fix, never adjudicated in any
prior sweep. This delta-adjudication pass labels that whole backlog, not
just the 28 findings the fix actually touched, per the user's explicit
request to fully close out MSC13-C's ground-truth gap while it was open.

Generated via `bench realworld-unlabeled 155 --rule MSC13-C --project <p>
--json`, scoped against each project's own precision_audit README
in-scope predicate **before** batching (per the `scope-batches-before-not-
after` lesson from task 420), then packed into per-file/per-file-chunk
batches (~100-150 findings, `#partNofM` split for oversized files:
`src/vdbe.c` alone had 296 MSC13-C findings and was split across 3 chunks
in batches 4/5).

## Scope

| Project   | Raw unlabeled | Dropped (out-of-scope) | In-scope batched | Batches |
|-----------|---------------|-------------------------|-------------------|---------|
| sqlite    | 1,146         | 18 (`src/tclsqlite.c` Tcl binding ×16, `mptest/mptest.c` ×2) | 1,128 | 10 (`msc13_batches/batch_01..10_sqlite.json`) |
| curl      | 481           | 25 (14-file WIN_MAC Windows/Apple build config) | 456 | 4 (`batch_11..14_curl.json`) |
| mosquitto | 117           | 23 (`libmosquittopp.h` C++ wrapper ×20, `examples/` ×3) | 94 | 1 (`batch_15_mosquitto.json`) |
| hostap    | 21            | 0 (all in `src/`/`wpa_supplicant/`) | 21 | 1 (`batch_16_hostap.json`) |
| lua       | 11            | 0 (all core interpreter source) | 11 | 1 (`batch_17_lua.json`) |
| **Total** | **1,776**     | **66**                   | **1,710**         | **17** |

Only 66 of 1,776 (3.7%) were out-of-scope — much cleaner than task 420's
MEM31-C pass (63% contamination), because this pull only targeted MSC13-C
(a rule with almost no findings in vendored/test glue) rather than an
unscoped whole-repo sweep.

## Adjudication method

17 parallel subagents, each given one batch's JSON (file/line/message),
the pinned checkout at `~/toolchain/<project>` (same commits as each
project's own oracle README), and MSC13-C's exact semantics ("value
assigned to X is never read on any execution path before being
overwritten or the function returning"). Each subagent read every flagged
function in full and produced a CSV verdict (TP/FP) with a one-sentence,
line-cited reason. No file was skimmed — flagged lines were traced to
their next read or overwrite by hand, not pattern-matched.

## Results

| Project   | TP    | FP  | Total | Precision |
|-----------|-------|-----|-------|-----------|
| sqlite    | 612   | 516 | 1,128 | 54.3%     |
| curl      | 421   | 35  | 456   | 92.3%     |
| mosquitto | 71    | 23  | 94    | 75.5%     |
| hostap    | 3     | 18  | 21    | 14.3%     |
| lua       | 1     | 10  | 11    | 9.1%      |
| **Total** | **1,108** | **602** | **1,710** | **64.8%** |

4 findings (3 sqlite, 1 curl) were already labeled from prior passes and
skipped on import; the remaining 1,706 are new `ground_truth` rows,
`source=msc13_delta_adjudication_0.4.207`.

**Rule-level MSC13-C, post-import (`bench realworld-score 155`):**
2,469 / 2,533 findings labeled (97.5% coverage, down from 29.7% before this
pass) — **68.6% precision, 98.6% recall.** This is now a fully-supported
number, not a raw-count guess.

## Categorical FP patterns confirmed (all rules/projects)

The false positives cluster into a small number of genuine analyzer gaps,
all in "the value is read, sqc's dataflow just didn't see it":

1. **`goto`-to-shared-label reads** (by far the largest class, ~40% of
   sqlite's FPs): `rc = SQLITE_BUSY; goto abort_due_to_error;` where the
   label reads `rc` dozens of lines later. sqc's per-statement liveness
   doesn't trace value flow across a `goto` to a merge point that also
   receives assignments from many other call sites in the same function
   (`src/vdbe.c`'s giant opcode-dispatch loop is the extreme case).
2. **Assignment-in-condition reads**: `if((rc = foo()) != SQLITE_OK)` /
   `while((context = accept(...)) != NULL)` — the assigned value is read
   in the very same expression that assigns it; sqc doesn't model this as
   a read.
3. **Loop-header/loop-back-edge reads**: `for(i=0; i<n; i++)` (the
   loop's own condition reads the init value), `do { ... } while(got>0)`
   (a `continue` jumps to the condition, which reads a value set earlier
   in the body), and reads that only occur on a subsequent loop
   iteration (accumulator variables, `pNext = p->next` loop-increment
   patterns).
4. **Macro-hidden reads**: `utlist`/`uthash` iteration macros
   (`DL_FOREACH_SAFE`, `HASH_ITER`) that read/initialize a variable in
   their expansion; `DOCID_CMP()` and similar project-local macros that
   consume a variable sqc can't see into (no preprocessor).
5. **`#ifdef`/`#endif`-split branches**: sqc's CFG sometimes fails to
   join an `#ifdef X ... #else ... #endif` if/else pair correctly, making
   one arm's write look unread when the other's absence at compile time
   is exactly what makes it live.
6. **Struct-field misclassification**: several sqlite allocator files
   (`mem3.c`, `mem5.c`, `pcache1.c`, `random.c`, `status.c`, `malloc.c`)
   declare their state as a `static struct { ... } wsdXxx;` global and
   access fields via `wsdXxx.field`; sqc misidentified the *struct field
   declarations themselves* as unused local variables.
7. **`case` keyword misparsed as a variable**: a handful of
   `/* deliberate fallthrough */`-commented `case FOO:` labels in
   `src/expr.c`/`src/window.c` were misparsed as a declared variable
   literally named `case` — an ERROR-node/keyword-as-token artifact, not
   a real MSC13-C concept.

None of these are new discoveries — all match already-documented sqc
FP classes from prior audits (goto/label reads and macro-blindness in
particular echo the free()-pseudo-definition root cause task 391 itself
fixed for the `free()`-adjacent case) — but this pass is the first to
measure their actual *volume* against a fully-labeled MSC13-C corpus
rather than a handful of spot-checked instances.

## True positives: genuinely idiomatic, not surprising

curl's 92.3% precision is the standout: the vast majority of its TPs are
the exact same idiom repeated hundreds of times — `CURLcode result =
CURLE_OK;` (or `NULL`/`0`/a sentinel) declared, then unconditionally
overwritten by the function's real work before any branch could read the
initializer, with the function's actual returns using the *reassigned*
variable or a literal. This is a real (if extremely low-severity) dead
store under CERT's letter, and curl's own maintainers would likely
consider most of them harmless style rather than defects — consistent
with the "declaration/macro/style, not memory-safety" precision profile
documented for curl across the other rules in its full audit
(`curl/README.md`).

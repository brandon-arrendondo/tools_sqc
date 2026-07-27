# MSC12-C / MSC13-C precision audit — sqc v0.4.141 (run sqc-0.4.141-2acbd455, 2026-07-27)

MSC12-C and MSC13-C were silently dark in every real-world benchmark run and
in the CLI's built-in default manifest from v0.3.51 through v0.4.140 (see the
build.rs `rules_templates` resync fix, commit 05bfe450). This is their first
ever real-world exposure. Manual adjudication of a seeded random sample (seed
20260727, 50 findings per rule, drawn proportionally to each project's share
of the rule's hits) across all 7 real-world codebases.

Files:
- `sample_msc12.json` / `sample_msc13.json` — the sampled findings (via
  `bench realworld-unlabeled --rule <ID> --limit 50 --seed 20260727 latest --json`).
- `adjudication_msc12.csv` / `adjudication_msc13.csv` — per-finding verdict
  (TP/FP) with a one-line reason, produced by two independent subagents each
  reading the actual pinned source at every cited line.

Imported into the persistent ground-truth oracle (`ground_truth` table in
`data/benchmarks.db`), pinned to each project's v0.4.141 codebase commit:

    python -m bench realworld-import-labels \
        data/precision_audit/msc12_msc13_0.4.141/adjudication_msc12.csv \
        --run 0.4.141 --source msc12_msc13_precision_audit \
        --adjudicator agent --date 2026-07-27
    python -m bench realworld-import-labels \
        data/precision_audit/msc12_msc13_0.4.141/adjudication_msc13.csv \
        --run 0.4.141 --source msc12_msc13_precision_audit \
        --adjudicator agent --date 2026-07-27

## Results

| Rule | Run count | Sample | TP | Precision |
|------|-----------|--------|----|-----------|
| MSC12-C (no-effect/dead code) | 4,857 | 50 | 0 | **0.0%** |
| MSC13-C (unused values) | 20,383 | 50 | 4 | **8.0%** |
| **Combined** | 25,240 | 100 | 4 | **4.0%** |

Together these two rules account for **~81%** of the entire real-world
violation-count increase brought by this session's manifest-drift fix
(25,240 of 31,442 net new findings), and both are far below every other
rule's measured precision in the current oracle (next-lowest is API07-C at
4.1%, MEM10-C/CON43-C at 0.0% but with only ~100-600 run counts, not
20k+). This volume is effectively unusable noise as shipped.

Adjudication standard: TP if the diagnosed condition is genuinely true (the
statement really has no effect / the value is really never read on any path)
and a competent reviewer would act on it; FP if sqc's syntactic scan missed a
real effect/read.

## Dominant FP root causes

**MSC12-C (0/50) — no-effect/dead-code detector:**
1. **Grouped case labels** (`case 'D': case 'd': ...code...`) — the standard
   C multi-value-case fallthrough idiom — misflagged as an empty/no-effect
   case body. By far the majority of the sample.
2. **Misattribution**: bodies flagged "empty" that actually contain real
   code (`return -1;`, `mcs_count = 1; break;`, a `TRACELOG(...)`/
   `wpa_printf(...)` call) — especially when the body's first statement is a
   call expression, which the check appears to skip past incorrectly when
   deciding whether the body is "empty".
3. Deliberate no-op scan loops (`for (...) {}`, all work done in the
   condition/increment clauses) flagged as empty control-flow bodies.
4. Bare macro tokens used as statements (`vmbreak`, `S3JniIfThrew`,
   `S3JniAutoExt_mutex_assertLocker`) that have real hidden control-flow or
   side effects sqc can't see through (no C preprocessor).
5. Declaration-terminating semicolons after attribute-style macros
   (`CURL_PRINTF(2,3);`, `PRINTF_FORMAT(3,4);`) flagged as stray semicolons.

**MSC13-C (4/50) — unused-value/dead-store detector:**
The documented blind spots (macro-hidden reads, `#ifdef`-only reads,
`(void)x;`, `__attribute__((unused))`) accounted for only 1 of 46 FPs. The
real, undocumented gap is a broad set of common control-flow idioms where a
value assigned in one place is read in a *later syntactic position the
read-detector doesn't walk into*:
1. **Loop-condition/goto-cleanup reads**: a variable assigned in a loop's
   init clause or body, then read in the loop's own condition on the next
   iteration (`for (pIter = pFirst; pIter != pEnd; ...)`,
   `do { rc = osPwrite(...); } while (rc < 0 && ...)`,
   `while (retval == CURL_FORMADD_OK)`).
2. **goto-to-shared-cleanup-label reads**: `result = CURLE_OUT_OF_MEMORY;
   goto oom;` where the `oom:` label's block later does `return result;`.
3. **Trailing `return x;`** reading a value set earlier in a conditional
   branch (`int rc = SQLITE_OK; ... return rc;`).
4. **Assignment-in-condition**: `if ((res = mp_sub(&x, &y, &x)) != MP_OKAY)`
   — the assignment target is read in the same expression it's assigned in.

The 4 genuine TPs were all cases where the initial value is provably
unreachable on every path — e.g. `char *cmdname = NULL;` immediately and
unconditionally overwritten with no intervening branch (mosquitto
`client_props.c:65`), or a function whose only `return` uses a literal
instead of the tracked variable (curl `schannel.c:368`).

## Implications

- **MSC12-C should not ship enabled as-is.** 0% measured precision with
  ~4,900 real-world hits; the dominant cause (grouped case-label bodies
  misread as empty) is a single, well-characterized parser/logic bug, not a
  fundamental scope problem — likely fixable by correctly walking a
  case_statement's absorbed-fallthrough body instead of stopping at the
  first case/default boundary.
- **MSC13-C's read-detection needs meaningfully more control-flow
  awareness** before its 20k+ real-world volume is trustworthy: loop-condition
  reads back into the loop's own init/body variables, goto-to-cleanup-label
  reads, and trailing-return reads are all missing from `is_read_context`'s
  walk, which currently only inspects the immediate parent node rather than
  scanning the rest of the enclosing loop/function for a later read.
- Recommend treating this as a P1/P2 fix task (same tier as tasks 350-377,
  the other detection-gap backlog) rather than disabling the rules —
  consistent with this project's standing FN-focus-over-disable policy: the
  ground-truth oracle now correctly measures these rules' real precision, so
  future benchmark runs will show the true cost of leaving them unfixed
  rather than hiding it.

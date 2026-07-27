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
  first case/default boundary. Filed as task 379, not yet fixed.
- **MSC13-C's read-detection needs meaningfully more control-flow
  awareness** before its 20k+ real-world volume is trustworthy: loop-condition
  reads back into the loop's own init/body variables, goto-to-cleanup-label
  reads, and trailing-return reads are all missing from `is_read_context`'s
  walk, which currently only inspects the immediate parent node rather than
  scanning the rest of the enclosing loop/function for a later read.
  Filed as task 380 — **fixed**, see below.
- Recommend treating this as a P1/P2 fix task (same tier as tasks 350-377,
  the other detection-gap backlog) rather than disabling the rules —
  consistent with this project's standing FN-focus-over-disable policy: the
  ground-truth oracle now correctly measures these rules' real precision, so
  future benchmark runs will show the true cost of leaving them unfixed
  rather than hiding it.

## Update: MSC13-C fixed in v0.4.142 (commit 0a248a73, 2026-07-27)

Task 380 fixed the root cause: `check_dead_stores` sorted **all** assignment
sites to a variable across the **whole function** by line number and flagged
consecutive pairs with no read strictly between them — unsound whenever
"consecutive by line number" isn't "consecutive on any executable path"
(mutually exclusive if/else-if branches, a loop's own next-iteration read in
its condition, an assignment before a `goto` paired against an unrelated
later assignment when the real read is at the jump target). Rewrote to only
pair a write with an immediately-following write **in the same straight-line
block**, dropping tracking entirely across any `goto`/label. Also fixed two
related coverage gaps surfaced while recovering lost true positives: the
read-check treated the base of `a.field`/`a[i]` as a pure write when the
whole expression was an assignment LHS (it's always a read — you need the
base value to compute the write address), and the dead-store extractor
skipped pointer-typed and multi-declarator (`char *a = NULL, *b = NULL;`)
declarations entirely.

**Real-world impact** (run sqc-0.4.142-0a248a73 vs. sqc-0.4.141-2acbd455,
same 7 codebases): MSC13-C's raw finding count dropped **91.3%**, 20,816 →
1,809, with no regressions in any other rule.

**Re-measured precision** on a fresh, independently seeded 50-finding sample
drawn from the post-fix run (`sample_msc13_postfix_0.4.142.json` /
`adjudication_msc13_postfix.csv`, same adjudication standard as the original
audit):

| | Pre-fix (0.4.141) | Post-fix (0.4.142) |
|---|---|---|
| Sample TP/50 | 4 | 34 |
| Sample precision | 8.0% | 68.0% |
| Official oracle precision (`bench realworld-score`) | 8.0% | **73.5%** (36 TP / 13 FP) |

Surviving FP patterns in the post-fix sample (worth a follow-up task, not
urgent — precision is now in line with the rest of the rule set):
1. **Self-referential RHS read missed** (5/16): a write immediately followed
   by an expression that reads the same variable as a call argument before
   reassigning it, e.g. `pos = f(pos); pos = g(hapd, pos, ...)` — the second
   write's own RHS reads the pending variable, but the current same-statement
   scan doesn't check a write statement's RHS against its own just-updated
   pending entry from the previous statement.
2. **Macro-hidden reads** (2/16): `IN_RENAME_OBJECT` macro reading `pParse`,
   `##` token-pasting inside a `ROUND()` macro reading `s3` — genuinely
   invisible without a real C preprocessor (see
   `docs/design/macro-expansion.md`).
3. **Assignment-in-condition self-read** (1/16): `while ((err =
   ERR_get_error()))` — the assignment's own value is read by the `while`
   condition it's embedded in.
4. **Misattribution** (5/16): flagged line isn't a variable assignment at
   all — a C++ default-parameter value, a `switch case` label, a struct
   field declaration.

## Update: MSC12-C fixed in v0.4.143 (commit f788a412, 2026-07-27)

Task 379 fixed the documented root cause: `check_empty_switch_case` parsed
grouped case labels (`case 'D': case 'd': ...code...`) as independent
sibling `case_statement` nodes (tree-sitter-c's actual structure — they are
not nested) and flagged the leading, body-less label as an empty case even
though it shares the following label's body. A bare label with no
statements of its own, immediately followed by another case/default label,
is now recognized as a grouped-label member and skipped. A second bug in
the same code path was fixed alongside it: the case-value-skip heuristic
(`prev_named_sibling() == None`) also misfired on a `default:` case's first
real statement (no value node precedes a `default:` label either), causing
misattribution false positives — replaced with an explicit
`child_by_field_name("value")` check.

**Real-world impact** (run sqc-0.4.143-f788a412 vs. sqc-0.4.141-2acbd455,
same 7 codebases): MSC12-C's raw finding count dropped **58.4%**, 4,952 →
2,059, with no regressions in any other rule (MSC12-C's own test suite
also gained a fixed test: `wiki_exceptions_2`, previously a stale baseline
failure).

**Re-measured precision** on a fresh, independently seeded 50-finding
sample drawn from the post-fix run
(`sample_msc12_postfix_0.4.143.json` / `adjudication_msc12_postfix.csv`,
same adjudication standard as the original audit):

| | Pre-fix (0.4.141) | Post-fix (0.4.143) |
|---|---|---|
| Sample TP/50 | 0 | 0 |
| Sample precision | 0.0% | **0.0%** (unchanged) |
| Official oracle precision (`bench realworld-score`) | 0.0% | **0.0%** (unchanged) |

**Precision did not move.** The grouped-case-label bug was real — only 1
residual instance turned up in the post-fix sample (`content_encoding.c:434`,
a `NEEDS_MORE_OUTPUT`/`NEEDS_MORE_INPUT` shared-break pair) — but it was
never the rule's dominant FP driver in the first place; it just happened to
be the single most *characterizable* one. The actual dominant driver,
unchanged by this fix, is structural: `check_empty_control_flow` /
`check_empty_switch_case` treat "syntactically empty body" as "no effect,"
but real-world C is full of *intentionally* empty bodies, and the rule has
no way to distinguish "empty by design" from "empty by mistake." FP
breakdown of the post-fix 50-sample:

1. **Deliberate empty branches with explanatory comments** (`/* No-op */`,
   `/* Do nothing */`, `/* already done */`, `/* Unknown header */`, etc.) — 16
2. **Intentional no-op stub functions** for disabled features or null
   backends (`sme.h`, `tls_none.c`, `crypto_nettle.c:crypto_unload`, etc.) — 9
3. **Deliberate no-op scan/retry loops** doing all work in the loop clauses
   (`while((*dest++=*src++)!=0){}`, `while(recvmsg()==-1 && EINTR);`) — 8
4. **Bare macro tokens with hidden real effects** sqc can't see through
   (`SQLITE_EXTENSION_INIT1`, `S3JniHook_mutex_enter`, `_DefGroup`) — 6
5. **Attribute-macro-terminating semicolons** (`PRINTF_FORMAT(2,3);`) — 2
6. **Misattribution to code with real effects** (a genuine call, a
   substantial case body, labels with real code after an `#ifdef`/`#endif`
   boundary, a C++ class-closing `};`, JS misparsed inside `EM_ASM()`) — 7
7. **Empty cases with clear idiomatic intent from context** (state-machine
   terminal states, sequential unhandled-event cases, uncommented) — 2

**Implication**: unlike MSC13-C, MSC12-C's real fix is not another
targeted parser bug — it needs either (a) an explicit "empty body is OK"
allowlist (a leading comment, a recognized no-op idiom, a stub-function
heuristic) or (b) to stop firing on empty-body patterns entirely and keep
only the higher-confidence sub-checks (self-assignment, duplicate
conditions, redundant logical operators, no-effect expressions), which
were not sampled here since none appeared in either 50-item draw. Given 0%
measured precision on two independent seeded samples and ~2,059 remaining
real-world hits, MSC12-C should be considered for disabling its
empty-body-family checks (or the whole rule) pending that redesign —
this is a `disable`-track recommendation, not a `fix`-and-reverify one,
since the fixed bug did not move the needle and no further quick win is
visible in the current sample data.

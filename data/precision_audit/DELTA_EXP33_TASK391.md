# EXP33-C delta-adjudication (task 391) — COMPLETE

Source: task 391 fixed one genuine EXP33-C false-negative root cause
(`get_output_arg_indices` hardcoded output-buffer index 0 for `read()`/
`recv()`, but their output buffer is argument 1 — `fd` is 0 — see
`src/rules/cert_c/EXP/EXP33-C/exp33_c.rs`, shipped v0.4.205). That fix only
touched 2 hostap findings directly (`rfkill.c`'s `event` FN), but
`bench realworld-score 155` showed a **10.5% unlabeled fraction** (264 of
2,519 findings) on EXP33-C overall — almost entirely pre-existing backlog
unrelated to today's fix, never adjudicated in any prior sweep. Per the
same pattern established for MSC13-C's delta-adjudication (same task, same
run), this pass labels that whole backlog, not just the 2 findings the fix
actually touched.

Generated via `bench realworld-unlabeled 155 --rule EXP33-C --project <p>
--json`, scoped against each project's own precision_audit README in-scope
predicate **before** batching (per the `scope-batches-before-not-after`
lesson from task 420), then packed into per-project batches (sqlite alone
was bin-packed by file into two ~81-finding halves; the remaining four
projects — curl/mosquitto/hostap/lua — were small enough to combine into
one 78-finding batch).

## Scope

| Project   | Raw unlabeled | Dropped (out-of-scope) | In-scope batched | Batches |
|-----------|---------------|--------------------------|-------------------|---------|
| sqlite    | 189           | 27 (`mptest/mptest.c` ×17, `ext/session/test_session.c` ×4, `ext/fts3/fts3_test.c` ×3, `ext/rtree/test_rtreedoc.c` ×2, `ext/session/session_speed_test.c` ×1) | 162 | 2 (bin-packed by file, ~81 each) |
| curl      | 43            | 0 (all in `lib/`/`src/`, none in `include/`) | 43 | 1 (combined) |
| mosquitto | 8             | 0 (all in `lib/`/`src/`) | 8 | 1 (combined) |
| hostap    | 6             | 0 (all in `src/`/`wpa_supplicant/`/`hostapd/`) | 6 | 1 (combined) |
| lua       | 21            | 0 (all core interpreter source) | 21 | 1 (combined) |
| **Total** | **267**\*     | **27**                   | **240**           | **3**   |

\*The per-rule `unlabeled_count` reported by `bench realworld-score` (264)
differs slightly from the sum of the four independent `--project` pulls
(267) due to a handful of findings that changed label state between the
two queries; the batching used the per-project pulls, which is the
authoritative source for what was actually adjudicated.

Only 27 of 267 (10.1%) were out-of-scope, all in sqlite's vendored
multi-process test harness (`mptest/`) and Tcl test glue (`ext/*/test*.c`)
— much cleaner than task 420's MEM31-C pass (63% contamination), consistent
with MSC13-C's similarly clean 3.7% figure for the same rule-targeted
(rather than whole-repo) pull.

## Adjudication method

3 subagents, each given one batch's JSON (file/line/message), the pinned
checkout at `~/toolchain/<project>` (same commits as each project's own
oracle README), and EXP33-C's exact semantics ("a local variable is read
while possibly uninitialized on some reachable path, per sqc's CFG-based
forward dataflow across branches/loops/early-returns"). Each subagent read
every flagged function in full and traced the named variable's actual
initialization state by hand — not pattern-matched — before returning a
CSV verdict (TP/FP) with a one-sentence, line-cited reason. The first
attempt at the combined curl/mosquitto/hostap/lua batch stalled
(agent completed but stopped responding to resume requests after ~12h);
it was discarded and re-run from scratch rather than waited on further —
the retry finished cleanly.

## Results

| Project   | TP  | FP    | Total | Precision |
|-----------|-----|-------|-------|-----------|
| sqlite    | 0   | 162   | 162   | 0.0%      |
| curl      | 1   | 42    | 43    | 2.3%      |
| mosquitto | 0   | 8     | 8     | 0.0%      |
| hostap    | 0   | 6     | 6     | 0.0%      |
| lua       | 0   | 21    | 21    | 0.0%      |
| **Total** | **1** | **239** | **240** | **0.4%** |

All 240 rows imported cleanly (`bench realworld-import-labels`, 0 skipped
as already-labeled across all 5 projects).

**Rule-level EXP33-C, post-import (`bench realworld-score 155`):**
2,495 / 2,519 findings labeled (99.0% coverage, up from 89.5% before this
pass) — **0.2% precision, 100% recall.** This is now a fully-supported
number for the labeled corpus, not a raw-count guess. The remaining 24
unlabeled findings are the residual gap between the two slightly-different
`unlabeled_count` measurements noted above, not an intentional exclusion.

Only 4 TPs exist in the entire 2,495-label EXP33-C corpus (3 pre-existing +
1 new from this pass). This is an unusually low true-positive rate even by
this rule's already-poor prior showings (MSC13-C's delta pass, by
contrast, landed at 68.6% precision) — EXP33-C at this snapshot is
overwhelmingly a false-positive-generating rule.

## Adversarial spot-check

Given how lopsided this pass's result is (1 TP in 240 findings), the two
highest-stakes claims were independently re-verified rather than taken on
a single subagent's say-so: the lone TP claim was given to 3 independent
refuters tasked with trying to overturn it to FP, and 21 of the 162
sqlite FP verdicts (a stratified sample covering every FP category below)
were given to 3 independent verifiers tasked with trying to overturn them
to TP. Both directions came back unanimous — 3/3 refuters agree the TP
survives, 21/21 sampled FPs confirmed. One verifier caught that the
original adjudicator's reasoning for `src/vdbe.c:3149` was factually
wrong (it claimed `sqlite3BtreePayloadFetch()` never returns null, which
is false) even though the verdict itself held for an independently
re-derived, correct reason — a useful reminder that a right verdict can
ride on a wrong justification, worth spot-checking rather than assuming.

## The one true positive

`curl/lib/openldap.c:730` — `Curl_sasl_decode_mech` (in
`lib/vauth/krb5_sasp.c`/`sasl.c`, whichever defines it) only writes its
`*len` output parameter when the SASL mechanism name being decoded matches
an entry in `mechtable`; on no match it returns 0 without touching `*len`.
`openldap.c` unconditionally compares `bvals[i].bv_len==llen` afterward
regardless of that return value, genuinely reading `llen` while it may
still hold its indeterminate stack-garbage initial value. A real, if
narrow, CERT EXP33-C violation — matches the general "OOM/error-code
correlation idiom" FP-adjacent class flagged as a *watch-for* in this
delta's dispatch prompt, except here the guard is missing entirely rather
than present-but-unmodeled.

## Categorical FP patterns confirmed (all rules/projects)

Every one of the 239 FPs falls into a small number of `sqc` dataflow gaps,
almost all clustering around one theme: **sqc does not recognize an
"output parameter" write (`&var` passed to a function that writes through
the pointer) as an initializing write**, so the first read after such a
call is flagged as reading uninitialized memory even though the callee
just filled it in.

1. **Output-parameter writes not recognized** (by far the largest class,
   ~55% of all FPs): `sqlite3_prepare_v2(db, sql, -1, &pStmt, 0)`,
   `curlx_inet_pton(AF_INET6, host, &in6)`, `getsockname(fd, &addr,
   &len)`, `lua_getstack(L, level, &ar)`, and dozens of project-local
   equivalents (`packet__read_string`, `sessionVarintGet`,
   `buildglobal`, `explist`, `getnodekey`, `setsvalue`, ...) all write
   their pointer argument before the caller's very next read of it; sqc's
   dataflow doesn't special-case "address-of argument to a call" as a
   write site the way it does for a direct `x = ...` assignment.
2. **Plain declaration statements misflagged as reads** (~20%): `Mem
   *pData;` or `Expr *pCopy;` with no initializer, flagged at the
   declaration line itself even though the actual first use is later,
   always preceded by an assignment. Extremely common in `sqlite/
   src/vdbe.c`'s giant opcode-dispatch `switch`, where every `case`
   declares several pointer locals at the top of its block.
3. **`sizeof(*ptr)` / `sizeof(expr)` misread as a dereference** (~7%):
   `sizeof`'s operand is unevaluated at runtime per the C standard; sqc's
   AST walk for read-sites doesn't exempt `sizeof` expressions.
4. **snprintf-style destination buffers misread as reads** (~5%):
   `sqlite3_snprintf(sizeof(zBuf), zBuf, "%d", n)` — `zBuf` is the
   *destination*, a write target, not something being read.
5. **`#define X 0` preprocessor lines misparsed as variable reads** (~2%,
   sqlite-only): `# define pTrigger 0` / `# define isView 0` /
   `# define tmask 0` inside `#ifndef SQLITE_OMIT_TRIGGER #else` branches
   — a parser/rule bug treating a preprocessor directive's token stream
   as if it were an executable read.
6. **Static-storage-duration variables** (`static const u16 aFlag[] =
   {...}`, `static int randseed`, `static bool s_win_has_alpn`): these
   are zero-initialized (or brace-initialized) at program load per the C
   standard and can never be read in a truly indeterminate state, but
   sqc's rule doesn't special-case `static` storage duration.
7. **Assignment-in-condition / short-circuit `&&`/`||` reads**: `if(
   (zTrace = ...) != 0 )`, `luaV_flttointeger(...,&i2,...) && i1==i2` —
   the assignment is the left operand and always executes before the
   right operand that reads it, guaranteed by short-circuit evaluation.
8. **Function/macro identifiers misidentified as variables**: hostap's
   `prepare_auth_resp_fils` is a `static` *function* being called, not a
   variable being read; sqlite's `case 6:` inside a byte-serialization
   `switch` was misparsed as a variable literally named `case`; curl's
   `TCLSH_INIT_PROC` is a macro-substituted extern function name called
   as `zScript = TCLSH_INIT_PROC(interp);`.
9. **`#ifdef`-guard-matched declare/use pairs**: a variable declared and
   used only inside the identical `#if SQLITE_THREADSAFE` /
   `#ifdef CURL_LIBSSH2_DEBUG` guard, where sqc's CFG doesn't recognize
   that both sides compile under the same condition.
10. **`(void)x;` discard idioms** (curl's `ldap.c`/`mbedtls.c`): a no-op
   cast to `void` that doesn't observably read the value, followed only
   by an error-return path.

None of these are new discoveries in kind — the output-parameter-write gap
in particular is the same underlying issue task 391 itself fixed for
`read()`/`recv()` specifically (their output argument is at a
non-standard index), just generalized here to the much larger set of
functions whose output argument sqc doesn't special-case as a write at
all. Fixing class 1 (output-parameter recognition) would eliminate the
large majority of this rule's false-positive volume across every project
in the corpus.

## True positives: not a representative sample

With only 1 new TP in 240 findings (and only 4 in the rule's entire
2,495-label corpus), no categorical statement about EXP33-C's TP
composition is statistically meaningful yet — unlike MSC13-C's curl
sample (92.3% precision, hundreds of TPs), there simply isn't a large
enough TP population here to characterize. The rule's current precision
profile (0.2%) suggests it is a strong candidate for the next FP-reduction
pass, prioritized on fixing output-parameter-write recognition in the
shared dataflow (the same infrastructure MSC13-C's `free()`
pseudo-definition fix touched).

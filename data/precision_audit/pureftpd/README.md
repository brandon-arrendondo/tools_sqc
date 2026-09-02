# pure-ftpd ground-truth audit — sqc v0.4.218, codebase commit cc28bff5

pure-ftpd (registry key `pureftpd`, checked out at `~/toolchain/pureftpd`) is
the suite's **SQL-client-API oracle**, onboarded via task 301. Task 8 (paper
taint tracking for CWE-78/CWE-89) needs a codebase that *calls*
`sqlite3_exec`/`mysql_query`/`PQexec` as a client to validate a future
CWE-89 (SQL injection) detection rule — none of the suite's other codebases
do this: sqlite *implements* `sqlite3_exec` rather than calling it, and
Juliet's C suite has no CWE-89 testcases at all (SQL injection is Java-only
there). `~/toolchain/pureftpd` is pinned to commit
`cc28bff52ca28e1d122a2142bf37f2dc578f4d3e`.

**Registry key note**: the upstream project is named `pure-ftpd` (hyphenated),
but both the registry key in `mcp_servers/realworld_server.py` and the
checkout directory basename are `pureftpd` (no hyphen). Two things in
`bench/db.py` break on a hyphenated project name: `ingest_realworld_run`'s
result-filename parsing (`sqc-{project}-{version}-{sha}.json`, split on `-`,
index 1) and `project_relpath`'s `/{project}/`-substring path normalization,
which also requires the checkout directory's basename to equal the project
name. Every other registry key so far happened to avoid this; `pureftpd` is
the first one that would have tripped it.

## Scope: the whole daemon (`src/**` + `puredb/**`)

**This section was widened by task 551 (2026-08-25); the original onboarding
scope is preserved below it.** The machine-readable mirror in
`data/benchmark_repos.json` is `"scope_include": ["src/**", "puredb/**"]`.
Those are the only two directories in the pinned tree holding `*.c`/`*.h`
(123 + 8 = 131 files); `gui/`, `pam/`, `man/` and `m4/` contain none, so no
exclude list is needed.

`ground_truth` currently holds 991 labels for `pureftpd@cc28bff5` across 72
distinct files, and every one of them falls inside that predicate. Only 6 of
those 72 files are the SQL-client files the original audit covered — the
other 66 came from task 551's whole-daemon 10% random sample (573 labels,
seed 20260825, source `precision_audit_task551_10pct_sample`) and from the
per-rule delta passes that followed. Task 578 tracks the remaining ~5,021
unlabeled findings.

The declaration was stale rather than the oracle wrong: it encoded the
original six-file onboarding scope below and was never updated when 551
widened it. Fixed under task 717 — see that task for the measured
92%-out-of-scope signal that surfaced it.

### Original onboarding scope (task 301): SQL-client files only

Unlike libcrc/raylib/lua (whole-project exhaustive labeling), this audit is
scoped to exactly the two files that motivated onboarding this codebase:
`src/log_mysql.c` + `src/log_pgsql.c` (and their headers `log_mysql.h`,
`log_mysql_p.h`, `log_pgsql.h`, `log_pgsql_p.h`) — the MySQL/PostgreSQL
authentication+logging backends, which call `mysql_real_query`/`PQexec` as
a genuine SQL client. **449 findings**, all 449 adjudicated (full coverage
of this scope). The rest of pure-ftpd (`ftpd.c`, `pure-pw.c`, `ls.c`, etc. —
~5,468 more findings across the whole daemon) is registered and scanned
(so the codebase is in the benchmark suite and its per-rule counts are
tracked), but **not yet labeled** — same "Partial" tier as
mosquitto/curl/hostap were before their own incremental audits. A follow-up
task can extend labeling to the rest of the daemon; it wasn't needed to
unblock task 8.

No CWE-89-specific rule exists in sqc yet (that's task 8's job, gated on
this task landing first — see `todo-sqlite-cli show 8`). So none of these
449 findings are CWE-89 findings; they're the *existing* CERT-C ruleset's
findings on real SQL-client code, which is exactly the ground truth a
future CWE-89 rule's real-world validation will need as its scoped
baseline, and useful in its own right as ordinary CERT-C signal.

## Method

Adjudicated in 4 batches of ~110-113 findings (`batch_1.json`..`batch_4.json`)
by four independent agents, each reading the full source files (not just the
flagged line) before judging. Results in `adjudicated_batch_1.json`..
`adjudicated_batch_4.json`, merged into `adjudication_pureftpd_sql_0.4.218.csv`
and imported via `bench realworld-import-labels`.

## Result: 25.4% raw precision (114 TP / 449), 27.3% on unique (file,line,rule) keys (108/395)

The gap between the two percentages is expected: several sqc findings share
a `(file, line, rule)` key (e.g. `API00-C` flagging 4 distinct unvalidated
parameters at one line), which `ground_truth` collapses into a single row —
verified all such collapsed groups agreed on verdict before import, so no
information was actually lost, just deduplicated.

**Dominant false-positive drivers** (consistent across both files):
- **EXP34-C** (0% precision, 0/229 in this scope): the analyzer's null-check
  tracking doesn't credit an in-`if`-condition assignment-and-check
  (`if ((var = call()) == NULL) goto bye;`) or a check made earlier in the
  same function once it re-encounters the variable — pure-ftpd's SQL
  backends are written almost entirely in this idiom.
- **ARR30-C/ARR37-C/STR34-C** (0% precision each): heap buffers accessed via
  pointer subscript/increment (the standard array-via-pointer idiom)
  misclassified as single-object/non-array pointers; STR34-C also flags
  plain char-to-char writes as "sign extension on widening."
- **EXP05-C** (0%, 0/32): `free((void*)x)` on a locally-`const`-qualified
  heap pointer is the standard, CERT-accepted idiom for preventing
  modification through an alias — flagging every such cast is a real
  analyzer gap, not a defect in this code.
- **MSC41-C** (0%, 0/11): flags config-keyword string literals
  (`"argon2"`, `"scrypt"`, `"cleartext"`, ...) as hardcoded secrets — they're
  algorithm-selector tags, not credentials.
- **DCL05-C** (0%, 0/9): misidentifies `PGconn`/`PGresult` as hidden-pointer
  typedefs; both are ordinary struct typedefs used with an explicit `*`.
- **ENV01-C** (0%, 0/8): fires on any fixed buffer whose size macro merely
  *contains* "MAX" (`NI_MAXHOST`, `MYSQL_MAX_REQUEST_LENGTH`); neither file
  calls `getenv()` at all.

**Genuine TPs cluster around**: unchecked `errno`/return values on
`strtoul`/`strtoull`/`atoi` (ERR01/ERR30/ERR33/ERR07/ERR34-C), unsigned
multiplication overflow on DB-derived quota/bandwidth values (INT30-C),
repeated magic numbers (DCL06-C), double-underscore reserved include guards
(DCL37-C), the `ZFREE`/`SNCHECK`-adjacent multiple-evaluation macro
(PRE00/PRE12-C), and the 8-character header-name collision (PRE08-C).

See `data/precision_audit/DELTA_*` for how to delta-adjudicate if the
underlying rules change (per `CLAUDE.md`'s benchmark protocol §6) before
citing a precision/recall change for a rule this audit covers.

## Adversarial re-verification (task 467, 2026-08-26): 27.3% → 17.7%

The original 114 TP verdicts above came from a single-pass adjudication (one
of 4 batch agents per finding, no second skeptic). Following the curl-audit
precedent (`data/precision_audit/curl/README.md`, which flipped 40/140 claimed
TPs on adversarial re-verify), all 114 claimed TPs were re-checked by 3
independent skeptic agents each (majority vote, default to refute when
uncertain), reading the actual code at the pinned `~/toolchain/pureftpd`
commit `cc28bff5`.

**Result: 44 of 114 TPs flipped to FP.** Unique-key precision on run #161
moved from **27.3% (108/395) to 17.7% (70/395)**. `ground_truth` was updated
in place via `bench realworld-import-labels --update` (source tag
`task467_adversarial_reverify`); the original CSV/batch files are left
as-is for history, `ground_truth` is the current source of truth.

Two rule-scope questions came up repeatedly enough across skeptics that a
raw majority vote would have produced internally inconsistent labels for
structurally identical code between `log_mysql.c` and `log_pgsql.c` (same
`strtoul`/`strtoull`-without-errno-handling idiom, opposite verdicts by
file). Both were resolved by reading `src/rules/cert_c/ERR/*` directly
rather than trusting either side's inference about the rule's intent:

- **ERR01-C is genuinely FILE-stream-scoped** — its own `description()` says
  "Use ferror() rather than errno to check for FILE stream errors", and its
  `check_errno_usage` path only fires when a FILE-stream call was seen
  nearby. But a *second*, undocumented check path
  (`check_errno_setting_functions`) also flags any `strtoul`/`strtoull`/etc.
  call with no errno check anywhere in the function, unconditional on any
  FILE stream — a real scope-creep bug in the rule itself, not covered by
  its own description. All 14 such findings (7 in each file) are labeled
  **FP**: the finding's underlying observation is accurate, but it's
  reported under the wrong CERT rule ID, and a reader following "ERR01-C"
  to CERT's real page would find it inapplicable. (Worth a follow-up rule
  fix: `check_errno_setting_functions` should fire as ERR30-C/ERR33-C
  territory, not ERR01-C.)
- **ERR30-C's actual check (`check_inband_function_call`) fires on a missing
  `errno = 0` reset before an in-band conversion call, independent of
  whether errno is ever read afterward** — it does not require "and then
  misreads errno" the way some skeptics assumed from CERT's summary text.
  Every one of these `strtoul`/`strtoull` call sites skips the reset, so
  all 14 are labeled **TP**, matching sqc's literal (and CERT-compliant)
  rule.
- **ERR33-C** (`q > 0` / `q > 0UL` checks after `strtoul`/`strtoull`) stayed
  **TP** across the board: the return value genuinely can't distinguish a
  legitimate 0 from a conversion failure, a real (if low-severity, given the
  code's safe fallback) violation of the coding standard as written — same
  "technically correct even at low severity counts as TP" standard already
  used for the PRE08-C/EXP02-C/EXP14-C-style findings elsewhere in this
  audit, not a judgment about whether the gap is exploitable in practice.

Other high-volume flip categories (no rule-source ambiguity, straightforward
majority vote): all 8 `API00-C` unchecked-pointer-parameter findings on the
two `pw_*_check` entry points (both files) — reached only through one fixed
internal auth-dispatch call site with always-valid stack-allocated
arguments, not an externally-exposed API boundary; several `DCL06-C`
"magic number" findings on self-evident bit-packing/unit-conversion idioms
(byte-shift widths, `0xff` masks, `1024`/`1024*1024` KB/MB factors, `65535`
as max TCP port) that don't obscure intent; both `MEM02-C`
uncast-`malloc`-to-typed-pointer findings on `log_mysql.c` (harmless modern
C, not a genuine defect); `MEM10-C`'s two flags of an ordinary
`if (ptr == NULL) return;` guard (not the ad-hoc-duplicated-validation
pattern the rule targets); and the `PRE08-C` 8-character header-collision
finding on `log_mysql.c` (refuted — no practical filesystem/linker collision
on any real target; the `log_pgsql.c` sibling finding at line 9 stayed TP on
a literal reading, an acknowledged inconsistency left as-is rather than
force-resolved).

## False negatives (task 468, 2026-08-27)

Task 301/467's adjudication only scored what sqc *did* flag (TP/FP verdicts
on the 449 findings). This pass instead independently close-read
`log_mysql.c`, `log_pgsql.c`, and their headers at the pinned commit
`cc28bff5` (confirmed == the checkout's current `HEAD` for these files — no
drift) hunting for constructs the currently-enabled ruleset (`rules-all.toml`,
~312 rule blocks) *should* plausibly catch but didn't. This is not a CWE-89
hunt (no such rule exists yet — separate task); it's about the existing
ruleset's blind spots.

**Upstream history cross-check**: the three known security-relevant fixes in
this file's git log (`efcc820` "Use escaped strings in the pgsql connection
string", `4019168`, `c3f0f3c` "PostgreSQL: don't escape the port number") are
already incorporated in the pinned commit — nothing outstanding there. Two
other historically-notable commits were checked for recurrence and found
clean: `f4dd5ef` ("Don't << 16 or << 24 on an int or anything promoted to an
int") — the decimal-IP-packing code at `log_mysql.c:357`/`log_pgsql.c:436`
already casts each byte to `(unsigned long)` before shifting, so the fix
holds; `0398dba`/`fe4413a` (canary buffer under-allocation) — the current
canary-buffer sizing (`to_len + 4U` in `log_mysql.c`, `to_len + 2U` in
`log_pgsql.c`) is one of the few things `INT32-C` and `ERR33-C`/`MEM02-C`/
`MEM04-C`/`MEM06-C` *do* already flag on this repo (see the finding list at
`log_mysql.c:59` / `log_pgsql.c:62`), so no gap there either.

### Confirmed FN: unsigned-multiply size computation escapes overflow detection when split from its `malloc` call

**One genuine, reproducible miss, found in 3 places** — all instances of the
same shape:

```c
from_len = strlen(from);
to_len = from_len * 2U + (size_t) 1U;      /* <-- zero findings on this line */
if ((to = malloc(to_len + (size_t) 4U)) == NULL) {   /* <-- INT32-C fires here */
```

- `log_mysql.c:58` — `pw_mysql_escape_string()`, `from` is `account`
  (attacker-controlled FTP username) or a `getnameinfo()`-derived host/port
  buffer.
- `log_pgsql.c:61` — `pw_pgsql_escape_string()`, same shape, same taint
  source.
- `log_pgsql.c:252` — `pw_pgsql_escape_conninfo()`, `from` is a config value
  (`server`/`db`/`user`/`pw`), lower attacker-relevance but the same
  analyzer gap.
- Also `log_pgsql.c:289-291` — `pw_pgsql_connect()`'s `sizeof_conninfo`
  computation (`sizeof PGSQL_CONNECT_FMTSTRING + strlen(...)*4 + 5U`) feeding
  `malloc(sizeof_conninfo)` two lines later at `log_pgsql.c:292`: zero
  findings on the multi-term addition itself, same class of gap.

**Why it's a genuine miss, not a documented idiom**: `INT32-C` demonstrably
*does* catch overflow-prone arithmetic when it appears directly inside a
`malloc()` call's argument expression — e.g. `log_mysql.c:59`'s
`malloc(to_len + (size_t) 4U)` is flagged (`to_len` treated as an opaque
tainted operand). But the rule does not walk back one more hop to the
assignment that actually *computed* `to_len` (`from_len * 2U + 1U`, an
unsigned multiplication of an attacker-influenced length with no upper bound
check before it), even though that assignment sits two lines above and feeds
directly into the flagged `malloc` call. On the sizeof_conninfo case the gap
is starker: the multi-term addition that IS the size computation gets zero
findings at all, only in a different function than the one that eventually
mallocs with it — no rule appears to track that dataflow. This is an
architectural blind spot (single-hop-back taint from a `malloc` call site,
not full backward slicing to the defining assignment), not a CERT scoping
question — `INT30-C`/`INT32-C`'s own descriptions cover exactly this
"unsigned wraparound feeding an allocation size" pattern.

**Practical severity note**: in this specific codebase, `from_len` is
bounded in practice by the FTP command-line buffer the caller reads into
(pure-ftpd's line-length cap), so this is not believed to be a *reachable*
overflow here — but that's an operational mitigation sqc's static analysis
has no visibility into, and the rule is meant to catch the pattern
regardless of an out-of-file bound. Recorded as a real analyzer miss, not
labeled a TP in `ground_truth` (no finding exists at these lines to label).

**Everything else checked came back clean** (documented as an explicit
negative result per the FN-hunt convention, not just omitted): resource-leak
paths in `pw_mysql_connect`/`pw_pgsql_connect`/`pw_mysql_check`/
`pw_pgsql_check` (every `mysql_init`/`PQconnectdb`/`malloc` handle is freed
on every exit path, including the "init succeeded, connect failed" case,
which is correctly propagated back through the `MYSQL**`/`PGconn**` output
parameter and closed by the caller's `bye:` label); the `strncpy`+explicit-
NUL-terminate pattern in both `*_getquery()` functions (correctly sized,
no overflow); the deliberate infinite `for(;;) *to++ = 0;` self-wipe loops
on canary corruption (intentional defensive design, not a missed-detection
target); and `log_pgsql.c`'s unused `scrambled_password` local (dead but
harmless — `free(NULL)` — not a rule-relevant defect).

**Follow-up recommendation** (not filed as a task here — flagging for the
coordinator to decide): if `INT30-C`/`INT32-C`'s malloc-argument check were
extended to walk back through one level of straight-line variable
definition (not full SSA/backward-slicing, just "the immediately-preceding
assignment to this identifier, if it's arithmetic on a taint-carrying
operand"), it would close all 4 instances above without materially changing
the rule's precision profile on this corpus (the existing MSC-class
FP drivers here are unrelated to `INT30-C`/`INT32-C`).

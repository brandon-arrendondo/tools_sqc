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

## Scope: SQL-client files only (not the whole daemon)

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

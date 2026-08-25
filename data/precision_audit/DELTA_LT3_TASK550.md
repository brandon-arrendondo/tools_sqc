# Long-tail rules group 3 delta-adjudication (task 550) — COMPLETE

Part of task 532's breakdown, the last of the P4 long-tail bundles and
the final piece of the orig-7-projects delta-adjudication sweep (tasks
534-550). 51 raw unlabeled findings across 36 rules, nearly all with
just 1-2 findings each. Adjudicated directly (19 in-scope findings after
32 dropped as out-of-scope — small enough not to need a subagent batch).

## Outcome

18 FP / 1 TP (5.3% batch precision). All 19 findings were individually
verified against the actual source.

## The 1 TP: a genuine test-harness divide-by-zero

**INT33-C, sqlite `ext/fts5/fts5_tcl.c:1313`** — `f5t_fts5HashKey`'s
`nSlot` parameter is read directly from a Tcl test-script integer
argument via `Tcl_GetIntFromObj` with no zero-check before being used as
a modulo divisor. Invoking the Tcl test command `f5t_token_hash 0 "foo"`
crashes with a real divide-by-zero. Test-harness-only impact (this file
is only reachable from SQLITE_TEST Tcl test scripts), but a genuine,
unguarded violation.

## FP causes — mostly single-instance checker mechanics, no dominant pattern

- **API05-C** — C11 Annex K `[static N]` conformant array syntax,
  consistent with established precedent (zero real-world adoption on any
  prior oracle).
- **ARR01-C** — `sizeof` applied to a struct *field* (`mosq->sock`), not
  the decayed pointer `mosq` itself — checker attributed the finding to
  the wrong operand.
- **CON04-C** — the thread handle genuinely IS joined, just in a
  different function (`sqlite3ThreadJoin`) than the one that created it —
  the checker's join-tracking doesn't look across functions.
- **DCL02-C (2 instances, lua + sqlite)** — the "visually similar
  identifiers" claim (`h` vs. `n`) doesn't hold up to a human reader in
  either instance; a normalization-heuristic bug, independently confirmed
  in two codebases.
- **DCL18-C** — `umask(0007)` is the standard, intentional octal
  permission-mask idiom, not an accidental-octal bug.
- **ENV30-C** — inside dead `#ifdef WIN32` code on the Linux benchmark
  host; the checker also conflates two mutually-exclusive branches
  (`LocalFree` frees a `FormatMessage`-allocated buffer, never the
  `#else` branch's `strerror()` result it claims).
- **EXP42-C (2 instances)** — `memcmp` compares fixed-size,
  no-padding fields (`struct in_addr`/`struct in6_addr`) directly, not a
  padded-struct comparison; the checker flags any `memcmp` on a
  struct-typed operand regardless of whether padding actually exists.
- **FIO10-C** — POSIX-only file (`os_unix.c`); `rename()`'s
  destination-overwrite semantics are well-defined on POSIX, and the
  return value is checked.
- **INT00-C (2 instances)** — `%lld` genuinely, correctly matches the
  variable's declared `sqlite3_int64` (64-bit) type; no data-model
  assumption is violated.
- **MSC14-C** — the call is inside a preprocessor guard explicitly
  selecting the POSIX (int-returning) `strerror_r` variant and correctly
  treats the return as an int; the ambiguity the rule warns about has
  already been resolved.
- **PRE02-C / PRE10-C** — both fire on the same macro
  (`MOSQUITTO_PLUGIN_DECLARE_VERSION`), which expands to an entire
  top-level function *definition*, not an expression or a
  multi-statement block meant to be embedded as a single caller
  statement — neither rule's underlying concern applies to this macro
  shape.
- **STR00-C** — all char-literal bytes in the mixed hex/char magic-number
  array are provably ASCII, no sign-extension risk; a standard binary
  magic-header idiom.
- **STR05-C (2 instances)** — C++ default-argument values on an
  already-`const`-qualified declaration-only prototype parameter in
  `libmosquittopp.h`, misread as unqualified pointer-variable
  assignments. **The 8th+ rule now confirmed hitting this same C++ header**
  — folds into the existing task 571 umbrella.

## Task 532 umbrella status

This closes the last of the 10 P2 individual-rule tasks, 4 P3 mid-tier
bundles, and 3 P4 long-tail bundles (534-550) — the full orig-7-projects
delta-adjudication sweep against the v0.4.120 baseline is now complete.
Remaining under the task 532 umbrella: **551** (pureftpd coverage
expansion, ~5,844 findings) and **552** (sel4 coverage expansion,
~4,905 findings) — both explicitly flagged as needing a
sampling-vs-full-coverage scope decision before starting, given their
volume.

## Follow-up

No new tasks filed. The 2 STR05-C findings fold into the existing task
571 umbrella (libmosquittopp.h). The remaining FP causes are each
single-instance checker-mechanics gaps too thin to generalize from on
their own (1-2 findings each) — worth revisiting only if future delta
passes surface more instances of the same specific rules.

CSVs: `data/precision_audit/{mosquitto,sqlite,lua,curl}/import_delta_lt3_task550.csv`.

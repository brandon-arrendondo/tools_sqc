# MSC24-C delta-adjudication (task 540) — COMPLETE

Part of task 532's breakdown (17,952 unlabeled findings across 205 rules,
run 187 v0.4.258 vs the v0.4.120 baseline). This tracks the delta pass for
**MSC24-C** ("do not use deprecated or obsolescent functions") — 234 raw
unlabeled findings, heavily concentrated in raylib (210 of 234).

## Scope

Derived each project's in-scope file predicate from its own
`data/precision_audit/<project>/README.md` before pulling findings:

| Project   | Raw unlabeled | Dropped (out-of-scope)                                   | In-scope | Batches |
|-----------|---------------|--------------------------------------------------------------|----------|---------|
| raylib    | 210           | 0                                                              | 210      | 2       |
| sqlite    | 12            | 6 (`mptest/`, `ext/*/test_*.c`, `ext/session/changesetfuzz.c`) | 6        | 1 (combined) |
| mosquitto | 7             | 0                                                              | 7        | 1 (combined) |
| lua       | 4             | 0                                                              | 4        | 1 (combined) |
| curl      | 1             | 0 (`lib/memdebug.c` — debug-build-only but still real, shippable-when-`CURLDEBUG`-enabled code, not dead) | 1 | 1 (combined) |
| hostap    | 0             | —                                                               | 0        | — |
| **Total** | **234**       | **6**                                                          | **228**  | **3**   |

sqlite/curl/mosquitto/lua's small residual counts (18 total) were combined
into a single batch.

## Different rule shape: a literal function-usage ban, not a context bug

Unlike the bounds/null-safety rules delta-adjudicated so far (MSC01/MEM31/
ARR30/ARR37/EXP34), MSC24-C flags a **specific, named call to a deprecated
libc function** (`sprintf`, `strcpy`, `strcat`, `sscanf`, `strtok`,
`rewind`, `setbuf`, …) and recommends a safer alternative. Per this repo's
standing philosophy (sqc surfaces every genuine rule violation as written;
noise/applicability judgment belongs to the user via suppression/config,
not the rule), adjudication defaulted to **TP whenever the flagged call is
live, real code** — regardless of whether the specific call site happens
to be safe in practice (a small fixed buffer being `sprintf`'d into still
counts as "using `sprintf`", which is exactly what the rule prohibits).
This produces a fundamentally different precision profile than the
bounds-safety rules.

## Outcome

| Project   | Findings | TP  | FP | Precision |
|-----------|----------|-----|----|-----------|
| raylib    | 210      | 196 | 14 | 93.3%  |
| sqlite    | 6        | 6   | 0  | 100.0% |
| mosquitto | 7        | 7   | 0  | 100.0% |
| lua       | 4        | 4   | 0  | 100.0% |
| curl      | 1        | 1   | 0  | 100.0% |
| **Total** | **228**  | **214** | **14** | **93.9%** |

Post-import measured precision for MSC24-C over the full labeled set
(`bench realworld-score 187`): **77.6%** (215 TP / 277 labeled), **100%
recall**. 22 findings remain unlabeled. This is by far the highest
precision measured in any delta pass so far — confirming MSC24-C is a
genuinely high-value, low-noise rule in this suite.

## The 14 FPs: one clean, fixable constant-folding gap

All 14 FPs are in a single raylib function, `ExportFontAsCode()` in
`src/rtext.c`, and share one root cause: **sqc doesn't evaluate the
`#define`/`#if defined()` relationship within the same file** to determine
a branch is dead:
- 6 FPs: `#if defined(SUPPORT_COMPRESSED_FONT_ATLAS)` / `#else` where
  `SUPPORT_COMPRESSED_FONT_ATLAS` is unconditionally `#define`d two lines
  above in the same function — the `#else` branch is provably dead, but
  sqc's `sprintf` scan still flags the call inside it.
- 8 FPs: `#if defined(SUPPORT_FONT_DATA_COPY)` where the corresponding
  `#define` is commented out (`//#define SUPPORT_FONT_DATA_COPY`) — same
  class of provably-dead branch.

This is the same general problem class as the existing `#if 0` dead-region
suppression scanner (`suppression.rs`, task 229) — an unconditionally-true/
false local `#define` should be recognized the same way — but is a
distinct, narrower case (a real macro whose definedness is locally
constant, not a hardcoded `#if 0`).

## Follow-up

Filed **task 560**: extend the dead-code/suppression scanner to recognize
a `#if defined(MACRO)` (or `#ifdef MACRO`) branch as dead when `MACRO` is
provably always-defined (an unconditional `#define MACRO` earlier in the
same file with no matching `#undef`) or provably never-defined (a
commented-out `#define MACRO`, or no `#define` anywhere in scope) — same
class of gap as the existing `#if 0` scanner but for named, locally-defined
macros rather than the literal `0` constant. All 14 measured FPs here would
be eliminated by this fix.

CSVs: `data/precision_audit/{raylib,sqlite,mosquitto,lua,curl}/import_delta_msc24_task540.csv`.

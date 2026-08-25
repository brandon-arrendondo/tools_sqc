# Long-tail rules group 1 delta-adjudication (task 548) — COMPLETE

Part of task 532's breakdown, the first of 3 P4 long-tail bundles. 573 raw
unlabeled findings across **35 rules**, each individually small
(9-27 findings): STR34-C, API02-C, MSC05-C, ARR38-C, ERR00-C, EXP33-C,
EXP20-C, DCL41-C, STR31-C, EXP36-C, EXP43-C, ERR07-C, DCL40-C, MSC09-C,
PRE01-C, DCL37-C, EXP05-C, DCL03-C, ERR34-C, INT13-C, PRE06-C, EXP37-C,
FIO24-C, INT07-C, INT14-C, PRE32-C, STR03-C, INT31-C, PRE00-C, PRE12-C,
DCL20-C, SIG00-C, ERR05-C, MEM05-C, SIG01-C.

## Scope and method

263 of 573 raw findings were out-of-scope (test/example/Windows/macOS
dead code, same categories as prior tasks; 4 rules — ERR05-C, ERR07-C,
ERR34-C, INT07-C — had zero in-scope findings after exclusion). 310
in-scope findings, batched into 3 combined batches grouping multiple
rules together (rather than one batch per rule, given how small most
rule counts were) with per-rule adjudication guidance embedded in each
agent's prompt. 306 unique label rows after same-line consolidation, **93
TP / 213 FP** overall (30.4% batch precision — notably higher than the
mid-tier bundles, since several long-tail rules here are mechanically
verifiable syntactic facts rather than context-dependent judgment calls).

| Rule | TP | FP | | Rule | TP | FP |
|---|---|---|---|---|---|---|
| SIG00-C | 8 | 0 | | DCL37-C | 5 | 9 |
| SIG01-C | 7 | 0 | | EXP05-C | 3 | 3 |
| PRE00-C | 7 | 0 | | INT31-C | 3 | 5 |
| PRE12-C | 7 | 0 | | DCL03-C | 3 | 0 |
| DCL40-C | 15 | 0 | | ERR00-C | 1 | 14 |
| MEM05-C | 4 | 0 | | STR03-C | 1 | 8 |
| EXP20-C | 14 | 1 | | EXP33-C | 0 | 1 |
| MSC09-C | 7 | 9 | | PRE01-C | 0 | 1 |
| PRE06-C | 8 | 3 | | PRE32-C | 0 | 3 |
| | | | | API02-C, ARR38-C, DCL20-C, DCL41-C, EXP36-C, EXP37-C, EXP43-C, FIO24-C, INT13-C, INT14-C, MSC05-C, STR31-C, STR34-C | **0** | **136 combined** |

## Rules that came back 100% (or near-100%) TP — mechanically verifiable, working correctly

**DCL40-C (15/15 TP)**, **DCL03-C (3/3)**, **SIG00-C (8/8)**,
**SIG01-C (7/7)**, **PRE00-C (7/7)**, **PRE12-C (7/7)**, **MEM05-C
(4/4)**: these are all rules whose violation is a directly checkable
syntactic/mechanical fact (a real 31-character identifier collision, a
literal constant-expression `assert`, a genuine `signal()` call, a macro
body that really does reference its parameter twice, a verified real
recursive call chain) — every claim was individually confirmed correct
by reading the actual code. **No rule-logic issue for any of these
rules** — they're working as designed on this sample. **EXP20-C (14/1)**
similarly mostly TP by the rule's literal text (`!strcmp(...)` is
genuinely an implicit boolean test, even though it's an extremely common,
low-severity idiom) — includes one sharper genuine bug
(`mosquitto/subs.c:101` tests `== 1` against an error-return enum with
30+ nonzero values, silently treating any error other than one specific
enum value as success).

## Rules that came back 0% (or near-0%) — clustered, mostly-fixable causes

- **API02-C (0/27)** + **DCL20-C (0/11)** + **EXP37-C (0/12)** — all 3
  fire almost entirely on `mosquitto/include/mosquitto/libmosquittopp.h`,
  applying **C-only rules to a C++ header** where the ambiguity these
  rules exist to catch (bare `()` argument-list, unmarked
  variable-length-buffer parameters without size args being confused for
  function-pointer/callback parameters) doesn't structurally exist in
  C++. **This is now the 6th+ distinct rule** (after DCL15-C/task 561,
  MSC13-C/task 564, WIN04-C/task 566, DCL19-C/task 561-corroboration) to
  misfire specifically on this one file — overwhelming evidence of one
  shared root cause: sqc likely parses this C++ header with C-oriented
  AST assumptions across many rules that share a common
  declaration-inspection helper.
- **DCL41-C (0/21)** — 100% FP, all in sqlite's `ext/fts5/fts5_tcl.c`:
  every flagged line is a `CASE(n, "...")` macro invocation expanding to
  `case n: assert(...);` — sqc's macro-blind switch-statement parser
  doesn't see the real `case` label hidden inside the macro expansion and
  misreads the call as a statement preceding the first visible label.
- **EXP36-C (0/13)** — 100% FP, a clean engine bug: every flagged cast
  targets a byte-sized type (`uint8_t`/`unsigned char`/`char`, real
  `alignof` 1), but the tool reports a bogus "alignment 4" for the target
  type, fabricating a mismatch that doesn't exist. One finding
  (`whereexpr.c:1532`) didn't even correspond to any pointer cast in the
  code at all.
- **FIO24-C (0/13)**, **ARR38-C (0/6)**, **STR31-C (0/6)** — mostly the
  familiar mutually-exclusive-branch pattern (each close/copy on its own
  return-terminated path) or genuinely-bounded operations the checker
  didn't trace through.
- **EXP43-C (0/10)** — the checker flags "overlapping memcpy" purely
  because src/dst textually derive from the same base pointer/struct
  name, without verifying they're actually the same memory region —
  every instance was a distinct allocation or distinct struct field.
- **MSC05-C (0/25/26)** and **INT13-C (0/4)**/**INT14-C (0/4)** — genuinely
  low-signal rules on this sample: standard POSIX `time_t` arithmetic and
  the ordinary positive-bitmask/promoted-unsigned-value idioms,
  respectively. Consistent with this repo's precedent of rules that are
  advisory-in-letter but essentially-never-a-real-bug in mature C
  codebases (cf. API05-C).
- **STR34-C (0/4)** — the rule's premise (sign-extension risk on a signed
  `char` *read*) doesn't apply to the flagged instances, which are all
  literal `*ptr = '\0'` *writes* of a compile-time-constant value — a
  scope bug (the rule shouldn't fire on constant-value assignment at
  all).

## Follow-up

Filed:
- **task 571**: the `libmosquittopp.h` C-rules-on-C++-header
  misapplication, now confirmed across 6+ distinct rules (API02-C,
  DCL20-C, EXP37-C here; DCL15-C/DCL19-C via task 561; MSC13-C via task
  564; WIN04-C via task 566) — proposing this be investigated as ONE
  shared root cause rather than continuing to accumulate one task per
  rule.
- **task 572**: EXP36-C's alignment computation reports a fabricated
  alignment value (4) for provably byte-sized target types — 13/13 FP,
  clean and precisely reproducible.
- **task 573**: DCL41-C doesn't expand `CASE(n, msg)`-style macros before
  switch-statement structural analysis, missing the real `case` label
  hidden inside — check `docs/design/macro-expansion.md` first per
  CLAUDE.md's macro-engine-check-before-heuristic guidance, since this may
  already be a solved problem for other rules.
- **task 574**: STR34-C fires on a constant-value assignment
  (`*ptr = '\0'`) where its sign-extension-on-read premise doesn't apply
  at all — should only fire on reads of a signed-char value in a
  sign-sensitive context, not writes.

Not filed separately (small samples, 3-10 findings, less clean single
root causes): EXP43-C's same-base-pointer aliasing heuristic, FIO24-C/
ARR38-C/STR31-C's mutually-exclusive-branch blindness (same broad class
as task 563's MEM30-C finding — could be addressed together in a future
CFG-path-sensitivity pass rather than per-rule).

CSVs: `data/precision_audit/{mosquitto,curl,sqlite,hostap,raylib,libcrc}/import_delta_lt1_task548.csv`.

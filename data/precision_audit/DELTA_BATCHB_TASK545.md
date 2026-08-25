# Mid-tier rules batch B delta-adjudication (task 545) — COMPLETE

Part of task 532's breakdown. Bundle of 5 rules, 297 raw unlabeled findings
across the orig-7 projects: **WIN04-C** (function-pointer-storage
encryption advisory), **MEM12-C** (resource-leak-on-early-return),
**ARR00-C** (uninitialized-array-read / out-of-bounds constant index),
**ARR02-C** (implicit array bounds), **INT30-C** (unsigned wraparound).

## Scope

162 of 297 raw findings were out-of-scope (test/example/fuzz/Windows-only
files — same categories as prior tasks). 135 in-scope findings batched
(WIN04-C's 68 raw / 40 unique-line adjudicated directly, mechanically
verified rather than via subagent; the rest across 4 subagent batches).

| Rule    | Raw | In-scope | TP | FP  | Batch precision |
|---------|-----|----------|----|-----|-----------------|
| WIN04-C | 69  | 68 (40 unique lines) | 0 | 40 | 0.0% |
| MEM12-C | 65  | 35       | 4  | 31  | 11.4% |
| ARR00-C | 59  | 7        | 0  | 7   | 0.0% |
| ARR02-C | 52  | 6        | 0  | 6   | 0.0% |
| INT30-C | 52  | 48       | 0  | 48  | 0.0% |
| **Total** | **297** | **164** | **4** | **131** (135 label rows) | **3.0%** |

Post-import measured precision over the full labeled set
(`bench realworld-score 187`): WIN04-C **0.0%** (0/136, no TPs exist at
all in the labeled set), MEM12-C 2.8% (85.7% recall), ARR00-C 0.4% (62.5%
recall), ARR02-C 6.3% (100% recall), INT30-C 2.4% (72.7% recall).

## WIN04-C: adjudicated directly, mechanically — checker malfunctioning on this file

All 68 raw findings (40 unique lines) are in mosquitto's
`include/mosquitto/libmosquittopp.h`, the exact same C++ header already
flagged for a declaration-only-prototype misparsing bug in MSC13-C
(task 544) and default-arg misparsing in WIN04-C itself here. Reading the
actual file showed something more severe than "declaration parsed as
storage": **most flagged lines don't even contain a function-pointer
token at all** — e.g. `int port=1883`, `const char *username=NULL` plain
default-argument lines are flagged under WIN04-C's "function pointer
stored without encryption" message. Only 2 of the ~15 declarations in this
span genuinely have a function-pointer *parameter*
(`subscribe_callback`'s `callback`, `tls_set`'s `pw_callback`) — and even
those are call-signature parameters, never persisted/stored anywhere, so
WIN04-C's actual concern (a long-lived function pointer in memory that an
attacker could overwrite/hijack) doesn't apply regardless. **100% FP**,
verified without a subagent since the finding is entirely structural
(check for a function-pointer *storage site* vs. a parameter in a
prototype with no body).

## Other FP causes

- **ARR00-C (sqlite, 5/5 FP)** — the checker's "unchecked
  function-parameter index" and "negative subscript" sub-checks in
  `ext/fts5/fts5_tcl.c` are systematically wrong: flagged offsets are
  tokenizer-produced values already bounds-consistent with their buffer
  (two directly adjacent to a `nReq > p->nBuf` realloc guard), and the
  flagged "negative subscript" `x[-8]` is a deliberate header-offset
  `ckfree` recovering a `ckalloc(nText+8)` pointer — correct C, not a bug.
- **ARR02-C (sqlite+hostap, 6/6 FP)** — every flagged `T x[]` is either
  (a) sized unambiguously by its brace initializer list (`int arr[] =
  {1,2,3};`, a standard, well-defined idiom CERT's rule doesn't target),
  or (b) an array-type function parameter (which C always adjusts to a
  pointer type — ARR02-C targets array *object* declarations, not
  parameters). **0% precision, systematic misses of both common
  legitimate array-declaration idioms** — likely the dominant FP driver
  for ARR02-C generally, not project-specific.
- **INT30-C (hostap 33/33, mosquitto 14/14, raylib 1/1 — 48/48 FP)** —
  every operand is either a fixed-size stack buffer offset
  (`ptr + sizeof(local_buf)`), a protocol/MTU-bounded network-field length
  (RADIUS/MKA/EAP/802.11 IE), a read-loop accumulator bounded by the exact
  "remaining bytes" value just passed to the read call, or already guarded
  by an explicit zero/minimum check in the same function — none come
  remotely close to the operand magnitudes needed for a real unsigned
  wrap. No genuine attacker-controlled-unbounded-wraparound case found in
  this batch.

## MEM12-C: 4 real leak bugs found in mosquitto's listener setup

`src/net.c`'s `net__socket_listen_tcp`/`net__socket_listen_unix` have
**inconsistent cleanup across sibling error branches** — most
bind()/listen()/interface-bind failure paths correctly call
`mosquitto_FREE(listener->socks)` and close the socket, but 4 sibling
paths (lines 891, 990, 995, 999) skip it, leaking the socket fd and/or the
`listener->socks` heap array. The remaining 31 FPs were mostly missed
frees the checker's control-flow analysis didn't follow, missed ownership
transfer via an out-parameter, or an `#ifdef WIN32` boundary
double-counting variables that don't exist in the branch being flagged.

## Follow-up

Filed:
- **task 566**: WIN04-C's function-pointer-storage detection flags
  arbitrary declaration lines that don't even contain a function-pointer
  token, in addition to misclassifying declaration-only prototype
  parameters as "stored" pointers — likely shares root cause with the
  MSC13-C/task-564 C++-header-prototype-misparsing bug (same file, same
  header-parsing code path); investigate together.
- **task 567**: ARR02-C doesn't recognize initializer-inferred array
  sizes (`T x[] = {...}`) or array-type function parameters as having a
  well-defined/inapplicable bound — 0% precision (0/6) here, both causes
  independently confirmed across sqlite and hostap.

Not filed as separate tasks (noted for context, no clean single fix
identified): ARR00-C's tokenizer-offset false positives (`fts5_tcl.c`
only, 5 findings, too narrow to generalize from) and INT30-C's broad
context-blindness (fixed-buffer-offset / protocol-bounded-length / guarded
patterns — spans many independent causes, not one fixable gap; consistent
with INT-family rules generally needing taint-gated opt-in per the
existing `int-overflow-rule-redesign-direction` decision rather than
per-pattern patching).

CSVs: `data/precision_audit/{hostap,mosquitto,sqlite,raylib}/import_delta_batchB_task545.csv`.

# Mid-tier rules batch C delta-adjudication (task 546) — COMPLETE

Part of task 532's breakdown. Bundle of 5 rules, 211 raw unlabeled
findings across the orig-7 projects: **CON07-C** (compound ops on shared
variables not atomic), **INT32-C** (signed integer overflow), **MSC21-C**
(loop termination robustness), **API00-C** (unvalidated pointer
parameter), **EXP39-C** (incompatible-type access / union punning).

## Scope

70 of 211 raw findings were out-of-scope. 141 in-scope findings batched
into 7 subagent batches (some combined across 2 rules or 4 projects to
keep batch sizes reasonable), yielding 139 unique label rows.

| Rule    | Raw | In-scope | TP | FP  | Batch precision |
|---------|-----|----------|----|-----|-----------------|
| CON07-C | 51  | 45       | 0  | 45  | 0.0%  |
| INT32-C | 42  | 6        | 0  | 6   | 0.0%  |
| MSC21-C | 41  | 41       | 0  | 41  | 0.0%  |
| API00-C | 39  | 11       | 0  | 9 (2 same-line consolidated) | 0.0% |
| EXP39-C | 38  | 38       | 3  | 35  | 7.9%  |
| **Total** | **211** | **141** | **3** | **136** | **2.2%** |

Post-import measured precision over the full labeled set
(`bench realworld-score 187`): CON07-C 2.4% (100% recall), INT32-C 4.2%
(76.9% recall), **MSC21-C 0.0% (0/51, no TPs exist at all in the labeled
set)**, API00-C 0.5% (50% recall), EXP39-C 4.5% (75% recall).

## MSC21-C: rule-scope bug, 100% FP, independently confirmed across 5 projects

**Every one of 41 findings across lua, curl, mosquitto, raylib, and
sqlite is FP**, converging on the same root cause: MSC21-C is meant to
flag arithmetic loops using `!=`/`==` termination with a non-unit step
(risking skipping the target). Instead it fires on loop shapes that have
**no numeric step at all**:
1. **Pointer-chasing linked-list traversal** (`p = head; p != NULL; p =
   p->next`) — 16+ of the 23 small-batch findings plus all 18 lua
   findings. A pointer walk terminates exactly at its sentinel (NULL, or
   a known list member) by construction; there is no way to "step past"
   it the way an arithmetic increment could.
2. **Function-return-driven loop conditions** — the tested variable is
   set by a function call inside the loop body, not by any arithmetic
   step (`rc = f(...); rc == SUCCESS; rc = f(...)`).
3. **Non-literal step provably always ±1** (raylib: a variable
   initialized to 1 and only ever reassigned to -1) — the checker
   apparently only recognizes a step as "unit" when it's a literal
   constant, not when it's provably ±1 by dataflow.
4. **Step-detection targeting the wrong sub-expression** entirely
   (sqlite `whereexpr.c:595`, a genuine `op++` +1 step where the checker
   seems to have keyed on an unrelated comparison expression).

## CON07-C: 100% FP, dominant cause is not recognizing non-mutation/single-thread

All 45 findings (sqlite + curl/lua/mosquitto/raylib) are FP, converging on
three causes: (1) the flagged "shared static" is actually `static const`
— a read-only lookup table never mutated at all, misidentified as a
compound-RMW target; (2) the call site is genuinely single-threaded by
design (raylib's main thread; mosquitto's sequential startup path before
the event loop/workers start) even though the variable has static storage
duration; (3) the mosquitto signal-handler-flag idiom (`flag_*` set by a
signal handler, polled once per main-loop tick) is a standard, correct
pattern, not an unsynchronized RMW race.

## API00-C / INT32-C: FPs from missed cross-file contracts and existing guards

API00-C (9/9 FP) misses parameters covered by a strong, codebase-wide
calling convention the single-file checker can't see across translation
units — curl's cfilter chain, SQLite's extension-loader/UDF-callback ABI,
hostap's config-parser dispatch table — or a callee that itself
null-checks. INT32-C (6/6 FP) misses an explicit overflow-avoidance clamp
a few lines above the flagged operation, or is structurally bounded well
below `INT_MAX` (OS command-line length limits, a fixed join-size
constant).

## EXP39-C: real bugs found, including a genuine copy-paste bug

3 of 38 findings are TP:
- **hostap `tls_cert_chain_failure_event`** (`tlsv1_client_read.c:343`) —
  a genuine **copy-paste bug**: the function writes
  `ev.peer_cert.subject` (from a different event-construction function)
  into the middle of building a `TLS_CERT_CHAIN_FAILURE` event, which the
  receiver interprets as the `cert_fail` member.
- **hostap `radius_server.c` (2 TP)** — the standard BSD-sockets
  `recvfrom(&ss)` → `sin`/`sin6` idiom, safe in practice via the
  `data->ipv6` discriminant but genuinely punning past the guaranteed
  common-prefix, surfaced per the letter of the rule.

**raylib's 31 FPs are a distinct engine bug, not a union-punning
applicability call.** The finding traces to `GetPixelColor()`
(`src/rtextures.c:5228-5303`), which legitimately casts `const void
*srcPtr` to `unsigned short*`/`float*` in mutually-exclusive
`switch(format)` branches — no union anywhere in this function. Traced
via clew directly to `src/rules/cert_c/EXP/EXP39-C/exp39_c.rs:716-781`:
`infer_source_type`/`infer_type_from_name` don't resolve function-parameter
types, so unmatched identifiers silently default to `"int"`, producing a
bogus `"'int' object accessed through incompatible pointer"` message.

## Follow-up

Filed:
- **task 568**: MSC21-C fires on non-arithmetic loop shapes (pointer
  traversal, function-return-driven conditions) that have no numeric step
  at all — 100% FP across 5 projects, 41/41 findings. The fix should
  restrict MSC21-C to loops whose termination test genuinely involves a
  constant/provably-fixed-sign numeric step, excluding pointer-typed
  induction variables and non-numeric loop conditions entirely.
- **task 569**: EXP39-C's `infer_source_type`/`infer_type_from_name`
  (`src/rules/cert_c/EXP/EXP39-C/exp39_c.rs:716-781`) don't resolve
  function-parameter types and silently default unmatched identifiers to
  `"int"`, producing bogus incompatible-pointer-cast messages — 31/31 of
  raylib's findings here. Precisely located during this adjudication pass
  (via clew), should be a fast, high-confidence fix.
- Noted but not filed separately (broad, multi-cause, lower confidence of
  one clean fix): CON07-C's const-blindness/single-thread-context
  recognition and API00-C's cross-file-contract blindness — both would
  need real cross-file ProjectContext work rather than a narrow patch.

CSVs: `data/precision_audit/{curl,hostap,sqlite,lua,mosquitto,raylib}/import_delta_batchC_task546.csv`.

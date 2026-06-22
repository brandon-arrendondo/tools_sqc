# Lua 5.5 ground-truth audit — authoritative file-by-file oracle

**Fifth** codebase in the audited-corpus oracle (after libcrc, sqlite, mosquitto,
curl). Unlike a pure tool-grading pass, this is an **authoritative, tool-independent
review of the Lua source** — a Juliet-style ground truth of which CERT-C
violations genuinely exist per file, scoreable for **precision AND recall** by
any analyzer.

- Pinned subject: `~/toolchain/lua` @ `40b76de2d77e66b70a9d4bf989c3f5340919973f`
  (`git describe`: `v5.5.0-34-g40b76de2`, 2026-06-03). Upstream HEAD is the same
  commit — no drift; `~/data-enterprise/lua-main` cross-ref is identical.
- sqc: v0.4.59, manifest `conf/realworld/lua-rules.toml` (same 14-rule advisory
  baseline as sqlite).

## Scope: 60 in-scope files (33 .c + 27 .h), 31,470 LOC

Flat root source = the shipping interpreter + stdlib + `lua.c` CLI. **Excluded**
(matches the benchmark `--exclude`): `onelua.c` (amalgamation, double-counts
under preprocessing tools), `ltests.c` **and `ltests.h`** (internal test/debug
harness, only built with `-DLUA_USER_H='"ltests.h"'`), `testes/` (C fixtures),
`manual/`.

> NOTE: the live benchmark `--exclude` currently drops `ltests.c` but not
> `ltests.h` (23 findings leaked into the v0.4.59 scan). The oracle corpus
> removes them; the benchmark config should add `**/ltests.h` for scoring-scope
> parity. (follow-up)

`lualib.h` is the one in-scope file with **zero** sqc findings — still read in
full for FNs.

## Corpus
- `lua_inscope_0.4.59.json` — 3309 sqc findings (3332 raw − 23 `ltests.h`),
  the reconciliation checklist.
- `batches/b01..b16.json` — file-based batches, bin-packed ~1.3–2.4K LOC each,
  every file carries its sqc findings (empty list = read-for-FN-only).
- `AGENT_INSTRUCTIONS.md` — authoritative-review protocol (ground-truth first,
  sqc as checklist).
- `categorical_patterns.md` — Lua FP traps (longjmp errors, `luaM_*` raise-on-OOM,
  macro-heavy headers, internal cross-TU API, portability-not-structural C99).
- `results/<batch>.result.json` — per-batch output (`ground_truth` / `labels` /
  `fns`).
- `adversarial/` — re-verification pass (FN-refute, TP-refute, FP-hunt).

## Top sqc rules (in-scope, 3309 findings)
    API00-C 511  DCL02-C 206  PRE00-C 190  PRE12-C 183  DCL15-C 149
    MSC37-C 136  DCL13-C 127  STR34-C 124  MSC04-C 123  PRE01-C 107
    EXP33-C 106  DCL40-C 95   EXP34-C 89   DCL00-C 80   MEM30-C 77   API05-C 69

## Recall caveat (FN coverage)
The first pass + the 3 adversarial agents (FN-refute / TP-refute / FP-hunt) all
counter **over**-claiming. None hunts for **missed** FNs, so a near-empty first
pass yields a possibly-falsely-perfect recall. Through b06 we have **0 FN across
1303 findings**. Decision (2026-06-22): continue the economical Sonnet first pass
as-is and accept low true-FN as plausible — this is a fresh post-fuzz / post-AI-
review commit, and most TPs/FPs self-correct when we revisit *why* sqc fires. If
recall turns out to matter, run a **stricter dedicated FN-hunt** (Opus, aggressive
find-new-FN mandate) over the untrusted-input/VM files — `lundump.c` (bytecode
loader, came back clean in b03; re-examine the untrusted-bytecode scope call),
`lvm.c`, `lcode.c`, `lstrlib.c`, `lapi.c`, `ltable.c`, `lgc.c` — before trusting
the recall number.

## First-pass result (complete)
All 16 batches reviewed. **3309 labels (== corpus), 0 TP / 3309 FP / 2 FN.**
sqc precision on Lua = **0/3309 = 0.0%**; every enabled-rule finding is a false
positive. Two genuine low-confidence violations sqc missed (the recall side):
- `lua.c:42` **SIG31-C** (med) — `globalL` non-volatile, modified by main thread
  and read in signal handler `laction`; sqc fired `CON03-C` (wrong rule) on the
  line → labeled FP, defect logged as FN.
- `lapi.c:1475` **EXP40-C** (low) — `lua_upvaluejoin`→`getupvalref` returns
  `(UpVal**)&nullup` (const cast away) for invalid `n`; `api_check` is a no-op in
  release, so `*up1=*up2` writes through a pointer to a `static const` (UB).

Both are documented-precondition / asserted-only gaps, not memory-corruption
bugs — consistent with Lua's maturity. No high-confidence TP or FN in the corpus.

### CERT-bar ≠ upstream-bug (do not re-litigate)
The two FNs are valid oracle entries (they grade sqc's **recall**), but **neither
is a reportable Lua bug, and neither should be filed upstream.** The oracle grades
against a strict CERT-C analyzer's bar — "what a conforming-C pedant *should
flag*" — which is deliberately stricter than "what the Lua maintainers consider a
defect." These two are exactly where the bars diverge: genuine letter-of-the-
standard UB sitting in code that is correct by design.
- `lua.c` `globalL` (SIG31-C): the canonical "defer signal work to a VM hook"
  pattern in the **standalone driver** (not the embeddable library). `globalL` is
  set once in `pmain` (line 166) *before* the handler is installed (167), then
  only read; `laction` does the minimum and lets `lstop` raise later in normal
  context. Decades-old, knowingly accepted → won't-fix upstream.
- `lapi.c` `getupvalref` (EXP40-C): the `static const UpVal *const nullup` sentinel
  exists *precisely* so the debug-build `api_check` (1474) can safely deref `*up1`
  and abort on an invalid index. The const-cast write (1475) is reachable only in
  a release build **and** with a caller-supplied invalid upvalue index — a
  documented-precondition violation the Lua manual already declares undefined and
  CERT itself assigns to the caller. Intentional → won't-fix upstream.

Net: 3309 sqc findings + these 2 "missed" violations, and **zero of them is an
actual reportable Lua bug.** This is the corpus's strongest statement of how far
"CERT-C violation" sits from "real defect" on mature, fuzzed code — carry it into
the paper's precision discussion, not an upstream issue tracker.

## Status
- [x] First-pass file-by-file review (16 batches) — 0 TP / 3309 FP / 2 FN
- [x] Adversarial re-verification — FN-refute (Opus) on the 2 entries: **both
      CONFIRMED_FN, 0 refuted** (`adversarial/fn_verdicts.json`). 0 TPs ⇒ TP-refute
      and FP→TP hunt had no inputs.
- [ ] (conditional) Stricter Opus FN-hunt on bug-dense files if recall matters
- [x] Reconcile + import to `ground_truth` (commit-keyed `40b76de2`); `realworld-score`

## Measured result (run #64, v0.4.59)
`python -m bench realworld-score 64`:
- **Precision 0.0%** (0 TP / 2741 labeled findings)
- **Recall 0.0%** (0 of 2 known real bugs flagged — both standing FNs)
- Label coverage 2741 / 2763 run findings; the 22 unlabeled are `ltests.h`
  (out of oracle scope; benchmark `--exclude` follow-up still open).
- Ground-truth rows: 2742 FP + 2 FN (`source=precision_audit_0.4.59`;
  FP adjudicator `claude-sonnet-4.6`, FN adjudicator `claude-opus-4.8 (FN-refute)`).
- Import CSV: `lua_labels_0.4.59.csv` (file column basename-normalized — the
  scorer keys on the bare project-relative name; some first-pass result JSONs
  emitted full paths, which must be basename'd before import).
- One FP label has a line that drifted off the run's finding set (2742 FP labels
  vs 2741 matched) — negligible.

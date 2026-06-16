# mosquitto adversarial re-verification — sqc v0.4.30 audit labels

The mosquitto full audit (`README.md`) ran **without** the adversarial
verification node that the sqlite audit used. This pass supplies it: 8 parallel
adversarial reviewers re-examined the decision-critical labels against the
pinned source (`~/toolchain/mosquitto` @ `d3ee5c5c`), each prompted to *refute*
the existing verdict.

**Scope:** all 38 "RISKY" TPs (rules zero-TP on sqlite but TP on mosquitto —
the disable-decision blockers) + a per-rule sample (cap 10/rule, 131 total) of
the 20 "SAFE" rules' FPs (zero-TP on both projects — the disable candidates).
169 findings, 56 files.

## Result: 2 corrections, otherwise labels confirmed

### Correction 1 — EXP20-C was over-credited (17 TP → 8 confirmed)
EXP20-C ("do not rely on the boolean value of relational/equality operators").
The audit credited 17 TPs; adversarial review splits them:

- **Refuted (TP→FP), 9:** the `!strcmp()`/`!strncmp()` idiom on a *function
  return* — `plugin_acl_check.c:79,91`, `alias_mosq.c:137`, `control.c:207`,
  `mosquitto.c:161,424,427,430`, `send_publish.c:153`. EXP20-C concerns
  relational/equality *operators* (`a==b`, `a<b`), not the logical-NOT of a
  function call. Strong letter-of-the-rule argument that these are FP.
- **Confirmed TP, 8:** `proxy_v1.c:65,67,69` (`!memcmp` on **untrusted
  PROXY-protocol** input), `http_serv.c:176,177,198` (`!strncmp`),
  `topic_tok.c:103`, `websockets.c:554`. Reviewers split on the idiom but these
  survived adversarial scrutiny.

**Disposition:** EXP20-C still has 8 real TPs → stays RISKY (do **not** disable),
but its real-world precision contribution is weaker than the raw audit implied.

### Correction 2 — INT33-C is NOT cleanly safe (1 missed TP in the "SAFE" set)
`INT33-C` (div/mod by zero) was zero-TP on both projects → slated for hard
disable. Adversarial review flipped `src/bridge.c:930` **FP→TP** (medium
confidence): `(high - low)` used as a divisor with no guard that `high > low`.
A label-only disposition would have silently dropped this rule and this bug.

**Disposition:** move INT33-C from hard-disable to **raise-evidence / keep**,
pending a closer read of `bridge.c:930`.

### Everything else confirmed
- **All non-EXP20 RISKY TPs confirmed TP (high confidence):** MEM31-C
  (`net.c:464,502,508,515` — ssl_ctx leaks on TLS-setup error paths), MEM12-C
  (`http_client.c:67`, `http_api.c:100`, `mosquitto.c:608` — realloc/alloc
  leaks), CON03-C (`libmosquitto.c:42`, `logging.c:67/68/72`, `mosquitto.c:62`,
  `retain.c:31`, `xtreport.c:116` — unsynchronized static state), ENV01-C
  (`service.c:87,90`), INT00-C (`xtreport.c:56` — signed-long printed `%lu`),
  EXP37-C (`broker_internal.h:788`). These 6 rules **must stay enabled** — they
  catch real bugs on a less-hardened codebase.
- **All sampled SAFE-set FPs confirmed FP (high confidence)** except INT33-C
  above. Root causes match the audit: `DL_FOREACH_SAFE`/`HASH_*` macro opacity
  (EXP33), `sizeof(struct.field)` misread (ARR01/ARR36/ARR39), `!`-idiom and
  `const char*` literals (STR30/STR34), `PRIu64`/size-arg format confusion
  (FIO47/PRE32), function-pointer params (DCL31), no-restrict prototypes
  (EXP43), `#ifdef`-branch return merging (MSC37), domain "open"/"tmp" name
  matches (FIO42/FIO50).

## Net disposition for task 170 (cross-validated + adversarially verified)

The group-A list below is **zero-TP + adversarially-confirmed FP** on the audited
oracles. These are general C safety rules (uninitialized memory, format strings,
sizeof-of-pointer, etc.) that DO apply to these codebases — their problem is
precision, not applicability.

- **High-FP / low-signal (19 rules):** EXP33, API02, EXP43, STR34, MSC37, FIO47,
  ARR01, DCL31, PRE32, STR30, FLP03, ARR36, FLP34, ARR39, FIO42, FIO50, EXP07,
  WIN04, EXP30 — zero TP on both projects, confirmed FP under adversarial review.
- **Raise-evidence / keep (8 rules):** EXP20, CON03, MEM31, INT00, MEM12, ENV01,
  EXP37 (real TPs on mosquitto) **+ INT33** (adversarial pass surfaced one TP).

### Disposition is RULE HARDENING, not config disable (corrected 2026-06-16)

**An earlier pass disabled the 19 group-A rules in the `conf/realworld/*.toml`
files. That was reverted.** Those configs exist only to drop rules *categorically
inapplicable* to a project (e.g. Windows-only rules on a POSIX target, advisory
style the project rejects) — not to suppress false positives. Per-finding FPs
among enabled rules are recorded in the `ground_truth` oracle (`data/benchmarks.db`)
so that **precision is measured, not hidden**: the benchmark's job is to show
recall and precision improving over time, and silencing a noisy-but-applicable
rule in the config would make that benchmark lie. None of the 19 group-A rules is
platform/domain-inapplicable (EXP33 = uninitialized memory applies to all C; even
WIN04 sees in-scope `src/os_win.c` on sqlite), so none belongs in the config
disable set.

**The libcrc third oracle proves why disabling is wrong even for "zero-TP" rules.**
All three real-world audits are exhaustive per-finding (the "cap 10/rule" sampling
above was only the second-pass adversarial *re-*verification, not the primary
audit). Group A was zero-TP across sqlite + mosquitto, yet **EXP33-C has a real TP
on libcrc (1 TP / 9 FP)** — a genuine uninitialized read, buried in macro-opacity
noise. On an *unaudited* codebase we have no basis to call any applicable rule pure
noise; the missed bug may be the needle in exactly that haystack. So the
generalizable fixes are (1) lower false negatives and (2) harden rules so the TP
isn't drowned — both measured by the benchmark, neither achieved by config disable.

**Where the group-A noise actually gets fixed (existing backlog):**
- EXP33 / EXP34 macro-opacity FP (the libcrc 9-FP root cause) → **task 180**
  (utlist/uthash external function-like macro opacity); also tasks 118, 146.
- Free-state precision (MEM30/MEM12) → **task 181**. EXP34 caller-contract FP →
  **task 175**.
- False-negative direction (taint untrusted-length → unguarded access, re-point
  ARR30) → **task 172**.

**Group-B raise-evidence (task 182):**
- EXP20-C: ~half its mosquitto credits were the `!strcmp`/`!strncmp` idiom (FP) —
  needs an evidence gate, not a disable.
- INT33-C: re-read `src/bridge.c:930` `(high - low)` divisor; confirm the missed
  div-by-zero TP and add a guarded-divisor check.

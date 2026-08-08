# hostap ground-truth audit (file-at-a-time) — sqc v0.4.169, codebase commit dcee60436

hostap is the **last holdout** codebase for the file-at-a-time audited-corpus
model (after libcrc, sqlite, mosquitto, curl). `~/toolchain/hostap` is pinned
to commit `dcee60436390dd34731560657c4257c3b4c839a6` (the commit referenced by
`conf/realworld/hostap-rules.toml`).

## Scope (task 159)

Per task 159: **`src/` + `wpa_supplicant/` + `hostapd/`** — the shipped
hostapd (AP) and wpa_supplicant (station) daemons and their shared library.
Excludes `tests/`, `wlantest/` (separate test/monitoring tool), `eap_example/`,
`hs20/`, `radius_example/`, `wpaspy/` — none of these ship as part of either
daemon.

| Scope | Files |
|-------|-------|
| Whole-repo .c/.h | 805 |
| − tests/wlantest/eap_example/hs20/radius_example/wpaspy | −69 |
| **In-scope (src+wpa_supplicant+hostapd)** | **736** |
| └ with ≥1 finding | 636 |
| └ 0 findings (FN-read only) | 100 |

Note: the standing realworld-benchmark entry (`mcp_servers/realworld_server.py`
`hostap` config) currently scans the **whole repo root** with no `--exclude`,
so its dashboard numbers include the 69 out-of-scope files' ~2,587 findings.
Unlike curl's WIN_MAC split (a build-configuration boundary), hostap's
out-of-scope directories are genuinely non-shipped tooling — the realworld
benchmark scan config should probably gain an `--exclude` for `tests/**` and
`wlantest/**` etc. to match this oracle's scoring denominator (follow-up, not
done as part of this audit yet).

## Method (mirrors curl/mosquitto/sqlite)

Binary: sqc v0.4.169 (commit 22c40f2c), fresh run
`sqc-hostap-0.4.169-22c40f2c` via the realworld-benchmark MCP server
(`run_analysis(codebase="hostap", tool="sqc")`), matching the benchmark's own
invocation (manifest `conf/realworld/hostap-rules.toml`, `-d src -d
wpa_supplicant`, `-I /usr/include -I /usr/include/libnl3 -I
/usr/include/dbus-1.0`).

Whole-repo run: 38,659 violations (23 suppressed). Filtered to in-scope
(`src/`, `wpa_supplicant/`, `hostapd/`): **36,072 findings across 636 files**,
174 distinct rules firing.

## Top rules (in-scope, 36,072 findings / 636 files)

    DCL13-C 5320   API00-C 3301   API05-C 1593   EXP33-C 1527   DCL30-C 1272
    EXP30-C 1216   ERR34-C  959   ERR07-C  913   MEM31-C  890   MEM30-C  749
    MSC01-C  744   DCL31-C  717   EXP34-C  666   MSC13-C  663   STR34-C  659

(API05-C was ~100% FP / advisory-never-adopted on all four prior oracles —
worth an early check on whether that generalizes here before spending batch
budget on it.)

## Buckets (in-scope) — batches generated, NOT yet adjudicated

| Bucket | Files | Findings | Batches (~110-150 findings each) |
|--------|-------|----------|-----------------------------------|
| 1-5   | 146 | 378    | 4   |
| 6-15  | 132 | 1,261  | 11  |
| 16-40 | 137 | 3,692  | 34  |
| 41+   | 221 | 30,741 | 288 |
| **Total** | **636** | **36,072** | **337** |

For comparison, curl (the previous-largest oracle) was 427 in-scope files /
11,199 findings / 89 batches. hostap is **~3.2x curl's finding volume** and
**~3.8x its batch count** — the 41+ bucket alone (driven by huge files like
`wpa_supplicant/ctrl_iface.c` at 1,551 findings and `src/drivers/driver_nl80211.c`
at 1,052) accounts for 85% of all findings. Files exceeding the 150-finding
per-batch cap are split into `<file>#partNofM` chunks within a batch (see
`AGENT_INSTRUCTIONS.md`) rather than each becoming one oversized batch.

Batches live in `batches/*.json`; adjudication results land in
`results/*.result.json`; running pattern notes accumulate in
`categorical_patterns.md`.

## Adversarial re-verification (planned, after main sweep)

Per curl/mosquitto precedent (`../mosquitto/adversarial_verification.md`,
`../curl/adversarial/`), the main sweep's labels are NOT the final word —
parallel adversarial reviewers, each prompted to *refute* the existing
verdict, re-examine the decision-critical subset once the main sweep is
done:
- Every TP on a rule that's zero-TP (or near-zero) elsewhere in the oracle
  corpus (the "RISKY" set — these are the labels a disable/keep decision
  would hinge on).
- A capped per-rule sample (mosquitto used 10/rule) of the FPs on rules
  that came back zero-TP here (the "SAFE" set — confirming they're really
  safe to treat as noise, not just under-sampled).
Historically this has flipped a small number of verdicts each time (mosquitto:
2 corrections out of 169 re-checked) rather than upending the sweep, but it's
cheap insurance against an over-credited TP or a missed one hiding in a
large "confirmed FP" rule bucket. Do not skip it just because the main sweep
looks clean.

## Status

**Not started.** Scaffolding only (this README, buckets, batches,
`AGENT_INSTRUCTIONS.md`, `categorical_patterns.md` placeholder) as of
2026-07-28. Given the batch count (337, vs. curl's 89), this needs deliberate
multi-session pacing — see task 159 for the live batch-count checkpoint.
</content>

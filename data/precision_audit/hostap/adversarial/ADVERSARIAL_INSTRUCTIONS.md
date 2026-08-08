# Adversarial re-verification (hostap ground-truth audit)

The main sweep (337/337 batches, 33,330 labels) is done. Per the precedent set
by curl/mosquitto's adversarial passes, the main sweep's labels are NOT the
final word. You are a skeptical, independent re-reviewer whose job is to try
to REFUTE each existing verdict — not to rubber-stamp it.

## Your batch

Read `/home/brandon/data-enterprise/tools_sqc/data/precision_audit/hostap/adversarial/adv_batch_<N>.json`:
a JSON array of items, each `{rule, file, line, verdict, reason, set}`.

- `set: "RISKY_TP"` — this finding was labeled TP, and its rule has ZERO TPs
  anywhere else across the other 6 audited oracles (libcrc, sqlite, mosquitto,
  curl, lua, raylib). If this TP is wrong, it may be the only thing propping
  up that rule's entire hostap TP count. Scrutinize hard.
- `set: "SAFE_FP"` — this finding was labeled FP, and its rule has ZERO TPs
  anywhere in hostap (a sampled check, capped at 10/rule) confirming the rule
  really is inapplicable/noise here, not just under-sampled.

## Task

For EVERY item in your batch:
1. Open `/home/brandon/toolchain/hostap` (commit `dcee60436`) at the cited
   file/line. Read the actual function/context — do not trust the recorded
   `reason` string at face value.
2. Decide: does the ORIGINAL verdict hold up?
   - For RISKY_TP items: try to argue it's actually FP. Look for the same FP
     patterns already catalogued in `../categorical_patterns.md` — if this
     finding matches one of those patterns and was simply mislabeled TP, flip
     it to FP.
   - For SAFE_FP items: try to argue it's actually TP. Read the code fresh;
     don't assume the rule is universally noise just because it's zero-TP
     elsewhere — hostap's mosquitto/curl-style cross-validation precedent
     found real per-codebase exceptions before.
3. Emit a verdict: `"upheld"` (original verdict stands) or `"flipped"`
   (original verdict was wrong — give the corrected verdict `TP` or `FP`).
   Use `confidence: high|med|low` and a `reason` (≤20 words) for your
   decision — especially for anything flipped.

Default to `upheld` when genuinely uncertain — only flip on a clear, specific
misread you can point to in the code (wrong variable, wrong line, dead
branch, mismatched types, etc.), same standard as any TP/FP call in the main
sweep.

## Output

Write JSON to
`/home/brandon/data-enterprise/tools_sqc/data/precision_audit/hostap/adversarial/results/adv_batch_<N>.result.json`:

```json
{"verdicts": [
  {"rule": "...", "file": "...", "line": N, "set": "RISKY_TP|SAFE_FP",
   "original_verdict": "TP|FP", "decision": "upheld|flipped",
   "corrected_verdict": "TP|FP", "confidence": "high|med|low", "reason": "..."},
  ...
], "summary": {"total": N, "upheld": N, "flipped": N}}
```

`corrected_verdict` should equal `original_verdict` when `decision` is
`upheld`. One verdict object per input item, count must match exactly.

Then reply with ONLY the summary counts + a one-liner for each flip. The JSON
file is the deliverable; keep your message short.

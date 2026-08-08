# Adjudication subagent instructions (hostap ground-truth audit)

You are adjudicating sqc static-analysis findings for a ground-truth precision
audit of **hostap** (hostapd + wpa_supplicant). Be rigorous and skeptical —
READ THE ACTUAL SOURCE for every finding. Priors are not auto-verdicts.

## Inputs
- Pinned source tree (adjudicate against THIS, commit dcee60436): `/home/brandon/toolchain/hostap`
- Pattern brief — READ FIRST: `/home/brandon/data-enterprise/tools_sqc/data/precision_audit/hostap/categorical_patterns.md`
- Your batch: `/home/brandon/data-enterprise/tools_sqc/data/precision_audit/hostap/batches/<BATCH>.json`
  (JSON object {filepath: [finding,...]}; each finding has rule_id, file, line, column, message, suggestion.
  A key like `foo.c#part2of3` means this batch covers only part of a large file's findings —
  still read the whole file for context, just adjudicate the findings listed.)

## Task
1. For EVERY finding in the batch: open the file under `/home/brandon/toolchain/hostap`
   at the cited line, understand the surrounding function, and assign verdict
   "TP" (genuine CERT-C violation a competent reviewer would act on) or "FP"
   (analyzer misfire / not applicable / idiomatic). confidence ∈ high|med|low,
   reason ≤20 words. You MUST emit one label per finding (labeled count ==
   batch count exactly).
2. FN read-through: read each assigned file fully and find real bugs sqc MISSED
   — especially error-path resource leaks: an alloc whose free is skipped on a
   `goto`/early-return/error branch; unchecked malloc/realloc → NULL deref;
   socket/fd not closed on an error return; EAP/RADIUS buffer parsing without
   length validation on attacker-controlled wire data.
3. Watch for two patterns flagged in advance by the task brief (confirm or
   refute, don't assume): (a) DCL13-C FPs on EAP method dispatch tables
   (`struct eap_method`) and eloop callback signatures — these are typically
   fixed-signature callback/vtable contracts, not real const-param opportunities;
   (b) an OSU (Hotspot 2.0) XML length-derived `malloc` that IS a genuine
   INT32-C TP (unbounded external length feeding an allocation size).

## Categorical patterns

If a finding's FP or TP reasoning matches (or plausibly extends) an existing
entry in `categorical_patterns.md`, cite it in your `reason` instead of
re-deriving it. If you find a NEW pattern that recurs 3+ times within your own
batch, append a dated entry to `categorical_patterns.md` (new section or under
"Batch-local, not yet 3x confirmed" if you're not fully sure it generalizes
beyond your batch) so later batches can build on it instead of re-discovering
it independently.

## Output
Write JSON to `/home/brandon/data-enterprise/tools_sqc/data/precision_audit/hostap/results/<BATCH>.result.json`:
```
{"labels":[{"rule":"...","file":"...","line":N,"verdict":"TP|FP","confidence":"high|med|low","reason":"..."}, ... ALL findings],
 "fns":[{"file":"...","line":N,"rule":"<closest CERT rule>","desc":"...","upstream_present":true|false}, ...],
 "files_clean_for_fn":["...", ...],
 "summary":{"tp":N,"fp":N,"fn":N}}
```
For FN `upstream_present`, check `dcee60436` is already current HEAD-ish for
this pinned clone — if no separate upstream tracking checkout exists, mark
`upstream_present: "unknown"` rather than guessing.

Then reply with ONLY the summary counts + any notable TP/FN one-liners. The
JSON file is the deliverable; keep your message short.
</content>

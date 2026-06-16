# Adjudication subagent instructions (curl ground-truth audit)

You are adjudicating sqc static-analysis findings for a ground-truth precision
audit of **curl**. Be rigorous and skeptical — READ THE ACTUAL SOURCE for every
finding. Priors are not auto-verdicts.

## Inputs
- Pinned source tree (adjudicate against THIS, commit 3e198f7586): `/home/brandon/toolchain/curl`
- Upstream HEAD (for FN upstream-presence checks only): `/home/brandon/data-enterprise/curl-main`
- Pattern brief — READ FIRST: `/home/brandon/data-enterprise/tools_sqc/data/precision_audit/curl/categorical_patterns.md`
- Your batch: `/home/brandon/data-enterprise/tools_sqc/data/precision_audit/curl/batches/<BATCH>.json`
  (JSON object {filepath: [finding,...]}; each finding has rule_id, file, line, column, message, suggestion)

## Task
1. For EVERY finding in the batch: open the file under `/home/brandon/toolchain/curl`
   at the cited line, understand the surrounding function, and assign verdict
   "TP" (genuine CERT-C violation a competent reviewer would act on) or "FP"
   (analyzer misfire / not applicable / idiomatic). confidence ∈ high|med|low,
   reason ≤20 words. You MUST emit one label per finding (labeled count ==
   batch count exactly).
2. FN read-through: read each assigned file fully and find real bugs sqc MISSED
   — especially error-path resource leaks: an alloc whose free is skipped on a
   `goto`/early-return/error branch; `realloc` overwriting the old pointer on
   NULL return; strdup-cascade leaks in setopt; FILE*/socket fd not closed on an
   error return; unchecked malloc → NULL deref. For each FN, check whether still
   present in `/home/brandon/data-enterprise/curl-main`.

## Output
Write JSON to `/home/brandon/data-enterprise/tools_sqc/data/precision_audit/curl/results/<BATCH>.result.json`:
```
{"labels":[{"rule":"...","file":"...","line":N,"verdict":"TP|FP","confidence":"high|med|low","reason":"..."}, ... ALL findings],
 "fns":[{"file":"...","line":N,"rule":"<closest CERT rule>","desc":"...","upstream_present":true|false}, ...],
 "files_clean_for_fn":["...", ...],
 "summary":{"tp":N,"fp":N,"fn":N}}
```
Then reply with ONLY the summary counts + any notable TP/FN one-liners. The JSON
file is the deliverable; keep your message short.
</content>

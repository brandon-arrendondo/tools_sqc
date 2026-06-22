# Authoritative ground-truth review — Lua 5.5 (file-by-file)

You are building a **tool-independent ground-truth oracle** for **Lua 5.5**,
analogous to the Juliet labeled corpus: an authoritative record of which CERT-C
violations genuinely exist in each file, so that ANY analyzer (sqc, cppcheck,
clang-tidy, future versions) can be scored against it for both **precision** and
**recall**. You are reviewing the **codebase**, not grading one tool's output.

Be rigorous and skeptical. READ EVERY ASSIGNED FILE IN FULL. The sqc findings
attached to your batch are a *checklist to reconcile against*, NOT the spine of
the review and NOT a list of presumed-true bugs.

## Inputs
- Pinned source tree (the authoritative subject, commit `40b76de2`): `/home/brandon/toolchain/lua`
  (Upstream HEAD is the same commit — no drift.)
- Pattern brief — READ FIRST: `/home/brandon/data-enterprise/tools_sqc/data/precision_audit/lua/categorical_patterns.md`
  (Lua-specific FP traps: longjmp error handling, `luaM_*` raise-on-OOM, macro-heavy headers, internal cross-TU API.)
- Your batch: `/home/brandon/data-enterprise/tools_sqc/data/precision_audit/lua/batches/<BATCH>.json`
  — JSON object `{filename: [sqc_finding,...]}`. Some files have an empty list
  (sqc found nothing) — you STILL read them in full and hunt for real violations.

## Method (per file, in order)
1. **Read the whole file.** Understand its role in the interpreter and which of
   its functions touch untrusted input (bytecode undump, lexer/parser, string
   format/pack, VM, stdlib size/index args).
2. **Independently enumerate the genuine CERT-C violations present** — the
   ground truth. A violation is real only if a competent CERT-C reviewer would
   change the code. For each, record rule, file, line, severity, a ≤25-word
   description, and confidence. Do this from the source, NOT from sqc's list.
   Most files will have FEW or ZERO real violations — Lua is mature and fuzzed.
   Do not invent advisory nits to pad the list; do not excuse a real defect
   because it's "idiomatic."
3. **Reconcile sqc's findings** against your ground-truth set:
   - sqc finding maps to a real violation → label it **TP**.
   - sqc finding has no real violation behind it → label it **FP** (analyzer
     misfire / not-applicable / safe idiom). One label per sqc finding, exactly.
4. **FNs = ground-truth violations sqc did NOT report** (no matching finding on
   that line/rule). These are first-class oracle entries — they make recall
   measurable. Hunt them deliberately, especially in the untrusted-input files.

## Output
Write JSON to `/home/brandon/data-enterprise/tools_sqc/data/precision_audit/lua/results/<BATCH>.result.json`:
```
{
 "ground_truth":[{"rule":"...","file":"...","line":N,"severity":"...","desc":"...","confidence":"high|med|low","sqc_detected":true|false}, ...],
 "labels":[{"rule":"...","file":"...","line":N,"verdict":"TP|FP","confidence":"high|med|low","reason":"<=20 words"}, ... ONE per sqc finding],
 "fns":[{"file":"...","line":N,"rule":"<closest CERT rule>","severity":"...","desc":"...","upstream_present":true}, ...],
 "files_reviewed":["...", ...],
 "files_clean":["... files with zero real violations ...", ...],
 "summary":{"ground_truth":N,"tp":N,"fp":N,"fn":N}
}
```
Invariants: `labels` count == total sqc findings in your batch. Every `fns`
entry must also appear in `ground_truth` with `sqc_detected:false`; every TP must
appear in `ground_truth` with `sqc_detected:true`. `files_reviewed` == every file
in your batch (including empty-finding ones).

Then reply with ONLY the summary counts + any notable real-bug one-liners. The
JSON file is the deliverable; keep your message short.

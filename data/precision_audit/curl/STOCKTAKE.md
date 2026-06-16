# What the curl audit shows — stock-take (2026-06-16)

curl is the 4th and largest precision oracle (427 in-scope files, 11,200
findings). Final: **101 TP / 11,099 FP = ~0.9% precision, 5 FNs** (post-adversarial,
audit-score run #46). Lowest precision of the four codebases. Three takeaways.

## 1. On the most-fuzzed C codebase, the enabled rules fire ~entirely on advisory items

The 101 TPs are almost all advisory/low-confidence: DCL13-C const-param (66),
MSC04-C recursion, ERR33-C unchecked return, DCL38-C struct-hack. **Zero
high-severity memory-safety TPs.** sqc found no real memory-safety bug in curl;
the 5 FNs are sqc *misses* and all minor (error-path leaks / a Windows edge /
an unchecked CLI fwrite). This is the expected ceiling for a heavily-fuzzed
target and is the empirical anchor for the Juliet-vs-real-world precision hedge
(cf. [[realworld-oracle-paper-context]]).

## 2. Macro opacity is the single biggest *mechanically-fixable* FP lever — Phase 2c target list

Phase 2c of the macro-expansion engine (task 185) was deliberately deferred to
this audit to choose expansion targets from real FP data. Here it is. **~1,460
of 11,099 FP (~13%)** are macro-related, but they split into two mechanisms with
different fixes:

### Tier 1 — pure macro-opacity, expansion fixes outright (~600 FP)

| FP class | count | macro(s) | why expansion fixes it |
|---|---|---|---|
| DCL31-C "called without prior declaration" | **337** | `curlx_free/calloc/realloc/strdup/memdup` (and `Curl_c*` wrappers) | the macro hides a call to a real declared function; expand → declaration visible → FP gone |
| EXP33-C / EXP34-C / ARR00-C "used uninitialized" | **~280** | `CF_DATA_SAVE(save,…)`, dynbuf macros, uthash `HASH_FIND`/`HASH_ITER`, utlist `DL_FOREACH_SAFE`, `MY_SETOPT_*` do-while | the macro initializes the var / sets the out-param / guards the loop; sqc can't see it → "uninitialized" |

These are unambiguous wins: expand the macro, the finding disappears, no
dataflow change required. **Highest ROI for Phase 2c.**

### Tier 2 — macro-opacity is *part* of the cause; expansion helps but task 181 finishes it (~1,114 FP)

The MEM30/MEM31/MEM12 cluster (1,848 FP total; ~1,114 macro-adjacent, incl. 665
MEM30) is driven by **`Curl_safefree(x)` — a macro that frees *and* sets
`x=NULL`.** sqc sees the free but not the null, so it reports "use-after-free"
on the next access. Expanding `Curl_safefree` exposes the `=NULL`, which **feeds**
the free-state precision work (task 181: free→reassign / conditional-free /
struct-field-free / loop conflation). Expansion alone won't clear all 1,114 —
the free-then-reassign and struct-field-free conflations are genuine dataflow
gaps — but it removes the macro blindfold that currently makes them unanalyzable.

**Recommended Phase 2c order (data-driven):**
1. `curlx_*` alloc/free wrappers → clears 337 DCL31-C outright.
2. `Curl_safefree` → exposes free+null; pair with task 181 for the MEM30 cluster.
3. `CF_DATA_SAVE` + dynbuf/uthash/utlist iteration macros → clears ~280 EXP33/34/ARR00.

### Cross-codebase validation (not curl-overfit)

The mosquitto audit found the **same mechanism** with different macro names:
`mosquitto_FREE` (frees+nulls ↔ `Curl_safefree`), `DL_FOREACH_SAFE`/utlist,
`HASH_FIND`/uthash, `packet__alloc` success-implies-non-null, `SAFE_FREE`/
`SAFE_PRINT`. So the expander targets a cross-project pattern, confirming the
"fix the engine, not per-macro allowlists" thesis ([[macro-expansion-strategy]]).
hostap (task 159) should be checked to triangulate before locking the target set.

## 3. Non-macro FP levers (for completeness)

- **API05-C (1,062 FP, 100% FP)** — "conformant array syntax" advisory curl (and
  every audited project) never adopts. Not macro-related, not a dataflow bug —
  this is a rule that should be advisory-only / disabled-by-default in the
  realworld base, or its severity dropped. Single biggest non-macro FP source.
- **Free-state precision (task 181)** — the MEM30 cluster's other half; the
  highest-value dataflow work, amplified by Tier-2 macro expansion above.
- **API00-C (743), STR34-C (693)** — internal-handle non-NULL contract and
  `char**`/char sign-extension misparse; separate hardening, not macro-driven.

## Bottom line for the roadmap

Phase 2c now has a concrete, FP-ranked target set (Tier 1 `curlx_*` +
init-macros for an immediate ~600-FP drop; `Curl_safefree` to unblock task 181).
The macro-expansion phases (185 → 186 migrate ~51 rule files → 187 optional
preprocessor) are no longer blocked on "which macros?" — curl + mosquitto answer
it. Recall-gate every expansion on Juliet, measure the FP delta on the curl +
mosquitto oracles (now both ingested and scorable via `audit-score`).

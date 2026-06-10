# Real-world precision audit — sqc v0.4.22 (run sqc-0.4.22-1c94dc95, 2026-06-10)

Manual adjudication of a seeded random sample (seed 20260610, 50 findings per rule)
drawn from the four noisiest / paper-relevant rules in the v0.4.22 real-world
benchmark run (libcrc, mosquitto, curl, sqlite, hostap; 133,369 total findings).

Files:
- `sample_0.4.22.json` — the sampled findings (reproducible: candidates sorted by
  file/line/col/message, then `random.Random(20260610).sample(...)`).
- `adjudication_0.4.22.csv` — per-finding verdict (TP/FP) with one-line reason.

## Results

| Rule | Run count | Sample | TP | Precision |
|------|-----------|--------|----|-----------|
| MEM30-C (use after free) | 12,635 | 50 | 0 | **0%** |
| DCL13-C (const params) | 12,115 | 50 | 17 | **34%** |
| INT32-C (signed overflow) | 11,232 | 50 | 3 | **6%** |
| EXP34-C (null deref) | 5,495 | 50 | 1 | **2%** |
| **Combined** | 41,477 | 200 | 21 | **10.5%** |

Adjudication standard: a finding is TP if the diagnosed condition is genuinely
possible on some feasible path (for INT32-C: input not structurally bounded far
below the overflow threshold; for DCL13-C: the const suggestion is correct AND
implementable, i.e. the signature is not fixed by a callback/vtable/ABI contract
and the pointee is never written).

## Dominant FP root causes (by rule)

**MEM30-C (0/50):** No path sensitivity in the free→use matcher.
1. Free-then-immediately-reassign (`free(x); x = malloc(...); if (!x) ...`)
   flagged as UAF — ~1/3 of sample.
2. Freeing member A then accessing *sibling* member B of the same base struct
   (`free(p->a); use(p->b);`), including sequential cleanup-function free lists.
3. Non-pointer fields treated as freed pointers (sqlite `pOp->p1`/`p3` ints,
   scalar counters like `cred->num_domain`).
4. Frees inside `#if 0` blocks, other branches, or prior loop iterations.

**DCL13-C (17/50):** The 33 FPs are nearly all *fixed-signature* functions:
Tcl command procs, sqlite3_module/io_methods vtables, EAP method tables,
eloop/libmosquitto/nghttp2/CURL callbacks, plugin ABIs, and `#ifdef` stub/real
prototype pairs. Two parser artifacts (Tcl `CONST` macro flagged as a parameter
name). A "is this function's address taken / does it implement a typedef'd
function pointer type" check would remove most FPs.

**INT32-C (3/50):** Flags syntactic arithmetic with no value-range reasoning:
loop counters bounded by data structures (list lengths, argc, page sizes,
SQLITE_MAX_COLUMN), guarded subtractions (`i<n` ⇒ `n-i` safe), u8-promoted
arithmetic (`u8*256+u8`), already-widened `(i64)x+1`, and asserts. The 3 TPs
involve genuinely unbounded external input (hostap OSU XML length → `len*2+1`
malloc; fts5 int accumulation of multiple 1GB-bounded sizes; unbounded hex
parse `v<<4` loop).

**EXP34-C (1/50):** Flow exists but three blind spots dominate:
1. Flags the allocation line itself when the NULL check is on the *next* line
   (`p = zalloc(...); if (!p) return;`) — most common single pattern.
2. "Does not check for NULL" claims about callees that *do* check
   (`os_free`, `xml_node_get_text_free`, `nodeRelease`, `sqlite3_finalize`).
3. No understanding of rc-guard idioms (`rc==SQLITE_OK && step(pStmt)`),
   list-iterator non-nullness, or function-success contracts.
The 1 TP: zipfile.c `strlen(sqlite3_value_text(...))` without OOM-NULL check.

## Implications (feeds tasks 147/150)

- The paper objection ("8,657 EXP34-C findings on real code") is confirmed in
  kind: measured EXP34-C real-world precision is ~2%, and the three noisiest
  rules together are ≤ ~10% precision. Juliet-driven FP fixes did NOT
  generalize to real-world precision for these rules.
- Highest-leverage fixes by rule volume: MEM30-C free-then-reassign +
  sibling-member discrimination; EXP34-C alloc-line/next-line-check
  suppression; DCL13-C function-pointer-context suppression. Each targets the
  single largest FP class in a 12k-finding rule.

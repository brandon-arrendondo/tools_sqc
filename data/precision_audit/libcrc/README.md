# libcrc full ground-truth audit — sqc v0.4.24 (codebase commit 7719e21)

This is the first *complete* per-finding ground-truth audit of a real-world
codebase: **every** enabled-rule sqc finding on libcrc is labelled TP or FP,
not just a sampled subset. It is the real-world analog of Juliet's per-case
oracle, and the template for adjudicating the larger codebases incrementally.

libcrc is small enough (19 files, 2,335 LOC: a CRC computation library +
`precalc` codegen tool + demo CLI + tests) to read in full and label
exhaustively.

## Oracle polarity (Juliet OMITBAD / OMITGOOD mapping)

In Juliet, the `bad()` function (guarded by `#ifndef OMITBAD`) is code the tool
**should** flag — a true positive; the `good()` function (`#ifndef OMITGOOD`)
is code the tool should **not** flag — any report there is a false positive.
Mapped onto this oracle:

| Juliet            | ground_truth verdict | meaning                                   |
|-------------------|----------------------|-------------------------------------------|
| `good` / OMITGOOD | **FP**               | sqc reported it, but it is not a defect   |
| `bad` / OMITBAD   | **TP**               | a genuine issue sqc should report         |

So the false positives we record are the *good-code* (OMITGOOD) oracle, and the
genuine issues — whether or not sqc currently flags them — are the *bad-code*
(OMITBAD) oracle. The subset of TPs sqc fails to flag are **false negatives**
(recall gaps); see the last section.

## Method

```bash
# Scan with the project config + include/context flags (a fair CI invocation):
sqc /path/to/libcrc \
    -I  libcrc/include \
    -d  libcrc/src -d libcrc/include \
    --manifest conf/realworld/libcrc-rules.toml \
    --export   data/precision_audit/libcrc/libcrc_cfg_0.4.24.json

python3 data/precision_audit/libcrc/adjudicate.py          # -> adjudication CSV
python3 -m bench realworld-import-labels \
    data/precision_audit/libcrc/adjudication_libcrc_0.4.24.csv \
    --run sqc-0.4.22-1c94dc95 --source libcrc_full_audit_0.4.24 \
    --adjudicator claude --date 2026-06-11
python3 -m bench realworld-score sqc-0.4.22-1c94dc95        # measured prec/recall
```

**Invocation matters.** Scanning with no `-I` (so `checksum.h` cannot be
resolved) inflated the raw finding count from 422 to 542: DCL31-C alone went
0 -> 44 ("called without prior declaration", all artifacts of the missing
include path) and DCL15-C 1 -> 25. The audit uses the resolved invocation so we
judge the analyzer, not our flags.

**Config = categorical policy; oracle = per-finding truth.**
`conf/realworld/libcrc-rules.toml` (reused by the real-world MCP server for every
libcrc run) is based on the sibling `d_lib_common` embedded-C policy and only disables rules
that are *categorically* inapplicable to libcrc (Windows-only rules on a POSIX
target; `FIO50-C` separate-function FP; `POS05-C` chroot for a non-privileged
tool; advisory rules the project rejects). It does **not** suppress the noisy
analyzer-misfire rules (FIO47, PRE32, INT36, API00, MEM30, …): those stay
enabled so their false positives are *measured* in the oracle and motivate
fixing the analyzer, rather than being hidden by config.

## Result

| Metric | Value |
|--------|-------|
| Enabled-rule findings | **422** (38 rules) |
| True positives | **13** (8 rules) |
| False positives | **409** |
| Precision (raw) | **3.1 %** |

Low precision is expected and honest for a *clean, tiny* library scanned with
the full advisory ruleset: 422 findings on 2,335 LOC is almost entirely noise.
The 409 FPs fall into two buckets:

**1. Advisory-style noise (~225 FP)** — rules that are working as designed but
flag cosmetic, non-defect patterns intrinsic to CRC code:
EXP12-C (ignore `printf` return ×59), EXP19-C (braces ×59), DCL06-C (magic
`256`/`0xFF` ×41), EXP14-C (sub-int promotion ×25), DCL16-C (lowercase `ull`
suffix ×15), EXP20-C, INT14-C, INT17-C, DCL04-C.

**2. Analyzer misfires (~184 FP)** — genuine sqc bugs; these are the
actionable tool-improvement targets:

| Rule | FP | Root cause |
|------|----|-----------|
| FIO47-C | 18 | `<inttypes.h>` `PRIxNN` macros in a concatenated format literal are not expanded; `%"` read as an invalid specifier |
| INT36-C | 14 | `(uintNN_t) *ptr` (deref-then-widen) mistaken for a pointer-to-integer conversion |
| API00-C | 14 | parameter validated through an alias (`ptr = input_str; if (ptr != NULL)`) not recognized |
| PRE32-C | 13 | `PRIxNN` object-like macros flagged as `#if`/`#include` directives in macro args |
| INT31-C | 12 | `(unsigned char) ch` after `!=EOF` is lossless (ch ∈ 0..255); not a lossy narrowing |
| INT32-C | 19 | counters bounded far below INT_MAX (test-failure count, argv index, byte value) |
| EXP33-C | 9 | CRC tables read after their `if(!_init) init_tab()` lazy-init guard, not uninitialized |
| INT30-C | 7 | unsigned-shift wrap is well-defined and intended in the CRC algorithm |
| ARR30-C | 7 | masked index `& 0x00FF` into a 256-entry table; or NUL/sentinel-terminated scan |
| MEM30-C | 4 | write to a *file-scope static* table element misread as a stack-pointer escape |
| INT16-C | 3 | the masked operand is the unsigned `crc_tab_precalc[a]`, not the index `a` |

(Per-finding rationale for all 422 is in `adjudication_libcrc_0.4.24.csv`.)

## The 13 true positives (OMITBAD oracle)

| Rule | Location | Issue |
|------|----------|-------|
| FIO40-C | tstcrc.c:102 | `input_string` used in do_ascii/do_hex paths after `fgets()` failure (only `perror`, no reset) — indeterminate read |
| EXP33-C | tstcrc.c:110 | same uninitialized-read defect, caught independently |
| FIO20-C | tstcrc.c:102 | `fgets(MAX-1)` truncation never detected; long input silently truncated |
| CON03-C | crc16/crcccitt/crcdnp/crckrmit | lazy table init (init flag + table write) is an unsynchronized **data race** if `crc_*` is called from multiple threads — the known libcrc thread-safety issue (×4) |
| ERR00-C | precalc.c:167 | `fclose()` on a *written* stream unchecked — a flush failure silently loses the generated table |
| EXP12-C | precalc.c:167 | same unchecked write-`fclose`, caught independently |
| DCL15-C | crc64.c:103 | `update_crc_64` has external linkage but is undeclared in `checksum.h` and unused cross-file — should be static or prototyped |
| DCL00-C | crc8.c:46 | `sht75_crc_table` is written once, never modified — should be `const` |
| PRE06-C | precalc.h:1, testall.h:1 | header genuinely lacks an include guard (×2) |

## False negatives (recall oracle / things sqc should report but misses)

libcrc is well-written; sqc catches its two substantive defects (the lazy-init
race, the uninitialized read). One genuine defect is **missed**:

- **precalc.c:136** — `generate_table()` returns `0` (success) on `fopen`
  failure; `main()` does `exit(retval)`, so a table-file that cannot be opened
  is reported to the build as success. sqc has no rule for "returns a success
  constant on an error path," so this maps to no enabled rule. It is documented
  here rather than inserted as a TP label under a mismatched `rule_id` (which
  would distort per-rule recall). If a suitable detector is added later, add the
  TP label at that point so recall measures it.

## Measured against run #34 (v0.4.22, libcrc @ 7719e21)

`realworld-score sqc-0.4.22-1c94dc95` joins these labels to that run's findings:
overall 8.7 % precision (mixing libcrc with the earlier 4-rule sample) and
**97.1 % recall (33/34)** — the one missed known-TP is the EXP12-C
write-`fclose`, because run #34's benchmark manifest does not emit EXP12-C at
all (`Run# 0`). That, and the several FP-labeled rules also showing `Run# 0`,
illustrate the design: the oracle is a commit-pinned **superset** of labels, and
each run is scored only against the subset of rules it actually produced.

---

## File-at-a-time audited corpus (2026-06-11, sqc 0.4.24, run #37) — COMPLETE

libcrc is the **first codebase converted to the file-at-a-time audited-corpus
model** (`audited_files` table; `bench audit-score` / `audit-coverage`). The file
is the atomic unit of "done": every sqc finding in it is labelled **and** the file
was read independently for missed bugs (FNs). libcrc is now **19/19 files = 100%
coverage**.

### Config-correctness fix (important)

The earlier "Measured against run #34" numbers were scored against a run whose
findings were produced with the **base benchmark manifest, not
`conf/realworld/libcrc-rules.toml`** — because the running real-world MCP server
process (started 11:27) pre-dated the per-codebase-manifest plumbing (committed
13:38). It silently used `rules_templates/rules-benchmark.toml`, so runs #33/#34/#35
all emitted 277 findings *including* rules the project config disables (POS05-C,
API05-C, FIO50-C, WIN03-C, …) and *omitting* advisory rules it enables
(DCL06-C, EXP12-C, EXP19-C, …). This is exactly the "settle the per-codebase
scope first" principle: out-of-scope rules (Windows on POSIX, chroot on a
non-privileged tool, …) are FP **by design** and must never reach the oracle.

After restarting the MCP server, a config-correct scan (`--manifest
conf/realworld/libcrc-rules.toml`) reproduces the full audit's **422 findings**
(POS05/API05/FIO50/WIN03 = 0; DCL06=42, EXP12=60), ingested as **run #37**. All
422 findings collapse to the 372 distinct (file,line,rule) keys already labelled —
**0 unlabelled**. (A latent `ingest_realworld_run` bug that globbed `*.json` and
choked on the `.meta.json` sidecar was fixed at the same time.)

### Result (`bench audit-score`, run #37)

| Metric | Value |
|--------|-------|
| Files audited | **19 / 19 (100 %)** |
| Findings (config-correct) | **372** distinct |
| True positives | **13** |
| False positives | **359** |
| **Precision** | **3.5 %** |
| False negatives (recall denom) | **1** |
| **Recall** | **92.9 % (13/14)** |

Honest recall is now possible because each file was read for misses, not just
sqc's own output. The standout signal: **INT31-C precision 0/12, recall 0/1** —
sqc's INT31-C detector fires only on false positives (lossless `(unsigned char)
ch` casts) and *misses* the one real narrowing in the tree.

### False negatives — two kinds

1. **Detector-recall gap (recorded as an `FN` ground-truth row):**
   - `test/testcrc.c:104` **INT31-C** — `int len = strlen(...)` narrows `size_t`→`int`.
     sqc emits 12 INT31-C in libcrc (all FP) yet misses this real narrowing.
     Confidence medium; provenance uncorroborated. This sits in the INT31-C recall
     denominator and flips to "detected" if sqc's INT31-C detector improves.

2. **Coverage gaps (no enabled rule cleanly fits — documented, not inserted, so
   per-rule recall stays honest):**
   - `precalc/precalc.c:136` — `generate_table()` returns `0` (success) on `fopen`
     failure; `main()` does `exit(retval)`, so an unopenable table-file is reported
     to the build as success. No CERT rule cleanly targets "returns a success
     constant on an error path." Add a TP/FN label if a suitable detector lands.
   - Low-confidence candidates (documented, not inserted): `src/crc8.c:99` DCL40-C
     (`update_crc_8` param `unsigned char` vs `uint8_t` decl — benign unless
     `uint8_t` isn't `unsigned char`); `precalc/precalc.h:44` & `test/testall.h:37`
     (`main` prototyped in a shared header). The testcrc.c:163 "stray X" format
     observation is **already** flagged by sqc (FIO47-C), so it is not an FN.

This supersedes the run-#34 measurement above; run #34 remains as the record of
the config-incorrect baseline that motivated the fix.

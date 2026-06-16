# curl ground-truth audit (file-at-a-time) — sqc v0.4.35, codebase commit 3e198f7586

curl is the **fourth codebase** converted to the file-at-a-time audited-corpus
model (after libcrc, sqlite, mosquitto). `~/toolchain/curl` is pinned to commit
`3e198f75861cc2e12daf299689e145949dddd19b` (the commit referenced by
`conf/realworld/curl-rules.toml` and the v0.4.22 4-rule sample in
`adjudication_0.4.22.csv`). `~/data-enterprise/curl-main` tracks upstream
`master` for trunk-validation / upstream-PR confirmation (libcrc/mosquitto-style).

## Scope (task 158) + file reduction (sqlite lesson)

Per task 158: **`lib/` (libcurl) + `src/` (curl CLI)** — the shipped product.
Excludes `tests/`, `docs/`, `scripts/`, vendored/build tooling.

Applying the **sqlite scope lesson** (measure precision on what you ship *and
run* in the benchmark environment): the benchmark host is **Linux**, so files
that are `#ifdef _WIN32`/Apple-only — dead code on this host — are **out of
scope for the Linux oracle**. These are *not* deleted from consideration: they
represent a **distinct Windows/Apple build configuration** that should get its
own audit config later (todo 188; sibling task: bring in Windows open-source
benchmarks). Unlike sqlite's vendored/bindings exclusions, this is a
configuration boundary, not a "don't ship it" boundary.

**Excluded (14 WIN_MAC files, 259 findings):**

    lib/vtls/schannel.c   lib/vtls/schannel.h   lib/vtls/schannel_int.h
    lib/vtls/schannel_verify.c
    lib/vtls/apple.c      lib/vtls/apple.h
    lib/system_win32.c    lib/system_win32.h
    lib/curlx/winapi.c    lib/curlx/winapi.h
    lib/curlx/version_win32.c   lib/curlx/version_win32.h
    lib/curlx/multibyte.c       lib/curlx/multibyte.h

**Kept in scope** (sqlite kept its optional shipped extensions fts/rtree/session,
so curl keeps its cross-platform optional backends): all non-openssl TLS
(`vtls/{gtls,mbedtls,wolfssl,rustls}`), QUIC (`vquic/*`: ngtcp2, quiche), and
SSH (`vssh/*`: libssh, libssh2) backends — cross-platform source that compiles
on Linux.

> The same WIN_MAC exclusion must be applied to the **curl realworld benchmark
> scoring scope** so the dashboard/precision denominator matches this oracle
> (handled the sqlite way: unlabeled out-of-scope files are not scored).

| Scope | Files |
|-------|-------|
| `lib/` + `src/` total c+h | 441 |
| − WIN_MAC (Windows/Apple config) | −14 |
| **In-scope (Linux oracle)** | **427** |
| └ with ≥1 finding | 329 |
| └ 0 findings (FN-read only) | 98 |

## Method (mirrors mosquitto/sqlite/libcrc)

Binary: snapshot of `target/release/sqc` v0.4.35 → `/tmp/sqc-curl-audit-bin`
(isolated so a concurrent rebuild can't corrupt the run). Config-correct
invocation matches the realworld benchmark (`mcp_servers/realworld_server.py`
curl entry: `-I /usr/include -I lib`, manifest `conf/realworld/curl-rules.toml`):

```bash
# from ~/toolchain/curl
sqc lib -I /usr/include -I lib -d . \
  --manifest .../conf/realworld/curl-rules.toml \
  --export .../curl/curl_lib_0.4.35.json     # 9669 findings
sqc src -I /usr/include -I lib -d . \
  --manifest .../conf/realworld/curl-rules.toml \
  --export .../curl/curl_src_0.4.35.json     # 1789 findings
```

In-scope merged corpus: `curl_inscope_0.4.35.json` (11,199 findings, WIN_MAC
removed). Per-finding labels accumulate in `adjudication_curl.csv` (same format
as `adjudication_mosquitto.csv`); imported via `bench realworld-import-labels`
once benchmark DB is free. Adversarial re-verification pass per the mosquitto
model (`adversarial_verification.md`) — re-challenges claimed TPs/FNs and
samples FP buckets, because the adversarial agent consistently surfaces
over-credited TPs, missed FPs, and overlooked FNs.

## Top rules (in-scope, 11,199 findings / 329 files)

    MEM30-C ~1400  API05-C ~1080  DCL13-C ~1060  API00-C ~740  STR34-C ~700
    EXP34-C ~460   EXP33-C ~400   DCL31-C ~350   ARR00-C ~290  EXP20-C ~250

## Buckets (in-scope)

| Bucket | Files | Findings | Status | TP | FP |
|--------|-------|----------|--------|----|----|
| 0 findings (FN read) | 98 | 0 | pending | | |
| 1-5 | 115 | 283 | **done** | 3 | 280 |
| 6-15 | 60 | 566 | **done** | 20 | 546 |
| 16-40 | 74 | 2013 | **done** | 50 | 1963 |
| 41+ | 80 | 8337 | **done** | 67 | 8270 |
| **Total (findings buckets)** | **329** | **11199** | | **140** | **11059** |

**All 4 finding-buckets complete (2026-06-16)** via 89 parallel adjudication
subagents (~110-130 findings/batch), 9 waves. **Pre-adversarial precision:
140 / 11199 = 1.25%** (87 advisory FP/finding; consistent with the
mosquitto/sqlite low-precision real-world pattern). Per-finding labels in
`adjudication_curl.csv`; per-batch JSON in `results/`; missed bugs in
`fns_curl.csv`. Coverage validated 100% (4 ±1 agent discrepancies reconciled by
hand: 16-40_b16, 41+_b26, 41+_b58, + 1 harmless extra label in 16-40_b13).

**TP composition (140):** almost entirely *advisory*, low-confidence —
DCL13-C const-param on genuine static read-only helpers (69), PRE00/PRE12-C
macro multiple-evaluation (37), MSC04-C recursion (10), ERR33-C unchecked
`time()`/`fwrite` (6). Only 1 high-confidence TP (`tool_doswin.c` WIN01-C
`TerminateThread`). 111/140 are low-confidence. This confirms the audit thesis:
sqc's enabled rules on curl fire overwhelmingly on style/advisory items, not
memory-safety defects.

**High-FP rules:** MEM30-C (1437, ~99% FP — cleanup-ladder / free-then-reassign
/ struct-member-free conflation, task 181 root cause), API05-C (1062, ~100% FP,
C99-array-syntax advisory curl never adopts), DCL13-C (987 FP vs 69 TP),
API00-C (743, internal-handle non-NULL contract), STR34-C (693, `char**`/char
sign-extension misparse), EXP34-C (447), EXP33-C (398, macro-opacity +
out-param), DCL31-C (337, `curlx_*` macro wrappers).

First-pass found 18 FN candidates; the **adversarial pass refuted 12 and
downgraded 1** (see below). Net **5 confirmed FNs** (`fns_curl_final.csv`):
- `lib/vtls/vtls_spack.c:277` (MEM31, med) — `Curl_ssl_session_unpack` leaks the
  first memdup0 copy on a duplicate ALPN/TICKET/QUICTP tag in crafted session
  data (untrusted-decode path, upstream).
- `src/tool_cb_hdr.c:279` (ERR33, low) — `save_etag` leaves `fwrite`/`fputc`
  unchecked while checking `ftruncate`/`fseek` in the same function (upstream).
- `src/var.c:309` (MEM31, low) — `varexpand` null-byte path skips
  `curlx_dyn_free(&buf)` when `funcp` populated it (untrusted variable content, upstream).
- `src/tool_doswin.c:869` (FIO42, low, Windows-only) — `win32_stdin_read_thread`
  leaks `socket_r` when post-thread setup fails (`tdata` already NULLed, upstream).
- `src/tool_setopt.c:438` (EXP34, low) — `c_escape()` NULL on OOM passed to
  `%s` (UB); **fixed upstream**.

> NOTE: the first-pass "headline memory-safety FNs" (mbedtls.c:499/611 OOB,
> curl_ntlm_core.c:586 overflow, x509asn1.c:982 underflow) were **REFUTED** by
> the adversarial pass — `Curl_setblobopt` rejects zero-length blobs (mbedtls
> unreachable); `target_info_len` is a uint16 bounded by the type-2 parser check
> (ntlm can't wrap); x509asn1 wrap feeds only a log/snprintf (cosmetic,
> downgraded). This is exactly the over-eager-FN correction the adversarial pass
> exists to catch.

## Adversarial verification pass (2026-06-16) — applied

Three adversarial agents (`adversarial/`), each defaulting to refute:
1. **FN-refute** (18 candidates): 5 CONFIRMED, 12 REFUTED, 1 DOWNGRADE. Most
   "leaks" refuted because dynbuf self-frees on failure / meta-registered dtors
   free on OOM; the memory-safety claims refuted as unreachable/bounded.
2. **TP-refute** (140 claimed): 100 CONFIRMED, **40 FLIPPED to FP**. All 35
   PRE00/PRE12-C macro-multiple-eval flipped (harmless advisory — every call site
   uses side-effect-free args), plus 5 DCL13/ERR33/EXP20 where the first pass
   misread the param or the guard. Flips applied to `adjudication_curl.csv`
   (confidence=`adversarial-flip`).
3. **FP-hunt** (176 Critical/High memory-safety FPs in decode files): **0
   overturned** — curl's heavily-fuzzed parsers hold; no real bug was mislabeled
   FP. Confirms the FP labels are not hiding defects.

### Final post-adversarial numbers

| | findings | TP | FP | precision |
|--|--|--|--|--|
| First pass | 11200 | 140 | 11060 | 1.25% |
| **Post-adversarial** | **11200** | **100** | **11100** | **0.89%** |

100 confirmed TP — 60 DCL13-C const-param (low-conf), ~15 MSC04/DCL38/ERR33/
MEM03 advisory, and a few genuine medium-confidence (EXP34-C `curl_sasl.c:232`
bufref NULL deref, API00-C `headers.c:121` unchecked public-API deref, MSC37-C
fall-off-end). Zero high-severity memory-safety TPs. **0.89% precision** is the
lowest of the four audited codebases — curl is the most mature/fuzzed, and its
enabled-rule firings are almost entirely advisory.

**Zero-finding FN read (98 files) — done.** 5 `.c` files (590 lines:
`curl_range.c`, `curlx/warnless.c`, `dllmain.c`, `fileinfo.c`, `macos.c`) +
2 inline-bearing headers (`easy_lock.h`, `sigpipe.h`) read in full; remaining
~91 headers are pure declarations. **0 FNs** — sqc's zero findings here are
correct (`adversarial/zero_finding_fns.json`).

## Audit COMPLETE — all 427 in-scope files

| | files | findings | TP | FP | precision | FN |
|--|--|--|--|--|--|--|
| 329 with-finding (adjudicated) | 329 | 11200 | 100 | 11100 | **0.89%** | |
| 98 zero-finding (FN-read) | 98 | 0 | — | — | | 0 |
| **Total** | **427** | **11200** | **100** | **11100** | **0.89%** | **5** |

**Operational remaining (DB writes — run when benchmark DB is free):**
`realworld-import-labels adjudication_curl.csv` → `audit-complete` per file →
`audit-coverage --set-total 427` → `realworld-score`. Apply the 14-file WIN_MAC
exclusion to the curl benchmark scoring scope so the dashboard denominator
matches this oracle (task 189 sibling).

Top files: `lib/url.c` (447), `lib/http.c` (374), `lib/ftp.c` (262),
`src/tool_getparam.c` (255), `lib/telnet.c` (245), `src/tool_operate.c` (216),
`lib/urlapi.c` (209), `lib/vtls/openssl.c` (207), `lib/mprintf.c` (188).

## Audited files

(updated as buckets complete)
</content>

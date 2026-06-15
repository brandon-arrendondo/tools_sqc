# mosquitto upstream-PR candidates — drafts index

Each file below is a self-contained bug report + fix (root cause, reproduction,
suggested diff) against upstream mainline `eclipse-mosquitto/mosquitto @ d3dd4463`.
Every defect was independently re-verified against mainline source by a dedicated
reviewer; two were corrected on deep analysis (see bottom).

PR criteria (per user): valgrind/ASan reproduction (or a documented
allocation/IO fault-injection harness when the path is OOM-only) + a concrete
suggested fix diff.

## File-a-PR (9 confirmed real defects)

| # | File | Defect | Severity | Reachability | Prior art |
|---|------|--------|----------|--------------|-----------|
| 01 | `lib/handle_auth.c` | MQTT5 `auth_method`/`auth_data` leak per AUTH packet | Medium | **Normal-path** (valgrind) | none |
| 02 | `lib/srv_mosq.c` | c-ares `reply` list never `ares_free_data`'d (WITH_SRV) | Low–Med | **Normal-path** (valgrind) | none |
| 11 | `src/persist_write.c` + `libcommon/file_common.c` | double `fclose` of same `FILE*` on write error | Med–High | Error-path (/dev/full repro) | none |
| 10 | `src/websockets.c` | `p`/`user` leak on `lws_create_context` failure | Low | Config-reachable | none |
| 03 | `lib/http_client.c` | `http_request`(/`key`) leak on handshake-setup failure | Low | Injection-only | none |
| 04 | `lib/options.c` | TLS-setter strdup-failure leaks (tls_set ×3, tls_psk_set) | Low | Injection-only (OOM) | #1116 (distinct) |
| 06 | `lib/net_mosq.c` | SSL+socket leak on `tls__set_verify_hostname` failure | Low | Injection-only | #592/#1116 (distinct) |
| 07 | `src/net.c` | `p=realloc(p,…)` socks fd leak (TCP+unix) | Low | Injection-only (OOM) | #3412 (fix never touched net.c — distinct) |
| 08 | `src/handle_connect.c` | `BIO_new` NULL deref in client-cert path | Crash/DoS | Injection-only (OOM) | none |

Suggested PR order: **01** and **02** first (normal-path, valgrind-reproducible,
one-spot fixes, zero prior art), then **11** (clear /dev/full repro), then the
OOM-robustness batch (03/04/06/07/08) which can reference each other as
"error-path resource-cleanup hardening."

## Defensive-hardening only (1)

| # | File | Note |
|---|------|------|
| 09 | `src/websockets.c` | `in[inlen-1]` `size_t` underflow is real but **not attacker-reachable** (libwebsockets rejects non-`/` URIs before the callback, so `inlen≥1`). File as a low-priority defensive guard, NOT a security issue. |

## Withdrawn — not a bug (1)

| # | File | Why |
|---|------|-----|
| 05 | `lib/options.c` `mosquitto_tls_opts_set` | The earlier audit FN (MEM30-C `:263` "double-free") is a **false finding**. `mosquitto_FREE(x)` frees *and* sets `x=NULL` (`libcommon_memory.h:75`), so every free leaves a NULL pointer — no double-free, no UAF, no OOM dangle. PR #2683 (2022) fully fixed the original multi-call leak. Do not file. |

## Correction to the FN tally

`upstream_candidates.md` reported "22/22 FNs real and present." Deep per-defect
analysis revises this: **#05 (`tls_opts_set:263`) is not a real bug**, and #09 is
real-but-unreachable. So the upstream-actionable set is **9 file-a-PR defects + 1
defensive guard**, not the full original count. The handle_auth NULL-to-`%s` TP
(`src/handle_auth.c:101/105`) remains a separate genuine finding.

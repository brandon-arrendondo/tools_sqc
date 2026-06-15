# mosquitto upstream-PR candidates — verified present in mainline

Cross-checked the mosquitto audit's confirmed real bugs against upstream HEAD to
identify which warrant upstream PRs and which can be cited in the paper as
real-world recall results.

- **Audit pin:** `~/toolchain/mosquitto` @ `d3ee5c5c` (v2.1.2-33)
- **Mainline:** `~/data-enterprise/mosquitto-main` @ `d3dd4463` (v2.1.2-166, 2026-06-14; ~133 commits newer)
- **Method:** 5 adversarial reviewers re-confirmed each defect at the pin (the
  audit lacked adversarial review), then located it in mainline by function/code
  (line numbers shifted) and judged present/fixed/refactored/absent.

## Headline result

**22 / 22 false-negative bugs are real and ALL persist in mainline** — none were
fixed in the 133 commits since the pin. 13 high-severity, 9 medium/low. These are
the bugs sqc *missed* (the recall gap, tasks 172–174) and are the strongest
upstream-PR set because they are independently confirmed and still live.

## Tier 1 — high-severity, high-confidence (PR-ready), 13

| Rule | mainline loc | Defect |
|------|--------------|--------|
| MEM31-C | `lib/handle_auth.c:63` | `auth_method` (mosquitto_property_read_string, caller must free) never freed → leak every MQTT5 AUTH packet |
| MEM31-C | `lib/handle_auth.c:64` | `auth_data` (mosquitto_property_read_binary) never freed → same leak |
| MEM31-C | `lib/srv_mosq.c:46` | `ares_parse_srv_reply` list never released with `ares_free_data()` → leak every SRV lookup |
| MEM12-C | `lib/http_client.c:59` | `context->http_request` leaked on `create_request_key` failure path |
| MEM12-C | `lib/http_client.c:62` | `context->http_request` + `key` leaked on `ws__create_accept_key` failure path |
| MEM31-C | `lib/options.c:163` | `mosquitto_tls_set`: `tls_capath` strdup fail leaks `tls_cafile` |
| MEM31-C | `lib/options.c:179` | `mosquitto_tls_set`: `tls_certfile` strdup fail leaks cafile+capath |
| MEM31-C | `lib/options.c:203` | `mosquitto_tls_set`: `tls_keyfile` strdup fail leaks cafile+capath+certfile |
| MEM30-C | `lib/options.c:263` | `mosquitto_tls_opts_set`: `tls_ciphers` freed then conditionally re-alloc → double-free on error path |
| MEM31-C | `lib/options.c:449` | `mosquitto_tls_psk_set`: `tls_ciphers` strdup fail leaks tls_psk + identity |
| MEM30-C | `lib/net_mosq.c:951` | `tls__set_verify_hostname` failure returns without `net__socket_close` → SSL + socket leak |
| MEM31-C | `src/net.c:852` | `mosquitto_realloc` for `listener->socks` overwrites old ptr on NULL → leaks open fds |
| MEM31-C | `src/bridge.c:144` | `bridge__new`: realloc failure leaks `new_context` (malloc'd + hash-registered) → heap + orphaned hash entry |
| EXP34-C | `src/handle_connect.c:945` | `BIO_new(BIO_s_mem())` NULL not checked before `X509_NAME_print_ex` → NULL deref |
| MEM31-C | `src/websockets.c:761` | `lws_create_context` failure leaks `p` (ws_protocol) + `user` |
| MEM12/INT | `src/websockets.c:380` | `http__canonical_filename`: `in[inlen-1]` with `inlen==0` → `size_t` underflow → OOB read |
| MEM30-C | `src/persist_write.c:399` | `persist__write_data` error block fcloses `db_fptr` without nulling; caller `libcommon/file_common.c` fcloses same FILE* → double `fclose` |

(17 rows; the lib/options.c cascade + net.c:852 dominate. The `options.c` TLS
strdup-leak cascade is one coherent PR.)

## Tier 2 — real but lower severity / rarer trigger, 5

| Rule | mainline loc | Defect | Why tier 2 |
|------|--------------|--------|-----------|
| MEM31-C | `src/net.c:967` | realloc-NULL leak in `net__socket_listen_unix` | unix-socket setup, rarely OOM |
| MEM31-C | `src/control.c:157` | malloc fail leaves orphaned `HASH_ADD` entry (cb_new) | depends on cleanup path; OOM-only |
| MEM31-C | `src/database.c:1024` | `msg_store_count/bytes` incremented before `db__msg_store_add`; inflated on ALREADY_EXISTS | counter drift, not memory unsafety |
| MSC00-C | `src/persist_read.c:458` | CRC field read from persistence header never validated → corrupt files load silently | integrity gap, not a crash |
| EXP34-C | `src/logging.c:333` | `get_time(&ti)` failure leaves `ti` from failed `localtime()` (NULL); `strftime(...,ti)` is UB | localtime failure is rare |

## Notes for upstream + paper

- **Upstream grouping:** the 5 `lib/options.c` TLS-setter leaks/double-free are
  one natural PR (all in adjacent functions); `lib/handle_auth.c` 2 leaks are
  another; `lib/http_client.c` 2 leaks another. See task 164.
- **Paper angle:** all 22 are *false negatives* — they demonstrate the recall
  ceiling on hardened real-world C and motivate tasks 172–174 (taint-aware
  untrusted-length, OOM null-deref, integer-truncation patterns). They are NOT
  sqc TPs.
- **TPs worth upstreaming are far fewer:** the audit's genuine-defect TP that is
  a real runtime bug is `src/handle_auth.c:101/105` (EXP34-C — `mosquitto_FREE`
  nulls `auth_method`, then it's passed to `log__printf("%s", auth_method)` →
  NULL-to-`%s` UB; README confirms present in mainline). Most other TPs are
  DCL13-C const-correctness / EXP20-C idioms, not PR material.

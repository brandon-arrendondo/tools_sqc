# mosquitto ground-truth audit (file-at-a-time) — sqc v0.4.30, codebase commit d3ee5c5c

mosquitto is the **second codebase converted to the file-at-a-time audited-corpus
model** (after libcrc, `data/precision_audit/libcrc/`), and the first
medium-sized one. `~/data/mosquitto` is pinned to commit
`d3ee5c5ca62c0fa4983308c6fff558ee978e878c` (the same commit referenced by
`conf/realworld/mosquitto-rules.toml` and the v0.4.22 sample in
`adjudication_0.4.22.csv`). `~/data/mosquitto-main` tracks upstream `master`
for later trunk-validation (libcrc-style).

## Scope

Per task 157: **`lib/` (libmosquitto client library) + `src/` (broker
daemon)** — the shipped product. Excludes `deps/` (vendored
picohttpparser), `test/`, `client/`, `apps/`, `plugins/` (example plugins),
`common/`/`libcommon/` (small shared helpers, pulled in only as cross-file
context).

Widened per the benchmarking_db 737 audit: `make install` also installs the
public API headers under `include/`, and both `lib/` and `src/` `#include`
them, so they belong in scope too. `config.h` (generated build config,
`#include`d throughout) is in scope for the same reason. The C++ wrapper
headers (`include/mosquitto/libmosquittopp.h`, `include/mosquittopp.h`) are
excluded by name — a language-scope call (sqc is CERT-C only), not an
oversight.

| Scope | Files |
|-------|-------|
| `lib/` | 59 |
| `src/` | 83 |
| `include/` + `config.h` | 37 |
| **Total in-scope** | **179** |

This is a stricter scope than the v0.4.22 sample (which drew from the whole
repo incl. `plugins/` and `test/`); the 15 existing v0.4.22 mosquitto labels in
`adjudication_0.4.22.csv` include some `plugins/`/`test/` paths that are
out-of-scope here and are not part of this corpus's denominator.

## Method (mirrors libcrc)

Invocation matters (libcrc lesson): scanning `src/` without `-I lib` inflated
DCL31-C from 9 -> 446 (broker headers `logging_mosq.h`, `mosquitto_internal.h`,
`net_mosq.h`, `tls_mosq.h` live in `lib/`, matching the real build's
`-I${R}/lib -I${R}/libcommon` per `src/Makefile`). The config-correct
invocations (from `~/data/mosquitto`):

```bash
# lib/ (59 files, 714 findings)
sqc lib \
  -I . -I include -I common -I libcommon -I lib \
  -I /usr/include -I /usr/include/cjson \
  -d common -d libcommon \
  --manifest ~/data/tools_sqc/conf/realworld/mosquitto-rules.toml \
  --export ~/data/tools_sqc/data/precision_audit/mosquitto/mosquitto_lib_0.4.30.json

# src/ (83 files, 2672 findings)
sqc src \
  -I . -I include -I common -I libcommon -I lib -I src \
  -I /usr/include -I /usr/include/cjson -I deps/picohttpparser \
  -d common -d libcommon -d lib \
  --manifest ~/data/tools_sqc/conf/realworld/mosquitto-rules.toml \
  --export ~/data/tools_sqc/data/precision_audit/mosquitto/mosquitto_src_0.4.30.json
```

Binary: built from source at Cargo.toml v0.4.30 into an isolated target dir
(`/tmp/sqc-mosquitto-audit`) so as not to disturb `target/release/sqc`, which
another concurrent session uses for the sqlite FP-reduction benchmark gate
(task 171).

## Top rules (config-correct, 3386 findings / 142 files)

    MEM30-C 657   API00-C 377   EXP33-C 264   EXP20-C 258   DCL13-C 172
    EXP34-C 148   MEM31-C 131   MEM12-C 118   API02-C  70   PRE08-C  64
    CON34-C  60   STR03-C  52   CON33-C  51   STR34-C  48   EXP43-C  46

126/142 files have at least one finding; 16 are clean
(`lib/handle_auth.c`, `lib/handle_connack.c`, `lib/handle_disconnect.c`,
`lib/http_client.h`, `lib/property_mosq.h`, `lib/pthread_compat.h`,
`lib/read_handle.h`, `lib/socks_mosq.h`, `src/acl_file.c`, `src/acl_file.h`,
`src/handle_disconnect.c`, `src/mux.h`, `src/password_file.c`,
`src/password_file.h`, `src/read_handle.c`, `src/sys_tree.h`) — these still
need a read-through for FNs.

## CSV-first workflow note (this session)

`data/benchmarks.db` is gitignored and another concurrent session is actively
writing the sqlite FP-reduction results to the local copy. To avoid
concurrent-writer risk, this session's labels accumulate in
`adjudication_mosquitto.csv` (same per-finding format as
`adjudication_libcrc_0.4.24.csv`) and are **not yet imported** via
`bench realworld-import-labels` / `bench audit-complete`. Import on the home
runner once the sqlite gate work lands:

```bash
python3 -m bench realworld-import-labels \
    data/precision_audit/mosquitto/adjudication_mosquitto.csv \
    --run <ingested-mosquitto-run> --source mosquitto_full_audit_0.4.30 \
    --adjudicator claude --date <date>
# then per audited file:
python3 -m bench audit-complete --project mosquitto --file <path> --adjudicator claude
python3 -m bench audit-coverage --project mosquitto --set-total 142 \
    --note "lib/ + src/ broker, excl. deps/test/client/apps/plugins (task 157)"
```

## Progress

| Bucket | Files | Status |
|--------|-------|--------|
| 0 findings (read-for-FN only) | 16 | **done** |
| 1-5 findings | 27 (69 findings) | **done** |
| 6-15 findings | 37 (340 findings) | **done** |
| 16-40 findings | 40 (1041 findings) | **done** |
| 41+ findings | 22 (1935 findings) | **done** |

Audited-file log and per-file TP/FP/FN tallies below, updated as files are
completed.

### Audited files

**0-finding bucket (16/16, complete):**

| File | Findings | FN |
|------|----------|----|
| `lib/handle_auth.c` | 0 | **2** (MEM31-C leak: `auth_method`, `auth_data` never freed — see below) |
| `lib/handle_connack.c` | 0 | 0 |
| `lib/handle_disconnect.c` | 0 | 0 |
| `lib/http_client.h` | 0 | 0 |
| `lib/property_mosq.h` | 0 | 0 |
| `lib/pthread_compat.h` | 0 | 0 |
| `lib/read_handle.h` | 0 | 0 |
| `lib/socks_mosq.h` | 0 | 0 |
| `src/acl_file.c` | 0 | 0 |
| `src/acl_file.h` | 0 | 0 |
| `src/handle_disconnect.c` | 0 | 0 |
| `src/mux.h` | 0 | 0 |
| `src/password_file.c` | 0 | 0 |
| `src/password_file.h` | 0 | 0 |
| `src/read_handle.c` | 0 | 0 |
| `src/sys_tree.h` | 0 | 0 |

**`lib/handle_auth.c` FN detail (MEM31-C, both real, both still present
upstream):** `mosquitto_property_read_string()` and
`mosquitto_property_read_binary()` (`libcommon_properties.h`) both document
"On success, value must be free()'d by the application." `handle__auth()`
reads `auth_method` (line 63) and `auth_data` (line 64) this way, passes both
as `const` to `callback__on_ext_auth()` (doesn't free — const params), then
returns without freeing either. Every MQTT5 AUTH packet during extended
(SASL-style) authentication leaks both. Verified identical in
`~/data/mosquitto-main` HEAD (`d3dd4463`, 2026-06-14) — not yet fixed
upstream, candidate for an upstream PR.

`src/handle_disconnect.c`'s double `mosquitto_property_free_all(&properties)`
(lines 60/63) is a logic quirk (DISCONNECT properties read then discarded
before `property__process_disconnect`, marked `FIXME - TEMPORARY` in source)
but not a double-free (the helper nulls the pointer) and maps to no CERT rule
— not recorded.

**1-5 finding bucket (27/27 files, 69 findings, complete):** 9 TP / 60 FP.

| File | Findings | TP | FP |
|------|----------|----|----|
| `lib/alias_mosq.h` | 3 | 0 | 3 |
| `lib/callbacks.h` | 2 | 0 | 2 |
| `lib/extended_auth.c` | 4 | 0 | 4 |
| `lib/handle_ping.c` | 2 | 0 | 2 |
| `lib/handle_pubackcomp.c` | 3 | 0 | 3 |
| `lib/handle_pubrec.c` | 1 | 0 | 1 |
| `lib/handle_pubrel.c` | 1 | 0 | 1 |
| `lib/handle_suback.c` | 3 | 0 | 3 |
| `lib/handle_unsuback.c` | 1 | 0 | 1 |
| `lib/logging_mosq.c` | 3 | 0 | 3 |
| `lib/logging_mosq.h` | 2 | 1 | 1 |
| `lib/messages_mosq.h` | 1 | 0 | 1 |
| `lib/read_handle.c` | 1 | 0 | 1 |
| `lib/send_connect.c` | 5 | 0 | 5 |
| `lib/send_disconnect.c` | 4 | 0 | 4 |
| `lib/send_mosq.h` | 1 | 0 | 1 |
| `lib/send_unsubscribe.c` | 4 | 1 | 3 |
| `lib/tls_mosq.c` | 3 | 1 | 2 |
| `lib/tls_mosq.h` | 2 | 2 | 0 |
| `lib/util_mosq.h` | 3 | 0 | 3 |
| `lib/will_mosq.h` | 1 | 0 | 1 |
| `src/handle_auth.c` | 2 | 1 | 1 |
| `src/handle_connack.c` | 2 | 0 | 2 |
| `src/plugin_reload.c` | 4 | 1 | 3 |
| `src/plugin_tick.c` | 4 | 1 | 3 |
| `src/plugin_v5.c` | 1 | 1 | 0 |
| `src/send_unsuback.c` | 5 | 0 | 5 |

New categorical FP patterns confirmed in this bucket (beyond PRE08-C header
collisions and the API00-C/API02-C/MEM10-C/INT30-C patterns from the 0-finding
bucket's supporting reads):

- **`#ifdef`/`#endif` branch-merging misfires**: MSC37-C ("non-void function
  may not return") and EXP33-C ("rc used uninitialized") both fire when an
  assignment/return lives inside one `#ifdef WITH_BROKER`/`WITH_BRIDGE` arm
  and sqc's path analysis doesn't connect it to the matching use in the same
  arm (`lib/handle_ping.c:40`, `lib/handle_pubackcomp.c:41`,
  `src/handle_connack.c:153`).
- **`utlist.h` macro opacity**: `DL_FOREACH_SAFE(head, el, el1)` expands to a
  for-loop that both initializes `el`/`el1` and guards the body on `el`
  non-NULL; sqc doesn't expand this external-header macro, so it flags the
  loop variables as "used uninitialized" (EXP33-C) and "potential NULL deref"
  (EXP34-C) every time `DL_FOREACH_SAFE` is used (`src/plugin_reload.c`,
  `src/plugin_tick.c` — likely recurs in `src/*.c` broker files using utlist).
- **`packet__alloc()` success implies non-NULL**: same shape as the
  `lib/extended_auth.c` FP — once `rc = packet__alloc(&packet, ...); if(rc)
  return rc;` passes, `packet` is non-NULL for the rest of the function, but
  sqc still flags every subsequent `packet__write_*`/`packet__queue` call as
  "passing null pointer 'packet'" (EXP34-C) (`src/send_unsuback.c`).
- **DCL13-C const-correctness recommendations are individually accurate but
  systemic** (172 total): `opts`/`mosq`/`topic` params that are genuinely
  never modified get flagged; labeled TP/low-confidence since the project
  doesn't follow this convention for any of its ~hundreds of handle/context
  params.

**Genuine bug found**: `src/handle_auth.c:101/105` — `mosquitto_FREE(auth_method)`
nulls `auth_method`, then the very next statement passes the now-NULL
`auth_method` to `log__printf("...%s...", ..., auth_method)` for the
"non-matching auth-method property (%s:%s)" diagnostic — the log message can
never show the actual mismatched value, and passing NULL to `%s` is UB
(C11 7.21.6.1). sqc's MEM30-C "use-after-free" framing is wrong (the pointer
was nulled, not left dangling) but the EXP34-C NULL-to-%s finding on the next
line is correct. Confirmed present in `~/data/mosquitto-main` HEAD.

**6-15 finding bucket (37/37 files, 340 findings, complete): 57 TP / 283 FP, 1 FN.**
Processed via 5 parallel subagents (mirroring the libcrc/sqlite methodology),
each given the full categorical-pattern list, the pinned-commit source tree,
and a slice of `mosquitto_lib_0.4.30.json`/`mosquitto_src_0.4.30.json`.

| File | Findings | TP | FP | FN |
|------|----------|----|----|----|
| `lib/actions_publish.c` | 9 | 0 | 9 | |
| `lib/actions_subscribe.c` | 9 | 0 | 9 | |
| `lib/actions_unsubscribe.c` | 11 | 1 | 10 | |
| `lib/handle_publish.c` | 10 | 0 | 10 | |
| `lib/helpers.c` | 12 | 3 | 9 | |
| `lib/loop.c` | 8 | 3 | 5 | |
| `lib/mosquitto_internal.h` | 6 | 6 | 0 | |
| `lib/net_mosq_ocsp.c` | 9 | 0 | 9 | |
| `lib/packet_mosq.h` | 7 | 0 | 7 | |
| `lib/property_mosq.c` | 8 | 0 | 8 | |
| `lib/send_mosq.c` | 15 | 0 | 15 | |
| `lib/send_subscribe.c` | 6 | 1 | 5 | |
| `lib/socks_mosq.c` | 13 | 0 | 13 | |
| `lib/srv_mosq.c` | 8 | 0 | 8 | **1** |
| `lib/thread_mosq.c` | 8 | 1 | 7 | |
| `lib/util_mosq.c` | 14 | 0 | 14 | |
| `src/broker_control.c` | 7 | 2 | 5 | |
| `src/control_common.c` | 9 | 2 | 7 | |
| `src/plugin_acl_check.c` | 13 | 7 | 6 | |
| `src/plugin_basic_auth.c` | 6 | 2 | 4 | |
| `src/plugin_cleanup.c` | 9 | 1 | 8 | |
| `src/plugin_client_offline.c` | 6 | 2 | 4 | |
| `src/plugin_connect.c` | 7 | 0 | 7 | |
| `src/plugin_disconnect.c` | 6 | 2 | 4 | |
| `src/plugin_extended_auth.c` | 10 | 0 | 10 | |
| `src/plugin_init.c` | 8 | 4 | 4 | |
| `src/plugin_message.c` | 11 | 0 | 11 | |
| `src/plugin_psk_key.c` | 10 | 0 | 10 | |
| `src/plugin_subscribe.c` | 7 | 2 | 5 | |
| `src/plugin_unsubscribe.c` | 7 | 2 | 5 | |
| `src/proxy_v1.c` | 10 | 7 | 3 | |
| `src/send_auth.c` | 8 | 0 | 8 | |
| `src/send_connack.c` | 6 | 0 | 6 | |
| `src/send_suback.c` | 8 | 0 | 8 | |
| `src/session_expiry.c` | 14 | 3 | 11 | |
| `src/watchdog.c` | 11 | 3 | 8 | |
| `src/will_delay.c` | 14 | 3 | 11 | |

All 37 TPs are low/medium confidence: DCL13-C const-param suggestions (genuinely
unmodified pointers, but the codebase doesn't follow this convention anywhere),
EXP45-C assign-in-if idioms, EXP20-C `!memcmp`/`!strncmp` boolean idioms,
DCL37-C `_GNU_SOURCE`/`__attribute__` reserved-identifier shims, ARR02-C
implicit-array-bound, PRE10-C/PRE00-C/PRE12-C/PRE01-C macro-hazard findings on
`lib/mosquitto_internal.h`'s `SAFE_PRINT`/`SAFE_FREE`/`STREMPTY`/`WINDOWS_SET_ERRNO_RW`
macros, and a small set of genuine medium-confidence findings: `lib/loop.c`
(`mosquitto_loop_read`/`mosquitto_loop_write` are exported API functions that
dereference `mosq` without a NULL check, unlike sibling `mosquitto_loop`/
`mosquitto_loop_misc`), `src/proxy_v1.c` (`atoi()` on untrusted PROXY-protocol
port fields, ERR07-C/ERR34-C), and `src/watchdog.c` (CON34-C `getenv`
thread-safety, ERR01-C/ERR30-C `strtol` errno not checked).

**FN found**: `lib/srv_mosq.c:49` — `srv_callback()` calls
`ares_parse_srv_reply(abuf, alen, &reply)`; on `ARES_SUCCESS` this allocates a
heap-resident `struct ares_srv_reply` linked list via `reply`, which is walked
but never released with `ares_free_data(reply)`. No such free exists anywhere
in `lib/` or `src/` — every successful SRV lookup leaks the c-ares reply list
(MEM31-C-class leak, not raised by sqc).

### New categorical patterns confirmed in this bucket

- **EXP34-C "passing NULL... does not check for NULL" where the callee
  internally traverses with `while(p)`/`for(p=ptr; p; p=p->next)`**: such
  loops are inherently NULL-safe (zero iterations on NULL) — FP even though
  sqc sees no explicit `if(p==NULL)` guard. Applies to
  `mosquitto_property_get_length_all`/`mosquitto_property_get_remaining_length`/
  `property__write_all` on a `mosquitto_property*` list, where NULL is the
  valid "empty properties" representation throughout the codebase.
- **STR30-C "string literal passed to function which may modify it"** on
  `pthread_setname_np`/`pthread_set_name_np`: FP — both take `const char *name`
  and copy it; verify the actual prototype before trusting this rule.
- **EXP05-C false "cast away const" on `memcpy(&dst, src, sizeof(T))`**:
  `sizeof(type)` in the size argument is misidentified as a cast expression —
  no cast exists. Seen in `lib/actions_publish.c:71`, `lib/actions_subscribe.c:71`.
- **API00-C on trivial one-line wrapper functions** (`return real_impl(...)`):
  FP when the wrapper never dereferences its pointer params itself and
  validation lives entirely in the delegate (e.g. `mosquitto_publish` ->
  `mosquitto_publish_v5`).
- **MEM30-C/MEM31-C "double-free" across mutually-exclusive normal-path vs.
  `error:`-label cleanup blocks**: FP when the two blocks are structurally
  identical but reached via disjoint control flow (normal fallthrough vs.
  `goto error` followed by unconditional `return`) — sqc treats them as both
  executing. Seen in `lib/net_mosq_ocsp.c:170,173`.
- **MEM30-C on `&struct.field` taken right after `mosquitto_FREE(struct.field)`**:
  `mosquitto_FREE` nulls the field; passing its address as an out-parameter for
  a callee to repopulate is not a freed-pointer access. Seen in
  `lib/handle_publish.c:71-72`.
- **MEM30-C/MEM12-C on free-then-reassign-then-check** (`mosquitto_FREE(x); x =
  alloc(...); if(!x){...}`): sqc sometimes flags the `if(!x)` check or even the
  `FREE` call as "use-after-free"/"double-free", missing the reassignment in
  between. Seen throughout `lib/socks_mosq.c`.
- **MEM12-C "leak" on struct-member allocations** (`mosq->in_packet.payload =
  malloc(...)`): FP — stored in a long-lived struct and freed later via
  `packet__cleanup`, not a stack-local that "leaks" on return.
- **MEM31-C "double free: 'item' was already freed" inside `DL_FOREACH_SAFE`**:
  `mosquitto_FREE(item)` frees a *different* list node each iteration; sqc
  conflates the loop variable across iterations as the same object. Seen in
  `src/will_delay.c:73,100`.
- **ARR36-C "pointer comparison/subtraction between different arrays"** on
  plain `time_t` scalar struct-member reads (`db.now_s`, `mosq->next_msg_out`):
  sqc appears to misparse struct-member-access expressions as array-pointer
  references. Seen in `lib/util_mosq.c:88`.
- **DCL30-C "local variable address escapes" on `*out = heap_ptr`**: storing a
  *heap* pointer value (from `mosquitto_malloc`) through an out-parameter is
  not a stack-address escape. Seen in `lib/util_mosq.c:164`.

**16-40 finding bucket (40/40 files, 1041 findings, complete): 187 TP / 854 FP / 5 FN.**
Processed via 8 parallel subagents (5 files × 8, balanced at ~130 findings/batch).

| File | Findings | TP | FP | FN |
|------|----------|----|----|----|
| `lib/alias_mosq.c` | 24 | 3 | 21 | |
| `lib/callbacks.c` | 26 | 17 | 9 | |
| `lib/connect.c` | 38 | 6 | 32 | |
| `lib/http_client.c` | 29 | 1 | 28 | **2** |
| `lib/libmosquitto.c` | 23 | 7 | 16 | |
| `lib/net_mosq.h` | 18 | 4 | 14 | |
| `lib/net_ws.c` | 20 | 0 | 20 | |
| `lib/packet_datatypes.c` | 17 | 0 | 17 | |
| `lib/packet_mosq.c` | 16 | 0 | 16 | |
| `lib/send_publish.c` | 33 | 8 | 25 | |
| `src/conf_includedir.c` | 28 | 0 | 28 | |
| `src/context.c` | 24 | 0 | 24 | |
| `src/control.c` | 31 | 2 | 29 | **1** |
| `src/handle_subscribe.c` | 32 | 0 | 32 | |
| `src/handle_unsubscribe.c` | 16 | 0 | 16 | |
| `src/http_serv.c` | 35 | 3 | 32 | |
| `src/keepalive.c` | 22 | 4 | 18 | |
| `src/listeners.c` | 27 | 1 | 26 | |
| `src/logging.c` | 32 | 9 | 23 | **1** |
| `src/loop.c` | 19 | 5 | 14 | |
| `src/mosquitto_broker_internal.h` | 24 | 2 | 22 | |
| `src/mux.c` | 16 | 0 | 16 | |
| `src/mux_epoll.c` | 34 | 11 | 23 | |
| `src/mux_kqueue.c` | 34 | 0 | 34 | |
| `src/mux_poll.c` | 16 | 5 | 11 | |
| `src/persist.h` | 30 | 6 | 24 | |
| `src/persist_read_v234.c` | 38 | 8 | 30 | |
| `src/persist_read_v5.c` | 35 | 0 | 35 | |
| `src/persist_write.c` | 36 | 10 | 26 | **1** |
| `src/persist_write_v5.c` | 35 | 12 | 23 | |
| `src/plugin_callbacks.c` | 21 | 2 | 19 | |
| `src/plugin_v2.c` | 16 | 12 | 4 | |
| `src/plugin_v3.c` | 17 | 7 | 10 | |
| `src/plugin_v4.c` | 19 | 13 | 6 | |
| `src/property_broker.c` | 16 | 1 | 15 | |
| `src/proxy_v2.c` | 35 | 0 | 35 | |
| `src/psk_file.c` | 27 | 6 | 21 | |
| `src/service.c` | 28 | 5 | 23 | |
| `src/topic_tok.c` | 16 | 2 | 14 | |
| `src/xtreport.c` | 38 | 15 | 23 | |

Notable TPs in this bucket: `lib/callbacks.c` (17 API00-C TPs — all `libmosq_EXPORT` callback-setter functions dereference `mosq` through a mutex lock without a prior NULL check, unlike equivalent non-exported internal functions); `src/xtreport.c` (15 TPs — DCL11-C `%lu` format mismatch with signed `long` counters, plus INT10-C and CON33-C); `src/persist_write_v5.c` (12 TPs — EXP05-C genuine const-casts); `src/plugin_v4.c` (13 TPs — EXP45-C assign-in-condition on every LIB_SYM optional lookup); `src/mux_epoll.c` (11 TPs — CON33-C `strerror` + ERR33-C unchecked `time(NULL)` + DCL37-C + DCL13-C).

**FNs found (5):**
- `lib/http_client.c:59` — `context->http_request` (allocated at line 53) is leaked on the `create_request_key` failure path (no free before `goto error` / `return` on that branch).
- `lib/http_client.c:62` — `context->http_request` and `key` both leaked on the `ws__create_accept_key` failure path — sqc missed this error branch entirely.
- `src/control.c:157` — On `mosquitto_malloc` failure for `ep` at line 157, the `HASH_ADD` that registered `cb_new` at line 153 is never reversed; `control__unregister_all_callbacks` cannot clean up orphaned hash entries.
- `src/logging.c:333` — `get_time(&ti)` can return 1 (failure), leaving `*ti` as the result of a failed `localtime()` call (NULL); `strftime(log_line, ..., ti)` is then called with a potentially NULL `struct tm*` (UB per C11 7.27.3.5).
- `src/persist_write.c:399` — `persist__write_data`'s error block calls `fclose(db_fptr)` but does not null `db_fptr`; its caller in `libcommon/file_common.c` calls `fclose(fptr)` (same `FILE*`) again on any write-function error return — double `fclose`.

### New categorical patterns confirmed in this bucket

- **`uthash` macros** (`HASH_FIND`, `HASH_ITER`, etc.): sqc doesn't expand `HASH_FIND(hh,head,key,len,out)` (which unconditionally sets `out=NULL`) or `HASH_ITER(hh,head,el,tmp)` (whose for-loop init assigns `el=head`), flagging the out/loop variable as EXP33-C "uninitialized" → FP (extends pattern #10).
- **EXP43-C on `mosquitto_callback_register(plugin, ..., NULL, plugin)`** (same pointer for `identifier` and `userdata`): FP — neither param is `restrict`-qualified in the actual prototype; this is the canonical registration idiom.
- **FIO09-C/FIO17-C on scalar `fread`** (reading a single `uint16_t`/`uint32_t` via `fread(&scalar, sizeof, 1, fp)` with explicit `ntohs`/`ntohl`): FP — not a struct-padding or null-termination issue.
- **Loop-iteration free conflation** (while/for loops that reset a local struct each iteration): sqc treats `free(sub.topic_filter)` in mutually-exclusive early-return branches as cascading UAF/double-free/leak across iterations — FP. Seen heavily in `src/handle_subscribe.c` and `src/handle_unsubscribe.c`.
- **STR34-C on `**ptr` double-pointer derefs**: sqc misidentifies `*ptr` (where `ptr` is `char**`) as a `char` value dereference → categorical FP across topic/subscription token handling.
- **MEM30-C on `mosq->wsd` struct fields** after `mosquitto_FREE(mosq->wsd.out_packet)`: sqc misidentifies subsequent accesses to *other* `wsd` sub-fields as accesses through the freed pointer — FP (sub-field free ≠ parent free).
- **FIO47-C snprintf/fprintf argument-position confusion**: sqc repeatedly misidentifies the `size` parameter (2nd arg to `snprintf`) as a format argument → categorical FP in `src/conf_includedir.c`, `lib/http_client.c`, `src/xtreport.c`.
- **ARR00-C on externally-initialized arrays** (`http_headers[N]` passed as out-param to `phr_parse_request`/`phr_parse_response`): sqc flags all subsequent reads as "uninitialized" — FP; the library fills the array before the loop runs.
- **MSC41-C false credential detection** (OpenSSL engine capability identifiers `"SECRET_MODE"`, `"PIN"`): these are API constant names, not actual secrets → FP.
- **ENV01-C on static fixed-size C arrays** unrelated to environment variables: sqc appears to fire this on any `static` fixed-size array — categorical FP.
- **CON34-C as redundant duplicate of CON33-C**: sqc emits both rules for every `strerror()` call at the same call site; CON34-C should be counted FP when CON33-C is already adjudicated TP for the same line.

**41+ finding bucket (22/22 files, 1935 findings, complete): 178 TP / 1757 FP / 14 FN.**
Processed via 9 parallel subagents (1 file for `src/conf.c` alone at 474 findings; 2-3 files per batch otherwise).

| File | Findings | TP | FP | FN |
|------|----------|----|----|----|
| `lib/messages_mosq.c` | 41 | 0 | 41 | |
| `lib/net_mosq.c` | 118 | 5 | 113 | **1** |
| `lib/options.c` | 53 | 10 | 43 | **5** |
| `lib/will_mosq.c` | 54 | 1 | 53 | |
| `src/bridge.c` | 114 | 0 | 114 | **1** |
| `src/bridge_topic.c` | 43 | 3 | 40 | |
| `src/conf.c` | 473 | 19 | 454 | |
| `src/database.c` | 128 | 0 | 128 | **1** |
| `src/handle_connect.c` | 66 | 0 | 66 | **1** |
| `src/handle_publish.c` | 110 | 0 | 110 | |
| `src/http_api.c` | 49 | 6 | 43 | |
| `src/mosquitto.c` | 89 | 32 | 57 | |
| `src/net.c` | 79 | 4 | 75 | **2** |
| `src/persist_read.c` | 61 | 17 | 44 | **1** |
| `src/plugin_persist.c` | 60 | 8 | 52 | |
| `src/plugin_public.c` | 43 | 5 | 38 | |
| `src/retain.c` | 52 | 14 | 38 | |
| `src/security_default.c` | 46 | 0 | 46 | |
| `src/signals.c` | 57 | 43 | 14 | |
| `src/subs.c` | 71 | 0 | 71 | |
| `src/sys_tree.c` | 47 | 0 | 47 | |
| `src/websockets.c` | 81 | 11 | 70 | **2** |

Notable findings in this bucket: `src/signals.c` has the highest precision (43/57 TP, 75%) — sqc correctly flags all signal-handler violations (SIG30-C/SIG31-C/SIG34-C) and unsafe-signal-handler function calls; `src/mosquitto.c` had the most structurally interesting TP (CON03-C on unprotected `g_run` volatile race, EXP34-C on `WinMain` NULL-mosq deref); `lib/options.c` had the most FNs (5 — progressive TLS strdup leak cascade in `mosquitto_tls_set` error paths).

**FNs found (14):**
- `lib/net_mosq.c:952` — `tls__set_verify_hostname` failure returns `MOSQ_ERR_TLS` without calling `net__socket_close`, leaking both the SSL object and the connected socket.
- `lib/options.c:163` — `mosquitto_tls_set`: `tls_capath` strdup failure leaks already-allocated `tls_cafile`.
- `lib/options.c:178` — `mosquitto_tls_set`: `tls_certfile` strdup failure leaks `tls_cafile` + `tls_capath`.
- `lib/options.c:202` — `mosquitto_tls_set`: `tls_keyfile` strdup failure leaks `tls_cafile` + `tls_capath` + `tls_certfile`.
- `lib/options.c:263` — `mosquitto_tls_opts_set`: `tls_ciphers` freed unconditionally then conditionally re-allocated — actual double-free on the error path.
- `lib/options.c:448` — `mosquitto_tls_psk_set`: `tls_ciphers` strdup failure leaks `tls_psk` + `tls_psk_identity`.
- `src/bridge.c:144` — `bridge__new`: `realloc` failure for `db.bridges` leaks `new_context` (init'd + registered in hash) — heap + hash entry orphaned.
- `src/database.c:1024` — `db__message_store`: counters `msg_store_count`/`msg_store_bytes` incremented before `db__msg_store_add`; if `MOSQ_ERR_ALREADY_EXISTS` path is taken, counters are permanently inflated.
- `src/handle_connect.c:945` — `BIO_new(BIO_s_mem())` return not NULL-checked; passed directly to `X509_NAME_print_ex` → NULL deref on BIO allocation failure.
- `src/net.c:857` — `mosquitto_realloc` for `listener->socks` overwrites old pointer on NULL return, leaking previously-opened socket fds.
- `src/net.c:971` — Same realloc-NULL pattern in `net__socket_listen_unix`.
- `src/persist_read.c:458` — CRC field read from persistence header is never checked against file data; corrupted persistence files load silently.
- `src/websockets.c:380` — `http__canonical_filename`: `in[inlen-1]` with `inlen == 0` causes `size_t` underflow → out-of-bounds read.
- `src/websockets.c:761` — `lws_create_context` failure leaks `p` (ws_protocol) and `user` (lws info.user) allocated earlier in the same call.

### New categorical patterns confirmed in this bucket

- **EXP20-C on `!strcmp()` config-key dispatch**: FP (idiomatic boolean config-key matching in `src/conf.c` and similar config-parsing files) — categorical FP for any config/key-dispatch file.
- **MEM30-C on `mosquitto_FREE(field)` + immediate reassignment in copy functions**: sqc doesn't track the free→assign→use chain in `config__copy`-style functions, generating ~50 false UAF per copy function → FP.
- **FIO50-C on integer variable named `tmp`**: sqc pattern-matches "tmp" to FILE* context → categorical FP.
- **MEM30-C cascade from conditional free** (`if(count==0){ mosquitto_FREE(arr); }`): sqc treats conditional free as unconditional and reports all subsequent `->member` accesses as UAF → FP.
- **struct-member-free cascade** (`mosquitto_FREE(base_msg->data.topic)` causes UAF reports on all `base_msg->data.*`): freeing one field of a struct, sqc incorrectly propagates to the whole struct → FP.
- **SIG34-C signal-handler context misattribution**: sqc fires "signal() called from within signal handler" on setup functions called from `main()`, not from any handler → FP.
- **MEM30-C on free-then-re-alloc loops** (bridge topic remapping with `mapped_topic`): sqc can't track conditional free+reassign through loop logic → FP.
- **FIO42-C on domain functions with "open" in name** (`db__open`): sqc mistakes domain open functions for POSIX `open()` fd → FP.
- **POS36-C on correct `setgid`/`setuid` ordering**: sqc fires on the `setgid` call even when it correctly precedes `setuid` → FP.
- **PRE32-C on adjacent string literal concatenation**: C compiler string concatenation across lines misidentified as preprocessor directive issues → FP.

## Overall audit summary (all 142 files)

| Bucket | Files | Findings | TP | FP | FN |
|--------|-------|----------|----|----|----|
| 0 findings (FN read) | 16 | 0 | 0 | 0 | **2** |
| 1-5 findings | 27 | 69 | 9 | 60 | 0 |
| 6-15 findings | 37 | 340 | 57 | 283 | **1** |
| 16-40 findings | 40 | 1041 | 187 | 854 | **5** |
| 41+ findings | 22 | 1935 | 178 | 1757 | **14** |
| **Total** | **142** | **3385** | **431** | **2954** | **22** |

**Overall tool precision: 431 / 3385 = 12.7%** (87.3% false-positive rate across all rules).
**FNs found: 22** confirmed missed bugs across 142 files.

Top TP sources by count: `src/signals.c` (43), `src/mosquitto.c` (32), `lib/mosquitto_internal.h` (6 macro-hazard), `src/persist_read.c` (17), `src/xtreport.c` (15), `src/retain.c` (14).
High-FP rules: MEM30-C (657 findings, ~95% FP), EXP33-C (264 findings, ~98% FP), API00-C (377 findings, ~95% FP), MEM12-C (118, ~99% FP).

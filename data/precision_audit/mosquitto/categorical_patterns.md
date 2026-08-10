# mosquitto categorical FP/TP patterns

## MEM31-C

Batch `delta_mem31_b1` (142 findings, task 420): 0 TP / 142 FP. All FP, grouped into these classes:

- **int status var misattributed as allocation.** Callee returns `int` (status code) but its
  name (`mosquitto_property_copy_all`, `mosquitto_property_add_string/_binary`,
  `acl_file__parse`, `handle__publish`, `handle__suback`, `bridge__connect`,
  `bridge__connect_step1`, `will__set`, `config__read_file`, `socks5__read`) sounds like an
  allocator; the real allocation (if any) goes through an output param, not the `rc`/`ret`
  return value. Examples: lib/connect.c:134, lib/extended_auth.c:34/45/51, lib/loop.c:394/398,
  lib/read_handle.c:55/67, src/acl_file.c:40, src/bridge.c:175/177/317/337/555/1142/1156,
  src/conf.c:787.

- **socket-fd close misidentified as free().** `mosq_sock_t`/int socket descriptors passed to
  `COMPAT_CLOSE()` get treated as heap pointers passed to `free()`; "double free" fires across
  mutually-exclusive loop iterations or branches that each operate on a fresh `socket()` value.
  Examples: lib/net_mosq.c:408/481/510 (`net__try_connect_tcp`), lib/net_mosq.c:1164-1216
  (`net__socketpair` WIN32 branch, `listensock`/`spR`).

- **scratch-variable reuse across two distinct heap objects in one function.** Same local name
  (e.g. `ar_request`, `topic_temp`, `h`) is assigned+freed once for object A earlier in the
  function, then reassigned+freed again later for an unrelated object B — flagged as a "double
  free" or repeat "not freed" even though each object is freed exactly once. Example:
  lib/net_mosq.c:354 (`ar_request`), lib/send_publish.c:154/166 (`topic_temp`).

- **cleanup helper frees the whole struct, tool doesn't see it.** `message__cleanup(&message)`
  (lib/messages_mosq.c:33-45) frees `msg.topic` + `msg.payload` + `properties` + the struct
  itself in one call; every finding claiming one of those fields "leaks" right after a
  `message__cleanup()` call is FP. Examples: lib/actions_publish.c:140/149/158/162/178,
  lib/handle_publish.c:175/185/199/206/219.

- **ownership transfer via queue/list/hash append.** Allocation is hung off a persistent
  structure (`message__queue`→`DL_APPEND` inflight list, `packet__queue`→send queue,
  `LL_APPEND(bridge->topics,...)`, `HASH_ADD_KEYPTR`, realloc'd array element,
  `*out_param = ptr`) and freed later by that structure's own lifecycle, not by the
  allocating function. Examples: lib/actions_publish.c:178 (message__queue), lib/http_client.c:65/85
  (packet__queue), lib/helpers.c:118/136 (`*messages` output param), lib/util_mosq.c:159/165
  (`*bin` output param), lib/property_mosq.c:192/217/219 (`*properties` output param + list free),
  src/bridge_topic.c:210/244/281/294 (LL_APPEND / output param), src/control.c:141/145/167
  (HASH_ADD_KEYPTR / DL_APPEND), src/conf.c:193/203 (`listener->security_options` array element),
  src/conf.c:3044/3056/3059/3074/3089/3093/3097/3101 (`bridge1->remote_clientid`/`local_clientid`
  struct fields), src/conf_includedir.c:105/116/137/166/172/189 (`*files` output param + correct
  realloc-grow idiom), src/bridge.c:859 (`db.bridges` realloc-into-global-field idiom).

- **goto-cleanup correctly frees everything.** `goto error`/`error:` label frees every field
  the finding claims is leaked; tool flags the `goto` site without checking the label's body.
  Examples: src/bridge_topic.c:212/220/225/233/239 (error: label frees local_prefix/
  remote_prefix/local_topic/remote_topic/topic/cur_topic), src/conf.c:935/944 (error: label
  frees plugin->config.name/path), src/conf_includedir.c:174 (error: label frees l_files).

- **stale taint after unconditional free, flagged at every later return.** Variable is freed
  once (often unconditionally, on every branch that reaches a shared join point), then every
  subsequent, unrelated `return` in the rest of the function gets a spurious "not freed before
  return" regardless of reachability. Examples: lib/logging_mosq.c:62, lib/srv_mosq.c:107,
  src/bridge.c:351/358/361/595/602/611/615/646/648/651/659 (`notification_topic`),
  src/bridge.c:729/737/758 (`bridge__on_connect`), src/conf.c:624/629/640/648/661/664
  (`db.tls_keylog`, also a persistent global whose lifetime isn't scoped to this function at all).

- **leak claim targets a variable/branch not reachable or not in scope.** Either the value is
  provably NULL/unallocated on that specific control-flow path (lib/send_publish.c:203/205 —
  vars declared `= NULL` and only ever set inside a block that always returns before falling
  through), the finding points at the wrong preprocessor branch entirely (lib/srv_mosq.c:115 —
  `#else`/`WITH_SRV`-undefined branch where the variable isn't even declared; src/control.c:173
  — `#else` branch where the callback feature is compiled out), or the named variable is
  block-scoped and already out of scope at the flagged return (src/bridge_topic.c:311 —
  `topic_temp` declared inside `if(match){...}`, dead by the time of the flagged `return`).

No genuine MEM31-C bugs were found in this batch's specific findings. This does **not**
contradict the existing confirmed mosquitto MEM31-C bugs on record (e.g. lib/handle_auth.c:63-64,
lib/srv_mosq.c:46 `ares_parse_srv_reply`) — those are at different lines/functions not covered
by this batch; lib/srv_mosq.c *is* in this batch but the flagged lines (85/93/107/115) are in an
unrelated function (`mosquitto_connect_srv`) from the confirmed bug's location (`srv_callback`,
line 46).

Batch `delta_mem31_b2` (149 findings, task 420, `src/*` broker daemon incl. persist_read*/
persist_write_v5): 147 FP / 2 TP. All 8 batch-1 classes recurred (int-status misattribution was
by far the largest single driver — `rc`/`res`/`rc2` from `property__read_all`,
`packet__read_binary`, `db__message_insert_incoming`, `alias__add_r2l`, `listeners__add_local`,
`net__socket_listen_unix`, `password_file__parse`, `persist__read_string_len`,
`persist__chunk_*_read_v56` all misattributed as allocations). Two new classes:

- **module-static global reallocated/allocated, mistaken for an abandoned local.** The
  malloc/calloc/realloc target is a file-scope `static` that outlives the function and is freed
  by a dedicated `*__cleanup()` at broker shutdown, not by the allocating function. Examples:
  src/keepalive.c:86/101 (`keepalive_list`, freed in `keepalive__cleanup` src/keepalive.c:114),
  src/mux_poll.c:71/81 (`pollfds`, freed in `mux_poll__cleanup` src/mux_poll.c:256),
  src/listeners.c:89/97/106/135 (`g_listensock`, realloc'd in place, freed at shutdown).

- **allocation stored into a persistent config-array element or struct field, not a plain
  local.** `listeners[db.config->listener_count].security_options`/`->auto_id_prefix`/`.host`
  (src/listeners.c:163-190) and `context->username` (src/net.c:358/366, freed in
  `context__cleanup` src/context.c:178) are freed correctly on every error branch and, on
  success, live for the lifetime of the owning long-lived struct — same ownership-transfer
  family as batch 1's queue/list/hash class but via array-index/struct-field assignment instead
  of an explicit `DL_APPEND`/`HASH_ADD`.

- **same-named local from a different function/scope misattributed (persist_write_v5.c:203).**
  Two sibling functions each declare a block-scoped `prop_packet` inside an `if`; an error label
  in the *second* function is only reachable via paths where that function's own `prop_packet`
  is out of scope or already freed, but the finding reads as if it were the same variable as the
  first function's differently-scoped copy of the same name.

Two genuine, if narrow, TPs confirmed:

- **src/http_api.c:100** — `http__canonical_filename()` mallocs `filename` (line 84) and frees it
  on every other error branch (WIN32 line 104 / POSIX line 112) before their respective returns,
  but the `filename_canonical = mosquitto_calloc(1, PATH_MAX)` failure branch (lines 98-101)
  returns `NULL` without freeing `filename` first. Reachable only on a calloc/OOM failure, but a
  real, previously-undetected leak.
- **src/mosquitto.c:600 (WinMain, WIN32-only)** — `argv = mosquitto_realloc(argv, ...)` (line 605)
  reassigns the only reference to `argv` in place; if realloc fails, the prior heap block becomes
  unreachable and the function returns `MOSQ_ERR_NOMEM` (line 608) without freeing it — the
  classic "realloc into the same variable" anti-pattern. Windows-only and OOM-triggered, so very
  narrow, but a genuine defect at the flagged site.

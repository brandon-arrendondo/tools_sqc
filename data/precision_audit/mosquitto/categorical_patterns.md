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

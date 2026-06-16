## Summary

Two related memory leaks in SRV (`WITH_SRV`) support, both reproduced and
fixed:

1. **Per-lookup leak** — `srv_callback()` never frees the
   `struct ares_srv_reply` list returned by `ares_parse_srv_reply()`.
2. **Per-client leak** — the c-ares channel `mosq->achan` created by
   `mosquitto_connect_srv()` (`ares_init`) is never released; `ares_destroy()`
   is called nowhere in the library.

Two commits, one per file (`lib/srv_mosq.c`, `lib/libmosquitto.c`).

## Bug 1 — reply list leaked in `srv_callback` (`lib/srv_mosq.c`)

```c
if(status == ARES_SUCCESS){
    status = ares_parse_srv_reply(abuf, alen, &reply);
    if(status == ARES_SUCCESS){
        mosquitto_connect(mosq, reply->host, reply->port, mosq->keepalive);
    }
}else{ ... }
```

`ares_parse_srv_reply()` allocates a list into `reply` that the c-ares API
requires the caller to release with `ares_free_data()`; it is never freed
(`ares_free_data` appears nowhere in the tree). Every successful SRV lookup
leaks the reply list.

## Bug 2 — c-ares channel leaked (`lib/libmosquitto.c`)

`mosquitto_connect_srv()` does `ares_init(&mosq->achan)`. The channel is used by
the event loop (`ares_fds`/`ares_process` in `lib/loop.c`) but `ares_destroy()`
is never called, and `mosquitto__destroy()` does not touch `achan`. Every client
that uses SRV connection leaks the whole channel (tens of KB) until process
exit; repeatedly creating/destroying SRV clients leaks one channel each.

## Reproduction

`WITH_SRV` build, a resolvable `_mqtt._tcp.<domain>` SRV record, and a minimal
client that calls `mosquitto_connect_srv()` then pumps the loop so the resolver
callback fires:

```sh
valgrind --leak-check=full --show-leak-kinds=all ./srv_client <domain>
```

## Validation (valgrind, c-ares 1.18.1)

```
unpatched:            definitely lost: 74,320 bytes in 2 blocks
  - 73 (56+17) bytes  ->  ares_parse_srv_reply -> srv_callback (srv_mosq.c)   [bug 1]
  - 74,264 bytes      ->  ares_init -> mosquitto_connect_srv (srv_mosq.c)     [bug 2]

bug 1 fix only:       definitely lost: 74,264 bytes in 1 blocks   (channel remains)
bug 2 fix only:       definitely lost: 73 bytes                   (reply remains)
both fixes:           no SRV-related "definitely lost" blocks
```

## Fixes

`lib/srv_mosq.c` — free the reply once consumed (`reply` is NULL-initialised and
`ares_free_data(NULL)` is a no-op, safe on the parse-failure sub-branch):

```diff
 		if(status == ARES_SUCCESS){
 			mosquitto_connect(mosq, reply->host, reply->port, mosq->keepalive);
 		}
+		ares_free_data(reply);
 	}else{
```

`lib/libmosquitto.c` — destroy the channel in `mosquitto__destroy()` (mosq is
`calloc`'d, so `achan` is NULL when SRV was unused; `<ares.h>` is already
included under `WITH_SRV`):

```diff
 	if(!mosq){
 		return;
 	}
+#ifdef WITH_SRV
+	if(mosq->achan){
+		ares_destroy(mosq->achan);
+		mosq->achan = NULL;
+	}
+#endif
```

---

*Disclosure: these fixes were identified with the help of AI-assisted static
analysis and the description was drafted with AI assistance. Both defects were
reproduced and validated under valgrind (output above), and I have reviewed and
verified the changes myself and sign off on them under the DCO/ECA.*

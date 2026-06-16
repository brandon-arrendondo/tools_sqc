## Summary

`srv_callback()` in `lib/srv_mosq.c` leaks the `struct ares_srv_reply` list
returned by `ares_parse_srv_reply()` on every successful SRV lookup (WITH_SRV
builds).

## Bug

```c
if(status == ARES_SUCCESS){
    status = ares_parse_srv_reply(abuf, alen, &reply);
    if(status == ARES_SUCCESS){
        // FIXME - choose which answer to use based on rfc2782 page 3. */
        mosquitto_connect(mosq, reply->host, reply->port, mosq->keepalive);
    }
}else{
    ...
}
```

`ares_parse_srv_reply()` allocates a heap-resident linked list into `reply`,
which the c-ares API requires the caller to release with `ares_free_data()`.
`srv_callback()` reads `reply->host`/`reply->port` but never frees it, and
`ares_free_data` appears nowhere in the source tree (`grep -rn ares_free_data
lib/ src/ libcommon/` returns nothing). Every successful `mosquitto_connect_srv()`
lookup leaks the reply list.

## Steps to reproduce

1. Build with SRV support (`-DWITH_SRV=ON`, requires libc-ares).
2. Write a small client that calls `mosquitto_connect_srv(mosq, domain, ...)` for
   a domain that has MQTT SRV records (e.g. `_mqtt._tcp.<domain>`), then runs the
   loop so the resolver callback fires.
3. Run under valgrind:
   ```
   valgrind --leak-check=full --show-leak-kinds=all ./srv_client
   ```

## Validation (valgrind)

Captured with a `WITH_SRV` debug build (c-ares 1.18.1) and a live SRV record,
using `harness/srv_leak_repro.c` (a minimal client calling
`mosquitto_connect_srv()` then pumping the loop):

```
LD_LIBRARY_PATH=.../lib valgrind --leak-check=full --show-leak-kinds=all ./srv_leak_repro <domain>
```

**Before the fix** — a "definitely lost" record rooted exactly in the SRV callback:

```
73 (56 direct, 17 indirect) bytes in 1 blocks are definitely lost in loss record 3 of 5
   by 0x...: ares_parse_srv_reply (libcares.so.2.5.1)
   by 0x...: srv_callback (srv_mosq.c:46)
...
LEAK SUMMARY:
   definitely lost: 74,320 bytes in 2 blocks
```

**After the fix** — the `srv_callback`/`ares_parse_srv_reply` record is gone:

```
LEAK SUMMARY:
   definitely lost: 74,264 bytes in 1 blocks   # only the unrelated channel block remains
```

(Full logs: `harness/srv_valgrind_before.txt` / `srv_valgrind_after.txt`.)

> The remaining 74,264-byte "definitely lost" block is a **separate** issue —
> the c-ares channel `mosq->achan` (`ares_init`, `srv_mosq.c:75`) is never
> released with `ares_destroy()` anywhere in the tree. Reported separately; this
> PR fixes only the per-reply leak.

## Fix

```diff
 	if(status == ARES_SUCCESS){
 		status = ares_parse_srv_reply(abuf, alen, &reply);
 		if(status == ARES_SUCCESS){
 			// FIXME - choose which answer to use based on rfc2782 page 3. */
 			mosquitto_connect(mosq, reply->host, reply->port, mosq->keepalive);
 		}
+		ares_free_data(reply);
 	}else{
```

`reply` is initialised to `NULL` and `ares_free_data(NULL)` is a no-op, so the
free is safe even on the `ares_parse_srv_reply` failure sub-branch. The `else`
(lookup-failure) branch never allocates `reply`, so no free is needed there.

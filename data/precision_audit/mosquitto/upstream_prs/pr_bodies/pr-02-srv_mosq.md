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

Expected **before** the fix — a "definitely lost" record rooted in the c-ares
parse inside the SRV callback:

```
==NNNN== N bytes in M blocks are definitely lost ...
==NNNN==    by 0x...: ares_parse_srv_reply (...)
==NNNN==    by 0x...: srv_callback (srv_mosq.c:46)
```

**After** the fix: record gone.

> Note: requires a c-ares build and a resolvable SRV record (or a stubbed
> resolver). Captured run to be attached from a WITH_SRV build environment.

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

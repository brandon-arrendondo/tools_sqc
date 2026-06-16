## Summary

`mosquitto_connect_srv()` initialises a c-ares resolver channel
(`ares_init(&mosq->achan)`), but it is never released — `ares_destroy()` is
called nowhere in the library, and `mosquitto__destroy()` does not free
`mosq->achan`. Every SRV-based client leaks the entire c-ares channel (tens of
KB) until process exit.

> Found while validating the per-reply SRV leak fix (companion PR for
> `lib/srv_mosq.c`): valgrind showed a second, much larger "definitely lost"
> block rooted at `ares_init`.

## Bug

`lib/srv_mosq.c`:

```c
rc = ares_init(&mosq->achan);          /* channel allocated */
...
ares_search(mosq->achan, h, ns_c_in, ns_t_srv, srv_callback, mosq);
```

`mosq->achan` is used by the event loop (`lib/loop.c` `ares_fds`/`ares_process`)
but never destroyed. `grep -rn ares_destroy lib/ src/` returns nothing, and
`mosquitto__destroy()` (`lib/libmosquitto.c`) has no `achan` handling. The
channel and all its internal state leak for the lifetime of the process; a
long-running application that repeatedly creates/destroys SRV-connecting
`struct mosquitto` instances leaks one channel per instance.

## Steps to reproduce

Same harness as the per-reply leak (`harness/srv_leak_repro.c`), a `WITH_SRV`
build, and a resolvable `_mqtt._tcp.<domain>` SRV record:

```sh
LD_LIBRARY_PATH=.../lib valgrind --leak-check=full --show-leak-kinds=all ./srv_leak_repro <domain>
```

## Validation (valgrind)

**Before** (captured, c-ares 1.18.1):

```
74,395 (74,264 direct, 131 indirect) bytes in 1 blocks are definitely lost
   by 0x...: ares_init  (libcares)
   by 0x...: mosquitto_connect_srv (srv_mosq.c:75)
```

**After** this fix (`harness/srv_valgrind_channelfix_after.txt`): the 74 KB block
is gone (`definitely lost: 56 bytes` — only the separate per-reply leak remains,
which the companion `srv_mosq.c` PR removes). With both fixes applied, valgrind
reports no SRV-related "definitely lost" blocks.

## Fix

```diff
 void mosquitto__destroy(struct mosquitto *mosq)
 {
 	if(!mosq){
 		return;
 	}

+#ifdef WITH_SRV
+	if(mosq->achan){
+		ares_destroy(mosq->achan);
+		mosq->achan = NULL;
+	}
+#endif
+
 #ifdef WITH_THREADING
```

`mosq` is `calloc`'d in `mosquitto_new()`, so `achan` is `NULL` when SRV was
never used and the guard is safe. `<ares.h>` is already included by
`mosquitto_internal.h` under `WITH_SRV`.

## Relationship to the per-reply SRV leak PR

Independent and complementary (different files). The per-reply PR adds
`ares_free_data(reply)` in `srv_callback`; this PR destroys the channel in
`mosquitto__destroy`. Can be reviewed separately or together.

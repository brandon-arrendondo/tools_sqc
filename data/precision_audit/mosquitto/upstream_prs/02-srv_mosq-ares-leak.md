# lib/srv_mosq.c: c-ares SRV reply list leaked on every successful SRV lookup

- **File / function:** `lib/srv_mosq.c`, `srv_callback()`
- **Severity:** Low–Medium (unbounded-per-lookup heap leak; one allocation leaked per successful SRV resolution)
- **Class:** Memory leak (missing free of library-allocated resource)
- **Affected:** Confirmed present on mainline `d3dd4463`. `WITH_SRV` builds only (the entire body is inside `#ifdef WITH_SRV`). Reachable on a *normal*, successful SRV lookup — no error condition or malformed input required.
- **Prior art:** None found. No existing issue/PR, and `ares_free_data` does not appear anywhere in the source tree (verified below).

## Summary

When mosquitto is built with `WITH_SRV` (c-ares / libcares), `mosquitto_connect_srv()`
issues an asynchronous SRV query whose result is delivered to `srv_callback()`.
On `ARES_SUCCESS`, the callback calls `ares_parse_srv_reply(abuf, alen, &reply)`,
which **allocates** a heap-resident `struct ares_srv_reply` linked list and stores
its head in `reply`. The callback reads `reply->host` / `reply->port` and then
returns — without ever calling `ares_free_data(reply)`.

c-ares requires the caller to release every successfully parsed reply with
`ares_free_data()`. Because mosquitto never does, **every successful SRV lookup
leaks the entire parsed reply list.** This is a clean, normally-reachable leak,
not dependent on any error path.

## Root cause

`lib/srv_mosq.c` mainline `d3dd4463`, lines 38–56:

```c
static void srv_callback(void *arg, int status, int timeouts, unsigned char *abuf, int alen)
{
	struct mosquitto *mosq = arg;
	struct ares_srv_reply *reply = NULL;

	UNUSED(timeouts);

	if(status == ARES_SUCCESS){
		status = ares_parse_srv_reply(abuf, alen, &reply);
		if(status == ARES_SUCCESS){
			// FIXME - choose which answer to use based on rfc2782 page 3. */
			mosquitto_connect(mosq, reply->host, reply->port, mosq->keepalive);
		}
	}else{
		log__printf(mosq, MOSQ_LOG_ERR, "Error: SRV lookup failed (%d).", status);
		/* FIXME - calling on_disconnect here isn't correct. */
		callback__on_disconnect(mosq, MOSQ_ERR_LOOKUP, NULL);
	}
}
```

- **Line 46** — `ares_parse_srv_reply(abuf, alen, &reply)` allocates the reply list
  and writes the head pointer into `reply`. Per the c-ares contract, on
  `ARES_SUCCESS` the caller owns this allocation.
- **Line 49** — the reply is *consumed* (`reply->host`, `reply->port` read into
  `mosquitto_connect`), but the local `reply` pointer goes out of scope when the
  function returns at line 56. There is no corresponding free.
- There is **no `ares_free_data` call anywhere in the function**, and no cleanup at
  the call site or elsewhere.

Tree-wide confirmation that the reply is never freed (mainline `d3dd4463`):

```
$ grep -rn "ares_free_data" lib/ src/
$ grep -rn "ares_free_data" .            # (excluding .git)
$            # no matches — ares_free_data appears nowhere in the source tree
```

The only c-ares calls in `srv_mosq.c` are `ares_init`, `ares_search`, and
`ares_parse_srv_reply`; there is no matching deallocation:

```
$ grep -n "ares_" lib/srv_mosq.c
41:	struct ares_srv_reply *reply = NULL;
46:		status = ares_parse_srv_reply(abuf, alen, &reply);
75:	rc = ares_init(&mosq->achan);
99:	ares_search(mosq->achan, h, ns_c_in, ns_t_srv, srv_callback, mosq);
```

The identical code is present on the pinned tree `d3ee5c5c`, so this is not a
recent regression.

## Reproduction

This leak is **normally reachable** — it fires on any successful SRV resolution,
with no error or malformed-input requirement. The only practical prerequisite is a
DNS name that actually has SRV records (or a stub resolver that returns one), since
`ares_parse_srv_reply` must return `ARES_SUCCESS` with a non-empty reply for the
leaking branch (line 49) to execute.

Build with SRV support and run a tiny client under valgrind:

```sh
# Build the library with c-ares SRV support
cmake -B build -DWITH_SRV=ON ...     # requires libc-ares-dev / libcares
cmake --build build
```

Minimal client (`srv_leak.c`):

```c
#include <mosquitto.h>
#include <unistd.h>

int main(void)
{
	mosquitto_lib_init();
	struct mosquitto *m = mosquitto_new(NULL, true, NULL);

	/* A domain that publishes _mqtt._tcp SRV records.
	 * Substitute one you control, or point your resolver at a stub
	 * that answers _mqtt._tcp.<host> with an SRV record. */
	mosquitto_connect_srv(m, "example-with-srv.test", 60, NULL);

	/* Drive the c-ares event loop so srv_callback() actually fires. */
	for(int i = 0; i < 50; i++){
		mosquitto_loop(m, 100, 1);
	}

	mosquitto_destroy(m);
	mosquitto_lib_cleanup();
	return 0;
}
```

```sh
valgrind --leak-check=full --show-leak-kinds=all ./srv_leak
```

**Expected:** valgrind reports a definitely/indirectly-lost block allocated under
`ares_parse_srv_reply` (the `struct ares_srv_reply` list and its strdup'd `host`),
attributable to `srv_callback` in `lib/srv_mosq.c`. The leak occurs once per
successful SRV callback invocation.

**Honesty note:** the leaking branch only runs when c-ares returns a real SRV
answer. If `example-with-srv.test` does not resolve to an SRV record, the callback
takes the error branch (line 51–54) and nothing is leaked — so reproduction requires
a resolvable SRV record or a local stub resolver (e.g. dnsmasq with an
`--srv-host=_mqtt._tcp.<host>,target,1883` entry, pointed to via
`ares_set_servers` / `resolv.conf`). The defect itself is unconditional given a
successful parse; it is not an injection-only or attacker-only path.

## Suggested fix

Free the reply with `ares_free_data()` once it has been consumed. The allocation
only exists when `ares_parse_srv_reply` returned `ARES_SUCCESS`, and `reply` is
initialized to `NULL` (line 41), so freeing after the inner `if` is correct and safe
on the parse-failure sub-branch too (`ares_free_data(NULL)` is a no-op). Keep the
free inside the outer `ARES_SUCCESS` block — the error `else` branch never allocates.

```diff
--- a/lib/srv_mosq.c
+++ b/lib/srv_mosq.c
@@ -45,6 +45,7 @@ static void srv_callback(void *arg, int status, int timeouts, unsigned char *abu
 	if(status == ARES_SUCCESS){
 		status = ares_parse_srv_reply(abuf, alen, &reply);
 		if(status == ARES_SUCCESS){
 			// FIXME - choose which answer to use based on rfc2782 page 3. */
 			mosquitto_connect(mosq, reply->host, reply->port, mosq->keepalive);
 		}
+		ares_free_data(reply);
 	}else{
 		log__printf(mosq, MOSQ_LOG_ERR, "Error: SRV lookup failed (%d).", status);
 		/* FIXME - calling on_disconnect here isn't correct. */
 		callback__on_disconnect(mosq, MOSQ_ERR_LOOKUP, NULL);
 	}
 }
```

## Notes

- **c-ares ownership.** `struct ares_srv_reply` is allocated by c-ares with its own
  internal allocator and **must** be released with `ares_free_data()`, *not* `free()`.
  `ares_free_data` walks and frees the entire linked list (including the strdup'd
  `host` strings), so a single call on the head pointer is sufficient.
- **NULL-safe.** `reply` is initialized to `NULL` at line 41 and `ares_free_data(NULL)`
  is a documented no-op, so placing the free after the inner `if` covers the
  parse-failure sub-case (where `ares_parse_srv_reply` returns non-success and may
  leave `reply` `NULL`) without an extra guard.
- **Scope of the fix.** Placing the free inside the outer `if(status == ARES_SUCCESS)`
  block (not after the whole `if/else`) avoids touching `reply` on the error path,
  where nothing was ever allocated. The `else` branch is unaffected.
- This addresses only the per-reply leak. The pre-existing `// FIXME` comments
  (RFC 2782 answer selection; incorrect `on_disconnect` on lookup failure) are
  separate, out-of-scope issues and are intentionally left untouched.

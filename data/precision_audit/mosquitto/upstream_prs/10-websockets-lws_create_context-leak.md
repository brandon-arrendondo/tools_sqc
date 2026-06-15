# Memory leak in `mosq_websockets_init()` when `lws_create_context()` fails

- **File / function:** `src/websockets.c` — `mosq_websockets_init()`
- **Severity:** Low (startup-time leak; process typically aborts shortly after)
- **Class:** Memory leak, startup error path
- **Affected:** mainline (`d3dd4463`); builds with `WITH_WEBSOCKETS == WS_IS_LWS` (libwebsockets backend)
- **Prior art:** none found

## Summary

When `lws_create_context()` fails and returns `NULL`, `mosq_websockets_init()`
returns without freeing the two heap allocations it made earlier in the
function:

- `p` — the `struct lws_protocols` array (`mosquitto_calloc` at line 685),
  also published into `listener->ws_protocol` at line 755.
- `user` — the `struct libws_mqtt_hack` (`mosquitto_calloc` at line 731),
  and, when an `http_dir` is configured, `user->http_dir` (a `realpath()` /
  `_fullpath()` result allocated at lines 740/742).

On success, ownership of `user` transfers to the libwebsockets context (via
`info.user`) and is reclaimed by `lws_context_destroy()`, while `p` is freed by
`listeners__stop()` via `mosquitto_FREE(listener->ws_protocol)`. On the failure
path neither happens: the caller `listeners__main_loop()` sees a `NULL`
`ws_context` and returns `1` immediately (`src/listeners.c:284-287`), so
`listeners__stop()` is never reached. Even if it were, the
`lws_context_destroy()` call that would reclaim `user` is guarded by
`ws_context` being non-`NULL` (`src/listeners.c:319-320`), which it is not on
failure. The result is `user` (+ `user->http_dir`) and `p` are definitely lost.

## Root cause

The allocations, from MAINLINE `src/websockets.c`:

```c
685		p = mosquitto_calloc(protocol_count+1, sizeof(struct lws_protocols));
```

```c
731		user = mosquitto_calloc(1, sizeof(struct libws_mqtt_hack));
```

```c
738		if(listener->http_dir){
739	#ifdef WIN32
740			user->http_dir = _fullpath(NULL, listener->http_dir, 0);
741	#else
742			user->http_dir = realpath(listener->http_dir, NULL);
743	#endif
```

The failure path. `lws_create_context()` is called and its `NULL` return is
stored but never checked for cleanup; the function simply ends:

```c
753		info.user = user;
754		info.pt_serv_buf_size = WS_SERV_BUF_SIZE;
755		listener->ws_protocol = p;
756	
757		lws_set_log_level(conf->websockets_log_level, log_wrap);
758	
759		log__printf(NULL, MOSQ_LOG_INFO, "Opening websockets listen socket on port %d.", listener->port);
760		listener->ws_in_init = true;
761		listener->ws_context = lws_create_context(&info);
762		listener->ws_in_init = false;
763	}
```

There is no `if(!listener->ws_context){ ... }` block after line 761 to release
`p` / `user` / `user->http_dir` when context creation fails.

## Reproduction

`lws_create_context()` fails on bad listener configuration, an unusable bind
address/port, or resource exhaustion. The leak is **config-reachable**: a
websockets listener whose bind address cannot be honored (e.g. a `bind_address`
that does not resolve to a local interface, or an otherwise unusable
`iface`/`port` combination) drives `lws_create_context()` to return `NULL`. The
broker then logs `Error: Unable to create websockets listener on port N` and
exits, but the allocations made in `mosq_websockets_init()` are already lost.

Example config that forces context creation to fail (bind address not local):

```conf
# leak.conf
listener 8081
protocol websockets
http_dir /tmp        # exercises the user->http_dir realpath() allocation too
bind_address 192.0.2.1   # TEST-NET-1, not a local interface -> lws bind fails
```

valgrind recipe (build with libwebsockets backend):

```sh
valgrind --leak-check=full --show-leak-kinds=definite \
    ./src/mosquitto -c leak.conf
```

Expected: `definitely lost` blocks attributed to `mosquitto_calloc` called from
`mosq_websockets_init` — the `lws_protocols` array (`p`) and the
`libws_mqtt_hack` (`user`) — plus the `realpath()` allocation for
`user->http_dir` when `http_dir` is set.

If a given platform/libwebsockets version refuses to fail on the above config,
the path is reachable deterministically by injecting a failure: stub
`lws_create_context()` to return `NULL` (LD_PRELOAD shim or a test build),
start with any `protocol websockets` listener, and observe the same
`definitely lost` records. In that mode treat it as **injection-only** with the
shim as the harness.

## Suggested fix

Free the function-local allocations on the `lws_create_context()`-failure path.
`p` is reachable via `listener->ws_protocol`; reset that field too so a later
`listeners__stop()` does not double-free. `user->http_dir` comes from
`realpath()` / `_fullpath()`, so it is released with plain `free()` to match the
existing convention in `src/conf.c` (lines 1838/1842); `user` and `p` use
`mosquitto_FREE`.

```diff
--- a/src/websockets.c
+++ b/src/websockets.c
@@ -758,6 +758,15 @@ void mosq_websockets_init(struct mosquitto__listener *listener, const struct mos
 	log__printf(NULL, MOSQ_LOG_INFO, "Opening websockets listen socket on port %d.", listener->port);
 	listener->ws_in_init = true;
 	listener->ws_context = lws_create_context(&info);
 	listener->ws_in_init = false;
+
+	if(!listener->ws_context){
+		/* Context creation failed: lws never took ownership of info.user,
+		 * and listeners__stop() will not run, so free what we allocated. */
+		if(user->http_dir){
+			free(user->http_dir);
+		}
+		mosquitto_FREE(user);
+		listener->ws_protocol = NULL;
+		mosquitto_FREE(p);
+	}
 }
```

## Notes

- Confirmed identical at the pinned tree (`d3ee5c5c`, `src/websockets.c:761`);
  the surrounding allocation/cleanup logic matches mainline, so the same fix
  applies there.
- `mosquitto_FREE(A)` is `do{ mosquitto_free(A); (A)=NULL; }while(0)`
  (`include/mosquitto/libcommon_memory.h:75`), so the NULL-out of `user`/`p` is
  handled by the macro; `listener->ws_protocol` is reset explicitly.
- On the success path, `user` (including `user->http_dir`) is owned by the
  libwebsockets context and `p` is freed by `listeners__stop()`
  (`src/listeners.c:319-322`); this patch only affects the failure path and
  introduces no double-free.
- Separately, `user->http_dir` (the `realpath()` result) does not appear to be
  freed even on normal shutdown, since `lws_context_destroy()` frees the
  `info.user` block but is unaware of the nested `http_dir` pointer. That is a
  distinct potential leak outside the scope of this report and is not addressed
  by the patch above.
- The pre-existing OOM error paths at lines 732-736 and 744-749 already free
  `p` / `user` correctly; this change makes the post-context-creation failure
  path consistent with them.

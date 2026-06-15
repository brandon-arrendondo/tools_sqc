# Memory leak in `http_c__context_init()` on WebSocket-handshake error paths (`lib/http_client.c`)

- **File / function:** `lib/http_client.c` — `http_c__context_init()`
- **Severity:** Low (memory leak; per-connection, bounded by number of failed handshake attempts)
- **Class:** Memory leak (CWE-401)
- **Affected:** Confirmed present on mainline (`d3dd4463`). Reachable on the built-in WebSocket-client handshake setup error paths (`create_request_key()` / `ws__create_accept_key()` failure). Code is compiled only when `WITH_WEBSOCKETS == WS_IS_BUILTIN`.
- **Prior art:** None found.

## Summary

`http_c__context_init()` allocates `context->http_request` (via `mosquitto_calloc`) early, then performs two operations that can fail: `create_request_key()` and `ws__create_accept_key()`. On either failure the function returns immediately without freeing the live allocations:

- On `create_request_key()` failure: `context->http_request` is leaked.
- On `ws__create_accept_key()` failure: both `context->http_request` **and** the locally-allocated `key` are leaked.

The leaked `context->http_request` is *not* recovered by `http_c__context_cleanup()` in practice on these paths because the init failure aborts the connection setup; even if cleanup did run, the local `key` is unreachable from any struct field and is unconditionally lost.

## Root cause

Mainline `lib/http_client.c` (`d3dd4463`):

```c
46	int http_c__context_init(struct mosquitto *context)
47	{
48		struct mosquitto__packet *packet;
49		char *key;
50		const char *path;
51	
52		context->transport = mosq_t_http;
53		context->http_request = mosquitto_calloc(1, (size_t)context->wsd.http_header_size + 1);
54		if(context->http_request == NULL){
55			return MOSQ_ERR_NOMEM;
56		}
57	
58		if(create_request_key(&key)){
59			return MOSQ_ERR_UNKNOWN;            // <-- leaks context->http_request
60		}
61		if(ws__create_accept_key(key, strlen(key), &context->wsd.accept_key)){
62			return MOSQ_ERR_UNKNOWN;            // <-- leaks context->http_request AND key
63		}
64	
65		packet = mosquitto_calloc(1, sizeof(struct mosquitto__packet) + 1024 + WS_PACKET_OFFSET);
66		if(!packet){
67			return MOSQ_ERR_NOMEM;
68		}
```

Per early-return, exactly what is live and leaked (verified against the helper definitions):

**Line 59 — `create_request_key()` failed:**
- `context->http_request`: **live, leaked.**
- `key`: **NOT allocated.** `create_request_key()` (lines 38-43) is just `mosquitto_base64_encode(bytes, 16, encoded)`. In `mosquitto_base64_encode()` (`libcommon/base64_common.c:33`), `*encoded` is assigned only inside the success branch (`*encoded = mosquitto_malloc(...)`, line 52, with `rc` set to `0` only when that malloc succeeds, line 56). Every non-zero return leaves `*encoded`/`key` untouched. So on this path `key` is still an **uninitialized local** — it must NOT be freed. Only `context->http_request` leaks.

**Line 62 — `ws__create_accept_key()` failed:**
- This line is only reached after `create_request_key()` returned `0`, which (per the above) means `key` was successfully `mosquitto_malloc`'d and NUL-terminated. So `key`: **live, leaked.**
- `context->http_request`: **live, leaked.**
- `ws__create_accept_key()` itself (`lib/net_ws.c:349`) frees its own `EVP_MD_CTX` on all paths and only sets `*encoded` (i.e. `context->wsd.accept_key`) on full success, so it does not leak internally; on failure `context->wsd.accept_key` is untouched.

On the normal path, `key` is freed at line 81 (`mosquitto_FREE(key)`) after being interpolated into the request, and `context->http_request` is owned by the context and freed later by `http_c__context_cleanup()` (lines 89-94). The two early returns above are the only places these are dropped.

## Reproduction

These paths are not naturally reachable with valid inputs:

- `create_request_key()` wraps `mosquitto_getrandom()` + `mosquitto_base64_encode()`. The base64 encode fails only if an OpenSSL BIO allocation fails (`BIO_new` returns NULL) or the internal `mosquitto_malloc` for the output buffer fails (`base64_common.c:40-41`, `:52-53`).
- `ws__create_accept_key()` wraps OpenSSL SHA-1 digest ops (`EVP_MD_CTX_new`, `EVP_DigestInit/Update/Final`) plus a final `mosquitto_base64_encode()`. Failure requires `EVP_get_digestbyname("sha1")` returning NULL (SHA-1 unavailable in the OpenSSL provider config) or an `EVP_MD_CTX_new()`/digest-op failure — again essentially allocation/provider failure.

There is no attacker-controlled input on these paths; the inputs are locally generated random bytes and fixed strings. **Realistically this is reachable only under allocation/OpenSSL-internal failure → injection-only.** A SHA-1-disabled OpenSSL provider configuration could also force the `ws__create_accept_key()` path, but the dominant trigger is allocation failure.

### Injection-only harness (malloc fault injection)

The simplest deterministic trigger is to fail an allocation. `LD_PRELOAD` wrapper that fails the Nth allocation:

```c
/* faultmalloc.c — cc -shared -fPIC -ldl faultmalloc.c -o faultmalloc.so */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdlib.h>
#include <stdatomic.h>

static atomic_long count = 0;
static long fail_at = -1;   /* FAULT_AT env: fail the Nth malloc/calloc */

__attribute__((constructor)) static void init(void){
    const char *e = getenv("FAULT_AT");
    if(e) fail_at = atol(e);
}

void *malloc(size_t n){
    static void *(*real)(size_t) = NULL;
    if(!real) real = dlsym(RTLD_NEXT, "malloc");
    if(fail_at >= 0 && atomic_fetch_add(&count, 1) + 1 == fail_at) return NULL;
    return real(n);
}

void *calloc(size_t a, size_t b){
    static void *(*real)(size_t, size_t) = NULL;
    if(!real) real = dlsym(RTLD_NEXT, "calloc");
    if(fail_at >= 0 && atomic_fetch_add(&count, 1) + 1 == fail_at) return NULL;
    return real(a, b);
}
```

Drive a builtin-WebSocket client connection and sweep the fault index until it lands inside `mosquitto_base64_encode()`'s output `mosquitto_malloc` (forcing `create_request_key()` / `ws__create_accept_key()` to return non-zero) while `context->http_request` is already allocated:

```sh
cc -shared -fPIC -ldl faultmalloc.c -o faultmalloc.so
for n in $(seq 1 60); do
  echo "== FAULT_AT=$n =="
  FAULT_AT=$n LD_PRELOAD=./faultmalloc.so \
    valgrind --leak-check=full --error-exitcode=99 \
    ./your_ws_client_repro 2>&1 | grep -E "definitely lost|http_c__context_init"
done
```

Expected valgrind output on the indices that hit the `create_request_key` path (one block lost — `context->http_request`):

```
==NN== HEAP SUMMARY:
==NN==    definitely lost: <header_size+1> bytes in 1 blocks
==NN==    by 0x...: http_c__context_init (http_client.c:53)
```

And on the `ws__create_accept_key` path (two blocks lost — `context->http_request` + `key`):

```
==NN==    definitely lost: ... bytes in 2 blocks
==NN==    by 0x...: http_c__context_init (http_client.c:53)   /* http_request */
==NN==    by 0x...: mosquitto_base64_encode (base64_common.c:52)
==NN==    by 0x...: create_request_key (http_client.c:42)     /* key */
```

(With the fix applied, `definitely lost: 0 bytes` on every index.)

## Suggested fix

Free the live allocations before each early return. `key` is uninitialized on the `create_request_key()` failure path, so it must NOT be freed there. `mosquitto_FREE` already NULL-checks-safe and nulls the pointer, matching file style. Minimal, leak-targeted diff against MAINLINE:

```diff
--- a/lib/http_client.c
+++ b/lib/http_client.c
@@ -55,10 +55,12 @@ int http_c__context_init(struct mosquitto *context)
 	}
 
 	if(create_request_key(&key)){
+		mosquitto_FREE(context->http_request);
 		return MOSQ_ERR_UNKNOWN;
 	}
 	if(ws__create_accept_key(key, strlen(key), &context->wsd.accept_key)){
+		mosquitto_FREE(key);
+		mosquitto_FREE(context->http_request);
 		return MOSQ_ERR_UNKNOWN;
 	}
```

For completeness, the later `packet` allocation failure at line 66-68 also returns leaving `key` and `context->http_request` live (`key` allocated, `http_request` allocated) — though that is a separate `MOSQ_ERR_NOMEM` path. If the maintainers prefer a single cleanup site, an equivalent `goto error` restructure would cover all three returns:

```diff
--- a/lib/http_client.c
+++ b/lib/http_client.c
@@ -46,6 +46,7 @@ int http_c__context_init(struct mosquitto *context)
 int http_c__context_init(struct mosquitto *context)
 {
 	struct mosquitto__packet *packet;
-	char *key;
+	char *key = NULL;
 	const char *path;
 
 	context->transport = mosq_t_http;
@@ -55,15 +56,15 @@ int http_c__context_init(struct mosquitto *context)
 	}
 
 	if(create_request_key(&key)){
-		return MOSQ_ERR_UNKNOWN;
+		goto error;
 	}
 	if(ws__create_accept_key(key, strlen(key), &context->wsd.accept_key)){
-		return MOSQ_ERR_UNKNOWN;
+		goto error;
 	}
 
 	packet = mosquitto_calloc(1, sizeof(struct mosquitto__packet) + 1024 + WS_PACKET_OFFSET);
 	if(!packet){
-		return MOSQ_ERR_NOMEM;
+		goto error;
 	}
 
 	path = context->wsd.http_path?context->wsd.http_path:"/mqtt";
@@ -80,6 +81,11 @@ int http_c__context_init(struct mosquitto *context)
 	packet->to_process = packet->packet_length;
 	context->http_request[0] = '\0';
 	return packet__queue(context, packet);
+
+error:
+	mosquitto_FREE(key);
+	mosquitto_FREE(context->http_request);
+	return MOSQ_ERR_UNKNOWN;
 }
```

The `goto error` variant relies on `key` being initialized to `NULL` (so `mosquitto_FREE(key)` is a no-op on the `create_request_key` path) — included above. Either patch is correct; the minimal two-hunk diff is the smaller change and is sufficient to close the reported leak. (Note the `goto` variant changes the `packet` NOMEM return from `MOSQ_ERR_NOMEM` to `MOSQ_ERR_UNKNOWN`; if that matters, keep that return distinct.)

## Notes

- `key` is verified to be unallocated on the `create_request_key()` failure path (`mosquitto_base64_encode` assigns `*encoded` only on success); freeing it there would be use of an uninitialized pointer. The minimal patch correctly frees `key` only on the `ws__create_accept_key()` path.
- `ws__create_accept_key()` (`lib/net_ws.c:349`) and `mosquitto_base64_encode()` (`libcommon/base64_common.c:33`) do not leak internally on failure; the leak is purely in the caller's missing cleanup.
- The pinned tree (`d3ee5c5c`) contains a byte-identical `http_c__context_init()`; the analysis and patch apply unchanged to both.
- Impact is bounded (one or two small heap blocks per failed handshake) and only triggers under allocation/OpenSSL-provider failure, hence Low severity.

# `listener->socks` realloc-into-self leaks memory and orphans open socket fds on OOM

- **File / functions:** `src/net.c` — `net__socket_listen_tcp()` (mainline line 852) and `net__socket_listen_unix()` (mainline line 967)
- **Severity:** Low (injection-only / OOM-only; not remotely triggerable)
- **Class:** Memory leak + file-descriptor leak (CWE-401 / CWE-775), via the classic `p = realloc(p, …)` antipattern
- **Affected:** mainline `master` @ `d3dd4463` (and pinned `d3ee5c5c`, lines 856 / 971 there)
- **Prior art:** GitHub issue **#3412** "Memory Leaks Detected During Mosquitto Startup Failure" (CLOSED) — see relationship below. **This realloc-socks path was not fixed by the work that closed #3412.**

## Summary

Both listener-setup functions grow the `listener->socks` array with:

```c
listener->socks = mosquitto_realloc(listener->socks, …);
```

`mosquitto_realloc()` is a thin wrapper over `realloc()` (it returns `realloc(ptr, size)` directly — see `libcommon/memory_common.c:273`), so it follows standard C semantics: on failure it returns `NULL` **without freeing the original block**. By assigning the return value straight back onto `listener->socks`, the only pointer to the previously allocated array is overwritten with `NULL`. On OOM that block is leaked.

In the TCP function this is worse than a plain memory leak: the array being orphaned already holds **valid, open socket file descriptors** opened and (for earlier iterations) bound/listened on by prior loop iterations. When realloc fails on the 2nd-or-later iteration, those fds are no longer referenced anywhere and are never closed — the OOM cleanup path only closes the single most-recent `sock` (line 856), not the previously-stored fds whose array it just lost. So both heap memory and N socket fds leak.

## Root cause (quoted MAINLINE lines)

### `net__socket_listen_tcp()` — `src/net.c:851`–`858`

```c
		listener->sock_count++;
		listener->socks = mosquitto_realloc(listener->socks, sizeof(mosq_sock_t)*(size_t)listener->sock_count);
		if(!listener->socks){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
			freeaddrinfo(ainfo);
			COMPAT_CLOSE(sock);
			return MOSQ_ERR_NOMEM;
		}
		listener->socks[listener->sock_count-1] = sock;
```

This runs inside the `for(rp = ainfo; rp; rp = rp->ai_next)` loop (line 837). `listener->socks` is initialised to `NULL` at line 835 and grown one element per accepted address family. The fds it tracks were created at line 846 (`socket(...)`), and on prior iterations already bound (line 896) and `listen()`-ed (line 910). When the realloc at line 852 returns `NULL`:

1. `listener->socks` (the old block holding `sock_count-1` already-open fds) is overwritten with `NULL` → **memory leak**.
2. The error branch closes only the current `sock` (line 856), never the earlier fds → **fd leak** of every previously-opened socket in this listener.

### `net__socket_listen_unix()` — `src/net.c:966`–`973`

```c
	listener->sock_count++;
	listener->socks = mosquitto_realloc(listener->socks, sizeof(mosq_sock_t)*(size_t)listener->sock_count);
	if(!listener->socks){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		COMPAT_CLOSE(sock);
		return MOSQ_ERR_NOMEM;
	}
	listener->socks[listener->sock_count-1] = sock;
```

Same antipattern. The unix path is not a loop, but `listener->socks` may already be non-NULL (e.g. carried over / mixed listener setup), and `sock_count` is incremented before the realloc, so a failure here still overwrites and leaks any prior block while only closing the current `sock`.

For reference, `mosquitto_realloc` (`libcommon/memory_common.c:273`):

```c
BROKER_EXPORT void *mosquitto_realloc(void *ptr, size_t size)
{
	return realloc(ptr, size);
}
```

— confirming standard `realloc` failure semantics (old block survives, caller must keep a reference to free it).

## Reproduction

This is **OOM-only / injection-only**. There is no input an attacker (or even an operator) can supply over the network or via config to force `realloc()` to fail on a small allocation; it requires genuine memory exhaustion. It is therefore reproducible deterministically only via allocation-fault injection, not through normal operation. It is not a remotely-reachable vulnerability.

Relationship to #3412: that issue was filed for ASan leaks observed when the broker **fails to bind ports at startup**. The functions here are exactly the port-bind setup path. However, #3412's scenario triggers the *bind/listen failure* exits (lines 905–916 in `net__socket_listen_tcp`), which already `mosquitto_FREE(listener->socks)`. The defect documented here is on a *different* exit — the realloc-failure branch — which #3412's reproduction would not exercise unless allocation itself fails. Hence fault injection is required to demonstrate it.

### Fault-injection harness (LD_PRELOAD failing realloc)

Build the broker with ASan or run under Valgrind, configure two TCP listeners so the array realloc runs more than once, and fail the realloc that grows `listener->socks`.

`fail_socks_realloc.c`:

```c
/* cc -shared -fPIC -o fail_socks_realloc.so fail_socks_realloc.c -ldl
 *
 * Fails the Nth realloc() call (1-based), set via FAIL_REALLOC_NTH.
 * Use a small N so it lands on the listener->socks growth in net.c.
 * Run the broker with >=2 TCP listeners so net__socket_listen_tcp loops
 * and the second realloc (with a populated old block) is the one that fails,
 * orphaning the already-opened first socket fd.
 */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>

static void *(*real_realloc)(void *, size_t) = NULL;
static long counter = 0;
static long fail_nth = -1;

__attribute__((constructor))
static void init(void){
	const char *e = getenv("FAIL_REALLOC_NTH");
	fail_nth = e ? atol(e) : -1;
	real_realloc = dlsym(RTLD_NEXT, "realloc");
}

void *realloc(void *ptr, size_t size){
	if(!real_realloc) real_realloc = dlsym(RTLD_NEXT, "realloc");
	counter++;
	if(fail_nth > 0 && counter == fail_nth){
		fprintf(stderr, "[inject] failing realloc #%ld (size=%zu)\n", counter, size);
		return NULL; /* old block deliberately not freed */
	}
	return real_realloc(ptr, size);
}
```

`leak-repro.conf`:

```
listener 18831 127.0.0.1
listener 18832 127.0.0.1
```

Run (sweep `FAIL_REALLOC_NTH` to land on the socks growth; the exact index depends on prior startup allocations, so iterate over a small range and watch for the `[inject] failing realloc` line to coincide with `Error: Out of memory.` from `net.c`):

```sh
cc -shared -fPIC -o fail_socks_realloc.so fail_socks_realloc.c -ldl

for N in $(seq 1 60); do
  echo "=== FAIL_REALLOC_NTH=$N ==="
  FAIL_REALLOC_NTH=$N \
  LD_PRELOAD=./fail_socks_realloc.so \
  valgrind --leak-check=full --error-exitcode=99 \
    ./src/mosquitto -c leak-repro.conf 2>&1 | \
    grep -E "Out of memory|definitely lost|open file descriptor|inject"
done
```

(Equivalently, build mosquitto with `-fsanitize=address` and run under the LD_PRELOAD shim without Valgrind.)

**Expected:** on the injection that hits the line-852 realloc during the *second* loop iteration, the broker logs `Error: Out of memory.` and exits, and Valgrind/ASan report:

- a `definitely lost` heap block — the orphaned `listener->socks` array (`sizeof(mosq_sock_t) * (sock_count-1)`); and
- (Valgrind `--track-fds=yes`, or ASan/`lsof` on a non-exiting variant) the first listener socket fd opened at line 846 left **open** and unreferenced.

This is the same family of "leak on startup bind failure" symptom reported in #3412 — overlapping in cause area (listener setup error paths) but on the realloc-failure exit specifically, which the #3412 fix work (which landed in `src/conf.c`, not `src/net.c`) did not address.

### #3412 relationship — stated honestly

#3412 (ASan: ~760 bytes leaked across 4 allocations on port-bind failure) is **closed**, and mainline `d3dd4463` still contains the antipattern documented here. Investigation of the CHANGELOG and git history shows the realloc-leak hardening that the maintainer did around that time used the temp-pointer pattern but landed exclusively in `src/conf.c`:

- `97adeae8` "Fix leak on realloc failure." → `src/conf.c` (introduces `bridges_new` temp pointer)
- `d9ce9006` "Fix realloc leaks on failure" → `src/conf.c`
- `08115800` "Fix potential realloc leaks caused by errors on startup only" (Closes #3363) → `src/conf.c`

The `listener->socks` realloc lines in `src/net.c` were last modified by `b5c25cf1` ("Refactor memory functions to common static library", Mar 2024) and were **not** touched by any of the realloc-leak fixes. Conclusion: **#3412 either covered a different set of allocations (e.g. the conf-side bridge/listener config arrays) or was closed without covering this `net.c` path; the socks-realloc antipattern remains unfixed in mainline.** The fix below simply applies the maintainer's own established temp-pointer pattern (cf. `97adeae8`) to `net.c`.

## Suggested fix

Use a temporary pointer, only assign back to `listener->socks` on success, and `freeaddrinfo`/close as the surrounding branches already do. The patch below matches the existing brace/tab style of `src/net.c` and reuses the same cleanup already present on each error exit.

```diff
--- a/src/net.c
+++ b/src/net.c
@@ -849,13 +849,15 @@ static int net__socket_listen_tcp(struct mosquitto__listener *listener)
 			continue;
 		}
 		listener->sock_count++;
-		listener->socks = mosquitto_realloc(listener->socks, sizeof(mosq_sock_t)*(size_t)listener->sock_count);
-		if(!listener->socks){
+		mosq_sock_t *socks_new = mosquitto_realloc(listener->socks, sizeof(mosq_sock_t)*(size_t)listener->sock_count);
+		if(!socks_new){
 			log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
 			freeaddrinfo(ainfo);
 			COMPAT_CLOSE(sock);
+			mosquitto_FREE(listener->socks);
 			return MOSQ_ERR_NOMEM;
 		}
+		listener->socks = socks_new;
 		listener->socks[listener->sock_count-1] = sock;
 
 #ifndef WIN32
@@ -964,12 +966,14 @@ static int net__socket_listen_unix(struct mosquitto__listener *listener)
 		return 1;
 	}
 	listener->sock_count++;
-	listener->socks = mosquitto_realloc(listener->socks, sizeof(mosq_sock_t)*(size_t)listener->sock_count);
-	if(!listener->socks){
+	mosq_sock_t *socks_new = mosquitto_realloc(listener->socks, sizeof(mosq_sock_t)*(size_t)listener->sock_count);
+	if(!socks_new){
 		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
 		COMPAT_CLOSE(sock);
+		mosquitto_FREE(listener->socks);
 		return MOSQ_ERR_NOMEM;
 	}
+	listener->socks = socks_new;
 	listener->socks[listener->sock_count-1] = sock;
 
 
```

The added `mosquitto_FREE(listener->socks)` on the OOM branch frees the old (still-valid) array, mirroring the `mosquitto_FREE(listener->socks)` already used on the bind/listen/nonblock error exits at lines 873, 906 and 914. This recovers the heap block; the temp-pointer change is what prevents the pointer from being lost in the first place.

### Residual fd-leak note (optional hardening)

Freeing the array recovers the memory but does **not** close the earlier fds it tracked, because on the OOM exit those fds are no longer enumerable once the array is freed. A fully clean fix would, before freeing, close each `listener->socks[i]` (e.g. `for(i=0; i<sock_count-1; i++) COMPAT_CLOSE(listener->socks[i]);`). Since the process is exiting/aborting startup on OOM anyway, the OS reclaims the fds at exit — so the fd portion is benign in practice, but worth a comment if the maintainer prefers an explicit close loop on all `net.c` error exits for symmetry. The minimal correctness fix is the temp-pointer change above.

## Notes

- Verified directly against mainline `/home/brandon/data-enterprise/mosquitto-main` @ `d3dd4463`; line numbers (852 TCP, 967 unix) and surrounding cleanup branches quoted from source, not inferred.
- `mosquitto_realloc` confirmed to be a direct `realloc` wrapper (`libcommon/memory_common.c:273`), so standard non-freeing-on-NULL semantics apply.
- Trigger is genuine memory exhaustion only — not attacker-controllable, hence Low severity. The value of fixing it is correctness/Coverity-cleanliness and consistency with the maintainer's own recent `conf.c` realloc-leak fixes.
- This is the same temp-pointer pattern the maintainer introduced in `97adeae8`/`d9ce9006`/`08115800` for `src/conf.c`; the diff just extends it to the two `net.c` sites those commits missed.

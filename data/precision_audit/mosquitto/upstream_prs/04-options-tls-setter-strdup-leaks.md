# Memory leaks on `strdup`-failure (OOM) error paths in `lib/options.c` TLS setters

- **File / functions:** `lib/options.c` — `mosquitto_tls_set()`, `mosquitto_tls_psk_set()`
- **Severity:** Low (memory leak, OOM-only error path; not reachable on normal input)
- **Class:** Memory leak on error path (`MOSQ_ERR_NOMEM` return after partial allocation)
- **Affected (mainline):** `eclipse-mosquitto/mosquitto` @ `d3dd4463936143935607e8db0e21130e75b33d06`
  (also present unchanged in the `d3ee5c5c` pinned tree — long-standing, not a recent regression)
- **Prior art (distinct from this report):**
  - **#1116** (closed) reported *general* `mosquitto_connect` / `mosquitto_tls_set` TLS-related
    leaks observed under valgrind. It is a general/observational report, not the specific
    sequential-`strdup` OOM error-path family below.
  - **#2683** (closed PR, 2022) fixed a **multiple-call** leak in `mosquitto_tls_opts_set`
    (calling the setter twice leaked the previously stored value because the old value was
    not freed before re-assignment).
  - **Distinction:** Both of the above concern *normal-path* / repeated-call behavior. The
    issue reported here is an **allocation-failure (OOM) error path**: when an *intermediate*
    `mosquitto_strdup()` returns `NULL`, the function returns `MOSQ_ERR_NOMEM` **without
    freeing the earlier `strdup`'d siblings it already stored in this same call**. The
    `mosquitto_FREE` guards added by #2683 only handle re-assignment, not mid-sequence OOM
    bailout. So this family is not covered by either prior fix.

## Summary

`mosquitto_tls_set()` and `mosquitto_tls_psk_set()` each perform a *sequence* of
`mosquitto_strdup()` calls into distinct `mosq->tls_*` fields. If an early `strdup` in the
sequence succeeds but a later one fails, the function returns `MOSQ_ERR_NOMEM` immediately
and leaves the already-allocated earlier fields with no cleanup on that path. Because the
struct fields are not reset and the public API does not contract the caller to call
`mosquitto_destroy()` after a failed setter, those earlier allocations are leaked.

Note the asymmetry that makes this clearly a bug rather than a deliberate ownership choice:
in the *same* function the `MOSQ_ERR_INVAL` file-open-failure branches **do** free the
earlier fields (see lines 173–174 and 190–197 below), but the parallel `MOSQ_ERR_NOMEM`
strdup-failure branches do not.

## Root cause

### `mosquitto_tls_set()` (mainline lines 135–222)

The setter strdups `cafile`, then `capath`, then `certfile`, then `keyfile` into separate
fields, each guarded by its own `MOSQ_ERR_NOMEM` early return:

```c
152		mosq->tls_cafile = mosquitto_strdup(cafile);
153
154		if(!mosq->tls_cafile){
155			return MOSQ_ERR_NOMEM;
156		}
157	}
158
159	mosquitto_FREE(mosq->tls_capath);
160	if(capath){
161		mosq->tls_capath = mosquitto_strdup(capath);
162		if(!mosq->tls_capath){
163			return MOSQ_ERR_NOMEM;          /* LEAK: tls_cafile (line 152) not freed */
164		}
165	}
166
167	mosquitto_FREE(mosq->tls_certfile);
168	if(certfile){
169		fptr = mosquitto_fopen(certfile, "rt", false);
170		if(fptr){
171			fclose(fptr);
172		}else{
173			mosquitto_FREE(mosq->tls_cafile);   /* INVAL path DOES clean up... */
174			mosquitto_FREE(mosq->tls_capath);
175			return MOSQ_ERR_INVAL;
176		}
177		mosq->tls_certfile = mosquitto_strdup(certfile);
178		if(!mosq->tls_certfile){
179			return MOSQ_ERR_NOMEM;          /* LEAK: tls_cafile + tls_capath not freed */
180		}
181	}
182
183	mosquitto_FREE(mosq->tls_keyfile);
184	if(keyfile){
185		if(mosq->tls_keyform == mosq_k_pem){
186			fptr = mosquitto_fopen(keyfile, "rt", false);
187			if(fptr){
188				fclose(fptr);
189			}else{
190				mosquitto_FREE(mosq->tls_cafile);   /* INVAL path DOES clean up... */
191				mosq->tls_cafile = NULL;
192
193				mosquitto_FREE(mosq->tls_capath);
194				mosq->tls_capath = NULL;
195
196				mosquitto_FREE(mosq->tls_certfile);
197				mosq->tls_certfile = NULL;
198				return MOSQ_ERR_INVAL;
199			}
200		}
201		mosq->tls_keyfile = mosquitto_strdup(keyfile);
202		if(!mosq->tls_keyfile){
203			return MOSQ_ERR_NOMEM;          /* LEAK: tls_cafile + tls_capath + tls_certfile */
204		}
205	}
```

Leaking returns:
- **Line 163** (`tls_capath` strdup fails): leaks `tls_cafile` (allocated at 152).
- **Line 179** (`tls_certfile` strdup fails): leaks `tls_cafile` + `tls_capath`.
- **Line 203** (`tls_keyfile` strdup fails): leaks `tls_cafile` + `tls_capath` + `tls_certfile`.

The `MOSQ_ERR_INVAL` branches at 173–175 and 190–198 demonstrate the intended cleanup
contract; the `MOSQ_ERR_NOMEM` branches simply omit it.

### `mosquitto_tls_psk_set()` (mainline lines 425–464)

```c
436	mosq->tls_psk = mosquitto_strdup(psk);
437	if(!mosq->tls_psk){
438		return MOSQ_ERR_NOMEM;
439	}
440
441	mosq->tls_psk_identity = mosquitto_strdup(identity);
442	if(!mosq->tls_psk_identity){
443		mosquitto_FREE(mosq->tls_psk);       /* this one is handled correctly */
444		return MOSQ_ERR_NOMEM;
445	}
446	if(ciphers){
447		mosq->tls_ciphers = mosquitto_strdup(ciphers);
448		if(!mosq->tls_ciphers){
449			return MOSQ_ERR_NOMEM;           /* LEAK: tls_psk + tls_psk_identity not freed */
450		}
451	}else{
452		mosq->tls_ciphers = NULL;
453	}
```

The `tls_psk_identity`-failure path at 442–444 correctly frees `tls_psk`, which confirms the
expected discipline. But the `tls_ciphers`-failure path at **line 449** frees neither
`tls_psk` (line 436) nor `tls_psk_identity` (line 441) — both leak.

Underlying allocator: `mosquitto_strdup()` (`libcommon/memory_common.c:304`) routes through
`mosquitto_malloc()`, which is plain `malloc()` in the default (non-tracking) build, so a
`NULL` return is the genuine OOM condition.

## Reproduction

**These leaks trigger ONLY when an allocation fails** — i.e. when the 2nd/3rd `strdup` in the
sequence returns `NULL`. On any normal input with sufficient memory every `strdup` succeeds
and the function reaches `MOSQ_ERR_SUCCESS` with all fields owned by the struct. There is no
attacker-controlled or normal-operation path that reaches these returns. This is an
**injection-only / OOM-robustness** defect; reproduction requires forcing `malloc` to fail.

Below is a self-contained `LD_PRELOAD` fault-injection harness that fails the *Nth* `malloc`,
so we can target the 2nd `strdup` (`tls_capath`) in `mosquitto_tls_set()` and observe the
first (`tls_cafile`) leak.

`malloc_fault.c` — fail the Nth malloc (N from `FAIL_MALLOC_NTH`):

```c
/* cc -shared -fPIC -o malloc_fault.so malloc_fault.c -ldl */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>

static void *(*real_malloc)(size_t) = NULL;
static long counter = 0;
static long fail_at = -1;   /* 1-based index of the malloc to fail; -1 = never */

__attribute__((constructor))
static void init(void){
    real_malloc = dlsym(RTLD_NEXT, "malloc");
    const char *e = getenv("FAIL_MALLOC_NTH");
    if(e) fail_at = atol(e);
}

void *malloc(size_t size){
    if(!real_malloc) real_malloc = dlsym(RTLD_NEXT, "malloc");
    long n = __sync_add_and_fetch(&counter, 1);
    if(fail_at > 0 && n == fail_at){
        fprintf(stderr, "[fault] failing malloc #%ld (size=%zu)\n", n, size);
        return NULL;
    }
    return real_malloc(size);
}
```

`driver.c` — minimal client calling the setter on the OOM error path:

```c
/* cc -o driver driver.c -lmosquitto */
#include <mosquitto.h>
#include <stdio.h>

int main(void){
    mosquitto_lib_init();
    struct mosquitto *m = mosquitto_new("oom-test", true, NULL);

    /* cafile must be a path that opens (the setter fopen()-checks it);
     * capath only needs to be non-NULL to reach the second strdup. */
    int rc = mosquitto_tls_set(m, "/etc/ssl/certs/ca-certificates.crt",
                               "/etc/ssl/certs", NULL, NULL, NULL);
    printf("rc=%d (%s)\n", rc, mosquitto_strerror(rc));

    mosquitto_destroy(m);
    mosquitto_lib_cleanup();
    return 0;
}
```

Procedure:

```bash
# Build mosquitto with ASan (or just run the driver under valgrind).
cc -shared -fPIC -o malloc_fault.so malloc_fault.c -ldl
cc -o driver driver.c -lmosquitto

# 1. Find the malloc ordinal that corresponds to the tls_capath strdup by
#    sweeping FAIL_MALLOC_NTH and watching for rc=MOSQ_ERR_NOMEM (1) returned
#    *after* the cafile strdup already succeeded. (Earlier failures abort
#    before tls_cafile is allocated and leak nothing; you want the first N
#    that yields rc=1 with a prior successful tls_cafile allocation.)
for n in $(seq 1 40); do
  echo "== N=$n =="
  FAIL_MALLOC_NTH=$n LD_PRELOAD=./malloc_fault.so ./driver
done

# 2. Re-run that N under valgrind / ASan to confirm the leak:
FAIL_MALLOC_NTH=<N> LD_PRELOAD=./malloc_fault.so \
  valgrind --leak-check=full --error-exitcode=99 ./driver
```

Expected: when `FAIL_MALLOC_NTH` is set to the ordinal of the `tls_capath` strdup, the driver
prints `rc=1 (Out of memory.)` and valgrind reports the `tls_cafile` block (allocated inside
`mosquitto_strdup` called from `mosquitto_tls_set`) as **"definitely lost"**. Targeting the
`tls_certfile`/`tls_keyfile` strdups (pass `certfile`+`keyfile` to the driver and bump N)
multiplies the lost blocks. The analogous harness for `mosquitto_tls_psk_set()` (build with
PSK support, fail the `tls_ciphers` strdup) shows `tls_psk` + `tls_psk_identity` lost.

> If the library is built with `WITH_REAL_MEMORY_TRACKING`, you can instead drive failure via
> `mosquitto_memory_set_limit()` (`libcommon/memory_common.c:63`) set just above the cafile
> size and below the cumulative size — the tracked `mosquitto_malloc` returns `NULL` once the
> limit is exceeded, removing the need for `LD_PRELOAD`.

## Suggested fix

On each `MOSQ_ERR_NOMEM` strdup-failure path, free (and NULL) the `tls_*` fields already
populated in this call, mirroring the existing `MOSQ_ERR_INVAL` cleanup style already present
in `mosquitto_tls_set()`.

```diff
--- a/lib/options.c
+++ b/lib/options.c
@@ -159,6 +159,8 @@ int mosquitto_tls_set(struct mosquitto *mosq, const char *cafile, const char *ca
 	mosquitto_FREE(mosq->tls_capath);
 	if(capath){
 		mosq->tls_capath = mosquitto_strdup(capath);
 		if(!mosq->tls_capath){
+			mosquitto_FREE(mosq->tls_cafile);
 			return MOSQ_ERR_NOMEM;
 		}
 	}
@@ -174,6 +176,8 @@ int mosquitto_tls_set(struct mosquitto *mosq, const char *cafile, const char *ca
 			return MOSQ_ERR_INVAL;
 		}
 		mosq->tls_certfile = mosquitto_strdup(certfile);
 		if(!mosq->tls_certfile){
+			mosquitto_FREE(mosq->tls_cafile);
+			mosquitto_FREE(mosq->tls_capath);
 			return MOSQ_ERR_NOMEM;
 		}
 	}
@@ -200,6 +204,9 @@ int mosquitto_tls_set(struct mosquitto *mosq, const char *cafile, const char *ca
 		}
 		mosq->tls_keyfile = mosquitto_strdup(keyfile);
 		if(!mosq->tls_keyfile){
+			mosquitto_FREE(mosq->tls_cafile);
+			mosquitto_FREE(mosq->tls_capath);
+			mosquitto_FREE(mosq->tls_certfile);
 			return MOSQ_ERR_NOMEM;
 		}
 	}
@@ -445,6 +452,8 @@ int mosquitto_tls_psk_set(struct mosquitto *mosq, const char *psk, const char *i
 	if(ciphers){
 		mosq->tls_ciphers = mosquitto_strdup(ciphers);
 		if(!mosq->tls_ciphers){
+			mosquitto_FREE(mosq->tls_psk);
+			mosquitto_FREE(mosq->tls_psk_identity);
 			return MOSQ_ERR_NOMEM;
 		}
 	}else{
```

`mosquitto_FREE` already NULLs its argument (`#define mosquitto_FREE(A) do{ mosquitto_free(A);
(A) = NULL; }while(0)`, `include/mosquitto/libcommon_memory.h:75`), so this also leaves the
struct in a clean state. A `goto cleanup;` label collapsing the three `tls_set` returns would
also be acceptable and arguably tidier, but the inline frees above match the function's
existing in-place cleanup style most closely.

## Notes

- **Ownership:** the `tls_*` fields are owned by `struct mosquitto`. On the success path
  ownership is fine. On the OOM error paths, however, the fields are left populated yet the
  caller is handed `MOSQ_ERR_NOMEM` with no API contract requiring a subsequent
  `mosquitto_destroy()` — and even if the caller did destroy, the *intent* of these branches
  is clearly to roll back this call's allocations, as the sibling `MOSQ_ERR_INVAL` and the
  `tls_psk_identity` branches already do. The asymmetry is the bug.
- **Why error-path correctness matters even though it's OOM-only:** mosquitto is embedded in
  long-running brokers, gateways, and constrained/embedded clients where transient allocation
  failure is a real operating condition. A leak on the OOM path compounds memory pressure
  exactly when memory is already scarce, and it defeats clean-shutdown/restart accounting and
  leak-check CI. The fix is small, local, and consistent with the surrounding code.
- **Scope:** purely a leak on a non-reachable-by-input path; no behavior change on success or
  on the existing `MOSQ_ERR_INVAL` paths. Not a security vulnerability in the
  attacker-controlled sense.

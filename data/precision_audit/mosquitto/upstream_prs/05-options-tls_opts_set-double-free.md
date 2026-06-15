# `mosquitto_tls_opts_set()` "double-free / use-after-free on `tls_ciphers`" — FALSE FINDING (no bug)

- **File / function:** `lib/options.c`, `mosquitto_tls_opts_set()` (mainline `d3dd4463`, lines 225–289; the `tls_ciphers` handling is lines 252–278).
- **Severity:** None (not a defect). A minor *redundant allocation* (cosmetic / micro-inefficiency) is present, not a memory-safety issue.
- **Class:** Reported as CWE-415 (Double Free) / CWE-416 (Use After Free). **Not present.**
- **Affected:** All builds with `WITH_TLS`. Verified identical in PINNED (`d3ee5c5c`) and MAINLINE (`d3dd4463`).
- **Prior art:** The free-before-set lines (253, 259, 263) were introduced in commit `b5c25cf1d` (Roger A. Light, 2024-03-17), the descendant of the PR #2683 ("Fix memory leak in `mosquitto_tls_opts_set()`", 2022) free-before-reassign pattern. That change makes repeated calls non-leaking. It used the `mosquitto_FREE()` macro, which **frees and then NULLs the pointer**, so it is double-free-safe by construction. #2683's lineage fully resolved the leak *without* introducing a dangling pointer.

## Summary (verdict up front)

**This is NOT a double-free, NOT a use-after-free, and NOT a dangling-pointer-on-OOM bug. No fix needed.**

The audit's premise — "`tls_ciphers` is freed unconditionally then conditionally re-allocated, leaving a dangling pointer that a later free double-frees" — rests on the assumption that the free leaves a dangling pointer. It does not. Every free in this function goes through the `mosquitto_FREE(A)` macro, which is defined as:

```c
/* include/mosquitto/libcommon_memory.h:75 */
#define mosquitto_FREE(A) do{ mosquitto_free(A); (A) = NULL;}while(0)
```

So after **every** `mosquitto_FREE(mosq->tls_ciphers)`, the field is `NULL`. On the `strdup`-fails (OOM) path the field holds `strdup`'s return value, which is `NULL`. There is never a freed-but-non-NULL pointer to double-free or dereference. `mosquitto_free()` and the destroy-time `mosquitto_FREE` at `lib/libmosquitto.c:300` are both NULL-safe. The only real (cosmetic) observation is a **redundant strdup**: lines 253–254 allocate `tls_ciphers` and line 263 immediately frees it before lines 266–278 reallocate — wasted work, not a safety bug.

## Root cause — quoted MAINLINE lines + exact pointer-state trace

MAINLINE `lib/options.c` (`d3dd4463`):

```c
252	if(ciphers){
253		mosquitto_FREE(mosq->tls_ciphers);          // free old, tls_ciphers = NULL
254		mosq->tls_ciphers = mosquitto_strdup(ciphers);
255		if(!mosq->tls_ciphers){
256			return MOSQ_ERR_NOMEM;                   // tls_ciphers == NULL here
257		}
258	}else{
259		mosquitto_FREE(mosq->tls_ciphers);          // free old, tls_ciphers = NULL
260		mosq->tls_ciphers = NULL;
261	}
262
263	mosquitto_FREE(mosq->tls_ciphers);              // free whatever 253/254 set, -> NULL
264	mosquitto_FREE(mosq->tls_13_ciphers);           // free old 1.3 ciphers, -> NULL
265
266	if(ciphers){
267		if(!strcasecmp(mosq->tls_version, "tlsv1.3")){
268			mosq->tls_13_ciphers = mosquitto_strdup(ciphers);
269			if(!mosq->tls_13_ciphers){
270				return MOSQ_ERR_NOMEM;
271			}
272		}else{
273			mosq->tls_ciphers = mosquitto_strdup(ciphers);
274			if(!mosq->tls_ciphers){
275				return MOSQ_ERR_NOMEM;
276			}
277		}
278	}
```

Macro (`include/mosquitto/libcommon_memory.h:75`):
```c
#define mosquitto_FREE(A) do{ mosquitto_free(A); (A) = NULL;}while(0)
```
`mosquitto_free()` is NULL-safe (`libcommon/memory_common.c:233` `if(!mem){ return; }`; the untracked variant at line 281 calls `free()`, NULL-safe per C).

### State trace — case A: `ciphers != NULL`, `strdup` succeeds
- 253: free old `p0`; `tls_ciphers = NULL`.
- 254: `tls_ciphers = p1` (valid).
- 263: free `p1`; `tls_ciphers = NULL`. *(p1 is the redundant allocation; freed exactly once.)*
- 264: free old `tls_13_ciphers`; → NULL.
- 266–277: one of `tls_13_ciphers` / `tls_ciphers` = `p2` (valid). The other stays NULL.
- Final: every pointer freed once, no dangle. **Safe.**

### State trace — case B: `ciphers != NULL`, `strdup` at line 254 FAILS (OOM)
- 253: free old `p0`; `tls_ciphers = NULL`.
- 254: `strdup` returns NULL → `tls_ciphers = NULL`.
- 255–256: `return MOSQ_ERR_NOMEM` with `tls_ciphers == NULL`.
- Later `mosquitto_destroy` → `mosquitto_FREE(mosq->tls_ciphers)` (`libmosquitto.c:300`) frees NULL = no-op.
- **No dangling pointer, no double-free.** (`p0` was freed exactly once at 253; this is the *intended* leak-fix behavior — the old value is correctly released.)

### State trace — case C: `ciphers != NULL`, `strdup` at line 268/273 FAILS (OOM)
- By line 263/264 both `tls_ciphers` and `tls_13_ciphers` are NULL.
- 268 or 273 `strdup` returns NULL → the assigned field is NULL → `return MOSQ_ERR_NOMEM`.
- Final: both fields NULL. Destroy frees NULL = no-op. **Safe.**

### Multi-call path (the scenario #2683 targeted)
Second call with a different `ciphers`: line 253/259/263 free the previous allocation (set in a prior call's 268/273) and NULL it before reallocation. No leak, no double-free. **Safe.**

There is **no** code path on which `tls_ciphers` is freed and left pointing at released memory. The reported double-free/UAF cannot occur.

## Reproduction

**No reproduction — there is no defect.**

For completeness, a malloc-fault-injection harness forcing the line-254 or line-268/273 `strdup` to return NULL, followed by `mosquitto_destroy()`, under valgrind/ASan, produces **no** invalid-free / double-free report: the failing field is NULL and `mosquitto_FREE`/`free()` tolerate NULL. (I did not build/run this because the source trace is dispositive — every free path provably NULLs the pointer.)

## Suggested fix

**No fix needed** for memory safety.

Optional cleanup only (NOT a correctness change): lines 252–261 + 263 do redundant work — the `ciphers` branch strdups into `tls_ciphers` at 254 only to free it at 263 and reallocate at 266–278. The whole first `if/else` (252–261) can be reduced to freeing the old values once, since 263–278 already re-derive the correct field. This saves one allocation per call and removes confusing dead state, but the current code is fully correct. Not submitting as a bug.

## Notes

- Conclusion category: **(d) actually safe.** PR #2683's free-before-set pattern, as it lives today (commit `b5c25cf1d`), is double-free-safe because it is built on the `mosquitto_FREE` free-and-NULL macro rather than a bare `mosquitto_free`. The finding is a **false positive**.
- The likely source of the false positive: a static analyzer that models `free()` semantics but does not model the macro's trailing `(A) = NULL` assignment (or treats lines 253 and 263 as two frees of "the same" pointer without tracking the intervening NULL/reassignment). The redundant strdup at 254→263 can also look like free-of-fresh-allocation noise.
- PINNED (`d3ee5c5c`) and MAINLINE (`d3dd4463`) are byte-identical across lines 240–289; the verdict applies to both.
- Had the code used a bare `mosquitto_free(mosq->tls_ciphers)` without NULLing, case B/C *would* leave a dangling pointer and the destroy-time free would be a double-free — so the audit's mechanism is plausible in the abstract; it simply does not match this source.

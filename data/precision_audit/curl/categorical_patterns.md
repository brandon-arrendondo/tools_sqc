# Carried-over categorical FP/TP patterns (sqlite + mosquitto + libcrc audits)

Every adjudication subagent reads this first. These patterns recur across
codebases; curl is no exception. **Each finding is still adjudicated by reading
the actual source** — these are priors, not auto-verdicts. Verdict = TP / FP,
plus FN read-through of every file for bugs sqc missed.

## Curl-specific priors (task 158)

- **DCL13-C const-param** is the dominant FP class. Curl is callback/vtable
  heavy: hash dtors, `Curl_cwrite`/cfilter vtable impls, nghttp2/nghttp3/c-ares
  allocator & callback signatures are **fixed by an external function-pointer
  typedef** → const cannot be added → **FP**. Only a `static` helper that (a) is
  not assigned to any function pointer and (b) genuinely never writes through
  the pointer is a **TP** (low/medium confidence — curl does not follow this
  convention project-wide). The v0.4.22 sample confirms this split exactly
  (`hash.c:342`, `http2.c:2981`, `vtls/vtls.c:1633/1656` = FP;
  `cf-h1-proxy.c:83`, `multi_ev.c:137`, `pingpong.c:351` = TP).
- **INT32-C / INT30-C on loop counters & list lengths**: curl counts addrinfo
  lists, header lists, connection counts — all bounded far below INT_MAX, often
  guarded by a prior early-return. Decrement/increment on such counters = **FP**
  (v0.4.22 sample: `easy.c:264`, `hostip.c:487` = FP).
- **MEM30-C on `Curl_safefree(x)` / `free(x)` of distinct struct members**:
  `Curl_safefree` nulls its arg; sequential frees of *different* members
  (`->host`, `->path`, embedded structs) are not double-frees. free-then-strdup
  reassignment is not UAF (v0.4.22 sample: `doh.c:1334`, `ftp.c:3119`,
  `url.c:537`, `tool_operate.c:776` = FP).
- **STR34-C on `char**` double-pointer derefs** and `*p` where `p` is `char*`
  being read as a signed char: categorical FP (seen in mosquitto topic tok).
- **EXP34-C "passing NULL, callee may not check"** where the callee traverses
  with `while(p)`/`for(;p;p=p->next)` — NULL-safe (zero iterations) → FP. Also
  the `x = alloc(); if(!x) return; ... use x` pattern: once the guard passes,
  subsequent use is non-NULL → FP.
- **EXP33-C "used uninitialized"** from external-header macros sqc cannot expand
  (curl: `Curl_llist`/`Curl_hash` iteration macros, `DEBUGF`, `infof`,
  `Curl_dyn_*`) and from `#ifdef` branch-merging (assignment in one arm, use in
  same arm sqc doesn't connect) → FP.

## Cross-project FP patterns (verdict FP unless source contradicts)

1. **API05-C "conformant array syntax"**: advisory style on every `char *buf,
   size_t len` signature. Curl never uses C99 array-param syntax → FP by
   convention (but it IS an enabled rule, so each is recorded FP, not suppressed).
2. **API00-C on thin wrappers** (`return real_impl(...)`) that never deref the
   pointer themselves → FP. TP only when the exported fn derefs a param (e.g.
   through a lock) with no NULL check that siblings do have.
3. **EXP20-C `!strcmp`/`!memcmp`/`!strncmp` boolean idiom** in config/option
   dispatch → FP (idiomatic). curl `tool_getparam.c`, `url.c` option matching.
4. **EXP05-C false "cast away const" on `memcpy(&dst, src, sizeof(T))`**:
   `sizeof(type)` misparsed as a cast → FP (no cast present).
5. **MEM30-C/MEM12-C free→reassign→use**, **conditional free treated as
   unconditional**, **struct-field free propagated to whole struct**,
   **loop-iteration free conflation** (free of a *different* node each iter) →
   FP. (Shared root cause, task 181.)
6. **MEM12-C "leak" on struct-member allocations** (`x->buf = malloc()` freed
   later by a `*_cleanup`/`*_free` fn) → FP (not a stack-local leak).
7. **ARR00-C "uninitialized array"** when an out-param array is filled by a
   callee before the loop reads it → FP.
8. **FIO47-C/snprintf arg-position confusion** (size arg read as a format arg) → FP.
9. **DCL37-C reserved identifier** on `_GNU_SOURCE`/`__attribute__`/leading-`_`
   platform shims → FP.
10. **CON34-C duplicate of CON33-C** at the same `strerror()`/`getenv` site →
    count CON34 FP when CON33 already TP.
11. **MSC41-C false credential** on API constant names ("SECRET", "PIN", engine
    capability strings) → FP.
12. **PRE32-C on adjacent string-literal concatenation** across lines → FP.
13. **STR30-C "string literal may be modified"** on fns taking `const char*`
    that copy → FP (verify the prototype).

## Genuine-TP signals (read carefully, often real)

- **SIG30/31/34-C** async-signal-unsafe calls in real signal handlers (curl CLI
  `src/tool_main.c` / progress) — mosquitto signals.c was 75% TP.
- **CON33/ERR33-C** unchecked `strtol`/`time(NULL)`/`strerror` returns.
- **EXP45-C** assign-in-condition on optional symbol lookups.
- **DCL11-C** `%lu`/`%d` format vs argument-type mismatch (real on logging).
- **FN class — error-path resource leaks**: curl is FULL of `goto error`/
  `goto fail` cleanup ladders. Look hard for: an alloc whose free is skipped on
  one early-return/error branch; `realloc` overwriting the old pointer on NULL
  return (leak); strdup-cascade leaks in setopt functions; FILE*/socket fd not
  closed on an error return. These are the highest-value findings (sqc misses
  most of them). Confirm against `~/data-enterprise/curl-main` HEAD to see if
  still-present upstream (upstream-PR candidate).

## Output format (per subagent)

Return a JSON array; one object per finding:
`{"rule":..., "file":..., "line":..., "verdict":"TP|FP", "confidence":"high|med|low", "reason":"<=20 words"}`
Plus a separate `"fns"` array for missed bugs:
`{"file":..., "line":..., "rule":"<closest CERT rule>", "desc":..., "upstream_present":true|false}`
Plus a `"files_clean_for_fn"` list of files you read fully and found no FN.
</content>

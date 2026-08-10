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

## MEM31-C

Task 420 delta batch (`delta_mem31_b1.json`, 146 findings across 26 `lib/*.c`
files) adjudicated **100% FP (146/146)**. No TPs, no confirmed new FNs found
incidentally.

Batch 2 (`delta_mem31_b2.json`, 150 findings across 20 `lib/*.c` files, incl.
`ldap.c` 38 and `socks_gssapi.c` 40) also adjudicated **100% FP (150/150)**.
Same root causes as b1 dominate (status-typed local ~55%, struct-field/
container ownership-transfer ~30%), plus one **new** class:

- **Stale-taint-after-unconditional-free, propagated to every later return**:
  the flagged pointer/buffer (`service.value`/`gss_send_token.value` in
  `socks_gssapi.c:159/388/395`, `filename` in `tftp.c:697/705`) is freed via a
  single straight-line `Curl_safefree()`/`curlx_free()` call, then the
  function has many subsequent unrelated `if(...) return ERR;` branches
  (auth/negotiation error paths, unrelated `switch` cases). sqc keeps
  reporting the *already-freed* variable as leaked at every one of those
  downstream returns instead of clearing the "allocated, not yet freed"
  state once the unconditional free executes. Distinct from the b1
  mutually-exclusive-branch class: here the free and every flagged return are
  on the **same straight-line path**, not divergent branches. Hits:
  `socks_gssapi.c` (all 40, both `service.value` and `gss_send_token.value`),
  `socks_sspi.c:499/517` (same shape via CURLcode status locals), `tftp.c`
  (all 3), `telnet.c` (both, though there `result` is also just CURLcode),
  `setopt.c` (17 of 19 `return Curl_setstropt(...)` lines that never even
  touch the flagged `result` variable — same underlying over-broad
  function-wide taint span).

Root causes, in order of prevalence:

- **CURLcode/int status-typed local misattributed as a pointer allocation**
  (by far the dominant class, ~60% of findings): a local named `result`,
  `res`, `error`, or `rc` receives the return value of a function whose real
  name suggests an allocator (`Curl_open`, `Curl_urldecode`,
  `Curl_dynhds_add`, `Curl_headers_push`, `Curl_shuffle_addr`,
  `Curl_cf_http_proxy_insert_after`, `http2_cfilter_add/insert_after`,
  `ftp_parse_url_path`, `doh_probe_run`, `doh2ai`, `Curl_http_req_make2`,
  `Curl_getaddrinfo_ex`, `Curl_HMAC_init`-lookalikes) but **actually returns
  `CURLcode`/`int`**, not a pointer. Always check the callee's real declared
  return type before accepting the finding — grep `^CURLcode <fn>(` /
  `^int <fn>(` first. Examples: `lib/dict.c:184-262` (`result` vs the real
  allocation `path`), `lib/http2.c:2873-2943`, `lib/hostip6.c:107-117`
  (`error` is `int`), `lib/ftp.c:3986-4220`.
- **Connection-metadata-cache "_get" accessor pattern** (curl-specific,
  ~15 findings): `Curl_auth_{krb5,gsasl,ntlm,nego}_get(conn, ...)` allocate
  once and register the struct as connection metadata via
  `Curl_conn_meta_set(conn, KEY, ptr, xxx_conn_dtor)` (see
  `lib/vauth/vauth.c:160-224`); the connection owns it and frees it via the
  registered dtor on teardown. Never a local leak. Hits: `lib/curl_sasl.c`
  (krb5/gsasl/ntlm, 10 findings), `lib/http.c:3476/3492`,
  `lib/http_negotiate.c:86/174`.
- **Struct-field / cache-list ownership transfer via `_append`/`_add`-style
  APIs**: `Curl_llist_append`, `Curl_slist_append_nodup`,
  `Curl_bufref_set(..., destructor)`, assignment into a persistent struct
  field (`ftpc->prevpath`, `outcurl->cookies/asi/hsts`, `req->scheme`) —
  freed by the owning struct's cleanup/dtor, or by an explicit `curlx_free`
  on the error branch right next to the allocation. Examples:
  `lib/altsvc.c:578` (llist), `lib/bufref.c:131` (bufref dtor),
  `lib/cookie.c:1010/1583`, `lib/easy.c:1001/1038/1047`,
  `lib/formdata.c` (13 findings — the whole `FormInfo`/`curl_httppost` chain
  is walked and freed unconditionally at `formdata.c:580-584` regardless of
  success/failure, and `AddHttpPost`'s `post` is linked into the caller's
  out-params).
- **Alias-variable misattribution** (curl-specific, Windows-only
  `lib/curlx/fopen.c`): sqc names a `const TCHAR *target`/`target_oldpath`
  local (a plain pointer copy used only for later `CreateFile`/`_wstat`
  calls) as the allocation site, while the *actual* heap pointer
  (`filename_t`, `path_w`, `tchar_oldpath`) — declared a few lines earlier —
  is correctly freed via `curlx_free`/`CURLX_FREE`. Check which variable the
  allocating call's return value was actually assigned to, not just the name
  in the message. `lib/curlx/fopen.c:265/292/415/436/449/450/501`.
- **Mutually-exclusive branch / out-of-scope goto misattribution**: a `goto
  error`/`return` inside an `if`/`else if` branch that cannot execute on the
  same path where the flagged variable was assigned (`lib/hostip.c:955/
  978/981/989`), or a return statement lexically outside the flagged
  variable's block scope entirely (`lib/http.c:1193-1396` — `u` is scoped to
  `http.c:1140-1172` and unconditionally `curl_url_cleanup`'d at line 1165,
  but flagged again at returns 20-200 lines later where `u` isn't even in
  scope).
- **`Curl_dnscache_mk_entry`/`Curl_resolv_unlink` cleanup handshake**:
  `Curl_dnscache_mk_entry(data, &addr, ...)` always nulls/frees `*paddr`
  internally (success or failure, `hostip.c:601-606`); its own return value
  (`dns`) is either transferred to the caller via an out-param or released
  via `Curl_resolv_unlink(data, &dns)` on the error path. Do not flag `addr`
  or `dns` as leaked past this call without checking both halves of that
  contract (`lib/hostip.c:942-1011`, `lib/doh.c:1251-1289`).
- **Free-pairing API frees its own input pointer**: `Curl_HMAC_final(ctxt,
  output)` calls `curlx_free(ctxt)` internally (`lib/hmac.c:123`) — a
  same-named `_init`/`_final` pair where the "closing" call owns the
  free, analogous to the existing `macro_nulls_param_indices` class but at
  function-call granularity rather than macro. `lib/hmac.c:150`.

## Output format (per subagent)

Return a JSON array; one object per finding:
`{"rule":..., "file":..., "line":..., "verdict":"TP|FP", "confidence":"high|med|low", "reason":"<=20 words"}`
Plus a separate `"fns"` array for missed bugs:
`{"file":..., "line":..., "rule":"<closest CERT rule>", "desc":..., "upstream_present":true|false}`
Plus a `"files_clean_for_fn"` list of files you read fully and found no FN.
</content>

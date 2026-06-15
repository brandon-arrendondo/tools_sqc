# `size_t` underflow → OOB read on `in[inlen-1]` when `inlen == 0` in `http__canonical_filename()` (websockets HTTP file server)

- **File / function:** `src/websockets.c` — `http__canonical_filename()`
- **Severity:** Low — **defensive hardening only.** Not attacker-reachable through the built-in libwebsockets HTTP path (see reachability verdict below). Would become a remotely triggerable out-of-bounds read only if the `in` argument could ever be an empty string, which the libwebsockets request parser prevents.
- **Class:** `size_t` underflow (`inlen - 1` with `inlen == 0` → `SIZE_MAX`) → out-of-bounds read.
- **Affected:** mainline (`d3dd4463`); built `WITH_WEBSOCKETS` on the libwebsockets backend (`WITH_WEBSOCKETS == WS_IS_LWS`). The function and its sole caller live inside the `#if defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS` block, and HTTP file serving requires a listener configured with `http_dir`. (Identical code present in the pinned tree `d3ee5c5c`.)
- **Prior art:** none found.

## Summary

**Reachability verdict: NOT attacker-reachable via the supported code path — defensive hardening only.**

`http__canonical_filename()` computes `inlen = strlen(in)` and then immediately reads `in[inlen-1]`. Because `inlen` is `size_t`, if `in` were ever an empty string (`inlen == 0`), `inlen - 1` wraps to `SIZE_MAX` and `in[SIZE_MAX]` is an out-of-bounds read.

However, the only caller is mosquitto's libwebsockets HTTP callback `callback_http()` (the `LWS_CALLBACK_HTTP` reason), which passes the request URI that libwebsockets hands to it as `in`. libwebsockets' own request dispatcher (`lws_http_action()`) rejects any request whose URI does not begin with `'/'` (returning HTTP 403) **before** invoking the protocol's `LWS_CALLBACK_HTTP` callback. Consequently every `in` that reaches `http__canonical_filename()` begins with `'/'`, so `strlen(in) >= 1` and the `inlen - 1` subtraction cannot underflow in practice.

The defect is therefore a latent bug worth fixing for robustness (it depends on an invariant enforced by an external library rather than by mosquitto itself), but it is not a remotely exploitable OOB read in the shipped builtin path. The fix below is one defensive line.

## Root cause

From `src/websockets.c` (mainline `d3dd4463`), `http__canonical_filename()` — note the unconditional `in[inlen-1]` read at line 380 right after the `strlen` at line 379:

```c
371 static char *http__canonical_filename(
372 		struct lws *wsi,
373 		const char *in,
374 		const char *http_dir)
375 {
376 	size_t inlen, slen;
377 	char *filename, *filename_canonical;
378
379 	inlen = strlen(in);
380 	if(in[inlen-1] == '/'){
381 		slen = strlen(http_dir) + inlen + strlen("/index.html") + 2;
382 	}else{
383 		slen = strlen(http_dir) + inlen + 2;
384 	}
```

If `inlen == 0`, `inlen-1` is `SIZE_MAX` and `in[SIZE_MAX]` is read. (The same `inlen-1` index is read again at line 390 after allocation; both are gated on the assumption `inlen >= 1`.)

### Caller trace (proving `in` is non-empty in the supported path)

The only caller is `callback_http()` in the same file, under `case LWS_CALLBACK_HTTP`:

```c
477 			filename_canonical = http__canonical_filename(wsi, (char *)in, http_dir);
```

Here `in` is the `void *in` parameter of the libwebsockets protocol callback `callback_http()` (line 432). This callback is registered as `protocols[0]` — the mandatory HTTP handler:

```c
72 static struct lws_protocols protocols[] = {
73 	/* first protocol must always be HTTP handler */
74 	{
75 		"http-only",                        /* name */
76 		callback_http,                      /* lws_callback_function */
```

For the `LWS_CALLBACK_HTTP` reason, libwebsockets passes the request URI string as `in`. That URI is produced and validated inside libwebsockets' `lws_http_action()` (`lib/roles/http/server/server.c`), which obtains the URI via `lws_http_get_uri_and_method()` (`lws_hdr_simple_ptr(wsi, WSI_TOKEN_GET_URI)` etc.) and then, **before** dispatching to the protocol callback, performs:

```c
if (!uri_ptr || uri_ptr[0] != '/') {
    lwsl_err("LWS_HTTP_ACTION_START: bailing due to missing or non-absolute uri_ptr\n");
    lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, NULL);
    goto bail_nuke_ah;
}
```

Only after this gate does libwebsockets bind to `protocols[0]` (the "no mount hit" path mosquitto relies on) and call `wsi->a.protocol->callback(wsi, LWS_CALLBACK_HTTP, ..., uri_ptr, uri_len)`. Because the URI must begin with `'/'`, it is always at least one byte, so `strlen(in) >= 1` when `http__canonical_filename()` runs. A degenerate request such as `GET  HTTP/1.1` (empty path) is rejected with 403 and never reaches mosquitto.

The mosquitto-internal pre-checks before the call (POST forbidden at line 472; `http_dir` non-NULL at line 466) do not constrain the path contents and do not change this conclusion — the non-empty guarantee comes from libwebsockets.

Note: there is a *second*, unrelated `http__canonical_filename()` in `src/http_api.c` (line 70) belonging to the non-lws HTTP API; it has a different signature (`url, http_dir, &error_code`) and is out of scope for this report.

## Reproduction

This is **defensive-hardening only**; an attacker cannot drive `inlen == 0` through the libwebsockets HTTP server because libwebsockets rejects non-`'/'`-prefixed URIs with a 403 before the callback fires (caller trace above). A normal request such as `GET / HTTP/1.1` yields `in == "/"`, `inlen == 1`, and exercises the `'/'` branch safely; there is no client-supplied request that makes `in` an empty string.

To demonstrate the latent bug directly (not via the network), call the function — or just the offending expression — with a zero-length input under AddressSanitizer. Building the function in isolation requires the libwebsockets headers and stubs for `lws_return_http_status`/`mosquitto_malloc`; the minimal standalone reproduction of the underflow is:

```c
/* build: cc -fsanitize=address -g repro.c -o repro */
#include <string.h>
#include <stdlib.h>

static volatile char sink;

int main(void)
{
    /* heap-allocate an exactly-zero-length string so ASan has redzones */
    char *in = malloc(1);
    in[0] = '\0';                 /* empty string: strlen(in) == 0 */

    size_t inlen = strlen(in);    /* 0 */
    sink = in[inlen - 1];         /* in[SIZE_MAX] -> heap-buffer-overflow read */

    free(in);
    return 0;
}
```

Expected ASan output: `ERROR: AddressSanitizer: heap-buffer-overflow ... READ of size 1`, with the read address far outside the allocation (the `in[SIZE_MAX]` access), pointing at the `in[inlen-1]` line. This mirrors lines 380/390 of `http__canonical_filename()` once `inlen == 0`.

## Suggested fix

Guard against the empty-input case before the first `in[inlen-1]` read. This is a single defensive line that removes reliance on the external libwebsockets invariant. Returning `NULL` matches the function's existing error contract (the caller already handles `NULL` by returning `-1`), and emitting a 403 mirrors how libwebsockets itself treats a bad/empty URI.

```diff
--- a/src/websockets.c
+++ b/src/websockets.c
@@ -376,6 +376,11 @@ static char *http__canonical_filename(
 	size_t inlen, slen;
 	char *filename, *filename_canonical;
 
 	inlen = strlen(in);
+	if(inlen == 0){
+		/* Empty path: would underflow in[inlen-1]. lws normally rejects
+		 * non-'/' URIs before we get here, but guard defensively. */
+		lws_return_http_status(wsi, HTTP_STATUS_FORBIDDEN, NULL);
+		return NULL;
+	}
 	if(in[inlen-1] == '/'){
 		slen = strlen(http_dir) + inlen + strlen("/index.html") + 2;
 	}else{
```

## Notes

Security framing, kept proportional to actual reachability: this is a genuine `size_t` underflow that *would* be a remotely triggerable out-of-bounds read **if** `in` could be empty, but it currently cannot be — libwebsockets' `lws_http_action()` validates `uri_ptr[0] == '/'` and returns 403 before the `LWS_CALLBACK_HTTP` callback runs, so every URI reaching mosquitto is at least one byte. The bug is best characterized as defensive hardening: mosquitto is depending on an input invariant guaranteed by a third-party library rather than enforcing it locally. Should that libwebsockets guarantee ever change (or should the function be reused from another path), the underflow becomes live, so the one-line guard is worthwhile. It should not be reported as a high-severity remote OOB vulnerability against current mainline.

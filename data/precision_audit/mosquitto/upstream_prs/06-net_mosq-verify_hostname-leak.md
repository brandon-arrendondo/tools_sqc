# Resource leak: `net__socket_connect_step3` leaks SSL object + socket fd when `tls__set_verify_hostname()` fails

- **File / function:** `lib/net_mosq.c` — `net__socket_connect_step3()`
- **Severity:** Low (resource leak on an error path; not memory-corrupting, not remotely triggerable in normal operation)
- **Class:** Resource leak — leaked OpenSSL `SSL` object (`mosq->ssl`) **and** the connected socket file descriptor (`mosq->sock`)
- **Affected (mainline):** mosquitto `2.1.2`, commit `d3dd4463936143935607e8db0e21130e75b33d06`. Confirmed identical in the pinned tree at `d3ee5c5c`.
- **Prior art:** Issues #592 and #1116 (both closed, against much older releases) reported TLS-connect-time leaks in this general area. They predate the current `net__socket_connect_step3` structure and the `tls__set_verify_hostname` helper (the hostname-verification call was refactored out into `lib/tls_mosq.c`). This specific early-return path appears distinct from those reports and is present in current mainline.

## Summary

`net__socket_connect_step3()` builds up TLS state incrementally: it creates the `SSL` object, attaches ex-data, wires up a `BIO` over the already-connected socket, sets the SNI hostname, and then calls `tls__set_verify_hostname()` to configure peer-certificate hostname verification.

Every error branch in the function calls `net__socket_close(mosq)` before returning — **except** the `tls__set_verify_hostname()` branch, which returns `MOSQ_ERR_TLS` directly. At that point `mosq->ssl` (allocated via `SSL_new()`) and `mosq->sock` (the connected fd) are both live, so the early return leaks both: the `SSL` object is never `SSL_free()`d and the socket is never closed.

## Root cause

Mainline `lib/net_mosq.c`, `net__socket_connect_step3()` (lines 909–966). The `SSL` object is allocated at line 924 and the socket fd was already connected earlier (in `net__socket_connect` → `net__try_connect`). The buggy branch:

```c
909	int net__socket_connect_step3(struct mosquitto *mosq, const char *host)
910	{
...
924		mosq->ssl = SSL_new(mosq->ssl_ctx);     /* SSL object now live */
...
947		if(SSL_set_tlsext_host_name(mosq->ssl, host) != 1){
948			net__socket_close(mosq);            /* sibling: closes correctly */
949			return MOSQ_ERR_TLS;
950		}
951		if(tls__set_verify_hostname(mosq, host)){
952			return MOSQ_ERR_TLS;                /* BUG: no net__socket_close() */
953		}
954
955		if(net__socket_connect_tls(mosq)){
956			net__socket_close(mosq);            /* sibling: closes correctly */
957			return MOSQ_ERR_TLS;
958		}
```

Contrast the buggy return at **line 951–953** with its siblings in the same function, all of which clean up before returning the same `MOSQ_ERR_TLS`:

- line 926–928 (`SSL_new` failure) → `net__socket_close(mosq)`
- line 932–934 (`SSL_set_ex_data` failure) → `net__socket_close(mosq)`
- line 938–940 (`BIO_new_socket` failure) → `net__socket_close(mosq)`
- line 948–949 (`SSL_set_tlsext_host_name` failure) → `net__socket_close(mosq)`
- line 956–957 (`net__socket_connect_tls` failure) → `net__socket_close(mosq)`

`net__socket_close()` (lines 215–266) is exactly the cleanup that is missing: it `SSL_free()`s `mosq->ssl` and sets it `NULL` (lines 228–234), and `COMPAT_CLOSE()`s `mosq->sock`, resetting it to `INVALID_SOCKET` (lines 250–262). The verify-hostname branch is the lone error return that skips it — a clear, mechanical inconsistency rather than an intentional difference.

## Reproduction

**This path is injection-only / internal-failure, not reachable via a certificate/hostname mismatch.** Determined by reading `tls__set_verify_hostname()` in `lib/tls_mosq.c` (lines 53–91):

```c
53	int tls__set_verify_hostname(struct mosquitto *mosq, const char *hostname)
54	{
...
61		if(mosq->tls_insecure == true
62				|| (mosq->tls_cafile == NULL && mosq->tls_capath == NULL && mosq->tls_use_os_certs == false)){
63			return MOSQ_ERR_SUCCESS;
64		}
...
80		X509_VERIFY_PARAM *param = SSL_get0_param(mosq->ssl);
81		if(ipv4_ok || ipv6_ok){
82			rc = X509_VERIFY_PARAM_set1_ip_asc(param, hostname);
83		}else{
84			rc = X509_VERIFY_PARAM_set1_host(param, hostname, 0);
85		}
86		if(rc == 1){
87			return MOSQ_ERR_SUCCESS;
88		}else{
89			return MOSQ_ERR_TLS;          /* the failure that triggers the leak */
90		}
91	}
```

The function returns `MOSQ_ERR_TLS` **only** when `X509_VERIFY_PARAM_set1_host()` (or `..._set1_ip_asc()`) returns a value other than 1. Those OpenSSL calls merely *register* the expected name into the verify parameters; they do **not** perform certificate matching here (matching happens later during the handshake inside `net__socket_connect_tls()`, which has its own `net__socket_close()` cleanup). `X509_VERIFY_PARAM_set1_host()` fails essentially only on internal allocation failure or a malformed/embedded-NUL hostname string. So a normal cert/hostname mismatch does **not** reach this branch — it fails later, on the properly-cleaned-up path at line 955–958.

Because a real network peer cannot drive this branch, a black-box valgrind recipe against a mismatched-cert server will not exercise it (it exits via line 956 instead). The leak is reachable only under OOM or by injecting the failure. Harness to demonstrate it deterministically:

1. Build the client lib with a wrapper/stub that forces `X509_VERIFY_PARAM_set1_host` (or `tls__set_verify_hostname`) to return failure on the first call — e.g. an `--wrap=X509_VERIFY_PARAM_set1_host` linker wrap returning 0, or a one-line local edit making `tls__set_verify_hostname` return `MOSQ_ERR_TLS`.
2. Configure a TLS client with a CA file set and `tls_insecure` false (so the early-return guards at lines 61–62 do not short-circuit), pointing at any reachable TLS listener:

   ```c
   mosquitto_tls_set(mosq, "ca.crt", NULL, NULL, NULL, NULL);
   mosquitto_connect(mosq, "127.0.0.1", 8883, 60);   /* TCP connects, then step3 runs */
   ```
3. Run under valgrind:

   ```sh
   valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./repro_client
   ```
   Expected with the injected failure: a definitely/indirectly-lost block attributed to `SSL_new` (the un-freed `SSL` object) and a leaked file descriptor reported by `--track-fds=yes` (the un-closed `mosq->sock`). With the fix applied, both disappear.

For completeness, the mismatched-cert generation that a black-box tester *might* try (and which will exercise the correctly-cleaned line 955–958 path, not this bug):

```sh
openssl req -x509 -newkey rsa:2048 -keyout server.key -out server.crt -days 1 -nodes \
  -subj "/CN=wrong.example.com"            # CN/SAN deliberately not "127.0.0.1"
# run a TLS broker/server with server.crt, then connect a verifying client to 127.0.0.1
```

## Suggested fix

Add the missing `net__socket_close(mosq)` to the verify-hostname error branch so it matches every sibling error return in the function.

```diff
--- a/lib/net_mosq.c
+++ b/lib/net_mosq.c
@@ -949,6 +949,7 @@ int net__socket_connect_step3(struct mosquitto *mosq, const char *host)
 			return MOSQ_ERR_TLS;
 		}
 		if(tls__set_verify_hostname(mosq, host)){
+			net__socket_close(mosq);
 			return MOSQ_ERR_TLS;
 		}
 
```

This mirrors the immediately-preceding `SSL_set_tlsext_host_name` branch (lines 947–950) and the following `net__socket_connect_tls` branch (lines 955–958). No new error string is needed, since `tls__set_verify_hostname` does not emit one of its own here and the existing siblings that lack a `net__print_ssl_error` (e.g. lines 947–950, 955–958) set the precedent.

## Notes

- Verified by reading the full function in MAINLINE (`/home/brandon/data-enterprise/mosquitto-main/lib/net_mosq.c`, lines 909–966); the same code is byte-identical in the pinned tree (`/home/brandon/toolchain/mosquitto/lib/net_mosq.c`, lines 909–966).
- `net__socket_close()` is idempotent/safe to call here: it null-checks `mosq->ssl` before freeing and only closes the socket when `net__is_connected(mosq)` is true (lines 228–262), so the added call cannot double-free or double-close.
- Impact in practice is bounded: the trigger requires CA-backed verification configured *and* an OpenSSL internal failure in `X509_VERIFY_PARAM_set1_host`/`set1_ip_asc`. It is not reachable by an attacker presenting a mismatched certificate, so this is a correctness/robustness hardening fix (consistency with sibling error paths), not a directly exploitable condition.
- Line numbers cited are from mainline at `d3dd4463`; please re-anchor against the diff context (`tls__set_verify_hostname` call site) rather than absolute lines when applying.

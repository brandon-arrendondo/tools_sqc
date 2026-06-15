# NULL pointer dereference: unchecked `BIO_new()` in `set_username_from_cert_subject_name()` (client-cert TLS path)

- **File / function:** `src/handle_connect.c` — `set_username_from_cert_subject_name()`
- **Severity:** Crash / DoS (broker SIGSEGV)
- **Class:** NULL pointer dereference (unchecked allocation return value)
- **Affected:** mainline (`d3dd4463`), built `WITH_TLS`, listener configured with `use_subject_as_username true`; reached on a TLS client connection presenting a valid client certificate. (Also present in the pinned tree `d3ee5c5c`.)
- **Prior art:** none found.

## Summary

`set_username_from_cert_subject_name()` calls `BIO_new(BIO_s_mem())` and immediately passes the
returned BIO to `X509_NAME_print_ex()` and `BIO_get_mem_data()` without checking it for `NULL`.
`BIO_new()` returns `NULL` on allocation failure. Both `X509_NAME_print_ex()` and
`BIO_get_mem_data()` dereference the BIO pointer, so a `NULL` return leads to a NULL pointer
dereference inside OpenSSL and a broker crash.

This function runs while authenticating an incoming TLS client connection that presents a client
certificate (the `use_subject_as_username` listener option), so the crash occurs in the broker's
connection-handling path.

## Root cause

From `src/handle_connect.c` (mainline `d3dd4463`), the BIO is allocated at line 945 and used at
lines 946 and 948 with no intervening NULL check:

```c
931 static int set_username_from_cert_subject_name(struct mosquitto *context)
932 {
933 	X509 *client_cert = NULL;
934 	X509_NAME *name = NULL;
935
936 	if(get_client_cert_and_subject_name(context, &client_cert, &name)){
937 		return MOSQ_ERR_AUTH;
938 	}
939
940 	char *subject = NULL;
941 	char *data_start = NULL;
942 	BIO *subject_bio = NULL;
943 	long name_length = 0;
944
945 	subject_bio = BIO_new(BIO_s_mem());
946 	X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
947 	data_start = NULL;
948 	name_length = BIO_get_mem_data(subject_bio, &data_start);
949 	subject = mosquitto_malloc(sizeof(char)*(size_t)(name_length+1));
950 	if(!subject){
951 		return free_x509_and_BIO_and_send_connack_error(context, client_cert, subject_bio, MOSQ_ERR_NOMEM);
952 	}
```

If `BIO_new()` at line 945 returns `NULL`:

- Line 946: `X509_NAME_print_ex(subject_bio, ...)` dereferences `subject_bio` (writes the formatted
  name into the BIO) → NULL deref.
- Line 948: `BIO_get_mem_data(subject_bio, ...)` is a macro
  (`BIO_ctrl(subject_bio, BIO_CTRL_INFO, 0, ...)`) that dereferences the BIO → NULL deref.

The cleanup helper that already exists in the file tolerates a NULL BIO
(`free_x509_and_BIO_and_send_connack_error()` → `BIO_free(subject_name)`, and `BIO_free(NULL)` is a
documented no-op), so a NULL check can route directly into the existing cleanup path:

```c
876 inline static int free_x509_and_BIO_and_send_connack_error(struct mosquitto *context, X509 *client_cert,
877 		BIO *subject_name, int rc)
878 {
879 	BIO_free(subject_name);
880 	return free_x509_and_send_connack_error(context, client_cert, rc);
881 }
```

## Reproduction

This is **injection-only**. The dereference is reachable only when `BIO_new()` actually returns
`NULL`, i.e. under OpenSSL/`CRYPTO_malloc` allocation failure (OOM). It is **not** triggerable by
crafted certificate contents or normal network input — there is no attacker-controlled path that
forces `BIO_new()` to fail, so this is a robustness/OOM-hardening defect, not a remotely
attacker-triggered crash. It is reported because a NULL deref on allocation failure still produces
an undefined-behavior crash rather than a clean `MOSQ_ERR_NOMEM` rejection.

To demonstrate the crash, fault-inject the allocation that backs `BIO_new(BIO_s_mem())`. `BIO_new()`
obtains its `BIO` object via OpenSSL's `CRYPTO_malloc` (i.e. `OPENSSL_malloc`); failing that one
allocation returns `NULL`.

1. Build the broker `WITH_TLS` and configure a TLS listener with a client CA and
   `use_subject_as_username true`. Start the broker under a debugger or with ASan:

   ```
   listener 8883
   cafile   ca.crt
   certfile server.crt
   keyfile  server.key
   require_certificate true
   use_subject_as_username true
   ```

2. Build an `LD_PRELOAD` shim that fails the targeted allocation. Two approaches:

   - **Preferred — fail the OpenSSL allocator directly.** OpenSSL routes allocations through a
     replaceable hook; install one that returns `NULL` for the small `BIO` allocation while the
     connection is being authenticated:

     ```c
     /* biofail.c — LD_PRELOAD'd; arm the failure right before the cert path runs */
     #include <openssl/crypto.h>
     #include <stdlib.h>
     #include <stddef.h>

     static volatile int armed = 0;   /* flip via a signal/env once TLS handshake completes */

     static void *fail_malloc(size_t n, const char *f, int l){
         if(armed) return NULL;       /* refine: gate on n == sizeof(BIO) if too broad */
         return malloc(n);
     }
     static void *passthru_realloc(void *p, size_t n, const char *f, int l){ return realloc(p, n); }
     static void  passthru_free(void *p, const char *f, int l){ free(p); }

     __attribute__((constructor))
     static void init(void){
         CRYPTO_set_mem_functions(fail_malloc, passthru_realloc, passthru_free);
     }
     ```

     ```
     gcc -shared -fPIC biofail.c -o biofail.so -lcrypto
     LD_PRELOAD=./biofail.so ./src/mosquitto -c broker.conf
     ```

   - **Alternative — symbol interposition.** If OpenSSL is built to use libc `malloc` directly,
     interpose `malloc`/`CRYPTO_malloc` and return `NULL` for the allocation request that occurs
     between the start of TLS authentication and `X509_NAME_print_ex` (gate on size/count so the
     handshake itself still succeeds).

3. Connect a TLS client presenting a valid client certificate (e.g.
   `mosquitto_pub --cafile ca.crt --cert client.crt --key client.key -h localhost -p 8883 -t t -m x`)
   so execution reaches `set_username_from_cert_subject_name()` with the failure armed.

**Expected result:** with the fault armed, `BIO_new()` returns `NULL` and the broker crashes with
**SIGSEGV** at `src/handle_connect.c:946` (`X509_NAME_print_ex` writing into the NULL BIO), or ASan
reports a null-pointer dereference inside the OpenSSL `BIO` write path. Without the fix, there is no
clean error return; with the fix below, the broker instead sends a CONNACK error and frees the
certificate.

## Suggested fix

Add a NULL check immediately after `BIO_new()` and route through the existing
`free_x509_and_BIO_and_send_connack_error()` cleanup helper (which safely calls `BIO_free(NULL)` and
`X509_free()`), matching the file's existing error-handling style. Unified diff against mainline
`d3dd4463`:

```diff
--- a/src/handle_connect.c
+++ b/src/handle_connect.c
@@ -942,6 +942,9 @@ static int set_username_from_cert_subject_name(struct mosquitto *context)
 	long name_length = 0;
 
 	subject_bio = BIO_new(BIO_s_mem());
+	if(!subject_bio){
+		return free_x509_and_BIO_and_send_connack_error(context, client_cert, subject_bio, MOSQ_ERR_NOMEM);
+	}
 	X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
 	data_start = NULL;
 	name_length = BIO_get_mem_data(subject_bio, &data_start);
```

(Passing the NULL `subject_bio` to the cleanup helper is harmless — `BIO_free(NULL)` is a no-op —
and keeps the single-return-via-helper pattern already used at line 951. Alternatively, `goto` a
cleanup label or `X509_free(client_cert); return MOSQ_ERR_NOMEM;` directly; the helper form matches
the surrounding code most closely.)

## Notes

- The dereference is OOM-only and not reachable via crafted certificate/network input; severity is
  DoS-on-allocation-failure, not a remote-attacker crash. Reported for robustness parity with the
  other NOMEM handling in the same function (line 950).
- A structurally identical unchecked `BIO_new(BIO_s_mem())` → `X509_NAME_print_ex()` /
  `BIO_get_mem_data()` sequence exists in `src/security_default.c` (around lines 249–253); the same
  hardening should be applied there. It is out of scope for this report, which is scoped to
  `src/handle_connect.c`.
- `BIO_free(NULL)` is a documented OpenSSL no-op, so the suggested fix introduces no double-free or
  invalid-free risk.

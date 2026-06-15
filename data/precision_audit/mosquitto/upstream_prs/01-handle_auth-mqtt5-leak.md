# Heap leak of `auth_method` / `auth_data` in client-side `handle__auth()` on every MQTT5 AUTH packet

- **File / function**: `lib/handle_auth.c` — `handle__auth()`
- **Severity**: Medium (unbounded heap leak in the client library; one leak per received AUTH packet, attacker/peer-controlled frequency during re-authentication)
- **Class**: Memory leak
- **Affected**: Confirmed on mainline `d3dd4463` (also present unchanged on `d3ee5c5c`). Reachable on the **normal MQTT v5 extended-authentication protocol flow** — not an OOM-only / fault-injection path. Any MQTT5 client that registers an `on_ext_auth` callback and talks to a broker performing extended auth leaks on every received AUTH packet.
- **Prior art**: None found. `git log -- lib/handle_auth.c` shows no leak fix touching these reads; no `auth_method`/`auth_data` leak commit in history.

## Summary

The client-side `handle__auth()` in `lib/handle_auth.c` reads the MQTT5 `Authentication Method` (string) and `Authentication Data` (binary) properties via `mosquitto_property_read_string()` / `mosquitto_property_read_binary()`. Both functions allocate a fresh heap copy that the documentation says "must be free()'d by the application", but `handle__auth()` frees only the `properties` list and returns without freeing `auth_method` or `auth_data` on any path. Every AUTH packet received by an MQTT5 client therefore leaks one string allocation and one binary allocation. The broker-side counterpart (`src/handle_auth.c`) already frees both correctly, confirming the contract and the omission.

## Root cause

`lib/handle_auth.c`, the entire function (mainline `d3dd4463`):

```c
33  int handle__auth(struct mosquitto *mosq)
34  {
35  	int rc = 0;
36  	uint8_t reason_code;
37  	char *auth_method = NULL;
38  	void *auth_data = NULL;
39  	uint16_t auth_data_len = 0;
40  	mosquitto_property *properties = NULL;
...
63  	mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &auth_method, false);
64  	mosquitto_property_read_binary(properties, MQTT_PROP_AUTHENTICATION_DATA, &auth_data, &auth_data_len, false);
65  	rc = callback__on_ext_auth(mosq, auth_method, auth_data_len, auth_data, properties);
66  	mosquitto_property_free_all(&properties);
67  
68  	return rc;
69  }
```

- **Line 63** — `auth_method` is filled by `mosquitto_property_read_string()`. That function allocates the returned buffer (`libcommon/property_common.c:950`, `*value = mosquitto_calloc(1, (size_t)p->value.s.len+1)`), and its contract states (`include/mosquitto/libcommon_properties.h:417`): *"On success, value must be free()'d by the application."*
- **Line 64** — `auth_data` is filled by `mosquitto_property_read_binary()`, which likewise allocates (`libcommon/property_common.c:911`, `*value = mosquitto_calloc(1, *len + 1U)`); contract (`include/mosquitto/libcommon_properties.h:388`): *"On success, value must be free()'d by the application."*
- **Line 65** — `callback__on_ext_auth()` receives both as `const char *auth_method` / `const void *auth_data` (`lib/callbacks.c:314`). It only forwards them to the user `on_ext_auth` callback as const (`lib/callbacks.c:325`) and does **not** take ownership or free them. So ownership remains with `handle__auth()`.
- **Line 66** — only `properties` is freed (`mosquitto_property_free_all(&properties)`).
- **Line 68** — function returns. `auth_method` and `auth_data` go out of scope still pointing at live heap allocations → **two leaked blocks per AUTH packet**.

There are no other return paths between the reads and the return that could free them. Every successful `handle__auth()` invocation leaks both.

**Confirming the contract by comparison — the broker version is correct.** `src/handle_auth.c` (the broker's own `handle__auth()`) reads the same two properties and *does* free them:

```c
101  		mosquitto_FREE(auth_method);          /* non-matching-method path */
108  	mosquitto_FREE(auth_method);          /* matched path */
...
128  	mosquitto_FREE(auth_data);
```

This is direct evidence in-tree that these buffers are caller-owned and must be freed — the client-library copy simply omits it.

## Reproduction

This is **normally reachable** — no fault injection. An MQTT5 client registers `on_ext_auth`, connects to a broker that drives extended authentication, the broker sends an AUTH (`CMD_AUTH`) packet carrying `Authentication Method` + `Authentication Data`, and the client's `handle__auth()` runs lines 63-66 and leaks.

### Cleanest path: the existing lib test under valgrind (recommended)

The repository already ships a test that exercises this exact path, and the test harness already runs clients under valgrind with leak checking — so no new infrastructure is needed.

- Client: `test/lib/c/01-extended-auth-continue.c` registers `on_ext_auth` and connects with protocol version 5.
- Driver: `test/lib/01-extended-auth-continue.py` (a fake broker) sends, at line 13, an AUTH packet (`reason_code=CONTINUE_AUTHENTICATION`) carrying both `AUTHENTICATION_METHOD = "test-method"` and `AUTHENTICATION_DATA = "test-request"`. This packet is delivered to the client and processed by `lib/handle_auth.c:handle__auth()`, hitting lines 63-64 and leaking both buffers.
- Harness: `test/mosq_test.py:148` launches the client under
  `valgrind -q --gen-suppressions=all --suppressions=test.supp --track-fds=yes --trace-children=yes --leak-check=full --show-leak-kinds=all`.

Recipe:

```bash
# build with tests enabled (valgrind invoked by the lib test harness)
cmake -B build -DWITH_TESTS=ON
cmake --build build

# run the extended-auth lib test under valgrind
cd test/lib
make 01-extended-auth-continue.py      # or run the .py directly with mosq_test under valgrind
# Expected valgrind output (on the client process), e.g.:
#   13 bytes in 1 blocks are definitely lost ... (auth_method: "test-method" + NUL)
#     by ... mosquitto_property_read_string
#     by ... handle__auth (handle_auth.c:63)
#   13 bytes in 1 blocks are definitely lost ... (auth_data: "test-request" + NUL)
#     by ... mosquitto_property_read_binary
#     by ... handle__auth (handle_auth.c:64)
```

After applying the fix below, both "definitely lost" blocks disappear. Note: today `01-extended-auth-continue.c`'s `on_ext_auth` calls `mosquitto_ext_auth_continue(...)` and the broker drives a second AUTH→CONNACK, so the leak is observable per AUTH packet received; under the failure variant (`01-extended-auth-failure`) it leaks once. No test source change is required to surface the leak — only running the existing test under the existing valgrind harness.

### Manual broker recipe (alternative)

Build the broker with an auth plugin that performs extended auth (returns `MOSQ_ERR_AUTH_CONTINUE`), connect any MQTT5 client doing extended auth, and run the **client** under
`valgrind --leak-check=full --show-leak-kinds=all`. Each AUTH packet the client receives produces the two "definitely lost" entries attributed to `handle__auth (handle_auth.c:63)` and `(handle_auth.c:64)`. The plugin/auth setup is the only nontrivial part; the lib-test path above avoids it entirely.

## Suggested fix

Free both buffers after the callback, alongside the existing `properties` free, using the same `mosquitto_FREE` idiom already used by the broker counterpart and throughout `lib/`. `mosquitto_FREE` (`include/mosquitto/libcommon_memory.h:75`) maps to `mosquitto_free`, the correct deallocator for the `mosquitto_calloc` allocations made by the read functions.

```diff
--- a/lib/handle_auth.c
+++ b/lib/handle_auth.c
@@ -63,7 +63,9 @@ int handle__auth(struct mosquitto *mosq)
 	mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &auth_method, false);
 	mosquitto_property_read_binary(properties, MQTT_PROP_AUTHENTICATION_DATA, &auth_data, &auth_data_len, false);
 	rc = callback__on_ext_auth(mosq, auth_method, auth_data_len, auth_data, properties);
+	mosquitto_FREE(auth_method);
+	mosquitto_FREE(auth_data);
 	mosquitto_property_free_all(&properties);
 
 	return rc;
 }
```

The free is placed after `callback__on_ext_auth()` returns, which is safe because the callback receives the pointers as `const` and does not retain them. The single combined return path means no other call sites need changes.

## Notes

- **Ownership convention**: `mosquitto_property_read_string()` and `mosquitto_property_read_binary()` always return freshly `mosquitto_calloc`'d buffers on success; the documented contract ("value must be free()'d by the application", `libcommon_properties.h:388` and `:417`) places ownership on the caller. `callback__on_ext_auth()` (`lib/callbacks.c:314-329`) does not take ownership — it forwards as const and returns.
- **Edge cases**: When the property is absent or has zero length, the read functions set `*value = NULL` (`property_common.c:917-919`, `:956-958`), so `mosquitto_FREE(NULL)` is a no-op — the fix is safe whether or not the properties are present. `mosquitto_FREE` also nulls the pointer, matching the file's style.
- **Scope**: This is the **client/library** `handle__auth()` (`lib/handle_auth.c`). The broker's own `handle__auth()` (`src/handle_auth.c`) is a separate implementation that already frees both buffers (lines 101/108 for `auth_method`, line 128 for `auth_data`) and is **not** affected.

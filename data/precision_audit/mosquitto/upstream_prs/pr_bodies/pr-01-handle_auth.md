## Summary

`handle__auth()` in `lib/handle_auth.c` leaks the MQTT v5 Authentication Method
and Authentication Data on every AUTH packet the client library processes.

## Bug

`handle__auth()` reads two properties:

```c
mosquitto_property_read_string(properties, MQTT_PROP_AUTHENTICATION_METHOD, &auth_method, false);
mosquitto_property_read_binary(properties, MQTT_PROP_AUTHENTICATION_DATA, &auth_data, &auth_data_len, false);
rc = callback__on_ext_auth(mosq, auth_method, auth_data_len, auth_data, properties);
mosquitto_property_free_all(&properties);
return rc;
```

Both `mosquitto_property_read_string()` and `mosquitto_property_read_binary()`
document that the returned value **must be freed by the caller** (the `false`
final argument means "allocate a copy"). Neither `auth_method` nor `auth_data`
is freed before returning, so both leak on every MQTT v5 extended-authentication
(AUTH) exchange. `callback__on_ext_auth()` receives them as `const` and does not
take ownership.

## Steps to reproduce

1. Build the client library with TLS (extended auth requires the AUTH packet path).
2. Connect an MQTT v5 client that performs extended authentication (sets an
   authentication method via `MOSQ_OPT_*` / an `on_ext_auth` callback) to a
   broker that responds with an `AUTH` packet, so `handle__auth()` runs.
3. Run the client under valgrind:
   ```
   valgrind --leak-check=full --show-leak-kinds=all ./your_mqtt5_auth_client
   ```

## Validation (valgrind)

Expected **before** the fix — two `definitely lost` records originating in
`handle__auth` via the property read functions:

```
==NNNN== N bytes in 1 blocks are definitely lost in loss record ...
==NNNN==    by 0x...: mosquitto_property_read_string (...)
==NNNN==    by 0x...: handle__auth (handle_auth.c:63)
==NNNN== M bytes in 1 blocks are definitely lost in loss record ...
==NNNN==    by 0x...: mosquitto_property_read_binary (...)
==NNNN==    by 0x...: handle__auth (handle_auth.c:64)
```

**After** the fix: both records gone, "All heap blocks were freed" for that path.

> Note: this path requires an MQTT v5 broker peer performing extended auth; the
> leak is on the normal protocol path, not an OOM/error path.

## Fix

```diff
 	rc = callback__on_ext_auth(mosq, auth_method, auth_data_len, auth_data, properties);
+	mosquitto_FREE(auth_method);
+	mosquitto_FREE(auth_data);
 	mosquitto_property_free_all(&properties);
```

`mosquitto_FREE` is the matching deallocator (it maps to `mosquitto_free`) and is
the idiom already used by the broker-side handler. Single return path, so no
other call sites change.

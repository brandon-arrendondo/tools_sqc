## Summary

When a persistence-database write fails partway through, the broker calls
`fclose()` on the same `FILE*` twice — once in the `persist__write_data()`
write-callback error path and again in `mosquitto_write_file()`'s error path.
A second `fclose()` on an already-closed stream is undefined behaviour (C11
7.21.3p4) and triggers glibc "double free or corruption" / a crash in practice.

## Bug

`mosquitto_write_file()` (`libcommon/file_common.c`) owns the stream lifecycle:
it opens `<path>.new`, invokes the `write_fn` callback, and on **any** error
closes the stream and unlinks the temp file at its own `error:` label:

```c
fptr = mosquitto_fopen(tmp_file_path, "wb", restrict_read);
...
if((rc = (*write_fn)(fptr, user_data)) != MOSQ_ERR_SUCCESS){
    goto error;          /* error: -> fclose(fptr); unlink(tmp_file_path); */
}
```

But the persistence callback `persist__write_data()` (`src/persist_write.c`)
*also* closes the stream in its error path:

```c
error:
    err = strerror(errno);
    log__printf(NULL, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", ...);
    if(db_fptr){
        fclose(db_fptr);   /* <-- first close */
    }
    return rc;
```

So a failed write (e.g. ENOSPC during `write_e`) closes `fptr` here, returns an
error, and `mosquitto_write_file()` then closes the same `fptr` again — double
`fclose`. The sibling callback `dynsec__write_json_config()` does **not** close
the stream, confirming the intended contract is "callback returns an error code,
caller owns and closes the stream."

## Steps to reproduce

The error path triggers when the persistence write fails after the temp file is
open. A clean, privilege-free way to force that is an `LD_PRELOAD` shim that fails
`fwrite()` to any fd whose path ends in `.new` (the persistence temp file), and a
deterministic save trigger.

`harness/fwrite_fail_dotnew.c` (in this directory):

```c
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dlfcn.h>
static size_t (*real_fwrite)(const void*, size_t, size_t, FILE*) = NULL;
size_t fwrite(const void *ptr, size_t size, size_t n, FILE *stream){
    if(!real_fwrite) real_fwrite = dlsym(RTLD_NEXT, "fwrite");
    int fd = stream ? fileno(stream) : -1;
    if(fd >= 0){
        char lp[64], path[1024];
        snprintf(lp, sizeof lp, "/proc/self/fd/%d", fd);
        ssize_t r = readlink(lp, path, sizeof(path)-1);
        if(r > 4 && !strcmp(path + r - 4, ".new")){ errno = ENOSPC; return 0; }
    }
    return real_fwrite(ptr, size, n, stream);
}
```

```sh
gcc -shared -fPIC -o inject.so fwrite_fail_dotnew.c -ldl

# mosquitto.conf: persistence true; persistence_location <dir>/; a listener.
# Deterministic save trigger: a broker exit while persistence has state —
# e.g. send a retained publish then stop the broker, which runs
# persist__backup() -> persist__write_data() on the way out.
LD_PRELOAD=./inject.so valgrind --track-fds=yes --leak-check=no \
    ./mosquitto -c mosquitto.conf
```

## Validation (valgrind / glibc)

With the injected write failure, the save path hits `persist__write_data`'s
`error:` block (first `fclose`) then `mosquitto_write_file`'s `error:` block
(second `fclose` on the same stream). Expected evidence:

- Under **glibc** (no valgrind): abort with `free(): double free detected` /
  `double free or corruption`, since `fclose` frees the `FILE` object.
- Under **valgrind `--track-fds=yes`**: an *Invalid read*/*Invalid free* inside the
  second `fclose` (operating on the already-freed `FILE`), or a double-close
  report for the underlying fd. After the fix: clean shutdown, single close, the
  partial `.new` file unlinked, previous `mosquitto.db` intact.

> Note: this is an error-path bug (requires a mid-write failure), not happy-path.
> The fault-injection shim above makes it deterministic. (A full captured run
> should be attached from a normal build environment.)

## Fix

Remove the redundant `fclose` from the callback; `mosquitto_write_file` performs
the single authoritative close and unlink:

```diff
 error:
 	err = strerror(errno);
 	log__printf(NULL, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", db.config->persistence_filepath, err);
-	if(db_fptr){
-		fclose(db_fptr);
-	}
 	return rc;
```

Nulling `db_fptr` instead would not help: it is passed by value, so it cannot
prevent `mosquitto_write_file` from closing its own local `fptr`. Removing the
callback's close brings `persist__write_data` in line with the
`dynsec__write_json_config` convention.

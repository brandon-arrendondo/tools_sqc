# Double `fclose()` of the same `FILE*` on persistence write error (CONFIRMED, reachable)

- **Files / functions:**
  - `src/persist_write.c` — `persist__write_data()` (error path, lines 395–401)
  - `libcommon/file_common.c` — `mosquitto_write_file()` (caller; error path, lines 444–448)
- **Severity:** Medium–High (undefined behavior; possible `double free or corruption` abort inside glibc stdio / heap corruption / crash → DoS on a broker with persistence + autosave enabled).
- **Class:** Double `fclose()` of the same stream / use-after-fclose (undefined behavior, CWE-415-adjacent / CWE-672).
- **Affected:** mainline `mosquitto` @ `d3dd4463` (`src/persist_write.c`, `libcommon/file_common.c`). Also present in pinned tree @ `d3ee5c5c`.
- **Prior art:** none found.

## Summary

**This is a real bug.** When a broker persistence write fails partway through (any `fwrite`/chunk-write error inside `persist__write_data`), the same `FILE*` is closed twice:

1. `persist__write_data()` `fclose()`s `db_fptr` in its `error:` block **without nulling it**, then returns an error code.
2. Its caller `mosquitto_write_file()` sees the non-success return, jumps to its own `error:` label, and `fclose()`s the *same* `FILE*` again (its local `fptr` was never nulled because the callback closed it, not the caller).

The result is a second `fclose()` on an already-closed stream — undefined behavior, and in practice a `FILE` object that stdio has already returned to the heap, i.e. a double free.

The codebase's ownership convention confirms which side is wrong: the *other* write callback used with `mosquitto_write_file`, `dynsec__write_json_config()` (`plugins/dynamic-security/config.c:187`), does **not** close the `FILE*` on error — it only returns an error code and lets `mosquitto_write_file` own and close the stream. `persist__write_data` is the outlier that wrongly closes a `FILE*` it does not own.

## Root cause

### Caller owns and (re-)closes the stream — `libcommon/file_common.c`

`mosquitto_write_file()` opens the file, invokes the write callback, and on any error path closes `fptr` itself. It is careful to null `fptr` after *its own* `fclose` (lines 426, 429) so the `error:` label does not double-close — but it has no way to know the callback already closed the stream:

```c
// libcommon/file_common.c:386
	if((rc = (*write_fn)(fptr, user_data)) != MOSQ_ERR_SUCCESS){
		goto error;                       // <-- callback already fclose()d fptr
	}
```

```c
// libcommon/file_common.c:444
error:
	if(fptr){
		fclose(fptr);                     // <-- SECOND fclose() of the same FILE*
		unlink(tmp_file_path);
	}
	return MOSQ_ERR_ERRNO;
```

At the `goto error` on line 387, the local `fptr` is still the original (non-NULL) `FILE*`, so `if(fptr)` is true and `fclose(fptr)` runs on a stream the callback already closed.

### Callback wrongly closes the caller's stream — `src/persist_write.c`

Write failures inside `persist__write_data` reach the `error:` label either via the `write_e` macro (`src/persist.h:37`: `#define write_e(f, b, c) if(fwrite(b, 1, c, f) != c){ goto error; }`) or via the `goto error;` after each `persist__*_save()` call:

```c
// src/persist_write.c:395
error:
	err = strerror(errno);
	log__printf(NULL, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", db.config->persistence_filepath, err);
	if(db_fptr){
		fclose(db_fptr);                  // <-- FIRST fclose(); db_fptr NOT nulled
	}
	return rc;
}
```

`db_fptr` is the same pointer value that `mosquitto_write_file` holds as `fptr`. Nulling it here would not help the caller (it is passed by value), so the correct fix is for the callback not to close a stream it does not own.

**Contrast — the well-behaved sibling callback** (`plugins/dynamic-security/config.c:187`) never closes `fptr`:

```c
	if(fwrite(json_str, 1, json_str_len, fptr) != json_str_len){
		mosquitto_log_printf(MOSQ_LOG_ERR, "Error saving Dynamic security plugin config: ...");
		rc = MOSQ_ERR_UNKNOWN;
	}
	mosquitto_free(json_str);
	return rc;                            // returns error; caller owns/closes fptr
```

## Reproduction

Trigger condition: a persistence write that fails *after* the file is opened but *during* writing/flushing, so `persist__write_data` reaches its `error:` block and `fclose`s, then `mosquitto_write_file` re-closes.

Concrete steps (Linux, glibc):

1. Build the broker normally (optionally with `-fsanitize=address` or run under valgrind for the clearest diagnostic).
2. Create a config enabling persistence and autosave so writes happen frequently:
   ```
   persistence true
   persistence_location /tmp/mosqtest/
   autosave_interval 1
   ```
3. Force the persistence write (to `<persistence_filepath>.new`) to fail mid-stream. Reliable options:
   - **`/dev/full`:** point the temp write target at `/dev/full`. `fopen("wb")` succeeds and small `fwrite`s buffer successfully, but the buffer flush returns `ENOSPC`, so one of the `write_e`/`fwrite` calls (or the later `fflush`) fails. Easiest: `mkdir /tmp/mosqtest; ln -s /dev/full /tmp/mosqtest/mosquitto.db.new` (the `.new` temp file is what `mosquitto_write_file` opens). Note line 374 `unlink(tmp_file_path)` removes a stale regular file but `errno==ENOENT`/symlink handling means you may need to recreate the symlink or instead set the persistence dir on a tmpfs sized to overflow.
   - **Quota/tiny tmpfs:** `mount -t tmpfs -o size=64k tmpfs /tmp/mosqtest` and load enough retained messages/clients that the serialized DB exceeds 64k; the write fails with `ENOSPC` mid-way.
4. Populate broker state (publish a few retained messages, connect a persistent-session client), then let autosave fire (or send `SIGUSR1`, or trigger a clean shutdown which calls `persist__backup`).
5. **Expected with the bug:** the second `fclose` hits an already-freed `FILE` object — glibc commonly reports `*** glibc detected *** ... double free or corruption` and aborts, or under ASan: `attempting double-free` / `bad-free`, or under valgrind: `Invalid read/free` inside `fclose`. Without sanitizers you may instead see a delayed crash or silent heap corruption.

Note: this path is only reached on I/O failure, so it is an error-handling (fault-injection-reachable) defect, not a happy-path crash. On a healthy disk the broker never double-closes.

## Suggested fix

Make `persist__write_data` follow the codebase's existing ownership convention (the one `dynsec__write_json_config` already follows): the write callback does **not** own the `FILE*`; `mosquitto_write_file` opens it and is solely responsible for closing it on every path. Remove the `fclose` from the callback's error block.

```diff
--- a/src/persist_write.c
+++ b/src/persist_write.c
@@ -395,9 +395,6 @@ static int persist__write_data(FILE *db_fptr, void *user_data)
 error:
 	err = strerror(errno);
 	log__printf(NULL, MOSQ_LOG_ERR, "Error during saving in-memory database %s: %s.", db.config->persistence_filepath, err);
-	if(db_fptr){
-		fclose(db_fptr);
-	}
 	return rc;
 }
```

Why this fix rather than nulling `db_fptr`:

- `db_fptr` is passed to the callback **by value**, so nulling it inside `persist__write_data` cannot prevent `mosquitto_write_file` from re-closing its own local `fptr`. Nulling here does not fix the double close.
- `mosquitto_write_file`'s `error:` label already closes `fptr` *and* `unlink`s the partial temp file (`libcommon/file_common.c:444-448`) — the desired cleanup happens correctly once the callback stops closing.
- `dynsec__write_json_config` establishes that the intended contract is "callback returns an error code, caller cleans up." This change brings `persist__write_data` into line with that contract.

After the fix, the single `fclose` is performed exactly once by `mosquitto_write_file`, and the partial `.new` temp file is unlinked, leaving the previous good `mosquitto.db` intact.

(Defense-in-depth alternative, not required if the above is applied: have `mosquitto_write_file` document/assert that `write_fn` must not close the stream. Do **not** "fix" only the caller by leaving the callback's `fclose` in place — that would still leave the callback closing a stream it does not own and would reintroduce the bug if the caller's cleanup ever changes.)

## Notes

- Confirmed against MAINLINE `@ d3dd4463`: `persist__write_data` error block at `src/persist_write.c:395-401`; the second close at `libcommon/file_common.c:444-448`; the invocation/`goto error` at `libcommon/file_common.c:386-387`. The `write_e` macro that jumps to the error label is `src/persist.h:37`.
- Only two callbacks are passed to `mosquitto_write_file` in-tree: `persist__write_data` (buggy — closes the stream) and `dynsec__write_json_config` (correct — does not). The fix touches only the buggy one.
- Reachability: error-path only. It requires an I/O failure during a persistence write; it is not reachable on the success path. Best demonstrated via fault injection (`/dev/full`, full tmpfs, or quota) under ASan/valgrind, where the double `fclose` is reported deterministically.

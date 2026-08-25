# Mid-tier rules batch A delta-adjudication (task 544) — COMPLETE

Part of task 532's breakdown (17,952 unlabeled findings across 205 rules,
run 187 v0.4.258 vs the v0.4.120 baseline). This bundle covers 5 rules,
529 raw unlabeled findings across the orig-7 projects: **MEM30-C**
(use-after-free/double-free), **MSC04-C** (recursive function calls),
**MSC13-C** (dead stores), **MSC06-C** (sensitive-data clearing may be
optimized away), **ERR33-C** (unchecked stdlib error returns).

## Scope

Derived each project's in-scope predicate from its README before pulling.
75 of 529 raw findings were out-of-scope (test/example/fuzz/Windows-only
files); 454 in-scope findings batched (some per-project, some combined
into small cross-project batches), yielding 447 unique `(rule, project,
file, line)` label rows after same-line consolidation.

| Rule    | Raw (orig-7) | In-scope | TP | FP | Batch precision |
|---------|--------------|----------|----|----|-----------------|
| MEM30-C | 143 | 143 | 4 | 132 | 2.9% |
| MSC04-C | 112 | 109 | 73 | 36 | 66.9% |
| MSC13-C | 106 | 60 | 12 | 48 | 20.0% |
| MSC06-C | 86 | 82 | 3 | 79 | 3.7% |
| ERR33-C | 82 | 60 | 3 | 57 | 5.0% |
| **Total** | **529** | **454** | **95** | **352** | **21.3%** |

Post-import measured precision over the full labeled set
(`bench realworld-score 187`): MEM30-C 1.3% (54.5% recall), MSC04-C 51.1%
(100% recall), MSC13-C 68.5% (98.4% recall), MSC06-C 3.4% (100% recall),
ERR33-C 30.9% (99.4% recall).

## MSC04-C: two genuine, large real recursion cycles found

Unlike the other 4 rules, MSC04-C came back **majority TP** (73/109).
Both sqlite and mosquitto have one real, large, verified indirect-recursion
cycle each:
- **sqlite**: `sqlite3OomFault -> sqlite3ErrorMsg -> sqlite3VMPrintf -> ...
  -> malloc routines -> sqlite3OomFault` — the OOM-handling/error-message/
  malloc subsystem cycles back to itself. Every link spot-checked as a
  real direct call.
- **mosquitto**: a large cycle through the send/receive/logging/mux
  subsystems (including compile-time `#ifdef WITH_EPOLL/WITH_KQUEUE`
  platform dispatch, correctly treated as TP since it's textual
  preprocessor branching, not a misresolved runtime vtable), plus one
  trivial direct self-recursion (`sub__tree_print`).

These are real findings about the codebases (unbounded recursion / stack
exhaustion risk under pathological OOM-retry or event storms), not sqc
bugs — noted here for completeness, not filed as upstream issues.

## Checker bugs found (6 distinct, several independently corroborated)

1. **Function-pointer/callback misresolution in MSC04-C** — independently
   confirmed in **3 separate codebases**: curl's `timer_cb` resolved to an
   unrelated function of the same name in `docs/examples/*.c`; sqlite's
   `xFree` (an allocator vtable field) resolved to an unrelated local
   `static xFree` in `ext/icu/icu.c`; mosquitto's `on_publish` (a
   user-registered callback) resolved to an unrelated same-named function
   in a test file. All 3 fabricate a spurious recursion cycle by chasing a
   function-pointer call site to one arbitrary same-named function instead
   of recognizing it as an ambiguous indirect dispatch. **Dominant FP
   cause for MSC04-C** (36 of 36 FPs traced to this one root cause).
2. **MEM30-C: realloc-grow idiom misread as premature free** (sqlite,
   ~22 of 32 sqlite FPs) — a `p = realloc(p_old, n); if(!p) ... else
   p_old = p;` idiom has its *input* pointer flagged as already-freed
   before the actual free (which only happens in the failure branch).
3. **MEM30-C: allocator-context argument confused with the freed argument**
   (sqlite, ~10 of 32 sqlite FPs) — `sqlite3DbFree(db, ptr)`/
   `sqlite3DbRealloc(db, ptr, n)`'s first argument `db` (the allocator
   context) is mistaken for the pointer being freed instead of `ptr`.
4. **MEM30-C: mutually-exclusive early-return branches misread as one
   path** (hostap ~90/96, small-batch 15/15) — independent `free(x);
   return;` branches (or realloc-growth idioms) treated as sequential,
   fabricating double-frees/UAFs that can't co-occur on any real path.
5. **MSC13-C: C++ default-argument prototypes misparsed as variable
   stores** (mosquitto, 20/20 FPs, 100% of mosquitto's MSC13-C findings)
   — every finding is in `libmosquittopp.h`'s declaration-only C++ method
   prototypes with default parameter values (e.g. `port=1883`); there's no
   function body and no store to be dead, but the checker parses the
   default-argument syntax as an initialization.
6. **MSC13-C/MSC06-C: macro-wrapped control flow confuses dead-store and
   scope-exit analysis** — sqlite's `SEH_TRY{...}SEH_EXCEPT(...)` idiom in
   `wal.c` causes MSC13-C to miss the plain `return rc;` right after
   (dominant sqlite MSC13-C FP cause). Separately, MSC06-C's "just before
   scope exit" heuristic is **systematically wrong across all 64 sqlite
   findings**: allocator helpers (`sqlite3MallocZero`-style) whose zeroed
   buffer is returned/escapes the function never actually go out of scope,
   and clear/reset helpers (`memset(p,0,sizeof(*p))` after freeing
   sub-fields) zero a caller-owned struct that persists past the call —
   neither pattern is a real dead store, but MSC06-C flags both as if the
   buffer dies at function exit.

Also noted but not filed as a standalone rule-bug (too narrow/one-off):
lua's `lmem.c` MEM30-C finding flagged `L` (`lua_State*`) as a reused
freed pointer when `L` is never freed anywhere in the file — likely a
variable-tracking name collision, only 1 occurrence, not independently
corroborated elsewhere.

## Real bugs found (worth security attention, not sqc follow-ups)

- **hostap MEM30-C (4 TP)**: `wpas_dbus_handler_remove_interface`
  (`wpa_supplicant/dbus/dbus_new_handlers.c:963-968`) reads `wpa_s->ifname`
  after `wpa_supplicant_remove_iface()` has already freed `wpa_s` via
  `wpa_supplicant_deinit_iface` on success — a genuine use-after-free.
- **curl MSC06-C (3 TP)**: `md4.c`/`md5.c` NTLM/digest hash-context
  scrubbing and `tool_getparam.c`'s `cleanarg()` (explicit
  username:password argv wipe) are real sensitive-data-clearing sites
  using a plain (non-hardened) `memset` a compiler could legally elide.
- **mosquitto ERR33-C (2 TP)**: `net_mosq.c:304` (PSK identity truncation
  into a fixed OpenSSL buffer used in the TLS handshake) and
  `conf_includedir.c:94` (admin-configured `include_dir` into a fixed
  `MAX_PATH` buffer with no length guard before `FindFirstFile`).
- **hostap ERR33-C (1 TP)**: unchecked `signal(SIGSEGV, …)` registration
  in `eloop_init`.

## Follow-up

Filed as new rule-fix tasks:
- **task 562**: MSC04-C function-pointer/callback dispatch fabricates
  spurious recursion cycles by resolving to an arbitrary same-named
  function instead of recognizing ambiguous indirect calls — 3
  independently-confirmed codebases (curl, sqlite, mosquitto).
- **task 563**: MEM30-C's free-tracking doesn't model mutually-exclusive
  control-flow branches (each freeing/growing the same pointer on its own
  independent path) as non-overlapping, and confuses an allocator-context
  first argument with the pointer actually being freed/reallocated — the
  two dominant, well-corroborated FP causes across sqlite/hostap.
- **task 564**: MSC13-C misparses C++ declaration-only prototypes with
  default-argument values as variable-store statements (100% of
  mosquitto's findings here) — likely affects any C++ header with default
  parameters sqc scans.
- **task 565**: MSC06-C's "clears sensitive data just before scope exit"
  heuristic doesn't check whether the cleared buffer actually escapes the
  function (returned/assigned to an out-param) or is a caller-owned struct
  that persists past the call — both patterns are structurally NOT
  dead stores but MSC06-C flags them as if they were; 100% of sqlite's 64
  findings here traced to this gap.
- Noted (SEH_TRY/SEH_EXCEPT macro blindness for MSC13-C) as likely
  covered by the broader macro-expansion engine work
  (`docs/design/macro-expansion.md`) rather than filed as a new task —
  check there first per CLAUDE.md's macro-engine-check-before-heuristic
  guidance.

CSVs: `data/precision_audit/{sqlite,curl,mosquitto,hostap,lua,libcrc,raylib}/import_delta_batchA_task544.csv`.

# Mid-tier rules batch D delta-adjudication (task 547) — COMPLETE

Part of task 532's breakdown, and the last of the P3 mid-tier bundles.
128 raw unlabeled findings across the orig-7 projects: **CON03-C** (shared
variable lacks volatile/atomic synchronization), **DCL31-C** (identifier
used without prior declaration), **DCL19-C** (function should be static),
**INT10-C** (`%` with a potentially-signed operand).

## Scope

57 of 128 raw findings were out-of-scope. Two new scope-refinement
exclusions added this pass, matching the established WIN_MAC-style split:
`sqlite/src/os_win.c` (`#if SQLITE_OS_WIN`-gated, Windows-only, dead code
on the Linux benchmark host) and `curl/lib/vtls/apple.c` (`#ifdef
USE_APPLE_SECTRUST`-gated, macOS Secure Transport backend). 71 in-scope
findings adjudicated (DCL31-C's single genuinely in-scope finding handled
directly; the rest in 3 subagent batches).

| Rule    | Raw | In-scope | TP | FP  | Batch precision |
|---------|-----|----------|----|-----|-----------------|
| CON03-C | 33  | 18       | 6  | 12  | 33.3% |
| DCL31-C | 32  | 1        | 0  | 1   | 0.0%  |
| DCL19-C | 32  | 26       | 1  | 25  | 3.8%  |
| INT10-C | 31  | 26       | 0  | 26  | 0.0%  |
| **Total** | **128** | **71** | **7** | **64** | **9.9%** |

Post-import measured precision over the full labeled set
(`bench realworld-score 187`): CON03-C 4.1% (100% recall), DCL19-C 5.3%
(100% recall), DCL31-C 0.0% (0/1107, no TPs anywhere in the full labeled
set — pre-existing, this task only added 1 label), INT10-C 3.2% (100%
recall).

## CON03-C: real bugs found in mosquitto

6 of 18 findings are TP, all mosquitto:
- **`lib/net_mosq.c:87`** — `is_tls_initialized`, a one-time-init guard,
  is reachable from `net__init_ssl_ctx()` during per-connection TLS setup,
  which multiple `libmosquitto` client instances (each with its own
  thread via `mosquitto_loop_start`) can invoke concurrently with no lock
  — a genuine cross-thread race in the client library.
- **`src/signals.c` (5 findings)** — `flag_reload`/`flag_log_rotate`/
  `flag_db_backup`/`flag_tree_print`/`flag_xtreport` are set inside signal
  handlers (SIGHUP/SIGRTMIN/SIGUSR1/SIGUSR2) and polled in the main loop,
  but declared plain `bool` rather than `volatile sig_atomic_t` — the
  classic signal-handler-flag visibility gap, real per CERT's strict
  reading even though the idiom usually works in practice.

The 12 FPs are mostly the broker's genuinely single-threaded event loop
(no `pthread_create` anywhere in `src/*.c`) misidentified as needing
synchronization, plus 2 sqlite FPs in Tcl-test-harness-only code.

## DCL19-C: third independent confirmation of the export-macro-blindness bug

25 of 26 sqlite FPs are, once again, `sqlite3_*` public C API functions —
the same export-macro-blindness root cause already tracked as **task
561** (originally found via DCL15-C, task 542). This is now confirmed
across **two different rules** (DCL15-C and DCL19-C) sharing what is very
likely the same underlying "does this function have external linkage"
detection helper — no new task filed, folding this confirmation into
task 561's scope. The 1 TP (`handle__accepted_publish`, mosquitto) is the
same finding already seen in task 542.

## DCL31-C: all 3 raw findings were platform-gated dead code or C++ parsing

- curl (`lib/vtls/apple.c`, 14 findings) and sqlite (`src/os_win.c`, 1
  finding): both platform-gated dead code on the Linux benchmark host
  (new scope exclusions added this pass, see above).
- curl (`src/tool_cb_wrt.c:326`, 1 finding): inside an `#ifdef _WIN32`
  block, also dead code on Linux — not formally excluded via the
  file-level pattern list (a single line inside an otherwise-in-scope
  file) but treated as out-of-scope by the same reasoning.
- mosquitto (`include/mosquitto/libmosquittopp.h:94`, 1 finding, the only
  genuinely in-scope one): a C++ constructor declaration
  (`mosquittopp(const char *id=NULL, ...)`) misparsed as "missing an
  explicit type specifier" — constructors have no return type by C++
  syntax rule. **Yet another finding in this same header file's
  C++-parsing bug family** (tasks 556, 558, 564, 566 all trace to
  `libmosquittopp.h`).

## INT10-C: 0/26 TP, clean type-tracking root cause

Every finding is FP. ~20/26 are operands actually declared
`size_t`/`u32`/`u64`/`unsigned int` at the point of the `%` operation —
the checker's "potentially signed operand" heuristic isn't tracking the
variable's real declared type through the assignment chain, instead
flagging based on a signed-looking sub-expression or a signed field
feeding an unsigned variable. A smaller cluster (hostap timer
calculations) involves a genuinely `int`-typed value provably bounded
non-negative by an immediately preceding clamp/guard. No case in this
batch had a genuinely-negative-capable signed operand feeding an
unguarded array index/bitmask/offset.

## Follow-up

Filed **task 570**: INT10-C's signed-operand detection doesn't track the
operand's actual declared type through assignment chains, flagging
provably-unsigned (`size_t`/`u32`/`u64`) values as potentially signed —
0/26 TP in this batch, dominant cause. No new task filed for DCL19-C
(folded into existing task 561, now confirmed across 2 rules) or the
DCL31-C `libmosquittopp.h` finding (folded into the existing
tasks 556/558/564/566 C++-header-parsing family — worth a single unifying
investigation across all 5+ instances rather than a 6th separate task).

CSVs: `data/precision_audit/{mosquitto,sqlite,hostap}/import_delta_batchD_task547.csv`.

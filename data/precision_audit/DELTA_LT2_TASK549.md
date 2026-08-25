# Long-tail rules group 2 delta-adjudication (task 549) — COMPLETE

Part of task 532's breakdown, the second of 3 P4 long-tail bundles. 155
raw unlabeled findings across 35 rules, each very small (1-9 findings):
STR30-C, FIO47-C, ENV34-C, EXP30-C, MSC37-C, MEM06-C, POS02-C, CON37-C,
DCL23-C, MSC42-C, ARR36-C, DCL30-C, DCL00-C, INT36-C, CON02-C, FIO05-C,
DCL11-C, MEM04-C, API07-C, CON34-C, MEM02-C, ARR39-C, INT34-C, FIO23-C,
CON33-C, SIG02-C, STR02-C, WIN03-C, MSC07-C, PRE05-C, PRE11-C, MSC41-C,
FIO14-C, FIO19-C, FIO17-C.

## Scope and method

85 of 155 raw findings were out-of-scope; 14 of the 35 rules had zero
in-scope findings after exclusion. 70 in-scope findings, all adjudicated
in a single combined batch (small enough not to need splitting), 11 TP /
59 FP (15.7% batch precision).

## Real bugs found

- **mosquitto CON02-C (5 TP)** — the same signal-handler-flag visibility
  gap as CON03-C (task 547): plain `bool` flags set in a signal handler,
  polled unsynchronized, not `volatile sig_atomic_t`.
- **mosquitto SIG02-C (2 TP)** — SIGUSR1/SIGUSR2 genuinely used as normal
  (non-error) operation triggers, an accepted-but-advisory Unix daemon
  pattern.
- **raylib FIO05-C (2 TP)** — public API file-open functions with no
  `fstat`/ownership verification anywhere.
- **PRE11-C (1 TP)**, **DCL00-C (1 TP)** — mechanically confirmed: a
  macro genuinely ends in `;`; a variable genuinely never reassigned.

## FP causes

Mostly control-flow/context the checker doesn't track rather than one
dominant systemic bug:
- **CON33-C/CON34-C/CON37-C (0/16 combined)** — mosquitto's broker
  `src/` never spawns pthreads; all "non-thread-safe function"/`signal()`
  findings are in genuinely single-threaded code.
- **POS02-C (0/7)** — mosquitto's binds/listens happen only after
  `drop_privileges()` in `main()`, or are non-privileged loopback
  sockets — the checker's scope for "subsequent privilege dropping"
  didn't look far enough.
- **ENV34-C (0/7)** — every `gmtime`/`localtime`/`strerror` pointer is
  used/copied immediately, several under a mutex.
- **MSC42-C (0/5)** — curl's DES usage is NTLM/LM-hash
  protocol-mandated, not a free crypto choice.
- **API07-C (0/4)** — all flagged `strncpy` destinations are
  zero-initialized (`memset`/`calloc`) buffers with exact-length copies,
  already effectively NUL-terminated.
- **INT36-C (0/3)** — the message itself contains garbled function
  signatures ("Pointer 'connect(const char *host, int port'
  initialized..."), a clear checker parsing bug: C++ default-argument
  values in `libmosquittopp.h` misread as pointer initializations. **Yet
  another rule hitting the same C++-header-misparsing file** already
  tracked as the umbrella task 571.
- **MSC07-C (0/2)**, **DCL30-C (0/2)**, **FIO47-C (0/2)** — each a
  distinct, single-instance checker mechanics gap (switch/goto-fallthrough
  reachability; a macro constant misparsed as a pointer and a value-copy
  misread as address-of-local; a format-specifier misattributed to the
  wrong argument position) — too few instances each (2 findings) to
  justify a standalone follow-up task on their own.

## Follow-up

No new tasks filed. INT36-C's 3 FPs fold into the existing **task 571**
umbrella (libmosquittopp.h C-rules-on-C++-header investigation) — now
confirmed on a 7th+ distinct rule. The remaining FP clusters (CON33/34/37,
POS02-C, ENV34-C, MSC42-C, API07-C) are each single-batch, context-only
misses without a clean one-line fix, and the 2-instance-each causes
(MSC07-C, DCL30-C, FIO47-C) are too thin a sample to generalize from —
worth re-examining if a future delta pass surfaces more instances of any
of these specific rules.

CSVs: `data/precision_audit/{mosquitto,sqlite,curl,hostap,raylib,lua}/import_delta_lt2_task549.csv`.

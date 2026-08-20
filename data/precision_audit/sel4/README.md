# seL4 onboarding + MSC12-C precision measurement — sqc v0.4.222 (run sqc-0.4.222-c52f9692, run #163, 2026-08-20)

Task 381: find and onboard an 8th real-world oracle codebase specifically to
give MSC12-C (empty-body/no-effect-code detection) its first real precision
measurement on a project that actually follows a no-empty-body convention —
MSC12-C had been categorically disabled in all 7 existing oracles
(`data/precision_audit/msc12_msc13_0.4.141/README.md`) with 0% measured
precision on two independent samples there.

## Candidate: seL4 microkernel

[seL4](https://github.com/seL4/seL4) (`@1326364`, cloned 2026-08-20) is a
formally verified microkernel with a public
[C style guide](https://sel4.systems/Contribute/style.html) prohibiting
dead/no-op code; its functional-correctness proof against an Isabelle/HOL
spec is a strong structural argument against *accidental* dead code. Before
onboarding, a literal grep for `if/for/while (...) {}` across the whole
kernel (`src/`, 183 `.c` files, ~50k lines) returned **zero** hits — a much
cleaner signal than curl/mosquitto/hostap's pervasive deliberate no-op
idioms. On that basis it was onboarded as CODEBASES["sel4"]
(`mcp_servers/realworld_server.py`), scoped to `src/` (the kernel proper;
`libsel4/` is the userspace binding library, not kernel code).

## The literal-braces grep missed the real idiom

Sampling and reading 40 of the resulting 180 MSC12-C findings (seed 20260820,
`data/precision_audit/sel4/adjudication_msc12_0.4.222.csv`) found the grep's
premise was wrong: the dominant empty-body idiom in low-level C is `while
(cond);` — a bare-semicolon busy-wait polling loop, not `while (cond) {}` —
which the grep never matched. seL4's driver/platform code is full of it
(UART TX/RX-ready polling, VT-d completion-bit polling, IRQ-controller reset
polling). Combined with two more idiom families also already characterized
in the other 7 oracles — documented no-op function/case stubs
(`/* Don't need to do anything */`, platform-abstraction shims with no L2
cache to operate on) and macro-hidden real effects (`NODE_UNLOCK_IF_HELD`,
`SCHED_APPEND_CURRENT_TCB`, `IPI_MEM_BARRIER`) — these three families account
for 36 of the 40 sampled findings.

**Measured precision: 10.0% (4 TP / 40 labeled)** — `bench realworld-score
sqc-0.4.222-c52f9692`. Higher than the other 7 oracles' 0%, but for a
mundane reason: 4 genuine stray double-semicolons (`stmt;;`, a real if
harmless typo pattern — `src/arch/arm/32/model/statedata.c:43`,
`src/fastpath/fastpath.c:215/452/881`), not evidence that seL4's control-flow
bodies are meaningfully cleaner than any other systems-C codebase's.

One seL4-specific FP family not seen in the other oracles: two findings
(`src/arch/x86/64/object/objecttype.c:233,239`) are "empty" if/else branches
that actually contain Isabelle proof-annotation comments (`/** AUXUPD: ...
*/`, `/** GHOSTUPD: ... */`) consumed by seL4's verification toolchain — a
misattribution class unique to formally-verified codebases.

## Conclusion

**The premise "some real-world C codebase avoids MSC12-C's no-empty-body
false positives entirely" does not hold**, at least not among systems/kernel
C: busy-wait polling loops and no-op platform stubs are load-bearing idioms
of low-level hardware-facing C in general, seL4 included, not a code-quality
gap specific to the other 7 (mostly userspace) oracles. MSC12-C stays
disabled for seL4 too (`conf/realworld/sel4-rules.toml`), consistent with the
other 7.

What *was* accomplished: MSC12-C now has a second real measured sample (10%,
vs. 0% twice before) confirming the same root causes generalize to a
structurally very different (formally verified, freestanding kernel)
codebase, and seL4 is onboarded as a genuine 8th real-world oracle for the
suite generally (novel domain — no libc, no POSIX, hardware-facing) even
though it didn't resolve the MSC12-C-specific goal task 381 set out for.

**Recommendation**: the actual fix is in the detector, not further corpus
hunting — teach `check_empty_control_flow` to recognize the `while
(cond);`/`for (...);` busy-wait idiom (bare-semicolon body polling a
volatile/memory-mapped condition) as intentional, the same way task 379/380
taught `check_empty_switch_case` to recognize grouped case labels. Filed as
a follow-up (see todo-sqlite-cli).

## Update: fixed in v0.4.223 (commit ca163f93, task 473)

Both `check_empty_control_flow` (braced-empty `while (cond) { }`/`for`/`do`
bodies) and `check_no_effect_expression` (bare-semicolon `while (cond);`
form, including the `while (cond) { ; }` variant) now skip the empty body
when the loop's condition invokes a function, or reads through
pointer/field/array indirection *combined with* an explicit
comparison/bitwise/logical operator — the "polling with a mask/comparison"
signature that covers essentially every real busy-wait in this sample
(`*UART_REG(x) & MASK`, `timer->stat != DONE`, `vtd_read(...) & 1`).
Deliberately narrow: a bare dereference/field-read with no operator
(`while (*flag);`, `while (!timer->tistat);`) is structurally
indistinguishable from a forgotten loop body and stays flagged — this is
pinned down by the pre-existing test `tests/fail/testcases_empty_while_body.c`
(`while (*flag) { }`), which the fix must not regress.

**Real-world impact** (sqc-0.4.223-ca163f93 vs. sqc-0.4.222-c52f9692, same
seL4 checkout): MSC12-C's raw finding count on seL4 dropped **23.9%**, 180 →
137 (all 43 removed were the `while`/bare-semicolon busy-wait "Stray
semicolon"/"Empty while loop body" messages; every other MSC12-C sub-check's
count was unchanged), with zero change to any other rule (5143 → 5100 total
findings, an exact match). Full test suite: 3760 passed, 0 failed — no
regressions, including the 40-item pre-fix sample re-verified against the
post-fix run.

**Re-measured precision** on the post-fix run, adjudicating a fresh
40-finding sample plus tracking all 4 previously-confirmed TPs (all
double-semicolon typos, all still present — recall held):

| | Pre-fix (0.4.222) | Post-fix (0.4.223) |
|---|---|---|
| MSC12-C raw findings | 180 | 137 |
| Sample labels accumulated | 40 | 59 (union of both samples still present in the post-fix run) |
| TP (labeled findings) | 4 | 5 |
| Sample/oracle precision | 10.0% | **8.5%** (`bench realworld-score sqc-0.4.223-ca163f93`) |

The nominal percentage moved within sampling noise (5/59 vs. 4/40) rather
than jumping, because precision here is bottlenecked by the *other* FP
families this fix didn't touch (documented no-op stubs, deliberate empty
switch cases, macro-hidden lock/barrier statements, and — new in this
sample — two misattribution classes: seL4's Isabelle proof-annotation
comments (`/** AUXUPD/GHOSTUPD ... */`) inside "empty" if/else branches, and
an attribute-macro-decorated declaration (`BOOT_BSS rootserver_mem_t
rootserver;`) misread as a discarded-expression statement). What the fix
demonstrably did: cut the raw finding volume by a quarter with zero
recall cost, confirmed against a real, disjoint 40-item resample. The
remaining FP families are each their own smaller, separate fix (not filed
as a task yet — none dominates the way busy-wait did).

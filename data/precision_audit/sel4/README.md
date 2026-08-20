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

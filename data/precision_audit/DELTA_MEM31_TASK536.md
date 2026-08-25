# MEM31-C residual-churn delta-adjudication (task 536) — COMPLETE

Part of task 532's breakdown (17,952 unlabeled findings across 205 rules,
run 187 v0.4.258 vs the v0.4.120 baseline). Task 420 already
delta-adjudicated MEM31-C once (`DELTA_MEM31_TASK420.md`, v0.4.176
baseline, 1,478 findings, 0.7% precision); this task covers the residual
churn from further rule changes since then (v0.4.176 -> v0.4.258) for the
orig-7 projects: sqlite=602, mosquitto=13, hostap=1.

## Scope finding: almost the entire pull is out-of-scope, not rule drift

Re-checked each project's `data/precision_audit/<project>/README.md`
scope predicate before batching (task 420 lesson) and found this task's
"616 unlabeled" figure is overwhelmingly **out-of-scope noise**, not
genuine new rule-drift findings requiring adjudication:

| Project   | Raw unlabeled | Breakdown | In-scope |
|-----------|---------------|-----------|----------|
| sqlite    | 602           | 586 `src/tclsqlite.c` (Tcl language binding, **explicitly excluded** by the sqlite README's own scope note: "same rationale as the `ext/jni`/`ext/wasm` exclusion") + 12 `ext/session/test_session.c` + 2 `ext/expert/test_expert.c` + 1 `ext/session/session_speed_test.c` + 1 `mptest/mptest.c` (all test-glue, matching the README's `test/`+`src/test*.c` exclusion class) | **0** |
| mosquitto | 13            | 7 `fuzzing/broker/fuzz_packet_read_base.c` (fuzz harness) + 6 `examples/{publish,subscribe}/*.c` (demo programs) — none of these are `lib/` or `src/`, the README's documented "shipped product" scope | **0** |
| hostap    | 1             | `hostapd/hostapd_cli.c` — inside `hostapd/`, explicitly in-scope per the README's "`src/` + `wpa_supplicant/` + `hostapd/`" | **1** |
| **Total** | **616**       |           | **1**    |

## Outcome

The single in-scope finding (`hostapd/hostapd_cli.c:178`, "memory allocated
... for variable 'ctrl_conn' is not freed") is **FP**: `ctrl_conn` is a
file-scope `static struct wpa_ctrl *`; `hostapd_cli_reconnect()` always
calls `hostapd_cli_close_connection()` (which calls `wpa_ctrl_close()`)
immediately before reassigning it at the flagged line, and the program's
exit path (`hostapd_cli_cleanup` -> `hostapd_cli_close_connection`)
unconditionally closes it too. The checker's ownership tracking loses the
free that happens across the reconnect call and at program exit —
consistent with task 420's finding that MEM31-C's residual FPs are almost
entirely long-lived/reassigned-handle cases the ownership heuristic can't
see across control-flow boundaries, not a new bug class.

Post-import measured precision for MEM31-C over the full labeled set
(`bench realworld-score 187`): **0.7%** (13 TP / 1,921 labeled) — unchanged
from task 420's figure, confirming this residual churn was genuinely just
noise/scope drift, not a fresh regression. 667 findings remain unlabeled
(the out-of-scope sqlite/mosquitto findings above, correctly left out of
the denominator, plus other rules' backlog).

## Follow-up

None needed — no rule-logic gap surfaced. Task 536 closes clean.

CSV: `data/precision_audit/hostap/import_delta_mem31_task536.csv`.

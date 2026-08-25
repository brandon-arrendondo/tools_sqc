# DCL13-C delta-adjudication (task 541) — COMPLETE

Part of task 532's breakdown (17,952 unlabeled findings across 205 rules,
run 187 v0.4.258 vs the v0.4.120 baseline). This tracks the delta pass for
**DCL13-C** ("pointer parameter is not modified and should be declared
const") — 187 raw unlabeled findings across sqlite/mosquitto/curl/hostap.

## Scope finding: almost the entire pull is out-of-scope, not rule drift

Re-checked each project's `data/precision_audit/<project>/README.md`
scope predicate before batching (task 420 lesson) and found this task's
"187 unlabeled" figure is **overwhelmingly out-of-scope noise**, mirroring
task 536's finding for MEM31-C:

| Project   | Raw unlabeled | Breakdown | In-scope |
|-----------|---------------|-----------|----------|
| sqlite    | 164           | `src/tclsqlite.c` (Tcl binding, explicitly excluded) + `ext/*/test_*.c` + `ext/*/fts*_test*.c` + `ext/rbu/test_rbu.c` + `ext/intck/test_intck.c` + `ext/rtree/test_rtreedoc.c` + `mptest/mptest.c` + `ext/session/session_speed_test.c` — every single file is test-glue or the Tcl binding | **0** |
| mosquitto | 10            | All 10 in `examples/{mysql_log,subscribe,subscribe_simple}/` — demo programs, not `lib/`/`src/` | **0** |
| curl      | 9             | All 9 in `lib/vtls/schannel*.c` — Windows-only, excluded per the WIN_MAC scope split | **0** |
| hostap    | 4             | `src/eap_server/eap_server_tnc.c` ×2, `src/pae/ieee802_1x_secy_ops.c`, `wpa_supplicant/config.c` — all within `src/`/`wpa_supplicant/`, in-scope | **4** |
| **Total** | **187**       |           | **4**    |

The 4 in-scope hostap findings were adjudicated directly (too small to
warrant a subagent batch).

## Outcome

All 4 are **TP** — genuine const-correctness gaps, each verified by
reading the full function body and confirming the flagged pointer
parameter is read-only throughout:
- `eap_server_tnc.c:126` — `sm` is entirely unreferenced in
  `eap_tnc_build`'s body (unused parameter).
- `eap_server_tnc.c:127` — `data` is only read via `data->tncs`, never
  reassigned.
- `ieee802_1x_secy_ops.c:108` — `kay` is only read (`kay->ctx`) in
  `secy_cp_control_current_cipher_suite`.
- `config.c:920` — `ssid` is only read via bitwise `key_mgmt` checks
  throughout `wpa_config_write_key_mgmt` (a config-serialization
  function), never written.

Post-import measured precision for DCL13-C over the full labeled set
(`bench realworld-score 187`): **50.1%** (3,894 TP / 7,767 labeled),
**97.7% recall**. 344 findings remain unlabeled (largely pureftpd/sel4,
out of this task's scope).

## Follow-up

None needed for rule logic — the 4 genuine findings confirm DCL13-C is
working correctly on in-scope code; this task's apparent 187-finding
"unlabeled backlog" was almost entirely a scope-mismatch artifact (test
harnesses, Tcl bindings, examples, and Windows-only code), not real
rule drift, consistent with task 536's identical pattern for MEM31-C.

CSV: `data/precision_audit/hostap/import_delta_dcl13_task541.csv`.

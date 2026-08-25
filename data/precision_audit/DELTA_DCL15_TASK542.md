# DCL15-C delta-adjudication (task 542) — COMPLETE

Part of task 532's breakdown (17,952 unlabeled findings across 205 rules,
run 187 v0.4.258 vs the v0.4.120 baseline). This tracks the delta pass for
**DCL15-C** ("declare file-scope objects/functions that don't need
external linkage as static") — 159 raw unlabeled findings across the
orig-7 projects (sel4's separate 63 tracked under task 552).

## Scope

Derived each project's in-scope file predicate from its own
`data/precision_audit/<project>/README.md` before pulling findings:

| Project   | Raw unlabeled | Dropped (out-of-scope)                                   | In-scope | Batches |
|-----------|---------------|--------------------------------------------------------------|----------|---------|
| sqlite    | 141           | 17 (`src/tclsqlite.c`, `ext/*/test_*.c`, `mptest/`)           | 124      | 2       |
| mosquitto | 16            | 15 (`examples/*` — demo programs)                              | 1        | 1 (adjudicated directly) |
| lua       | 2             | 0                                                              | 2        | 1 (adjudicated directly) |
| **Total** | **159**       | **32**                                                         | **127**  | **3**   |

## Outcome

| Project   | Findings | TP | FP  | Precision |
|-----------|----------|----|-----|-----------|
| sqlite    | 124      | 0  | 124 | 0.0% |
| mosquitto | 1        | 1  | 0   | 100.0% |
| lua       | 2        | 0  | 2   | 0.0% |
| **Total** | **127**  | **1** | **126** | **0.8%** |

Post-import measured precision for DCL15-C over the full labeled set
(`bench realworld-score 187`): **2.4%** (16 TP / 659 labeled), **100%
recall**. 95 findings remain unlabeled (largely pureftpd/sel4).

## Root cause is uniform and clean: sqc doesn't check public-API export macros/headers

**100% of the 124 sqlite FPs and both lua FPs share one exact cause,
independently confirmed across two unrelated codebases with two different
export-macro conventions**: the checker verifies there is no *plain* (macro-
free) cross-file reference to a function, but doesn't recognize a function
declared in a public header behind a library export macro as needing
external linkage:

- **sqlite**: every flagged function is `sqlite3_*`, declared with
  `SQLITE_API` in `src/sqlite.h.in` (the template that generates the public
  `sqlite3.h`) — the library's entire public C API surface. One sqlite
  finding (`ext/misc/series.c`'s `sqlite3_series_init`) is a Tcl/shell
  extension entry point referenced via an `extern` declaration in
  `test1.c`'s Tcl command table, a related but distinct cross-file-linkage
  pattern the checker also misses.
- **lua**: `luaL_newstate` (`LUALIB_API`, declared in `lauxlib.h`) and
  `lua_newthread` (`LUA_API`, declared in `lua.h`) — Lua's public C API.

The single genuine TP (`mosquitto/src/handle_publish.c:67`,
`handle__accepted_publish`) is a real finding: verified via grep that the
function is referenced only within its own file (definition plus one call
site), never declared in any header or called from another translation
unit.

## Follow-up

Filed **task 561**: DCL15-C should recognize a function declared in a
header behind a project's own export/visibility macro (`SQLITE_API`,
`LUA_API`/`LUALIB_API`, and by extension `CURL_EXTERN`,
`mosquitto_EXPORT`-style macros in other projects) as needing external
linkage, not just a plain unmarked prototype. This single fix would
eliminate the overwhelming majority of DCL15-C's measured FPs across at
least two independently-confirmed codebases (sqlite: 124/124 FPs here;
lua: 2/2) — very likely the dominant FP driver for this rule generally,
not a per-project quirk.

CSVs: `data/precision_audit/{sqlite,mosquitto,lua}/import_delta_dcl15_task542.csv`.

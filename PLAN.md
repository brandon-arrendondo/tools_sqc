# SqC — Plans & Roadmap

**Last Updated**: 2026-03-24 (v0.3.39 benchmarked)

For completed work, see [CHANGELOG.md](CHANGELOG.md).
For benchmark data, see [JULIET_RESULTS.md](JULIET_RESULTS.md) and [REALWORLD_RESULTS.md](REALWORLD_RESULTS.md).
For competitor research and academic references, see the [Developer Guide bibliography](docs/bibliography.rst).

---

## Immediate Next Steps

### Real-World Benchmark Runner Broken (Priority 1)

v0.3.42 real-world runs failed on all codebases except libcrc. The MCP runner invokes `sqc_parallel_scan.py` with `-I` include path flags, but the script doesn't accept them — they get passed through as unrecognized arguments:

```
sqc_parallel_scan.py: error: unrecognized arguments: -I /usr/include -I /home/brandon/data/curl/lib
```

**Root cause**: `mcp_servers/server.py` (or the shell wrapper it calls) passes `-I` flags from the per-project config to `sqc_parallel_scan.py`, which only accepts `--sqc`, `-m`, `-e`, `-d`, `-j`, `--min-files`, `--prescan-cache-dir`, `--rebuild-prescan`. The `-I` flags need to either be added to the parallel scan script's argparse, or forwarded to the underlying `sqc` invocations via a passthrough mechanism.

**Impact**: Can't get real-world benchmark data for any project that uses `-I` (curl, hostap, mosquitto, sqlite). Only libcrc works (no `-I` needed).

**Fix options**:
1. Add `-I` / `--include` to `sqc_parallel_scan.py` argparse and forward to each `sqc` subprocess
2. Add a generic `--sqc-args` passthrough in the parallel scanner
3. Fix the MCP runner to invoke `sqc` directly (not via parallel scanner) for projects where parallelization isn't needed

### Real-World FP Reduction — Top Rules (Priority 1)

v0.3.39 per-rule data (all 5 codebases, 153.2K total violations, rules-benchmark.toml).

| Rule | Count | Issue | Status |
|------|------:|-------|--------|
| MEM30-C | 15,330 | Use-after-free | Needs field-level free tracking (deferred) |
| DCL13-C | 12,138 | Const correctness | Needs alias tracking (deferred) |
| INT32-C | 12,050 | Signed overflow | Stable after VRA |
| API00-C | 9,227 | Missing size parameter | Stable |
| INT30-C | 8,474 | Unsigned overflow | Stable after VRA |
| DCL31-C | 7,366 | Undeclared function | Prescan + `-I` headers |
| DCL07-C | 7,291 | Implicit int declaration | Prescan + `-I` headers |
| EXP33-C | ~6,100 | Uninitialized | v0.3.38: FP fixes |
| MEM31-C | 5,440 | Memory leak | Needs ownership model (deferred) |
| EXP34-C | 5,267 | Null deref | Stable |
| ARR00-C | 2,157 | Array bounds | v0.3.37: -480 (-18.2%) |
| ERR33-C | 989 | Unchecked return | v0.3.37: -818 (-45.3%) |
| ARR36-C | 829 | Pointer subtraction | v0.3.39: −3,856 (−82.3%), regression resolved |

### Juliet TP Rate — Path to 50%

v0.3.39: **48.4% TP rate** (unchanged since v0.3.37 — ARR36-C fix was real-world only). Remaining gap dominated by high-FP rules where Juliet good/bad patterns are structurally identical to our analysis: INT32-C (55% FP), ENV33-C (58% FP), STR31-C (59% FP), INT33-C (65% FP), FLP03-C (69% FP).

### EXP33-C — Remaining

- **Cross-function variants 63/64**: pointer passed between source files, needs inter-procedural analysis

### Real-World DCL31-C/DCL07-C — Remaining Include Gaps

v0.3.38 removed library-specific whitelists and added `-I` include paths for
system-installed third-party headers. Most projects at or below v0.3.34 baseline,
but two still show increases from unresolved function declarations:

| Project | v0.3.34 | v0.3.38 | Delta | Unresolved Functions |
|---------|---------|---------|-------|---------------------|
| mosquitto | 15,221 | 16,481 | +1,260 | `cJSON_*` (1424, header found but includes as `<cjson/cJSON.h>` — need `-I /usr/include` not `/usr/include/cjson`), `CU_*` (692, CUnit test framework), `sqlite3_*` (496), `mysql_*` (24) |
| sqlite | 50,517 | 53,578 | +3,061 | `Tcl_*` (3010, `#include "tclsqlite.h"` internal header — `-I /usr/include/tcl8.6` doesn't help), internal `sqlite3_*` (32, not caught by `-d` prescan) |

**Fix approaches**:
- mosquitto `cJSON_`: fix `-I` path — currently `/usr/include/cjson` but mosquitto includes as `<cjson/cJSON.h>` so `/usr/include` is correct (already set, may need prescan rebuild with both paths)
- mosquitto `CU_`/`sqlite3_`: these are test dependencies — add `-I /usr/include` covers them (already set)
- sqlite `Tcl_`: Tcl embeds via `#include "tclsqlite.h"` which includes `<tcl.h>` internally — sqc can't follow this chain. Need to add Tcl source dir to `-d` or accept as residual.
- Alternative: implement glob/prefix suppression (see Architecture Evolution)

---

## Medium Term

### CWE-457: Uninitialized Variable — Remaining Gaps (Priority 1)

v0.3.37: 165 TP, 302 FP, **35.3% TP rate** (up from 32.2% in v0.3.34). The arr[0].field fix and initializer suffix matching removed 72 FP while losing only 13 TP — clean improvement.

Remaining gaps:
- Cross-function variants 63/64 (~70 files): pointer passed between source files, needs inter-procedural analysis
- Per-element tracking for stack arrays: `team[0].x = 1; use(team[3].x)` correctly flags, but no way to track that ALL elements are initialized
- 302 FP still high — likely dominated by cross-function initialization patterns Juliet's "good" functions use

### CWE-190/191: Integer Overflow/Underflow (Priority 3)

v0.3.37: CWE-190 655 TP/790 FP (45.3%), CWE-191 560 TP/716 FP (43.9%). Unchanged since v0.3.28. INT30-C/INT32-C matched. Stable after VRA — gap is coverage not precision.

### CWE-690: Null Deref from Return (Priority 4)

v0.3.37: 203 TP, 12 FP, **94.4% TP rate**, 18.1% per-file. Best precision of any high-volume CWE. Per-file rate (18.1%) still below 30% target — 74% undetected are likely cross-function patterns.

### EXP34-C Phase 4 — Remaining Edge Cases

- Relay chains (3+ hops): multi-pass handles single-hop, deep chains still Unknown
- Indirect data flow (variants 63–67): not addressed
- Cross-file globals (variant 68): not addressed
- EXP34-C/FIO06-C regression investigation from Phase 3

### Real-World FP — Deferred Hard Issues

Require new analysis capabilities beyond current architecture:

| Rule | v0.3.37 Count | Issue | Required Capability |
|------|--------:|-------|---------------------|
| MEM30-C | 15,330 | Sequential struct/member frees, cross-function free propagation | Field-level free tracking |
| MEM31-C | 5,440 | Cross-function ownership (strdup → struct field → custom_Delete) | Ownership model |
| DCL13-C | 12,138 | Const correctness — pointer params through struct fields | Alias/points-to tracking |

### Zero-Detection CWEs — Remaining

| CWE | Portable Files | Mapped Rules | Issue | Effort |
|-----|------:|--------------|-------|--------|
| CWE-468 (incorrect pointer scaling) | 36 | ARR39-C, EXP08-C | Implicit void* casts losing type info | High |

4 formerly zero-detection CWEs resolved in v0.3.35–v0.3.36 (see CHANGELOG). 13 are Windows-only (not actionable).

---

## Long Term

### Test Coverage (Priority 2)

**Coverage gate**: 80% line coverage enforced (`scripts/coverage-gate.sh`). Current: 80.06% (24,908 uncovered of 124,904 lines).

#### Raise Coverage Gate to 81%

Reaching 81% requires ~1,175 additional covered lines. Highest-impact targets:

| File | Uncovered Lines | Current Coverage | Path to Improve |
|------|----------------:|-----------------:|-----------------|
| ARR38-C | 759 | 70.7% | Add `.c` test cases for library-specific patterns |
| value_range.rs | 567 | 70.9% | Add unit tests for uncovered condition/assignment handlers |
| ERR33-C | 550 | 71.2% | Add more `.c` test cases for various stdlib error checks |
| INT31-C | 516 | 70.1% | Add `.c` tests for integer conversion patterns |
| INT34-C | 451 | 54.7% | Add `.c` tests exercising VRA and range-extraction paths |
| analyze/mod.rs | 401 | 40.4% | `analyze_project` integration test (needs manifest + source fixture) |

#### FP Regression Tests — Rounds 3, 6, 7

Rounds 8–11 covered. Still needed:
- Std function database lookups should not trigger DCL31-C/DCL07-C (Round 3)
- Cross-file function definitions should not trigger DCL31-C/DCL07-C (Round 6)
- Unknown-type pointer casts should not trigger EXP36-C (Round 7)

#### Expand Wiki-Only Rules

~105 rules have exactly 2 test files (1 fail + 1 pass). Prioritize top-FP rules from real-world benchmarks:
- EXP33-C (6,100 real-world violations) — only 10 wiki tests
- EXP34-C (5,267) — 6 wiki tests
- ERR33-C (989) — 11 wiki tests

#### Architecture Improvements (effort: high)

1. **CLI integration tests** — test `--diff`, `--export json/csv/sarif`, `-I`, `-d`, `--save-prescan`/`--load-prescan`
2. **`expected_fail/` test category** — for known limitations (e.g., EXP34-C intra-file null deref without call-site analysis)
3. **Fix FIO10-C rule** — accept POSIX `rename()` with error checking as compliant (matches CERT wiki)
4. **EXP34-C intra-file call-site analysis** — 3 tests waiting in pass/ to move to fail/
5. **Inter-procedural `.c` tests** — multi-file C test cases for prescan/call-site propagation
6. **No tests for CLI flags** — `--diff`, `--export`, `--format`, `-I`, `--save-prescan`, `--load-prescan`, suppression

### Implementation Bugs Found via Testing (Fix Queue)

Bugs discovered during test infrastructure improvements (2026-03-24):

| Component | Bug | Severity | Test File |
|-----------|-----|----------|-----------|
| ~~`const_eval.rs`~~ | ~~Double-nested parens fail~~ | ~~Medium~~ | **FIXED v0.3.42** — loop-based paren stripping |
| ~~DCL02-C~~ | ~~Function-scope declarations not checked~~ | ~~Medium~~ | **FIXED v0.3.42** — `init_declarator` + root-node traversal |
| ~~EXP40-C~~ | ~~Only double-pointer const bypass detected~~ | ~~Medium~~ | **FIXED v0.3.42** — per-scope const variable tracking |
| ~~STR34-C~~ | ~~init_declarator path not checked~~ | ~~Medium~~ | **FIXED v0.3.42** — parameter tracking + per-function scoping |
| ~~CON34-C~~ | ~~Thread-unsafe functions not detected~~ | ~~Medium~~ | **FIXED v0.3.42** — banned function list |
| ~~CON07-C~~ | ~~Unprotected shared variable access not detected~~ | ~~Medium~~ | **FIXED v0.3.42** — file-scope global collection |
| ~~MSC38-C~~ | ~~Signal handler printf not detected~~ | ~~Medium~~ | **RECLASSIFIED** — test moved to SIG30-C (already detected) |
| STR03-C | `strncpy` with prior `strlen` validation still flagged. Rule doesn't recognize `if (strlen(src) < dest_size)` as validation before `strncpy`. | Low | N/A (test adjusted to avoid strncpy) |
| FIO10-C | Rule requires explicit `access()+remove()` before `rename()`, but CERT wiki's compliant POSIX example uses plain `rename()` with error checking. | Low | `tests/pass/wiki_posix.c` (TODO: fix rule to accept POSIX rename()) |
| INT00-C | Unsigned subtraction without guard and mixed signed/unsigned comparison not detected. Rule may not check these patterns. | Medium | `tests/pass/testcases_unsigned_wrap.c` (TODO: move to fail/) |
| INT16-C | Signed-to-unsigned conversion without range check not detected. `unsigned int u = signed_val;` produces no violation. | Medium | `tests/pass/testcases_signed_unsigned_conversion.c` (TODO: move to fail/) |
| ~~ERR01-C~~ | ~~Missing errno check after strtol/sqrt not detected~~ | ~~Medium~~ | **FIXED** — errno-setting function detection |
| ~~MEM10-C~~ | ~~`sizeof(pointer)` misuse in malloc/memset not detected~~ | ~~Medium~~ | **FIXED** — sizeof(pointer) detection in alloc/memory calls |
| ~~POS50-C~~ | ~~TOCTOU race (`stat()` then `fopen()` on same path) not detected~~ | ~~Medium~~ | **FIXED** — check-then-use TOCTOU detection |
| ~~DCL17-C~~ | ~~K&R style function declaration without prototype not detected~~ | ~~Low~~ | **FIXED** — K&R definition + empty param list detection |
| WIN30-C | `CreateFileA` with NULL security attributes not detected. | Low | `tests/pass/testcases_win_api_misuse.c` (TODO: move to fail/) |
| MEM04-C | `malloc(sizeof(int))` falsely flagged as "potentially zero size" — sizeof(int) is always > 0. | Low | N/A (FP, test adjusted) |

### Fake-Passing Tests — Periodic Review Needed

**Action item**: Periodically grep `tests/pass/` for `TODO.*move to fail` and `Known limitation` to find tests that pass only because the implementation has a gap. As each implementation bug above is fixed, move the corresponding test from `pass/` to `fail/` and verify it now triggers the expected violation.

Current inventory (2026-03-25, 7 remaining — 7 fixed in v0.3.42, 4 fixed in this batch):

| Rule/Component | Test File | What Should Fail | Blocker |
|----------------|-----------|-----------------|---------|
| EXP34-C | `pass/testcases_func_param.c` | Intra-file null deref via function parameter | Test infra — see below |
| EXP34-C | `pass/testcases_list_null.c` | Same — NULL passed to function that dereferences | Test infra — see below |
| EXP34-C | `pass/testcases_callback_null.c` | Same — callback receives NULL | Test infra — see below |
| FIO10-C | `pass/wiki_posix.c` | POSIX rename() with error checking | Rule design |
| INT00-C | `pass/testcases_unsigned_wrap.c` | Unsigned wrap and mixed signed/unsigned comparison | Pattern mismatch — rule checks format specifiers/casts, not unsigned wrap |
| INT16-C | `pass/testcases_signed_unsigned_conversion.c` | Signed-to-unsigned conversion without range check | Pattern mismatch — rule checks bitwise ops on signed ints, not conversion |
| WIN30-C | `pass/testcases_win_api_misuse.c` | CreateFileA with NULL security attributes | Pattern mismatch — rule checks alloc/dealloc pairing, not security attrs |

**EXP34-C analysis (2026-03-25)**: The inter-procedural null analysis (Phases 1-3, call-site propagation, multi-pass relay) **does detect these patterns** in real-world usage with `-d`/prescan. The blocker is the test infrastructure: `build.rs` generates tests that call `rule.check()` directly on a single parsed file with no prescan context. Without `set_project_context()`, `collect_param_pointer_state()` defaults pointer params to NotNull (line 1128 of `null_state.rs`). Fix requires test infra enhancement: either generate tests that build intra-file prescan before invoking the rule, or run these through `analyze_project` instead of `rule.check()`.

Quick check command: `grep -r "TODO.*move to fail\|TODO.*Move to fail\|Known limitation" src/rules/cert_c/**/pass/*.c src/analyze/*.rs`

### Architecture Evolution

- [x] **Prescan cache** (v0.3.28) — `--save-prescan`/`--load-prescan`, persistent in `data/prescan_cache/`, `--rebuild-prescan` for stale cache
- [x] **External parallelization** (v0.3.27–v0.3.28) — `scripts/sqc_parallel_scan.py` with subdirectory splitting + prescan cache
- [ ] **Internal parallelization** — rayon for file-level parallelism within a single sqc invocation
- [ ] **File-size-aware batching** — current subdir splitting can leave one large unit dominating wall time (e.g., wpa_supplicant/ 69 files = 1061s). Batch by file size rather than directory to balance work across workers.
- [ ] **Incremental parsing** — only re-parse changed files
- [ ] **Baseline-aware suppression** — "only new violations" mode
- [ ] **Glob/prefix suppression in `.sqc-suppress.toml`** — current TOML format only supports per-file/per-line hash-matched suppressions. Real-world projects need pattern-based suppressions (e.g., suppress DCL31-C for all `wolfSSL_*` calls, or suppress a rule for an entire directory). Valgrind uses `obj:*libXt.so*` glob syntax; cppcheck uses `rule:*` file wildcards; clang-tidy uses `NOLINTNEXTLINE(cert-*)` prefix matching. Candidates for sqc:
  - `file_glob` field: `"src/vendor/**"` — suppress rule for all files matching glob
  - `function_prefix` field: `"wolfSSL_"` — suppress DCL31-C/DCL07-C for function name prefixes
  - `rule_glob` field: `"DCL*"` — suppress all DCL rules (useful for vendor/third-party code)
- [ ] **Docker image** — containerized CI/CD distribution

### Analysis Capabilities Lacking

- No preprocessor expansion (macros appear as function calls; macro aliases partially addressed via `collect_macro_aliases`)
- No alias analysis (pointer aliasing not resolved; file-scoped alias collection causes cross-function issues)
- No symbolic execution
- No SSA form (beyond reaching definitions)
- Value-range analysis is intra-procedural with inter-procedural return ranges (v0.3.23–v0.3.24). No inter-procedural argument ranges or field-sensitive VRA.
- Limited whole-program analysis (function summaries + call-site null state + multi-pass relay propagation + local variable tracking + `-I` header resolution)
- Struct field type resolution limited to structs visible during prescan (INT32-C/INT30-C only)

### DCL13-C: Alias Tracking for Last FP

`ringbuffer.c:275 ptrBuffer` — pointer stored into struct field, then `memset` writes through the struct member. Requires alias/points-to tracking. Possible shortcut: if a pointer param is stored into a struct field, treat it as potentially modified.

### Definition of Done

**Tier 1 — Minimum Viable for CI/CD** (COMPLETE)
- [x] `--fail-on-violation` and `--fail-on-severity` flags
- [x] JSON, CSV, SARIF output
- [x] Incremental analysis (`--diff`)
- [x] Severity threshold filtering
- [x] GitHub Actions + Azure DevOps example workflows

**Tier 2 — Production Quality**
- [x] Real-world validation on 5+ open-source projects
- [ ] Baseline-aware suppression
- [ ] Docker image
- [ ] CWE-matched TP rate >= 50% on key CWEs (currently 48.4% overall; 16 CWEs already at 100%, 6 above 50%)

**Tier 3 — Competitive**
- [ ] Direct benchmarked comparison with Infer, Frama-C (see [bibliography](docs/bibliography.rst))
- [ ] Published comparison results
- [ ] Per-file detection >= 30% on top 10 CWEs by volume

# SqC — Plans & Roadmap

**Last Updated**: 2026-03-24 (v0.3.39 benchmarked)

For completed work, see [CHANGELOG.md](CHANGELOG.md).
For benchmark data, see [JULIET_RESULTS.md](JULIET_RESULTS.md) and [REALWORLD_RESULTS.md](REALWORLD_RESULTS.md).
For Juliet per-CWE coverage, see [JULIET_COVERAGE.md](JULIET_COVERAGE.md).
For test infrastructure audit and coverage gaps, see [TEST_INFRASTRUCTURE.md](TEST_INFRASTRUCTURE.md).
For competitor research, see [RESEARCH.md](RESEARCH.md).

---

## Immediate Next Steps

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

Current test infrastructure auto-generates integration tests from `.c` files in `tests/` directories. Coverage gaps:
- Many rules have only wiki-sourced tests (1-3 cases) — need broader pattern coverage
- No tests for inter-procedural analysis paths (prescan, call-site propagation, `-d` directories)
- No regression tests for specific FP patterns fixed in each round (real-world patterns from arraylist.c, intset.c, file_util.c etc.)
- EXP34-C param-null tests had to be restructured when default changed — need tests that exercise with/without project context
- No negative tests for false-positive patterns (verify FP stays suppressed)

### Implementation Bugs Found via Testing (Fix Queue)

Bugs discovered during test infrastructure improvements (2026-03-24):

| Component | Bug | Severity | Test File |
|-----------|-----|----------|-----------|
| `const_eval.rs` | `try_evaluate_text("((10))")` returns `None` — double-nested parens fail. Single-level `(10)` works but recursive paren stripping doesn't re-enter after unwrapping. | Medium | `test_try_evaluate_text_nested_parens` (commented assertion) |
| DCL02-C | Function-scope declarations not checked for visual similarity. Only file-scope `int id_O; int id_0;` is detected; identical patterns inside function bodies produce zero violations. | Medium | `tests/pass/testcases_function_scope_gap.c` (TODO: move to fail/) |
| EXP40-C | Only detects double-pointer const bypass (`const T **ipp = &ip`). Simple `int *p = &const_int` and `int *p = const_ptr_param` not detected. | Medium | `tests/pass/testcases_const_pointer_assignment.c` (TODO: move to fail/) |
| STR34-C | `init_declarator` path not checked for char→int sign extension. Only `assignment_expression` and `cast_expression` detected. `int val = *str;` produces no violation. | Medium | `tests/pass/testcases_char_to_int_init.c` (TODO: move to fail/) |
| STR03-C | `strncpy` with prior `strlen` validation still flagged. Rule doesn't recognize `if (strlen(src) < dest_size)` as validation before `strncpy`. | Low | N/A (test adjusted to avoid strncpy) |
| FIO10-C | Rule requires explicit `access()+remove()` before `rename()`, but CERT wiki's compliant POSIX example uses plain `rename()` with error checking. | Low | `tests/pass/wiki_posix.c` (TODO: fix rule to accept POSIX rename()) |
| INT00-C | Unsigned subtraction without guard and mixed signed/unsigned comparison not detected. Rule may not check these patterns. | Medium | `tests/pass/testcases_unsigned_wrap.c` (TODO: move to fail/) |
| INT16-C | Signed-to-unsigned conversion without range check not detected. `unsigned int u = signed_val;` produces no violation. | Medium | `tests/pass/testcases_signed_unsigned_conversion.c` (TODO: move to fail/) |
| ERR01-C | Missing errno check after strtol/sqrt not detected. Rule may only check specific patterns. | Medium | `tests/pass/testcases_errno_not_checked.c` (TODO: move to fail/) |
| MEM10-C | `sizeof(pointer)` misuse in malloc/memset not detected. `malloc(sizeof(arr))` where arr is a pointer should be flagged. | Medium | `tests/pass/testcases_sizeof_pointer.c` (TODO: move to fail/) |
| POS50-C | TOCTOU race (`stat()` then `fopen()` on same path) not detected. Classic check-use vulnerability. | Medium | `tests/pass/testcases_toctou.c` (TODO: move to fail/) |
| CON07-C | Unprotected shared variable access (no mutex lock) not detected. | Medium | `tests/pass/testcases_unprotected_shared.c` (TODO: move to fail/) |
| CON34-C | Thread-unsafe functions (`localtime`, `strtok`, `rand`) not detected. | Medium | `tests/pass/testcases_thread_unsafe.c` (TODO: move to fail/) |
| DCL17-C | K&R style function declaration without prototype not detected. | Low | `tests/pass/testcases_no_prototype.c` (TODO: move to fail/) |
| MSC38-C | Signal handler calling `printf` (not async-signal-safe) not detected. | Medium | `tests/pass/testcases_signal_handler.c` (TODO: move to fail/) |
| WIN30-C | `CreateFileA` with NULL security attributes not detected. | Low | `tests/pass/testcases_win_api_misuse.c` (TODO: move to fail/) |
| MEM04-C | `malloc(sizeof(int))` falsely flagged as "potentially zero size" — sizeof(int) is always > 0. | Low | N/A (FP, test adjusted) |

### Fake-Passing Tests — Periodic Review Needed

**Action item**: Periodically grep `tests/pass/` for `TODO.*move to fail` and `Known limitation` to find tests that pass only because the implementation has a gap. As each implementation bug above is fixed, move the corresponding test from `pass/` to `fail/` and verify it now triggers the expected violation.

Current inventory (2026-03-24, 18 tests across 15 rules + 1 infra):

| Rule/Component | Test File | What Should Fail |
|----------------|-----------|-----------------|
| EXP34-C | `pass/testcases_func_param.c` | Intra-file null deref via function parameter |
| EXP34-C | `pass/testcases_list_null.c` | Same — NULL passed to function that dereferences |
| EXP34-C | `pass/testcases_callback_null.c` | Same — callback receives NULL |
| EXP40-C | `pass/testcases_const_pointer_assignment.c` | Single-level const-to-non-const pointer |
| DCL02-C | `pass/testcases_function_scope_gap.c` | Function-scope visual similarity |
| STR34-C | `pass/testcases_char_to_int_init.c` | Init declarator char→int without cast |
| FIO10-C | `pass/wiki_posix.c` | POSIX rename() with error checking |
| INT00-C | `pass/testcases_unsigned_wrap.c` | Unsigned wrap and mixed signed/unsigned comparison |
| INT16-C | `pass/testcases_signed_unsigned_conversion.c` | Signed-to-unsigned conversion without range check |
| ERR01-C | `pass/testcases_errno_not_checked.c` | Missing errno check after strtol/sqrt |
| MEM10-C | `pass/testcases_sizeof_pointer.c` | sizeof(pointer) misuse in malloc/memset |
| POS50-C | `pass/testcases_toctou.c` | stat()+fopen() TOCTOU race |
| CON07-C | `pass/testcases_unprotected_shared.c` | Unprotected shared variable access |
| CON34-C | `pass/testcases_thread_unsafe.c` | Thread-unsafe functions (localtime, strtok, rand) |
| DCL17-C | `pass/testcases_no_prototype.c` | K&R function declaration without prototype |
| MSC38-C | `pass/testcases_signal_handler.c` | printf in signal handler |
| WIN30-C | `pass/testcases_win_api_misuse.c` | CreateFileA with NULL security attributes |
| `const_eval.rs` | `test_try_evaluate_text_nested_parens` | Double-nested parens `((10))` |

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
- [ ] Direct benchmarked comparison with Infer, Frama-C (see [RESEARCH.md](RESEARCH.md))
- [ ] Published comparison results
- [ ] Per-file detection >= 30% on top 10 CWEs by volume

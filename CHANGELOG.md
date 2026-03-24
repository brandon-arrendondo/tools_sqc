# SqC — Changelog

## v0.3.39 (2026-03-24)

### ARR36-C: Real-World FP Reduction (−82%)

v0.3.36 added strchr-based pointer subtraction detection (CWE-469, 100% TP on
Juliet). On real-world code it regressed +2,727 violations due to several
analysis bugs. This release fixes them all.

**Per-function scoping** — Root cause: file-global `PointerAnalyzer` caused
cross-function variable name collisions. Same-named vars (`pos`, `end`, `buf`)
in different functions overwrote each other's array bases. Now creates a fresh
analyzer per `function_definition` with file-scope globals as shared base.

**Identifier chain resolution** — `extract_array_base` follows `variable_arrays`
for known identifiers. Fixes `pos = buf` where buf is a parameter: pos now
correctly inherits buf's `param:buf` base instead of getting raw `"buf"`.

**Compound assignment skip** — `pos += ret` no longer overwrites pos's array
base. `+=`/`-=` advance within the same array; only simple `=` changes the base.

**os_\* wrapper recognition** — `os_strchr`, `os_strstr`, `os_strrchr` etc.
treated as their standard equivalents for pointer-into-first-arg tracking.

**Allocation function bases** — `malloc`/`calloc`/`realloc`/`aligned_alloc` get
unique per-call bases (distinct heap objects).

**Address-of vs dereference** — `&var` returns the variable name even if
untracked (single-element array). `*ptr` follows the alias chain.

**File-scope + bare declarations** — Global/static array declarations tracked
across functions. `int arr[N];` without initializer now tracked.

### Benchmark: v0.3.38 → v0.3.39

**Juliet**: Zero delta — 8,508 TP, 9,067 FP, **48.4% TP rate** (unchanged).

**Real-world** (5 codebases, sqc-only): 157,688 → 153,156 (**−4,532**, −2.9%).

| Codebase | v0.3.38 | v0.3.39 | Delta |
|----------|--------:|--------:|------:|
| hostap | 62,044 | 59,407 | −2,637 |
| sqlite | 53,964 | 52,978 | −986 |
| mosquitto | 16,481 | 15,800 | −681 |
| curl | 24,893 | 24,665 | −228 |
| libcrc | 306 | 306 | 0 |

Per-rule: ARR36-C **4,685 → 829 (−3,856, −82.3%)**. DCL31-C −334, DCL07-C −333
(side effect of improved scoping). Zero regressions on any rule or CWE.

## v0.3.38 (2026-03-24)

### EXP33-C: Real-World FP Reduction + DCL31-C/DCL07-C Include Path Support

Removed library-specific whitelists from DCL31-C/DCL07-C and added `-I` include
paths for system-installed third-party headers. EXP33-C conditional-init, cast
unwrap, and array arg fixes.

## v0.3.37 (2026-03-23)

### FIO30-C: CWE-134 Format String Taint Improvements

FIO30-C was producing zero CWE-matched detections on all 3,360 CWE-134 Juliet files.
Three fixes unlock detection across all taint source patterns.

**recv/recvfrom/recvmsg taint tracking**
- Added socket receive functions to `process_string_manipulation_call` — 2nd argument (buffer) is now marked as tainted
- Handles cast expressions in buffer arg: `recv(sock, (char *)(data + offset), ...)`
- Unlocks connect_socket and listen_socket Juliet patterns (~1,232 files)

**Macro alias resolution**
- Integrated `const_eval::collect_macro_aliases` to resolve `#define GETENV getenv` and similar macro indirections
- All function name lookups in taint tracking, source detection, and sink detection now go through `resolve_func_name()`
- Unlocks environment Juliet patterns (~616 files)

**get_base_variable expression handling**
- Added support for `cast_expression`, `parenthesized_expression`, and `binary_expression` in base variable extraction
- `(char *)(data + dataLen)` now correctly resolves to base variable `data`

Result: All 5 CWE-134 taint sources now produce FIO30-C detections (was 2/5). Variant 01 across all source×sink combinations: 15/15 detected.

### Benchmark: v0.3.35–v0.3.37 Combined (vs v0.3.34)

**Juliet** (fast mode, 68 CWEs): TP 8,300→8,508 (**+208**), FP 9,157→9,067 (**-90**), TP rate 47.5%→**48.4%** (+0.9pp). Per-file 13.8%→14.3%. Zero regressions.

| Change | TP Δ | FP Δ | Impact |
|--------|-----:|-----:|--------|
| CWE-761 (API07-C free-after-arithmetic) | +104 | 0 | New CWE, 100% TP, 15.5% per-file |
| CWE-134 (FIO30-C recv taint) | +113 | +23 | TP rate 47.9%→59.9% (+12pp) |
| CWE-464 (STR03-C sentinel) | +14 | 0 | New CWE, 100% TP, 25.0% per-file |
| CWE-469 (ARR36-C strchr) | +12 | 0 | New CWE, 100% TP, 33.3% per-file |
| CWE-843 (API07-C type confusion) | +12 | 0 | New CWE, 100% TP, 12.0% per-file |
| CWE-457 (EXP33-C arr[0].field) | -13 | -72 | TP rate 32.2%→35.3% (+3.1pp) |
| CWE-665 (EXP33-C initializers) | -27 | -41 | TP rate 41.3%→41.9% (+0.6pp) |

**Real-world** (5 codebases, sqc-only): 152,590→153,568 (**+978**, +0.6%).

| Codebase | v0.3.34 | v0.3.37 | Delta |
|----------|--------:|--------:|------:|
| hostap | 61,681 | 62,152 | +471 |
| sqlite | 50,517 | 51,091 | +574 |
| curl | 24,858 | 24,891 | +33 |
| mosquitto | 15,221 | 15,128 | -93 |
| libcrc | 313 | 306 | -7 |

Per-rule: ARR36-C **+2,727** (strchr tracking fires broadly on real-world pointer arithmetic), ERR33-C **-818** (printf suppression), ARR00-C **-480** (pointer chain fix), EXP33-C **-477** (arr[0].field + suffix matching). Excluding ARR36-C, net **-1,749** FP reduction.

**Source commits pinned** in BENCHMARK_INSTALL.md for all 5 codebases.

## v0.3.36 (2026-03-23)

### Juliet Zero-Detection CWE Coverage: +114 TPs

Three previously-zero-detection CWEs now have detections, all with 100% precision.

**ARR36-C: CWE-469 — strchr/wcschr cross-array subtraction (36 files)**
- `PointerAnalyzer` now tracks `strchr`, `strrchr`, `wcschr`, `wcsrchr`, `memchr`, `strstr`, `wcsstr`, `strpbrk`, `wcspbrk` return values as pointing into their first argument
- Array declarations (`char arr[] = ...`) now treated as their own base (not aliased to initializer)
- Assignment expressions tracked (not just declarations) for pointer origin resolution
- Result: 36/36 TP, 0 FP

**STR03-C: CWE-464 — (char)atoi() null sentinel detection (56 files)**
- Detects `(char)atoi(...)` pattern: `atoi()` returns 0 on failure, which becomes null terminator `'\0'` when cast to char, truncating strings
- Also covers `strtol`, `strtoul`, `atol` with char cast
- Result: 38/38 TP, 0 FP (remaining 18 are cross-function variants)

**API07-C: CWE-843 — void* type confusion detection (100 files)**
- Tracks `void*` variable assignments: `data = &charBuffer` records source type
- Detects dereference with incompatible cast: `*((int*)data)` where cast type is larger than source type
- Type size comparison: char(1) < short(2) < int(4) < long(8)
- Result: 40/40 TP, 0 FP (remaining 60 are cross-function variants)

## v0.3.35 (2026-03-23)

### Real-World FP Reduction + CWE-761 Detection

Four rule improvements targeting real-world false positives and Juliet coverage.

**EXP33-C: arr[0].field tracking + initializer suffix matching**
- `extract_nested_base_ex()` recurses through nested subscript/field chains to resolve base variable (`arr[0].field` → `arr`)
- `MallocUninitialized` + subscript-in-chain → `MallocInitialized` (fixes dominant hostap FP pattern)
- Stack `Uninitialized` + subscript-in-chain preserved (partial-init detection for `team[0].x`/`team[3].x`)
- `match_initializing_function()` suffix-matches wrapper functions: `os_memset` → `memset`, `wp_memcpy` → `memcpy`, etc.
- `*zalloc()` recognized as zero-initializing allocator (`os_zalloc`, `wpa_zalloc`)

**ARR00-C: Pointer subtraction chain resolution**
- `find_pointer_source_array()` now recursively resolves pointer derivation chains (depth limit 5)
- `end = pos + buflen; pos = buf;` → `end` resolves to base `buf`, same as `pos`
- Eliminates FPs on `end - pos` buffer arithmetic (dominant ARR00-C FP pattern)

**ERR33-C: Suppress printf-family return value checks**
- All formatted output functions suppressed: `fprintf`, `sprintf`, `snprintf`, `vprintf`, `vfprintf`, `vsprintf`, `vsnprintf`
- Rationale: checking return values is impractical — failures are rare and unrecoverable. Applies equally to stderr and file output (serialization code calls fprintf hundreds of times).

**API07-C: CWE-761 free-pointer-not-at-start detection**
- New detection: `free(ptr)` where `ptr` was modified by pointer arithmetic after allocation
- Tracks `malloc`/`calloc`/`realloc` assignments, detects `ptr++`/`ptr+=N`/`ptr--` modifications, flags subsequent `free(ptr)`
- Handles reassignment resets and reallocation resets
- 984 portable Juliet CWE-761 test files; 5/5 spot-checked detect correctly

**Other**:
- `BENCHMARK_INSTALL.md`: Updated all sqc examples from `rules-all.toml` to `rules-benchmark.toml` (matches MCP server)

### Benchmark

See v0.3.37 entry for combined v0.3.35–v0.3.37 benchmark results (Juliet + real-world).

## v0.3.34 (2026-03-23)

### EXP33-C: Preproc recursion fix + field-write refinement

- **Critical bug fix**: Functions inside `#ifdef`/`#ifndef`/`#if` preprocessor blocks were silently skipped by EXP33-C's AST recursion. The guard condition `node.kind() != "translation_unit"` excluded all non-top-level parents (preproc_ifdef, preproc_if, etc.). This made EXP33-C completely dead on Juliet (all test functions are inside `#ifndef OMITBAD`). Removed the incorrect skip condition.
- **Field-write MallocUninitialized preservation**: Re-applied after verifying it only affects `MallocUninitialized` state (heap allocations), not `Uninitialized` (stack variables). Field writes (`ptr->field = val`) on malloc'd pointers no longer upgrade to MallocInitialized, correctly flagging reads of other uninitialized fields/flexible array members. Stack struct field writes (`emp.id = 1`) still upgrade Uninitialized → Initialized.
- **Reverted `extract_field_base` nested subscript resolution**: Resolving `arr[0].field` → `arr` caused `arr` to be marked Initialized on first element write, suppressing valid partial-init detections. Kept original behavior (returns empty for nested subscript+field patterns).

### Benchmark: v0.3.34

**Juliet** (vs v0.3.32): TP 8,156 → 8,300 (**+144**), FP 9,180 → 9,157 (**-23**), TP rate 47.0% → **47.5%** (+0.5pp). EXP33-C: 294/561 → 431/559 (+137 TP, -2 FP). CWE-758: 50.1% → 64.3% (+14.2pp).

**Real-world** (vs v0.3.32): 150,337 → 152,590 (+2,253). EXP33-C +2,899 (hostap +2,525 dominant) from `arr[0].field` tracking limitation. API00-C -624 (improvement). Net regression — EXP33-C FP reduction is Priority 1.

## v0.3.33 (2026-03-23)

### EXP33-C: CFG-Based Forward Dataflow Rewrite

Complete rewrite of EXP33-C from simple AST-walk to CFG-based forward dataflow analysis. New `init_state.rs` (~1,200 lines) implements proper initialization-state tracking.

**Architecture**:
- **InitState lattice**: 5 states per variable — `Uninitialized`, `MaybeUninitialized`, `Initialized`, `MallocUninitialized`, `MallocInitialized`
- **Forward worklist algorithm**: Propagates init state through CFG with lattice join at merge points
- **Point queries**: `get_var_info_at()` answers "what is variable X's state at byte offset Y?" by replaying transfer function from block entry

**New capabilities**:
- Branch-sensitive: `if (c) { x = 1; } else { x = 2; } use(x)` → correctly Initialized
- Loop-aware: Array init loops (`for (i=0; i<n; i++) arr[i] = 0`) upgrade MallocUninitialized → MallocInitialized via join rule
- Malloc content tracking: Distinguishes pointer-set (MallocUninitialized) from content-written (MallocInitialized)
- Realloc wrapper detection: Pre-scans for functions wrapping `realloc()` without `memset`; `classify_initializer` checks `realloc_wrapper_fns` from config
- Conditionally-initializing function detection: Pre-scans for functions that only init pointer params inside if-blocks
- ~30 initializing functions with per-argument output indices (memset, fgets, scanf, etc.)
- Non-initializing function list (printf, strlen, memcmp, etc.)
- File-scope static variable tracking
- Subscript read extraction through field_expression chains (`arr->data[i]` → root variable `arr`)

**Also includes**:
- API00-C refinements
- CFG `condition_range` support
- Realworld benchmark MCP server enhancements (SQLite-first pattern, `get_rule_trend`, `get_project_history`)
- Legacy shell scripts removed (batch_analyze_sqlite, bench_mosquitto_versions, compare_tools, run_juliet_multi_cwe, run_juliet_parallel, test_single_file)
- bench/db.py improvements

**Tests**: 67/67 EXP33-C tests pass (52 integration + 15 unit). Covers branches, loops, malloc, calloc, memset, va_start, fgets, struct fields, subscript writes, goto, switch, early return, compound assignments, sizeof exclusion, unsigned char exception, realloc wrappers, conditional-init functions, flexible array members.

## v0.3.32 (2026-03-22)

### Prescan Quality Audit + DCL07-C/DCL31-C FP Reduction (−11,556 real-world violations, −7.1%)

Total: 161,893 → 150,337 across 5 codebases. Juliet: zero change (47.0% TP rate preserved).

**Root cause analysis**: Audited all 16 rules consuming prescan `ProjectContext`. Found three root causes for DCL07-C/DCL31-C FPs:

1. **Prescan deep recursion** (`prescan.rs`): `collect_function_names` only recursed into `preproc_*` blocks. Tree-sitter misparses files with complex macros (e.g., sqlite's `sqliteInt.h`), burying `function_definition` nodes inside `ERROR` or `compound_statement` at the top level. Now recurses into ALL child nodes. Found 358 additional functions in sqlite alone.

2. **Macro alias integration** (`dcl07_c.rs`, `dcl31_c.rs`): Prescan collected `macro_aliases` (`#define ALIAS target`) but DCL rules didn't check them. Now adds all alias names to `cross_file_functions` in `set_project_context()`.

3. **External library whitelist + `defined` operator** (`dcl07_c.rs`, `dcl31_c.rs`): Added `is_external_library_function()` covering OpenSSL (`SSL_`, `BIO_`, `X509_`, `EVP_`, `PEM_`, `ERR_`), Tcl/Tk (`Tcl_`, `Jim_`), Apple CoreFoundation (`CF*`), mbedTLS (`mbedtls_`), cJSON (`cJSON_`), zlib, GnuTLS, wolfSSL. Skip `defined` (preprocessor operator parsed as call_expression by tree-sitter).

**Per-rule impact**:
- **DCL07-C**: 10,617 → 4,765 (**−55.1%**)
- **DCL31-C**: 10,543 → 4,840 (**−54.1%**)

**Per-project impact**:

| Project | v0.3.31 | v0.3.32 | Delta |
|---------|--------:|--------:|------:|
| sqlite | 58,789 | 50,616 | −8,173 (−13.9%) |
| mosquitto | 16,609 | 15,164 | −1,445 (−8.7%) |
| hostap | 60,609 | 59,549 | −1,060 (−1.7%) |
| curl | 25,571 | 24,693 | −878 (−3.4%) |
| libcrc | 315 | 315 | 0 |

**Remaining DCL07-C/DCL31-C FPs** (9.6K combined): Macro definitions in files tree-sitter can't parse (e.g., curl's `curl_setup.h` — `curlx_free` et al.), and project-internal functions buried in unrecoverable parse errors (e.g., sqlite's `prepare.c:937` — `sqlite3_prepare_v2`). Diminishing returns without preprocessor expansion.

## v0.3.31 (2026-03-22)

### FP Reduction: 5 Rules (−20,127 real-world violations, −11.1%)

Total: 182,020 → 161,893 across 5 codebases.

- **INT07-C**: Skip `char *` pointer/array declarations in `is_plain_char_declaration`. 96.5% of violations were pointer arithmetic on `char *`, not `char` value operations. Added `is_pointer_or_array_declaration()` AST check. 6,230 → 207 (**−96.7%**).
- **ERR33-C**: Fixed duplicate violation bug (standalone calls emitted via both `expression_statement` and `call_expression` paths). Suppressed printf/puts/putchar/fputs/fputc/putc diagnostic output. Suppressed `fprintf(stderr, ...)`. Suppressed `signal(SIG*, SIG_IGN/SIG_DFL)`. Suppressed `time(&t)` with output parameter. Recognized `==0`/`!=0` as NULL checks. Added `putenv`/`strdup`/`strndup` to `is_error_returning_function`. 7,182 → 1,807 (**−74.8%**).
- **MSC41-C**: Removed overly broad substring keywords (`db`, `connect`). `key`/`token` require strict word boundaries. `auth`/`login`/`pwd`/`database` require leading word boundary. Always apply `looks_like_sensitive_data()` heuristic in function context (previously flagged ALL non-empty strings). Added relaxed `looks_like_sensitive_in_context()` that skips format strings, debug labels, error messages, algorithm identifiers. 3,954 → 293 (**−92.6%**).
- **ARR00-C**: Rewrote `is_array_identifier()` from text-search (`name[` in function) to AST-based `has_array_declaration()` that walks for `array_declarator` nodes. Only flags true arrays, not subscripted pointers. Guarded `check_uninitialized_array_read`, `check_constant_out_of_bounds`, `check_subscript_bounds` with array-only checks. Fixed `check_array_assignment` to skip compound assignments (`+=`, `-=`). 7,341 → 2,637 (**−64.1%**).
- **EXP33-C**: Added `scanf`/`fscanf`/`sscanf` `&var` argument initialization tracking (all pointer args marked as initialized). Added for-each macro recognition (15 common macros: `dl_list_for_each`, `TAILQ_FOREACH`, etc.) that marks iterator variables as initialized. 4,554 → 4,189 (**−8.0%**).

### Juliet Benchmark: v0.3.31

- Overall: 8,156 TP / 9,180 FP, **47.0% TP rate** (−0.4pp vs v0.3.30)
- Zero FP change. −129 TP from intentional printf suppression (CWE-252: −119, CWE-391: −10)
- No regressions on any other CWE

### Real-World: Per-Project

| Project | v0.3.30 | v0.3.31 | Delta |
|---------|--------:|--------:|------:|
| hostap | 69,329 | 60,609 | −8,720 (−12.6%) |
| sqlite | 66,435 | 58,789 | −7,646 (−11.5%) |
| curl | 27,640 | 25,571 | −2,069 (−7.5%) |
| mosquitto | 18,199 | 16,609 | −1,590 (−8.7%) |
| libcrc | 417 | 315 | −102 (−24.5%) |

## v0.3.30 (2026-03-22)

### FP Reduction: EXP34-C Count-Based Callsite Aggregation

- Replaced lattice join in `aggregate_callsite_null_states()` with count-based voting. One PossiblyNull callsite no longer poisons parameters with 50 NotNull callers. DefinitelyNull always propagates; PossiblyNull requires majority.
- EXP34-C: 26,457 → 5,290 (**−80.0%**) across 5 real-world codebases.

### Juliet Benchmark: v0.3.30

- Overall: 8,285 TP / 9,180 FP, **47.4% TP rate** (−0.1pp vs v0.3.28)
- CWE-690 TP rate 83.8% → 94.3% (+10.5pp). Zero regressions.

## v0.3.28 (2026-03-21)

### Prescan Cache

- **`--save-prescan FILE` / `--load-prescan FILE`**: New CLI flags to serialize/deserialize `ProjectContext` to a binary cache file (bincode format). Eliminates repeated prescan overhead in parallel scanning — each worker loads the cache instead of re-scanning the full codebase.
- `ProjectContext`, `FunctionSummary`, `NullState`, `ValueRange` gain `Serialize`/`Deserialize` derives. Cache includes: known_functions, function_summaries, call_graph, macro_constants, macro_aliases, struct_field_types.
- Cache sizes: ~3.6 MB for hostap (12K functions), ~2.4 MB for sqlite (312 files).
- **Persistent cache**: `data/prescan_cache/{codebase}.cache` files reused across benchmark runs. `--rebuild-prescan` flag forces regeneration after prescan logic changes.
- Per-worker speedup: hostap eap_example 11.2s → 0.8s (93% faster), sqlite prescan 866s → 0s (reuse).
- New dependency: `bincode = "1.3"`.

### Parallel Scanner Improvements

- Balanced subdirectory splitting: recursive `find_scan_units()` splits large directories (e.g., hostap/src/ 361 files → 20 subdirs) while keeping directories with root-level .c files intact to avoid overlap.
- Prescan cache integration: generates cache once, passes `--load-prescan` to each parallel worker.
- Prescan timeout increased from 600s to 1800s (sqlite's 250K-line files need >10 min).
- Graceful fallback: `TimeoutExpired` caught, falls back to per-worker `-d` mode.

## v0.3.27 (2026-03-21)

### Benchmark Infrastructure

- **`rules-benchmark.toml`**: New benchmark-specific manifest with 13 style/recommendation rules disabled (zero Juliet CWE contribution, ~114K realworld violations of pure noise): EXP19-C, DCL08-C, DCL06-C, EXP02-C, EXP14-C, EXP12-C, EXP10-C, DCL04-C, INT02-C, INT01-C, INT17-C, INT16-C, PRE31-C. `rules-all.toml` unchanged for end users.
- **Parallel sqc scanner** (`scripts/sqc_parallel_scan.py`): Splits codebases by subdirectory and runs multiple sqc processes via `ProcessPoolExecutor`. Auto-detects parallelism (falls back to single process for <50 files). Deduplicates merged JSON outputs.
- All benchmark infrastructure (bench/config.py, MCP servers, shell scripts) updated to use `rules-benchmark.toml`.
- MCP realworld server uses parallel scanner for all sqc scans, with `rebuild_prescan` parameter on `run_analysis()`/`run_all()`.

### FP Reduction: POS49-C

- **POS49-C**: Restricted from flagging all shared struct field writes to only actual bit-field writes. `collect_bitfield_names()` identifies fields declared with `bitfield_clause`, only flags `assignment_expression`/`update_expression` to known bit-fields. 15,693 → 107 violations (99.3% reduction).

### Benchmark: Juliet v0.3.28

- Overall: 8,390 TP / 9,252 FP, **47.6% TP rate** (+0.3pp vs v0.3.26), 14.0% per-file
- TP rate improvement from disabled noise rules no longer contributing FPs to CWE-matched analysis

### Real-World: −89,002 violations (−40.7%) across 4 codebases

| Project | v0.3.27 | v0.3.28 | Delta |
|---------|--------:|--------:|------:|
| hostap | 141,810 | 78,470 | −63,340 (−44.7%) |
| curl | 50,532 | 31,722 | −18,810 (−37.2%) |
| mosquitto | 25,769 | 19,202 | −6,567 (−25.5%) |
| libcrc | 704 | 419 | −285 (−40.5%) |

All deltas from 13 disabled noise rules. Zero regressions on any active rule.

## v0.3.26 (2026-03-20)

### FP Reduction: 6 Rules

- **EXP33-C**: Conditional init early-return detection — if/else-if/else chains where non-initializing branches have unconditional exits (return/break/continue/goto) no longer flag the variable as uninitialized. New `if_chain_covers_init_or_exit()` analysis integrated into `block_has_preceding_assignment()`. Targets real-world patterns (`arraylist.c:132`, `intset.c:124`).
- **ERR33-C (CWE-253)**: `== 0` for NonZero functions (fseek, fclose, fflush, remove, rename, fsetpos, atexit, raise) no longer flagged as incorrect check. `== 0` is a valid success-path pattern. Also: `== 0` for Count functions (fread, fwrite) no longer flagged — only `< 0` on unsigned size_t is genuinely incorrect.
- **FIO47-C**: Fixed 100% FP rate (0 TP, 119 FP → 0 FP). Root cause: `count_arguments()` miscounted non-data args for `sprintf`, `sscanf`, `dprintf` (skipped 1 instead of 2). Also relaxed overly strict format validation: `%lf` valid in C99+, removed `'` flag and `+/space` with `%o/%u/%x` checks.
- **PRE00-C**: Restricted from flagging ALL function-like macros to only macros with multi-evaluation risk (parameter used >1 time in body) or side effects (++/-- in body). Estimated ~1,700 real-world FP reduction.
- **EXP34-C**: Pointer function parameters default to NotNull when no inter-procedural call-site data exists (callers responsible for null checks). With prescan/`-d`, call-site states still override. Test cases restructured: 3 param-null tests moved to pass/, 2 wiki tests rewritten to use malloc.
- **FIO47-C format validation**: `l` length modifier with float specifiers (`%lf`) now accepted (valid C99+). `'` grouping flag and `+/space` with unsigned specifiers no longer flagged.

### Benchmark: Juliet v0.3.26

- Overall: 8,390 TP / 9,252 FP, **47.6% TP rate** (+0.3pp vs v0.3.25), 14.0% per-file
- FIO47-C: 119 FP → 0 FP (100% FP elimination). CWE-134 TP rate 33.4% → 47.9% (+14.5pp)
- CWE-685: 0 TP / 6 FP → 3 TP / 0 FP (arg count fix revealed true positives)
- CWE-253: −33 TP (ERR33-C `== 0` trade-off — Juliet treats success-path checks as incorrect)

### Real-World: −5,366 violations (−1.5%)

| Project | v0.3.25 | v0.3.26 | Delta |
|---------|--------:|--------:|------:|
| hostap | 160,121 | 157,403 | −2,718 (−1.7%) |
| sqlite | 116,642 | 115,491 | −1,151 (−1.0%) |
| curl | 55,975 | 54,767 | −1,208 (−2.2%) |
| mosquitto | 26,470 | 26,182 | −288 (−1.1%) |
| libcrc | 705 | 704 | −1 |

Per-rule: PRE00-C −4,274, EXP34-C −1,758, EXP33-C −163, FIO47-C −149.
Cumulative from v0.3.5: 402,633 → 354,547 (−48,086, −11.9%).

## v0.3.25 (2026-03-20)

### CWE-457: EXP33-C Detection Improvements

- **ALLOCA/alloca tracking**: `alloca()` and `ALLOCA()` allocations now treated as uninitialized memory (like `malloc`), catching reads from uninitialized stack-allocated arrays. +52 new TPs across alloca no_init variants 01–18.
- **Conditional init heuristic fix**: `check_conditional_init_pattern` broadened from incomplete conditionals (if without else) to any conditional body. Combined with new `inits_share_conditional` check: assignments in separate independent if-else blocks no longer falsely suppress violations. +18 new TPs for variant 12 (`globalReturnsTrueOrFalse()` pattern).
- **INT31-C VRA improvement** (from v0.3.24): CWE-194 TP rate 41.7%→56.8% (+15.1pp, −310 FP/−31 TP). CWE-195: −32 FP, 0 TP.

### Benchmark: Juliet 68/68 CWEs

- Overall: 8,420 TP / 9,371 FP, **47.3% TP rate** (+0.6pp vs v0.3.24), 14.1% per-file
- CWE-121 (new): 1,027 TP / 1,152 FP (47.1% TP rate) — first full benchmark for stack buffer overflow
- CWE-194: 41.7%→56.8% (+15.1pp), CWE-195: 40.8%→42.2% (+1.4pp) — INT31-C VRA FP reduction
- CWE-457: 34.7%→31.8% (−2.9pp) — broader detection (+33 TP) but +109 FP from alloca/conditional init changes
- CWE-758: 51.9%→50.1% (−1.8pp), CWE-690: 82.4%→82.0% (−0.4pp) — minor regressions
- Zero regressions on remaining 62 CWEs

## v0.3.24 (2026-03-19)

### VRA Phase 5: Inter-Procedural Return Ranges

- `FunctionSummary` gains `return_range: Option<ValueRange>` — computed during prescan by evaluating all `return expr;` statements as constant ranges (literals, macros, sizeof, arithmetic). Conservative: `None` if any return is parameter-dependent or unevaluable.
- VRA transfer function resolves `call_expression` RHS in assignments and declarations: `int x = get_count();` uses callee's return range instead of full type range.
- Return ranges stored in `RangeAnalysisResult` for intra-block replay consistency in `eval_expr_range_at()` / `get_var_range_at()`.
- Prescan reordered: macro constants collected before function summaries so `#define`-based return values resolve correctly.
- Benefits all 4 VRA-consuming rules (INT30-C, INT32-C, INT33-C, INT34-C) — e.g., `x = get_nonzero(); y / x;` no longer flagged by INT33-C when `get_nonzero` provably returns `[1, N]`.

### VRA Phase 6: INT31-C Migration

- INT31-C (integer conversion/truncation) gains full VRA integration, becoming the 5th VRA-consuming rule.
- VRA-based range narrowing supplements the existing syntactic `is_inside_bounds_checked_block()` heuristic: if VRA proves the value fits in the target type's range at the cast/assignment site, the violation is suppressed.
- Covers all three check sites: cast expressions (signed→unsigned, unsigned→signed, narrowing), implicit assignment narrowing, and signed→size_t call arguments.

### Benchmark: Juliet 67/68 CWEs

- Overall: 7,393 TP / 8,433 FP, **46.7% TP rate** (+2.4pp vs v0.3.21), 13.9% per-file
- INT32-C: −63 FP, INT33-C: −33 FP, INT30-C: −5 FP (VRA Phases 1–5)
- CWE-124: 36.9%→52.6%, CWE-122: 36.6%→43.8%, CWE-126: 37.2%→43.8% (ARR FP fixes from v0.3.22)
- Zero regressions across all CWEs
- **Performance fix**: ~6x speedup on large non-VRA CWEs. Root cause: `compute_return_range` ran during prescan for all 105K+ Juliet files even when no VRA rules were enabled. Fix: `compute_summaries` takes `compute_return_ranges: bool` flag, prescan passes `needs_vra` from manifest. Per-file macro/summary computation in analysis loop moved behind `needs_vra` guard. CWE-121 with prescan: 14m29s → 2m23s. VRA CWEs unchanged.

## v0.3.23 (2026-03-19)

### CFG-Based Forward Value-Range Analysis

New `value_range.rs` module implements proper forward dataflow on the CFG, replacing syntactic ancestor walks for integer range reasoning. Follows the same worklist pattern as `null_state.rs`.

**Phases 1–4: Core engine + rule migration**

- **Core VRA engine**: worklist algorithm with interval lattice, edge refinement for all comparison operators (`<`, `<=`, `>`, `>=`, `==`, `!=`), compound conditions (`&&`, `||`), negation, and bare identifier conditions
- **Type-aware initial ranges**: `unsigned int` → `[0, UINT_MAX]`, `int` → `[INT_MIN, INT_MAX]`, etc. Extracts signedness and bit width from declaration AST
- **Widening**: after 3 iterations of back-edge targets, growing dimensions widen to type bounds — guarantees termination for loops
- **Caching**: VRA computed once per file per function, shared across all rules via `set_vra_results()` trait method; only computed when at least one enabled rule requests it via `needs_vra()`
- **INT33-C**: `divisor_provably_nonzero()` tries VRA first via `eval_expr_range_at()`, falls back to syntactic analysis. Handles early-return guard patterns (`if (b == 0) return;`) and sequential assignments across blocks
- **INT34-C**: `check_shift_operation()` tries VRA first for shift amount range, falls back to syntactic analysis
- **INT32-C**: all 6 `expression_fits_in_signed` call sites replaced with VRA-backed `expression_fits_in_signed_vra()`
- **INT30-C**: all 4 `expression_fits_in_unsigned` call sites replaced with VRA-backed `expression_fits_in_unsigned_vra()`
- Added `PartialEq` derive to `ValueRange` in `const_eval.rs`

## v0.3.22 (2026-03-19)

### ARR38-C/ARR30-C False Positive Reduction

- ARR38-C: function-scoped alias resolution — `collect_pointer_aliases` now runs per-function instead of file-wide, preventing cross-function contamination (e.g., `data = dataBadBuffer` vs `data = dataGoodBuffer`). Eliminates ~69 CWE805 FPs.
- ARR38-C: skip heuristic checks when buffer size is verified — `is_hardcoded_large_size` no longer fires when `check_size_exceeds_buffer` already confirmed the copy fits the known buffer.
- ARR30-C: multi-assignment constant resolution — `try_resolve_variable_to_constant` now resolves to the last value when ALL assignments are constants (handles `data = -1; data = 7;` goodG2B patterns). Eliminates ~67 CWE129 FPs.
- ARR38-C CWE806 (−183 FP): `strncat(dest, data, strlen(data))` compared buffer allocation size instead of actual content. Fix: function-scoped `find_content_size_in_function()` tracks `memset(var, char, N)` and uses N as effective strlen bound.
- ARR30-C CWE129 (−67 FP): `check_if_bounds_against_size` searched full if-body text (matched for-loops). Fix: extract only `parenthesized_expression` condition from AST.

## v0.3.21 (2026-03-19)

### CWE-121/122: Buffer Overflow Detection

- ARR30-C: literal loop bounds, ALLOCA tracking, pointer alias tracking
- ARR38-C: ALLOCA detection, strlen/wcslen overflow, snprintf variants, pointer alias resolution, N*sizeof(type) parsing
- Benchmark: CWE-121 39.3%→39.9% TP rate (+205 TP, +281 FP), CWE-122 41.7%→36.6% (−5.1pp, +43 TP, +134 FP)

## v0.3.20 (2026-03-18)

### Benchmark Infrastructure Overhaul

- New `bench/` package replaces shell scripts with Python runner + SQLite
- `bench/runner.py`: `ProcessPoolExecutor`-based parallel CWE runner, writes directly to `data/benchmarks.db`
- `bench/analyzer.py`: TP/FP classifier extracted from `analyze_juliet_results.py`, returns structured data
- `bench/db.py`: SQLite schema (7 tables), WAL mode, full CRUD + query API
- `mcp_servers/server.py`: Updated to launch `python -m bench juliet`, queries SQLite first with legacy fallback
- `scripts/backfill_juliet_results.py`: Imported 21 Juliet runs + 7 real-world runs from markdown docs
- Fast mode default, resume support, machine metadata collection

### First 68-CWE Fast Benchmark

- Overall: 8,413 TP / 10,484 FP, 44.5% TP rate, 14.0% per-file
- 10 CWEs at 100% precision, 24 at zero detection
- 48 min on 4-core i5-6200U

## v0.3.19 (2026-03-15)

### CWE-78: ENV03-C + STR02-C Improvements

- ENV03-C: function-scoped clearenv() — checks sanitization per-function instead of file-level
- STR02-C: intra-function taint tracking (recv, fgets, fgetws, scanf, getenv, etc.) with cast handling and propagation
- Precision 42.0% → 45.5%, FP −330, TP −78 (cross-function patterns remain undetected)

## v0.3.18 (2026-03-14)

### Fast Benchmark Mode (CWE-Focused Manifests)

- `generate_rule_cwe_map.py` generates 147 per-CWE manifest TOMLs in `rules_templates/cwe/`
- `run_juliet_parallel.sh --fast` uses per-CWE manifests for targeted scanning
- Validated on CWE-476: noise drops from 61.8% → 0%, TP rate 39.5% → 46.5%, per-file detection unchanged (29.0%)

### CWE-194/195: Signed-to-size_t Implicit Conversion Detection (Priority 8)

- **INT31-C**: Added `check_call_argument_conversion()` — detects signed integer variables (`short`, `int`, `int32_t`, etc.) passed to functions expecting `size_t`. Covers 20 standard library functions including `malloc`, `calloc`, `realloc`, `memcpy`, `memmove`, `memset`, `strncpy`, `strncat`, `snprintf`, `fread`, `fwrite`, etc.
- Suppressions: explicit cast `(size_t)data`, `sizeof` expressions, numeric literals, limit-macro bounds check, non-negative guard
- Previously 0% CWE-matched detection on 2,688 Juliet files

### Continued FP Fixes

- Various false positive reduction improvements

## v0.3.17 (2026-03-12)

### CWE-78: Macro Alias + Windows API Coverage (Priority 6)

- Added `collect_macro_aliases()` to const_eval.rs — collects `#define ALIAS identifier` patterns
- ENV33-C, ENV03-C, STR02-C now resolve macro aliases before matching dangerous function lists
- Added Windows exec/spawn variants to ENV33-C: `_execl`, `_execv`, `_execlp`, `_execvp`, `_execle`, `_execve`, `_spawnl`, `_spawnle`, `_spawnlp`, `_spawnv`, `_spawnve`, `_spawnvp`
- **Benchmark**: CWE-78 CWE-matched TP 1,282, FP 1,773, precision 42.0%, per-file 13.0%

### CWE-253: Incorrect Return Value Check (Priority 7)

- ERR33-C now validates comparison correctness when a function call is directly embedded in a `binary_expression`
- Functions classified by `ErrorReturnKind`: NullPointer, NegativeInt, Eof, NonZero, Count
- Detects: pointer functions with ordered comparison, negative-on-error compared `== 0`, EOF-returning compared `== 0`, non-zero-on-error compared `== 0`, count-returning compared `< 0`
- Macro alias resolution added to ERR33-C; extended function coverage to wchar_t variants
- **Benchmark**: CWE-253 CWE-matched TP 178, FP 0, **100% precision**, 26.0% per-file detection

## v0.3.15 (2026-03-12)

### CWE-Aware Scoring System

- Implemented 5 new benchmark metrics: FLAW-line hit rate, CWE-matched TP rate, per-file detection rate, noise ratio, incidental TP/FP
- `scripts/generate_rule_cwe_map.py` produces `data/rule_cwe_map.json` (117 rules to 144 CWEs)
- Analysis pipeline fully integrated: `analyze_juliet_results.py`, `mcp_servers/server.py`, `run_juliet_parallel.sh`
- **Key finding**: 95% of Juliet findings are noise from unrelated rules; CWE-matched TP rate is 45.6% vs 44.4% incidental

### CWE Mapping Fixes

- Added CWE-124, CWE-126, CWE-127 mappings to ARR30-C, ARR38-C, STR31-C (buffer underwrite/overread/underread)

## v0.3.14 (2026-03-11)

### Juliet Regression Investigation

- Full-suite benchmark: 126,106 TP, 158,036 FP, 44.4% TP rate (-0.2pp from v0.3.5)
- Investigated 5 suspected root causes — all ruled out as dominant
- Confirmed regression is cumulative effect of many individually correct suppressions
- Discovered two scoring methodology issues: off-by-one in FLAW-line matching, incidental noise scored as TP

## v0.3.13

### EXP34-C Multi-Pass Prescan Propagation

- Added `propagate_param_null_states()` — multi-pass prescan resolves relay chains: `high(p) { if(!p) return; mid(p); }` → `mid(p) { low(p); }` → `low(p) { *p = 42; }` — `p` now NotNull at `low`

### EXP33-C For-Loop Init Recognition

- `has_preceding_assignment_in_block()` walks ancestor scopes
- `for_init_assigns_var()` recognizes for-statement init clauses as dominating assignments
- Handles `for (i = 0; ...)`, `for (int i = 0; ...)`, and comma expressions

### INT30-C Subtraction Guard

- `is_subtraction_guarded_by_comparison()` detects `if (a >= b) { a - b }` patterns
- Supports `>=`, `>`, `<=`, `<` and compound `&&` conditions
- Generalized `1U`/`1u` suffix handling in loop-bound and compound addition checks

## v0.3.8

### STR31-C Function Parameter Guard

- Gated string-literal suppression on `!is_function_parameter()` in `check_strcpy_safety` and `check_strcat_safety`
- Fixed `check_sequential_strcat_overflow` to scan only current function's line range
- Expected recovery: ~300-400 TPs on CWE-124/127

### EXP33-C Field/Subscript Write Fix

- Field/subscript write no longer treated as read (-576 FP, -299 TP on 12-CWE Juliet)

### API00-C Validation Pattern Expansion

- 4 new validation patterns recognized for parameter checking

## v0.3.5

### Struct Field Type Resolution

- Prescan collects struct definitions into `struct_field_types` in `ProjectContext`
- `infer_type()` resolves `field_expression` types (e.g., `s->count` → `unsigned int`)
- Integrated with INT32-C and INT30-C

## v0.3.3

### Suppression Elimination (d_lib_networking)

- **POS49-C**: Added `is_local_variable()` — skip stack-local struct member assignments
- **EXP12-C**: Already fixed — `connect() != 0` in binary_expression not flagged
- **INT30-C**: `is_literal_one()` strips unsigned/long suffixes; `is_preceded_by_increment()` checks for `var++`/`++var`/`var += 1` before subtraction

## v0.2.25

### STR04-C, INT18-C, EXP05-C Type/Const Fixes

- STR04-C: binary buffer skip — only flag `unsigned char` arrays with string literal evidence
- INT18-C: uint64_t recognition via `type_identifier` nodes
- EXP05-C: AST-based const detection replacing text-based check
- d_lib_networking: 51 → 47 violations (-4 FP)

## v0.2.22

### ARR02-C, POS02-C, PRE31-C, MEM05-C Fixes

- ARR02-C: skip implicit bounds check for string-literal-initialized arrays
- POS02-C: removed `socket`/`setsockopt` from privileged operations
- PRE31-C: strip string literal content before function-call pattern checks
- MEM05-C: ALL_CAPS macro constant VLA detection + word-boundary recursion matching

### INT32-C While/For Loop-Bound Detection

- Extended `is_inside_bounds_checked_block()` to while/for statements
- `extract_mutation_target()` ensures loop-bounded variable matches operation target

### INT30-C uint64_t Subtraction Skip

- Skip subtraction when either operand has declared type `uint64_t`

### Const-Eval Negative-Shift Clamp

- `ValueRange::shl()` clamps negative shift-amount lower bounds to 0

## v0.2.21

### Const-Eval / Value-Range Analysis

- New `src/analyze/const_eval.rs` module (~550 lines)
- `MacroConstantMap` for `#define` constant collection
- `ValueRange { min, max }` interval arithmetic
- `try_evaluate_expr()` / `try_evaluate_range()` for recursive AST constant folding
- `extract_loop_var_ranges()` for for/while/do loop bounds
- Integration with INT32-C (`expression_fits_in_signed`) and INT30-C (`expression_fits_in_unsigned`)
- d_lib_networking INT32-C: 10 → 8 (-2 FP via constant folding)

### Benchmark Measurement Fix

- Analysis script now outputs all rules (previously top 10 only)
- All 16 existing benchmark runs reanalyzed with full per-rule data
- Eliminated phantom regressions from top-10 truncation (POS02-C, ERR05-C, MEM06-C were artifacts)

## v0.2.20

### Real-World FP Fixes (d_lib_networking, Rounds 1-4)

- MSC37-C: `STATIC void` macro prefix — `has_void_specifier()` scans all children for `void`
- INT36-C: `(void)` discard cast — bare `void` no longer matched as pointer type
- PRE02-C: trailing comment stripping in macro values
- ERR33-C: `(void)` cast recognized as intentional discard
- CON03-C: skip `const`-qualified variables and synchronization primitive types
- DCL30-C: scalar value copy through pointer no longer flagged as address escape
- FIO47-C: snprintf argument count corrected (subtract 3, not 1)
- EXP37-C: init_declarator skip for K&R-style declarations
- API00-C: skip static functions (-12 FP) + caller-aware suppression via NotNull

### `-I`/`--include-path` Flag

- Pre-pass extracts `#include` directives, resolves against `-I` search paths
- Transitive include resolution with cycle prevention
- d_lib_networking: 223 → 205 violations (-18) with 3 include dirs

### INT01-C Dedup Fix

- Eliminated double-visit of `function_declarator`/`parameter_list` nodes (-3 duplicate violations)

### EXP34-C Stack Array NotNull

- Array declarations tracked as NotNull in prescan

### Juliet Benchmark

- v0.2.19 → v0.2.20: -2,720 FP, +0.1pp TP rate (44.1% → 44.2%)

## v0.2.17

### EXP34-C Phase 3: API Rule Narrowing + Prescan Enhancement

- MEM10-C positive guard suppression (-38 FP)
- API02-C `const wchar_t *` exclusion
- API00-C caller-aware suppression via function summaries
- Prescan local variable tracking for callsite null state resolution
- CWE-476: TP 313→320 (+7), FP 542→512 (-30), rate 36.6%→38.5%
- CWE-690 bonus: +36 TP, -63 FP

## v0.2.16

### EXP34-C Phase 2: Call-Site Null Propagation

- Call-site flagging for DefinitelyNull args
- Callee param seeding via `infer_arg_null_state()` in function_summary.rs
- Multi-pass aggregation with lattice join
- CWE-476: +19 TP, +17 FP (rate 35.9% → 36.6%)

## v0.2.15

### EXP34-C Phase 1: CFG-Based Null State Dataflow

- `src/analyze/null_state.rs` — forward dataflow with NullState lattice
- EXP34-C rewritten from ~1200-line linear walk to CFG-based analysis
- MEM10-C parameter-only null check fix (-106 FP on CWE-476)

### d_lib_common FP.md Round 2 (v0.2.14-0.2.15)

- Resolved all 17 FP patterns (~51 violations) from FP.md
- Key fixes: FIO46-C source-order stream tracking, INT32-C field_expression skip, FLP03-C scientific notation, EXP12-C parent-check, INT01-C sizeof skip
- Juliet: -10,678 FP (-5.4%), TP rate 44.7% → 44.2%

## v0.2.13

### INT31-C Implicit Narrowing Assignment Detection

- `check_assignment_conversion()` for `init_declarator` and `assignment_expression`
- Type width comparison with FP suppressions (double-flag, validated vars, bounds-check, literal-fits, bitmask)
- Real-world: curl +24, hostap +156, sqlite +49 new findings
- Juliet: 44.6% → 44.7% TP rate, -13,961 FP (-6.6%)

### d_lib_common REFACTOR.md Round 1

- DCL19-C: `STATIC` macro recognition
- INT32-C: skip unsigned operands in binary overflow checks
- DCL15-C: skip functions with prototypes in `.h` headers
- INT36-C: exclude struct field access and array subscript
- PRE31-C: skip string literal arguments from side-effect analysis
- EXP30-C: recognize `x = f(x)` as safe
- INT30-C: detect `if (var > 0)` guard before unsigned decrement
- DCL07-C/31-C: skip indirect calls and preprocessor-guarded blocks

### Prescan Infrastructure Improvements

- `linkage_specification` (`extern "C" {}`) traversal in all prescan walkers
- `pointer_declarator` handling for pointer-returning prototypes
- `header_declared_functions` field in `ProjectContext`

# SqC — Running Benchmarks (MCP Server Guide)

**Last Updated**: 2026-03-17

How Claude uses the MCP benchmark servers to run Juliet and real-world benchmarks.

---

## Architecture

All benchmark results are stored in **`data/benchmarks.db`** (SQLite, WAL mode). The MCP server (`mcp_servers/server.py`) is a thin query layer over this database. The benchmark runner (`bench/runner.py`) writes directly to SQLite — no intermediate text files.

```
bench/
  __init__.py      Package marker
  __main__.py      CLI: python -m bench juliet [--full] [--jobs N]
  config.py        Paths, constants, defaults
  db.py            SQLite schema, WAL mode, CRUD + query API
  analyzer.py      TP/FP classifier (Juliet ground truth)
  runner.py        Parallel CWE runner
  machine.py       Machine metadata (CPU, RAM, hostname)
```

### SQLite Schema

| Table | Purpose |
|-------|---------|
| `runs` | One row per benchmark execution (version, SHA, mode, status, machine metadata) |
| `cwe_scans` | One row per CWE per run (file count, violations, duration, status) |
| `violations` | Every individual sqc finding with TP/FP classification |
| `cwe_metrics` | Pre-computed aggregates per CWE (TP/FP rates, CWE-aware metrics) |
| `rule_cwe_breakdown` | Per-rule per-CWE counts |
| `realworld_runs` | Real-world benchmark runs (sqc version, machine) |
| `realworld_results` | Per-project per-tool violation counts |

Historical data from `JULIET_RESULTS.md` and `REALWORLD_RESULTS.md` has been backfilled via `scripts/backfill_juliet_results.py`.

---

## Benchmark Workflow Protocol (CRITICAL)

1. **Version bump + commit BEFORE benchmark**: Always bump the version in `Cargo.toml`, rebuild (`cargo build --release`), and commit before starting a benchmark run. The run_id is `sqc-{version}-{sha}`.

2. **NEVER modify code while a benchmark is running**: The benchmark uses `target/release/sqc`. Rebuilding while running corrupts results mid-run.

3. **Wait for completion**: Fast-mode benchmarks take ~8-10 minutes (4-core) or ~3-5 minutes (24-core). Full-suite takes ~40-50 minutes. Check status no more than once every 5 minutes.

4. **Compare runs after completion**: Use `compare_runs()` to diff against previous runs.

5. **Workflow sequence**:
   ```
   implement changes → bump version → commit → build release → run benchmark → wait → analyze
   ```

---

## Juliet Benchmark (MCP: juliet-benchmark)

### Available Tools

| Tool | Purpose |
|------|---------|
| `run_benchmark(mode)` | Start a new benchmark run (`"fast"` default or `"full"`) |
| `get_status` | Check progress (%, ETA, recently completed CWEs) |
| `get_results(sort_by, run)` | Aggregated TP/FP across all completed CWEs |
| `get_cwe_detail(cwe_id, run)` | Detailed TP/FP breakdown for a specific CWE |
| `list_runs` | List all benchmark runs (SQLite + legacy) |
| `compare_runs(base, target)` | Compare two runs showing TP/FP deltas |
| `compare_cwe(cwe_id, base, target)` | Compare a specific CWE between two runs |
| `cancel_benchmark` | Kill a running benchmark |
| `clear_results` | Remove old result directories |
| `reanalyze_run(run)` | Re-run analysis on existing CSVs (legacy runs) |

### Typical Workflow

```
1. run_benchmark()                          # Start (fast mode, SQLite output)
2. get_status()                             # Check progress (every 5 min)
3. get_results()                            # After completion: overall summary
4. get_results(sort_by="fp_count")          # Top FP rules
5. get_cwe_detail(cwe_id="476")             # Deep dive into specific CWE
6. compare_runs(base="sqc-0.3.17-historical", target="latest")  # Compare versions
7. list_runs()                              # See all available runs
```

### Run Identifiers

The `run` parameter in `get_results()`, `get_cwe_detail()`, and comparison tools accepts:
- `"latest"` — most recent run (default)
- Full run name: `"sqc-0.3.20-abc1234"`
- Commit SHA: `"abc1234"`
- Historical runs: `"sqc-0.3.17-historical"`

### Results Location

**Primary**: `data/benchmarks.db` (SQLite — all TP/FP data, metrics, violations)

**Logs**: `/tmp/juliet_results/sqc-{version}-{sha}/benchmark.log` (stdout from runner)

### CLI Alternative

```bash
python -m bench juliet [--full] [--jobs N] [--keep-csv]
python -m bench status [RUN_ID]
python -m bench compare BASE TARGET
python -m bench runs
```

### Important Notes

- `run_benchmark()` returns immediately — use `get_status()` to monitor
- If a benchmark is already running, `run_benchmark()` returns the existing PID
- **Fast mode** (default): uses per-CWE manifests, only CWE-matched rules. ~10x faster, no noise
- **Full mode**: all 283 rules against every CWE. More violations but higher noise ratio
- Results from `get_results()` only include completed CWEs — wait for full completion for accurate totals
- Resume: if a run is interrupted, re-running skips already-completed CWEs

---

## Real-World Benchmark (MCP: realworld-benchmark)

### Available Tools

| Tool | Purpose |
|------|---------|
| `run_analysis` | Run one tool against one codebase |
| `run_all` | Run all tool×codebase combinations (or filter) |
| `get_status` | Show status of all tracked runs |
| `get_results` | Parse and display results for a run or all runs |
| `compare_runs` | Compare results between two version directories |
| `list_runs` | List all version directories and result files |
| `cancel_run` | Cancel a specific run or all active runs |
| `purge_run` | Remove stale/zombie runs from tracking |
| `clear_results` | Remove old result directories |
| `deploy_sqc` | Deploy sqc binary + manifest to remote hosts |

### Supported Tools and Codebases

**Tools**: `sqc`, `cppcheck`, `clang-tidy`

**Codebases**: `libcrc`, `sqlite`, `mosquitto`, `curl`, `hostap`

### Typical Workflow

```
1. run_all(tool="sqc")           # Run sqc against all 5 codebases
2. get_status()                  # Monitor progress
3. get_results()                 # View all results after completion
4. get_results(run_id="sqc-libcrc-0.2.7-abc1234")  # Specific run
5. compare_runs(base="0.2.6", target="0.2.7")      # Compare versions
```

### Results Location

Real-world results are stored in both:
- `data/benchmarks.db` tables `realworld_runs` + `realworld_results` (historical data)
- `/tmp/realworld_results/sqc-{version}-{commit}/` (raw tool output files)

---

## Pre-Benchmark Checklist

Before starting any benchmark:

- [ ] All code changes committed
- [ ] Version bumped in `Cargo.toml` (for Juliet)
- [ ] `cargo build --release` successful
- [ ] No other benchmark currently running (`get_status()`)
- [ ] Previous results compared if needed (`compare_runs()`)

---

## Comparing Across Runs

### Juliet

```
compare_runs(base="sqc-0.3.17-historical", target="latest")
compare_cwe(cwe_id="476", base="sqc-0.3.14-historical", target="latest")
```

Positive FP delta = regression (more FPs). Negative = improvement.

### Real-World

```
compare_runs(base_version="0.2.6", target_version="0.2.7")
compare_runs(base_version="0.2.6", target_version="0.2.7", tool="sqc", codebase="sqlite")
```

---

## Troubleshooting

| Issue | Solution |
|-------|---------|
| "Benchmark already running" | Use `get_status()` to check, `cancel_benchmark()` to stop |
| Old results consuming disk | `clear_results()` removes non-active result dirs |
| Results directory wrong version | Ensure version bump + commit happened BEFORE `cargo build --release` |
| SQLite locked | WAL mode handles concurrent reads; if stuck, check for zombie processes |
| Historical run not found | Run `python3 scripts/backfill_juliet_results.py` to import from markdown |
| Need to re-backfill | Delete runs from DB first, then re-run backfill script |

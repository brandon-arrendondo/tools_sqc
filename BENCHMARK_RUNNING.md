# SqC — Running Benchmarks (MCP Server Guide)

**Last Updated**: 2026-02-25

How Claude uses the MCP benchmark servers to run Juliet and real-world benchmarks. This replaces the manual shell script approach.

---

## Benchmark Workflow Protocol (CRITICAL)

1. **Version bump + commit BEFORE benchmark**: Always bump the version in `Cargo.toml`, rebuild (`cargo build --release`), and commit before starting a benchmark run. This ensures the results directory is tagged with the correct version and commit SHA.

2. **NEVER modify code while a benchmark is running**: The benchmark uses `target/release/sqc`. Rebuilding while running corrupts results mid-run.

3. **Wait for completion**: Juliet benchmarks take ~40–50 minutes. Check status no more than once every 10 minutes.

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
| `run_benchmark` | Start a new benchmark run against sqc |
| `get_status` | Check progress (%, ETA, recently completed CWEs) |
| `get_results` | Aggregated TP/FP across all completed CWEs |
| `get_cwe_detail` | Detailed TP/FP breakdown for a specific CWE |
| `list_runs` | List all benchmark run directories |
| `compare_runs` | Compare two runs showing TP/FP deltas |
| `compare_cwe` | Compare a specific CWE between two runs |
| `cancel_benchmark` | Kill a running benchmark |
| `clear_results` | Remove old result directories |

### Typical Workflow

```
1. run_benchmark()              # Start the run
2. get_status()                 # Check progress (every 10 min)
3. get_results()                # After completion: overall summary
4. get_results(sort_by="fp_count")  # Top FP rules
5. get_cwe_detail(cwe_id="476")     # Deep dive into specific CWE
6. compare_runs(base="v0.2.6", target="latest")  # Compare versions
```

### Results Location

```
/tmp/juliet_results/
  sqc-{version}-{commit}/       # Per-run directory
    CWE{id}_{name}_analysis.txt # Ground truth analysis per CWE
```

### Important Notes

- `run_benchmark()` returns immediately — use `get_status()` to monitor
- If a benchmark is already running, `run_benchmark()` returns the existing PID
- Results from `get_results()` only include completed CWEs — wait for full completion for accurate totals
- `compare_runs()` accepts version names, commit SHAs, or "latest"
- `clear_results()` protects directories with active runs

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
6. compare_runs(base="0.2.6", target="0.2.7", tool="sqc", codebase="sqlite")
```

### Remote Execution

For running on remote hosts:

```
1. deploy_sqc()                  # Push binary + manifest to all remote hosts
2. deploy_sqc(host="10.0.0.97")  # Or specific host
3. run_all(host="10.0.0.97")    # Run all on remote host
4. run_analysis(tool="sqc", codebase="sqlite", host="workstation-97")
```

Remote hosts are configured in `mcp/remote_hosts.json`.

### Results Location

```
/tmp/realworld_results/
  sqc-{version}-{commit}/       # Per-version directory
    sqc-{codebase}.json         # sqc JSON export
    cppcheck-{codebase}.xml     # cppcheck XML output
    clang-tidy-{codebase}.txt   # clang-tidy text output
```

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
compare_runs(base="sqc-0.2.6-dadadb4dc", target="latest")
compare_cwe(cwe_id="476", base="0.2.6", target="0.2.7")
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
| Zombie runs stuck as "running" | `purge_run(zombies=True)` cleans up dead processes |
| Old results consuming disk | `clear_results()` removes non-active result dirs |
| Remote host not working | `deploy_sqc(host="...")` to push binary first |
| Results directory wrong version | Ensure version bump + commit happened BEFORE `cargo build --release` |

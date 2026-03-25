Running Benchmarks (MCP Server)
===============================

The MCP benchmark servers provide a programmatic interface for running Juliet and
real-world benchmarks. All results are stored in ``data/benchmarks.db`` (SQLite,
WAL mode).

Benchmark Infrastructure
------------------------

::

    bench/
      __init__.py      Package marker
      __main__.py      CLI: python -m bench juliet [--full] [--jobs N]
      config.py        Paths, constants, defaults
      db.py            SQLite schema, WAL mode, CRUD + query API
      analyzer.py      TP/FP classifier (Juliet ground truth)
      runner.py        Parallel CWE runner
      machine.py       Machine metadata (CPU, RAM, hostname)

SQLite Schema
~~~~~~~~~~~~~

=======================  =============================================================
Table                    Purpose
=======================  =============================================================
``runs``                 One row per benchmark (version, SHA, mode, status, machine)
``cwe_scans``            One row per CWE per run (file count, violations, duration)
``violations``           Every individual sqc finding with TP/FP classification
``cwe_metrics``          Pre-computed aggregates per CWE (TP/FP rates)
``rule_cwe_breakdown``   Per-rule per-CWE counts
``realworld_runs``       Real-world benchmark runs (sqc version, machine)
``realworld_results``    Per-project per-tool violation counts
=======================  =============================================================

Historical data from ``JULIET_RESULTS.md`` and ``REALWORLD_RESULTS.md`` has been
backfilled via ``scripts/backfill_juliet_results.py``.

Benchmark Workflow Protocol
---------------------------

.. important::

    1. **Version bump + commit BEFORE benchmark**: Always bump the version in
       ``Cargo.toml``, rebuild (``cargo build --release``), and commit before
       starting. The run_id is ``sqc-{version}-{sha}``.

    2. **NEVER modify code while a benchmark is running**: The benchmark uses
       ``target/release/sqc``. Rebuilding while running corrupts results.

    3. **Wait for completion**: Fast-mode ~8-10 min (4-core), ~3-5 min (24-core).
       Full-suite ~40-50 min. Check status no more than once every 5 minutes.

    4. **Compare runs after completion**.

    5. **Sequence**: ``implement -> bump version -> commit -> build release ->
       run benchmark -> wait -> analyze``

Pre-Benchmark Checklist
~~~~~~~~~~~~~~~~~~~~~~~~

- All code changes committed
- Version bumped in ``Cargo.toml`` (for Juliet)
- ``cargo build --release`` successful
- No other benchmark currently running (``get_status()``)
- Previous results compared if needed (``compare_runs()``)

Juliet Benchmark Tools
----------------------

==========================================  =================================================
Tool                                        Purpose
==========================================  =================================================
``run_benchmark(mode)``                     Start benchmark (``"fast"`` default or ``"full"``)
``get_status``                              Check progress (%, ETA, recent CWEs)
``get_results(sort_by, run)``               Aggregated TP/FP across completed CWEs
``get_cwe_detail(cwe_id, run)``             TP/FP breakdown for a specific CWE
``list_runs``                               List all benchmark runs
``compare_runs(base, target)``              Compare two runs (TP/FP deltas)
``compare_cwe(cwe_id, base, target)``       Compare a CWE between two runs
``cancel_benchmark``                        Kill a running benchmark
``clear_results``                           Remove old result directories
``reanalyze_run(run)``                      Re-run analysis on existing CSVs
==========================================  =================================================

Typical Juliet workflow::

    1. run_benchmark()                          # Start (fast mode)
    2. get_status()                             # Check progress (every 5 min)
    3. get_results()                            # After completion: summary
    4. get_results(sort_by="fp_count")          # Top FP rules
    5. get_cwe_detail(cwe_id="476")             # Deep dive
    6. compare_runs(base="sqc-0.3.17-historical", target="latest")
    7. list_runs()                              # All available runs

Run identifiers accepted by query tools:

- ``"latest"`` -- most recent run (default)
- Full run name: ``"sqc-0.3.20-abc1234"``
- Commit SHA: ``"abc1234"``
- Historical runs: ``"sqc-0.3.17-historical"``

**Notes**:

- ``run_benchmark()`` returns immediately -- use ``get_status()`` to monitor
- If a benchmark is already running, ``run_benchmark()`` returns the existing PID
- **Fast mode** (default): per-CWE manifests, CWE-matched rules only. ~10x faster
- **Full mode**: all 283 rules against every CWE. Higher noise ratio
- Results from ``get_results()`` only include completed CWEs
- Resume: interrupted runs skip already-completed CWEs on re-run

CLI Alternative
~~~~~~~~~~~~~~~

.. code-block:: bash

    python -m bench juliet [--full] [--jobs N] [--keep-csv]
    python -m bench status [RUN_ID]
    python -m bench compare BASE TARGET
    python -m bench runs

Real-World Benchmark Tools
--------------------------

==========================================  =================================================
Tool                                        Purpose
==========================================  =================================================
``run_analysis``                            Run one tool against one codebase
``run_all``                                 Run all tool x codebase combinations (or filter)
``get_status``                              Show status of all tracked runs
``get_results``                             Parse and display results
``compare_runs``                            Compare results between two versions
``list_runs``                               List all version directories
``cancel_run``                              Cancel a specific or all active runs
``purge_run``                               Remove stale/zombie runs
``clear_results``                           Remove old result directories
``deploy_sqc``                              Deploy sqc binary + manifest to remote hosts
==========================================  =================================================

Supported tools: ``sqc``, ``cppcheck``, ``clang-tidy``

Supported codebases: ``libcrc``, ``sqlite``, ``mosquitto``, ``curl``, ``hostap``

Typical real-world workflow::

    1. run_all(tool="sqc")           # Run sqc against all 5 codebases
    2. get_status()                  # Monitor progress
    3. get_results()                 # View all results
    4. compare_runs(base="0.2.6", target="0.2.7")

Comparing Across Runs
---------------------

Juliet
~~~~~~

::

    compare_runs(base="sqc-0.3.17-historical", target="latest")
    compare_cwe(cwe_id="476", base="sqc-0.3.14-historical", target="latest")

Positive FP delta = regression. Negative = improvement.

Real-World
~~~~~~~~~~

::

    compare_runs(base_version="0.2.6", target_version="0.2.7")
    compare_runs(base_version="0.2.6", target_version="0.2.7", tool="sqc", codebase="sqlite")

Troubleshooting
---------------

=======================================  =============================================
Issue                                    Solution
=======================================  =============================================
"Benchmark already running"              ``get_status()``, then ``cancel_benchmark()``
Old results consuming disk               ``clear_results()``
Results show wrong version               Ensure version bump + commit before build
SQLite locked                            WAL handles concurrent reads; check for zombies
Historical run not found                 Run ``scripts/backfill_juliet_results.py``
=======================================  =============================================

Resolved Issues
~~~~~~~~~~~~~~~

- **DCL02-C Stack Overflow** (Fixed 2026-01-07): Unbounded recursive AST traversal
  in DCL02-C caused stack overflow on large files (SQLite). Converted to iterative
  with depth limit.

- **STR31-C ``detect_manual_string_loop`` Runaway** (Fixed 2026-02-25): Caused
  36--49% of all violations on 3 of 5 real-world projects. File-wide fallback
  removed; pattern matching restricted to loop condition and body.

- **Output Buffer Saturation**: SqC emits one status line per rule per file
  (~100 rules × N files). Always suppress or redirect output during scans::

      ./target/release/sqc directory/ --export results.csv 2>/dev/null

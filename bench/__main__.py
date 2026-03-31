"""CLI entry point: python -m bench <command> [options]

Commands:
  juliet [--full] [--jobs N] [--keep-csv]  Run Juliet benchmark
  status [RUN_ID]                          Show benchmark progress/results
  compare BASE TARGET                      Compare two runs
  runs                                     List all runs
  realworld [RUN] [--compare BASE]         Real-world FP dashboard
  realworld-runs                           List real-world benchmark runs
"""

import argparse
import json
import sys

from bench.config import DEFAULT_JOBS
from bench.db import BenchDB


def cmd_juliet(args):
    from bench.runner import run_benchmark
    run_benchmark(fast=not args.full, jobs=args.jobs, keep_csv=args.keep_csv)


def cmd_status(args):
    db = BenchDB()
    run_id = args.run_id

    if not run_id:
        resolved = db.resolve_run("latest")
        if not resolved:
            print("No runs found.")
            return
        run_id = resolved

    resolved = db.resolve_run(run_id)
    if not resolved:
        print(f"Run '{run_id}' not found.")
        runs = db.list_runs()
        if runs:
            print("Available runs:")
            for r in runs:
                print(f"  {r['run_id']}  ({r['status']})")
        return

    run = db.get_run(resolved)
    if run["status"] == "running":
        progress = db.get_progress(resolved)
        print(f"Run: {resolved}  Status: running")
        print(f"Progress: {progress['done_cwes']}/{progress['total_cwes']} "
              f"({progress['progress_pct']}%)")
        if progress["recently_completed"]:
            print("\nRecently completed:")
            for c in progress["recently_completed"]:
                print(f"  {c['cwe_dir_name']} | {c['duration_s']}s | "
                      f"{c['violation_count']} violations")
    else:
        summary = db.get_run_summary(resolved)
        s = summary["summary"]
        print(f"Run: {resolved}  Status: {run['status']}")
        print(f"CWEs: {s['cwes_analyzed']}  "
              f"TP: {s['total_tp']}  FP: {s['total_fp']}  "
              f"TP Rate: {s['tp_rate_pct']}%")
        if summary.get("cwe_aware"):
            ca = summary["cwe_aware"]
            print(f"\nCWE-Aware: TP {ca['cwe_matched_tp']}  FP {ca['cwe_matched_fp']}  "
                  f"TP Rate {ca['cwe_matched_tp_rate_pct']}%  "
                  f"Noise {ca['noise_ratio_pct']}%")


def cmd_compare(args):
    db = BenchDB()
    base = db.resolve_run(args.base)
    target = db.resolve_run(args.target)

    if not base:
        print(f"Cannot resolve base run '{args.base}'.")
        return
    if not target:
        print(f"Cannot resolve target run '{args.target}'.")
        return

    result = db.compare_runs(base, target)
    if "error" in result:
        print(f"Error: {result['error']}")
        return

    s = result["summary"]
    d = s["delta"]
    print(f"Comparing: {s['base_run']} → {s['target_run']}")
    print(f"\nOverall Delta: TP {d['tp']:+d}  FP {d['fp']:+d}  "
          f"TP Rate {d['tp_rate_pp']:+.2f}pp")

    if result.get("cwe_improvements"):
        print(f"\nTop CWE Improvements (FP reduced):")
        for c in result["cwe_improvements"][:5]:
            print(f"  {c['cwe_id']}: FP {c['delta_fp']:+d}  TP {c['delta_tp']:+d}")

    if result.get("cwe_regressions"):
        print(f"\nTop CWE Regressions (FP increased):")
        for c in result["cwe_regressions"][:5]:
            print(f"  {c['cwe_id']}: FP {c['delta_fp']:+d}  TP {c['delta_tp']:+d}")


def cmd_runs(args):
    db = BenchDB()
    runs = db.list_runs()
    if not runs:
        print("No benchmark runs found.")
        return
    print(f"{'Run ID':<35} {'Status':<12} {'CWEs':<6} {'Started'}")
    print("-" * 75)
    for r in runs:
        # Count CWEs from DB
        progress = db.get_progress(r["run_id"])
        done = progress["done_cwes"]
        total = progress["total_cwes"]
        started = r.get("started_at", "")[:19]
        print(f"{r['run_id']:<35} {r['status']:<12} {done}/{total:<4} {started}")


def cmd_realworld(args):
    db = BenchDB()

    # Resolve target run
    target_id = db.resolve_realworld_run(args.run or "latest")
    if not target_id:
        print("No real-world runs found.")
        return

    # Resolve base run for comparison
    base_id = None
    if args.compare:
        base_id = db.resolve_realworld_run(args.compare)
        if not base_id:
            print(f"Cannot resolve base run '{args.compare}'.")
            return
    else:
        # Default: compare against previous run
        runs = db.list_realworld_runs()
        for i, r in enumerate(runs):
            if r["id"] == target_id and i + 1 < len(runs):
                base_id = runs[i + 1]["id"]
                break

    dashboard = db.get_realworld_dashboard(target_id, base_id, top_n=args.top)
    if "error" in dashboard:
        print(f"Error: {dashboard['error']}")
        return

    run = dashboard["run"]
    total = dashboard["total_violations"]

    # Header
    print(f"Real-world FP Dashboard — v{run['sqc_version']}"
          f" ({run.get('commit_sha', '?')[:8]})")
    if base_id and "base_run" in dashboard:
        base = dashboard["base_run"]
        delta = dashboard["total_delta"]
        sign = "+" if delta >= 0 else ""
        print(f"  vs v{base['sqc_version']}"
              f" ({base.get('commit_sha', '?')[:8]})"
              f": {sign}{delta} ({sign}{delta / dashboard['base_total'] * 100:.1f}%)"
              if dashboard["base_total"] > 0
              else f"  vs v{base['sqc_version']}")
    print(f"  Total violations: {total:,}")
    print()

    # Per-project summary
    print(f"{'Project':<12} {'Violations':>10}  {'Duration':>10}")
    print("-" * 36)
    for p in sorted(dashboard["per_project"], key=lambda x: -x["violation_count"]):
        dur = f"{p['duration_s']:.0f}s" if p.get("duration_s") else "—"
        print(f"{p['project']:<12} {p['violation_count']:>10,}  {dur:>10}")
    print()

    # Top rules
    if base_id:
        print(f"{'Rule':<12} {'Count':>8} {'Base':>8} {'Delta':>8}")
        print("-" * 40)
        for r in dashboard["top_rules"]:
            delta = r.get("delta", 0)
            base_count = r.get("base_count", "—")
            sign = "+" if delta > 0 else ""
            delta_str = f"{sign}{delta}" if delta != 0 else "="
            base_str = f"{base_count:>8,}" if isinstance(base_count, int) else f"{'—':>8}"
            print(f"{r['rule_id']:<12} {r['count']:>8,} {base_str} {delta_str:>8}")
    else:
        print(f"{'Rule':<12} {'Count':>8}")
        print("-" * 22)
        for r in dashboard["top_rules"]:
            print(f"{r['rule_id']:<12} {r['count']:>8,}")
    print()

    # Per-project top 5 rules
    if not args.compact:
        for p in sorted(dashboard["per_project"], key=lambda x: -x["violation_count"]):
            print(f"  {p['project']}:")
            for r in p.get("top_rules", [])[:5]:
                print(f"    {r['rule_id']:<12} {r['count']:>6,}")
        print()


def cmd_realworld_runs(args):
    db = BenchDB()
    runs = db.list_realworld_runs()
    if not runs:
        print("No real-world runs found.")
        return
    print(f"{'ID':>4}  {'Version':<10} {'Commit':<10} {'Scanned At':<20} {'Host'}")
    print("-" * 65)
    for r in runs:
        sha = (r.get("commit_sha") or "—")[:8]
        scanned = (r.get("scanned_at") or "—")[:19]
        host = r.get("hostname") or "—"
        print(f"{r['id']:>4}  {r['sqc_version']:<10} {sha:<10} {scanned:<20} {host}")


def main():
    parser = argparse.ArgumentParser(
        prog="bench",
        description="sqc Juliet benchmark infrastructure",
    )
    sub = parser.add_subparsers(dest="command")

    # juliet
    p_juliet = sub.add_parser("juliet", help="Run Juliet benchmark")
    p_juliet.add_argument("--full", action="store_true",
                          help="Use all rules (default: fast/CWE-matched only)")
    p_juliet.add_argument("--jobs", "-j", type=int, default=DEFAULT_JOBS,
                          help=f"Parallel workers (default: {DEFAULT_JOBS})")
    p_juliet.add_argument("--keep-csv", action="store_true",
                          help="Keep intermediate CSV files")
    p_juliet.set_defaults(func=cmd_juliet)

    # status
    p_status = sub.add_parser("status", help="Show run status")
    p_status.add_argument("run_id", nargs="?", default=None,
                          help="Run ID (default: latest)")
    p_status.set_defaults(func=cmd_status)

    # compare
    p_compare = sub.add_parser("compare", help="Compare two runs")
    p_compare.add_argument("base", help="Base run (older)")
    p_compare.add_argument("target", help="Target run (newer)")
    p_compare.set_defaults(func=cmd_compare)

    # runs
    p_runs = sub.add_parser("runs", help="List all runs")
    p_runs.set_defaults(func=cmd_runs)

    # realworld
    p_rw = sub.add_parser("realworld", help="Real-world FP dashboard")
    p_rw.add_argument("run", nargs="?", default=None,
                      help="Run identifier (version, ID, or 'latest')")
    p_rw.add_argument("--compare", "-c", default=None,
                      help="Base run to compare against (default: previous)")
    p_rw.add_argument("--top", "-n", type=int, default=25,
                      help="Number of top rules to show (default: 25)")
    p_rw.add_argument("--compact", action="store_true",
                      help="Skip per-project rule breakdown")
    p_rw.set_defaults(func=cmd_realworld)

    # realworld-runs
    p_rw_runs = sub.add_parser("realworld-runs", help="List real-world runs")
    p_rw_runs.set_defaults(func=cmd_realworld_runs)

    args = parser.parse_args()
    if not args.command:
        parser.print_help()
        sys.exit(1)

    args.func(args)


if __name__ == "__main__":
    main()

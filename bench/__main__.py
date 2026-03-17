"""CLI entry point: python -m bench <command> [options]

Commands:
  juliet [--full] [--jobs N] [--keep-csv]  Run Juliet benchmark
  status [RUN_ID]                          Show benchmark progress/results
  compare BASE TARGET                      Compare two runs
  runs                                     List all runs
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

    args = parser.parse_args()
    if not args.command:
        parser.print_help()
        sys.exit(1)

    args.func(args)


if __name__ == "__main__":
    main()

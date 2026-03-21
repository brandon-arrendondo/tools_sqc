#!/usr/bin/env python3
"""Parallel sqc scanner — splits a codebase by subdirectory.

Finds top-level subdirectories containing .c files and runs one sqc process
per subdirectory in parallel, then merges JSON outputs. Falls back to a
single sqc invocation for small codebases (< --min-files).

Usage:
    python3 scripts/sqc_parallel_scan.py /path/to/codebase \
        --sqc target/release/sqc \
        -m rules_templates/rules-benchmark.toml \
        -e /tmp/results.json \
        -d /path/to/codebase \
        --jobs 4
"""

import argparse
import json
import os
import subprocess
import sys
import tempfile
import time
from concurrent.futures import ProcessPoolExecutor, as_completed
from pathlib import Path


def find_scan_units(base_path: Path, target_max: int = 0) -> list[Path]:
    """Find subdirectories to scan, splitting large dirs for balance.

    Args:
        base_path: Root directory to split.
        target_max: Max .c files per unit before recursive splitting.
                    0 = auto-calculate from total files and CPU count.
    """
    total = count_c_files(base_path)
    if target_max <= 0:
        cpus = os.cpu_count() or 4
        # Aim for ~2x as many units as CPUs for good load balancing
        target_max = max(total // (cpus * 2), 20)

    def _split(path: Path) -> list[Path]:
        n = count_c_files(path)
        if n <= target_max:
            return [path]

        # Don't split if this directory has .c files directly at this level —
        # sqc scans recursively, so splitting would cause overlap between
        # the parent (scans everything) and children.
        direct_c = sum(1 for _ in path.glob("*.c"))
        if direct_c > 0:
            return [path]

        # All .c files are in subdirectories — safe to split
        children = []
        for entry in sorted(path.iterdir()):
            if entry.is_dir() and not entry.name.startswith('.'):
                if count_c_files(entry) > 0:
                    children.append(entry)

        if not children:
            return [path]

        units = []
        for child in children:
            units.extend(_split(child))
        return units

    return _split(base_path)


def count_c_files(path: Path) -> int:
    return sum(1 for _ in path.rglob("*.c"))


def scan_one(sqc_bin: str, scan_dir: Path, manifest: str,
             context_dirs: list[str], output_path: str,
             label: str = "") -> dict:
    """Run sqc on one directory. Returns summary dict."""
    cmd = [sqc_bin, str(scan_dir), "-m", manifest, "-e", output_path]
    for d in context_dirs:
        cmd.extend(["-d", d])

    start = time.monotonic()
    proc = subprocess.run(cmd, capture_output=True, timeout=3600)
    elapsed = time.monotonic() - start

    violations = 0
    if os.path.exists(output_path):
        try:
            with open(output_path) as f:
                data = json.load(f)
            violations = len(data)
        except Exception:
            pass

    return {
        "dir": label or scan_dir.name,
        "elapsed_s": round(elapsed, 1),
        "violations": violations,
        "returncode": proc.returncode,
    }


def run_single(sqc_bin: str, scan_path: str, manifest: str,
               context_dirs: list[str], export_path: str) -> int:
    """Run sqc as a single process (no parallelism)."""
    cmd = [sqc_bin, scan_path, "-m", manifest, "-e", export_path]
    for d in context_dirs:
        cmd.extend(["-d", d])
    result = subprocess.run(cmd, timeout=3600)
    return result.returncode


def main():
    parser = argparse.ArgumentParser(description="Parallel sqc scanner")
    parser.add_argument("scan_path", help="Directory to scan")
    parser.add_argument("--sqc", default="sqc", help="Path to sqc binary")
    parser.add_argument("-m", "--manifest", required=True, help="Rules manifest")
    parser.add_argument("-e", "--export", required=True, help="Output JSON path")
    parser.add_argument("-d", "--directory", action="append", default=[],
                        help="Context directory for cross-file analysis (repeatable)")
    parser.add_argument("-j", "--jobs", type=int, default=0,
                        help="Parallel workers (0=auto, max 8)")
    parser.add_argument("--min-files", type=int, default=50,
                        help="Min C files to trigger parallel mode (default 50)")
    args = parser.parse_args()

    scan_path = Path(args.scan_path).resolve()
    jobs = args.jobs or min(os.cpu_count() or 4, 8)

    total_files = count_c_files(scan_path)
    units = find_scan_units(scan_path)

    # Ensure context includes the scan root for cross-file analysis
    context_dirs = list(args.directory)
    scan_str = str(scan_path)
    if scan_str not in context_dirs:
        context_dirs.append(scan_str)

    # Fall back to single process for small codebases
    if total_files < args.min_files or len(units) <= 1:
        print(f"[parallel] {scan_path.name}: {total_files} files, running single process",
              file=sys.stderr, flush=True)
        rc = run_single(args.sqc, str(scan_path), args.manifest,
                        context_dirs, args.export)
        sys.exit(rc)

    print(f"[parallel] {scan_path.name}: {total_files} files across "
          f"{len(units)} units, {jobs} workers", file=sys.stderr, flush=True)
    for u in units:
        n = count_c_files(u)
        rel = u.relative_to(scan_path)
        print(f"[parallel]   {rel}/: {n} files", file=sys.stderr, flush=True)

    all_violations = []
    completed = 0

    with tempfile.TemporaryDirectory() as tmpdir:
        with ProcessPoolExecutor(max_workers=jobs) as executor:
            futures = {}
            for i, unit in enumerate(units):
                rel = str(unit.relative_to(scan_path))
                safe_name = rel.replace("/", "_").replace("\\", "_")
                output = os.path.join(tmpdir, f"{safe_name}.json")
                future = executor.submit(
                    scan_one, args.sqc, unit, args.manifest,
                    context_dirs, output, label=rel,
                )
                futures[future] = (unit, output)

            for future in as_completed(futures):
                unit, output = futures[future]
                completed += 1
                try:
                    result = future.result()
                    print(f"[parallel] Completed {completed}/{len(units)}: "
                          f"{result['dir']} | {result['elapsed_s']}s | "
                          f"{result['violations']} violations",
                          file=sys.stderr, flush=True)

                    if os.path.exists(output):
                        with open(output) as f:
                            violations = json.load(f)
                        all_violations.extend(violations)
                except Exception as e:
                    print(f"[parallel] FAIL {completed}/{len(units)}: "
                          f"{unit.name} | {e}", file=sys.stderr, flush=True)

    # Deduplicate (safety net for any overlap between scan units)
    seen = set()
    unique = []
    for v in all_violations:
        key = (v.get("file", ""), v.get("line", 0),
               v.get("rule_id", ""), v.get("message", ""))
        if key not in seen:
            seen.add(key)
            unique.append(v)

    with open(args.export, 'w') as f:
        json.dump(unique, f)

    print(f"[parallel] Done: {len(unique)} unique violations "
          f"(from {len(all_violations)} raw)",
          file=sys.stderr, flush=True)


if __name__ == "__main__":
    main()

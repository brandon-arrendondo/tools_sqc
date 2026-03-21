#!/usr/bin/env python3
"""Parallel sqc scanner — splits a codebase by subdirectory.

Generates a prescan cache once, then runs one sqc process per subdirectory
in parallel (each loading the cache instead of re-prescanning). Falls back
to a single sqc invocation for small codebases (< --min-files).

Prescan caches are stored persistently in --prescan-cache-dir so they can
be reused across benchmark runs. Use --rebuild-prescan to force regeneration
(e.g., after changing prescan logic in sqc).

Usage:
    python3 scripts/sqc_parallel_scan.py /path/to/codebase \
        --sqc target/release/sqc \
        -m rules_templates/rules-benchmark.toml \
        -e /tmp/results.json \
        -d /path/to/codebase \
        --jobs 4 \
        --prescan-cache-dir data/prescan_cache
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


def resolve_prescan_cache(sqc_bin: str, manifest: str,
                          context_dirs: list[str],
                          cache_dir: str | None,
                          codebase_name: str,
                          rebuild: bool) -> str | None:
    """Resolve prescan cache: reuse existing or generate new.

    Returns path to a valid cache file, or None if caching failed.
    """
    if cache_dir:
        os.makedirs(cache_dir, exist_ok=True)
        cache_path = os.path.join(cache_dir, f"{codebase_name}.cache")

        # Reuse existing cache if available and not forced to rebuild
        if os.path.exists(cache_path) and not rebuild:
            size_kb = os.path.getsize(cache_path) / 1024
            mtime = time.strftime("%Y-%m-%d %H:%M",
                                  time.localtime(os.path.getmtime(cache_path)))
            print(f"[parallel] Reusing prescan cache: {cache_path} "
                  f"({size_kb:.0f} KB, generated {mtime})",
                  file=sys.stderr, flush=True)
            return cache_path
    else:
        # No persistent dir — use temp file (will be cleaned up)
        cache_path = None

    # Generate new cache
    if cache_path is None:
        import tempfile as tf
        fd, cache_path = tf.mkstemp(suffix=".cache", prefix="prescan_")
        os.close(fd)

    print(f"[parallel] Generating prescan cache...",
          file=sys.stderr, flush=True)
    cache_start = time.monotonic()
    ok = _generate_prescan_cache(sqc_bin, manifest, context_dirs, cache_path)
    cache_elapsed = time.monotonic() - cache_start

    if ok:
        size_kb = os.path.getsize(cache_path) / 1024
        print(f"[parallel] Prescan cache ready: {size_kb:.0f} KB "
              f"in {cache_elapsed:.1f}s → {cache_path}",
              file=sys.stderr, flush=True)
        return cache_path
    else:
        print(f"[parallel] Prescan cache generation failed "
              f"({cache_elapsed:.1f}s), falling back to -d per worker",
              file=sys.stderr, flush=True)
        return None


def _generate_prescan_cache(sqc_bin: str, manifest: str,
                            context_dirs: list[str],
                            cache_path: str) -> bool:
    """Run sqc --save-prescan to generate a prescan cache file."""
    # Find one .c file to use as a minimal scan target
    scan_target = None
    for d in context_dirs:
        for f in Path(d).rglob("*.c"):
            scan_target = str(f)
            break
        if scan_target:
            break

    if not scan_target:
        return False

    cmd = [
        sqc_bin, scan_target,
        "-m", manifest,
        "--save-prescan", cache_path,
        "-e", os.devnull,
    ]
    for d in context_dirs:
        cmd.extend(["-d", d])

    try:
        proc = subprocess.run(cmd, capture_output=True, timeout=1800)
        return proc.returncode == 0 and os.path.exists(cache_path)
    except subprocess.TimeoutExpired:
        return False


def scan_one(sqc_bin: str, scan_dir: Path, manifest: str,
             prescan_cache: str, output_path: str,
             label: str = "") -> dict:
    """Run sqc on one directory with prescan cache. Returns summary dict."""
    cmd = [
        sqc_bin, str(scan_dir),
        "-m", manifest,
        "-e", output_path,
        "--load-prescan", prescan_cache,
    ]

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


def scan_one_no_cache(sqc_bin: str, scan_dir: Path, manifest: str,
                      context_dirs: list[str], output_path: str,
                      label: str = "") -> dict:
    """Run sqc on one directory without cache (fallback). Returns summary dict."""
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
    parser.add_argument("--prescan-cache-dir",
                        help="Directory for persistent prescan cache files. "
                             "If set, caches are reused across runs.")
    parser.add_argument("--rebuild-prescan", action="store_true",
                        help="Force prescan cache regeneration (use after "
                             "changing prescan logic in sqc)")
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

    # Resolve prescan cache (reuse or generate)
    codebase_name = scan_path.name
    cache_path = resolve_prescan_cache(
        args.sqc, args.manifest, context_dirs,
        args.prescan_cache_dir, codebase_name,
        args.rebuild_prescan,
    )

    all_violations = []
    completed = 0

    with tempfile.TemporaryDirectory() as tmpdir:
        with ProcessPoolExecutor(max_workers=jobs) as executor:
            futures = {}
            for i, unit in enumerate(units):
                rel = str(unit.relative_to(scan_path))
                safe_name = rel.replace("/", "_").replace("\\", "_")
                output = os.path.join(tmpdir, f"{safe_name}.json")

                if cache_path:
                    future = executor.submit(
                        scan_one, args.sqc, unit, args.manifest,
                        cache_path, output, label=rel,
                    )
                else:
                    future = executor.submit(
                        scan_one_no_cache, args.sqc, unit, args.manifest,
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

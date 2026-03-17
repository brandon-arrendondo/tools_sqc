#!/usr/bin/env python3
"""Backfill historical Juliet benchmark results into SQLite from JULIET_RESULTS.md.

Imports the Version History table data as run records with synthetic aggregate
cwe_scan/cwe_metrics entries. Per-CWE data is imported where available
(v0.3.14, v0.3.17, v0.3.19).

Usage: python3 scripts/backfill_juliet_results.py [--dry-run]
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from bench.db import BenchDB

# ── Version History (from JULIET_RESULTS.md "Version History" table) ──────────
# Each entry: (version, tp_rate, fp, tp, runtime, machine, mode, notes)
# runtime: string or None. machine: "24-core" or "4-core"

VERSION_HISTORY = [
    ("v0.2.1",  41.1, 839341, None,    None,       "24-core", "full", "Baseline"),
    ("v0.2.4",  43.8, 243849, 189950,  None,       "24-core", "full", "Windows API + multiple rule fixes"),
    ("v0.2.6",  44.5, 215672, 172708,  None,       "24-core", "full", "CFG null state + bounds-check detection"),
    ("v0.2.7",  44.5, 215671, 172780,  None,       "24-core", "full", "INT36-C TP restore + INT31-C FP fix"),
    ("v0.2.12", 44.6, 210138, 169161,  None,       "24-core", "full", "DCL13-C pointer modification + INT01-C sizeof skip"),
    ("v0.2.13", 44.7, 196177, 158403,  None,       "24-core", "full", "INT31-C implicit narrowing + d_lib_common FP fixes"),
    ("v0.2.15", 44.2, 185499, 146714,  None,       "24-core", "full", "d_lib_common FP.md cleanup (17 patterns)"),
    ("v0.2.16", 44.2, 185510, 146733,  None,       "24-core", "full", "EXP34-C call-site null propagation (Phase 2)"),
    ("v0.2.17", 44.2, 185591, 146913,  None,       "24-core", "full", "Phase 3: MEM10-C, API00-C, API02-C, prescan"),
    ("v0.2.18", 44.1, 184645, 145639,  None,       "24-core", "full", "INT31-C pointer cast, ARR36-C, API00-C, INT30-C guards"),
    ("v0.2.19", 44.1, 184644, 145639,  None,       "24-core", "full", "INT30-C loop guards, prescan null guards, ARR00-C fix"),
    ("v0.2.20", 44.2, 181924, 144278,  None,       "24-core", "full", "d_lib_networking FP fixes"),
    ("v0.2.21", 44.0, 175667, 137921,  None,       "24-core", "full", "const_eval value-range analysis"),
    ("v0.2.22", 44.0, 175673, 137921,  None,       "24-core", "full", "INT30-C: extend upper-bound guard"),
    ("v0.2.23", 44.6, 163585, 131661,  None,       "24-core", "full", "INT32-C const_eval + INT30-C uint64_t + built-in macros"),
    ("v0.2.25", 44.6, 161965, 130199,  None,       "24-core", "full", "ARR32-C tightening, rule removals, value-range FP fixes"),
    ("v0.3.5",  44.6, 161510, 130004,  None,       "24-core", "full", "Struct field type resolution"),
    ("v0.3.8",  44.3, 77826,  61799,   None,       "4-core",  "full", "12-CWE subset only"),
    ("v0.3.14", 44.4, 158036, 126106,  "1h 14m",  "24-core", "full", "EXP33-C, INT30-C, EXP34-C FP fixes (full suite)"),
    ("v0.3.17", 44.4, 160496, 128038,  "1h 47m",  "24-core", "full", "CWE-78 macro alias + CWE-253 incorrect return check"),
]

# ── Machine metadata ──────────────────────────────────────────────────────────

MACHINES = {
    "24-core": {
        "hostname": "workstation",
        "cpu_model": "24-core workstation",
        "cpu_cores": 24,
        "ram_gb": 0,
        "os_version": "Linux",
    },
    "4-core": {
        "hostname": "laptop",
        "cpu_model": "Intel Core i5-6200U",
        "cpu_cores": 4,
        "ram_gb": 7.5,
        "os_version": "Linux",
    },
}

# ── Per-CWE data for v0.3.14 (from "Top CWEs by FP count" table) ────────────
# (cwe_id, tp, fp, tp_rate)
V0314_PER_CWE = [
    ("CWE-78",  17350, 5592,  75.6),
    ("CWE-197", 1148,  227,   83.5),
    ("CWE-506", 666,   120,   84.7),
    ("CWE-617", 312,   48,    86.7),
    ("CWE-464", 48,    5,     90.6),
    ("CWE-457", 704,   2549,  21.6),
    ("CWE-563", 54,    250,   17.8),
    ("CWE-366", 1,     12,    7.7),
]

# ── Per-CWE data for v0.3.17 (CWE-aware highlights) ─────────────────────────
# (cwe_id, cwe_matched_tp, cwe_matched_fp, precision, per_file_rate)
V0317_CWE_AWARE = [
    ("CWE-253", 178,  0,    100.0, 26.0),
    ("CWE-252", 179,  0,    100.0, 16.5),
    ("CWE-690", 290,  62,   82.4,  25.9),
    ("CWE-78",  1282, 1773, 42.0,  13.0),
    ("CWE-590", 94,   0,    100.0, 10.4),
    ("CWE-467", 20,   0,    100.0, 37.0),
    ("CWE-481", 12,   0,    100.0, 66.7),
    ("CWE-391", 36,   22,   62.1,  37.0),
]

# ── Per-CWE data for v0.3.19 fast mode (CWE-aware highlights) ───────────────
V0319_CWE_AWARE = [
    ("CWE-252", 179,  0,    100.0, 16.5),
    ("CWE-253", 178,  0,    100.0, 26.0),
    ("CWE-690", 290,  62,   82.4,  25.9),
    ("CWE-197", 259,  126,  67.5,  37.4),
    ("CWE-401", 284,  287,  49.7,  21.7),
    ("CWE-78",  1204, 1443, 45.5,  13.0),
    ("CWE-476", 140,  161,  46.5,  29.0),
    ("CWE-190", 650,  820,  44.2,  12.9),
    ("CWE-194", 447,  626,  41.7,  27.0),
    ("CWE-195", 406,  588,  40.8,  24.8),
    ("CWE-121", 854,  1317, 39.3,  12.8),
]

# ── Round 1 baseline per-CWE data (full table from JULIET_RESULTS.md) ────────
# (cwe_id, files, tp, fp, tp_rate)
ROUND1_PER_CWE = [
    ("CWE-506", 158,   3421,  552,    86.1),
    ("CWE-15",  56,    1255,  422,    74.8),
    ("CWE-427", 560,   7656,  2798,   72.0),
    ("CWE-78",  5600,  79292, 30203,  72.4),
    ("CWE-617", 354,   2685,  1192,   69.3),
    ("CWE-197", 1008,  7899,  3733,   67.9),
    ("CWE-123", 168,   2239,  1213,   64.9),
    ("CWE-114", 672,   8839,  4973,   64.0),
    ("CWE-194", 1344,  18260, 12440,  59.5),
    ("CWE-510", 70,    1450,  1037,   58.3),
    ("CWE-195", 1344,  16087, 11865,  57.6),
    ("CWE-90",  560,   12600, 10252,  55.1),
    ("CWE-464", 56,    334,   280,    54.4),
    ("CWE-526", 18,    69,    58,     54.3),
    ("CWE-587", 18,    36,    31,     53.7),
    ("CWE-680", 336,   5381,  4715,   53.3),
    ("CWE-188", 36,    286,   275,    51.0),
    ("CWE-843", 100,   279,   340,    45.1),
    ("CWE-481", 18,    195,   239,    44.9),
    ("CWE-480", 18,    79,    97,     44.9),
    ("CWE-121", 5906,  50353, 66007,  43.3),
    ("CWE-122", 3656,  42202, 58891,  41.7),
    ("CWE-134", 3360,  52276, 90251,  36.7),
    ("CWE-476", 372,   1222,  2475,   33.1),
    ("CWE-190", 5040,  26103, 54636,  32.3),
    ("CWE-191", 3864,  19849, 40831,  32.7),
    ("CWE-401", 1228,  10976, 23198,  32.1),
    ("CWE-416", 150,   1787,  4698,   27.6),
    ("CWE-457", 616,   5045,  36338,  12.2),
]


def _version_to_run_id(version: str) -> str:
    """Convert version string to run_id format."""
    v = version.lstrip("v")
    return f"sqc-{v}-historical"


def _parse_runtime(runtime_str: str | None) -> float | None:
    """Parse '1h 47m' to seconds."""
    if not runtime_str:
        return None
    import re
    hours = 0
    minutes = 0
    h_match = re.search(r'(\d+)h', runtime_str)
    m_match = re.search(r'(\d+)m', runtime_str)
    if h_match:
        hours = int(h_match.group(1))
    if m_match:
        minutes = int(m_match.group(1))
    return hours * 3600 + minutes * 60


def backfill(dry_run: bool = False):
    db = BenchDB()
    existing_runs = {r["run_id"] for r in db.list_runs()}

    imported = 0
    skipped = 0

    for version, tp_rate, fp, tp, runtime, machine_key, mode, notes in VERSION_HISTORY:
        run_id = _version_to_run_id(version)

        if run_id in existing_runs:
            print(f"  SKIP (exists): {run_id}")
            skipped += 1
            continue

        machine = MACHINES[machine_key]
        duration_s = _parse_runtime(runtime)
        sqc_version = version.lstrip("v")

        # Compute TP if missing (from tp_rate and fp)
        if tp is None and tp_rate and fp:
            # tp_rate = tp / (tp + fp) * 100
            # tp = tp_rate * (tp + fp) / 100
            # tp = tp_rate * fp / (100 - tp_rate)
            tp = round(tp_rate * fp / (100 - tp_rate))

        total = (tp or 0) + (fp or 0)

        print(f"  {'DRY-RUN' if dry_run else 'IMPORT'}: {run_id} "
              f"(TP={tp}, FP={fp}, rate={tp_rate}%, {notes})")

        if dry_run:
            imported += 1
            continue

        # Create run record
        started_at = f"2025-01-01T00:00:00+00:00"  # placeholder
        finished_at = f"2025-01-01T00:00:00+00:00"
        db.create_run(
            run_id=run_id,
            sqc_version=sqc_version,
            commit_sha="historical",
            mode=mode,
            started_at=started_at,
            pid=0,
            jobs=12,
            total_cwes=118,
            machine=machine,
        )
        db.finish_run(run_id, "completed", finished_at)

        # Create synthetic aggregate cwe_scan + metrics
        scan_id = db.create_cwe_scan(run_id, "ALL", "AGGREGATE", file_count=54484)
        db.update_cwe_scan(scan_id,
                           status="completed",
                           violation_count=total,
                           duration_s=duration_s)
        db.insert_cwe_metrics({
            "cwe_scan_id": scan_id,
            "tp_count": tp or 0,
            "fp_count": fp or 0,
            "tp_rate_pct": tp_rate,
            "flaw_lines_total": 0,
            "flaw_lines_detected": 0,
            "flaw_detection_rate_pct": 0,
            "cwe_matched_tp": 0,
            "cwe_matched_fp": 0,
            "noise_count": 0,
            "noise_ratio": 0,
            "per_file_detected": 0,
            "per_file_total": 0,
            "per_file_rate": 0,
            "flaw_hit_detected": 0,
            "flaw_hit_total": 0,
            "flaw_hit_rate": 0,
        })

        imported += 1

    # ── Import per-CWE data for v0.2.1 (Round 1 baseline) ────────────────
    run_id = _version_to_run_id("v0.2.1")
    if run_id not in existing_runs and not dry_run:
        _import_per_cwe_basic(db, run_id, ROUND1_PER_CWE)
        print(f"    + {len(ROUND1_PER_CWE)} per-CWE entries for {run_id}")

    # ── Import per-CWE data for v0.3.14 ──────────────────────────────────
    run_id = _version_to_run_id("v0.3.14")
    if run_id not in existing_runs and not dry_run:
        _import_per_cwe_basic(db, run_id, [
            (cwe, 0, tp, fp, rate) for cwe, tp, fp, rate in V0314_PER_CWE
        ])
        print(f"    + {len(V0314_PER_CWE)} per-CWE entries for {run_id}")

    # ── Import CWE-aware data for v0.3.17 ────────────────────────────────
    run_id = _version_to_run_id("v0.3.17")
    if run_id not in existing_runs and not dry_run:
        _import_cwe_aware(db, run_id, V0317_CWE_AWARE)
        print(f"    + {len(V0317_CWE_AWARE)} CWE-aware entries for {run_id}")

    # ── Import CWE-aware data for v0.3.19 ────────────────────────────────
    # v0.3.19 is fast mode — add as a separate run
    run_id_319 = "sqc-0.3.19-fast-historical"
    if run_id_319 not in existing_runs:
        if not dry_run:
            machine = MACHINES["4-core"]
            db.create_run(
                run_id=run_id_319,
                sqc_version="0.3.19",
                commit_sha="historical",
                mode="fast",
                started_at="2025-01-01T00:00:00+00:00",
                pid=0, jobs=12, total_cwes=68,
                machine=machine,
            )
            db.finish_run(run_id_319, "completed", "2025-01-01T00:00:00+00:00")

            # Aggregate for v0.3.19 fast mode — not in Version History table
            # Sum from CWE-aware data
            total_tp = sum(tp for _, tp, _, _, _ in V0319_CWE_AWARE)
            total_fp = sum(fp for _, _, fp, _, _ in V0319_CWE_AWARE)
            total = total_tp + total_fp
            agg_scan_id = db.create_cwe_scan(run_id_319, "ALL", "AGGREGATE", file_count=0)
            db.update_cwe_scan(agg_scan_id, status="completed", violation_count=total,
                               duration_s=480)  # ~8 min
            db.insert_cwe_metrics({
                "cwe_scan_id": agg_scan_id,
                "tp_count": total_tp, "fp_count": total_fp,
                "tp_rate_pct": round(total_tp / total * 100, 1) if total else 0,
                "flaw_lines_total": 0, "flaw_lines_detected": 0,
                "flaw_detection_rate_pct": 0, "cwe_matched_tp": total_tp,
                "cwe_matched_fp": total_fp, "noise_count": 0, "noise_ratio": 0,
                "per_file_detected": 0, "per_file_total": 0, "per_file_rate": 0,
                "flaw_hit_detected": 0, "flaw_hit_total": 0, "flaw_hit_rate": 0,
            })

            _import_cwe_aware(db, run_id_319, V0319_CWE_AWARE)
            print(f"  IMPORT: {run_id_319} + {len(V0319_CWE_AWARE)} CWE-aware entries")
            imported += 1
        else:
            print(f"  DRY-RUN: {run_id_319} (v0.3.19 fast mode)")
            imported += 1

    # ── Real-world results ───────────────────────────────────────────────
    print("\n--- Real-World Results ---")
    rw_imported = _backfill_realworld(db, dry_run)
    imported += rw_imported

    print(f"\nDone: {imported} imported, {skipped} skipped")


# ── Real-world benchmark data (from REALWORLD_RESULTS.md) ────────────────────

# Project metadata: (project, c_files, loc)
PROJECTS = {
    "libcrc":    (16,  2130),
    "sqlite":    (310, 402321),
    "mosquitto": (384, 88717),
    "curl":      (697, 240412),
    "hostap":    (505, 541441),
}

# v0.2.3 baseline — all three tools
# (project, sqc, cppcheck, clang_tidy)
V023_RESULTS = [
    ("libcrc",    954,    40,   52),
    ("sqlite",    424842, 1182, 2291),
    ("mosquitto", 47417,  598,  907),
    ("curl",      207476, 551,  1653),
    ("hostap",    473862, 1066, 1083),
]

# v0.2.7 — all three tools (first MCP benchmark)
V027_RESULTS = [
    ("libcrc",    842,    40,   4),
    ("sqlite",    180011, 517,  204),
    ("mosquitto", 39177,  364,  160),
    ("curl",      93576,  297,  1314),
    ("hostap",    234421, 1675, 2957),
]

# v0.3.5 — all three tools
V035_RESULTS = [
    ("libcrc",    734,    43,   2),
    ("sqlite",    129035, 1181, 135),
    ("mosquitto", 29824,  747,  44),
    ("curl",      63207,  519,  114),
    ("hostap",    179833, 2118, 2279),
]

# sqc-only version history (from sqc Version History table)
# (version, [(project, violations), ...])
SQC_ONLY_VERSIONS = {
    "0.2.13": [
        ("libcrc", 811), ("mosquitto", 33638), ("sqlite", 147091),
        ("curl", 73816), ("hostap", 206906),
    ],
    "0.2.16": [
        ("libcrc", 790), ("mosquitto", 33200), ("sqlite", 144581),
        ("curl", 73239), ("hostap", 204560),
    ],
    "0.2.21": [
        ("libcrc", 777), ("mosquitto", 29997), ("sqlite", 130774),
        ("curl", 64393), ("hostap", 184952),
    ],
    "0.2.22": [
        ("libcrc", 777), ("mosquitto", 29989), ("sqlite", 130802),
        ("curl", 64389), ("hostap", 185197),
    ],
}


def _backfill_realworld(db: BenchDB, dry_run: bool) -> int:
    """Backfill real-world benchmark results."""
    existing = {(r["sqc_version"], r.get("notes", ""))
                for r in db.list_realworld_runs()}
    imported = 0

    # Helper to check and insert
    def _insert_run(version, commit_sha, notes, results, machine="24-core"):
        nonlocal imported
        if (version, notes) in existing:
            print(f"  SKIP (exists): rw {version} ({notes})")
            return
        print(f"  {'DRY-RUN' if dry_run else 'IMPORT'}: rw {version} ({notes}) "
              f"— {len(results)} project entries")
        if dry_run:
            imported += 1
            return

        m = MACHINES.get(machine, MACHINES["24-core"])
        run_id = db.create_realworld_run(
            sqc_version=version,
            commit_sha=commit_sha,
            hostname=m["hostname"],
            cpu_model=m["cpu_model"],
            cpu_cores=m["cpu_cores"],
            notes=notes,
        )
        for entry in results:
            if len(entry) == 4:
                project, sqc, cppcheck, clang_tidy = entry
                files, loc = PROJECTS.get(project, (0, 0))
                db.insert_realworld_result(run_id, project, "sqc",
                                           c_files=files, loc=loc,
                                           violation_count=sqc)
                db.insert_realworld_result(run_id, project, "cppcheck",
                                           c_files=files, loc=loc,
                                           violation_count=cppcheck)
                db.insert_realworld_result(run_id, project, "clang-tidy",
                                           c_files=files, loc=loc,
                                           violation_count=clang_tidy)
            elif len(entry) == 2:
                project, violations = entry
                files, loc = PROJECTS.get(project, (0, 0))
                db.insert_realworld_result(run_id, project, "sqc",
                                           c_files=files, loc=loc,
                                           violation_count=violations)
        imported += 1

    # v0.2.3 baseline
    _insert_run("0.2.3", None, "baseline (before STR31-C rewrite)", V023_RESULTS)

    # v0.2.7 first MCP benchmark
    _insert_run("0.2.7", "54819432", "first MCP benchmark", V027_RESULTS)

    # sqc-only intermediate versions
    for version, data in sorted(SQC_ONLY_VERSIONS.items()):
        _insert_run(version, None, f"sqc-only version history", data)

    # v0.3.5 latest with all tools
    _insert_run("0.3.5", "8b8e1eec", "latest all-tools benchmark", V035_RESULTS)

    return imported


def _import_per_cwe_basic(db: BenchDB, run_id: str, data: list):
    """Import per-CWE data as (cwe_id, files, tp, fp, tp_rate) tuples."""
    for entry in data:
        if len(entry) == 4:
            cwe_id, tp, fp, tp_rate = entry
            files = 0
        else:
            cwe_id, files, tp, fp, tp_rate = entry

        # Create a CWE dir name from the ID
        cwe_num = cwe_id.replace("CWE-", "")
        cwe_dir_name = f"CWE{cwe_num}"

        scan_id = db.create_cwe_scan(run_id, cwe_id, cwe_dir_name, file_count=files)
        db.update_cwe_scan(scan_id, status="completed", violation_count=tp + fp)
        db.insert_cwe_metrics({
            "cwe_scan_id": scan_id,
            "tp_count": tp, "fp_count": fp, "tp_rate_pct": tp_rate,
            "flaw_lines_total": 0, "flaw_lines_detected": 0,
            "flaw_detection_rate_pct": 0,
            "cwe_matched_tp": 0, "cwe_matched_fp": 0,
            "noise_count": 0, "noise_ratio": 0,
            "per_file_detected": 0, "per_file_total": 0, "per_file_rate": 0,
            "flaw_hit_detected": 0, "flaw_hit_total": 0, "flaw_hit_rate": 0,
        })


def _import_cwe_aware(db: BenchDB, run_id: str, data: list):
    """Import CWE-aware data as (cwe_id, matched_tp, matched_fp, precision, per_file_rate)."""
    for cwe_id, matched_tp, matched_fp, precision, per_file_rate in data:
        cwe_num = cwe_id.replace("CWE-", "")
        cwe_dir_name = f"CWE{cwe_num}"
        total = matched_tp + matched_fp

        scan_id = db.create_cwe_scan(run_id, cwe_id, cwe_dir_name, file_count=0)
        db.update_cwe_scan(scan_id, status="completed", violation_count=total)
        db.insert_cwe_metrics({
            "cwe_scan_id": scan_id,
            "tp_count": matched_tp, "fp_count": matched_fp,
            "tp_rate_pct": precision,
            "flaw_lines_total": 0, "flaw_lines_detected": 0,
            "flaw_detection_rate_pct": 0,
            "cwe_matched_tp": matched_tp, "cwe_matched_fp": matched_fp,
            "noise_count": 0, "noise_ratio": 0,
            "per_file_detected": 0, "per_file_total": 0,
            "per_file_rate": per_file_rate,
            "flaw_hit_detected": 0, "flaw_hit_total": 0, "flaw_hit_rate": 0,
        })


if __name__ == "__main__":
    dry_run = "--dry-run" in sys.argv
    if dry_run:
        print("=== DRY RUN (no changes) ===\n")
    else:
        print("=== Backfilling historical results into SQLite ===\n")
    backfill(dry_run=dry_run)

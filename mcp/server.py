#!/usr/bin/env python3
"""
MCP server for running and monitoring the Juliet CERT C benchmark against sqc.

Tools:
  run_benchmark          - Start (or restart) a fresh benchmark run
  get_status             - Progress %, ETA, recently completed CWEs
  get_results(sort_by)   - Aggregated TP/FP stats + per-rule breakdown
  get_cwe_detail(cwe_id) - Detailed stats for one CWE
"""

import json
import os
import re
import subprocess
import time
from pathlib import Path

from mcp.server.fastmcp import FastMCP

# ── Paths ─────────────────────────────────────────────────────────────────────
_HERE = Path(__file__).parent
PROJECT_DIR = _HERE.parent
SCRIPT = PROJECT_DIR / "scripts" / "run_juliet_parallel.sh"
RESULTS_DIR = Path("/tmp/juliet_results")
LOG_FILE = Path("/tmp/juliet_bench.log")
PID_FILE = Path("/tmp/juliet_bench.pid")

# The benchmark script knows its total CWE list; we use 118 as the known count.
KNOWN_TOTAL_CWES = 118

# ── MCP server ────────────────────────────────────────────────────────────────
mcp = FastMCP(
    "juliet-benchmark",
    instructions="Run and monitor the Juliet C benchmark suite against sqc",
)


# ── Internal helpers ──────────────────────────────────────────────────────────

def _read_state() -> dict | None:
    """Read persisted benchmark state (PID + start time) from disk."""
    try:
        return json.loads(PID_FILE.read_text())
    except Exception:
        return None


def _write_state(pid: int, start_time: float) -> None:
    PID_FILE.write_text(json.dumps({"pid": pid, "start_time": start_time}))


def _process_alive(pid: int) -> bool:
    try:
        os.kill(pid, 0)
        return True
    except (ProcessLookupError, PermissionError):
        return False


def _parse_log() -> dict:
    """
    Parse /tmp/juliet_bench.log and return:
      done   - list of completed CWEs with timing/violation data
      started - set of CWE names that have been started
      errors  - error lines
    """
    if not LOG_FILE.exists():
        return {"done": [], "started": set(), "errors": []}

    done: list[dict] = []
    started: set[str] = set()
    errors: list[str] = []
    done_names: set[str] = set()  # dedup (script may log twice on retry)

    for line in LOG_FILE.read_text().splitlines():
        if line.startswith("DONE:"):
            # DONE: CWE78_OS_Command_Injection | 1276s | 125780 violations | 5600 files
            m = re.match(
                r"DONE: (\S+) \| (\d+)s \| (\d+) violations \| (\d+) files", line
            )
            if m and m.group(1) not in done_names:
                done_names.add(m.group(1))
                done.append(
                    {
                        "cwe": m.group(1),
                        "duration_s": int(m.group(2)),
                        "violations": int(m.group(3)),
                        "files": int(m.group(4)),
                    }
                )
        elif line.startswith("START:"):
            # START: CWE78_OS_Command_Injection (5600 files)
            m = re.match(r"START: (\S+)", line)
            if m:
                started.add(m.group(1))
        elif line.startswith("ERROR:"):
            errors.append(line)

    return {"done": done, "started": started, "errors": errors}


def _parse_analysis(content: str) -> dict:
    """Extract TP/FP counts and per-rule breakdown from an analysis .txt file."""
    tp_m = re.search(r"Violations in OMITBAD \(TP\): (\d+)", content)
    fp_m = re.search(r"Violations in OMITGOOD \(FP\): (\d+)", content)
    flaw_m = re.search(r"FLAW lines detected: (\d+) / (\d+)", content)
    files_m = re.search(r"Files analyzed: (\d+)", content)

    tp = int(tp_m.group(1)) if tp_m else 0
    fp = int(fp_m.group(1)) if fp_m else 0

    top_tp: list[dict] = []
    top_fp: list[dict] = []
    flaw_rules: list[dict] = []
    in_bad = in_good = in_flaw = False

    for line in content.splitlines():
        if "Top 10 Rules in OMITBAD" in line:
            in_bad, in_good, in_flaw = True, False, False
        elif "Top 10 Rules in OMITGOOD" in line:
            in_bad, in_good, in_flaw = False, True, False
        elif "Top Rules on FLAW Lines" in line:
            in_bad, in_good, in_flaw = False, False, True
        elif line.startswith("---") or line.startswith("==="):
            in_bad = in_good = in_flaw = False
        else:
            m = re.match(r"\s+(\w[\w-]+):\s+(\d+)", line)
            if m:
                entry = {"rule": m.group(1), "count": int(m.group(2))}
                if in_bad:
                    top_tp.append(entry)
                elif in_good:
                    top_fp.append(entry)
                elif in_flaw:
                    flaw_rules.append(entry)

    return {
        "tp": tp,
        "fp": fp,
        "files": int(files_m.group(1)) if files_m else 0,
        "flaw_detected": int(flaw_m.group(1)) if flaw_m else 0,
        "flaw_total": int(flaw_m.group(2)) if flaw_m else 0,
        "top_tp_rules": top_tp,
        "top_fp_rules": top_fp,
        "flaw_line_rules": flaw_rules,
    }


# ── Tools ─────────────────────────────────────────────────────────────────────

@mcp.tool()
def run_benchmark() -> str:
    """
    Start a fresh Juliet benchmark run against sqc.

    Clears all previous results from /tmp/juliet_results/ and restarts.
    Returns immediately — use get_status() to monitor progress.
    If a benchmark is already running, returns its current PID instead of starting a new one.
    """
    state = _read_state()
    if state and _process_alive(state["pid"]):
        elapsed = int(time.time() - state["start_time"])
        return json.dumps(
            {
                "status": "already_running",
                "pid": state["pid"],
                "elapsed_seconds": elapsed,
                "message": "Benchmark already running. Use get_status() to monitor.",
            }
        )

    # Clear old results
    if RESULTS_DIR.exists():
        for f in RESULTS_DIR.glob("*.csv"):
            f.unlink(missing_ok=True)
        for f in RESULTS_DIR.glob("*.txt"):
            f.unlink(missing_ok=True)
    else:
        RESULTS_DIR.mkdir(parents=True)

    LOG_FILE.unlink(missing_ok=True)

    # Launch benchmark detached from the MCP server process so it survives
    # even if the MCP server is restarted.
    log_fh = LOG_FILE.open("w")
    proc = subprocess.Popen(
        ["bash", str(SCRIPT)],
        stdout=log_fh,
        stderr=subprocess.STDOUT,
        start_new_session=True,  # detach from MCP server process group
    )
    log_fh.close()  # MCP server doesn't need to hold the handle

    start_time = time.time()
    _write_state(proc.pid, start_time)

    return json.dumps(
        {
            "status": "started",
            "pid": proc.pid,
            "message": (
                f"Benchmark started (PID {proc.pid}). "
                "Results appear in /tmp/juliet_results/. "
                "Use get_status() to monitor progress."
            ),
        }
    )


@mcp.tool()
def get_status() -> str:
    """
    Get the current status of the Juliet benchmark run.

    Returns progress percentage, estimated time remaining, number of CWEs
    completed vs total, and the 5 most recently completed CWEs with their
    timing and violation counts.
    """
    state = _read_state()
    if state is None:
        return json.dumps(
            {
                "state": "not_started",
                "message": "No benchmark has been run yet. Use run_benchmark() to start.",
            }
        )

    log_data = _parse_log()
    done = log_data["done"]
    done_count = len(done)
    is_running = _process_alive(state["pid"])

    summary_file = RESULTS_DIR / "multi_cwe_summary.txt"
    is_complete = summary_file.exists() and not is_running

    elapsed_s = int(time.time() - state["start_time"])

    # Use the known total; fall back to observed started count if higher.
    total_cwes = max(KNOWN_TOTAL_CWES, len(log_data["started"]), done_count)

    progress_pct = 0.0
    eta_s = None
    if done_count > 0:
        progress_pct = round(done_count / total_cwes * 100, 1)
        if is_running and elapsed_s > 0:
            rate = done_count / elapsed_s  # CWEs per second
            remaining = total_cwes - done_count
            eta_s = int(remaining / rate) if rate > 0 else None

    result: dict = {
        "state": (
            "completed" if is_complete else "running" if is_running else "crashed"
        ),
        "progress_pct": progress_pct,
        "done_cwes": done_count,
        "total_cwes": total_cwes,
        "elapsed_seconds": elapsed_s,
        "eta_seconds": eta_s,
        "recently_completed": done[-5:],
        "errors": log_data["errors"],
    }

    if is_complete:
        result["message"] = (
            f"Benchmark complete. {done_count}/{total_cwes} CWEs analyzed. "
            "Use get_results() for aggregated stats or get_cwe_detail(cwe_id) for specifics."
        )
    elif is_running:
        eta_str = f"{eta_s // 60}m {eta_s % 60}s" if eta_s else "unknown"
        result["message"] = (
            f"{done_count}/{total_cwes} CWEs done ({progress_pct}%). ETA: {eta_str}."
        )

    return json.dumps(result)


@mcp.tool()
def get_results(sort_by: str = "fp_count") -> str:
    """
    Get aggregated TP/FP results across all completed CWEs.

    Args:
        sort_by: How to sort the per-rule breakdown.
                 One of: "fp_count" (default), "fp_rate", "tp_count"

    Returns a summary (total TP, FP, TP rate), the top 20 rules by the chosen
    sort key, and a per-CWE table sorted by FP count.
    """
    if not RESULTS_DIR.exists() or not list(RESULTS_DIR.glob("*_analysis.txt")):
        return json.dumps(
            {
                "error": (
                    "No analysis files found. "
                    "Run run_benchmark() and wait for it to complete."
                )
            }
        )

    total_tp = 0
    total_fp = 0
    rule_tp: dict[str, int] = {}
    rule_fp: dict[str, int] = {}
    per_cwe: list[dict] = []

    for f in sorted(RESULTS_DIR.glob("*_analysis.txt")):
        cwe_name = f.stem.replace("_analysis", "")
        parsed = _parse_analysis(f.read_text())
        tp, fp = parsed["tp"], parsed["fp"]
        cwe_total = tp + fp

        total_tp += tp
        total_fp += fp

        per_cwe.append(
            {
                "cwe": cwe_name,
                "tp": tp,
                "fp": fp,
                "total": cwe_total,
                "tp_pct": round(tp / cwe_total * 100, 1) if cwe_total else 0,
                "fp_pct": round(fp / cwe_total * 100, 1) if cwe_total else 0,
            }
        )

        for entry in parsed["top_tp_rules"]:
            rule_tp[entry["rule"]] = rule_tp.get(entry["rule"], 0) + entry["count"]
        for entry in parsed["top_fp_rules"]:
            rule_fp[entry["rule"]] = rule_fp.get(entry["rule"], 0) + entry["count"]

    # Build per-rule table
    all_rules = set(rule_tp) | set(rule_fp)
    rules_data: list[dict] = []
    for rule in all_rules:
        tp = rule_tp.get(rule, 0)
        fp = rule_fp.get(rule, 0)
        total = tp + fp
        rules_data.append(
            {
                "rule": rule,
                "fp": fp,
                "tp": tp,
                "total": total,
                "fp_pct": round(fp / total * 100, 1) if total else 0,
            }
        )

    sort_keys = {
        "fp_count": lambda x: -x["fp"],
        "fp_rate": lambda x: -x["fp_pct"],
        "tp_count": lambda x: -x["tp"],
    }
    rules_data.sort(key=sort_keys.get(sort_by, sort_keys["fp_count"]))

    grand_total = total_tp + total_fp
    return json.dumps(
        {
            "summary": {
                "total_violations": grand_total,
                "total_tp": total_tp,
                "total_fp": total_fp,
                "tp_rate_pct": round(total_tp / grand_total * 100, 1) if grand_total else 0,
                "fp_rate_pct": round(total_fp / grand_total * 100, 1) if grand_total else 0,
                "cwes_analyzed": len(per_cwe),
                "sort_by": sort_by,
            },
            "top_rules": rules_data[:20],
            "per_cwe": sorted(per_cwe, key=lambda x: -x["fp"]),
        }
    )


@mcp.tool()
def get_cwe_detail(cwe_id: str) -> str:
    """
    Get detailed TP/FP breakdown for a specific CWE.

    Args:
        cwe_id: CWE identifier. Accepts any of:
                "CWE78", "78", "CWE78_OS_Command_Injection"

    Returns file count, TP/FP rates, top contributing rules for TPs and FPs,
    and FLAW-line detection statistics.
    """
    if not RESULTS_DIR.exists():
        return json.dumps(
            {"error": "No results found. Run run_benchmark() first."}
        )

    # Normalise: ensure it starts with "CWE" (case-insensitive match)
    needle = cwe_id.upper()
    if not needle.startswith("CWE"):
        needle = "CWE" + needle

    # Match CWE78 → CWE78_... but NOT CWE780_... by requiring _ or end after the ID.
    matches = [
        f
        for f in RESULTS_DIR.glob("*_analysis.txt")
        if re.match(rf"^{re.escape(needle)}(_|$)", f.name.upper())
    ]

    if not matches:
        available = sorted(
            f.stem.replace("_analysis", "")
            for f in RESULTS_DIR.glob("*_analysis.txt")
        )
        return json.dumps(
            {
                "error": f"No results found for '{cwe_id}'.",
                "tip": "Use get_status() to see which CWEs are done.",
                "available_cwes": available,
            }
        )

    f = matches[0]
    cwe_name = f.stem.replace("_analysis", "")
    parsed = _parse_analysis(f.read_text())

    tp, fp = parsed["tp"], parsed["fp"]
    total = tp + fp

    return json.dumps(
        {
            "cwe": cwe_name,
            "files_analyzed": parsed["files"],
            "summary": {
                "total_violations": total,
                "tp": tp,
                "fp": fp,
                "tp_rate_pct": round(tp / total * 100, 1) if total else 0,
                "fp_rate_pct": round(fp / total * 100, 1) if total else 0,
                "flaw_lines_detected": parsed["flaw_detected"],
                "flaw_lines_total": parsed["flaw_total"],
                "flaw_detection_rate_pct": (
                    round(parsed["flaw_detected"] / parsed["flaw_total"] * 100, 1)
                    if parsed["flaw_total"]
                    else 0
                ),
            },
            "top_tp_rules": parsed["top_tp_rules"],
            "top_fp_rules": parsed["top_fp_rules"],
            "flaw_line_rules": parsed["flaw_line_rules"],
        }
    )


if __name__ == "__main__":
    mcp.run()

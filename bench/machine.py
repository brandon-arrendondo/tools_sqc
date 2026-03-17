"""Collect machine metadata (CPU, RAM, hostname) for benchmark provenance."""

import os
import platform
import re
from pathlib import Path


def get_machine_metadata() -> dict:
    """Return a dict of machine metadata for embedding in benchmark runs."""
    return {
        "hostname": platform.node(),
        "cpu_model": _get_cpu_model(),
        "cpu_cores": os.cpu_count() or 0,
        "ram_gb": _get_ram_gb(),
        "os_version": _get_os_version(),
    }


def _get_cpu_model() -> str:
    try:
        for line in Path("/proc/cpuinfo").read_text().splitlines():
            if line.startswith("model name"):
                return line.split(":", 1)[1].strip()
    except Exception:
        pass
    return platform.processor() or "unknown"


def _get_ram_gb() -> float:
    try:
        for line in Path("/proc/meminfo").read_text().splitlines():
            if line.startswith("MemTotal:"):
                kb = int(re.search(r"\d+", line).group())
                return round(kb / 1048576, 1)
    except Exception:
        pass
    return 0.0


def _get_os_version() -> str:
    try:
        return platform.platform()
    except Exception:
        return "unknown"

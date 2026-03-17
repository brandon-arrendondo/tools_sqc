#!/usr/bin/env python3
"""
MCP server for running sqc, cppcheck, and clang-tidy against real open-source
codebases (libcrc, sqlite, mosquitto, curl, hostap) and tracking results with
version+commit SHA tagging.

Supports local and remote execution via SSH. Remote hosts share identical paths
(same username, same directory layout). Results are fetched back via scp.

Tools:
  run_analysis(tool, codebase, host) - Start one tool×codebase run (local or remote)
  run_all(codebase, tool, host)      - Convenience: run multiple combos
  get_status()                       - Show all active/completed/failed runs
  get_results(run_id)                - Parse results for a run or latest version
  compare_runs(base, target)         - Compare two version dirs
  list_runs()                        - List all version directories
  cancel_run(run_id)                 - Cancel a specific or all active runs
  purge_run(run_id, zombies)         - Remove stale/zombie runs from tracking
  clear_results()                    - Remove old result directories
  deploy_sqc(host)                   - Deploy sqc binary to remote host(s)
"""

import fcntl
import json
import os
import re
import shlex
import shutil
import signal
import subprocess
import tempfile
import time
import xml.etree.ElementTree as ET
from pathlib import Path

from mcp.server.fastmcp import FastMCP

# ── Paths ─────────────────────────────────────────────────────────────────────
_HERE = Path(__file__).parent
PROJECT_DIR = _HERE.parent
RESULTS_BASE = Path("/tmp/realworld_results")
STATE_FILE = Path("/tmp/realworld_bench.json")
MANIFEST = PROJECT_DIR / "rules_templates" / "rules-all.toml"
SQC_BIN = PROJECT_DIR / "target" / "release" / "sqc"

VALID_TOOLS = {"sqc", "cppcheck", "clang-tidy"}

# ── Remote execution ─────────────────────────────────────────────────────────
# Loaded from mcp/remote_hosts.json (gitignored). If missing, remote is disabled.
REMOTE_HOSTS_CONFIG = _HERE / "remote_hosts.json"
SSH_OPTS = ["-o", "BatchMode=yes", "-o", "ConnectTimeout=10"]


def _load_remote_config() -> tuple[dict[str, str], str]:
    """Load remote hosts config. Returns (hosts_dict, ssh_user)."""
    try:
        data = json.loads(REMOTE_HOSTS_CONFIG.read_text())
        hosts = data.get("hosts", {})
        user = data.get("ssh_user", "brandon")
        return hosts, user
    except (FileNotFoundError, json.JSONDecodeError):
        return {}, "brandon"

# ── Codebase Registry ─────────────────────────────────────────────────────────
CODEBASES = {
    "libcrc": {
        "path": Path.home() / "data" / "libcrc",
        "sqc": {
            "scan_path": None,  # same as path (whole project)
            "extra_args": [],
        },
        "cppcheck": {
            "includes": ["-I", "{path}/include"],
            "source_dirs": ["{path}/src/"],
            "extra_args": [],
        },
        "clang-tidy": {
            "includes": ["-I", "{path}/include"],
            "source_dirs": ["{path}/src/"],
        },
    },
    "sqlite": {
        "path": Path.home() / "data" / "sqlite",
        "sqc": {
            "scan_path": None,
            "extra_args": [],
        },
        "cppcheck": {
            "includes": ["-I", "{path}/src"],
            "source_dirs": ["{path}/src/"],
            "extra_args": [],
        },
        "clang-tidy": {
            "includes": ["-I", "{path}/src"],
            "source_dirs": ["{path}/src/"],
        },
    },
    "mosquitto": {
        "path": Path.home() / "data" / "mosquitto",
        "sqc": {
            "scan_path": None,
            "extra_args": [],
        },
        "cppcheck": {
            "includes": [
                "-I", "{path}/include",
                "-I", "{path}/common",
            ],
            "source_dirs": ["{path}/lib/", "{path}/src/"],
            "extra_args": ["-i", "{path}/deps"],
        },
        "clang-tidy": {
            "includes": [
                "-I", "{path}/include",
                "-I", "{path}/common",
            ],
            "source_dirs": ["{path}/lib/", "{path}/src/"],
        },
    },
    "curl": {
        "path": Path.home() / "data" / "curl",
        "sqc": {
            "scan_path": None,
            "extra_args": [],
        },
        "cppcheck": {
            "includes": [
                "-I", "{path}/include",
                "-I", "{path}/lib",
            ],
            "source_dirs": ["{path}/lib/", "{path}/src/"],
            "extra_args": [],
        },
        "clang-tidy": {
            "includes": [
                "-I", "{path}/include",
                "-I", "{path}/lib",
            ],
            "source_dirs": ["{path}/lib/", "{path}/src/"],
        },
    },
    "hostap": {
        "path": Path.home() / "data" / "hostap",
        "sqc": {
            "scan_path": None,
            "extra_args": [
                "-d", "{path}/src",
                "-d", "{path}/wpa_supplicant",
            ],
        },
        "cppcheck": {
            "includes": [
                "-I", "{path}/src",
                "-I", "{path}/src/utils",
                "-I", "{path}/src/common",
            ],
            "source_dirs": ["{path}/src/", "{path}/wpa_supplicant/"],
            "extra_args": [],
        },
        "clang-tidy": {
            "includes": [
                "-I", "{path}/src",
                "-I", "{path}/src/utils",
                "-I", "{path}/src/common",
                "-I", "{path}/src/crypto",
            ],
            "source_dirs": ["{path}/src/"],
        },
    },
}

# ── MCP server ────────────────────────────────────────────────────────────────
mcp = FastMCP(
    "realworld-benchmark",
    instructions=(
        "Run sqc, cppcheck, and clang-tidy against real open-source C codebases "
        "(libcrc, sqlite, mosquitto, curl, hostap) and compare results across versions. "
        "Supports remote execution via SSH if mcp/remote_hosts.json is configured — "
        "pass host parameter to run_analysis/run_all. Use deploy_sqc to push "
        "the sqc binary to remote hosts before running."
    ),
)


# ── Internal helpers ──────────────────────────────────────────────────────────

def _fmt_duration(seconds: int) -> str:
    h, rem = divmod(seconds, 3600)
    m, s = divmod(rem, 60)
    parts = []
    if h:
        parts.append(f"{h}h")
    if m or h:
        parts.append(f"{m}m")
    parts.append(f"{s}s")
    return " ".join(parts)


def _get_sqc_version() -> str:
    try:
        for line in (PROJECT_DIR / "Cargo.toml").read_text().splitlines():
            m = re.match(r'^version\s*=\s*"([^"]+)"', line)
            if m:
                return m.group(1)
    except Exception:
        pass
    return "unknown"


def _get_git_sha() -> str:
    try:
        result = subprocess.run(
            ["git", "rev-parse", "--short", "HEAD"],
            capture_output=True, text=True, cwd=PROJECT_DIR, timeout=5,
        )
        return result.stdout.strip() if result.returncode == 0 else "unknown"
    except Exception:
        return "unknown"


def _get_tool_version(tool: str) -> str:
    """Get the version string for any supported tool."""
    if tool == "sqc":
        return _get_sqc_version()
    elif tool == "cppcheck":
        try:
            result = subprocess.run(
                ["cppcheck", "--version"],
                capture_output=True, text=True, timeout=5,
            )
            # Output: "Cppcheck 2.13" or similar
            m = re.search(r"(\d+\.\d+(?:\.\d+)?)", result.stdout)
            return m.group(1) if m else "unknown"
        except Exception:
            return "unknown"
    elif tool == "clang-tidy":
        try:
            result = subprocess.run(
                ["clang-tidy", "--version"],
                capture_output=True, text=True, timeout=5,
            )
            # Look for "LLVM version X.Y.Z" or "clang-tidy version X.Y.Z"
            m = re.search(r"version\s+(\d+\.\d+(?:\.\d+)?)", result.stdout)
            return m.group(1) if m else "unknown"
        except Exception:
            return "unknown"
    return "unknown"


def _make_version_dir_name(tool: str, version: str, sha: str) -> str:
    """Build version directory name: sqc includes git SHA, others just tool-version."""
    if tool == "sqc":
        return f"sqc-{version}-{sha}"
    return f"{tool}-{version}"


_STATE_LOCK = Path("/tmp/realworld_bench.lock")


def _read_state() -> dict:
    """Read persisted state. Returns dict with 'runs' key (map of run_id → run info)."""
    try:
        data = json.loads(STATE_FILE.read_text())
        if "runs" not in data:
            data["runs"] = {}
        return data
    except Exception:
        return {"runs": {}}


def _write_state(state: dict) -> None:
    """Atomic write: write to temp file then rename (POSIX atomic)."""
    content = json.dumps(state, indent=2)
    fd, tmp = tempfile.mkstemp(dir=STATE_FILE.parent, suffix=".tmp")
    closed = False
    try:
        os.write(fd, content.encode())
        os.fsync(fd)
        os.close(fd)
        closed = True
        os.rename(tmp, str(STATE_FILE))
    except Exception:
        if not closed:
            os.close(fd)
        Path(tmp).unlink(missing_ok=True)
        raise


class _StateLock:
    """Context manager for exclusive access to the state file.

    Usage:
        with _StateLock() as state:
            state["runs"][run_id] = {...}
        # State is automatically written on exit
    """

    def __init__(self):
        self._fd = None
        self._state = None

    def __enter__(self) -> dict:
        self._fd = open(_STATE_LOCK, "w")
        fcntl.flock(self._fd, fcntl.LOCK_EX)
        self._state = _read_state()
        return self._state

    def __exit__(self, exc_type, exc_val, exc_tb):
        try:
            if exc_type is None:
                _write_state(self._state)
        finally:
            fcntl.flock(self._fd, fcntl.LOCK_UN)
            self._fd.close()
        return False


def _process_alive(pid: int) -> bool:
    try:
        os.kill(pid, 0)
    except (ProcessLookupError, PermissionError):
        return False
    try:
        status = Path(f"/proc/{pid}/status").read_text()
        for line in status.splitlines():
            if line.startswith("State:") and "zombie" in line.lower():
                return False
    except Exception:
        pass
    return True


def _remote_check_done(host: str, version_dir: Path, run_id: str) -> bool:
    """Check if a remote run finished by looking for its .done sentinel file."""
    _, ssh_user = _load_remote_config()
    sentinel = f"{version_dir / (run_id + '.done')}"
    try:
        result = subprocess.run(
            ["ssh"] + SSH_OPTS + [f"{ssh_user}@{host}", f"test -f {shlex.quote(sentinel)}"],
            capture_output=True, timeout=10,
        )
        return result.returncode == 0
    except Exception:
        return False


def _kill_process_group(pid: int) -> None:
    try:
        os.killpg(pid, signal.SIGTERM)
    except (ProcessLookupError, PermissionError):
        return
    time.sleep(1.0)
    try:
        os.killpg(pid, signal.SIGKILL)
    except (ProcessLookupError, PermissionError):
        pass
    try:
        os.waitpid(pid, os.WNOHANG)
    except ChildProcessError:
        pass


def _dir_size_human(path: Path) -> str:
    total = 0
    try:
        for f in path.rglob("*"):
            if f.is_file():
                total += f.stat().st_size
    except Exception:
        pass
    for unit in ("B", "KB", "MB", "GB"):
        if total < 1024:
            return f"{total:.1f} {unit}"
        total /= 1024
    return f"{total:.1f} TB"


def _make_run_id(tool: str, codebase: str, version: str, sha: str) -> str:
    return f"{tool}-{codebase}-{version}-{sha}"


def _expand(template_list: list[str], path: str) -> list[str]:
    """Expand {path} placeholders in argument lists."""
    return [s.replace("{path}", path) for s in template_list]


def _is_local(host: str | None) -> bool:
    """Returns True if host is None, 'local', or loopback."""
    return host is None or host.lower() in ("local", "localhost", "127.0.0.1")


def _resolve_host(host: str | None) -> str | dict:
    """Validate host. Returns IP string, or error dict if invalid."""
    if _is_local(host):
        return "local"
    hosts, _ = _load_remote_config()
    if not hosts:
        return {"error": "Remote execution not configured. Create mcp/remote_hosts.json."}
    host = host.strip()
    if host in hosts:
        return host
    # Try matching by nickname
    for ip, name in hosts.items():
        if host == name:
            return ip
    return {
        "error": f"Unknown host '{host}'. Must be one of: local, "
                 + ", ".join(f"{ip} ({name})" for ip, name in hosts.items()),
    }


def _build_remote_shell_cmd(tool: str, cmd: list[str], version_dir: Path, run_id: str) -> str:
    """Convert a local command + I/O redirections into a single shell string for SSH.

    Every remote command ends by touching a .done sentinel file so the local
    server can cheaply detect completion without tracking remote PIDs.
    """
    dir_str = shlex.quote(str(version_dir))
    mkdir = f"mkdir -p {dir_str}"
    done = shlex.quote(str(version_dir / f"{run_id}.done"))
    sentinel = f"touch {done}"

    if tool == "sqc":
        # sqc writes JSON via --export; stdout/stderr → log
        log = shlex.quote(str(version_dir / f"{run_id}.log"))
        cmd_str = " ".join(shlex.quote(c) for c in cmd)
        return f"{mkdir} && {cmd_str} > {log} 2>&1 ; {sentinel}"

    elif tool == "cppcheck":
        # cppcheck: stderr is XML output, stdout → log
        xml = shlex.quote(str(version_dir / f"{run_id}.xml"))
        log = shlex.quote(str(version_dir / f"{run_id}.log"))
        cmd_str = " ".join(shlex.quote(c) for c in cmd)
        return f"{mkdir} && {cmd_str} > {log} 2> {xml} ; {sentinel}"

    else:  # clang-tidy
        # clang-tidy cmd is ["bash", "-c", "pipeline..."]
        # Extract the pipeline string and wrap with redirections
        pipeline = cmd[2]  # the bash -c argument
        txt = shlex.quote(str(version_dir / f"{run_id}.txt"))
        log = shlex.quote(str(version_dir / f"{run_id}.log"))
        return f"{mkdir} && bash -c {shlex.quote(pipeline)} > {txt} 2> {log} ; {sentinel}"


def _fetch_remote_results(host: str, version_dir: Path, run_id: str) -> dict:
    """SCP result + log files from remote host. Returns {fetched, failed}."""
    fetched = []
    failed = []
    version_dir.mkdir(parents=True, exist_ok=True)
    _, ssh_user = _load_remote_config()
    remote = f"{ssh_user}@{host}"

    for ext in (".json", ".xml", ".txt", ".log"):
        remote_file = f"{version_dir / (run_id + ext)}"
        local_file = version_dir / (run_id + ext)
        if local_file.exists() and local_file.stat().st_size > 0:
            continue  # already fetched
        try:
            result = subprocess.run(
                ["scp"] + SSH_OPTS + [f"{remote}:{remote_file}", str(local_file)],
                capture_output=True, text=True, timeout=30,
            )
            if result.returncode == 0 and local_file.exists() and local_file.stat().st_size > 0:
                fetched.append(ext)
            else:
                # Remote file may not exist (not all tools produce all extensions)
                local_file.unlink(missing_ok=True)
        except Exception as e:
            failed.append(f"{ext}: {e}")

    return {"fetched": fetched, "failed": failed}


def _build_sqc_cmd(codebase: str, cfg: dict, results_dir: Path, run_id: str) -> list[str]:
    path = str(cfg["path"])
    scan_path = cfg["sqc"].get("scan_path") or path
    output_file = results_dir / f"{run_id}.json"
    cmd = [
        str(SQC_BIN), scan_path,
        "--manifest", str(MANIFEST),
        "--export", str(output_file),
    ]
    extra = _expand(cfg["sqc"].get("extra_args", []), path)
    # If extra_args already contain -d flags, use those; otherwise add default -d {path}
    if "-d" not in extra:
        cmd.extend(["-d", path])
    cmd.extend(extra)
    return cmd


def _build_cppcheck_cmd(codebase: str, cfg: dict, results_dir: Path, run_id: str) -> list[str]:
    path = str(cfg["path"])
    cmd = [
        "cppcheck",
        "--enable=all",
        "--std=c11",
        "--xml", "--xml-version=2",
        "--suppress=missingIncludeSystem",
    ]
    cmd.extend(_expand(cfg["cppcheck"].get("includes", []), path))
    cmd.extend(_expand(cfg["cppcheck"].get("extra_args", []), path))
    cmd.extend(_expand(cfg["cppcheck"].get("source_dirs", []), path))
    return cmd


def _build_clang_tidy_cmd(codebase: str, cfg: dict, results_dir: Path, run_id: str) -> list[str]:
    """Build a shell command string for clang-tidy (uses find|xargs pipeline)."""
    path = str(cfg["path"])
    source_dirs = _expand(cfg["clang-tidy"].get("source_dirs", []), path)
    includes = _expand(cfg["clang-tidy"].get("includes", []), path)

    # Build find command for all source dirs
    find_parts = []
    for sd in source_dirs:
        find_parts.append(f"find {sd} -name '*.c'")
    find_cmd = " && ".join(find_parts) if len(find_parts) > 1 else find_parts[0]
    if len(find_parts) > 1:
        find_cmd = "( " + " ; ".join(find_parts) + " )"

    includes_str = " ".join(includes)
    return [
        "bash", "-c",
        f"{find_cmd} | xargs -P $(nproc) -I{{}} clang-tidy "
        f"-checks='-*,cert-*,clang-analyzer-*' {{}} "
        f"-- -std=c11 {includes_str}"
    ]


def _check_tool_available(tool: str) -> bool:
    """Check if a tool binary is available."""
    if tool == "sqc":
        return SQC_BIN.exists()
    try:
        subprocess.run(
            [tool if tool != "clang-tidy" else "clang-tidy", "--version"],
            capture_output=True, timeout=5,
        )
        return True
    except (FileNotFoundError, subprocess.TimeoutExpired):
        return False


# ── Result parsers ────────────────────────────────────────────────────────────

def _parse_sqc_json(filepath: Path) -> dict:
    """Parse sqc JSON export → violation count + per-rule breakdown."""
    try:
        violations = json.loads(filepath.read_text())
    except Exception as e:
        return {"error": str(e), "total": 0, "per_rule": {}}

    per_rule: dict[str, int] = {}
    for v in violations:
        rule = v.get("rule_id", "unknown")
        per_rule[rule] = per_rule.get(rule, 0) + 1

    return {
        "total": len(violations),
        "per_rule": dict(sorted(per_rule.items(), key=lambda x: -x[1])),
    }


def _parse_cppcheck_xml(filepath: Path) -> dict:
    """Parse cppcheck XML output → error count + per-id breakdown."""
    try:
        tree = ET.parse(filepath)
        root = tree.getroot()
    except Exception as e:
        return {"error": str(e), "total": 0, "per_rule": {}}

    per_rule: dict[str, int] = {}
    total = 0
    for error in root.iter("error"):
        error_id = error.get("id", "unknown")
        severity = error.get("severity", "")
        # Skip informational messages (missingInclude, etc.)
        if severity == "information":
            continue
        per_rule[error_id] = per_rule.get(error_id, 0) + 1
        total += 1

    return {
        "total": total,
        "per_rule": dict(sorted(per_rule.items(), key=lambda x: -x[1])),
    }


def _parse_clang_tidy_txt(filepath: Path) -> dict:
    """Parse clang-tidy text output → warning count + per-check breakdown."""
    try:
        content = filepath.read_text()
    except Exception as e:
        return {"error": str(e), "total": 0, "per_rule": {}}

    per_rule: dict[str, int] = {}
    total = 0
    # Match lines like: file.c:10:5: warning: ... [cert-err33-c]
    pattern = re.compile(r": warning: .+\[([^\]]+)\]$")
    for line in content.splitlines():
        m = pattern.search(line)
        if m:
            check_name = m.group(1)
            per_rule[check_name] = per_rule.get(check_name, 0) + 1
            total += 1

    return {
        "total": total,
        "per_rule": dict(sorted(per_rule.items(), key=lambda x: -x[1])),
    }


def _parse_result_file(run_id: str, filepath: Path) -> dict:
    """Dispatch to the right parser based on file extension."""
    if filepath.suffix == ".json":
        return _parse_sqc_json(filepath)
    elif filepath.suffix == ".xml":
        return _parse_cppcheck_xml(filepath)
    elif filepath.suffix == ".txt":
        return _parse_clang_tidy_txt(filepath)
    return {"error": f"Unknown file type: {filepath.suffix}", "total": 0, "per_rule": {}}


def _parse_sqc_log_progress(version_dir: Path, run_id: str) -> dict | None:
    """Parse tail of sqc log for file progress (Scanning: [file N/M])."""
    log_file = version_dir / f"{run_id}.log"
    if not log_file.exists():
        return None
    try:
        size = log_file.stat().st_size
        with open(log_file, "rb") as f:
            # Read last 4KB to find the most recent progress line
            f.seek(max(0, size - 4096))
            tail = f.read().decode("utf-8", errors="replace")
        matches = re.findall(r"Scanning: \[file (\d+)/(\d+)\]", tail)
        if matches:
            current, total = matches[-1]
            return {"current_file": int(current), "total_files": int(total)}
    except Exception:
        pass
    return None


def _get_result_file(results_dir: Path, run_id: str) -> Path | None:
    """Find the result file for a run (json, xml, or txt)."""
    for ext in (".json", ".xml", ".txt"):
        p = results_dir / f"{run_id}{ext}"
        if p.exists():
            return p
    return None


def _get_version_dir(identifier: str | None = None) -> Path | None:
    """Find a results directory by name, commit SHA, or 'latest'.

    Args:
        identifier: One of:
          - Full dir name (e.g. "sqc-0.2.4-abc1234")
          - Commit SHA (e.g. "abc1234") — matches sqc dirs
          - "latest" or None — most recently modified dir
    """
    if not RESULTS_BASE.exists():
        return None

    if identifier and identifier != "latest":
        # Try exact dir name match
        p = RESULTS_BASE / identifier
        if p.exists():
            return p
        # Try as commit SHA — search sqc dirs
        for d in RESULTS_BASE.iterdir():
            if d.is_dir() and d.name.endswith(f"-{identifier}"):
                return d
        # Try legacy format (version-sha without tool prefix)
        for d in RESULTS_BASE.iterdir():
            if d.is_dir() and identifier in d.name:
                return d
        return None

    # Find latest by modification time
    dirs = sorted(
        (d for d in RESULTS_BASE.iterdir() if d.is_dir()),
        key=lambda d: d.stat().st_mtime,
        reverse=True,
    )
    return dirs[0] if dirs else None


def _parse_version_dir_name(name: str) -> tuple[str, str, str]:
    """Parse dir name into (tool, version, sha).

    Formats:
      sqc-{version}-{sha}     → ("sqc", version, sha)
      cppcheck-{version}      → ("cppcheck", version, "")
      clang-tidy-{version}    → ("clang-tidy", version, "")
      {version}-{sha}         → ("sqc", version, sha)  [legacy]
    """
    if name.startswith("clang-tidy-"):
        return ("clang-tidy", name[len("clang-tidy-"):], "")
    if name.startswith("cppcheck-"):
        return ("cppcheck", name[len("cppcheck-"):], "")
    if name.startswith("sqc-"):
        rest = name[len("sqc-"):]
        parts = rest.rsplit("-", 1)
        version = parts[0] if parts else "unknown"
        sha = parts[1] if len(parts) > 1 else "unknown"
        return ("sqc", version, sha)
    # Legacy format: {version}-{sha}
    parts = name.rsplit("-", 1)
    return ("sqc", parts[0], parts[1] if len(parts) > 1 else "unknown")


def _list_version_dirs() -> list[dict]:
    """List all version directories with metadata."""
    runs = []
    if not RESULTS_BASE.exists():
        return runs

    for entry in sorted(RESULTS_BASE.iterdir()):
        if not entry.is_dir():
            continue
        tool, version, sha = _parse_version_dir_name(entry.name)

        result_files = [
            f for f in entry.iterdir()
            if f.is_file() and f.suffix in (".json", ".xml", ".txt") and f.stem != "benchmark"
        ]

        try:
            mtime = entry.stat().st_mtime
        except Exception:
            mtime = 0

        runs.append({
            "dir_name": entry.name,
            "path": str(entry),
            "tool": tool,
            "version": version,
            "commit_sha": sha,
            "result_files": len(result_files),
            "size": _dir_size_human(entry),
            "modified": time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(mtime)),
        })

    runs.sort(key=lambda r: r["modified"], reverse=True)
    return runs


# ── Tools ─────────────────────────────────────────────────────────────────────

@mcp.tool()
def run_analysis(tool: str, codebase: str, host: str | None = None) -> str:
    """
    Start a single analysis run (one tool against one codebase).

    Args:
        tool: One of "sqc", "cppcheck", "clang-tidy"
        codebase: One of "libcrc", "sqlite", "mosquitto", "curl", "hostap"
        host: Optional remote host IP or nickname (e.g. "10.0.0.97", "workstation-97").
              If omitted, runs locally.

    Returns immediately. Use get_status() to monitor progress.
    """
    tool = tool.strip().lower()
    codebase = codebase.strip().lower()

    if tool not in VALID_TOOLS:
        return json.dumps({
            "error": f"Unknown tool '{tool}'. Must be one of: {', '.join(sorted(VALID_TOOLS))}",
        })

    if codebase not in CODEBASES:
        return json.dumps({
            "error": f"Unknown codebase '{codebase}'. Must be one of: {', '.join(sorted(CODEBASES))}",
        })

    # Resolve host
    resolved = _resolve_host(host)
    if isinstance(resolved, dict):
        return json.dumps(resolved)
    remote = not _is_local(host)

    cfg = CODEBASES[codebase]

    # Local-only checks (can't verify paths/tools on remote)
    if not remote:
        if not cfg["path"].exists():
            return json.dumps({
                "error": f"Codebase path does not exist: {cfg['path']}",
                "hint": "Clone the project first. See COMPARISON_SETUP.md section 8.",
            })

        if not _check_tool_available(tool):
            return json.dumps({
                "error": f"Tool '{tool}' not found on PATH.",
                "hint": "Install it first. See COMPARISON_SETUP.md sections 1-2.",
            })

    version = _get_tool_version(tool)
    sha = _get_git_sha()
    dir_name = _make_version_dir_name(tool, version, sha)
    version_dir = RESULTS_BASE / dir_name
    version_dir.mkdir(parents=True, exist_ok=True)

    run_id = _make_run_id(tool, codebase, version, sha)

    # Check if already running (locked read)
    with _StateLock() as state:
        if run_id in state["runs"]:
            existing = state["runs"][run_id]
            if _process_alive(existing.get("pid", 0)):
                elapsed = int(time.time() - existing["start_time"])
                return json.dumps({
                    "status": "already_running",
                    "run_id": run_id,
                    "pid": existing["pid"],
                    "elapsed_seconds": elapsed,
                    "message": f"Run '{run_id}' already in progress (PID {existing['pid']}). Use get_status().",
                })

    # Clean up stale local result files from prior runs with same run_id
    for ext in (".json", ".xml", ".txt", ".log", ".ssh.log", ".done"):
        stale = version_dir / f"{run_id}{ext}"
        stale.unlink(missing_ok=True)

    # Build command
    if tool == "sqc":
        cmd = _build_sqc_cmd(codebase, cfg, version_dir, run_id)
    elif tool == "cppcheck":
        cmd = _build_cppcheck_cmd(codebase, cfg, version_dir, run_id)
    else:  # clang-tidy
        cmd = _build_clang_tidy_cmd(codebase, cfg, version_dir, run_id)

    if remote:
        # Build SSH-wrapped command
        _, ssh_user = _load_remote_config()
        shell_cmd = _build_remote_shell_cmd(tool, cmd, version_dir, run_id)
        ssh_cmd = ["ssh"] + SSH_OPTS + [f"{ssh_user}@{resolved}", shell_cmd]

        # Log SSH command for debugging
        # NOTE: file handle intentionally left open — subprocess writes to it.
        # It will be closed when the subprocess exits and the fd is GC'd.
        ssh_log_path = version_dir / f"{run_id}.ssh.log"
        ssh_log_fh = ssh_log_path.open("w")
        ssh_log_fh.write(f"# SSH command: {' '.join(ssh_cmd)}\n")
        ssh_log_fh.flush()

        proc = subprocess.Popen(
            ssh_cmd,
            stdout=ssh_log_fh,
            stderr=subprocess.STDOUT,
            start_new_session=True,
        )
    else:
        # Local execution (unchanged)
        log_path = version_dir / f"{run_id}.log"
        log_fh = log_path.open("w")

        if tool == "cppcheck":
            result_path = version_dir / f"{run_id}.xml"
            result_fh = result_path.open("w")
            proc = subprocess.Popen(
                cmd,
                stdout=log_fh,
                stderr=result_fh,
                start_new_session=True,
            )
            result_fh.close()
        elif tool == "clang-tidy":
            result_path = version_dir / f"{run_id}.txt"
            result_fh = result_path.open("w")
            proc = subprocess.Popen(
                cmd,
                stdout=result_fh,
                stderr=log_fh,
                start_new_session=True,
            )
            result_fh.close()
        else:
            proc = subprocess.Popen(
                cmd,
                stdout=log_fh,
                stderr=subprocess.STDOUT,
                start_new_session=True,
            )
        log_fh.close()

    # Record new run in state (locked read-modify-write)
    start_time = time.time()
    with _StateLock() as state:
        state["runs"][run_id] = {
            "pid": proc.pid,
            "start_time": start_time,
            "tool": tool,
            "codebase": codebase,
            "version": version,
            "commit_sha": sha,
            "version_dir": str(version_dir),
            "status": "running",
            "host": resolved if remote else "local",
            "results_fetched": not remote,  # local results are always "fetched"
        }

    hosts, _ = _load_remote_config()
    host_label = f" on {hosts.get(resolved, resolved)}" if remote else ""
    return json.dumps({
        "status": "started",
        "run_id": run_id,
        "pid": proc.pid,
        "tool": tool,
        "codebase": codebase,
        "host": resolved if remote else "local",
        "version_dir": str(version_dir),
        "message": f"Started {tool} on {codebase}{host_label} (PID {proc.pid}). Use get_status() to monitor.",
    })


@mcp.tool()
def run_all(codebase: str | None = None, tool: str | None = None,
            host: str | None = None) -> str:
    """
    Run all tool×codebase combinations (or filter by codebase and/or tool).

    Args:
        codebase: Optional filter — only run against this codebase
        tool: Optional filter — only run this tool
        host: Optional remote host IP or nickname. If omitted, runs locally.

    Launches each combo as a separate subprocess. Returns summary of what was started.
    """
    tools = [tool.strip().lower()] if tool else sorted(VALID_TOOLS)
    codebases = [codebase.strip().lower()] if codebase else sorted(CODEBASES)

    # Validate
    for t in tools:
        if t not in VALID_TOOLS:
            return json.dumps({"error": f"Unknown tool '{t}'. Must be one of: {', '.join(sorted(VALID_TOOLS))}"})
    for cb in codebases:
        if cb not in CODEBASES:
            return json.dumps({"error": f"Unknown codebase '{cb}'. Must be one of: {', '.join(sorted(CODEBASES))}"})

    results = []
    for t in tools:
        for cb in codebases:
            try:
                result = json.loads(run_analysis(t, cb, host=host))
            except Exception as e:
                result = {"status": "error", "error": str(e)}
            results.append({
                "tool": t,
                "codebase": cb,
                "status": result.get("status", "error"),
                "host": result.get("host", "local"),
                "message": result.get("message") or result.get("error", ""),
            })

    started = sum(1 for r in results if r["status"] == "started")
    already = sum(1 for r in results if r["status"] == "already_running")
    errors = sum(1 for r in results if r["status"] not in ("started", "already_running"))

    return json.dumps({
        "started": started,
        "already_running": already,
        "errors": errors,
        "total": len(results),
        "runs": results,
        "message": f"Launched {started} runs ({already} already running, {errors} errors).",
    })


@mcp.tool()
def get_status() -> str:
    """
    Show status of all tracked runs (active, completed, failed).

    Returns per-run status with timing, plus overall summary.
    """
    with _StateLock() as state:
        if not state["runs"]:
            return json.dumps({
                "status": "no_runs",
                "message": "No runs tracked. Use run_analysis() or run_all() to start.",
            })

        now = time.time()
        statuses = []
        active_count = 0
        completed_count = 0
        failed_count = 0

        for run_id, run_info in sorted(state["runs"].items()):
            pid = run_info.get("pid", 0)
            is_alive = _process_alive(pid)
            elapsed_s = int(now - run_info["start_time"])
            run_host = run_info.get("host", "local")
            is_remote = run_host != "local"

            # Determine status
            if run_info.get("status") == "cancelled":
                status = "cancelled"
                failed_count += 1
            elif is_alive:
                status = "running"
                active_count += 1
            elif is_remote and not run_info.get("results_fetched"):
                # Remote run: SSH process exited but remote job may still be running.
                # Check for .done sentinel on the remote host.
                version_dir = Path(run_info.get("version_dir", ""))
                if _remote_check_done(run_host, version_dir, run_id):
                    fetch_result = _fetch_remote_results(run_host, version_dir, run_id)
                    run_info["results_fetched"] = True
                    run_info["fetch_info"] = fetch_result
                    # Now check local result files
                    result_file = _get_result_file(version_dir, run_id)
                    if result_file and result_file.stat().st_size > 0:
                        status = "completed"
                        completed_count += 1
                        run_info["status"] = "completed"
                        run_info["end_time"] = now
                    else:
                        log_file = version_dir / f"{run_id}.log"
                        if log_file.exists() and log_file.stat().st_size > 0:
                            status = "completed"
                            completed_count += 1
                            run_info["status"] = "completed"
                        else:
                            status = "failed"
                            failed_count += 1
                elif elapsed_s > 14400:  # 4 hours
                    # Local SSH died, no sentinel, very old → zombie
                    status = "zombie"
                    failed_count += 1
                else:
                    # Sentinel not found — remote still running
                    status = "running"
                    active_count += 1
            else:
                # Local run (or already-fetched remote): check result files
                version_dir = Path(run_info.get("version_dir", ""))
                result_file = _get_result_file(version_dir, run_id)
                if result_file and result_file.stat().st_size > 0:
                    status = "completed"
                    completed_count += 1
                    if run_info.get("status") != "completed":
                        run_info["status"] = "completed"
                        run_info["end_time"] = now
                else:
                    log_file = version_dir / f"{run_id}.log"
                    if log_file.exists() and log_file.stat().st_size > 0:
                        status = "completed"
                        completed_count += 1
                        if run_info.get("status") != "completed":
                            run_info["status"] = "completed"
                    else:
                        status = "failed"
                        failed_count += 1

            entry = {
                "run_id": run_id,
                "tool": run_info.get("tool"),
                "codebase": run_info.get("codebase"),
                "host": run_host,
                "status": status,
                "pid": pid,
                "elapsed_seconds": elapsed_s,
                "elapsed_human": _fmt_duration(elapsed_s),
            }

            # Add file-level progress for running sqc runs
            if status == "running" and run_info.get("tool") == "sqc":
                version_dir = Path(run_info.get("version_dir", ""))
                file_progress = _parse_sqc_log_progress(version_dir, run_id)
                if file_progress:
                    entry["file_progress"] = file_progress

            statuses.append(entry)

    return json.dumps({
        "active": active_count,
        "completed": completed_count,
        "failed": failed_count,
        "total": len(statuses),
        "runs": statuses,
        "message": (
            f"{active_count} running, {completed_count} completed, {failed_count} failed "
            f"out of {len(statuses)} tracked runs."
        ),
    })


@mcp.tool()
def get_results(run_id: str | None = None) -> str:
    """
    Parse and display results for a specific run or all runs in the latest version dir.

    Args:
        run_id: Optional run ID (e.g. "sqc-libcrc-0.2.4-abc1234"). If omitted,
                shows results for all runs in the latest version directory.

    For sqc: parses JSON export (violation count, per-rule breakdown).
    For cppcheck: parses XML (error counts by id).
    For clang-tidy: parses text output (warning counts by check name).
    """
    if run_id:
        # Find specific run
        state = _read_state()
        run_info = state["runs"].get(run_id)
        if not run_info:
            return json.dumps({
                "error": f"Run '{run_id}' not found in state.",
                "available": sorted(state["runs"].keys()),
            })
        version_dir = Path(run_info["version_dir"])
        result_file = _get_result_file(version_dir, run_id)
        if not result_file:
            return json.dumps({
                "error": f"No result file found for '{run_id}'.",
                "version_dir": str(version_dir),
                "hint": "The run may still be in progress. Use get_status().",
            })
        parsed = _parse_result_file(run_id, result_file)
        return json.dumps({
            "run_id": run_id,
            "tool": run_info.get("tool"),
            "codebase": run_info.get("codebase"),
            "version": run_info.get("version"),
            "commit_sha": run_info.get("commit_sha"),
            "result_file": str(result_file),
            "total_violations": parsed["total"],
            "per_rule_top20": dict(list(parsed["per_rule"].items())[:20]),
            "total_rules": len(parsed["per_rule"]),
        })

    # Show all results from latest version dir
    version_dir = _get_version_dir()
    if not version_dir:
        return json.dumps({
            "error": "No results found. Run run_analysis() or run_all() first.",
        })

    all_results = []
    for f in sorted(version_dir.iterdir()):
        if f.is_file() and f.suffix in (".json", ".xml", ".txt") and not f.stem.endswith(".log"):
            # Skip log files
            if f.suffix == ".txt" and f.stem.endswith(".log"):
                continue
            run_name = f.stem
            parsed = _parse_result_file(run_name, f)
            # Extract tool and codebase from run_id pattern: {tool}-{codebase}-{version}-{sha}
            parts = run_name.split("-")
            tool_name = parts[0] if parts else "unknown"
            codebase_name = parts[1] if len(parts) > 1 else "unknown"
            # Handle clang-tidy (has hyphen in name)
            if tool_name == "clang" and len(parts) > 1 and parts[1] == "tidy":
                tool_name = "clang-tidy"
                codebase_name = parts[2] if len(parts) > 2 else "unknown"

            all_results.append({
                "run_id": run_name,
                "tool": tool_name,
                "codebase": codebase_name,
                "total_violations": parsed["total"],
                "top_rules": dict(list(parsed["per_rule"].items())[:10]),
                "total_rules": len(parsed["per_rule"]),
            })

    return json.dumps({
        "version_dir": str(version_dir),
        "dir_name": version_dir.name,
        "runs": all_results,
        "total_runs": len(all_results),
    })


@mcp.tool()
def compare_runs(base_version: str, target_version: str,
                 codebase: str | None = None, tool: str | None = None) -> str:
    """
    Compare results between two version directories.

    Args:
        base_version: Base (older) version dir name, commit SHA, or "latest"
        target_version: Target (newer) version dir name, commit SHA, or "latest"
        codebase: Optional filter — only compare this codebase
        tool: Optional filter — only compare this tool

    Returns violation count deltas per tool per codebase.
    Positive delta = regression (more violations), negative = improvement.
    """
    base_dir = _resolve_version_dir(base_version)
    target_dir = _resolve_version_dir(target_version)

    if not base_dir:
        return json.dumps({
            "error": f"Could not resolve base version '{base_version}'.",
            "available": [d["dir_name"] for d in _list_version_dirs()],
        })
    if not target_dir:
        return json.dumps({
            "error": f"Could not resolve target version '{target_version}'.",
            "available": [d["dir_name"] for d in _list_version_dirs()],
        })
    if base_dir == target_dir:
        return json.dumps({
            "error": "Base and target resolve to the same directory.",
            "resolved": str(base_dir),
        })

    base_results = _load_version_results(base_dir)
    target_results = _load_version_results(target_dir)

    # Filter
    if tool:
        tool = tool.strip().lower()
    if codebase:
        codebase = codebase.strip().lower()

    comparisons = []
    all_keys = set(base_results) | set(target_results)
    for key in sorted(all_keys):
        # key is (tool, codebase)
        k_tool, k_codebase = key
        if tool and k_tool != tool:
            continue
        if codebase and k_codebase != codebase:
            continue

        b = base_results.get(key, {"total": 0, "per_rule": {}})
        t = target_results.get(key, {"total": 0, "per_rule": {}})

        # Per-rule deltas
        all_rules = set(b["per_rule"]) | set(t["per_rule"])
        rule_deltas = []
        for r in sorted(all_rules):
            b_count = b["per_rule"].get(r, 0)
            t_count = t["per_rule"].get(r, 0)
            if b_count != t_count:
                rule_deltas.append({
                    "rule": r,
                    "base": b_count,
                    "target": t_count,
                    "delta": t_count - b_count,
                })
        rule_deltas.sort(key=lambda x: abs(x["delta"]), reverse=True)

        comparisons.append({
            "tool": k_tool,
            "codebase": k_codebase,
            "base_total": b["total"],
            "target_total": t["total"],
            "delta": t["total"] - b["total"],
            "top_rule_changes": rule_deltas[:10],
        })

    return json.dumps({
        "base_dir": base_dir.name,
        "target_dir": target_dir.name,
        "comparisons": comparisons,
        "total_compared": len(comparisons),
    })


@mcp.tool()
def list_runs() -> str:
    """
    List all version directories and their result files.

    Shows version, commit SHA, number of result files, size,
    and modification date for each version directory.
    """
    dirs = _list_version_dirs()
    if not dirs:
        return json.dumps({
            "runs": [],
            "message": f"No result directories found under {RESULTS_BASE}.",
        })

    return json.dumps({
        "runs": dirs,
        "count": len(dirs),
        "message": f"{len(dirs)} version directory(ies) found.",
    })


@mcp.tool()
def cancel_run(run_id: str | None = None) -> str:
    """
    Cancel a specific run or all active runs.

    Args:
        run_id: Run ID to cancel. If omitted, cancels ALL active runs.
    """
    with _StateLock() as state:
        if not state["runs"]:
            return json.dumps({
                "status": "no_runs",
                "message": "No runs tracked.",
            })

        cancelled = []
        not_running = []

        targets = {}
        if run_id:
            if run_id not in state["runs"]:
                return json.dumps({
                    "error": f"Run '{run_id}' not found.",
                    "available": sorted(state["runs"].keys()),
                })
            targets = {run_id: state["runs"][run_id]}
        else:
            targets = dict(state["runs"])

        for rid, info in targets.items():
            pid = info.get("pid", 0)
            if _process_alive(pid):
                _kill_process_group(pid)
                info["status"] = "cancelled"
                cancelled.append(rid)
            else:
                not_running.append(rid)

    return json.dumps({
        "cancelled": cancelled,
        "not_running": not_running,
        "message": f"Cancelled {len(cancelled)} run(s). {len(not_running)} were not running.",
    })


@mcp.tool()
def purge_run(run_id: str | None = None, zombies: bool = False) -> str:
    """
    Remove stale/zombie runs from tracking state (does not delete result files).

    Use this to clean up runs that are stuck as "running" but whose processes
    are actually dead (zombies). Does NOT remove result files on disk.
    Completed and cancelled runs are never purged by zombies=True — only
    runs that never reached a terminal status are candidates.

    Args:
        run_id: Specific run ID to purge. If omitted, requires zombies=True.
        zombies: If True, purge all zombie runs (dead process, not completed/cancelled,
                 elapsed > 4 hours). Ignored if run_id is specified.
    """
    zombie_threshold = 14400  # 4 hours

    with _StateLock() as state:
        if not state["runs"]:
            return json.dumps({"status": "no_runs", "message": "No runs tracked."})

        purged = []

        if run_id:
            if run_id not in state["runs"]:
                return json.dumps({
                    "error": f"Run '{run_id}' not found.",
                    "available": sorted(state["runs"].keys()),
                })
            info = state["runs"].pop(run_id)
            purged.append({
                "run_id": run_id,
                "host": info.get("host", "local"),
                "elapsed": _fmt_duration(int(time.time() - info["start_time"])),
            })
        elif zombies:
            now = time.time()
            to_purge = []
            for rid, info in state["runs"].items():
                pid = info.get("pid", 0)
                elapsed_s = int(now - info["start_time"])
                is_alive = _process_alive(pid)
                is_remote = info.get("host", "local") != "local"
                already_done = info.get("status") in ("completed", "cancelled")

                # Skip completed/cancelled runs — they're not zombies
                if already_done:
                    continue

                if not is_alive and elapsed_s > zombie_threshold:
                    if is_remote and not info.get("results_fetched"):
                        # Remote zombie: SSH dead, no results fetched, old
                        to_purge.append(rid)
                    elif not is_remote:
                        # Local zombie: process dead, old
                        to_purge.append(rid)

            for rid in to_purge:
                info = state["runs"].pop(rid)
                purged.append({
                    "run_id": rid,
                    "host": info.get("host", "local"),
                    "elapsed": _fmt_duration(int(time.time() - info["start_time"])),
                })
        else:
            return json.dumps({
                "error": "Specify run_id or set zombies=True to purge stale runs.",
            })

    return json.dumps({
        "purged": purged,
        "count": len(purged),
        "message": f"Purged {len(purged)} run(s) from tracking state.",
    })


@mcp.tool()
def clear_results() -> str:
    """
    Remove old result directories (protects directories with active runs).
    """
    if not RESULTS_BASE.exists():
        return json.dumps({
            "status": "nothing_to_clear",
            "message": f"{RESULTS_BASE} does not exist.",
        })

    # Find active version dirs
    with _StateLock() as state:
        active_dirs = set()
        for info in state["runs"].values():
            if _process_alive(info.get("pid", 0)):
                active_dirs.add(info.get("version_dir"))

        removed = []
        skipped = []
        errors = []

        for entry in sorted(RESULTS_BASE.iterdir()):
            if not entry.is_dir():
                continue
            if str(entry) in active_dirs:
                skipped.append(entry.name)
                continue
            try:
                size = _dir_size_human(entry)
                shutil.rmtree(entry)
                removed.append({"name": entry.name, "size": size})
            except Exception as e:
                errors.append(f"Failed to remove {entry.name}: {e}")

        # Clean up state entries for removed dirs
        to_remove = []
        for rid, info in state["runs"].items():
            vdir = info.get("version_dir", "")
            if not Path(vdir).exists() and not _process_alive(info.get("pid", 0)):
                to_remove.append(rid)
        for rid in to_remove:
            del state["runs"][rid]

    return json.dumps({
        "removed": removed,
        "skipped_active": skipped,
        "state_entries_cleaned": len(to_remove),
        "errors": errors,
        "message": f"Removed {len(removed)} directories, skipped {len(skipped)} active.",
    })


@mcp.tool()
def deploy_sqc(host: str | None = None) -> str:
    """
    Deploy sqc binary + manifest TOML to remote host(s).

    Args:
        host: Remote host IP or nickname. If omitted, deploys to ALL remote hosts.

    Copies target/release/sqc and rules_templates/rules-all.toml, then verifies
    by running sqc --version on the remote.
    """
    if not SQC_BIN.exists():
        return json.dumps({
            "error": f"sqc binary not found at {SQC_BIN}. Run 'cargo build --release' first.",
        })
    if not MANIFEST.exists():
        return json.dumps({
            "error": f"Manifest not found at {MANIFEST}.",
        })

    hosts, ssh_user = _load_remote_config()
    if not hosts:
        return json.dumps({
            "error": "Remote execution not configured. Create mcp/remote_hosts.json.",
        })

    targets: list[str] = []
    if host:
        resolved = _resolve_host(host)
        if isinstance(resolved, dict):
            return json.dumps(resolved)
        if resolved == "local":
            return json.dumps({"error": "deploy_sqc is for remote hosts only."})
        targets = [resolved]
    else:
        targets = list(hosts.keys())

    results = []
    for ip in targets:
        name = hosts.get(ip, ip)
        remote = f"{ssh_user}@{ip}"
        entry = {"host": ip, "name": name, "status": "unknown", "details": []}

        # scp binary
        try:
            r = subprocess.run(
                ["scp"] + SSH_OPTS + [str(SQC_BIN), f"{remote}:{SQC_BIN}"],
                capture_output=True, text=True, timeout=60,
            )
            if r.returncode == 0:
                entry["details"].append("binary: copied")
            else:
                entry["details"].append(f"binary: FAILED ({r.stderr.strip()})")
                entry["status"] = "failed"
                results.append(entry)
                continue
        except Exception as e:
            entry["details"].append(f"binary: FAILED ({e})")
            entry["status"] = "failed"
            results.append(entry)
            continue

        # scp manifest
        try:
            r = subprocess.run(
                ["scp"] + SSH_OPTS + [str(MANIFEST), f"{remote}:{MANIFEST}"],
                capture_output=True, text=True, timeout=30,
            )
            if r.returncode == 0:
                entry["details"].append("manifest: copied")
            else:
                entry["details"].append(f"manifest: FAILED ({r.stderr.strip()})")
        except Exception as e:
            entry["details"].append(f"manifest: FAILED ({e})")

        # Verify
        try:
            r = subprocess.run(
                ["ssh"] + SSH_OPTS + [remote, str(SQC_BIN), "--version"],
                capture_output=True, text=True, timeout=15,
            )
            if r.returncode == 0:
                version_str = r.stdout.strip()
                entry["details"].append(f"verify: {version_str}")
                entry["status"] = "ok"
            else:
                entry["details"].append(f"verify: FAILED ({r.stderr.strip()})")
                entry["status"] = "partial"
        except Exception as e:
            entry["details"].append(f"verify: FAILED ({e})")
            entry["status"] = "partial"

        results.append(entry)

    ok = sum(1 for r in results if r["status"] == "ok")
    return json.dumps({
        "deployed": ok,
        "total": len(results),
        "hosts": results,
        "message": f"Deployed to {ok}/{len(results)} hosts.",
    })


# ── Internal helpers for compare ──────────────────────────────────────────────

def _resolve_version_dir(identifier: str) -> Path | None:
    """Resolve a version dir identifier to a path."""
    ident = identifier.strip()

    if ident.lower() in ("latest", "current"):
        return _get_version_dir()

    if ident.startswith("/"):
        p = Path(ident)
        return p if p.exists() else None

    # Direct dir name match
    p = RESULTS_BASE / ident
    if p.exists():
        return p

    # SHA suffix match
    if RESULTS_BASE.exists():
        for entry in sorted(RESULTS_BASE.iterdir(), reverse=True):
            if entry.is_dir() and entry.name.endswith(f"-{ident}"):
                return entry

    # Substring match
    if RESULTS_BASE.exists():
        for entry in sorted(RESULTS_BASE.iterdir(), reverse=True):
            if entry.is_dir() and ident in entry.name:
                return entry

    return None


def _load_version_results(version_dir: Path) -> dict[tuple[str, str], dict]:
    """Load all results from a version dir. Returns {(tool, codebase): parsed_data}."""
    results = {}
    for f in sorted(version_dir.iterdir()):
        if not f.is_file():
            continue
        if f.suffix not in (".json", ".xml", ".txt"):
            continue
        if f.stem.endswith(".log") or f.suffix == ".log":
            continue

        run_name = f.stem
        parsed = _parse_result_file(run_name, f)

        # Extract tool and codebase from run_id: {tool}-{codebase}-{version}-{sha}
        tool_name, codebase_name = _parse_run_id(run_name)
        results[(tool_name, codebase_name)] = parsed

    return results


def _parse_run_id(run_id: str) -> tuple[str, str]:
    """Extract (tool, codebase) from a run_id string."""
    # Handle clang-tidy which has a hyphen
    if run_id.startswith("clang-tidy-"):
        rest = run_id[len("clang-tidy-"):]
        parts = rest.split("-")
        codebase = parts[0] if parts else "unknown"
        return ("clang-tidy", codebase)

    parts = run_id.split("-")
    tool = parts[0] if parts else "unknown"
    codebase = parts[1] if len(parts) > 1 else "unknown"
    return (tool, codebase)


if __name__ == "__main__":
    mcp.run()

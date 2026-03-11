#!/bin/bash
# Benchmark mosquitto v2.1.2 across sqc versions.
#
# Usage:
#   ./scripts/bench_mosquitto_versions.sh [BINARIES_DIR]
#
# Prerequisites:
#   - mosquitto v2.1.2 cloned at ~/data/mosquitto
#   - sqc binaries placed in BINARIES_DIR (default: /tmp/sqc_binaries/)
#     Named: sqc-0.2.16, sqc-0.2.21, sqc-0.2.25, sqc-0.3.5, sqc-0.3.12, sqc-0.3.13
#   - v0.2.4 and v0.2.7 are built from source (see build_old_versions below)
#
# Downloads from Azure Artifacts:
#   for v in 0.2.16 0.2.21 0.2.25 0.3.5 0.3.12 0.3.13; do
#     mkdir -p /tmp/sqc_dl && cd /tmp/sqc_dl
#     az artifacts universal download \
#       --organization "https://dev.azure.com/bissell/" \
#       --project "700f4857-3fc0-4f5f-9f44-2f59026f9354" \
#       --scope project --feed "ELEC_SW_Rust_Packages" \
#       --name "sqc" --version "$v" --path .
#     cp sqc /tmp/sqc_binaries/sqc-$v
#     chmod +x /tmp/sqc_binaries/sqc-$v
#   done
#
# Build v0.2.4 and v0.2.7 from source:
#   git stash  # save current work
#   git checkout ac4215bb && cargo build --release && cp target/release/sqc /tmp/sqc_binaries/sqc-0.2.4
#   git checkout 54819432 && cargo build --release && cp target/release/sqc /tmp/sqc_binaries/sqc-0.2.7
#   git checkout -  # return to original branch
#   git stash pop

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BINARIES_DIR="${1:-/tmp/sqc_binaries}"
MOSQUITTO="${HOME}/data/mosquitto"
RESULTS_DIR="/tmp/mosquitto_bench_results"

VERSIONS=(0.2.4 0.2.7 0.2.16 0.2.21 0.2.25 0.3.5 0.3.12 0.3.13)

# Architecture notes for each version
declare -A NOTES
NOTES[0.2.4]="cross-file prescan + std_functions baseline"
NOTES[0.2.7]="+CFG null state dataflow"
NOTES[0.2.16]="+prescan Phase 2 (call-site null, double AST walk)"
NOTES[0.2.21]="+const_eval module (550 lines)"
NOTES[0.2.25]="+const_eval: built-in macros, sizeof, INT34-C"
NOTES[0.3.5]="+struct field resolution, v0.3.0 suppression redesign"
NOTES[0.3.12]="pre-perf-fix (INT33-C O(n), prescan double walk, const_eval alloc)"
NOTES[0.3.13]="perf fix: INT33-C cache, prescan merge, const_eval LazyLock"

# ── Preflight checks ────────────────────────────────────────────────────────

if [ ! -d "$MOSQUITTO" ]; then
    echo "ERROR: mosquitto not found at $MOSQUITTO"
    echo "Clone it: git clone --depth 1 --branch v2.1.2 https://github.com/eclipse-mosquitto/mosquitto.git $MOSQUITTO"
    exit 1
fi

echo "================================================================"
echo "SQC MOSQUITTO BENCHMARK"
echo "Target: mosquitto v2.1.2 at $MOSQUITTO"
echo "Binaries: $BINARIES_DIR"
echo "Results: $RESULTS_DIR"
echo "================================================================"
echo ""

mkdir -p "$RESULTS_DIR"

missing=0
for v in "${VERSIONS[@]}"; do
    bin="$BINARIES_DIR/sqc-$v"
    if [ ! -x "$bin" ]; then
        echo "MISSING: $bin"
        missing=1
    fi
done

if [ "$missing" -eq 1 ]; then
    echo ""
    echo "Place missing binaries in $BINARIES_DIR and re-run."
    echo "See script header for download/build instructions."
    exit 1
fi

file_count=$(find "$MOSQUITTO" -name "*.c" | wc -l)
echo "Mosquitto: $file_count C files"
echo ""

# ── Run benchmarks sequentially ─────────────────────────────────────────────

printf "%-10s  %10s  %10s  %12s  %s\n" "VERSION" "TIME (s)" "TIME (ms)" "VIOLATIONS" "NOTES"
printf "%-10s  %10s  %10s  %12s  %s\n" "-------" "--------" "---------" "----------" "-----"

for v in "${VERSIONS[@]}"; do
    bin="$BINARIES_DIR/sqc-$v"
    json="$RESULTS_DIR/mosquitto-$v.json"

    # Warm filesystem cache (discard output)
    "$bin" "$MOSQUITTO/src/mosquitto.c" -e /dev/null > /dev/null 2>&1 || true

    # Timed scan
    start_ns=$(date +%s%N)

    "$bin" "$MOSQUITTO" \
        -d "$MOSQUITTO" \
        -e "$json" \
        > /dev/null 2>&1 || true

    end_ns=$(date +%s%N)
    elapsed_ms=$(( (end_ns - start_ns) / 1000000 ))
    elapsed_s=$(( elapsed_ms / 1000 ))

    # Count violations
    if [ -f "$json" ]; then
        violations=$(python3 -c "import json; print(len(json.load(open('$json'))))" 2>/dev/null || echo "?")
    else
        # Fallback: try CSV count
        csv="$RESULTS_DIR/mosquitto-$v.csv"
        if [ -f "$csv" ]; then
            violations=$(( $(wc -l < "$csv") - 1 ))
        else
            violations="?"
        fi
    fi

    printf "v%-9s  %8ds  %8dms  %12s  %s\n" "$v" "$elapsed_s" "$elapsed_ms" "$violations" "${NOTES[$v]}"
done

echo ""
echo "================================================================"
echo "BENCHMARK COMPLETE"
echo "================================================================"

# ── Generate JSON summary ────────────────────────────────────────────────────

python3 << 'PYEOF'
import json, os, glob

results_dir = os.environ.get("RESULTS_DIR", "/tmp/mosquitto_bench_results")
versions = []

for f in sorted(glob.glob(f"{results_dir}/mosquitto-*.json")):
    basename = os.path.basename(f)
    version = basename.replace("mosquitto-", "").replace(".json", "")

    try:
        data = json.load(open(f))
        violations = len(data)

        import collections
        counts = collections.Counter(v["rule_id"] for v in data)
        severity = collections.Counter(v["severity"] for v in data)

        versions.append({
            "version": version,
            "violations": violations,
            "unique_rules": len(counts),
            "by_severity": dict(severity),
            "top_10_rules": [
                {"rule": r, "count": c}
                for r, c in counts.most_common(10)
            ],
        })
    except Exception as e:
        versions.append({"version": version, "error": str(e)})

summary = {
    "benchmark_target": "mosquitto v2.1.2",
    "versions": versions,
}

output = f"{results_dir}/summary.json"
with open(output, "w") as f:
    json.dump(summary, f, indent=2)

print(f"Summary written to: {output}")
PYEOF

echo ""
echo "Raw results in: $RESULTS_DIR/mosquitto-{version}.json"
echo "Summary in:     $RESULTS_DIR/summary.json"

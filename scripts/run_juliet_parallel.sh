#!/bin/bash
# Run SqC Juliet benchmark in parallel across CWE categories.
# Usage: ./scripts/run_juliet_parallel.sh [--fast] [JOBS]
#   --fast: Use per-CWE manifests (only CWE-matched rules). Much faster, less noise.
#   JOBS:   Parallelism level (default: 12)

set -uo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
SQC="$PROJECT_DIR/target/release/sqc"
MANIFEST_ALL="$PROJECT_DIR/rules_templates/rules-all.toml"
MANIFEST_CWE_DIR="$PROJECT_DIR/rules_templates/cwe"
JULIET_BASE="${HOME}/data/benchmarks/juliet-test-suite-c/testcases"
ANALYZE="$SCRIPT_DIR/analyze_juliet_results.py"
GENERATE_MAP="$SCRIPT_DIR/generate_rule_cwe_map.py"
RULE_CWE_MAP="$PROJECT_DIR/data/rule_cwe_map.json"
RESULTS_DIR="${RESULTS_DIR:-/tmp/juliet_results}"

# Parse arguments
FAST_MODE=0
JOBS=12
for arg in "$@"; do
    if [ "$arg" = "--fast" ]; then
        FAST_MODE=1
    elif [[ "$arg" =~ ^[0-9]+$ ]]; then
        JOBS="$arg"
    fi
done

mkdir -p "$RESULTS_DIR"

# Auto-regenerate rule-CWE map and per-CWE manifests
if [ -f "$GENERATE_MAP" ]; then
    python3 "$GENERATE_MAP" 2>/dev/null || true
fi

# Resolve manifest for a CWE directory name (e.g., CWE190_Integer_Overflow → CWE-190)
resolve_manifest() {
    local cwe_dirname="$1"
    if [ "$FAST_MODE" -eq 1 ]; then
        # Extract CWE number from directory name
        local cwe_num
        cwe_num=$(echo "$cwe_dirname" | grep -oP '^CWE\K\d+')
        if [ -n "$cwe_num" ]; then
            local manifest="$MANIFEST_CWE_DIR/CWE-${cwe_num}.toml"
            if [ -f "$manifest" ]; then
                echo "$manifest"
                return 0
            fi
        fi
        # No per-CWE manifest — skip in fast mode
        return 1
    fi
    echo "$MANIFEST_ALL"
    return 0
}

scan_cwe() {
    local cwe="$1"
    local cwe_dir="$JULIET_BASE/$cwe"
    local csv_file="$RESULTS_DIR/${cwe}.csv"
    local analysis_file="$RESULTS_DIR/${cwe}_analysis.txt"

    # Skip if already completed
    if [ -f "$analysis_file" ] && [ -s "$analysis_file" ]; then
        echo "SKIP (already done): $cwe"
        return 0
    fi

    if [ ! -d "$cwe_dir" ]; then
        echo "SKIP (not found): $cwe"
        return 0
    fi

    # Resolve manifest (fast mode may skip CWEs without mappings)
    local manifest
    manifest=$(resolve_manifest "$cwe")
    if [ $? -ne 0 ]; then
        echo "SKIP (no CWE manifest): $cwe"
        return 0
    fi

    local file_count
    file_count=$(find "$cwe_dir" -name "*.c" | wc -l)

    local mode_label=""
    if [ "$FAST_MODE" -eq 1 ]; then
        mode_label=" [fast]"
    fi
    echo "START${mode_label}: $cwe ($file_count files)"

    local start_time
    start_time=$(date +%s)
    if ! "$SQC" "$cwe_dir" -m "$manifest" -d "$JULIET_BASE" -d "${JULIET_BASE}/../testcasesupport" -e "$csv_file" >/dev/null 2>&1; then
        echo "ERROR: $cwe scan failed"
        return 1
    fi
    local end_time
    end_time=$(date +%s)
    local elapsed=$((end_time - start_time))

    local violation_count
    violation_count=$(tail -n +2 "$csv_file" 2>/dev/null | wc -l)

    # Run analysis (with CWE-aware metrics if map is available)
    local analyze_args=(--csv "$csv_file" --dir "$cwe_dir")
    if [ -f "$RULE_CWE_MAP" ]; then
        analyze_args+=(--rule-cwe-map "$RULE_CWE_MAP")
    fi
    python3 "$ANALYZE" "${analyze_args[@]}" > "$analysis_file" 2>&1

    echo "DONE${mode_label}: $cwe | ${elapsed}s | ${violation_count} violations | ${file_count} files"
}

export -f scan_cwe resolve_manifest
export SQC MANIFEST_ALL MANIFEST_CWE_DIR JULIET_BASE ANALYZE RULE_CWE_MAP RESULTS_DIR FAST_MODE

# Get all CWE directories
ALL_CWES=$(ls "$JULIET_BASE")

MODE_DESC="FULL (all rules)"
if [ "$FAST_MODE" -eq 1 ]; then
    MODE_DESC="FAST (CWE-matched rules only)"
fi

echo "================================================================"
echo "PARALLEL JULIET BENCHMARK — $MODE_DESC"
echo "CWEs: $(echo "$ALL_CWES" | wc -w) | Jobs: $JOBS"
echo "================================================================"

echo "$ALL_CWES" | xargs -P "$JOBS" -I {} bash -c 'scan_cwe "$@"' _ {}

echo ""
echo "================================================================"
echo "ALL SCANS COMPLETE"
echo "================================================================"

# Generate summary
SUMMARY_FILE="$RESULTS_DIR/multi_cwe_summary.txt"
echo "JULIET MULTI-CWE BENCHMARK SUMMARY ($MODE_DESC)" > "$SUMMARY_FILE"
echo "Date: $(date)" >> "$SUMMARY_FILE"
echo "========================================" >> "$SUMMARY_FILE"

for cwe in $ALL_CWES; do
    analysis="$RESULTS_DIR/${cwe}_analysis.txt"
    if [ -f "$analysis" ]; then
        tp_rate=$(grep "True Positive Rate:" "$analysis" | awk '{print $NF}')
        fp_rate=$(grep "False Positive Rate:" "$analysis" | awk '{print $NF}')
        echo "$cwe: TP=${tp_rate} FP=${fp_rate}" >> "$SUMMARY_FILE"
    fi
done

echo "Summary written to: $SUMMARY_FILE"

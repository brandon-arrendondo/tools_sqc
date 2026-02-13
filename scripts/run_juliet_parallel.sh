#!/bin/bash
# Run SqC Juliet benchmark in parallel across CWE categories.
# Usage: ./scripts/run_juliet_parallel.sh [JOBS]
# Default: number of CPUs / 2 (sqc is CPU-intensive)

set -uo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
SQC="$PROJECT_DIR/target/release/sqc"
MANIFEST="$PROJECT_DIR/rules_templates/rules-all.toml"
JULIET_BASE="${HOME}/data/benchmarks/juliet-test-suite-c/testcases"
ANALYZE="$SCRIPT_DIR/analyze_juliet_results.py"
RESULTS_DIR="/tmp/juliet_results"

JOBS="${1:-12}"

mkdir -p "$RESULTS_DIR"

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

    local file_count
    file_count=$(find "$cwe_dir" -name "*.c" | wc -l)

    echo "START: $cwe ($file_count files)"

    local start_time
    start_time=$(date +%s)
    if ! "$SQC" "$cwe_dir" -m "$MANIFEST" -e "$csv_file" >/dev/null 2>&1; then
        echo "ERROR: $cwe scan failed"
        return 1
    fi
    local end_time
    end_time=$(date +%s)
    local elapsed=$((end_time - start_time))

    local violation_count
    violation_count=$(tail -n +2 "$csv_file" 2>/dev/null | wc -l)

    # Run analysis
    python3 "$ANALYZE" --csv "$csv_file" --dir "$cwe_dir" > "$analysis_file" 2>&1

    echo "DONE: $cwe | ${elapsed}s | ${violation_count} violations | ${file_count} files"
}

export -f scan_cwe
export SQC MANIFEST JULIET_BASE ANALYZE RESULTS_DIR

# Get all CWE directories
ALL_CWES=$(ls "$JULIET_BASE")

echo "================================================================"
echo "PARALLEL JULIET BENCHMARK"
echo "CWEs: $(echo "$ALL_CWES" | wc -w) | Jobs: $JOBS"
echo "================================================================"

echo "$ALL_CWES" | xargs -P "$JOBS" -I {} bash -c 'scan_cwe "$@"' _ {}

echo ""
echo "================================================================"
echo "ALL SCANS COMPLETE"
echo "================================================================"

# Generate summary
echo "JULIET MULTI-CWE BENCHMARK SUMMARY (PARALLEL)" > "$RESULTS_DIR/multi_cwe_summary.txt"
echo "Date: $(date)" >> "$RESULTS_DIR/multi_cwe_summary.txt"
echo "========================================" >> "$RESULTS_DIR/multi_cwe_summary.txt"

for cwe in $ALL_CWES; do
    analysis="$RESULTS_DIR/${cwe}_analysis.txt"
    if [ -f "$analysis" ]; then
        tp_rate=$(grep "True Positive Rate:" "$analysis" | awk '{print $NF}')
        fp_rate=$(grep "False Positive Rate:" "$analysis" | awk '{print $NF}')
        echo "$cwe: TP=${tp_rate} FP=${fp_rate}" >> "$RESULTS_DIR/multi_cwe_summary.txt"
    fi
done

echo "Summary written to: $RESULTS_DIR/multi_cwe_summary.txt"

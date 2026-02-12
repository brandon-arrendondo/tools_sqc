#!/bin/bash
# Run SqC against multiple Juliet CWE categories and analyze results.
# Usage: ./scripts/run_juliet_multi_cwe.sh [CWE_DIR...]
# If no args, runs the default priority set.

set -euo pipefail

SQC="$(dirname "$0")/../target/release/sqc"
JULIET_BASE="${HOME}/data/benchmarks/juliet-test-suite-c/testcases"
ANALYZE="$(dirname "$0")/analyze_juliet_results.py"
RESULTS_DIR="/tmp/juliet_results"

mkdir -p "$RESULTS_DIR"

# Default priority CWE categories
DEFAULT_CWES=(
    CWE190_Integer_Overflow
    CWE191_Integer_Underflow
    CWE476_NULL_Pointer_Dereference
    CWE122_Heap_Based_Buffer_Overflow
    CWE401_Memory_Leak
    CWE416_Use_After_Free
    CWE415_Double_Free
    CWE369_Divide_by_Zero
    CWE134_Uncontrolled_Format_String
    CWE457_Use_of_Uninitialized_Variable
    CWE252_Unchecked_Return_Value
    CWE78_OS_Command_Injection
)

if [ $# -gt 0 ]; then
    CWES=("$@")
else
    CWES=("${DEFAULT_CWES[@]}")
fi

SUMMARY_FILE="$RESULTS_DIR/multi_cwe_summary.txt"
echo "JULIET MULTI-CWE BENCHMARK SUMMARY" > "$SUMMARY_FILE"
echo "Date: $(date)" >> "$SUMMARY_FILE"
echo "========================================" >> "$SUMMARY_FILE"

for cwe in "${CWES[@]}"; do
    cwe_dir="$JULIET_BASE/$cwe"
    if [ ! -d "$cwe_dir" ]; then
        echo "SKIP: $cwe (directory not found)"
        continue
    fi

    file_count=$(find "$cwe_dir" -name "*.c" | wc -l)
    csv_file="$RESULTS_DIR/${cwe}.csv"
    cwe_id="${cwe%%_*}"

    echo ""
    echo "================================================================"
    echo "SCANNING: $cwe ($file_count .c files)"
    echo "================================================================"

    # Run SqC scan
    start_time=$(date +%s)
    "$SQC" "$cwe_dir" -e "$csv_file" >/dev/null 2>&1
    end_time=$(date +%s)
    elapsed=$((end_time - start_time))

    violation_count=$(tail -n +2 "$csv_file" | wc -l)
    echo "  Time: ${elapsed}s | Violations: $violation_count"

    # Run ground truth analysis
    echo ""
    python3 "$ANALYZE" --csv "$csv_file" --dir "$cwe_dir" 2>&1 | tee "$RESULTS_DIR/${cwe}_analysis.txt"

    # Append to summary
    echo "" >> "$SUMMARY_FILE"
    echo "$cwe_id ($cwe):" >> "$SUMMARY_FILE"
    echo "  Files: $file_count | Violations: $violation_count | Time: ${elapsed}s" >> "$SUMMARY_FILE"
done

echo ""
echo "================================================================"
echo "ALL RESULTS SAVED TO: $RESULTS_DIR/"
echo "SUMMARY: $SUMMARY_FILE"
echo "================================================================"

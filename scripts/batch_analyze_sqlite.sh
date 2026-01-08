#!/bin/bash
# Batch analysis script for SQLite codebase - file-by-file comparison
# Compares SqC, Clang Static Analyzer, and Cppcheck on each file

set -euo pipefail

SQLITE_SRC="${HOME}/data/sqlite/src"
OUTPUT_DIR="${HOME}/data/sqlite_analysis/batch"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
LOG_FILE="${OUTPUT_DIR}/batch_analysis_${TIMESTAMP}.log"
SQC_BIN="./target/release/sqc"

# Create output directory structure
mkdir -p "$OUTPUT_DIR"/{sqc,clang,cppcheck,csv}

# Initialize summary files
SUMMARY_FILE="${OUTPUT_DIR}/summary_${TIMESTAMP}.csv"
echo "filename,loc,sqc_violations,sqc_time,clang_warnings,clang_time,cppcheck_issues,cppcheck_time,sqc_status,clang_status,cppcheck_status" > "$SUMMARY_FILE"

log() {
    echo "[$(date +'%Y-%m-%d %H:%M:%S')] $*" | tee -a "$LOG_FILE"
}

count_lines() {
    wc -l < "$1" | tr -d ' '
}

# Get list of all C files
find "$SQLITE_SRC" -maxdepth 1 -name "*.c" -type f | sort > "${OUTPUT_DIR}/file_list.txt"
TOTAL_FILES=$(wc -l < "${OUTPUT_DIR}/file_list.txt")

log "=== SQLite Batch Analysis Started ==="
log "Source directory: $SQLITE_SRC"
log "Output directory: $OUTPUT_DIR"
log "Total files to process: $TOTAL_FILES"
log "SqC binary: $SQC_BIN"
log ""

# Check if SqC binary exists
if [ ! -f "$SQC_BIN" ]; then
    log "ERROR: SqC binary not found at $SQC_BIN"
    log "Run: cargo build --release"
    exit 1
fi

# Verify tools are available
command -v clang >/dev/null 2>&1 || { log "WARNING: clang not found, skipping Clang analysis"; SKIP_CLANG=1; }
command -v cppcheck >/dev/null 2>&1 || { log "WARNING: cppcheck not found, skipping Cppcheck analysis"; SKIP_CPPCHECK=1; }

FILE_NUM=0

# Process each file
while IFS= read -r file; do
    FILE_NUM=$((FILE_NUM + 1))
    filename=$(basename "$file" .c)

    log "[$FILE_NUM/$TOTAL_FILES] Processing: $filename.c"

    LOC=$(count_lines "$file")

    # === 1. SqC Analysis ===
    SQC_START=$(date +%s)
    SQC_STATUS="success"
    SQC_VIOLATIONS=0

    if timeout 120 "$SQC_BIN" "$file" --export "${OUTPUT_DIR}/csv/sqc_${filename}.csv" \
        > "${OUTPUT_DIR}/sqc/sqc_${filename}.log" 2>&1; then
        # Count violations from CSV (skip header)
        if [ -f "${OUTPUT_DIR}/csv/sqc_${filename}.csv" ]; then
            SQC_VIOLATIONS=$(tail -n +2 "${OUTPUT_DIR}/csv/sqc_${filename}.csv" | wc -l)
        fi
    else
        SQC_STATUS="failed"
        log "  ⚠️  SqC failed or timed out"
    fi
    SQC_END=$(date +%s)
    SQC_TIME=$((SQC_END - SQC_START))
    log "  SqC: $SQC_VIOLATIONS violations in ${SQC_TIME}s [$SQC_STATUS]"

    # === 2. Clang Static Analyzer ===
    CLANG_WARNINGS=0
    CLANG_TIME=0
    CLANG_STATUS="skipped"

    if [ -z "${SKIP_CLANG:-}" ]; then
        CLANG_START=$(date +%s)
        CLANG_STATUS="success"

        if timeout 120 clang --analyze \
            -Xclang -analyzer-checker=security \
            -Xclang -analyzer-checker=unix \
            -Xclang -analyzer-checker=core \
            -Xclang -analyzer-checker=alpha.security \
            "$file" > "${OUTPUT_DIR}/clang/clang_${filename}.log" 2>&1; then
            # Count warnings (Clang outputs to stderr and creates .plist files)
            CLANG_WARNINGS=$(grep -c "warning:" "${OUTPUT_DIR}/clang/clang_${filename}.log" || echo 0)
        else
            # Check if it's a build artifact issue
            if grep -q "fatal error.*not found" "${OUTPUT_DIR}/clang/clang_${filename}.log" 2>/dev/null; then
                CLANG_STATUS="needs_build"
                log "  ⚠️  Clang needs build artifacts (parse.h missing)"
            else
                CLANG_STATUS="failed"
                log "  ⚠️  Clang failed or timed out"
            fi
        fi
        CLANG_END=$(date +%s)
        CLANG_TIME=$((CLANG_END - CLANG_START))
        log "  Clang: $CLANG_WARNINGS warnings in ${CLANG_TIME}s [$CLANG_STATUS]"
    fi

    # === 3. Cppcheck ===
    CPPCHECK_ISSUES=0
    CPPCHECK_TIME=0
    CPPCHECK_STATUS="skipped"

    if [ -z "${SKIP_CPPCHECK:-}" ]; then
        CPPCHECK_START=$(date +%s)
        CPPCHECK_STATUS="success"

        if timeout 120 cppcheck --enable=all --inconclusive --force \
            "$file" > "${OUTPUT_DIR}/cppcheck/cppcheck_${filename}.log" 2>&1; then
            # Count all issue types: error, warning, style, performance, portability
            CPPCHECK_ISSUES=$(grep -cE "(error:|warning:|style:|performance:|portability:)" "${OUTPUT_DIR}/cppcheck/cppcheck_${filename}.log" || echo 0)
        else
            CPPCHECK_STATUS="failed"
            log "  ⚠️  Cppcheck failed or timed out"
        fi
        CPPCHECK_END=$(date +%s)
        CPPCHECK_TIME=$((CPPCHECK_END - CPPCHECK_START))
        log "  Cppcheck: $CPPCHECK_ISSUES issues in ${CPPCHECK_TIME}s [$CPPCHECK_STATUS]"
    fi

    # Write to summary CSV
    echo "$filename,$LOC,$SQC_VIOLATIONS,$SQC_TIME,$CLANG_WARNINGS,$CLANG_TIME,$CPPCHECK_ISSUES,$CPPCHECK_TIME,$SQC_STATUS,$CLANG_STATUS,$CPPCHECK_STATUS" >> "$SUMMARY_FILE"

    log ""

done < "${OUTPUT_DIR}/file_list.txt"

log "=== Batch Analysis Complete ==="
log "Summary file: $SUMMARY_FILE"
log "Detailed logs in: $OUTPUT_DIR"

# Generate quick statistics
log ""
log "=== Quick Statistics ==="
log "Total files analyzed: $TOTAL_FILES"
log ""

# Aggregate stats using awk
awk -F',' 'NR>1 {
    sqc_total += $3; sqc_time_total += $4;
    clang_total += $5; clang_time_total += $6;
    cppcheck_total += $7; cppcheck_time_total += $8;
    sqc_success += ($9 == "success" ? 1 : 0);
    clang_success += ($10 == "success" ? 1 : 0);
    cppcheck_success += ($11 == "success" ? 1 : 0);
}
END {
    print "SqC:"
    print "  Total violations: " sqc_total
    print "  Total time: " sqc_time_total "s"
    print "  Successful: " sqc_success "/" NR-1
    print ""
    print "Clang Static Analyzer:"
    print "  Total warnings: " clang_total
    print "  Total time: " clang_time_total "s"
    print "  Successful: " clang_success "/" NR-1
    print ""
    print "Cppcheck:"
    print "  Total issues: " cppcheck_total
    print "  Total time: " cppcheck_time_total "s"
    print "  Successful: " cppcheck_success "/" NR-1
}' "$SUMMARY_FILE" | tee -a "$LOG_FILE"

log ""
log "To view results:"
log "  Summary: cat $SUMMARY_FILE"
log "  Full log: cat $LOG_FILE"
log "  Individual results: ls $OUTPUT_DIR/{sqc,clang,cppcheck}/"

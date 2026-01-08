#!/bin/bash
# Test the batch analysis on a single file

TEST_FILE="${1:-${HOME}/data/sqlite/src/complete.c}"
OUTPUT_DIR="${HOME}/data/sqlite_analysis/test"
SQC_BIN="./target/release/sqc"

mkdir -p "$OUTPUT_DIR"

echo "Testing on: $TEST_FILE"
echo "Output dir: $OUTPUT_DIR"
echo ""

filename=$(basename "$TEST_FILE" .c)

# Test SqC
echo "=== Testing SqC ==="
time "$SQC_BIN" "$TEST_FILE" --export "$OUTPUT_DIR/sqc_${filename}.csv" 2>&1 | tee "$OUTPUT_DIR/sqc_${filename}.log"
echo ""

# Test Clang
echo "=== Testing Clang ==="
time clang --analyze \
    -Xclang -analyzer-checker=security \
    -Xclang -analyzer-checker=unix \
    -Xclang -analyzer-checker=core \
    -Xclang -analyzer-checker=alpha.security \
    "$TEST_FILE" 2>&1 | tee "$OUTPUT_DIR/clang_${filename}.log"
echo ""

# Test Cppcheck
echo "=== Testing Cppcheck ==="
time cppcheck --enable=all --inconclusive --force \
    "$TEST_FILE" 2>&1 | tee "$OUTPUT_DIR/cppcheck_${filename}.log"
echo ""

echo "=== Results Summary ==="
echo "SqC violations: $(tail -n +2 "$OUTPUT_DIR/sqc_${filename}.csv" 2>/dev/null | wc -l)"
echo "Clang warnings: $(grep -c "warning:" "$OUTPUT_DIR/clang_${filename}.log" 2>/dev/null || echo 0)"
echo "Cppcheck issues: $(grep -cE "(error:|warning:)" "$OUTPUT_DIR/cppcheck_${filename}.log" 2>/dev/null || echo 0)"
echo ""
echo "Output files in: $OUTPUT_DIR"
ls -lh "$OUTPUT_DIR"

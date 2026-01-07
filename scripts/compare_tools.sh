#!/bin/bash
# Quick comparison script for CERT C analysis tools

if [ -z "$1" ]; then
    echo "Usage: $0 <path_to_c_file_or_directory>"
    exit 1
fi

TARGET="$1"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
OUTPUT_DIR="./analysis_${TIMESTAMP}"

mkdir -p "$OUTPUT_DIR"

echo "Starting CERT C analysis comparison..."
echo "Target: $TARGET"
echo "Output: $OUTPUT_DIR"
echo ""

# 1. SqC Analysis
echo "=== Running SqC ==="
./target/release/sqc "$TARGET" \
    --export "$OUTPUT_DIR/sqc_violations.csv" \
    2>&1 | tee "$OUTPUT_DIR/sqc_output.txt"
echo ""

# 2. Clang Static Analyzer
echo "=== Running Clang Static Analyzer ==="
if [ -f "$TARGET" ]; then
    clang --analyze \
        -Xclang -analyzer-checker=security \
        -Xclang -analyzer-checker=unix \
        -Xclang -analyzer-checker=core \
        -Xclang -analyzer-checker=alpha.security \
        "$TARGET" 2>&1 | tee "$OUTPUT_DIR/clang_output.txt"
else
    echo "Clang requires individual files. Use scan-build for projects:"
    echo "  cd $TARGET && scan-build make"
fi
echo ""

# 3. Cppcheck
echo "=== Running Cppcheck ==="
cppcheck --enable=all \
    --inconclusive \
    --force \
    --xml \
    --xml-version=2 \
    "$TARGET" \
    2> "$OUTPUT_DIR/cppcheck_output.xml"

cppcheck --enable=all \
    --inconclusive \
    --force \
    "$TARGET" \
    2>&1 | tee "$OUTPUT_DIR/cppcheck_output.txt"
echo ""

echo "=== Analysis Complete ==="
echo "Results saved to: $OUTPUT_DIR"
echo ""
echo "Files generated:"
ls -lh "$OUTPUT_DIR"

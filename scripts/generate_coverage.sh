#!/bin/bash
# scripts/generate_coverage.sh - Generate coverage report with exclusions

set -e

BUILD_DIR="${1:-build}"
OUTPUT_DIR="${2:-coverage_report}"

echo "=== Generating Coverage Report ==="
echo "Build directory: $BUILD_DIR"
echo "Output directory: $OUTPUT_DIR"

# Step 1: Capture coverage data
echo ""
echo "Step 1: Capturing coverage data..."
lcov --capture \
    --directory "$BUILD_DIR" \
    --output-file "$BUILD_DIR/coverage_total.info" \
    --ignore-errors mismatch

# Step 2: Remove unwanted files
echo ""
echo "Step 2: Removing excluded files..."
lcov --remove "$BUILD_DIR/coverage_total.info" \
    '*/dependencies/*' \
    '*/tests/*' \
    '*/build/*' \
    '*/tolua_*.c' \
    '*/utility/*.c' \
    '*/common/scriptcore/*.c' \
    '*/common/networking/connection.c' \
    '*/common/aicore/cm.c' \
    '*/common/aicore/path_finding.c' \
    --output-file "$BUILD_DIR/coverage_filtered.info" \
    --ignore-errors unused

# Step 3: Generate summary
echo ""
echo "Step 3: Coverage Summary"
echo "========================"
lcov --summary "$BUILD_DIR/coverage_filtered.info"

# Step 4: Generate HTML report
echo ""
echo "Step 4: Generating HTML report..."
genhtml "$BUILD_DIR/coverage_filtered.info" \
    --output-directory "$OUTPUT_DIR" \
    --title "Freeciv Test Coverage" \
    --ignore-errors unused

echo ""
echo "=== Coverage report generated successfully ==="
echo "Open $OUTPUT_DIR/index.html to view the report"

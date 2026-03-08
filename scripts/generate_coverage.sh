#!/bin/bash
# scripts/generate_coverage.sh - Generate coverage report with exclusions

set -e

BUILD_DIR="${1:-build}"
OUTPUT_DIR="${2:-coverage_report}"

echo "=== Generating Coverage Report ==="
echo "Build directory: $BUILD_DIR"
echo "Output directory: $OUTPUT_DIR"

# Get absolute path to project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Step 1: Capture coverage data
echo ""
echo "Step 1: Capturing coverage data..."
lcov --ignore-errors=gcov,unused,empty \
    --capture \
    --directory "$BUILD_DIR" \
    --output-file "$BUILD_DIR/coverage_total.info"

# Step 2: Remove unwanted files
echo ""
echo "Step 2: Removing excluded files..."
lcov --ignore-errors=empty,unused \
    --remove "$BUILD_DIR/coverage_total.info" \
    "${PROJECT_ROOT}/dependencies/*" \
    "${PROJECT_ROOT}/tests/*" \
    "${PROJECT_ROOT}/build/*" \
    '*/tolua_*.c' \
    "${PROJECT_ROOT}/utility/*" \
    "${PROJECT_ROOT}/common/scriptcore/*" \
    "${PROJECT_ROOT}/common/networking/connection.c" \
    "${PROJECT_ROOT}/common/aicore/cm.c" \
    "${PROJECT_ROOT}/common/aicore/path_finding.c" \
    "${PROJECT_ROOT}/server/*" \
    "${PROJECT_ROOT}/client/*" \
    "${PROJECT_ROOT}/editor/*" \
    '*/tolua/*' \
    '*/lua-*/*' \
    '*/luasql/*' \
    --output-file "$BUILD_DIR/coverage_filtered.info"

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

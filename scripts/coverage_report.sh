#!/bin/bash
BUILD_DIR=${1:-build}
OUTPUT_DIR=${2:-coverage_report}

lcov --capture --directory $BUILD_DIR --output-file $BUILD_DIR/coverage.info
lcov --remove $BUILD_DIR/coverage.info '*/dependencies/*' '*/tests/*' --output-file $BUILD_DIR/coverage_filtered.info
genhtml $BUILD_DIR/coverage_filtered.info --output-directory $OUTPUT_DIR

echo "Coverage report generated in: $OUTPUT_DIR"

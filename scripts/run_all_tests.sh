#!/bin/bash
set -e

BUILD_DIR=${1:-build}

echo "Setting up build..."
meson setup $BUILD_DIR --wipe -Dtests=true -Dcoverage=true

echo "Compiling..."
meson compile -C $BUILD_DIR

echo "Running tests..."
meson test -C $BUILD_DIR --verbose

echo "Generating coverage report..."
./scripts/coverage_report.sh $BUILD_DIR

echo "Checking coverage threshold..."
python3 scripts/check_coverage.py --build-dir $BUILD_DIR --threshold 85

echo "All tests passed!"

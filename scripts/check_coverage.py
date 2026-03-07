#!/usr/bin/env python3
"""Check if test coverage meets threshold."""
import subprocess
import sys
import argparse


def run_lcov(build_dir):
    """Run lcov to capture coverage data."""
    output_file = f"{build_dir}/coverage_total.info"
    subprocess.run([
        "lcov", "--capture",
        "--directory", build_dir,
        "--output-file", output_file
    ], check=True)
    return output_file


def filter_coverage(info_file):
    """Filter out dependencies and tests from coverage data."""
    filtered = info_file.replace(".info", "_filtered.info")
    subprocess.run([
        "lcov", "--remove", info_file,
        "*/dependencies/*", "*/tests/*",
        "--output-file", filtered
    ], check=True)
    return filtered


def get_coverage_percent(info_file):
    """Extract coverage percentage from lcov summary."""
    result = subprocess.run(
        ["lcov", "--summary", info_file],
        capture_output=True, text=True, check=True
    )
    for line in result.stderr.split("\n"):
        if "lines" in line and "%" in line:
            parts = line.split("%")
            percent_str = parts[0].split(":")[-1].strip()
            return float(percent_str)
    return 0.0


def main():
    parser = argparse.ArgumentParser(
        description="Check test coverage meets threshold"
    )
    parser.add_argument(
        "--build-dir",
        default="build",
        help="Build directory containing coverage data"
    )
    parser.add_argument(
        "--threshold",
        type=float,
        default=85.0,
        help="Minimum coverage percentage required"
    )
    args = parser.parse_args()

    info_file = run_lcov(args.build_dir)
    filtered = filter_coverage(info_file)
    percent = get_coverage_percent(filtered)

    print(f"Coverage: {percent:.1f}% (threshold: {args.threshold}%)")
    return 0 if percent >= args.threshold else 1


if __name__ == "__main__":
    sys.exit(main())

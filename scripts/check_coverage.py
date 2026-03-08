#!/usr/bin/env python3
"""Check if test coverage meets threshold."""
import subprocess
import sys
import argparse
import os


def run_lcov(build_dir):
    """Run lcov to capture coverage data."""
    output_file = f"{build_dir}/coverage_total.info"
    # Use shell=True and ignore errors for robust capture
    cmd = (
        f"lcov --ignore-errors=gcov,unused,empty "
        f"--capture --directory {build_dir} "
        f"--output-file {output_file} 2>/dev/null"
    )
    subprocess.run(cmd, shell=True)
    return output_file


def filter_coverage(info_file):
    """Filter out dependencies and tests from coverage data."""
    filtered = info_file.replace(".info", "_filtered.info")
    # Get absolute path to project root
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.dirname(script_dir)

    # Build exclusion patterns with absolute paths
    exclude_patterns = [
        f"{project_root}/dependencies/*",
        f"{project_root}/tests/*",
        f"{project_root}/build/*",
        "*/tolua_*.c",
        f"{project_root}/utility/*",
        f"{project_root}/common/scriptcore/*",
        f"{project_root}/common/networking/connection.c",
        f"{project_root}/common/aicore/cm.c",
        f"{project_root}/common/aicore/path_finding.c",
    ]

    cmd = ["lcov", "--ignore-errors=empty,unused", "--remove", info_file]
    cmd.extend(exclude_patterns)
    cmd.extend(["--output-file", filtered])

    subprocess.run(cmd, check=True)
    return filtered


def get_coverage_percent(info_file):
    """Extract coverage percentage from lcov summary."""
    result = subprocess.run(
        ["lcov", "--summary", info_file],
        capture_output=True, text=True
    )
    # Parse stderr for coverage data
    for line in result.stderr.split("\n"):
        if "lines" in line and "%" in line:
            # Format: "  lines......: 21.1% (5447 of 25870 lines)"
            try:
                percent_part = line.split("%")[0]
                percent_str = percent_part.split(":")[-1].strip()
                return float(percent_str)
            except (ValueError, IndexError):
                continue
    # Fallback: try stdout
    for line in result.stdout.split("\n"):
        if "lines" in line and "%" in line:
            try:
                percent_part = line.split("%")[0]
                percent_str = percent_part.split(":")[-1].strip()
                return float(percent_str)
            except (ValueError, IndexError):
                continue
    return 0.0


def get_function_coverage(info_file):
    """Extract function coverage from lcov data."""
    total_funcs = 0
    hit_funcs = 0
    with open(info_file, 'r') as f:
        for line in f:
            if line.startswith("FNF:"):
                total_funcs += int(line.split(":")[1].strip())
            elif line.startswith("FNH:"):
                hit_funcs += int(line.split(":")[1].strip())
    if total_funcs == 0:
        return 0.0
    return (hit_funcs / total_funcs) * 100.0


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
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Show detailed coverage breakdown"
    )
    args = parser.parse_args()

    print("=== Coverage Check ===")
    print(f"Build directory: {args.build_dir}")
    print(f"Threshold: {args.threshold}%")
    print()

    print("Step 1: Capturing coverage data...")
    info_file = run_lcov(args.build_dir)

    print("Step 2: Filtering excluded files...")
    filtered = filter_coverage(info_file)

    print("Step 3: Analyzing coverage...")
    percent = get_coverage_percent(filtered)
    func_percent = get_function_coverage(filtered)

    print()
    print(f"Line coverage: {percent:.1f}% (threshold: {args.threshold}%)")
    print(f"Function coverage: {func_percent:.1f}%")

    if args.verbose:
        print()
        subprocess.run(["lcov", "--list", filtered], check=False)

    if percent >= args.threshold:
        print("✓ Coverage requirement met!")
        return 0
    else:
        gap = args.threshold - percent
        print(f"✗ Coverage gap: {gap:.1f}%")
        return 1


if __name__ == "__main__":
    sys.exit(main())

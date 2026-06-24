#!/usr/bin/env python3
"""
run_tests.py — Master test runner.

Runs all test_*.py files in order and prints a global summary.

Usage:
    python3 run_tests.py            # all tests
    python3 run_tests.py parser     # only test_parser.py
    python3 run_tests.py brain fsm  # multiple filters
"""
import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../src"))
sys.path.insert(0, os.path.dirname(__file__))

import subprocess
import time
import sys, os


# ANSI
GREEN  = "\033[92m"
RED    = "\033[91m"
YELLOW = "\033[93m"
CYAN   = "\033[96m"
BOLD   = "\033[1m"
DIM    = "\033[2m"
RESET  = "\033[0m"

TESTS_DIR = os.path.dirname(os.path.abspath(__file__))

TEST_FILES = [
    ("tests_parser.py", "Parser (unit)"),
    ("tests_brain.py",  "Brain & Constants (unit)"),
    ("tests_fsm.py",    "FSM integration (mock server)"),
]


def run_file(filepath: str, label: str) -> bool:
    print(f"\n{BOLD}{CYAN}{'━' * 60}{RESET}")
    print(f"{BOLD}{CYAN}  {label}{RESET}")
    print(f"{BOLD}{CYAN}{'━' * 60}{RESET}\n")

    start = time.time()
    result = subprocess.run(
        [sys.executable, filepath],
        cwd=TESTS_DIR,
    )
    elapsed = time.time() - start

    ok = result.returncode == 0
    status = f"{GREEN}PASSED{RESET}" if ok else f"{RED}FAILED{RESET}"
    print(f"\n  → {status}  ({elapsed:.2f}s)\n")
    return ok


def main() -> None:
    filters = sys.argv[1:]   # optional filename filters

    results = []
    for filename, label in TEST_FILES:
        if filters and not any(f in filename for f in filters):
            continue
        filepath = os.path.join(TESTS_DIR, filename)
        ok = run_file(filepath, label)
        results.append((label, ok))

    # Global summary
    total  = len(results)
    passed = sum(1 for _, ok in results if ok)
    failed = total - passed

    print(f"\n{BOLD}{'═' * 60}{RESET}")
    print(f"{BOLD}  GLOBAL SUMMARY: {passed}/{total} suites passed{RESET}")
    if failed:
        print(f"\n{RED}  Failed suites:{RESET}")
        for label, ok in results:
            if not ok:
                print(f"    {RED}✗{RESET} {label}")
    else:
        print(f"  {GREEN}✓ Everything green!{RESET}")
    print(f"{BOLD}{'═' * 60}{RESET}\n")

    sys.exit(0 if failed == 0 else 1)


if __name__ == "__main__":
    main()
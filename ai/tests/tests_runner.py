"""
test_runner.py — Minimal test framework with verbose output.

Usage: imported by each test file, never run directly.
"""

import sys
import traceback
from typing import Callable, List, Tuple


# ANSI colors
GREEN  = "\033[92m"
RED    = "\033[91m"
YELLOW = "\033[93m"
CYAN   = "\033[96m"
BOLD   = "\033[1m"
DIM    = "\033[2m"
RESET  = "\033[0m"

_results: List[Tuple[str, bool, str]] = []   # (name, passed, detail)


def section(title: str) -> None:
    print(f"\n{BOLD}{CYAN}{'─' * 60}{RESET}")
    print(f"{BOLD}{CYAN}  {title}{RESET}")
    print(f"{BOLD}{CYAN}{'─' * 60}{RESET}")


def run_test(name: str, fn: Callable) -> bool:
    print(f"\n{DIM}▶ {name}{RESET}")
    try:
        fn()
        print(f"  {GREEN}✓ PASS{RESET}")
        _results.append((name, True, ""))
        return True
    except AssertionError as e:
        msg = str(e) or "assertion failed"
        print(f"  {RED}✗ FAIL  →  {msg}{RESET}")
        _results.append((name, False, msg))
        return False
    except Exception as e:
        msg = f"{type(e).__name__}: {e}"
        print(f"  {RED}✗ ERROR →  {msg}{RESET}")
        traceback.print_exc()
        _results.append((name, False, msg))
        return False


def log(msg: str) -> None:
    """Verbose log inside a test — printed indented."""
    print(f"     {DIM}{msg}{RESET}")


def summary() -> bool:
    total  = len(_results)
    passed = sum(1 for _, ok, _ in _results if ok)
    failed = total - passed

    print(f"\n{BOLD}{'═' * 60}{RESET}")
    print(f"{BOLD}  Results: {passed}/{total} passed", end="")
    if failed:
        print(f"  {RED}({failed} failed){RESET}")
    else:
        print(f"  {GREEN}✓ All good!{RESET}")
    print(f"{BOLD}{'═' * 60}{RESET}\n")

    if failed:
        print(f"{RED}Failed tests:{RESET}")
        for name, ok, detail in _results:
            if not ok:
                print(f"  {RED}✗{RESET} {name}  →  {detail}")
        print()

    return failed == 0
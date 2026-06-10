"""
mock_server.py — Fake Zappy server for integration tests.

Runs in a background thread and speaks the exact same protocol
as the real C++ server.  Tests set up a script of expected
commands and canned responses, then verify the AI behaved correctly.

Usage (from test_fsm.py):
    with MockServer(script) as ms:
        ai = ZappyAI("127.0.0.1", ms.port, "team1")
        ai.run()
"""

import socket
import threading
import time
import sys
from typing import List, Tuple, Optional, Callable

# Verbose flag — set to True to see every line exchanged
VERBOSE = True

GREEN = "\033[92m"
BLUE  = "\033[94m"
DIM   = "\033[2m"
RESET = "\033[0m"


def _log_server(msg: str) -> None:
    if VERBOSE:
        print(f"  {DIM}[MockServer]{RESET} {msg}", file=sys.stderr)


# ─────────────────────────────────────────────────────────────────────
# Script types
# ============ #

class Expect:
    """Assert the AI sends this exact command (case-insensitive prefix)."""
    def __init__(self, cmd: str, respond: str = "ok"):
        self.cmd     = cmd.strip().lower()
        self.respond = respond

    def __repr__(self):
        return f"Expect({self.cmd!r} → {self.respond!r})"


class Push:
    """Push an unsolicited line (e.g. broadcast message) to the AI."""
    def __init__(self, line: str):
        self.line = line

    def __repr__(self):
        return f"Push({self.line!r})"


class Done:
    """Signal that the test is over — server closes the connection."""
    pass


# ========== #
# MockServer
# ========== #

class MockServer:
    """
    A one-shot TCP server that accepts exactly one client, runs through
    the given script, then closes.

    Script is a list of Expect / Push / Done entries.
    After the script finishes, the errors list is checked by the test.
    """

    def __init__(
        self,
        script: List,
        team:   str  = "team1",
        width:  int  = 10,
        height: int  = 10,
        slots:  int  = 5,
    ):
        self.script  = script
        self.team    = team
        self.width   = width
        self.height  = height
        self.slots   = slots
        self.errors: List[str] = []
        self.received: List[str] = [] #all commands the AI sent

        self._server_sock: Optional[socket.socket] = None
        self._thread: Optional[threading.Thread]   = None
        self._ready  = threading.Event()
        self.port: int = 0

    #context manager
    def __enter__(self):
        self.start()
        return self

    def __exit__(self, *_):
        self.stop()

    #lifecycle 
    def start(self) -> None:
        self._server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self._server_sock.bind(("127.0.0.1", 0))   # OS picks a free port
        self._server_sock.listen(1)
        self.port = self._server_sock.getsockname()[1]
        _log_server(f"Listening on port {self.port}")

        self._thread = threading.Thread(target=self._serve, daemon=True)
        self._thread.start()
        self._ready.wait(timeout=2.0)

    def stop(self) -> None:
        if self._server_sock:
            try:
                self._server_sock.close()
            except Exception:
                pass
        if self._thread:
            self._thread.join(timeout=3.0)

    #server loop
    def _serve(self) -> None:
        self._ready.set()
        try:
            conn, addr = self._server_sock.accept()
        except OSError:
            return

        _log_server(f"Client connected from {addr}")
        rbuf = ""

        def send(line: str) -> None:
            msg = line if line.endswith("\n") else line + "\n"
            _log_server(f"{GREEN}→ AI{RESET} {msg.rstrip()!r}")
            conn.sendall(msg.encode())

        def recv_line() -> str:
            nonlocal rbuf
            while "\n" not in rbuf:
                chunk = conn.recv(4096).decode(errors="replace")
                if not chunk:
                    raise ConnectionError("AI disconnected")
                rbuf += chunk
            line, rbuf = rbuf.split("\n", 1)
            line = line.rstrip("\r")
            _log_server(f"{BLUE}← AI{RESET} {line!r}")
            self.received.append(line)
            return line

        # Handshake
        send("WELCOME")
        team_line = recv_line()
        if team_line != self.team:
            self.errors.append(f"Expected team={self.team!r}, got {team_line!r}")

        send(str(self.slots))
        send(f"{self.width} {self.height}")

        # Script
        for step in self.script:
            if isinstance(step, Done):
                _log_server("Script Done — closing connection")
                break

            elif isinstance(step, Push):
                send(step.line)

            elif isinstance(step, Expect):
                try:
                    line = recv_line()
                except ConnectionError as e:
                    self.errors.append(f"Connection lost while expecting {step.cmd!r}: {e}")
                    break

                # Check command matches (case-insensitive prefix match)
                line_lower = line.lower()
                if not line_lower.startswith(step.cmd):
                    self.errors.append(
                        f"Expected command starting with {step.cmd!r}, "
                        f"got {line!r}"
                    )
                # Send the canned response
                send(step.respond)

        conn.close()
        _log_server("Connection closed")
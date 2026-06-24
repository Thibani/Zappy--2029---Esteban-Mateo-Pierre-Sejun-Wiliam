"""
connection.py — Raw TCP socket wrapper for the Zappy server protocol.

Handles:
  - Connection + handshake (WELCOME → team → client_num + world_size)
  - Non-blocking line-based send/recv
  - Pending response queue so the FSM can do send+receive cleanly
"""

import socket
import sys
from typing import Optional


class Connection:
    BUFFER_SIZE = 4096

    def __init__(self, host: str, port: int, team: str) -> None:
        self._host   = host
        self._port   = port
        self._team   = team
        self._sock:  Optional[socket.socket] = None
        self._rbuf   = "" # raw recv buffer (incomplete lines)

        # Info received during handshake
        self.client_num:  int = 0
        self.world_width: int = 0
        self.world_height: int = 0

    def connect(self) -> None:
        """Connect and perform the handshake: WELCOME → team → slots + size."""
        self._sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._sock.connect((self._host, self._port))
        self._sock.setblocking(True)

        welcome = self._read_line()
        if welcome != "WELCOME":
            raise ConnectionError(f"Expected WELCOME, got: {welcome!r}")

        self._send_line(self._team)

        # Server sends: CLIENT-NUM\n then X Y\n
        slot_line = self._read_line()
        self.client_num = int(slot_line.strip())

        size_line = self._read_line()
        parts = size_line.strip().split()
        self.world_width  = int(parts[0])
        self.world_height = int(parts[1])

        print(f"[Conn] Connected — team={self._team}  slots={self.client_num}"
              f"  world={self.world_width}x{self.world_height}", file=sys.stderr)

    def send_command(self, cmd: str) -> None:
        """Send a command string (newline added automatically)."""
        self._send_line(cmd)

    def read_line(self) -> str:
        """Block until a complete line is available and return it (no trailing \\n)."""
        return self._read_line()

    def close(self) -> None:
        if self._sock:
            self._sock.close()
            self._sock = None

    def _send_line(self, text: str) -> None:
        msg = text if text.endswith("\n") else text + "\n"
        self._sock.sendall(msg.encode())

    def _read_line(self) -> str:
        """Read bytes until we have a complete \\n-terminated line."""
        while "\n" not in self._rbuf:
            chunk = self._sock.recv(self.BUFFER_SIZE).decode(errors="replace")
            if not chunk:
                raise ConnectionError("Server closed connection")
            self._rbuf += chunk

        line, self._rbuf = self._rbuf.split("\n", 1)
        return line.rstrip("\r")
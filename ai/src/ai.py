"""
zappy_ai.py — Main AI controller.

Implements the FSM loop:
  1. Send a command
  2. Read the response
  3. Update Brain state
  4. Transition to next state

Command pipeline:
The server allows up to 10 queued commands per client.
We use a simple 1-command-at-a-time model here for correctness
(easier to reason about state). Pipelining can be added later.

Look field of view (for reference):
Level 1: tile 0 (self), tiles 1-3 (row ahead)
Level 2: + tiles 4-8 (two rows ahead)
Level 3: + tiles 9-15
etc.
Tile indices in Look response: row-major, left to right per row.
"""

import sys
import random
from typing import Optional

from connection import Connection
from brain import Brain
from fsm import State
from constants import Resource
from parser import (
    parse_inventory,
    parse_look,
    count_players_on_tiles,
    parse_broadcast,
    parse_eject
)


class ZappyAI:
    def __init__(self, host: str, port: int, team: str) -> None:
        self._conn = Connection(host, port, team)
        self._brain: Optional[Brain] = None
        self._state = State.IDLE
        self._pending_forward = False
        self._last_leader_dir = None
        self._follow_stale_ticks = 0

    # =========== #
    # Entry point
    # =========== #

    def run(self) -> None:
        self._conn.connect()
        self._brain = Brain(
            team=self._conn._team,
            world_width=self._conn.world_width,
            world_height=self._conn.world_height,
        )
        self._brain.log("FSM started")
        self._state = State.COLLECT

        while True:
            self._tick()

    # ============= #
    # Main FSM tick
    # ============= #

    def _tick(self) -> None:
        b = self._brain

        self._process_messages()

        # Emergency override: food is critically low
        if b.is_hungry and self._state not in (State.FORAGE,):
            b.log(f"Food low ({b.food}) — switching to FORAGE")
            self._state = State.FORAGE

        elif (b.follow_incantation
              and self._state not in (State.FOLLOW_LEADER, State.INCANTATE)):
            b.log(f"Leader called! Following dir={b.incantation_leader_dir}")
            self._state = State.FOLLOW_LEADER

        # Dispatch 
        if   self._state == State.FORAGE:        self._state_forage()
        elif self._state == State.COLLECT:       self._state_collect()
        elif self._state == State.SEEK_TILE:     self._state_seek_tile()
        elif self._state == State.COORDINATE:    self._state_coordinate()
        elif self._state == State.INCANTATE:     self._state_incantate()
        elif self._state == State.FOLLOW_LEADER: self._state_follow_leader()
        else:                                    self._state_idle()

    # =========================================== #
    # STATE: FORAGE — eat until safe, then resume
    # =========================================== #

    def _state_forage(self) -> None:
        b = self._brain

        if self._pending_forward:
            self._pending_forward = False
            self._send("Forward")
            self._recv()
            inv = self._cmd_inventory()
            if inv[Resource.FOOD] >= 20:
                b.log("Food replenished — back to COLLECT")
                self._state = State.COLLECT
            return

        # Look around
        look = self._cmd_look()
        # Try to grab food on current tile first
        if look and look[0][Resource.FOOD] > 0:
            self._cmd_take("food")
            return
        # Find food on visible tiles
        food_tile = self._nearest_tile_with(look, Resource.FOOD)
        if food_tile is not None:
            self._move_toward_tile(food_tile, len(look))
        else:
            # Explore randomly
            self._random_move()

        # Re-check food
        inv = self._cmd_inventory()
        if inv[Resource.FOOD] >= 20:
            b.log("Food replenished — back to COLLECT")
            self._state = State.COLLECT

    # ====================================================== #
    # STATE: COLLECT — gather resources for next incantation
    # ====================================================== #

    def _state_collect(self) -> None:
        b = self._brain

        if b.is_max_level:
            b.log("Max level reached!")
            self._state = State.IDLE
            return

        if self._pending_forward:
            self._pending_forward = False
            self._send("Forward")
            self._recv()
            return

        # Pick up any useful resource on current tile first
        inv = self._cmd_inventory()
        look = self._cmd_look()

        if not look:
            self._random_move()
            return

        current_tile = look[0]
        missing = b.missing_resources()

        # Grab what we need from the current tile
        grabbed = False
        for res, needed in missing.items():
            if current_tile.get(res, 0) > 0:
                name = res.name.lower()
                self._cmd_take(name)
                grabbed = True
                break  # one take per tick; loop will re-check

        if grabbed:
            return

        # Check if we have everything
        if b.has_enough_resources():
            b.log(f"Resources ready for lv{b.level+1} — coordinating")
            self._state = State.COORDINATE
            return

        # Find a vi"sible tile with a needed resource
        target_res = b.best_resource_to_collect()
        if target_res:
            tile_idx = self._nearest_tile_with(look, target_res)
            if tile_idx is not None:
                self._move_toward_tile(tile_idx, len(look))
                return

        # Nothing visible — explore
        self._random_move()

    # ================================================================= #
    # STATE: SEEK_TILE — walk toward a specific tile index seen in Look
    # ================================================================= #

    def _state_seek_tile(self) -> None:
        self._state = State.COLLECT

    # =========================================================== #
    # STATE: COORDINATE — broadcast readiness, wait for teammates
    # =========================================================== #

    def _state_coordinate(self) -> None:
        b = self._brain
        req = b.requirements_for_next_level()
        if req is None:
            self._state = State.IDLE
            return

        self._cmd_inventory()

        needed_players = req.get("players", 1)

        if needed_players == 1:
            # Solo incantation — go directly
            b.log("Solo incantation possible — INCANTATE")
            self._state = State.INCANTATE
            return

        # Check for incoming follow-leader messages first
        self._process_messages()
        if b.follow_incantation:
            b.log(f"Leader called! Following dir={b.incantation_leader_dir}")
            self._state = State.FOLLOW_LEADER
            return

        # Broadcast CALL and count ready teammates
        call_msg = f"CALL_LV{b.level}"
        self._cmd_broadcast(call_msg)

        # Do a look to count players already on tile
        look_raw = self._do_look_raw()
        counts = count_players_on_tiles(look_raw)
        players_here = counts[0] if counts else 1  # includes self

        # Grab any ready replies
        self._process_messages()
        ready = b.ready_teammates + players_here  # teammates already on tile

        b.log(f"COORDINATE: {ready}/{needed_players} players ready")

        if ready >= needed_players:
            # Tell everyone to start
            self._cmd_broadcast(f"START_LV{b.level}")
            b.ready_teammates = 0
            self._state = State.INCANTATE
        else:
            pass

    # ====================================== #
    # STATE: INCANTATE — trigger incantation
    # ====================================== #

    def _state_incantate(self) -> None:
        b = self._brain

        if b.is_hungry:
            b.log(f"Too hungry ({b.food}) to safely incantate — switching to FORAGE")
            self._state = State.FORAGE
            return

        b.log(f"Attempting incantation at lv{b.level}")

        # Drop the resources on the current tile
        req = b.requirements_for_next_level()
        if req:
            for res in [Resource.LINEMATE, Resource.DERAUMERE, Resource.SIBUR,
                        Resource.MENDIANE, Resource.PHIRAS, Resource.THYSTAME]:
                needed = req.get(res, 0)
                for _ in range(needed):
                    self._cmd_set(res.name.lower())

        # Fire incantation
        result = self._cmd_incantation()

        success = False
        if result and "current level" in result.lower():
            # Extract new level: "Current level: N"
            try:
                new_level = int(result.split(":")[1].strip())
                b.level = new_level
                b.log(f"Incantation SUCCESS — now lv{b.level}")
                success = True
            except Exception:
                b.level += 1
                b.log(f"Incantation SUCCESS (parsed) — now lv{b.level}")
                success = True
        elif result == "ko":
            b.log("Incantation FAILED (ko) — re-collecting")
            # Resources were consumed even on failure; re-collect
        else:
            # Could be "ko" or partial string; try to detect success
            if result and result != "ko":
                b.level += 1
                b.log(f"Incantation result={result!r} — assuming lv{b.level}")
                success = True

        if success and b.is_well_fed:
            if self._cmd_fork():
                b.log("Forked — new egg laid for the team")

        b.follow_incantation = False
        b.waiting_for_incantation = False
        b.ready_teammates = 0
        self._state = State.COLLECT

    # ======================================================= #
    # STATE: FOLLOW_LEADER — walk toward the broadcast source
    # ======================================================= #

    def _state_follow_leader(self) -> None:
        b = self._brain
        direction = b.incantation_leader_dir
        self._cmd_inventory()

        if direction == 0:
            # Already on the leader's tile
            reply = f"READY_LV{b.level}"
            self._cmd_broadcast(reply)
            b.incantation_leader_dir = 0
            b.follow_incantation = False
            self._last_leader_dir = None
            self._follow_stale_ticks = 0
            self._state = State.COORDINATE
            return

        # Move according to direction (1=N, 2=NE, 3=E, 4=SE, 5=S, 6=SW, 7=W, 8=NW)
        self._move_by_broadcast_dir(direction)

        # Check for START signal
        self._process_messages()
        if not b.follow_incantation:
            # Leader cancelled or we lost track
            self._last_leader_dir = None
            self._follow_stale_ticks = 0
            self._state = State.COLLECT
            return

        if b.incantation_leader_dir == self._last_leader_dir:
            self._follow_stale_ticks += 1
        else:
            self._follow_stale_ticks = 0
        self._last_leader_dir = b.incantation_leader_dir

        if self._follow_stale_ticks >= 8:
            b.log("Leader silent for too long — abandoning follow")
            b.follow_incantation = False
            b.incantation_leader_dir = 0
            self._last_leader_dir = None
            self._follow_stale_ticks = 0
            self._state = State.COLLECT

    # ================================ #
    # STATE: IDLE — random exploration
    # ================================ #

    def _state_idle(self) -> None:
        b = self._brain
        self._random_move()
        inv = self._cmd_inventory()
        if not b.is_max_level:
            self._state = State.COLLECT

    # =========================== #
    # Helpers — look + navigation
    # =========================== #

    def _cmd_look(self):
        raw = self._do_look_raw()
        tiles = parse_look(raw)
        counts = count_players_on_tiles(raw)
        self._brain.look_tiles   = tiles
        self._brain.look_players = counts
        return tiles

    def _do_look_raw(self) -> str:
        self._send("Look")
        return self._recv()

    def _tile_row_col(self, idx: int):
        """Return (row, col_in_row, center_col) for a Look tile index."""
        row = 0
        acc = 0
        while acc + (2 * row + 1) <= idx:
            acc += 2 * row + 1
            row += 1
        return row, idx - acc, row

    def _nearest_tile_with(self, look, resource: Resource) -> Optional[int]:
        """Return the index of the tile with this resource that is truly
        closest to the player, not just the first match found while
        scanning the Look list left-to-right.

        Bug this fixes: scanning order means index 1 (front-left of row 1)
        is always checked before index 2 (straight ahead, actually closer)
        or index 3 (front-right). When a resource is common (e.g. food),
        the leftmost tile in every row almost always matches first, so the
        bot always turns Left and never Forward. Turning Left + advancing
        every single tick walks the bot in a perpetual square, never
        closing the distance to anything — exactly the death-by-starvation
        loop seen while standing right next to visible food.
        """
        if not look:
            return None
        best_idx = None
        best_key = None  # (row distance, lateral offset from straight-ahead)
        for i, tile in enumerate(look):
            if tile.get(resource, 0) <= 0:
                continue
            row, col_in_row, center = self._tile_row_col(i)
            lateral = abs(col_in_row - center)
            key = (row, lateral)
            if best_key is None or key < best_key:
                best_key, best_idx = key, i
        return best_idx

    def _move_toward_tile(self, tile_idx: int, total_tiles: int) -> None:
        """
        Given a tile index from Look, move toward it.

        Look layout for level L (total 1 + 3 + 5 + ... = (L+1)^2 tiles):
          Row 0: tile 0 (self)
          Row 1: tiles 1, 2, 3  (left, center, right)
          Row 2: tiles 4, 5, 6, 7, 8
          ...

        Strategy: go forward if tile is ahead (center of row), turn then forward otherwise.
        """
        if tile_idx == 0:
            return  # already here

        # Determine which row and column the tile is in
        row = 0
        acc = 0
        while acc + (2 * row + 1) <= tile_idx:
            acc += 2 * row + 1
            row += 1
        col_in_row = tile_idx - acc      # 0-based within this row
        center     = row                 # center column of this row

        if col_in_row < center:
            self._send("Left")
            self._recv()
            # Commit to advancing next tick instead of re-looking and
            # potentially flipping the turn direction back and forth.
            self._pending_forward = True
        elif col_in_row > center:
            self._send("Right")
            self._recv()
            self._pending_forward = True
        else:
            self._send("Forward")
            self._recv()

    def _random_move(self) -> None:
        action = random.choice(["Forward", "Forward", "Forward", "Left", "Right"])
        self._send(action)
        self._recv()

    def _move_by_broadcast_dir(self, direction: int) -> None:
        """
        Broadcast direction 1-8 (0 = same tile).
        1=N(ahead), 2=NE, 3=E, 4=SE, 5=S, 6=SW, 7=W, 8=NW
        """
        moves = {
            1: ["Forward"],
            2: ["Right", "Forward"],
            3: ["Right", "Forward"],
            4: ["Right", "Right", "Forward"],
            5: ["Right", "Right", "Forward"],   # back = turn around
            6: ["Left", "Left", "Forward"],
            7: ["Left", "Forward"],
            8: ["Left", "Forward"],
        }
        for move in moves.get(direction, ["Forward"]):
            self._send(move)
            self._recv()

    # ================== #
    # Helpers — commands
    # ================== #

    def _cmd_inventory(self):
        self._send("Inventory")
        raw = self._recv()
        inv = parse_inventory(raw)
        self._brain.inventory = inv
        return inv

    def _cmd_take(self, resource_name: str) -> bool:
        self._send(f"Take {resource_name}")
        resp = self._recv()
        if resp == "ok":
            # Refresh inventory lazily next tick
            return True
        return False

    def _cmd_set(self, resource_name: str) -> bool:
        self._send(f"Set {resource_name}")
        resp = self._recv()
        return resp == "ok"

    def _cmd_broadcast(self, text: str) -> None:
        self._send(f"Broadcast {text}")
        self._recv()  # "ok"

    def _cmd_fork(self) -> bool:
        """Lay an egg on the current tile so a new teammate can connect
        and grow the team's player slots. Costs no resources, just time."""
        self._send("Fork")
        resp = self._recv()
        return resp == "ok"

    def _cmd_incantation(self) -> str:
        self._send("Incantation")
        # First response: "Elevation underway" or "ko"
        resp1 = self._recv()
        if resp1 == "ko":
            return "ko"
        # Second response: "Current level: N" after the incantation resolves
        resp2 = self._recv()
        return resp2

    # ======================= #
    # Helpers — message queue
    # ======================= #

    def _process_messages(self) -> None:
        """Check for any incoming broadcast messages (non-blocking peek)."""
        b = self._brain
        # Peek at what's in the read buffer already
        while "\n" in self._conn._rbuf:
            line, self._conn._rbuf = self._conn._rbuf.split("\n", 1)
            line = line.rstrip("\r")
            self._handle_unsolicited(line)

    def _handle_unsolicited(self, line: str) -> None:
        b = self._brain
        parsed = parse_broadcast(line)
        if parsed:
            direction, text = parsed
            b.log(f"Broadcast from dir={direction}: {text!r}")
            lv_tag = f"LV{b.level}"
            if text.startswith(f"CALL_{lv_tag}"):
                if direction != 0:
                    b.log(f"Following leader (dir={direction})")
                    b.follow_incantation = True
                    b.incantation_leader_dir = direction
            elif text.startswith(f"READY_{lv_tag}"):
                b.ready_teammates += 1
            elif text.startswith(f"START_{lv_tag}"):
                # Leader confirmed — we should incantate now
                b.follow_incantation = False
                if self._state == State.FOLLOW_LEADER:
                    self._state = State.INCANTATE
            return

        eject_dir = parse_eject(line)
        if eject_dir is not None:
            b.log(f"Ejected! direction={eject_dir}")
            return

        if line.startswith("dead"):
            b.log("We died!")
            raise SystemExit(0)

    # ===================== #
    # Low-level send / recv
    # ===================== #

    def _send(self, cmd: str) -> None:
        self._brain.log(f"→ {cmd}")
        self._conn.send_command(cmd)

    def _recv(self) -> str:
        """
        Read the next line, handling unsolicited messages (broadcasts, dead)
        transparently — they are queued in brain.messages and we keep reading.
        """
        while True:
            line = self._conn.read_line()
            # Unsolicited server push?
            if (line.startswith("message ") or
                line.startswith("eject:")   or
                line == "dead"):
                self._handle_unsolicited(line)
                if line == "dead":
                    raise SystemExit(0)
                continue
            self._brain.log(f"← {line}")
            return line
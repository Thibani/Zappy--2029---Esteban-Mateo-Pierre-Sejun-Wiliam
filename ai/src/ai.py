"""
zappy_ai.py — Main AI controller.

Implements the FSM loop:
  1. Send a command
  2. Read the response
  3. Update Brain state
  4. Transition to next state

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
    parse_eject,
    is_look_response,
)


class ZappyAI:
    def __init__(self, host: str, port: int, team: str) -> None:
        self._conn = Connection(host, port, team)
        self._brain: Optional[Brain] = None
        self._state = State.IDLE
        self._pending_forward = False
        self._last_leader_dir = None
        self._follow_stale_ticks = 0
        self._follow_absolute_ticks = 0
        self._ticks_since_fork = 0
        self._coordinate_ticks = 0

    # =========== #
    # Entry point
    # =========== #

    def run(self) -> None:
        """Connect to the server, initialise the Brain, and enter the main FSM loop."""
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
        """Run one FSM cycle: process pending messages, apply emergency overrides, then dispatch to the active state handler."""
        b = self._brain
        self._process_messages()

        if b.is_hungry and self._state not in (State.FORAGE,):
            b.log(f"Food low ({b.food}) — switching to FORAGE")
            self._state = State.FORAGE

        elif (b.follow_incantation
              and self._state not in (State.FOLLOW_LEADER, State.INCANTATE)):
            b.log(f"Leader called! Following dir={b.incantation_leader_dir}")
            self._follow_absolute_ticks = 0
            self._follow_stale_ticks = 0
            self._last_leader_dir = None
            self._state = State.FOLLOW_LEADER

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
        """Consume food from nearby tiles until the inventory reaches a safe threshold, then return to COLLECT."""
        b = self._brain

        if self._pending_forward:
            self._pending_forward = False
            look = self._cmd_look()
            if look and look[0][Resource.FOOD] > 0:
                grabbed = 0
                while look[0][Resource.FOOD] > 0:
                    if not self._cmd_take("food"):
                        break
                    look[0][Resource.FOOD] -= 1
                    grabbed += 1
                b.log(f"Forage: grabbed {grabbed} food from tile")
            else:
                self._send("Forward")
                self._recv()
            inv = self._cmd_inventory()
            if inv[Resource.FOOD] >= 20:
                b.log("Food replenished — back to COLLECT")
                self._state = State.COLLECT
            return

        look = self._cmd_look()
        if look and look[0][Resource.FOOD] > 0:
            grabbed = 0
            while look[0][Resource.FOOD] > 0:
                if not self._cmd_take("food"):
                    break
                look[0][Resource.FOOD] -= 1
                grabbed += 1
            b.log(f"Forage: grabbed {grabbed} food from tile")
            inv = self._cmd_inventory()
            b.inventory = inv
            if inv[Resource.FOOD] >= 20:
                b.log("Food replenished — back to COLLECT")
                self._state = State.COLLECT
            return

        food_tile = self._nearest_tile_with(look, Resource.FOOD)
        if food_tile is not None:
            self._move_toward_tile(food_tile, len(look))
        else:
            self._random_move()

        inv = self._cmd_inventory()
        if inv[Resource.FOOD] >= 20:
            b.log("Food replenished — back to COLLECT")
            self._state = State.COLLECT

    # ====================================================== #
    # STATE: COLLECT — gather resources for next incantation
    # ====================================================== #

    def _state_collect(self) -> None:
        """Gather the resources required for the next incantation, periodically fork, and transition to COORDINATE when ready."""
        b = self._brain

        if b.is_max_level:
            b.log("Max level reached!")
            self._state = State.IDLE
            return

        if self._pending_forward:
            self._pending_forward = False
            look = self._cmd_look()
            missing = b.missing_resources()
            if look and any(look[0].get(res, 0) > 0 for res in missing):
                for res in missing:
                    if look[0].get(res, 0) > 0:
                        self._cmd_take(res.name.lower())
                        break
            else:
                self._send("Forward")
                self._recv()
            return

        self._ticks_since_fork += 1
        if self._ticks_since_fork >= 50 and b.level >= 2:
            b.log("Fork périodique — pose d'un oeuf")
            self._cmd_fork()
            self._ticks_since_fork = 0

        inv = self._cmd_inventory()
        look = self._cmd_look()

        if not look:
            self._random_move()
            return

        current_tile = look[0]
        missing = b.missing_resources()

        grabbed = False
        for res in missing:
            if current_tile.get(res, 0) > 0:
                self._cmd_take(res.name.lower())
                grabbed = True
                break

        if grabbed:
            return

        if b.has_enough_resources():
            b.log(f"Resources ready for lv{b.level+1} — coordinating")
            self._coordinate_ticks = 0
            self._state = State.COORDINATE
            return

        target_res = b.best_resource_to_collect()
        if target_res:
            tile_idx = self._nearest_tile_with(look, target_res)
            if tile_idx is not None:
                self._move_toward_tile(tile_idx, len(look))
                return

        self._random_move()

    # ================================================================= #
    # STATE: SEEK_TILE — walk toward a specific tile index seen in Look
    # ================================================================= #

    def _state_seek_tile(self) -> None:
        """Placeholder state that immediately falls back to COLLECT."""
        self._state = State.COLLECT

    # =========================================================== #
    # STATE: COORDINATE — broadcast readiness, wait for teammates
    # =========================================================== #

    def _state_coordinate(self) -> None:
        """Broadcast readiness to teammates and wait until enough players are gathered on the tile before starting the incantation."""
        b = self._brain
        req = b.requirements_for_next_level()
        if req is None:
            self._state = State.IDLE
            return

        self._coordinate_ticks += 1

        if self._coordinate_ticks > 200:
            b.log("COORDINATE timeout — restarting COLLECT")
            b.ready_teammates = 0
            self._coordinate_ticks = 0
            self._state = State.COLLECT
            return

        needed_players = req.get("players", 1)

        if needed_players == 1:
            b.log("Solo incantation possible — INCANTATE")
            self._state = State.INCANTATE
            return

        if b.follow_incantation:
            b.log(f"Leader called! Following dir={b.incantation_leader_dir}")
            self._follow_absolute_ticks = 0
            self._follow_stale_ticks = 0
            self._last_leader_dir = None
            self._state = State.FOLLOW_LEADER
            return

        call_msg = f"CALL_LV{b.level}"
        self._cmd_broadcast(call_msg)

        inv = self._cmd_inventory()
        b.inventory = inv
        self._process_messages()

        look_raw = self._do_look_raw()
        counts = count_players_on_tiles(look_raw)
        teammates_on_tile = max(0, (counts[0] if counts else 1) - 1)
        self._process_messages()

        total = 1 + b.ready_teammates + teammates_on_tile
        b.log(f"COORDINATE: {total}/{needed_players} "
              f"(ready_acks={b.ready_teammates}, on_tile={teammates_on_tile})")

        if total >= needed_players:
            self._cmd_broadcast(f"START_LV{b.level}")
            b.ready_teammates = 0
            self._coordinate_ticks = 0
            self._state = State.INCANTATE

    # ====================================== #
    # STATE: INCANTATE — trigger incantation
    # ====================================== #

    def _state_incantate(self) -> None:
        """Drop required resources on the tile, trigger the incantation, update the level on success, and optionally fork afterward."""
        b = self._brain

        if b.is_hungry:
            b.log(f"Too hungry ({b.food}) to safely incantate — FORAGE")
            self._state = State.FORAGE
            return

        b.log(f"Attempting incantation at lv{b.level}")

        req = b.requirements_for_next_level()
        if req:
            has_resources = all(
                b.inventory.get(res, 0) >= req.get(res, 0)
                for res in [Resource.LINEMATE, Resource.DERAUMERE, Resource.SIBUR,
                            Resource.MENDIANE, Resource.PHIRAS, Resource.THYSTAME]
            )
            if has_resources:
                b.log("Leader role: dropping resources on tile")
                for res in [Resource.LINEMATE, Resource.DERAUMERE, Resource.SIBUR,
                            Resource.MENDIANE, Resource.PHIRAS, Resource.THYSTAME]:
                    needed = req.get(res, 0)
                    for _ in range(needed):
                        self._cmd_set(res.name.lower())
            else:
                b.log("Follower role: resources already on tile, not dropping")

        result = self._cmd_incantation()

        success = False
        if result and "current level" in result.lower():
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
        else:
            if result and result != "ko":
                b.level += 1
                b.log(f"Incantation result={result!r} — assuming lv{b.level}")
                success = True

        if success and b.is_well_fed:
            if self._cmd_fork():
                b.log("Forked after incantation — new egg laid")

        b.follow_incantation = False
        b.waiting_for_incantation = False
        b.ready_teammates = 0
        self._follow_absolute_ticks = 0
        self._ticks_since_fork = 0
        self._coordinate_ticks = 0
        self._state = State.COLLECT

    # ======================================================= #
    # STATE: FOLLOW_LEADER — walk toward the broadcast source
    # ======================================================= #

    def _state_follow_leader(self) -> None:
        """Move toward the broadcast source tile by tile, sending READY once on the same tile, and abort if the leader goes silent or a timeout is reached."""
        b = self._brain
        direction = b.incantation_leader_dir
        self._cmd_inventory()

        self._follow_absolute_ticks += 1

        if self._follow_absolute_ticks >= 60:
            b.log("Timeout absolu FOLLOW_LEADER — leader probablement mort, retour COLLECT")
            self._reset_follow()
            self._state = State.COLLECT
            return

        if b.is_hungry:
            b.log("Trop faim pour suivre le leader — FORAGE")
            self._reset_follow()
            self._state = State.FORAGE
            return

        if direction == 0:
            reply = f"READY_LV{b.level}"
            self._cmd_broadcast(reply)
            b.log("On leader tile — waiting for START")
            self._follow_stale_ticks = 0
            self._process_messages()
            return

        self._move_by_broadcast_dir(direction)
        self._cmd_broadcast(f"WHERE_LV{b.level}")
        self._process_messages()

        if not b.follow_incantation:
            self._reset_follow()
            return

        if b.incantation_leader_dir == self._last_leader_dir:
            self._follow_stale_ticks += 1
        else:
            self._follow_stale_ticks = 0
        self._last_leader_dir = b.incantation_leader_dir

        if self._follow_stale_ticks >= 20:
            b.log("Leader silencieux — abandon follow, retour COLLECT")
            self._reset_follow()
            self._state = State.COLLECT

    def _reset_follow(self) -> None:
        """Clear all follow-leader state and transition back to COLLECT."""
        b = self._brain
        b.follow_incantation = False
        b.incantation_leader_dir = 0
        self._last_leader_dir = None
        self._follow_stale_ticks = 0
        self._follow_absolute_ticks = 0
        self._state = State.COLLECT

    # ================================ #
    # STATE: IDLE — random exploration
    # ================================ #

    def _state_idle(self) -> None:
        """Wander randomly and switch back to COLLECT if the player has not yet reached the maximum level."""
        b = self._brain
        self._random_move()
        inv = self._cmd_inventory()
        if not b.is_max_level:
            self._state = State.COLLECT

    # =========================== #
    # Helpers — look + navigation
    # =========================== #

    def _cmd_look(self):
        """Send a Look command, parse the tile and player data, store results in Brain, and return the tile list."""
        raw = self._do_look_raw()
        tiles = parse_look(raw)
        counts = count_players_on_tiles(raw)
        self._brain.look_tiles   = tiles
        self._brain.look_players = counts
        return tiles

    def _do_look_raw(self) -> str:
        """Send a Look command and return the raw unparsed response string."""
        self._send("Look")
        return self._recv()

    def _tile_row_col(self, idx: int):
        """Convert a flat Look tile index into (row, col_in_row, center_col)."""
        row = 0
        acc = 0
        while acc + (2 * row + 1) <= idx:
            acc += 2 * row + 1
            row += 1
        return row, idx - acc, row

    def _nearest_tile_with(self, look, resource: Resource) -> Optional[int]:
        """Return the index of the closest tile in the Look result that contains at least one unit of the given resource, or None if none is visible."""
        if not look:
            return None
        best_idx = None
        best_key = None
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
        """Issue one movement command (Forward, Left, or Right) toward the tile at the given Look index.

        If a turn is needed, _pending_forward is set so the next tick re-checks
        tile contents before advancing.
        """
        if tile_idx == 0:
            return

        row = 0
        acc = 0
        while acc + (2 * row + 1) <= tile_idx:
            acc += 2 * row + 1
            row += 1
        col_in_row = tile_idx - acc
        center     = row

        if col_in_row < center:
            self._send("Left")
            self._recv()
            self._pending_forward = True
        elif col_in_row > center:
            self._send("Right")
            self._recv()
            self._pending_forward = True
        else:
            self._send("Forward")
            self._recv()

    def _random_move(self) -> None:
        """Issue a random movement command, weighted toward Forward."""
        action = random.choice(["Forward", "Forward", "Forward", "Left", "Right"])
        self._send(action)
        self._recv()

    def _move_by_broadcast_dir(self, direction: int) -> None:
        """Translate a broadcast direction (1-8) into the appropriate sequence of movement commands.

        Direction mapping: 1=N (ahead), 2=NE, 3=E, 4=SE, 5=S (behind),
        6=SW, 7=W, 8=NW. Direction 0 means same tile and is a no-op.
        """
        if direction == 1:
            self._send("Forward"); self._recv()
        elif direction == 2:
            self._send("Right");   self._recv()
            self._send("Forward"); self._recv()
        elif direction == 3:
            self._send("Right");   self._recv()
        elif direction == 4:
            self._send("Right");   self._recv()
            self._send("Right");   self._recv()
            self._send("Forward"); self._recv()
        elif direction == 5:
            self._send("Right");   self._recv()
            self._send("Right");   self._recv()
            self._send("Forward"); self._recv()
        elif direction == 6:
            self._send("Left");    self._recv()
            self._send("Left");    self._recv()
            self._send("Forward"); self._recv()
        elif direction == 7:
            self._send("Left");    self._recv()
        elif direction == 8:
            self._send("Left");    self._recv()
            self._send("Forward"); self._recv()

    # ================== #
    # Helpers — commands
    # ================== #

    def _cmd_inventory(self):
        """Send an Inventory command, parse the response, update Brain, and return the inventory dict."""
        self._send("Inventory")
        raw = self._recv()
        if raw.startswith("[") and "," in raw and not any(
            c.isdigit() for c in raw[:40]
        ):
            self._brain.log(f"WARNING: expected inventory, got look-like response, retrying")
            raw = self._recv()
        inv = parse_inventory(raw)
        self._brain.inventory = inv
        return inv

    def _cmd_take(self, resource_name: str) -> bool:
        """Send a Take command for the given resource and return True if the server acknowledged with 'ok'."""
        self._send(f"Take {resource_name}")
        resp = self._recv()
        return resp == "ok"

    def _cmd_set(self, resource_name: str) -> bool:
        """Send a Set command to drop the given resource on the current tile and return True on success."""
        self._send(f"Set {resource_name}")
        resp = self._recv()
        return resp == "ok"

    def _cmd_broadcast(self, text: str) -> None:
        """Broadcast a message to all players and consume the server acknowledgement."""
        self._send(f"Broadcast {text}")
        self._recv()

    def _cmd_fork(self) -> bool:
        """Lay an egg on the current tile to open a new player slot, and return True if the server accepted the command."""
        self._send("Fork")
        resp = self._recv()
        return resp == "ok"

    def _cmd_incantation(self) -> str:
        """Trigger an incantation and return the final server response, reading a second line if the first is only the elevation preamble."""
        self._send("Incantation")
        resp1 = self._recv()
        if resp1 == "ko":
            return "ko"
        if "current level" in resp1.lower():
            return resp1
        resp2 = self._recv()
        return resp2
    
    # ======================= #
    # Helpers — message queue
    # ======================= #

    def _process_messages(self) -> None:
        """Drain all complete lines already buffered in the TCP receive buffer and dispatch each to _handle_unsolicited."""
        b = self._brain
        while "\n" in self._conn._rbuf:
            line, self._conn._rbuf = self._conn._rbuf.split("\n", 1)
            line = line.rstrip("\r")
            self._handle_unsolicited(line)

    def _handle_unsolicited(self, line: str) -> None:
        """Process an unsolicited server message (broadcast, eject, dead, elevation) and update Brain state accordingly."""
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
                else:
                    b.follow_incantation = True
                    b.incantation_leader_dir = 0

            elif text.startswith(f"WHERE_{lv_tag}"):
                if self._state == State.COORDINATE:
                    self._cmd_broadcast(f"CALL_{lv_tag}")

            elif text.startswith(f"READY_{lv_tag}"):
                b.ready_teammates += 1
                b.log(f"Teammate ready (total={b.ready_teammates})")

            elif text.startswith(f"START_{lv_tag}"):
                b.follow_incantation = False
                if self._state in (State.FOLLOW_LEADER, State.COORDINATE):
                    b.log("START received — switching to INCANTATE")
                    self._state = State.INCANTATE

            return

        eject_dir = parse_eject(line)
        if eject_dir is not None:
            b.log(f"Ejected! direction={eject_dir}")
            return

        if "elevation" in line.lower() or "current level" in line.lower():
            b.log(f"Unsolicited elevation message: {line!r}")
            return

        if line.startswith("dead"):
            b.log("We died!")
            raise SystemExit(0)

    def _send(self, cmd: str) -> None:
        """Log and transmit a command string to the server."""
        self._brain.log(f"→ {cmd}")
        self._conn.send_command(cmd)

    # ===================== #
    # Low-level send / recv
    # ===================== #

    def _recv(self) -> str:
        """Read the next non-unsolicited line from the server, transparently dispatching any broadcasts, ejects, or elevation messages encountered along the way."""
        while True:
            line = self._conn.read_line()
            if (line.startswith("message ") or
                line.startswith("eject:")   or
                line == "dead"             or
                "elevation" in line.lower()):
                self._handle_unsolicited(line)
                if line == "dead":
                    raise SystemExit(0)
                continue
            self._brain.log(f"← {line}")
            return line
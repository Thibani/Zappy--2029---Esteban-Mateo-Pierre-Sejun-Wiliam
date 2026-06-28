"""
brain.py — Mutable state of the AI player.

Centralises everything the FSM needs to make decisions:
  - inventory
  - current level
  - look result (surrounding tiles)
  - pending broadcast messages
  - coordination flags (incantation readiness)
"""

import sys
from typing import List, Optional, Tuple
from constants import Resource, INCANTATION, FOOD_DANGER_THRESHOLD, FOOD_SAFE_THRESHOLD
from parser import Inventory, empty_inventory


class Brain:
    def __init__(self, team: str, world_width: int, world_height: int) -> None:
        self.team         = team
        self.world_width  = world_width
        self.world_height = world_height

        self.level:     int       = 1
        self.inventory: Inventory = empty_inventory()

        # Last result from Look (list of tile inventories)
        self.look_tiles: List[Inventory] = []
        # Player counts per tile from last Look
        self.look_players: List[int] = []

        # Pending coordination messages from teammates
        # Each entry: (direction, text)
        self.messages: List[Tuple[int, str]] = []

        # Set to True when we broadcast "READY_FOR_INCANTATION" and are waiting
        self.waiting_for_incantation: bool = False

        # How many teammates replied "READY" to our call
        self.ready_teammates: int = 0

        # Flag set when we receive "START_INCANTATION" from a leader
        self.follow_incantation: bool = False
        self.incantation_leader_dir: int = 0  # direction to follow leader
        self.follow_incantation_since: int = 0  # tick counter
        self.tick_count: int = 0

        # Misc
        self.steps_without_food: int = 0


    @property
    def food(self) -> int:
        return self.inventory[Resource.FOOD]

    @property
    def is_hungry(self) -> bool:
        return self.food <= FOOD_DANGER_THRESHOLD

    @property
    def is_well_fed(self) -> bool:
        return self.food >= FOOD_SAFE_THRESHOLD

    @property
    def is_max_level(self) -> bool:
        return self.level >= 8

    def requirements_for_next_level(self) -> Optional[dict]:
        """Returns the incantation requirements for going from current level to next, or None if max."""
        if self.level >= 8:
            return None
        return INCANTATION[self.level]

    def missing_resources(self) -> dict:
        """
        Returns a dict of {Resource: missing_count} for the next incantation.
        Empty dict = we have everything (resources-wise).
        """
        req = self.requirements_for_next_level()
        if req is None:
            return {}
        missing = {}
        for res in [Resource.LINEMATE, Resource.DERAUMERE, Resource.SIBUR,
                    Resource.MENDIANE, Resource.PHIRAS, Resource.THYSTAME]:
            needed = req.get(res, 0)
            have   = self.inventory.get(res, 0)
            if have < needed:
                missing[res] = needed - have
        return missing

    def has_enough_resources(self) -> bool:
        return len(self.missing_resources()) == 0

    def needed_players(self) -> int:
        req = self.requirements_for_next_level()
        if req is None:
            return 0
        return req.get("players", 1)

    def best_resource_to_collect(self) -> Optional[Resource]:
        """Returns the most-needed resource (largest deficit), or None."""
        missing = self.missing_resources()
        if not missing:
            return None
        return max(missing, key=lambda r: missing[r])

    def log(self, msg: str) -> None:
        print(f"[AI lv{self.level}] {msg}", file=sys.stderr)
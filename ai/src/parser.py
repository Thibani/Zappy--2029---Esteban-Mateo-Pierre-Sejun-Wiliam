"""
parser.py — Parse server responses into Python objects.

Formats:
  Look → [ item item item, item item, ... ]
        Tiles separated by commas, items within a tile by spaces.
        Tile 0 = current tile, then in the field of view.

  Inventory → [ food N, linemate N, deraumere N, sibur N,
    mendiane N, phiras N, thystame N ]

  Broadcast → message K, text
    K = 0 if same tile, 1-8 for direction
"""

import re
from typing import Dict, List, Optional
from constants import Resource, RESOURCE_NAMES


Inventory = Dict[Resource, int]


def empty_inventory() -> Inventory:
    return {r: 0 for r in Resource}


def parse_inventory(line: str) -> Inventory:
    """
    Parse: [ food 9, linemate 0, deraumere 0, sibur 0,
             mendiane 0, phiras 0, thystame 0 ]
    Returns empty inventory on malformed input (never raises).
    """
    inv = empty_inventory()
    content = line.strip().strip("[]")

    for part in content.split(","):
        part = part.strip()
        if not part:
            continue
        tokens = part.split()
        # Expect exactly "name quantity" — skip malformed tokens
        if len(tokens) == 2:
            name, qty_str = tokens
            name = name.lower()
            if name in RESOURCE_NAMES:
                try:
                    inv[RESOURCE_NAMES[name]] = int(qty_str)
                except ValueError:
                    pass  # ignore malformed quantity
    return inv


def is_look_response(line: str) -> bool:
    """Return True if *line* looks like a Look response (bracketed tile list)."""
    stripped = line.strip()
    return stripped.startswith("[") and "," in stripped


def is_inventory_response(line: str) -> bool:
    """Return True if *line* looks like an Inventory response."""
    stripped = line.strip()
    if not (stripped.startswith("[") and stripped.endswith("]")):
        return False
    # Inventory entries are "name number" pairs separated by commas
    content = stripped.strip("[]")
    for part in content.split(","):
        tokens = part.strip().split()
        if len(tokens) == 2 and tokens[0].lower() in RESOURCE_NAMES:
            try:
                int(tokens[1])
                return True  # at least one valid pair → likely inventory
            except ValueError:
                pass
    return False


def parse_look(line: str) -> List[Inventory]:
    """
    Parse: [ player food, linemate, food player, ... ]
    Returns list of tile inventories (empty tile becomes empty_inventory()).
    Never raises — malformed tiles produce empty_inventory().
    """
    content = line.strip().strip("[]")
    tiles: List[Inventory] = []

    for tile_str in content.split(","):
        tile_str = tile_str.strip()
        inv = empty_inventory()
        for token in tile_str.split():
            token = token.lower()
            if token in RESOURCE_NAMES:
                inv[RESOURCE_NAMES[token]] += 1
        tiles.append(inv)

    # Always return at least one tile
    if not tiles:
        tiles.append(empty_inventory())
    return tiles


def count_players_on_tiles(line: str) -> List[int]:
    """
    Same as parse_look but returns number of 'player' tokens per tile.
    """
    content = line.strip().strip("[]")
    counts: List[int] = []

    for tile_str in content.split(","):
        counts.append(tile_str.lower().split().count("player"))
    return counts


def parse_broadcast(line: str):
    """
    Parse: message K, text
    Returns (direction: int, text: str) or None if malformed.
    """
    m = re.match(r"message (\d+),\s*(.*)", line)
    if not m:
        return None
    return int(m.group(1)), m.group(2).strip()


def parse_eject(line: str):
    """
    Parse: eject: K
    Returns direction int or None.
    """
    m = re.match(r"eject:\s*(\d+)", line)
    if not m:
        return None
    return int(m.group(1))
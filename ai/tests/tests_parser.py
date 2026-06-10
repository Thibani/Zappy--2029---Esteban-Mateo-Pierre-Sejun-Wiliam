"""
test_parser.py — Unit tests for parser.py

Tests every response format the server can send:
  - Inventory
  - Look (various levels, player counts)
  - Broadcast
  - Eject
"""
import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../src"))

from tests_runner import section, run_test, log, summary
from parser import (
    parse_inventory, parse_look, count_players_on_tiles,
    parse_broadcast, parse_eject, empty_inventory
)
from constants import Resource


# ========= #
# INVENTORY
# ========= #

def test_inventory_standard():
    raw = "[ food 9, linemate 2, deraumere 0, sibur 1, mendiane 0, phiras 3, thystame 0 ]"
    log(f"input : {raw!r}")
    inv = parse_inventory(raw)
    log(f"result: {dict(inv)}")
    assert inv[Resource.FOOD]     == 9,  f"food={inv[Resource.FOOD]}"
    assert inv[Resource.LINEMATE] == 2,  f"linemate={inv[Resource.LINEMATE]}"
    assert inv[Resource.SIBUR]    == 1,  f"sibur={inv[Resource.SIBUR]}"
    assert inv[Resource.PHIRAS]   == 3,  f"phiras={inv[Resource.PHIRAS]}"


def test_inventory_all_zeros():
    raw = "[ food 0, linemate 0, deraumere 0, sibur 0, mendiane 0, phiras 0, thystame 0 ]"
    log(f"input : {raw!r}")
    inv = parse_inventory(raw)
    log(f"result: {dict(inv)}")
    for r in Resource:
        assert inv[r] == 0, f"{r.name}={inv[r]} (expected 0)"


def test_inventory_no_spaces():
    """Server might send compact format without leading space."""
    raw = "[food 5, linemate 1, deraumere 0, sibur 0, mendiane 0, phiras 0, thystame 2]"
    log(f"input : {raw!r}")
    inv = parse_inventory(raw)
    log(f"result: {dict(inv)}")
    assert inv[Resource.FOOD]     == 5
    assert inv[Resource.THYSTAME] == 2


def test_inventory_large_values():
    raw = "[ food 999, linemate 100, deraumere 50, sibur 30, mendiane 20, phiras 10, thystame 5 ]"
    log(f"input : {raw!r}")
    inv = parse_inventory(raw)
    log(f"result: {dict(inv)}")
    assert inv[Resource.FOOD]    == 999
    assert inv[Resource.LINEMATE] == 100


# ==== #
# LOOK
# ==== #

def test_look_empty_tiles():
    """Look with nothing visible."""
    raw = "[ , ,  ]"
    log(f"input : {raw!r}")
    tiles = parse_look(raw)
    log(f"tiles : {tiles}")
    assert len(tiles) == 3
    for tile in tiles:
        for r in Resource:
            assert tile[r] == 0


def test_look_level1_simple():
    """Level 1 look: 4 tiles (1 + 3)."""
    raw = "[ player food, linemate, food food, food ]"
    log(f"input : {raw!r}")
    tiles = parse_look(raw)
    log(f"tiles : {tiles}")
    assert len(tiles) == 4
    assert tiles[0][Resource.FOOD]     == 1,  f"tile0 food={tiles[0][Resource.FOOD]}"
    assert tiles[1][Resource.LINEMATE] == 1,  f"tile1 linemate={tiles[1][Resource.LINEMATE]}"
    assert tiles[2][Resource.FOOD]     == 2,  f"tile2 food={tiles[2][Resource.FOOD]}"
    assert tiles[3][Resource.FOOD]     == 1,  f"tile3 food={tiles[3][Resource.FOOD]}"


def test_look_multiple_same_resource():
    """Multiple of the same resource on one tile."""
    raw = "[ linemate linemate linemate, food food ]"
    log(f"input : {raw!r}")
    tiles = parse_look(raw)
    log(f"tiles : {tiles}")
    assert tiles[0][Resource.LINEMATE] == 3
    assert tiles[1][Resource.FOOD]     == 2


def test_look_all_resources():
    """One tile with every resource type."""
    raw = "[ food linemate deraumere sibur mendiane phiras thystame ]"
    log(f"input : {raw!r}")
    tiles = parse_look(raw)
    log(f"tiles : {tiles}")
    assert len(tiles) == 1
    for r in Resource:
        assert tiles[0][r] == 1, f"{r.name}={tiles[0][r]}"


def test_look_player_count():
    """count_players_on_tiles counts 'player' tokens."""
    raw = "[ player player, player, ]"
    log(f"input : {raw!r}")
    counts = count_players_on_tiles(raw)
    log(f"counts: {counts}")
    assert counts[0] == 2, f"tile0 players={counts[0]}"
    assert counts[1] == 1, f"tile1 players={counts[1]}"
    assert counts[2] == 0, f"tile2 players={counts[2]}"


def test_look_player_not_counted_as_resource():
    """'player' tokens must NOT appear in resource inventory."""
    raw = "[ player food player ]"
    log(f"input : {raw!r}")
    tiles = parse_look(raw)
    log(f"tiles : {tiles}")
    assert tiles[0][Resource.FOOD] == 1
    total_resources = sum(tiles[0][r] for r in Resource)
    assert total_resources == 1, f"total resources={total_resources}, expected 1"


def test_look_level2():
    """Level 2 look should have 9 tiles (1+3+5)."""
    tiles_data = ", ".join(["food"] * 9)
    raw = f"[ {tiles_data} ]"
    log(f"input : {raw!r}")
    tiles = parse_look(raw)
    log(f"tile count: {len(tiles)}")
    assert len(tiles) == 9


# ========= #
# BROADCAST
# ========= #

def test_broadcast_direction_0():
    """Direction 0 = same tile as sender."""
    line = "message 0, hello world"
    log(f"input : {line!r}")
    result = parse_broadcast(line)
    log(f"result: {result}")
    assert result == (0, "hello world")


def test_broadcast_direction_8():
    """All 8 directions should parse."""
    for k in range(0, 9):
        line = f"message {k}, CALL_LV3"
        result = parse_broadcast(line)
        log(f"dir {k}: {result}")
        assert result == (k, "CALL_LV3"), f"failed for direction {k}"


def test_broadcast_empty_text():
    line = "message 2, "
    log(f"input : {line!r}")
    result = parse_broadcast(line)
    log(f"result: {result}")
    assert result is not None
    assert result[0] == 2


def test_broadcast_coordination_messages():
    """Test all our coordination protocol messages."""
    for level in range(1, 8):
        for msg_type in ("CALL", "READY", "START"):
            line = f"message 1, {msg_type}_LV{level}"
            result = parse_broadcast(line)
            log(f"  {line!r} → {result}")
            assert result == (1, f"{msg_type}_LV{level}")


def test_broadcast_invalid():
    """Non-broadcast lines should return None."""
    for line in ["ok", "ko", "dead", "elevation underway", "", "Forward"]:
        result = parse_broadcast(line)
        log(f"  {line!r} → {result}")
        assert result is None, f"Expected None for {line!r}, got {result}"


# ===== #
# EJECT
# ===== #

def test_eject_all_directions():
    for k in range(1, 9):
        line = f"eject: {k}"
        result = parse_eject(line)
        log(f"  {line!r} → {result}")
        assert result == k


def test_eject_invalid():
    for line in ["ok", "message 1, hi", "eject", ""]:
        result = parse_eject(line)
        log(f"  {line!r} → {result}")
        assert result is None


# === #
# Run
# === #

if __name__ == "__main__":
    section("INVENTORY PARSING")
    run_test("standard inventory",      test_inventory_standard)
    run_test("all-zeros inventory",     test_inventory_all_zeros)
    run_test("compact format",          test_inventory_no_spaces)
    run_test("large values",            test_inventory_large_values)

    section("LOOK PARSING")
    run_test("empty tiles",             test_look_empty_tiles)
    run_test("level 1 simple",          test_look_level1_simple)
    run_test("multiple same resource",  test_look_multiple_same_resource)
    run_test("all resources on tile",   test_look_all_resources)
    run_test("player count",            test_look_player_count)
    run_test("player not a resource",   test_look_player_not_counted_as_resource)
    run_test("level 2 tile count",      test_look_level2)

    section("BROADCAST PARSING")
    run_test("direction 0",             test_broadcast_direction_0)
    run_test("all directions 0-8",      test_broadcast_direction_8)
    run_test("empty text",              test_broadcast_empty_text)
    run_test("coordination messages",   test_broadcast_coordination_messages)
    run_test("invalid lines → None",    test_broadcast_invalid)

    section("EJECT PARSING")
    run_test("all directions 1-8",      test_eject_all_directions)
    run_test("invalid lines → None",    test_eject_invalid)

    ok = summary()
    sys.exit(0 if ok else 1)
"""
test_brain.py — Unit tests for brain.py and constants.py

Tests:
  - Incantation table integrity (all 7 levels)
  - Brain derived properties (food, hungry, missing resources)
  - Resource tracking across multiple operations
"""
import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../src"))

from tests_runner import section, run_test, log, summary
from brain import Brain
from constants import Resource, INCANTATION, MAX_LEVEL


def make_brain(**kwargs) -> Brain:
    b = Brain(team="testteam", world_width=10, world_height=10)
    for k, v in kwargs.items():
        if k == "inventory":
            b.inventory = v
        elif k == "level":
            b.level = v
    return b


# ================= #
# INCANTATION TABLE
# ================= #

def test_incantation_table_has_7_levels():
    log(f"INCANTATION keys: {sorted(INCANTATION.keys())}")
    assert len(INCANTATION) == 7, f"Expected 7 levels, got {len(INCANTATION)}"


def test_incantation_player_counts():
    expected = {1: 1, 2: 2, 3: 2, 4: 4, 5: 4, 6: 6, 7: 6}
    for lv, nb in expected.items():
        actual = INCANTATION[lv]["players"]
        log(f"  level {lv}: {actual} players (expected {nb})")
        assert actual == nb, f"Level {lv}: players={actual}, expected {nb}"


def test_incantation_level1_solo():
    """Level 1→2 only needs 1 player and 1 linemate."""
    req = INCANTATION[1]
    log(f"  level 1 req: {req}")
    assert req["players"]         == 1
    assert req[Resource.LINEMATE] == 1
    assert req.get(Resource.DERAUMERE, 0) == 0


def test_incantation_level7_hardest():
    """Level 7→8 is the hardest: 6 players + 2 of each stone + 1 thystame."""
    req = INCANTATION[7]
    log(f"  level 7 req: {req}")
    assert req["players"]          == 6
    assert req[Resource.LINEMATE]  == 2
    assert req[Resource.DERAUMERE] == 2
    assert req[Resource.SIBUR]     == 2
    assert req[Resource.MENDIANE]  == 2
    assert req[Resource.PHIRAS]    == 2
    assert req[Resource.THYSTAME]  == 1


def test_incantation_no_food_requirement():
    """Food is never required for incantation (not in the table)."""
    for lv, req in INCANTATION.items():
        assert Resource.FOOD not in req, f"Level {lv} requires food — unexpected"
    log("  No level requires food ✓")


# ======================= #
# BRAIN — FOOD & SURVIVAL
# ======================= #

def test_brain_food_property():
    b = make_brain()
    b.inventory[Resource.FOOD] = 42
    log(f"  brain.food = {b.food}")
    assert b.food == 42


def test_brain_hungry_below_threshold():
    b = make_brain()
    b.inventory[Resource.FOOD] = 5
    log(f"  food=5, is_hungry={b.is_hungry}")
    assert b.is_hungry


def test_brain_not_hungry_above_threshold():
    b = make_brain()
    b.inventory[Resource.FOOD] = 15
    log(f"  food=15, is_hungry={b.is_hungry}")
    assert not b.is_hungry


def test_brain_well_fed():
    b = make_brain()
    b.inventory[Resource.FOOD] = 25
    log(f"  food=25, is_well_fed={b.is_well_fed}")
    assert b.is_well_fed


def test_brain_not_well_fed():
    b = make_brain()
    b.inventory[Resource.FOOD] = 19
    log(f"  food=19, is_well_fed={b.is_well_fed}")
    assert not b.is_well_fed


# ========================= #
# BRAIN — MISSING RESOURCES
# ========================= #

def test_missing_resources_empty_inventory_level1():
    b = make_brain(level=1)
    # Level 1→2 needs 1 linemate; empty inventory → missing 1 linemate
    missing = b.missing_resources()
    log(f"  missing (lv1, empty inv): {missing}")
    assert Resource.LINEMATE in missing
    assert missing[Resource.LINEMATE] == 1


def test_missing_resources_already_has_them():
    b = make_brain(level=1)
    b.inventory[Resource.LINEMATE] = 1
    missing = b.missing_resources()
    log(f"  missing (lv1, has linemate): {missing}")
    assert len(missing) == 0, f"Expected empty, got {missing}"


def test_has_enough_resources_false():
    b = make_brain(level=2)
    # Level 2→3 needs 1 linemate + 1 deraumere + 1 sibur
    b.inventory[Resource.LINEMATE] = 1
    # deraumere and sibur still missing
    result = b.has_enough_resources()
    log(f"  has_enough (lv2, partial): {result}")
    assert not result


def test_has_enough_resources_true():
    b = make_brain(level=2)
    req = INCANTATION[2]
    for res in [Resource.LINEMATE, Resource.DERAUMERE, Resource.SIBUR,
                Resource.MENDIANE, Resource.PHIRAS, Resource.THYSTAME]:
        b.inventory[res] = req.get(res, 0)
    result = b.has_enough_resources()
    log(f"  has_enough (lv2, full inv): {result}")
    assert result


def test_missing_partial_fulfillment():
    b = make_brain(level=7)
    req = INCANTATION[7]
    # Give half the needed stones
    b.inventory[Resource.LINEMATE]  = 1   # needs 2, missing 1
    b.inventory[Resource.DERAUMERE] = 2   # needs 2, OK
    b.inventory[Resource.SIBUR]     = 0   # needs 2, missing 2
    missing = b.missing_resources()
    log(f"  missing (lv7, partial): {missing}")
    assert missing.get(Resource.LINEMATE,  0) == 1
    assert missing.get(Resource.DERAUMERE, 0) == 0
    assert missing.get(Resource.SIBUR,     0) == 2


# ============================ #
# BRAIN — LEVEL & REQUIREMENTS
# ============================ #

def test_requirements_at_max_level():
    b = make_brain(level=8)
    req = b.requirements_for_next_level()
    log(f"  requirements at lv8: {req}")
    assert req is None


def test_is_max_level():
    b = make_brain(level=8)
    log(f"  is_max_level at lv8: {b.is_max_level}")
    assert b.is_max_level


def test_not_max_level():
    for lv in range(1, 8):
        b = make_brain(level=lv)
        assert not b.is_max_level, f"Level {lv} reported as max"
    log("  Levels 1-7 correctly not max ✓")


def test_needed_players_per_level():
    b = make_brain()
    expected = {1: 1, 2: 2, 3: 2, 4: 4, 5: 4, 6: 6, 7: 6}
    for lv, nb in expected.items():
        b.level = lv
        result = b.needed_players()
        log(f"  level {lv}: needed_players={result} (expected {nb})")
        assert result == nb


def test_best_resource_to_collect():
    b = make_brain(level=7)
    # Level 7 needs 2 linemate, 2 deraumere, 2 sibur, 2 mendiane, 2 phiras, 1 thystame
    # Give 1 linemate, nothing else
    b.inventory[Resource.LINEMATE] = 1
    # Largest deficit = 2 (for deraumere, sibur, mendiane, phiras, thystame sort varies)
    best = b.best_resource_to_collect()
    log(f"  best_resource at lv7 (has 1 linemate): {best}")
    assert best is not None
    # Should not suggest linemate (only missing 1) when others are missing 2
    missing = b.missing_resources()
    assert missing[best] == max(missing.values())


def test_best_resource_none_when_complete():
    b = make_brain(level=1)
    b.inventory[Resource.LINEMATE] = 1
    best = b.best_resource_to_collect()
    log(f"  best_resource when complete: {best}")
    assert best is None


# === #
# Run
# === #

if __name__ == "__main__":
    section("INCANTATION TABLE")
    run_test("7 levels defined",         test_incantation_table_has_7_levels)
    run_test("player counts per level",  test_incantation_player_counts)
    run_test("level 1 solo + linemate",  test_incantation_level1_solo)
    run_test("level 7 hardest",          test_incantation_level7_hardest)
    run_test("food never required",      test_incantation_no_food_requirement)

    section("BRAIN — FOOD & SURVIVAL")
    run_test("food property",            test_brain_food_property)
    run_test("is_hungry below 10",       test_brain_hungry_below_threshold)
    run_test("not hungry above 10",      test_brain_not_hungry_above_threshold)
    run_test("is_well_fed above 20",     test_brain_well_fed)
    run_test("not well_fed at 19",       test_brain_not_well_fed)

    section("BRAIN — MISSING RESOURCES")
    run_test("empty inv at lv1",         test_missing_resources_empty_inventory_level1)
    run_test("has everything at lv1",    test_missing_resources_already_has_them)
    run_test("has_enough false partial", test_has_enough_resources_false)
    run_test("has_enough true complete", test_has_enough_resources_true)
    run_test("partial fulfillment lv7",  test_missing_partial_fulfillment)

    section("BRAIN — LEVEL & REQUIREMENTS")
    run_test("requirements at max lv",   test_requirements_at_max_level)
    run_test("is_max_level at 8",        test_is_max_level)
    run_test("not max lv 1-7",           test_not_max_level)
    run_test("needed_players per level", test_needed_players_per_level)
    run_test("best resource to collect", test_best_resource_to_collect)
    run_test("best resource when done",  test_best_resource_none_when_complete)

    ok = summary()
    sys.exit(0 if ok else 1)
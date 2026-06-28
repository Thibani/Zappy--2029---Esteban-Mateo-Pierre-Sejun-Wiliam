"""
test_fsm.py — FSM integration tests using MockServer.

Each test:
  1. Scripts the mock server with expected commands + canned responses
  2. Runs the AI in a thread (with a tick limit to avoid infinite loops)
  3. Asserts the AI sent the right commands in the right order

We test:
  - Handshake
  - FORAGE mode (food low → go find food)
  - COLLECT mode (picks up required stones)
  - Solo incantation (level 1, no teammates needed)
  - Broadcast ejection handling
  - Dead message handling
"""
import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../src"))

import threading
import time

from tests_runner import section, run_test, log, summary
from tests_server import MockServer, Expect, Push, Done
from ai import ZappyAI
from fsm import State
from brain import Brain
from constants import Resource


# ======= #
# Helpers
# ======= #

LOOK_EMPTY    = "[ , , , ]"
LOOK_FOOD     = "[ food, , , ]"
LOOK_LINEMATE = "[ linemate, , , ]"
LOOK_FOOD_AND_LINEMATE = "[ food linemate, , , ]"

INV_FULL_FOOD    = "[ food 30, linemate 0, deraumere 0, sibur 0, mendiane 0, phiras 0, thystame 0 ]"
INV_HUNGRY       = "[ food 5,  linemate 0, deraumere 0, sibur 0, mendiane 0, phiras 0, thystame 0 ]"
INV_HAS_LINEMATE = "[ food 25, linemate 1, deraumere 0, sibur 0, mendiane 0, phiras 0, thystame 0 ]"
INV_EMPTY        = "[ food 0,  linemate 0, deraumere 0, sibur 0, mendiane 0, phiras 0, thystame 0 ]"


def run_ai_limited(host: str, port: int, team: str, max_ticks: int = 30) -> ZappyAI:
    """
    Run the AI in a thread, forcibly stop it after max_ticks state transitions
    by patching _tick to count calls.
    """
    ai = ZappyAI(host, port, team)
    ticks = [0]
    original_tick = ai._tick

    def counting_tick():
        ticks[0] += 1
        if ticks[0] > max_ticks:
            raise SystemExit(0)
        original_tick()

    ai._tick = counting_tick

    t = threading.Thread(target=_safe_run, args=(ai,), daemon=True)
    t.start()
    t.join(timeout=5.0)
    return ai


def _safe_run(ai: ZappyAI) -> None:
    try:
        ai.run()
    except (SystemExit, ConnectionError, BrokenPipeError):
        pass
    except Exception as e:
        print(f"  [AI thread error] {e}", file=sys.stderr)


# =============== #
# TEST: Handshake
# =============== #

def test_handshake():
    """AI must send team name after WELCOME and accept the world size."""
    script = [Done()]
    with MockServer(script, team="myteam", width=20, height=15) as ms:
        log(f"Mock server on port {ms.port}")
        ai = run_ai_limited("127.0.0.1", ms.port, "myteam", max_ticks=1)

    log(f"Server errors   : {ms.errors}")
    log(f"Commands received: {ms.received}")
    assert ms.errors == [], f"Handshake errors: {ms.errors}"
    assert "myteam" in ms.received, f"Team name not sent: {ms.received}"


# ========================================== #
# TEST: FORAGE — food low → Look → Take food
# ========================================== #

def test_forage_takes_food_on_current_tile():
    """
    When food is low, the AI should Look and then Take food if it's on the tile.
    """
    script = [
        Expect("look",      LOOK_FOOD), #sees food on tile 0
        Expect("take food", "ok"),
        Done(),
    ]
    with MockServer(script) as ms:
        log(f"Mock server on port {ms.port}")

        def patched_run(ai):
            ai._conn.connect()
            ai._brain = Brain("team1", 10, 10)
            ai._brain.inventory[Resource.FOOD] = 5 #hungry!
            ai._state = State.FORAGE
            try:
                ai._tick()   #one forage tick
                ai._tick()   #inventory check tick
            except (SystemExit, ConnectionError):
                pass

        ai = ZappyAI("127.0.0.1", ms.port, "team1")
        t = threading.Thread(target=patched_run, args=(ai,), daemon=True)
        t.start()
        t.join(timeout=5.0)

    log(f"Server errors   : {ms.errors}")
    log(f"Commands received: {ms.received}")
    assert "look" in [r.lower() for r in ms.received], "AI never sent Look"
    assert any("take" in r.lower() for r in ms.received), "AI never took food"
    assert ms.errors == [], f"Unexpected errors: {ms.errors}"


# =============================================== #
# TEST: COLLECT — sees linemate on tile, takes it
# =============================================== #

def test_collect_takes_linemate():
    """
    At level 1, AI needs 1 linemate. If it sees linemate on current tile, it
    should Take linemate.
    """
    script = [
        Expect("inventory", INV_FULL_FOOD),
        Expect("look",      LOOK_LINEMATE),
        Expect("take linemate", "ok"),
        Done(),
    ]
    with MockServer(script) as ms:
        log(f"Mock server on port {ms.port}")

        def patched_run(ai):
            ai._conn.connect()
            ai._brain = Brain("team1", 10, 10)
            ai._brain.inventory[Resource.FOOD] = 25 #well fed
            ai._brain.level = 1
            ai._state = State.COLLECT
            try:
                ai._tick()
            except (SystemExit, ConnectionError):
                pass

        ai = ZappyAI("127.0.0.1", ms.port, "team1")
        t = threading.Thread(target=patched_run, args=(ai,), daemon=True)
        t.start()
        t.join(timeout=5.0)

    log(f"Server errors   : {ms.errors}")
    log(f"Commands received: {ms.received}")
    assert any("take" in r.lower() and "linemate" in r.lower() for r in ms.received), \
        f"AI never took linemate. Commands: {ms.received}"
    assert ms.errors == [], f"Unexpected errors: {ms.errors}"


# ================================ #
# TEST: Solo incantation (level 1)
# ================================ #

def test_solo_incantation_level1():
    """
    Level 1 incantation: 1 player + 1 linemate.
    AI should Set linemate then send Incantation.
    """
    script = [
        Expect("set linemate",  "ok"),
        Expect("incantation",   "Elevation underway"),
        Expect("",              "Current level: 2"), #server pushes level
        Done(),
    ]
    with MockServer(script) as ms:
        log(f"Mock server on port {ms.port}")

        def patched_run(ai):
            ai._conn.connect()
            ai._brain = Brain("team1", 10, 10)
            ai._brain.level = 1
            ai._brain.inventory[Resource.FOOD]     = 30
            ai._brain.inventory[Resource.LINEMATE] = 1
            ai._state = State.INCANTATE
            try:
                ai._tick()
            except (SystemExit, ConnectionError):
                pass

        ai = ZappyAI("127.0.0.1", ms.port, "team1")
        t = threading.Thread(target=patched_run, args=(ai,), daemon=True)
        t.start()
        t.join(timeout=5.0)

    log(f"Server errors   : {ms.errors}")
    log(f"Commands received: {ms.received}")
    assert any("set" in r.lower() and "linemate" in r.lower() for r in ms.received), \
        f"AI never Set linemate. Commands: {ms.received}"
    assert any("incantation" in r.lower() for r in ms.received), \
        f"AI never sent Incantation. Commands: {ms.received}"


# =========================== #
# TEST: Dead message handling
# =========================== #

def test_dead_causes_exit():
    """
    When the server sends 'dead', the AI should stop cleanly (SystemExit or disconnect).
    """
    script = [
        Push("dead"),
        Done(),
    ]
    with MockServer(script) as ms:
        log(f"Mock server on port {ms.port}")
        ai = run_ai_limited("127.0.0.1", ms.port, "team1", max_ticks=5)

    log(f"Server errors: {ms.errors}")


# ======================================= #
# TEST: Eject handling — AI doesn't crash
# ======================================= #

def test_eject_does_not_crash():
    """
    When the server pushes 'eject: 3', the AI should silently handle it
    and continue normally.
    """
    script = [
        Push("eject: 3"),
        Expect("inventory", INV_FULL_FOOD),
        Expect("look",      LOOK_EMPTY),
        Done(),
    ]
    with MockServer(script) as ms:
        log(f"Mock server on port {ms.port}")

        def patched_run(ai):
            ai._conn.connect()
            ai._brain = Brain("team1", 10, 10)
            ai._brain.inventory[Resource.FOOD] = 30
            ai._brain.level = 1
            ai._state = State.COLLECT
            try:
                ai._tick()
            except (SystemExit, ConnectionError):
                pass

        ai = ZappyAI("127.0.0.1", ms.port, "team1")
        t = threading.Thread(target=patched_run, args=(ai,), daemon=True)
        t.start()
        t.join(timeout=5.0)

    log(f"Server errors   : {ms.errors}")
    log(f"Commands received: {ms.received}")
    # The test passes if no crash occurred
    assert ms.errors == [], f"Unexpected errors: {ms.errors}"


# ================================================ #
# TEST: Broadcast coordination — follower response
# ================================================ #

def test_follower_responds_to_call():
    """
    When AI receives 'message 3, CALL_LV1', it should switch to FOLLOW_LEADER
    and eventually reply with 'Broadcast READY_LV1'.
    """
    script = [
        Push("message 3, CALL_LV1"),
        Expect("broadcast",  "ok"),
        Done(),
    ]
    with MockServer(script) as ms:
        log(f"Mock server on port {ms.port}")

        def patched_run(ai):
            ai._conn.connect()
            ai._brain = Brain("team1", 10, 10)
            ai._brain.inventory[Resource.FOOD] = 30
            ai._brain.level = 1
            ai._brain.follow_incantation     = True
            ai._brain.incantation_leader_dir = 0 #already on tile
            ai._state = State.FOLLOW_LEADER
            try:
                ai._tick()
            except (SystemExit, ConnectionError):
                pass

        ai = ZappyAI("127.0.0.1", ms.port, "team1")
        t = threading.Thread(target=patched_run, args=(ai,), daemon=True)
        t.start()
        t.join(timeout=5.0)

    log(f"Server errors   : {ms.errors}")
    log(f"Commands received: {ms.received}")
    ready_sent = any("broadcast" in r.lower() and "ready" in r.lower()
                     for r in ms.received)
    assert ready_sent, f"AI never sent READY broadcast. Commands: {ms.received}"


# === #
# Run
# === #

if __name__ == "__main__":
    section("HANDSHAKE")
    run_test("correct team name sent", test_handshake)

    section("FORAGE MODE")
    run_test("takes food on current tile", test_forage_takes_food_on_current_tile)

    section("COLLECT MODE")
    run_test("takes linemate on current tile", test_collect_takes_linemate)

    section("INCANTATION")
    run_test("solo incantation lv1", test_solo_incantation_level1)

    section("UNSOLICITED MESSAGES")
    run_test("dead causes clean exit", test_dead_causes_exit)
    run_test("eject doesn't crash AI", test_eject_does_not_crash)

    section("COORDINATION")
    run_test("follower sends READY on same tile", test_follower_responds_to_call)

    ok = summary()
    sys.exit(0 if ok else 1)
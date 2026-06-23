"""
fsm.py — Finite State Machine for the Zappy AI.

States:
FORAGE          Collect food — highest priority, triggered when food is low.
COLLECT         Collect resources needed for the next incantation.
SEEK_TILE       Move toward a tile that has desired resources (seen via Look).
COORDINATE      Broadcast readiness + wait for teammates before incantation.
INCANTATE       Perform the incantation (only when on correct tile with teammates).
FOLLOW_LEADER   Move toward an incantation leader who called us.
IDLE            Explore randomly when nothing else to do.

Transitions:
Any state → FORAGE        if food ≤ FOOD_DANGER_THRESHOLD
FORAGE    → COLLECT       if food ≥ FOOD_SAFE_THRESHOLD
COLLECT   → COORDINATE    if all resources collected AND level < 8
COORDINATE→ INCANTATE     if enough teammates replied READY on same tile
INCANTATE → COLLECT       (after response, level updated by server)
FOLLOW_LEADER → COORDINATE once arrived (dir == 0)
IDLE      → COLLECT       periodically

Broadcasts used for coordination:
  "CALL_LV<N>"       — Leader calls level-N players to incantate
  "READY_LV<N>"      — Follower answers "I'm on your tile"
  "START_LV<N>"      — Leader confirms everyone is here, start now
"""

from enum import Enum, auto


class State(Enum):
    FORAGE         = auto()  # Collect food, survival mode
    COLLECT        = auto()  # Collect stones for next incantation
    SEEK_TILE      = auto()  # Move toward spotted resources
    COORDINATE     = auto()  # Broadcast + wait for teammates
    INCANTATE      = auto()  # Execute incantation
    FOLLOW_LEADER  = auto()  # Walk toward incantation leader
    IDLE           = auto()  # Random exploration
"""
constants.py — All game-wide constants for Zappy.

Incantation requirements (per official Epitech subject):
  Level L means "to go from L to L+1".
  Each entry: (nb_players, food, linemate, deraumere, sibur, mendiane, phiras, thystame)
  (food is NOT consumed during incantation, only players + stones matter)
"""

from enum import IntEnum


# Ressources
class Resource(IntEnum):
    FOOD      = 0
    LINEMATE  = 1
    DERAUMERE = 2
    SIBUR     = 3
    MENDIANE  = 4
    PHIRAS    = 5
    THYSTAME  = 6


RESOURCE_NAMES = {
    "food":      Resource.FOOD,
    "linemate":  Resource.LINEMATE,
    "deraumere": Resource.DERAUMERE,
    "sibur":     Resource.SIBUR,
    "mendiane":  Resource.MENDIANE,
    "phiras":    Resource.PHIRAS,
    "thystame":  Resource.THYSTAME,
}

# Incantation table
# Each level index = current level (1-based, so index 0 unused)
# Tuple: (nb_players, linemate, deraumere, sibur, mendiane, phiras, thystame)
INCANTATION = {
    1: {"players": 1, Resource.LINEMATE: 1, Resource.DERAUMERE: 0, Resource.SIBUR: 0,
        Resource.MENDIANE: 0, Resource.PHIRAS: 0, Resource.THYSTAME: 0},
    2: {"players": 2, Resource.LINEMATE: 1, Resource.DERAUMERE: 1, Resource.SIBUR: 1,
        Resource.MENDIANE: 0, Resource.PHIRAS: 0, Resource.THYSTAME: 0},
    3: {"players": 2, Resource.LINEMATE: 2, Resource.DERAUMERE: 0, Resource.SIBUR: 1,
        Resource.MENDIANE: 0, Resource.PHIRAS: 2, Resource.THYSTAME: 0},
    4: {"players": 4, Resource.LINEMATE: 1, Resource.DERAUMERE: 1, Resource.SIBUR: 2,
        Resource.MENDIANE: 0, Resource.PHIRAS: 1, Resource.THYSTAME: 0},
    5: {"players": 4, Resource.LINEMATE: 1, Resource.DERAUMERE: 2, Resource.SIBUR: 1,
        Resource.MENDIANE: 3, Resource.PHIRAS: 0, Resource.THYSTAME: 0},
    6: {"players": 6, Resource.LINEMATE: 1, Resource.DERAUMERE: 2, Resource.SIBUR: 3,
        Resource.MENDIANE: 0, Resource.PHIRAS: 1, Resource.THYSTAME: 0},
    7: {"players": 6, Resource.LINEMATE: 2, Resource.DERAUMERE: 2, Resource.SIBUR: 2,
        Resource.MENDIANE: 2, Resource.PHIRAS: 2, Resource.THYSTAME: 1},
}

MAX_LEVEL = 8

# Command duration
COST = {
    "Forward":     7,
    "Right":       7,
    "Left":        7,
    "Look":        7,
    "Inventory":   1,
    "Broadcast":   7,
    "Connect_nbr": 0,
    "Fork":        42,
    "Eject":       7,
    "Incantation": 300,
    "Take":        7,
    "Set":         7,
}

# Food units per food item (1 food = 126 time units of life)
FOOD_LIFE_UNITS = 126

# Minimum food threshold before switching to FORAGE mode
FOOD_DANGER_THRESHOLD = 20
FOOD_SAFE_THRESHOLD   = 40
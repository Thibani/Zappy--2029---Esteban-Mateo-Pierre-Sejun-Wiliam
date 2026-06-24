/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** orientation.hpp - Player orientation enum.
**
** The numeric values MUST match the GUI protocol's O field
** (in pnw and ppo messages): 1=N, 2=E, 3=S, 4=W.
*/

#pragma once

namespace Zappy {
    enum class Orientation : int {
        NORTH = 1,
        EAST  = 2,
        SOUTH = 3,
        WEST  = 4
    };
} // namespace Zappy
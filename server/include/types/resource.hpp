/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** resource.hpp - Resource enum + Inventory struct shared across server modules.
**
** The numeric values MUST stay in sync with the GUI protocol (q0..q6 in
** bct/pin messages, and the i field in pdr/pgt).
*/

#pragma once

namespace Zappy {
    enum class Resource : int {
        FOOD = 0,
        LINEMATE = 1,
        DERAUMERE = 2,
        SIBUR = 3,
        MENDIANE = 4,
        PHIRAS = 5,
        THYSTAME = 6
    };

    constexpr int kRessourceCount = 7;

    /**
     * Snapshot of resource counts on a tile or in a player's inventory.
     * Index with the Resource enum, e.g. inv[Resource::FOOD].
     */
    struct Inventory {
        int q[kRessourceCount] = {0, 0, 0, 0, 0, 0, 0};

        int &operator[](Resource r) noexcept
        {
            return q[static_cast<int>(r)];
        }

        int operator[](Resource r) const noexcept
        {
            return q[static_cast<int>(r)];
        }
    };
} // namespace Zappy
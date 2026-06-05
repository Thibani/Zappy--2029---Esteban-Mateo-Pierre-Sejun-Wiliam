/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** gameEvents.hpp - IGameEventListener interface called by game logic on
** every state mutation.
**
** ─── CONTRACT FOR GAME LOGIC AUTHORS (Sejun) ─────────────────────────────
**
** Your Game/World/CommandHandler holds a pointer or reference to one
** IGameEventListener. Every time a piece of world state changes, call
** the matching on*() method. You do NOT need to know whether anyone is
** listening, or what they do with the event. The default implementation
** (GUIProtocol) pushes the matching protocol line to all connected GUIs.
**
** Granularity rule: methods that group multiple GUI messages (pgt + bct +
** pin for Take, for example) accept all the post-state data they need so
** the listener can emit every required line in one call. You don't have
** to call onTileChanged + onPlayerInventoryChanged separately.
**
** For unit tests or headless runs, pass a NullGameEventListener — every
** method is a no-op.
*/

#pragma once

#include <string>
#include <vector>

#include "../types/orientation.hpp"
#include "../types/resource.hpp"

namespace Zappy {
    class IGameEventListener {
        
    };

    class NullGameEventListener : public IGameEventListener {

    };
} // namespace Zappy
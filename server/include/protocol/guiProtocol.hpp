/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** guiProtocol.hpp - Translates game events into GUI protocol lines
** (v3.3) and pushes them through Server.
**
** ─── Design ──────────────────────────────────────────────────────────────
** This class has two roles:
**   1. It implements IGameEventListener, so Game logic can notify it of
**      every world mutation. It formats the matching protocol line and
**      broadcasts it to all connected GUIs.
**   2. It exposes emit*() methods for direct query responses (when a
**      GUI asks msz, bct, mct, tna, ppo, plv, pin, sgt).
**
** It holds a reference to Server and uses two public methods:
**   - Server::broadcastToGuis(line)  for pushed events to all GUIs
**   - Server::sendToClient(fd, line) for targeted responses
**
** ─── Pure formatters ─────────────────────────────────────────────────────
** All fmt*() methods are static + inline so they can be unit-tested in
** isolation without instantiating GUIProtocol or Server.
*/

#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "events/gameEvents.hpp"
#include "types/orientation.hpp"
#include "types/resource.hpp"

namespace Zappy {
    class Server; // forward declararation

    class GUIProtocol : public IGameEventListener {
        public:
            explicit GUIProtocol(Server &server);

            // ── Direct emitters (query responses / initial-state dump) ──────
            // Pass guiFd >= 0 to target one client, or guiFd = -1 to broadcast.
            void emitMapSize(int width, int height, int guiFd = -1);
            void emitTileContent(int x, int y, const Inventory &contents, int guiFd = -1);
            void emitTeamName(const std::string &name, int guiFd = -1);
            void emitPlayerPosition(int playerId, int x, int y, Orientation o, int guiFd = -1);
            void emitPlayerLevel(int playerId, int level, int guiFd = -1);
            void emitPlayerInventory(int playerId, int x, int y, const Inventory &inv, int guiFd = -1);
            void emitTimeUnit(int frequency, int guiFd = -1);
        private:

    };
} // namespace Zappy
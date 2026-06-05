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

            // ── Error responses ─────────────────────────────────────────────
            void emitUnknownCommand(int guiFd); // "suc"
            void emitBadParameters(int guiFd);  // "sbp"

            // ── IGameEventListener implementation ───────────────────────────
            void onMapSize(int width, int height) override;
            void onTeamRegistered(const std::string &name) override;
            void onTileChanged(int x, int y, const Inventory &contents) override;
            void onPlayerConnected(int playerId, int x, int y, Orientation orientation, int level, const std::string &teamName) override;
            void onPlayerMoved(int playerId, int x, int y, Orientation orientation) override;
            void onPlayerExpelled(int playerId) override;
            void onPlayerBroadcast(int playerId, const std::string &message) override;
            void onPlayerForked(int playerId) override;
            void onPlayerDied(int playerId) override;
            void onPlayerTookResource(int playerId, Resource resource, int x, int y, const Inventory &newTileContents, const Inventory &newPlayerInventory) override;
            void onPlayerDroppedResource(int playerId, Resource resource, int x, int y, const Inventory &newTileContents, const Inventory &newPlayerInventory) override;
            void onIncantationStarted(int x, int y, int level, const std::vector<int> &playerIds) override;
            void onIncantationEnded(int x, int y, bool success, const std::vector<int> &playerIds, int newLevel, const Inventory &newTileContents) override;
            void onEggLaid(int eggId, int parentPlayerId, int x, int y) override;
            void onEggHatched(int eggId) override;
            void onEggDied(int eggId) override;
            void onTimeUnitChanged(int newfrequency) override;
            void onGameEnded(const std::string &winnerTeam) override;
            void onServerMessage(const std::string &message) override;

            // ─── Pure formatters (inline, no I/O) ────────────────────────────
            // Public + static so unit tests can verify the spec format without
            // instantiating GUIProtocol or Server.
            // R = 1 for success, R = 0 for failure (incantation result).

            static inline std::string fmtMsz(int width, int height)
            {
                std::ostringstream os;
                os << "msz " << width << ' ' << height << '\n';
                return os.str();
            }

            static inline std::string fmtBct(int x, int y, const Inventory &inv)
            {
                std::ostringstream os;
                os << "bct " << x << ' ' << y
                   << ' ' << inv[Resource::FOOD]
                   << ' ' << inv[Resource::LINEMATE]
                   << ' ' << inv[Resource::DERAUMERE]
                   << ' ' << inv[Resource::SIBUR]
                   << ' ' << inv[Resource::MENDIANE]
                   << ' ' << inv[Resource::PHIRAS]
                   << ' ' << inv[Resource::THYSTAME]
                   << '\n';
                return os.str();
            }

            static inline std::string fmtTna(const std::string &name)
            {
                return "tna " + name + "\n";
            }

            static inline std::string fmtPnw(int playerId, int x, int y, Orientation o, int level, const std::string &team)
            {
                std::ostringstream os;
                os << "pnw #" << playerId << ' ' << x << ' ' << y
                   << ' ' << static_cast<int>(o)
                   << ' ' << level
                   << ' ' << team
                   << '\n';
                return os.str();
            }

            static inline std::string fmtPpo(int playerId, int x, int y, Orientation o)
            {
                std::ostringstream os;
                os << "ppo #" << playerId << ' ' << x << ' ' << y
                   << ' ' << static_cast<int>(o)
                   << '\n';
                return os.str();
            }

            static inline std::string fmtPlv(int playerId, int level)
            {
                std::ostringstream os;
                os << "plv #" << playerId << ' ' << level << '\n';
                return os.str();
            }

            static inline std::string fmtPin(int playerId, int x, int y, const Inventory &inv)
            {
                std::ostringstream os;
                os << "pin #" << playerId << ' ' << x << ' ' << y
                   << ' ' << inv[Resource::FOOD]
                   << ' ' << inv[Resource::LINEMATE]
                   << ' ' << inv[Resource::DERAUMERE]
                   << ' ' << inv[Resource::SIBUR]
                   << ' ' << inv[Resource::MENDIANE]
                   << ' ' << inv[Resource::PHIRAS]
                   << ' ' << inv[Resource::THYSTAME]
                   << '\n';
                return os.str();
            }

            static inline std::string fmtPex(int id)
            {
                std::ostringstream os;
                os << "pex #" << id << '\n';
                return os.str();
            }

            static inline std::string fmtPbc(int id, const std::string &message)
            {
                std::ostringstream os;
                os << "pbc #" << id << ' ' << message << '\n';
                return os.str(); 
            }

            static inline std::string fmtPic(int x, int y, int level, const std::vector<int> &playerIds)
            {
                std::ostringstream os;
                os << "pic " << x << ' ' << y << ' ' << level;
                for (int id : playerIds)
                    os << " #" << id;
                os << '\n';
                return os.str();
            }

            static inline std::string fmtPie(int x, int y, bool success)
            {
                std::ostringstream os;
                os << "pie " << x << ' ' << y << ' ' << (success ? 1 : 0) << '\n';
                return os.str();
            }
        private:

    };
} // namespace Zappy
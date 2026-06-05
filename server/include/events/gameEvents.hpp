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
        public:
            virtual ~IGameEventListener() = default;

            // ─── World / map ─────────────────────────────────────────────────
            /** Map dimensions, typically sent once at server start. */
            virtual void onMapSize(int width, int height) = 0;

            /** A new team is registered (typically at startup, from -n names). */
            virtual void onTeamRegistered(const std::string &name) = 0;

            /**
            * A tile's resource content changed for a reason NOT already
            * covered by a take/drop event (e.g. periodic resource respawn
            * every 20 ticks, or items destroyed by Eject).
            */
           virtual void onTileChanged(int x, int y, const Inventory &contents) = 0;

            // ─── Players ─────────────────────────────────────────────────────
            /** A new player just connected (egg hatched into a player). */
            virtual void onPlayerConnected(int playerId, int x, int y, Orientation orientation, int level, const std::string &teamName) = 0;

            /** Player moved (Forward) or turned (Right/Left). */
            virtual void onPlayerMoved(int playerId, int x, int y, Orientation orientation) = 0;

            /** Player was ejected from their tile by another player's Eject. */
            virtual void onPlayerExpelled(int playerId) = 0;

            /** Player sent a Broadcast message. */
            virtual void onPlayerBroadcast(int playerId, const std::string &message) = 0;

            /** Player issued a Fork (lays an egg). The egg triggers onEggLaid. */
            virtual void onPlayerForked(int playerId) = 0;

            /** Player died (starvation or end of game). */
            virtual void onPlayerDied(int playerId) = 0;

            /**
            * Player executed Take successfully.
            * Implementation emits pgt + bct + pin.
            */
            virtual void onPlayerTookResource(int playerId, Resource resource, int x, int y, const Inventory &newTileContents, const Inventory &newPlayerInventory) = 0;

            /**
            * Player executed Set successfully.
            * Implementation emits pdr + bct + pin.
            */
            virtual void onPlayerDroppedResource(int playerId, Resource resource, int x, int y, const Inventory &newTileContents, const Inventory &newPlayerInventory) = 0;

            // ─── Incantation ─────────────────────────────────────────────────
            /**
            * Incantation started. The full list of player ids participating
            * is given (used in the pic message: pic X Y L #n #n ...).
            */
            virtual void onIncantationStarted(int x, int y, int level, const std::vector<int> &playerIds) = 0;

            /**
            * Incantation ended. If success, implementation also emits plv per
            * player and bct for the tile (since stones are consumed).
            * @param newLevel  The level players now have (only meaningful if success).
            * @param newTileContents  Tile inventory AFTER stones consumed.
            */
            virtual void onIncantationEnded(int x, int y, bool success, const std::vector<int> &playerIds, int newLevel, const Inventory &newTileContents) = 0;

            // ─── Eggs ────────────────────────────────────────────────────────
            /** An egg was laid (either at startup via -c, or after a Fork). */
            virtual void onEggLaid(int eggId, int parentPlayerId, int x, int y) = 0;

            /** A client connected and took possession of this egg. */
            virtual void onEggHatched(int eggId) = 0;

            /** An egg was destroyed (e.g. by Eject on its tile). */
            virtual void onEggDied(int eggId) = 0;

            // ─── Server-level ────────────────────────────────────────────────
            /** A GUI changed the server's time unit frequency (sst). */
            virtual void onTimeUnitChanged(int newfrequency) = 0;

            /** A team reached the victory condition (6 level-8 players). */
            virtual void onGameEnded(const std::string &winnerTeam) = 0;

            /** Generic server-pushed message to all GUIs (smg). */
            virtual void onServerMessage(const std::string &message) = 0;
    };

    /**
    * No-op listener. Use in unit tests, headless runs, or as a placeholder
    * before GUIProtocol is wired in.
    */
    class NullGameEventListener : public IGameEventListener {
        public:
            void onMapSize(int, int) override {}
            void onTeamRegistered(const std::string &) override {}
            void onTileChanged(int, int, const Inventory &) override {}
            void onPlayerConnected(int, int, int, Orientation, int, const std::string &) override {}
            void onPlayerMoved(int, int, int, Orientation) override {}
            void onPlayerExpelled(int) override {}
            void onPlayerBroadcast(int, const std::string &) override {}
            void onPlayerForked(int) override {}
            void onPlayerDied(int) override {}
            void onPlayerTookResource(int, Resource, int, int, const Inventory &, const Inventory &) override {}
            void onPlayerDroppedResource(int, Resource, int, int, const Inventory &, const Inventory &) override {}
            void onIncantationStarted(int, int, int, const std::vector<int> &) override {}
            void onIncantationEnded(int, int, bool, const std::vector<int> &, int, const Inventory &) override {}
            void onEggLaid(int, int, int, int) override {}
            void onEggHatched(int) override {}
            void onEggDied(int) override {}
            void onTimeUnitChanged(int) override {}
            void onGameEnded(const std::string &) override {}
            void onServerMessage(const std::string &) override {}
    };
} // namespace Zappy
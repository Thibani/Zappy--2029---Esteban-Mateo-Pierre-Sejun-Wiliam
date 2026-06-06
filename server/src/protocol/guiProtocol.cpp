/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** guiProtocol.cpp - Implementation of GUIProtocol.
**
** All 25 message types from GUI protocol v3.3 are implemented.
**
** ----- Dependency ---------------------------------------------------
** This file requires Server to expose two public methods:
**   - void Server::broadcastToGuis(const std::string &line);
**   - void Server::sendToClient(int fd, const std::string &line);
** They must be added before this PR can build. Coordinated with Esteban.
*/

#include "protocol/guiProtocol.hpp"
#include "server/server.hpp"

namespace Zappy {
    GUIProtocol::GUIProtocol(Server &server)
        : _server(server)
    {}

    void GUIProtocol::_send(const std::string &line, int guiFd)
    {
        if (guiFd < 0)
            _server.broadcastToGuis(line);
        else
            _server.sendToClient(guiFd, line);
    }

    // ---------------------------------------------------------------------
    // Direct emitters — for GuiCommandHandler responses or initial state
    // ---------------------------------------------------------------------
    void GUIProtocol::emitMapSize(int width, int height, int guiFd)
    {
        _send(fmtMsz(width, height), guiFd);
    }

    void GUIProtocol::emitTileContent(int x, int y, const Inventory &contents, int guiFd)
    {
        _send(fmtBct(x, y, contents), guiFd);
    }

    void GUIProtocol::emitTeamName(const std::string &name, int guiFd)
    {
        _send(fmtTna(name), guiFd);
    }

    void GUIProtocol::emitPlayerPosition(int playerId, int x, int y, Orientation o, int guiFd)
    {
        _send(fmtPpo(playerId, x, y, o), guiFd);
    }

    void GUIProtocol::emitPlayerLevel(int playerId, int level, int guiFd)
    {
        _send(fmtPlv(playerId, level), guiFd);
    }

    void GUIProtocol::emitPlayerInventory(int playerId, int x, int y, const Inventory &inv, int guiFd)
    {
        _send(fmtPin(playerId, x, y, inv), guiFd);
    }

    void GUIProtocol::emitTimeUnit(int frequency, int guiFd)
    {
        _send(fmtSgt(frequency), guiFd);
    }

    // ---------------------------------------------------------------------
    // Error responses
    // ---------------------------------------------------------------------
    void GUIProtocol::emitUnknownCommand(int guiFd)
    {
        _send("suc\n", guiFd);
    }

    void GUIProtocol::emitBadParameters(int guiFd)
    {
        _send("sbp\n", guiFd);
    }

    // ---------------------------------------------------------------------
    // IGameEventListener — pushed events broadcast to all GUIs
    // ---------------------------------------------------------------------
    void GUIProtocol::onMapSize(int width, int height)
    {
        _send(fmtMsz(width, height), -1);
    }

    void GUIProtocol::onTeamRegistered(const std::string &name)
    {
        _send(fmtTna(name), -1);
    }

    void GUIProtocol::onTileChanged(int x, int y, const Inventory &contents)
    {
        _send(fmtBct(x, y, contents), -1);
    }

    void GUIProtocol::onPlayerConnected(int playerId, int x, int y, Orientation orientation, int level, const std::string &teamName)
    {
        _send(fmtPnw(playerId, x, y, orientation, level, teamName), -1);
    }

    void GUIProtocol::onPlayerMoved(int playerId, int x, int y, Orientation orientation)
    {
        _send(fmtPpo(playerId, x, y, orientation), -1);
    }

    void GUIProtocol::onPlayerExpelled(int playerId)
    {
        _send(fmtPex(playerId), -1);
    }

    void GUIProtocol::onPlayerBroadcast(int playerId, const std::string &message)
    {
        _send(fmtPbc(playerId, message), -1);
    }

    void GUIProtocol::onPlayerForked(int playerId)
    {
        _send(fmtPfk(playerId), -1);
    }

    void GUIProtocol::onPlayerDied(int playerId)
    {
        _send(fmtPdi(playerId), -1);
    }

    void GUIProtocol::onPlayerTookResource(int playerId, Resource resource, int x, int y, const Inventory &newTileContents, const Inventory &newPlayerInventory)
    {
        _send(fmtPgt(playerId, resource), -1);
        _send(fmtBct(x, y, newTileContents), -1);
        _send(fmtPin(playerId, x, y, newPlayerInventory), -1);
    }

    void GUIProtocol::onPlayerDroppedResource(int playerId, Resource resource, int x, int y, const Inventory &newTileContents, const Inventory &newPlayerInventory)
    {
        _send(fmtPdr(playerId, resource), -1);
        _send(fmtBct(x, y, newTileContents), -1);
        _send(fmtPin(playerId, x, y, newPlayerInventory), -1);
    }

    void GUIProtocol::onIncantationStarted(int x, int y, int level, const std::vector<int> &playerIds)
    {
        _send(fmtPic(x, y, level, playerIds), -1);
    }

    void GUIProtocol::onIncantationEnded(int x, int y, bool success, const std::vector<int> &playerIds, int newLevel, const Inventory &newTileContents)
    {
        _send(fmtPie(x, y, success), -1);
        if (success) {
            _send(fmtBct(x, y, newTileContents), -1);
            for (int id : playerIds)
                _send(fmtPlv(id, newLevel), -1);
        }
    }

    void GUIProtocol::onEggLaid(int eggId, int parentPlayerId, int x, int y)
    {
        _send(fmtEnw(eggId, parentPlayerId, x, y), -1);
    }

    void GUIProtocol::onEggHatched(int eggId)
    {
        _send(fmtEbo(eggId), -1);
    }

    void GUIProtocol::onEggDied(int eggId)
    {
        _send(fmtEdi(eggId), -1);
    }

    void GUIProtocol::onTimeUnitChanged(int newfrequency)
    {
        _send(fmtSst(newfrequency), -1);
    }

} // namespace Zappy

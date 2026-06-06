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
} // namespace Zappy

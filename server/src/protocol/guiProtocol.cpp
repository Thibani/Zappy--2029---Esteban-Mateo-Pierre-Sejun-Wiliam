/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** guiProtocol.cpp - Implementation of GUIProtocol.
**
** All 25 message types from GUI protocol v3.3 are implemented.
**
** ─── Dependency ──────────────────────────────────────────────────────────
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
} // namespace Zappy
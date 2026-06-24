/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Server.hpp
*/
#pragma once

#include "utils/args.hpp"
#include "utils/clock.hpp"
#include "server/client.hpp"
#include "server/commandHandler.hpp"
#include "protocol/guiProtocol.hpp"
#include "exceptions/serverException.hpp"

#include <unordered_map>
#include <vector>
#include <poll.h>

namespace Zappy {

    // Forward declarations — implemented by person 2 and person 3
    class Game;

    class Server {
    public:
        Server(const Args &args, Game &game);
        ~Server();

        // Non-copyable
        Server(const Server &)            = delete;
        Server &operator=(const Server &) = delete;

        void run();

        // -- GUI protocol interface (person 3) --
        // Sends a line to every connected GUI client
        void broadcastToGuis(const std::string &line);
        // Sends a line to one specific client by fd (targeted GUI response)
        void sendToClient(int fd, const std::string &line);

    private:
        // Initialisation
        void _initSocket();

        // poll() helpers
        void _acceptClient();
        void _handleRead(int fd);
        void _handleWrite(int fd);
        void _disconnectClient(int fd, const std::string &reason = "");

        // Incremental poll array management
        void _addPollFd(int fd, short events);
        void _removePollFd(int fd);
        void _setPollOut(int fd, bool enable);

        // Clock-driven tick
        int  _nearestDeadlineMs();
        void _processPendingActions();

        Args                             _args;
        int                              _serverFd;
        std::unordered_map<int, Client>  _clients;
        std::vector<struct pollfd>       _pollfds;
        GUIProtocol                      _guiProtocol;
        CommandHandler                   _cmdHandler;
    };

} // namespace Zappy
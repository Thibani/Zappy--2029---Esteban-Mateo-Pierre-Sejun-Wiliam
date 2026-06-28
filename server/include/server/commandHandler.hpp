/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** CommandHandler.hpp
*/
#pragma once

#include "server/client.hpp"
#include "server/guiCommandHandler.hpp"

#include <string>
#include <unordered_map>
#include <functional>

namespace Zappy {

    // Forward declarations — implemented by person 2 and person 3
    class Game;
    class GUIProtocol;

    class CommandHandler {
    public:
        CommandHandler(Game &game, GUIProtocol &guiProtocol);
        ~CommandHandler() = default;

        // Non-copyable
        CommandHandler(const CommandHandler &)            = delete;
        CommandHandler &operator=(const CommandHandler &) = delete;

        // Main entry point — called by Server after popping a command from a client
        void dispatch(Client &client, const std::string &raw);

    private:
        // Authentication step — first message from any client is either a team
        // name or "GRAPHIC"
        void _handleAuth(Client &client, const std::string &teamName);

        // AI command dispatchers
        void _handleForward(const Client &client, const std::string &args);
        void _handleRight(const Client &client, const std::string &args);
        void _handleLeft(const Client &client, const std::string &args);
        void _handleLook(const Client &client, const std::string &args);
        void _handleInventory(const Client &client, const std::string &args);
        void _handleBroadcast(const Client &client, const std::string &args);
        void _handleConnectNbr(const Client &client, const std::string &args);
        void _handleFork(const Client &client, const std::string &args);
        void _handleEject(const Client &client, const std::string &args);
        void _handleIncantation(Client &client, const std::string &args);
        void _handleTake(const Client &client, const std::string &args);
        void _handleSet(const Client &client, const std::string &args);

        // Sends "ko\n" back to the client
        static void _sendKo(Client &client);

        // Command dispatch table: command string -> handler function
        using Handler = std::function<void(Client &, const std::string &)>;
        std::unordered_map<std::string, Handler> _aiHandlers;

        Game        &_game;
        GUIProtocol &_guiProtocol;
        GUICommandHandler _guiCmdHandler;
    };

} // namespace Zappy
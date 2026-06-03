/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** CommandHandler.hpp
*/
#pragma once

#include "server/client.hpp"

#include <string>
#include <unordered_map>
#include <functional>

namespace Zappy {

    // Forward declaration — Game is Sejun's, we only hold a reference
    class Game;

    class CommandHandler {
    public:
        explicit CommandHandler(Game &game);
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
        void _handleForward(Client &client, const std::string &args);
        void _handleRight(Client &client, const std::string &args);
        void _handleLeft(Client &client, const std::string &args);
        void _handleLook(Client &client, const std::string &args);
        void _handleInventory(Client &client, const std::string &args);
        void _handleBroadcast(Client &client, const std::string &args);
        void _handleConnectNbr(Client &client, const std::string &args);
        void _handleFork(Client &client, const std::string &args);
        void _handleEject(Client &client, const std::string &args);
        void _handleIncantation(Client &client, const std::string &args);
        void _handleTake(Client &client, const std::string &args);
        void _handleSet(Client &client, const std::string &args);

        // Sends "ko\n" back to the client
        void _sendKo(Client &client);

        // Command dispatch table: command string -> handler function
        using Handler = std::function<void(Client &, const std::string &)>;
        std::unordered_map<std::string, Handler> _aiHandlers;

        Game &_game;
    };

} // namespace Zappy
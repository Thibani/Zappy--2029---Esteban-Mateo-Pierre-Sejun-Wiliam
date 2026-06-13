/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** guiCommandHandler.hpp - Parses incoming GUI protocol commands.
**
** ---- Role -----------------------------------------------------------------
** When a client authenticated as GUI sends a command (msz, bct, mct, tna,
** ppo, plv, pin, sgt, sst), CommandHandler::dispatch delegates to this
** class. We parse the command line, validate the parameters, and call the
** matching emit*() method on GUIProtocol to produce the response.
*/

#pragma once

#include "server/client.hpp"

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

namespace Zappy {
    class GUIProtocol;

    class GUICommandHandler {
        public:
            explicit GUICommandHandler(GUIProtocol &guiProtocol);
            ~GUICommandHandler() = default;

            // Non-copyable
            GUICommandHandler(const GUICommandHandler &)                = delete;
            GUICommandHandler &operator=(const GUICommandHandler &)    = delete;

            // Entry point — called by CommandHandler::dispatch when
            // client.getType() == ClientType::GUI.
            // cmd  : lowercase command name (e.g. "bct")
            // args : everything after the command (e.g. "5 5")
            void handle(Client &client, const std::string &cmd, const std::string &args);

            // ─── Pure parsing helpers (static, testable in isolation) ───
            // Parses `args` as a whitespace-separated list of `expected` ints.
            // Returns false on any failure (wrong count, non-integer, overflow).
            static bool parseInts(const std::string &args, std::vector<int> &out, std::size_t expected);
    
            // Parses a "#n" argument into an integer player id.
            // Returns false if `arg` doesn't start with '#' or isn't followed
            // by a valid non-negative integer.
            static bool parsePlayerId(const std::string &arg, int &out);

        private:
            // Per command parsers
            void _handleMsz(const Client &client, const std::string &args);
            void _handleBct(const Client &client, const std::string &args);
            void _handleMct(const Client &client, const std::string &args);
            void _handleTna(const Client &client, const std::string &args);
            void _handlePpo(const Client &client, const std::string &args);
            void _handlePlv(const Client &client, const std::string &args);
            void _handlePin(const Client &client, const std::string &args);
            void _handleSgt(const Client &client, const std::string &args);
            void _handleSst(const Client &client, const std::string &args);

            // Dispatch table: lowercase command -> handler function
            using Handler = std::function<void(const Client &, const std::string &)>;
            std::unordered_map<std::string, Handler> _handlers;

            GUIProtocol &_guiProtocol;
    };
} // namespace Zappy
/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** CommandHandler.cpp
*/
#include "server/commandHandler.hpp"
#include "exceptions/serverException.hpp"

#include <iostream>
#include <sstream>
#include <algorithm>

namespace Zappy {

    // -------------------------------------------------------------------------
    // Ctor
    // -------------------------------------------------------------------------

    CommandHandler::CommandHandler(Game &game, GUIProtocol &guiProtocol)
        : _game(game), _guiProtocol(guiProtocol)
    {
        // Build dispatch table — keys are lowercase for case-insensitive matching
        _aiHandlers["forward"]     = [this](Client &c, const std::string &a) { _handleForward(c, a);     };
        _aiHandlers["right"]       = [this](Client &c, const std::string &a) { _handleRight(c, a);       };
        _aiHandlers["left"]        = [this](Client &c, const std::string &a) { _handleLeft(c, a);        };
        _aiHandlers["look"]        = [this](Client &c, const std::string &a) { _handleLook(c, a);        };
        _aiHandlers["inventory"]   = [this](Client &c, const std::string &a) { _handleInventory(c, a);   };
        _aiHandlers["broadcast"]   = [this](Client &c, const std::string &a) { _handleBroadcast(c, a);   };
        _aiHandlers["connect_nbr"] = [this](Client &c, const std::string &a) { _handleConnectNbr(c, a);  };
        _aiHandlers["fork"]        = [this](Client &c, const std::string &a) { _handleFork(c, a);        };
        _aiHandlers["eject"]       = [this](Client &c, const std::string &a) { _handleEject(c, a);       };
        _aiHandlers["incantation"] = [this](Client &c, const std::string &a) { _handleIncantation(c, a); };
        _aiHandlers["take"]        = [this](Client &c, const std::string &a) { _handleTake(c, a);        };
        _aiHandlers["set"]         = [this](Client &c, const std::string &a) { _handleSet(c, a);         };
    }

    // -------------------------------------------------------------------------
    // Public
    // -------------------------------------------------------------------------

    void CommandHandler::dispatch(Client &client, const std::string &raw)
    {
        if (raw.empty())
            return;

        // Not authenticated yet — first message must be a team name or "GRAPHIC"
        if (!client.isAuthenticated()) {
            _handleAuth(client, raw);
            return;
        }

        // Split raw into command + args
        // e.g. "Broadcast hello world" -> cmd="broadcast", args="hello world"
        std::istringstream iss(raw);
        std::string        cmd;
        std::string        args;

        iss >> cmd;
        std::getline(iss >> std::ws, args);

        // Lowercase the command for case-insensitive matching
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

        if (client.getType() == ClientType::AI) {
            auto it = _aiHandlers.find(cmd);
            if (it == _aiHandlers.end()) {
                std::cerr << "[CommandHandler] Unknown command from fd="
                          << client.getFd() << ": \"" << cmd << "\"\n";
                _sendKo(client);
                return;
            }
            it->second(client, args);
            return;
        }

        // GUI commands are handled by the GUI protocol (person 3)
        // For now just log unknown GUI commands
        if (client.getType() == ClientType::GUI) {
            std::cout << "[CommandHandler] GUI fd=" << client.getFd()
                      << " cmd=\"" << cmd << "\"\n";
            return;
        }
    }

    // -------------------------------------------------------------------------
    // Private — auth
    // -------------------------------------------------------------------------

    void CommandHandler::_handleAuth(Client &client, const std::string &teamName)
    {
        if (teamName == "GRAPHIC") {
            client.setType(ClientType::GUI);
            client.setAuthenticated(true);
            std::cout << "[CommandHandler] fd=" << client.getFd()
                      << " authenticated as GUI\n";
            // TODO: send msz, sgt, mct, etc. (GUI protocol handshake — person 3)
            return;
        }

        // TODO: validate teamName against Game::getTeams() (person 2)
        // For now accept any team name
        client.setType(ClientType::AI);
        client.setAuthenticated(true);
        std::cout << "[CommandHandler] fd=" << client.getFd()
                  << " authenticated as AI team=\"" << teamName << "\"\n";

        // TODO: send CLIENT-NUM\n then X Y\n (person 2 provides these values)
        client.pushToWriteBuffer("0\n");        // placeholder CLIENT-NUM
        client.pushToWriteBuffer("10 10\n");    // placeholder world size
    }

    // -------------------------------------------------------------------------
    // Private — AI command stubs
    // All stubs send "ko" until Game is wired in (person 2)
    // -------------------------------------------------------------------------

    void CommandHandler::_handleForward(Client &client, const std::string &)
    {
        // TODO: Game::moveForward(client) — takes 7/f seconds
        std::cout << "[CommandHandler] fd=" << client.getFd() << " Forward\n";
        client.pushToWriteBuffer("ok\n");
    }

    void CommandHandler::_handleRight(Client &client, const std::string &)
    {
        // TODO: Game::turnRight(client) — takes 7/f seconds
        std::cout << "[CommandHandler] fd=" << client.getFd() << " Right\n";
        client.pushToWriteBuffer("ok\n");
    }

    void CommandHandler::_handleLeft(Client &client, const std::string &)
    {
        // TODO: Game::turnLeft(client) — takes 7/f seconds
        std::cout << "[CommandHandler] fd=" << client.getFd() << " Left\n";
        client.pushToWriteBuffer("ok\n");
    }

    void CommandHandler::_handleLook(Client &client, const std::string &)
    {
        // TODO: Game::look(client) — takes 7/f seconds, returns tile contents
        std::cout << "[CommandHandler] fd=" << client.getFd() << " Look\n";
        client.pushToWriteBuffer("[]\n");
    }

    void CommandHandler::_handleInventory(Client &client, const std::string &)
    {
        // TODO: Game::inventory(client) — takes 1/f seconds
        std::cout << "[CommandHandler] fd=" << client.getFd() << " Inventory\n";
        client.pushToWriteBuffer("[food 0, linemate 0, deraumere 0, sibur 0, mendiane 0, phiras 0, thystame 0]\n");
    }

    void CommandHandler::_handleBroadcast(Client &client, const std::string &args)
    {
        // TODO: Game::broadcast(client, args) — takes 7/f seconds, sends to all clients
        std::cout << "[CommandHandler] fd=" << client.getFd()
                  << " Broadcast \"" << args << "\"\n";
        client.pushToWriteBuffer("ok\n");
    }

    void CommandHandler::_handleConnectNbr(Client &client, const std::string &)
    {
        // TODO: Game::connectNbr(client) — immediate, returns free slots
        std::cout << "[CommandHandler] fd=" << client.getFd() << " Connect_nbr\n";
        client.pushToWriteBuffer("0\n");
    }

    void CommandHandler::_handleFork(Client &client, const std::string &)
    {
        // TODO: Game::fork(client) — takes 42/f seconds, lays an egg
        std::cout << "[CommandHandler] fd=" << client.getFd() << " Fork\n";
        client.pushToWriteBuffer("ok\n");
    }

    void CommandHandler::_handleEject(Client &client, const std::string &)
    {
        // TODO: Game::eject(client) — takes 7/f seconds, pushes others off tile
        std::cout << "[CommandHandler] fd=" << client.getFd() << " Eject\n";
        client.pushToWriteBuffer("ok\n");
    }

    void CommandHandler::_handleIncantation(Client &client, const std::string &)
    {
        // TODO: Game::incantation(client) — takes 300/f seconds
        std::cout << "[CommandHandler] fd=" << client.getFd() << " Incantation\n";
        client.pushToWriteBuffer("ko\n");
    }

    void CommandHandler::_handleTake(Client &client, const std::string &args)
    {
        // TODO: Game::take(client, args) — takes 7/f seconds
        std::cout << "[CommandHandler] fd=" << client.getFd()
                  << " Take \"" << args << "\"\n";
        client.pushToWriteBuffer("ok\n");
    }

    void CommandHandler::_handleSet(Client &client, const std::string &args)
    {
        // TODO: Game::set(client, args) — takes 7/f seconds
        std::cout << "[CommandHandler] fd=" << client.getFd()
                  << " Set \"" << args << "\"\n";
        client.pushToWriteBuffer("ok\n");
    }

    // -------------------------------------------------------------------------
    // Private — helpers
    // -------------------------------------------------------------------------

    void CommandHandler::_sendKo(Client &client)
    {
        client.pushToWriteBuffer("ko\n");
    }

} // namespace Zappy
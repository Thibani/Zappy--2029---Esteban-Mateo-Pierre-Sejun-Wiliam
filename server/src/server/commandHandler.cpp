/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** CommandHandler.cpp
*/
#include "server/commandHandler.hpp"
#include "protocol/guiProtocol.hpp"
#include "exceptions/serverException.hpp"
#include "game/game.hpp"

#include <iostream>
#include <sstream>
#include <algorithm>

namespace Zappy {

    // -------------------------------------------------------------------------
    // Ctor
    // -------------------------------------------------------------------------

    CommandHandler::CommandHandler(Game &game, GUIProtocol &guiProtocol)
        : _game(game), _guiProtocol(guiProtocol), _guiCmdHandler(guiProtocol, game)
    {
        // Build dispatch table — keys are lowercase for case-insensitive matching
        _aiHandlers["forward"]     = [this](const Client &c, const std::string &a) { _handleForward(c, a);     };
        _aiHandlers["right"]       = [this](const Client &c, const std::string &a) { _handleRight(c, a);       };
        _aiHandlers["left"]        = [this](const Client &c, const std::string &a) { _handleLeft(c, a);        };
        _aiHandlers["look"]        = [this](const Client &c, const std::string &a) { _handleLook(c, a);        };
        _aiHandlers["inventory"]   = [this](const Client &c, const std::string &a) { _handleInventory(c, a);   };
        _aiHandlers["broadcast"]   = [this](const Client &c, const std::string &a) { _handleBroadcast(c, a);   };
        _aiHandlers["connect_nbr"] = [this](const Client &c, const std::string &a) { _handleConnectNbr(c, a);  };
        _aiHandlers["fork"]        = [this](const Client &c, const std::string &a) { _handleFork(c, a);        };
        _aiHandlers["eject"]       = [this](const Client &c, const std::string &a) { _handleEject(c, a);       };
        _aiHandlers["incantation"] = [this](Client &c, const std::string &a) { _handleIncantation(c, a); };
        _aiHandlers["take"]        = [this](const Client &c, const std::string &a) { _handleTake(c, a);        };
        _aiHandlers["set"]         = [this](const Client &c, const std::string &a) { _handleSet(c, a);         };
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

        if (client.getType() == ClientType::GUI) {
            std::cout << "[CommandHandler] GUI fd=" << client.getFd()
                      << " cmd=\"" << cmd << "\" args=\"" << args <<  "\"\n";
            _guiCmdHandler.handle(client, cmd, args);
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
            _guiProtocol.sendInitialState(client.getFd());
            return;
        }

        auto teams = _game.getTeams();
        if (std::find(teams.begin(), teams.end(), teamName) == teams.end()) {
            std::cout << "[CommandHandler] fd=" << client.getFd()
                      << " AI auth rejected: unknown team \"" << teamName << "\"\n";
            client.pushToWriteBuffer("ko\n");
            return;
        }
        int freeSlots = _game.getTeamNbEggs(teamName);
        if (freeSlots <= 0) {
            std::cout << "[CommandHandler] fd=" << client.getFd()
                      << " AI auth rejected: no free slots in team \"" << teamName << "\"\n";
            client.pushToWriteBuffer("ko\n");
            return;
        }
        int playerId = client.getFd();
        _game.eggHatching(playerId, teamName);
        client.setType(ClientType::AI);
        client.setAuthenticated(true);
        std::cout << "[CommandHandler] fd=" << client.getFd()
                  << " authenticated as AI team=\"" << teamName
                  << "\" playerId=" << playerId << "\n";
        client.pushToWriteBuffer(std::to_string(freeSlots - 1) + "\n");
        auto [w, h] = _game.getMapSize();
        client.pushToWriteBuffer(std::to_string(w) + " " + std::to_string(h) + "\n");
    }

    // -------------------------------------------------------------------------
    // Private — AI command stubs
    // All stubs send "ko" until Game is wired in (person 2)
    // -------------------------------------------------------------------------

    void CommandHandler::_handleForward(const Client &client, const std::string &)
    {
        _game.addClientAction(client.getFd(), Zappy::Game::ActionType::FORWARD, "");
        // TODO: Game::moveForward(client) — takes 7/f seconds
        // std::cout << "[CommandHandler] fd=" << client.getFd() << " Forward\n";
        // _game.moveForward(client.getFd());
        // client.pushToWriteBuffer("ok\n");
    }

    void CommandHandler::_handleRight(const Client &client, const std::string &)
    {
        _game.addClientAction(client.getFd(), Zappy::Game::ActionType::RIGHT, "");
        // TODO: Game::turnRight(client) — takes 7/f seconds
        // std::cout << "[CommandHandler] fd=" << client.getFd() << " Right\n";
        // _game.turnRight(client.getFd());
        // client.pushToWriteBuffer("ok\n");
    }

    void CommandHandler::_handleLeft(const Client &client, const std::string &)
    {
        _game.addClientAction(client.getFd(), Zappy::Game::ActionType::LEFT, "");
        // TODO: Game::turnLeft(client) — takes 7/f seconds
        // std::cout << "[CommandHandler] fd=" << client.getFd() << " Left\n";
        // _game.turnLeft(client.getFd());
        // client.pushToWriteBuffer("ok\n");
    }

    void CommandHandler::_handleLook(const Client &client, const std::string &)
    {
        _game.addClientAction(client.getFd(), Zappy::Game::ActionType::LOOK, "");
        // std::cout << "[CommandHandler] fd=" << client.getFd() << " Look\n";
        // std::string result = _game.look(client.getFd());
        // if (result.empty() || result.back() != '\n')
        //     result += "\n";
        // client.pushToWriteBuffer(result);
    }

    void CommandHandler::_handleInventory(const Client &client, const std::string &)
    {
        _game.addClientAction(client.getFd(), Zappy::Game::ActionType::INVENTORY, "");
        // std::cout << "[CommandHandler] fd=" << client.getFd() << " Inventory\n";
        // std::string result = _game.inventory(client.getFd());
        // if (result.empty() || result.back() != '\n')
        //     result += "\n";
        // client.pushToWriteBuffer(result);
    }

    void CommandHandler::_handleBroadcast(const Client &client, const std::string &args)
    {
        _game.addClientAction(client.getFd(), Zappy::Game::ActionType::BROADCAST, args);
        // std::cout << "[CommandHandler] fd=" << client.getFd()
        //           << " Broadcast \"" << args << "\"\n";
        // // TODO
        // client.pushToWriteBuffer("ok\n");
    }

    void CommandHandler::_handleConnectNbr(const Client &client, const std::string &)
    {
        _game.addClientAction(client.getFd(), Zappy::Game::ActionType::CONNECT_NBR, "");
        // std::cout << "[CommandHandler] fd=" << client.getFd() << " Connect_nbr\n";
        // int n = _game.connectNbr(client.getFd());
        // client.pushToWriteBuffer(std::to_string(n) + "\n");
    }

    void CommandHandler::_handleFork(const Client &client, const std::string &)
    {
        _game.addClientAction(client.getFd(), Zappy::Game::ActionType::FORK, "");
        // std::cout << "[CommandHandler] fd=" << client.getFd() << " Fork\n";
        // _game.fork(client.getFd());
        // client.pushToWriteBuffer("ok\n");
    }

    void CommandHandler::_handleEject(const Client &client, const std::string &)
    {
        _game.addClientAction(client.getFd(), Zappy::Game::ActionType::EJECT, "");
        // std::cout << "[CommandHandler] fd=" << client.getFd() << " Eject\n";
        // _game.eject(client.getFd());
        // client.pushToWriteBuffer("ok\n");
    }

    void CommandHandler::_handleIncantation(Client &client, const std::string &)
    {
        std::string result = _game.incantationStart(client.getFd());
        client.pushToWriteBuffer(result);
    }

    void CommandHandler::_handleTake(const Client &client, const std::string &args)
    {
        _game.addClientAction(client.getFd(), Zappy::Game::ActionType::TAKE, args);
        // TODO: Game::take(client, args) — takes 7/f seconds
        // std::cout << "[CommandHandler] fd=" << client.getFd()
        //           << " Take \"" << args << "\"\n";
        // bool ok = _game.take(client.getFd(), args);
        // client.pushToWriteBuffer(ok ? "ok\n" : "ko\n");
    }

    void CommandHandler::_handleSet(const Client &client, const std::string &args)
    {
        _game.addClientAction(client.getFd(), Zappy::Game::ActionType::SET, args);
        // TODO: Game::set(client, args) — takes 7/f seconds
        // std::cout << "[CommandHandler] fd=" << client.getFd()
        //           << " Set \"" << args << "\"\n";
        // bool ok = _game.set(client.getFd(), args);
        // client.pushToWriteBuffer(ok ? "ok\n" : "ko\n");
    }

    // -------------------------------------------------------------------------
    // Private — helpers
    // -------------------------------------------------------------------------

    void CommandHandler::_sendKo(Client &client)
    {
        client.pushToWriteBuffer("ko\n");
    }

} // namespace Zappy
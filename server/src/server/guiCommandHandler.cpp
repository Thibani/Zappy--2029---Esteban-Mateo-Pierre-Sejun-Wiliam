/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** guiCommandHandler.cpp - Implementation of GUI command parsing.
*/

#include "server/guiCommandHandler.hpp"
#include "protocol/guiProtocol.hpp"

#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdlib>

namespace Zappy {
    // ----------------------------------------------------------------------
    // Placeholder constants — remove once Sejun's Game is wired in.
    // ----------------------------------------------------------------------
    namespace {
        constexpr int kPlaceholderMapWidth  = 10;
        constexpr int kPlaceholderMapHeight = 10;
        constexpr int kPlaceholderFrequency = 100;
    }

    // ----------------------------------------------------------------------
    // Ctor
    // ----------------------------------------------------------------------
    GUICommandHandler::GUICommandHandler(GUIProtocol &guiProtocol)
        : _guiProtocol(guiProtocol)
    {
        _handlers["msz"] = [this](Client &c, const std::string &a) { _handleMsz(c, a); };
        _handlers["bct"] = [this](Client &c, const std::string &a) { _handleBct(c, a); };
        _handlers["mct"] = [this](Client &c, const std::string &a) { _handleMct(c, a); };
        _handlers["tna"] = [this](Client &c, const std::string &a) { _handleTna(c, a); };
        _handlers["ppo"] = [this](Client &c, const std::string &a) { _handlePpo(c, a); };
        _handlers["plv"] = [this](Client &c, const std::string &a) { _handlePlv(c, a); };
        _handlers["pin"] = [this](Client &c, const std::string &a) { _handlePin(c, a); };
        _handlers["sgt"] = [this](Client &c, const std::string &a) { _handleSgt(c, a); };
        _handlers["sst"] = [this](Client &c, const std::string &a) { _handleSst(c, a); };
    }

    // ----------------------------------------------------------------------
    // Public — entry point
    // ----------------------------------------------------------------------
    void GUICommandHandler::handle(Client &client, const std::string &cmd, const std::string &args)
    {
        auto it = _handlers.find(cmd);
        if (it == _handlers.end()) {
            _guiProtocol.emitUnknownCommand(client.getFd());
            return;
        }
        it->second(client, args);
    }

    // ----------------------------------------------------------------------
    // Per-command handlers
    // ----------------------------------------------------------------------
    void GUICommandHandler::_handleMsz(Client &client, const std::string &args)
    {
        if (!args.empty()) {
            _guiProtocol.emitBadParameters(client.getFd());
            return;
        }
        _guiProtocol.emitMapSize(kPlaceholderMapWidth, kPlaceholderMapHeight, client.getFd());
    }

    void GUICommandHandler::_handleBct(Client &client, const std::string &args)
    {

    }

    void GUICommandHandler::_handleMct(Client &client, const std::string &args)
    {

    }

    void GUICommandHandler::_handleTna(Client &client, const std::string &args)
    {

    }

    void GUICommandHandler::_handlePpo(Client &client, const std::string &args)
    {

    }

    void GUICommandHandler::_handlePlv(Client &client, const std::string &args)
    {

    }

    void GUICommandHandler::_handlePin(Client &client, const std::string &args)
    {

    }

    void GUICommandHandler::_handleSgt(Client &client, const std::string &args)
    {

    }

    void GUICommandHandler::_handleSst(Client &client, const std::string &args)
    {

    }

    bool GUICommandHandler::parseInts(const std::string &args, std::vector<int> &out, std::size_t expected)
    {
        out.clear();
        std::istringstream iss(args);
        int n = 0;
        while (iss >> n)
            out.push_back(n);
        if (!iss.eof()) {
            iss.clear();
            std::string leftover;
            iss >> leftover;
            if (!leftover.empty())
                return false;
        }
        return out.size() == expected;
    }

    bool GUICommandHandler::parsePlayerId(const std::string &arg, int &out)
    {
        if (arg.size() < 2 || arg[0] != '#')
            return false;
        for (size_t i = 1; i < arg.size(); i++) {
            if (!std::isdigit(static_cast<unsigned char>(arg[i])))
                return false;
        }
        try {
            out = std::stoi(arg.substr(1));
        } catch (const std::exception &) {
            return false;
        }
        return out >= 0;
    }

} // namespace Zappy
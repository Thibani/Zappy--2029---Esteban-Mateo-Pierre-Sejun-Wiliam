/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** guiCommandHandler.cpp - Implementation of GUI command parsing.
*/

#include "server/guiCommandHandler.hpp"
#include "protocol/guiProtocol.hpp"
#include "game/game.hpp"
#include "map/tile.hpp"
#include "player/player.hpp"

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
    GUICommandHandler::GUICommandHandler(GUIProtocol &guiProtocol, Game &game)
        : _guiProtocol(guiProtocol), _game(game)
    {
        _handlers["msz"] = [this](const Client &c, const std::string &a) { _handleMsz(c, a); };
        _handlers["bct"] = [this](const Client &c, const std::string &a) { _handleBct(c, a); };
        _handlers["mct"] = [this](const Client &c, const std::string &a) { _handleMct(c, a); };
        _handlers["tna"] = [this](const Client &c, const std::string &a) { _handleTna(c, a); };
        _handlers["ppo"] = [this](const Client &c, const std::string &a) { _handlePpo(c, a); };
        _handlers["plv"] = [this](const Client &c, const std::string &a) { _handlePlv(c, a); };
        _handlers["pin"] = [this](const Client &c, const std::string &a) { _handlePin(c, a); };
        _handlers["sgt"] = [this](const Client &c, const std::string &a) { _handleSgt(c, a); };
        _handlers["sst"] = [this](const Client &c, const std::string &a) { _handleSst(c, a); };
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

    // "msz" → "msz W H\n"
    void GUICommandHandler::_handleMsz(const Client &client, const std::string &args)
    {
        if (!args.empty()) {
            _guiProtocol.emitBadParameters(client.getFd());
            return;
        }
        _guiProtocol.emitMapSize(kPlaceholderMapWidth, kPlaceholderMapHeight, client.getFd());
    }

    // "bct X Y" → "bct X Y q0..q6\n"
    void GUICommandHandler::_handleBct(const Client &client, const std::string &args)
    {
        std::vector<int> nums;
        if (!parseInts(args, nums, 2)) {
            _guiProtocol.emitBadParameters(client.getFd());
            return;
        }
        const int x = nums[0];
        const int y = nums[1];
        if (x < 0 || x >= kPlaceholderMapWidth || y < 0 || y >= kPlaceholderMapHeight) {
            _guiProtocol.emitBadParameters(client.getFd());
            return;
        }
        Inventory emptyTile; // TODO
        _guiProtocol.emitTileContent(x, y, emptyTile, client.getFd());
    }

    // "mct" → 100 lines of "bct X Y 0 0 0 0 0 0 0\n"
    void GUICommandHandler::_handleMct(const Client &client, const std::string &args)
    {
        if (!args.empty()) {
            _guiProtocol.emitBadParameters(client.getFd());
            return;
        }
        Inventory emptyTile; // TODO
        for (int y = 0; y < kPlaceholderMapHeight; y++) {
            for (int x = 0; x < kPlaceholderMapWidth; x++) 
                _guiProtocol.emitTileContent(x, y, emptyTile, client.getFd());
        }
    }

    // "tna" → "tna NAME\n" per team
    void GUICommandHandler::_handleTna(const Client &client, const std::string &args)
    {
        if (!args.empty()) {
            _guiProtocol.emitBadParameters(client.getFd());
            return;
        }
        // TODO: iterate over Game::teams() once Sejun's PR lands.
        // For now emit a single placeholder team name to keep clients happy.
        _guiProtocol.emitTeamName("team1", client.getFd());
    }

    // "ppo #n" → sbp (no players exist yet)
    void GUICommandHandler::_handlePpo(const Client &client, const std::string &args)
    {
        int playerId = 0;
        if (!parsePlayerId(args, playerId)) {
            _guiProtocol.emitBadParameters(client.getFd());
            return;
        }
         // TODO: lookup player by id in Game (Sejun). Until then any id
        // is invalid because no players exist.
        _guiProtocol.emitBadParameters(client.getFd());
    }

    // "plv #n" → sbp (no players exist yet)
    void GUICommandHandler::_handlePlv(const Client &client, const std::string &args)
    {
        int playerId = 0;
        if (!parsePlayerId(args, playerId)) {
            _guiProtocol.emitBadParameters(client.getFd());
            return;
        }
        // TODO: lookup player by id in Game (Sejun).
        _guiProtocol.emitBadParameters(client.getFd());
    }

    // "pin #n" → sbp (no players exist yet)
    void GUICommandHandler::_handlePin(const Client &client, const std::string &args)
    {
        int playerId = 0;
        if (!parsePlayerId(args, playerId)) {
            _guiProtocol.emitBadParameters(client.getFd());
            return;
        }
        // TODO: lookup player by id in Game (Sejun).
        _guiProtocol.emitBadParameters(client.getFd());
    }

    // "sgt" → "sgt T\n"
    void GUICommandHandler::_handleSgt(const Client &client, const std::string &args)
    {
        if (!args.empty()) {
            _guiProtocol.emitBadParameters(client.getFd());
            return;
        }
        _guiProtocol.emitTimeUnit(kPlaceholderFrequency, client.getFd());
    }

    // "sst T" → broadcast "sst T\n" to all GUIs (per spec) + apply
    void GUICommandHandler::_handleSst(const Client &client, const std::string &args)
    {
        std::vector<int> nums;
        if (!parseInts(args, nums, 1) || nums[0] <= 0) {
            _guiProtocol.emitBadParameters(client.getFd());
            return;
        }
        const int newFrequency = nums[0];
        // TODO: update Clock::frequency once Game owns it (Sejun).
        // The protocol-level effect (broadcast sst T) is what GUIs care
        // about — onTimeUnitChanged sends sst to ALL connected GUIs.
        _guiProtocol.onTimeUnitChanged(newFrequency);
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
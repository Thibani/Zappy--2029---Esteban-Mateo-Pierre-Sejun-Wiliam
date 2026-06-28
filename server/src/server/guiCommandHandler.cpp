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
        auto [w, h] = _game.getMapSize();
        _guiProtocol.emitMapSize(w, h, client.getFd());
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
        auto [w, h] = _game.getMapSize();
        if (x < 0 || x >= w || y < 0 || y >= h) {
            _guiProtocol.emitBadParameters(client.getFd());
            return;
        }
        Pos p{x, y};
        const Tile *tile = _game.map->getTile(p);
        Inventory inv;
        if (tile) {
            const auto &raw = tile->resources();
            for (int i = 0; i < 7; i++)
                inv.set(static_cast<TypeResource>(i), raw[i]);
        }
        _guiProtocol.emitTileContent(x, y, inv, client.getFd());
    }

    // "mct" → 100 lines of "bct X Y 0 0 0 0 0 0 0\n"
    void GUICommandHandler::_handleMct(const Client &client, const std::string &args)
    {
        if (!args.empty()) {
            _guiProtocol.emitBadParameters(client.getFd());
            return;
        }
        auto [w, h] = _game.getMapSize();
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                Pos p{x, y};
                const Tile *tile = _game.map->getTile(p);
                Inventory inv;
                if (tile) {
                    const auto &raw = tile->resources();
                    for (int i = 0; i < 7; i++)
                        inv.set(static_cast<TypeResource>(i), raw[i]);
                }
                _guiProtocol.emitTileContent(x, y, inv, client.getFd());
            }
        }
    }

    // "tna" → "tna NAME\n" per team
    void GUICommandHandler::_handleTna(const Client &client, const std::string &args)
    {
        if (!args.empty()) {
            _guiProtocol.emitBadParameters(client.getFd());
            return;
        }
        for (const auto &teamName : _game.getTeams())
            _guiProtocol.emitTeamName(teamName, client.getFd());
    }

    // "ppo #n" → sbp (no players exist yet)
    void GUICommandHandler::_handlePpo(const Client &client, const std::string &args)
    {
        int playerId = 0;
        if (!parsePlayerId(args, playerId)) {
            _guiProtocol.emitBadParameters(client.getFd());
            return;
        }
        auto players = _game.getPlayers();
        auto it = players.find(playerId);
        if (it == players.end()) {
            _guiProtocol.emitBadParameters(client.getFd());
            return;
        }
        const Player *p = it->second;
        Pos pos = p->getPosition();
        _guiProtocol.emitPlayerPosition(playerId, pos.x, pos.y, p->getDirection(), client.getFd());
    }

    // "plv #n" → sbp (no players exist yet)
    void GUICommandHandler::_handlePlv(const Client &client, const std::string &args)
    {
        int playerId = 0;
        if (!parsePlayerId(args, playerId)) {
            _guiProtocol.emitBadParameters(client.getFd());
            return;
        }
        auto players = _game.getPlayers();
        auto it = players.find(playerId);
        if (it == players.end()) {
            _guiProtocol.emitBadParameters(client.getFd());
            return;
        }
        const Player *p = it->second;
        _guiProtocol.emitPlayerLevel(playerId, p->getLevel(), client.getFd());
    }

    // "pin #n" → sbp (no players exist yet)
    void GUICommandHandler::_handlePin(const Client &client, const std::string &args)
    {
        int playerId = 0;
        if (!parsePlayerId(args, playerId)) {
            _guiProtocol.emitBadParameters(client.getFd());
            return;
        }
        auto players = _game.getPlayers();
        auto it = players.find(playerId);
        if (it == players.end()) {
            _guiProtocol.emitBadParameters(client.getFd());
            return;
        }
        const Player *p = it->second;
        Pos pos = p->getPosition();
        const auto &raw = p->getInventory();
        Inventory inv;
        for (int i = 0; i < 7; i++)
            inv.set(static_cast<TypeResource>(i), raw[i]);
        _guiProtocol.emitPlayerInventory(playerId, pos.x, pos.y, inv, client.getFd());
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
        if (std::any_of(arg.begin() + 1, arg.end(),
            [](unsigned char ch) { return !std::isdigit(ch); }))
            return false;
        try {
            out = std::stoi(arg.substr(1));
        } catch (const std::exception &) {
            return false;
        }
        return out >= 0;
    }

} // namespace Zappy
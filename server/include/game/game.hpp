#pragma once

#include "map/map.hpp"
#include "events/gameEvents.hpp"

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <chrono>

namespace Zappy {

    class Team;
    class Player;
    class Egg;
    class Tile;

    using TimePoint = std::chrono::steady_clock::time_point;


    class Game {

        public:

            enum ActionType {
            FORWARD,
            RIGHT,
            LEFT,
            LOOK,
            BROADCAST,
            TAKE,
            SET,
            EJECT,
            INVENTORY,
            CONNECT_NBR,
            FORK,
            INCANTATION,
            EAT
        };

            Game();
            ~Game();
            Map* map;
            void initialize(int freq, int mapWidth, int mapHeight, std::vector<std::string> teamNames, int nbEggs);
            void initActionCost();
            bool eggHatching(int clientId, const std::string teamName);
            std::string moveForward(int clientId);
            std::string turnRight(int clientId);
            std::string turnLeft(int clientId);
            std::string look(int clientId);
            std::string inventory(int clientId);
            std::string connectNbr(int clientId);
            std::string fork(int clientId);
            std::string eject(int clientId);
            std::string incantation(int clientId);
            std::string take(int clientId, const std::string obj);
            std::string set(int clientId, const std::string obj);
            bool eat(int clientId);
            void removePlayer(int clientId);
            bool checkLevelUp(int level, Tile* tile);
            std::vector<std::string> getTeams();
            std::pair<int, int> getMapSize();
            int getTeamNbEggs(const std::string teamName);
            int getMapWidth() { return map->getWidth(); }
            int getMapHeight() { return map->getHeight(); }
            std::map<int, Player*> getPlayers() { return _idPlayers; }
            // void setListener(IGameEventListener *_listener) { _listener = _listener; }
            void setListener(IGameEventListener *listener) {
                _listener = listener;
                std::cout << "[Game::setListener] called listener=" << (listener ? "non-null" : "null") << "\n";
            }
            const std::vector<Team*> &getTeamObjects() const { return _teams; }
            bool hasIdAction(int clientId);
            void addClientAction(int clientId, ActionType actionType, std::string arg);
            std::vector<std::pair<int, std::string>> executeAllClientActions();

        private:
            struct Action {
                ActionType actionType;
                std::string arg;
                TimePoint deadLine;
            };

            int _freq;
            std::vector<Team*> _teams;
            std::map<int, Player*> _idPlayers;
            std::map<int, Action> _idActions;
            std::map<ActionType, int> _actionCosts;
            std::vector<Player*> _players;
            IGameEventListener *_listener = nullptr;
            Team *getTeam(const std::string teamName);
            bool hasIdPlayer(int clientId);
            void addPlayer(Player* player);
    };
}
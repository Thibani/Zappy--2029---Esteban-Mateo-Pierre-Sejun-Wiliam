#pragma once

#include "map/map.hpp"
#include "events/gameEvents.hpp"
#include "player/player.hpp"
#include "types/position.hpp"

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

            Game(const Game &) = delete;
            Game &operator=(const Game &) = delete;

            Map* map;
            void initialize(int freq, int mapWidth, int mapHeight, const std::vector<std::string> &teamNames, int nbEggs);
            void initActionCost();
            bool eggHatching(int clientId, const std::string &teamName);
            std::string moveForward(int clientId);
            std::string turnRight(int clientId);
            std::string turnLeft(int clientId);
            std::string look(int clientId);
            std::string inventory(int clientId);
            std::vector<std::pair<int, std::string>> broadcast(int clientId, const std::string &obj);
            std::string connectNbr(int clientId);
            std::string fork(int clientId);
            std::string eject(int clientId);
            std::string incantationStart(int clientId);
            std::string incantationEnd(int clientId);
            std::string take(int clientId, const std::string &obj);
            std::string set(int clientId, const std::string &obj);
            bool eat(int clientId);
            void removePlayer(int clientId);
            static bool checkLevelUp(int level, Tile* tile);
            std::vector<std::string> getTeams();
            std::pair<int, int> getMapSize();
            int getTeamNbEggs(const std::string &teamName);
            int getMapWidth() { return map->getWidth(); }
            int getMapHeight() { return map->getHeight(); }
            const std::map<int, Player*> &getPlayers() const { return _idPlayers; }
            void setListener(IGameEventListener *listener) { _listener = listener; }
            const std::vector<Team*> &getTeamObjects() const { return _teams; }
            bool hasIdAction(int clientId);
            void addClientAction(int clientId, ActionType actionType, const std::string &arg);
            void addClientEatAction(int clientId);
            std::vector<std::pair<int, std::string>> executeAllClientActions();
            std::vector<std::pair<int, std::string>> executeAllEatActions();
            void executeSpawnResources();
            void checkWinCondition();
            int nearestDeadlineMs();

        private:
            struct Action {
                ActionType actionType;
                std::string arg;
                TimePoint deadLine;
            };

            int _freq;
            bool _isVictory;
            std::vector<Team*> _teams;
            std::map<int, Player*> _idPlayers;
            std::map<int, Action> _idActions;
            std::map<ActionType, int> _actionCosts;
            std::map<int, Action> _idEatActions;
            IGameEventListener *_listener = nullptr;
            Action _spawnResourceDeadline;
            Team *getTeam(const std::string &teamName);
            bool hasIdPlayer(int clientId);
            void setNextResourcesDeadline();
            int computeDirection(Pos emitter, const Player *receiver);
            static Pos shortestVector(Pos emitter, Pos receiver, int width, int height);
            static Pos rotate(Pos v, Direction d);
            void addPlayersToRitual(Player *player, int ritualId);
            void removePlayersFromRitual(int ritualId, Pos position);
    };
}
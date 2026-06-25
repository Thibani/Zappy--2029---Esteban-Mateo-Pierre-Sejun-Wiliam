#pragma once

#include "map/map.hpp"

#include <vector>
#include <map>
#include <string>

namespace Zappy {

    class Team;
    class Player;
    class Egg;
    class Tile;

    class Game {

        public:
            Game();
            ~Game();
            Map* map;
            void initialize(int mapWidth, int mapHeight, std::vector<std::string> teamNames, int nbEggs);
            bool eggHatching(int clientId, const std::string teamName);
            void moveForward(int clientId);
            void turnRight(int clientId);
            void turnLeft(int clientId);
            std::string look(int clientId);
            std::string inventory(int clientId);
            int connectNbr(int clientId);
            int fork(int clientId);
            void eject(int clientId);
            bool incantation(int clientId);
            bool take(int clientId, const std::string obj);
            bool set(int clientId, const std::string obj);
            bool eat(int clientId);
            void removePlayer(int clientId);
            bool checkLevelUp(int level, Tile* tile);
            std::vector<std::string> getTeams();
            std::pair<int, int> getMapSize();
            int getTeamNbEggs(const std::string teamName);
            int getMapWidth() { return map->getWidth(); }
            int getMapHeight() { return map->getHeight(); }
            std::map<int, Player*> getPlayers() { return _idPlayers; }

        private:
            std::vector<Team*> _teams;
            std::map<int, Player*> _idPlayers;
            std::vector<Player*> _players;

            Team *getTeam(const std::string teamName);
            bool hasIdPlayer(int clientId);
            void addPlayer(Player* player);
    };
}
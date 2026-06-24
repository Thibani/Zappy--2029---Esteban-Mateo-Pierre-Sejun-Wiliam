#pragma once
#include "Map.hpp"
#include "Egg.hpp"
#include <vector>
#include <map>

namespace Zappy {

    class Team;
    class Map;

    class Game {

        public:
            Game();
            ~Game();
            Map* map;
            void initialize(int mapWidth, int mapHeight, std::vector<std::string> teamNames, int nbEggs);
            std::vector<std::string> getTeams();
            std::pair<int, int> getMapSize();
            int getTeamNbEggs(std::string teamName);
            void addPlayer(Player* player);
            void moveForward(int clientId);
            void turnRight(int clientId);
            void turnLeft(int clientId);
            void look(int clientId);
            void inventory(int clientId);
            // void broadcast(int clientId, args);
            void connectNbr(int clientId);
            void fork(int clientId);
            void eject(int clientId);
            void incantation(int clientId);
            void take(int clientId, const std::string obj);
            void set(int clientId, const std::string obj);

            // void createTeam(std::string teamName);

        private:
            std::vector<Team*> _teams;
            std::map<int, Player*> _idPlayers;
            std::vector<Egg*> _eggs;
            std::vector<Player*> _players;
    };
}
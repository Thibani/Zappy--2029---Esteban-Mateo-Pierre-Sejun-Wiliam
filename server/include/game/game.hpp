#pragma once
#include <vector>
#include <map>
#include <string>

namespace Zappy {

    class Team;
    class Map;
    class Player;
    class Egg;

    class Game {

        public:
            Game();
            ~Game();
            Map* map;
            void initialize(int mapWidth, int mapHeight, std::vector<std::string> teamNames, int nbEggs);
            void eggHatching(int clientId, const std::string teamName);
            void moveForward(int clientId);
            void turnRight(int clientId);
            void turnLeft(int clientId);
            std::string look(int clientId);
            std::string inventory(int clientId);
            // void broadcast(int clientId, args);
            int connectNbr(int clientId);
            int fork(int clientId);
            void eject(int clientId);
            void incantation(int clientId);
            bool take(int clientId, const std::string obj);
            bool set(int clientId, const std::string obj);
            bool eat(int clientId);
            void removePlayer(int clientId);

        private:
            std::vector<Team*> _teams;
            std::map<int, Player*> _idPlayers;
            std::vector<Player*> _players;

            Team *getTeam(const std::string teamName);
            bool hasIdPlayer(int clientId);
            std::vector<std::string> getTeams();
            std::pair<int, int> getMapSize();
            int getTeamNbEggs(const std::string teamName);
            void addPlayer(Player* player);
    };
}
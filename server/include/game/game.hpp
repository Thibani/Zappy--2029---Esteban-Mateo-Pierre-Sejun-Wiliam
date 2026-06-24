#pragma once
#include "map/map.hpp"
#include <vector>


namespace Zappy {

    class Team;
    class Map;

    class Game {

        public:
            Game(int mapHeight, int mapWidth);
            Map map;
            // void createTeam(std::string teamName);

        private:
            std::vector<Team*> _teams;
    };
}
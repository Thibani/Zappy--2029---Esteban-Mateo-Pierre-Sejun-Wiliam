#include "game/game.hpp"
#include "map/map.hpp"
#include "team/team.hpp"

namespace Zappy {

    Game::Game(int mapHeight, int mapWidth) : map(mapHeight, mapWidth)
    {

    }

    // void Game::createTeam(std::string teamName)
    // {
    //     Team team = Team("teamName");

    //     _teams.push_back(&team);
    // }
}
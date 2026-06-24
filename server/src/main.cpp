#include "GameLogic/Map.hpp"
#include "GameLogic/Player.hpp"
#include "GameLogic/Position.hpp"
#include "GameLogic/Tile.hpp"
#include <iostream>
#include "GameLogic/Game.hpp"

    int main()
    {
        std::vector<std::string> teams;
        teams.push_back("team rocket");
        //Zappy::Map map = Zappy::Map(5, 5);
        Zappy::Game game = Zappy::Game();
        game.initialize(5, 7, teams, 5);
        game.map->setRessource();
        game.map->debugDisplayMap();
        Zappy::Player* player = new Zappy::Player("caca");
        game.addPlayer(player);
        // player.moveForward(map);
        //player._level = 2;
        //auto temp = player.lookUp(map);
        // auto temp = player.lookDown(map);
        // auto temp = player.lookRight(map);
        // auto temp = player.lookLeft(map);
        auto temp = player->look((game.map));
        // for (int i = 0; i < (int)temp.size(); i++){
        //     std::cout << temp[i].x << ":" << temp[i].y << std::endl;
        // }
        std::vector<std::string> teamNames = game.getTeams();
        std::cout << temp << std::endl;
        std::cout << game.getTeamNbEggs("team rocket") << std::endl;
        std::cout << "x:" << game.getMapSize().first << "y:" << game.getMapSize().second << std::endl;
        for (uint i = 0; i < teamNames.size(); i++){
            std::cout << teamNames[i] << std::endl;
        }
        return 0;
    }

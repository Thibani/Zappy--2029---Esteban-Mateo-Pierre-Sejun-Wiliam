#include "GameLogic/Map.hpp"
#include "GameLogic/Player.hpp"
#include "GameLogic/Position.hpp"
#include "GameLogic/Tile.hpp"
#include <iostream>
#include "GameLogic/Game.hpp"

    int main()
    {
        //Zappy::Map map = Zappy::Map(5, 5);
        Zappy::Game game = Zappy::Game(5, 5);
        game.map.setRessource();
        game.map.debugDisplayMap();
        Zappy::Player player = Zappy::Player("caca");
        game.map.addPlayerOnTile(&player);

        // player.moveForward(map);
        //player._level = 2;
        //auto temp = player.lookUp(map);
        // auto temp = player.lookDown(map);
        // auto temp = player.lookRight(map);
        // auto temp = player.lookLeft(map);
        auto temp = player.look(game.map);
        // for (int i = 0; i < (int)temp.size(); i++){
        //     std::cout << temp[i].x << ":" << temp[i].y << std::endl;
        // }
        std::cout << temp << std::endl;
        return 0;
    }

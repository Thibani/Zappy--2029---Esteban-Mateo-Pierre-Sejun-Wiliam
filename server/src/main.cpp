/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** main.cpp - Entry point
*/
// #include "utils/args.hpp"
// #include "server/server.hpp"
#include "../include/game/game.hpp"
#include "../include/player/player.hpp"
#include "../include/map/map.hpp"
// #include "exceptions/serverException.hpp"

#include <iostream>
#include <csignal>

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

//     std::signal(SIGINT,  signalHandler);
//     std::signal(SIGTERM, signalHandler);

//     try {
//         Zappy::Game        game(args.height, args.width);
//         Zappy::Server server(args, game);
//         server.run();
//     } catch (const Zappy::ServerException &e) {
//         std::cerr << e.what() << "\n";
//         return 84;
//     } catch (const std::exception &e) {
//         std::cerr << "Unexpected error: " << e.what() << "\n";
//         return 84;
//     }

//     std::cout << "[Server] Shutdown complete.\n";
//     return 0;
// }

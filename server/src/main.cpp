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
        game.eggHatching(4, "team rocket");
        std::string output = game.inventory(4);
        if (game.incantation(4) == false)
        game.fork(4);
            printf("coucou\n");
        std::cout << output << std::endl;
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

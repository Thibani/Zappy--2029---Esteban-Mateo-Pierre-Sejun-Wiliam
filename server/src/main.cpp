/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** main.cpp - Entry point
*/

#include "utils/args.hpp"
#include "server/server.hpp"
#include "game/game.hpp"
#include "exceptions/serverException.hpp"

#include <iostream>
#include <csignal>

int main(int ac, char **av)
{
    try {
        Zappy::Args args = Zappy::ArgsParser::parse(ac, av);
        Zappy::Game game;
        game.initialize(args.width, args.height, args.teams, args.clientsNb);
        Zappy::Server server(args, game);
        server.run();
    } catch (const Zappy::ServerException &e) {
        std::cerr << e.what() << "\n";
        return 84;
    } catch (const std::exception &e) {
        std::cerr << "Unexpected error: " << e.what() << "\n";
        return 84;
    }
    std::cout << "[Server] Shutdown complete.\n";
    return 0;
}


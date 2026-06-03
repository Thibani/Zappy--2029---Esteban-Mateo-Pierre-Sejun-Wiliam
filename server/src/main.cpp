/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** main.cpp - Entry point
*/
#include "utils/args.hpp"
#include "server/server.hpp"
#include "exceptions/serverException.hpp"

#include <iostream>

// TODO: remove this stub once Sejun implements Game
namespace Zappy { class Game {}; }

int main(int argc, char **argv)
{
    Zappy::Args args;

    try {
        args = Zappy::ArgsParser::parse(argc, argv);
    } catch (const Zappy::ArgsException &e) {
        std::cerr << e.what() << "\n\n";
        Zappy::ArgsParser::printUsage(argv[0]);
        return 84;
    }

    try {
        Zappy::Game   game;
        Zappy::Server server(args, game);
        server.run();
    } catch (const Zappy::ServerException &e) {
        std::cerr << e.what() << "\n";
        return 84;
    } catch (const std::exception &e) {
        std::cerr << "Unexpected error: " << e.what() << "\n";
        return 84;
    }

    return 0;
}
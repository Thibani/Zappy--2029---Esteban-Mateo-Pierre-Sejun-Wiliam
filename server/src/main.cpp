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

static volatile bool g_running = true;

static void signalHandler(int sig)
{
    (void)sig;
    g_running = false;
}

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

    std::signal(SIGINT,  signalHandler);
    std::signal(SIGTERM, signalHandler);

    try {
        Zappy::Game        game(args.height, args.width);
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

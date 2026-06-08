/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** args.hpp
*/

#pragma once

#include <string>
#include <vector>
#include "exceptions/serverException.hpp"

namespace Zappy {

    struct Args {
        int                      port       = -1;
        int                      width      = -1;
        int                      height     = -1;
        int                      clientsNb  = -1;
        int                      freq       = 100;
        std::vector<std::string> teams;
    };

    class ArgsParser {
    public:
        // Parses argc/argv and returns a filled Args struct.
        // Throws std::invalid_argument with a usage message on any error.
        static Args parse(int argc, char **argv);

        static void printUsage(const char *binary);

    private:
        static bool isValidPort(int port);
        static bool isPositive(int value);
    };

} // namespace Zappy

/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** args.cpp
*/

#include "utils/args.hpp"
#include <iostream>
#include "exceptions/serverException.hpp"
#include <string>

namespace Zappy {

    void ArgsParser::printUsage(const char *binary)
    {
        std::cerr << "USAGE: " << binary
                  << " -p port -x width -y height -n name1 name2 ... -c clientsNb -f freq" << std::endl
                  << std::endl
                  << "  option   description" << std::endl
                  << "  -p       port number" << std::endl
                  << "  -x       width of the world" << std::endl
                  << "  -y       height of the world" << std::endl
                  << "  -n       name of the team(s) (one or more)" << std::endl
                  << "  -c       number of authorized clients per team" << std::endl
                  << "  -f       reciprocal of time unit (default: 100)" << std::endl;
    }

    bool ArgsParser::isValidPort(int port)
    {
        return port > 0 && port <= 65535;
    }

    bool ArgsParser::isPositive(int value)
    {
        return value > 0;
    }

    Args ArgsParser::parse(int argc, char **argv)
    {
        if (argc < 2) {
            printUsage(argv[0]);
            throw ArgsException("No arguments provided.");
        }

        Args args;

        for (int i = 1; i < argc; ++i) {
            std::string flag(argv[i]);

            // -p port
            if (flag == "-p") {
                if (++i >= argc)
                    throw ArgsException("Missing value for -p");
                args.port = std::stoi(argv[i]);
                if (!isValidPort(args.port))
                    throw ArgsException("Invalid port: must be 1-65535");

            // -x width
            } else if (flag == "-x") {
                if (++i >= argc)
                    throw ArgsException("Missing value for -x");
                args.width = std::stoi(argv[i]);
                if (!isPositive(args.width))
                    throw ArgsException("Width must be > 0");

            // -y height
            } else if (flag == "-y") {
                if (++i >= argc)
                    throw ArgsException("Missing value for -y");
                args.height = std::stoi(argv[i]);
                if (!isPositive(args.height))
                    throw ArgsException("Height must be > 0");

            // -c clientsNb
            } else if (flag == "-c") {
                if (++i >= argc)
                    throw ArgsException("Missing value for -c");
                args.clientsNb = std::stoi(argv[i]);
                if (!isPositive(args.clientsNb))
                    throw ArgsException("ClientsNb must be > 0");

            // -f freq
            } else if (flag == "-f") {
                if (++i >= argc)
                    throw ArgsException("Missing value for -f");
                args.freq = std::stoi(argv[i]);
                if (!isPositive(args.freq))
                    throw ArgsException("Freq must be > 0");

            // -n name1 name2 ... (consumes until next flag or end)
            } else if (flag == "-n") {
                if (i + 1 >= argc || argv[i + 1][0] == '-')
                    throw ArgsException("At least one team name required after -n");
                while (i + 1 < argc && argv[i + 1][0] != '-') {
                    ++i;
                    std::string name(argv[i]);
                    if (name == "GRAPHIC")
                        throw ArgsException("Team name 'GRAPHIC' is reserved");
                    args.teams.push_back(name);
                }

            } else {
                throw ArgsException("Unknown flag: " + flag);
            }
        }

        // Final validation — all mandatory fields must be set
        if (args.port == -1)      throw ArgsException("Missing -p (port)");
        if (args.width == -1)     throw ArgsException("Missing -x (width)");
        if (args.height == -1)    throw ArgsException("Missing -y (height)");
        if (args.clientsNb == -1) throw ArgsException("Missing -c (clientsNb)");
        if (args.teams.empty())   throw ArgsException("Missing -n (team names)");

        return args;
    }

} // namespace Server
/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** serverException.hpp
*/

#pragma once

#include <stdexcept>
#include <string>

namespace Zappy {

    // Base exception for everything in the server
    class ServerException : public std::runtime_error {
    public:
        explicit ServerException(const std::string &msg)
            : std::runtime_error(msg) {}
    };

    // Bad CLI arguments
    class ArgsException : public ServerException {
    public:
        explicit ArgsException(const std::string &msg)
            : ServerException("Args error: " + msg) {}
    };

    // Socket / network failures
    class NetworkException : public ServerException {
    public:
        explicit NetworkException(const std::string &msg)
            : ServerException("Network error: " + msg) {}
    };

    // poll() failures
    class PollException : public ServerException {
    public:
        explicit PollException(const std::string &msg)
            : ServerException("Poll error: " + msg) {}
    };

    // Game logic violations (Sejun will use this)
    class GameException : public ServerException {
    public:
        explicit GameException(const std::string &msg)
            : ServerException("Game error: " + msg) {}
    };

} // namespace Zappy
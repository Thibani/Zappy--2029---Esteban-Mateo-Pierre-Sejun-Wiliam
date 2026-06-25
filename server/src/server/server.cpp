/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Server.cpp
*/
#include "server/server.hpp"
#include "exceptions/serverException.hpp"
#include "utils/clock.hpp"
#include "game/game.hpp"

#include <iostream>
#include <cerrno>
#include <cstring>

#include <algorithm>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace Zappy {

    // -------------------------------------------------------------------------
    // Ctor / Dtor
    // -------------------------------------------------------------------------

    Server::Server(const Args &args, Game &game)
        : _args(args), _serverFd(-1), _guiProtocol(*this), _cmdHandler(game, _guiProtocol)
    {
        game.setListener(&_guiProtocol);
        _initSocket();
    }

    Server::~Server()
    {
        for (auto &[fd, _] : _clients)
            close(fd);
        if (_serverFd != -1)
            close(_serverFd);
    }

    // -------------------------------------------------------------------------
    // Public
    // -------------------------------------------------------------------------

    void Server::run()
    {
        std::cout << "[Server] Listening on port " << _args.port << "\n";

        while (true) {
            // Compute timeout = time until the nearest pending action deadline.
            // If no deadlines are pending, block indefinitely (-1).
            // This replaces the naive -1 timeout and makes the game tick fire on time.
            int timeout = _nearestDeadlineMs();

            int ready = poll(_pollfds.data(), _pollfds.size(), timeout);

            if (ready < 0) {
                if (errno == EINTR)  // interrupted by signal, just retry
                    continue;
                throw PollException(std::strerror(errno));
            }

            // Timeout expired — process any actions whose deadline has passed
            if (ready == 0) {
                _processPendingActions();
                continue;
            }

            for (std::size_t i = 0; i < _pollfds.size(); ++i) {
                auto &pfd = _pollfds[i];

                if (pfd.revents == 0)
                    continue;

                // Error or hang-up on a client fd
                if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
                    if (pfd.fd == _serverFd)
                        throw NetworkException("Error on server socket");
                    _disconnectClient(pfd.fd, "POLLERR/POLLHUP");
                    // _pollfds was modified — restart iteration
                    i = 0;
                    continue;
                }

                // Ctrl+D on stdin — clean shutdown
                if (pfd.fd == STDIN_FILENO && (pfd.revents & POLLIN)) {
                    char buf[16];
                    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
                    if (n == 0) {
                        std::cout << "[Server] EOF on stdin — shutting down.\n";
                        return;
                    }
                    continue;
                }

                // New incoming connection
                if (pfd.fd == _serverFd && (pfd.revents & POLLIN)) {
                    _acceptClient();
                    continue;
                }

                // Readable client
                if (pfd.revents & POLLIN)
                    _handleRead(pfd.fd);

                // Writable client (only enabled when write buffer is non-empty)
                if (pfd.revents & POLLOUT)
                    _handleWrite(pfd.fd);
            }

            // Also check deadlines after I/O — a command may have just been queued
            _processPendingActions();

            // Flush any write buffers that were filled during this iteration
            // without waiting for the next poll() cycle
            for (auto &[fd, client] : _clients) {
                if (!client.getWriteBuffer().empty())
                    _handleWrite(fd);
            }
        }
    }

    // -------------------------------------------------------------------------
    // Private — init
    // -------------------------------------------------------------------------

    void Server::_initSocket()
    {
        _serverFd = socket(AF_INET, SOCK_STREAM, 0);
        if (_serverFd < 0)
            throw NetworkException("socket(): " + std::string(std::strerror(errno)));

        // Allow immediate reuse of the port after restart
        int opt = 1;
        if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
            throw NetworkException("setsockopt(): " + std::string(std::strerror(errno)));

        // Non-blocking
        if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) < 0)
            throw NetworkException("fcntl(): " + std::string(std::strerror(errno)));

        struct sockaddr_in addr{};
        addr.sin_family      = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port        = htons(static_cast<uint16_t>(_args.port));

        if (bind(_serverFd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0)
            throw NetworkException("bind(): " + std::string(std::strerror(errno)));

        if (listen(_serverFd, SOMAXCONN) < 0)
            throw NetworkException("listen(): " + std::string(std::strerror(errno)));

        // Register the server fd in the poll array (POLLIN only, never POLLOUT)
        _addPollFd(_serverFd, POLLIN);

        // Register stdin so Ctrl+D triggers a clean shutdown
        _addPollFd(STDIN_FILENO, POLLIN);
    }

    // -------------------------------------------------------------------------
    // Private — poll array helpers
    // -------------------------------------------------------------------------

    void Server::_addPollFd(int fd, short events)
    {
        _pollfds.push_back({fd, events, 0});
    }

    void Server::_removePollFd(int fd)
    {
        auto it = std::find_if(_pollfds.begin(), _pollfds.end(),
            [fd](const struct pollfd &pfd) { return pfd.fd == fd; });
        if (it != _pollfds.end())
            _pollfds.erase(it);
    }

    void Server::_setPollOut(int fd, bool enable)
    {
        auto it = std::find_if(_pollfds.begin(), _pollfds.end(),
            [fd](const struct pollfd &pfd) { return pfd.fd == fd; });
        if (it != _pollfds.end()) {
            if (enable)
                it->events |= POLLOUT;
            else
                it->events &= ~POLLOUT;
        }
    }

    // -------------------------------------------------------------------------
    // Private — client lifecycle
    // -------------------------------------------------------------------------

    void Server::_acceptClient()
    {
        struct sockaddr_in addr{};
        socklen_t len = sizeof(addr);

        int clientFd = accept(_serverFd,
            reinterpret_cast<struct sockaddr *>(&addr), &len);

        if (clientFd < 0) {
            // EAGAIN/EWOULDBLOCK is normal on non-blocking socket
            if (errno != EAGAIN && errno != EWOULDBLOCK)
                std::cerr << "[Server] accept() failed: " << std::strerror(errno) << "\n";
            return;
        }

        // Set client fd non-blocking too
        if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0) {
            std::cerr << "[Server] fcntl() on client failed: " << std::strerror(errno) << "\n";
            close(clientFd);
            return;
        }

        _clients.emplace(clientFd, Client(clientFd));
        _addPollFd(clientFd, POLLIN);

        std::cout << "[Server] New connection fd=" << clientFd
                  << " from " << inet_ntoa(addr.sin_addr) << "\n";

        // Send WELCOME as per protocol
        _clients.at(clientFd).pushToWriteBuffer("WELCOME\n");
        _setPollOut(clientFd, true);
    }

    void Server::_disconnectClient(int fd, const std::string &reason)
    {
        std::cout << "[Server] Client fd=" << fd << " disconnected";
        if (!reason.empty())
            std::cout << " (" << reason << ")";
        std::cout << "\n";

        _clients.erase(fd);
        _removePollFd(fd);
        close(fd);
    }

    // -------------------------------------------------------------------------
    // Private — I/O
    // -------------------------------------------------------------------------

    void Server::_handleRead(int fd)
    {
        auto it = _clients.find(fd);
        if (it == _clients.end())
            return;

        Client &client = it->second;
        char    buf[4096];

        ssize_t n = recv(fd, buf, sizeof(buf) - 1, 0);

        if (n <= 0) {
            // 0 = clean disconnect, <0 = error
            _disconnectClient(fd, n == 0 ? "closed by peer" : std::strerror(errno));
            return;
        }

        buf[n] = '\0';
        client.feedReadBuffer(std::string(buf, n));

        // Extract and dispatch complete commands
        std::string cmd;
        while (client.popCommand(cmd))
            _cmdHandler.dispatch(client, cmd);

        // If CommandHandler pushed responses, enable POLLOUT
        if (!client.getWriteBuffer().empty())
            _setPollOut(fd, true);
    }

    void Server::_handleWrite(int fd)
    {
        auto it = _clients.find(fd);
        if (it == _clients.end())
            return;

        Client &client = it->second;
        const std::string &wbuf = client.getWriteBuffer();

        if (wbuf.empty()) {
            _setPollOut(fd, false);
            return;
        }

        ssize_t n = send(fd, wbuf.data(), wbuf.size(), MSG_NOSIGNAL);

        if (n < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK)
                _disconnectClient(fd, std::strerror(errno));
            return;
        }

        client.consumeWriteBuffer(static_cast<std::size_t>(n));

        // Disable POLLOUT if write buffer is now empty
        if (client.getWriteBuffer().empty())
            _setPollOut(fd, false);
    }

} // namespace Zappy

// These are stubs — will be fully implemented once CommandHandler and Game are in place

namespace Zappy {

    int Server::_nearestDeadlineMs()
    {
        // TODO: iterate over clients, find the nearest pending action deadline,
        // return Clock::msUntil(nearest) cast to int.
        // For now return -1 (block forever) until Game/CommandHandler exist.
        return -1;
    }

    void Server::_processPendingActions()
    {
        // TODO: iterate over clients, check if their current action deadline
        // has passed via Clock::hasPassed(), and if so dispatch the result
        // to CommandHandler / Game.
    }

}

namespace Zappy {

    void Server::broadcastToGuis(const std::string &line)
    {
        for (auto &[fd, client] : _clients) {
            if (client.getType() == ClientType::GUI) {
                client.pushToWriteBuffer(line);
                _setPollOut(fd, true);
            }
        }
    }

    void Server::sendToClient(int fd, const std::string &line)
    {
        auto it = _clients.find(fd);
        if (it == _clients.end()) {
            std::cerr << "[Server] sendToClient: fd=" << fd << " not found\n";
            return;
        }
        it->second.pushToWriteBuffer(line);
        _setPollOut(fd, true);
    }

}
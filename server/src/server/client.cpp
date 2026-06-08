/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Client.cpp
*/
#include "server/client.hpp"

#include <iostream>

namespace Zappy {

    // -------------------------------------------------------------------------
    // Ctor
    // -------------------------------------------------------------------------

    Client::Client(int fd)
        : _fd(fd),
          _type(ClientType::UNKNOWN),
          _authenticated(false)
    {}

    // -------------------------------------------------------------------------
    // Read buffer
    // -------------------------------------------------------------------------

    void Client::feedReadBuffer(const std::string &data)
    {
        _readBuffer += data;

        // Extract all complete newline-terminated commands
        std::size_t pos = 0;
        std::size_t nl  = 0;

        while ((nl = _readBuffer.find('\n', pos)) != std::string::npos) {
            std::string cmd = _readBuffer.substr(pos, nl - pos);

            // Strip trailing \r if the client sends \r\n
            if (!cmd.empty() && cmd.back() == '\r')
                cmd.pop_back();

            if (!cmd.empty()) {
                if (_cmdQueue.size() >= MAX_CMD_QUEUE) {
                    std::cerr << "[Client fd=" << _fd << "] Command queue full"
                              << " — dropping: \"" << cmd << "\"" << std::endl;
                } else {
                    _cmdQueue.push(cmd);
                }
            }
            pos = nl + 1;
        }

        // Keep only the incomplete tail in the buffer
        _readBuffer = _readBuffer.substr(pos);
    }

    bool Client::popCommand(std::string &out)
    {
        if (_cmdQueue.empty())
            return false;
        out = _cmdQueue.front();
        _cmdQueue.pop();
        return true;
    }

    // -------------------------------------------------------------------------
    // Write buffer
    // -------------------------------------------------------------------------

    void Client::pushToWriteBuffer(const std::string &data)
    {
        _writeBuffer += data;
    }

    const std::string &Client::getWriteBuffer() const
    {
        return _writeBuffer;
    }

    void Client::consumeWriteBuffer(std::size_t n)
    {
        if (n >= _writeBuffer.size())
            _writeBuffer.clear();
        else
            _writeBuffer.erase(0, n);
    }

    // -------------------------------------------------------------------------
    // Accessors
    // -------------------------------------------------------------------------

    int Client::getFd() const
    {
        return _fd;
    }

    ClientType Client::getType() const
    {
        return _type;
    }

    bool Client::isAuthenticated() const
    {
        return _authenticated;
    }

    std::size_t Client::commandQueueSize() const
    {
        return _cmdQueue.size();
    }

    // -------------------------------------------------------------------------
    // Setters
    // -------------------------------------------------------------------------

    void Client::setType(ClientType type)
    {
        _type = type;
    }

    void Client::setAuthenticated(bool auth)
    {
        _authenticated = auth;
    }

} // namespace Zappy
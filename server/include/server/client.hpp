/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Client.hpp
*/
#pragma once

#include <string>
#include <queue>

namespace Zappy {

    // Client type — determined after authentication
    enum class ClientType {
        UNKNOWN,
        AI,
        GUI
    };

    class Client {
    public:
        explicit Client(int fd);
        ~Client() = default;

        // Non-copyable, movable (needed for emplace in unordered_map)
        Client(const Client &)            = delete;
        Client &operator=(const Client &) = delete;
        Client(Client &&)                 = default;
        Client &operator=(Client &&)      = default;

        // -- Read buffer --
        // Feed raw bytes from recv() into the client
        void feedReadBuffer(const std::string &data);
        // Pop one complete command (newline-delimited) into out.
        // Returns false if no complete command is available yet.
        bool popCommand(std::string &out);

        // -- Write buffer --
        void              pushToWriteBuffer(const std::string &data);
        const std::string &getWriteBuffer() const;
        void              consumeWriteBuffer(std::size_t n);

        // -- Accessors --
        int        getFd()             const;
        ClientType getType()           const;
        bool       isAuthenticated()   const;
        std::size_t commandQueueSize() const;

        // -- Setters --
        void setType(ClientType type);
        void setAuthenticated(bool auth);

        static constexpr std::size_t MAX_CMD_QUEUE = 10;

    private:
        int         _fd;
        ClientType  _type;
        bool        _authenticated;

        std::string              _readBuffer;
        std::string              _writeBuffer;
        std::queue<std::string>  _cmdQueue;
    };

} // namespace Zappy
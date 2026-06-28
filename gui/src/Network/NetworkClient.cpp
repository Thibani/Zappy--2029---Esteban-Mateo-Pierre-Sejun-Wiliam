#include "NetworkClient.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cerrno>
#include <cstring>

NetworkClient::~NetworkClient()
{
    _disconnect();
}

bool NetworkClient::connect(const std::string& host, int port)
{
    struct addrinfo hints{};
    struct addrinfo* res = nullptr;

    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    std::string portStr = std::to_string(port);
    int err = getaddrinfo(host.c_str(), portStr.c_str(), &hints, &res);
    if (err != 0) {
        std::cerr << "[NetworkClient] getaddrinfo() failed: " << gai_strerror(err) << "\n";
        return false;
    }

    _fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (_fd < 0) {
        std::cerr << "[NetworkClient] socket() failed: " << std::strerror(errno) << "\n";
        freeaddrinfo(res);
        return false;
    }

    if (::connect(_fd, res->ai_addr, res->ai_addrlen) < 0) {
        std::cerr << "[NetworkClient] connect() failed: " << std::strerror(errno) << "\n";
        freeaddrinfo(res);
        _disconnect();
        return false;
    }

    freeaddrinfo(res);

    if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0) {
        std::cerr << "[NetworkClient] fcntl() failed: " << std::strerror(errno) << "\n";
        _disconnect();
        return false;
    }

    std::cout << "[NetworkClient] Connected to " << host << ":" << port << "\n";
    return true;
}

void NetworkClient::send(const std::string& line)
{
    if (_fd < 0)
        return;
    ::send(_fd, line.c_str(), line.size(), MSG_NOSIGNAL);
}

bool NetworkClient::pollLine(std::string& out)
{
    char buf[4096];
    ssize_t n = recv(_fd, buf, sizeof(buf) - 1, 0);

    if (n > 0) {
        _readBuffer.append(buf, n);
    } else if (n == 0) {
        std::cerr << "[NetworkClient] Server closed connection\n";
        _disconnect();
        return false;
    } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
        std::cerr << "[NetworkClient] recv() error: " << std::strerror(errno) << "\n";
        _disconnect();
        return false;
    }

    std::size_t nl = _readBuffer.find('\n');
    if (nl == std::string::npos)
        return false;

    out = _readBuffer.substr(0, nl);
    if (!out.empty() && out.back() == '\r')
        out.pop_back();

    _readBuffer.erase(0, nl + 1);
    return true;
}

void NetworkClient::_disconnect()
{
    if (_fd != -1) {
        close(_fd);
        _fd = -1;
    }
}
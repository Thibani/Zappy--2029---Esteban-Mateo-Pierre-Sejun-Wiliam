#include "NetworkClient.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd < 0) {
        std::cerr << "[NetworkClient] socket() failed: " << std::strerror(errno) << "\n";
        return false;
    }

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(static_cast<uint16_t>(port));

    if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0) {
        std::cerr << "[NetworkClient] Invalid address: " << host << "\n";
        _disconnect();
        return false;
    }

    if (::connect(_fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "[NetworkClient] connect() failed: " << std::strerror(errno) << "\n";
        _disconnect();
        return false;
    }

    // Set non-blocking AFTER connecting
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
    // Try to read available data from socket
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

    // Extract one complete line from buffer
    std::size_t nl = _readBuffer.find('\n');
    if (nl == std::string::npos)
        return false;

    out = _readBuffer.substr(0, nl);
    // Strip trailing \r if present
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
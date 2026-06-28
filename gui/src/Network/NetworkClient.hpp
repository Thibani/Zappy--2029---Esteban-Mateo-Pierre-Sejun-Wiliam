#pragma once
#include <string>

class NetworkClient {
public:
    NetworkClient() = default;
    ~NetworkClient();

    bool connect(const std::string& host, int port);
    void send(const std::string& line);
    bool pollLine(std::string& out);
    bool isConnected() const { return _fd != -1; }

    NetworkClient(const NetworkClient&) = delete;
    NetworkClient& operator=(const NetworkClient&) = delete;

private:
    int         _fd = -1;
    std::string _readBuffer;

    void _disconnect();
};
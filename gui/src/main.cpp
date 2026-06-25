#include <raylib.h>
#include <iostream>

void gameSetup(const char* host, int port);

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cerr << "USAGE: " << argv[0] << " <host> <port>\n";
        return 84;
    }
    gameSetup(argv[1], std::atoi(argv[2]));
    return 0;
}
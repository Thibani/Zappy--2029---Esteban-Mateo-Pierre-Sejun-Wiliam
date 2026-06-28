#include <string>

struct Character {
    int         id;
    std::string name;
    std::string team;
    int         tileX;
    int         tileY;
    int         level = 1;
    int         inv[7] = {0};
};
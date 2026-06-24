#pragma once
#include <string>

struct Character {
    std::string name;
    int         tileX;
    int         tileY;
    int         level = 1;
};
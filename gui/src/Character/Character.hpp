#pragma once
#include <string>

struct Character {
    int         id;
    std::string name;
    int         tileX;
    int         tileY;
    int         level = 1;
};
#include <string>

// Character.hpp
struct Character {
    int         id;
    std::string name;
    int         tileX;
    int         tileY;
    int         level = 1;
    int         inv[7] = {0}; // q0..q6
};
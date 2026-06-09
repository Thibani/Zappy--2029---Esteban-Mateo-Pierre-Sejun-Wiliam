#include "Map.hpp"

Map::Map(int height, int width) : _case(height, std::vector<Case>(width))
{
    _height = height;
    _width = width;
}

std::vector<std::vector<Case>> Map::getCase()
{
    return _case;
}

// void Map::setRessource()
// {
//     while (_ressour)
// }
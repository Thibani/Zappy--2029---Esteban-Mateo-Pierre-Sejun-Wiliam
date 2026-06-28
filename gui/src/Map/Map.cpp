#include "Map.hpp"

Map::Map(int width, int height)
{
    resize(width, height);
}

void Map::resize(int width, int height)
{
    _width  = width;
    _height = height;
    _tiles.assign(width * height, Tile{});
}

Tile& Map::getTile(int x, int y) {
    return _tiles[y * _width + x];
}

const Tile& Map::getTile(int x, int y) const
{
    return _tiles[y * _width + x];
}
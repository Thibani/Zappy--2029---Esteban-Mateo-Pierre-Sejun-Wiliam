#pragma once
#include "../Tile/Tile.hpp"
#include <vector>

class Map {
public:
    Map() = default;
    Map(int width, int height);

    void        resize(int width, int height);
    Tile&       getTile(int x, int y);
    const Tile& getTile(int x, int y) const;

    int getWidth()  const { return _width; }
    int getHeight() const { return _height; }

private:
    int               _width  = 0;
    int               _height = 0;
    std::vector<Tile> _tiles;
};
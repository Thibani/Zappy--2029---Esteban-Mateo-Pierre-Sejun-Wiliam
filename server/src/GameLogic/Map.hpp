#pragma once
#include <cstdio>
#include <vector>
#include "Position.hpp"
#include "Resource.hpp"


namespace Zappy {

    class Tile;
    class Player;

    class Map {

        public:
            Map(int width, int height);
            ~Map() = default;
            Tile* getTile(Pos position);
            void setRessource();
            int getHeight() { return _height; }
            int getWidth() { return _width; }
            void correctPos(Pos *position);
            void addPlayerOnTile(Player *player);
            void debugDisplayMap();

        private:
            std::vector<std::vector<Tile*>> _tiles;
            int _height;
            int _width;
            float _densities[7] = {0.5, 0.3, 0.15, 0.1, 0.1, 0.08, 0.05};
    };
}
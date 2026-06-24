#include "map/map.hpp"
#include "types/resource.hpp"
#include "map/tile.hpp"
#include "player/player.hpp"

#include <cstdio>
#include <random>

namespace Zappy {

    Map::Map(int height, int width) : _tiles(height, std::vector<Tile*>(width))
    {
        Tile* tile;

        _height = height;
        _width = width;
        for (int y = 0; y < height; y++){
            for (int x = 0; x < width; x++){
                tile = new Tile();
                tile->position.x = x;
                tile->position.y = y;
                _tiles[y][x] = tile;
            }
        }
    }

    Tile* Map::getTile(Pos position)
    {
        return _tiles[position.y][position.x];
    }

    void Map::setRessource()
    {
        int quantity;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, _width - 1);
        std::uniform_int_distribution<int> larg(0, _height - 1);

        for (int i = 0; i < 7; i++){
            quantity = _height * _width * _densities[i];
            while (quantity > 0){
                int x = dist(gen);
                int y = larg(gen);
                _tiles[y][x]->addResource((TypeResource)i);
                quantity--;
            }
        }
    }

    void Map::debugDisplayMap()
    {
        Pos p;
        for (int y = 0; y < _height; y++)
        {
            for (int x = 0; x < _width; x++)
            {
                p.x = x;
                p.y = y;
                std::printf("[ %d ]", getTile(p)->resources()[FOOD]);
            }
            printf("\n");
        }
    }

    void Map::correctPos(Pos *position)
    {
        if (position->x < 0)
            position->x += _width;
        if (position->x >= _width)
            position->x -= _width;
        if (position->y < 0)
            position->y += _height;
        if (position->y >= _height)
            position->y -= _height;
    }

    void Map::addPlayerOnTile(Player *player)
    {
        Tile* tile = getTile(player->getPosition());
        tile->addPlayer(player);
    }
}

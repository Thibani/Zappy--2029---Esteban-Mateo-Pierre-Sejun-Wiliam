#include "../../include/map/map.hpp"
#include "../../include/types/resource.hpp"
#include "../../include/map/tile.hpp"
#include "../../include/player/player.hpp"
#include "../../include/egg/egg.hpp"
#include <cstdio>
#include <random>

namespace Zappy {

    Map::Map(int width, int height) : _tiles(height, std::vector<Tile*>(width))
    {
        Tile* tile;

        _height = height;
        _width = width;
        for (int i = 0; i < height; i++){
            for (int j = 0; j < width; j++){
                tile = new Tile();
                _tiles[i][j] = tile;
            }
        }
    }

    Map::~Map()
    {
        for (const auto &row : _tiles)
            for (Tile *t : row)
                delete t;
    }

    Tile* Map::getTile(Pos position)
    {
        return _tiles[position.y][position.x];
    }

    void Map::setRessource()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, _width - 1);
        std::uniform_int_distribution<int> larg(0, _height - 1);

        std::vector<int> resources = countResources();
        for (int i = 0; i < 7; i++){
            int quantity = _height * _width * _densities[i] - resources[i];
            while (quantity > 0){
                int x = dist(gen);
                int y = larg(gen);
                _tiles[y][x]->addResource((TypeResource)i);
                quantity--;
            }
        }
    }

    std::vector<int> Map::countResources()
    {
        std::vector<int> resources(7, 0);
        for (int i = 0; i < _height; i++){
            for (int j = 0; j < _width; j++){
                resources[FOOD] += _tiles[i][j]->getNbResources(FOOD);
                resources[LINEMATE] += _tiles[i][j]->getNbResources(LINEMATE);
                resources[DERAUMERE] += _tiles[i][j]->getNbResources(DERAUMERE);
                resources[SIBUR] += _tiles[i][j]->getNbResources(SIBUR);
                resources[MENDIANE] += _tiles[i][j]->getNbResources(MENDIANE);
                resources[PHIRAS] += _tiles[i][j]->getNbResources(PHIRAS);
                resources[THYSTAME] += _tiles[i][j]->getNbResources(THYSTAME);
            }
        }
        return resources;
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
                (void)getTile(p);
                //std::printf("[ %d ]", tile->_resources[FOOD]);
                //std::printf("[ %d ]", getTile(p)->_eggs.size());
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

    Player* Map::eggHatching(Egg* egg, const std::string &teamName)
    {
        Tile* tile = getTile(egg->getPosition());
        tile->removeEgg(egg);
        Player *player = new Player(teamName);
        player->setPosition(egg->getPosition());
        addPlayerOnTile(player);
        delete egg;
        return player;
    }
}

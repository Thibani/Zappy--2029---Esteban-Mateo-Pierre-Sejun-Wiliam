#include "Map.hpp"
#include <random>

Map::Map(int height, int width) : _case(height, std::vector<Case>(width))
{
    _height = height;
    _width = width;
}

std::vector<std::vector<Case>> Map::getCase()
{
    return _case;
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
            _case[y][x].ressource[i]++;
            quantity--;
        }
    }
}
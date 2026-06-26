#include "EggFactory.hpp"
#include <algorithm>
#include <iostream>

void EggFactory::addEgg(int eggId, int parentId, int tileX, int tileY)
{
    _eggs.push_back({eggId, parentId, tileX, tileY});
    std::cout << "[EggFactory] Egg #" << eggId << " laid at " << tileX << "," << tileY << "\n";
}

void EggFactory::removeById(int eggId)
{
    _eggs.erase(
        std::remove_if(_eggs.begin(), _eggs.end(),
            [eggId](const Egg& e) { return e.eggId == eggId; }),
        _eggs.end());
    std::cout << "[EggFactory] Egg #" << eggId << " removed\n";
}
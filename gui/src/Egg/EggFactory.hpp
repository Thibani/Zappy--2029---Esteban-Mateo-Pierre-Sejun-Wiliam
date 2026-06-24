#pragma once
#include "Egg.hpp"
#include <vector>

class EggFactory {
public:
    void               addEgg(int eggId, int parentId, int tileX, int tileY);
    void               removeById(int eggId);
    const std::vector<Egg>& getAll() const { return _eggs; }
private:
    std::vector<Egg> _eggs;
};
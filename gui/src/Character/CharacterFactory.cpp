#include "CharacterFactory.hpp"
#include <algorithm>

Character& CharacterFactory::createCharacter(int id, const std::string& name, int tileX, int tileY, int level)
{
    _characters.push_back({id, name, tileX, tileY, level});
    return _characters.back();
}

Character* CharacterFactory::findById(int id)
{
    auto it = std::find_if(_characters.begin(), _characters.end(),
        [id](const Character& c) { return c.id == id; });
    return it == _characters.end() ? nullptr : &(*it);
}

void CharacterFactory::removeById(int id)
{
    _characters.erase(
        std::remove_if(_characters.begin(), _characters.end(),
            [id](const Character& c) { return c.id == id; }),
        _characters.end());
}
#include "CharacterFactory.hpp"

Character& CharacterFactory::createCharacter(const std::string& name, int tileX, int tileY, int level) {
    _characters.push_back({name, tileX, tileY, level});
    return _characters.back();
}
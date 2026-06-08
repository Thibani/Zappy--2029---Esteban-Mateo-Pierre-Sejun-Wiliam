#include "Character.hpp"

Character& Character::CharacterFactory::createCharacter(const std::string& name, int tileX, int tileY) {
    _characters.push_back({name, tileX, tileY});
    return _characters.back();
}
#pragma once
#include "Character.hpp"
#include <vector>

class CharacterFactory {
public:
    Character& createCharacter(const std::string& name, int tileX, int tileY, int level = 1);
    const std::vector<Character>& getAll() const { return _characters; }
private:
    std::vector<Character> _characters;
};
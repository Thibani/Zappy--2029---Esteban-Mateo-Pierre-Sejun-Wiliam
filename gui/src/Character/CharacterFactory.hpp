#pragma once
#include "Character.hpp"
#include <vector>

class CharacterFactory {
public:
    Character& createCharacter(int id, const std::string& name, const std::string& team, int tileX, int tileY, int level = 1);
    const std::vector<Character>& getAll() const { return _characters; }
    Character*  findById(int id);
    void        removeById(int id);
private:
    std::vector<Character> _characters;
};
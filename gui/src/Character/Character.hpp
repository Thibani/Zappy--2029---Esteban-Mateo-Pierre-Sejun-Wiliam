#pragma once

#include <string>
#include <raylib.h>
#include <vector>
#include "../Constants.hpp"

struct Character {
    std::string name;
    int tileX;
    int tileY;

    Vector3 getWorldPosition() const {
        return {
            tileX * TILE_SIZE + TILE_SIZE / 2.f,
            0.1f,
            tileY * TILE_SIZE + TILE_SIZE / 2.f
        };
    }

    class CharacterFactory {
    public:
        Character& createCharacter(const std::string& name, int tileX, int tileY);
        const std::vector<Character>& getAll() const { return _characters; }
    private:
        std::vector<Character> _characters;
    };
};